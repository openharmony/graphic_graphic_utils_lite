/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef GRAPHIC_LITE_FILTER_BLUR_H
#define GRAPHIC_LITE_FILTER_BLUR_H

#include "gfx_utils/diagram/common/common_basics.h"
#include "gfx_utils/graphic_math.h"
#include "graphic_config.h"
#include "securec.h"

namespace OHOS {
class Filterblur {
#if defined(GRAPHIC_ENABLE_BLUR_EFFECT_FLAG) && GRAPHIC_ENABLE_BLUR_EFFECT_FLAG

public:
    /* ARGB8888 pixel layout: [B=0, G=1, R=2, A=3] */
    static constexpr int32_t ARGB_ALPHA_BYTE_OFFSET = 3;
    static constexpr int32_t ARGB_BYTES_PER_PIXEL = 4;
    static constexpr int32_t HALF_DIVISOR = 2;

    Filterblur()
    {
        integral_ = nullptr;
        alphaIntegral_ = nullptr;
        imageWidth_ = 0;
        imageHeight_ = 0;
        alphaWidth_ = 0;
        alphaHeight_ = 0;
    }
    ~Filterblur()
    {
        if (integral_ != nullptr) {
            free(integral_);
        }
        if (alphaIntegral_ != nullptr) {
            free(alphaIntegral_);
        }
    }

    bool AllocateBuffer(int32_t width, int32_t height, int32_t channel)
    {
        if (integral_ != nullptr) {
            free(integral_);
            integral_ = nullptr;
        }
        int64_t totalBytes = (static_cast<int64_t>(width) + 1) * (static_cast<int64_t>(height) + 1) * channel *
                sizeof(int32_t);
        if ((totalBytes <= 0) || (totalBytes > SIZE_MAX)) {
            return false;
        }
        integral_ = (int32_t*)malloc(static_cast<size_t>(totalBytes));
        if (integral_ == nullptr) {
            return false;
        }
        return true;
    }

    template <class Img>
    void BoxBlur(Img& img, uint16_t radius, int32_t channel, int32_t stride)
    {
        if (radius < 1) {
            return;
        }
        int32_t width = img.GetWidth();
        int32_t height = img.GetHeight();
        bool isGetRGBAIntegral = false;
        if (integral_ == nullptr || ((imageWidth_ * imageHeight_) != (width * height))) {
            if (!AllocateBuffer(width, height, channel)) {
                return;
            }
            isGetRGBAIntegral = true;
        }
        if (channel == FOUR_TIMES) {
            if (isGetRGBAIntegral) {
                GetRGBAIntegralImage((uint8_t*)img.PixValuePtr(0, 0), width, height, stride);
            }
#ifndef __ICCARM__
#pragma omp parallel for
#endif
            for (int32_t y = 0; y < height; y++) {
                int32_t y1 = MATH_MAX(y - radius, 0);
                int32_t y2 = MATH_MIN(y + radius + 1, height);
                uint8_t* lineImageHeader = (uint8_t*)img.PixValuePtr(0, 0) + y * stride;
                uint8_t* linePD = lineImageHeader;
                int32_t* lineP1 = integral_ + y1 * ((width + 1) << 2);
                int32_t* lineP2 = integral_ + y2 * ((width + 1) << 2);

                for (int32_t x = 0; x < width; x++) {
                    int32_t x1 = MATH_MAX(x - radius, 0);
                    int32_t x2 = MATH_MIN(x + radius + 1, width);
                    int32_t index1 = x1 << 2;
                    int32_t index2 = x2 << 2;
                    int32_t sumB = lineP2[index2 + 0] - lineP1[index2 + 0] -
                               lineP2[index1 + 0] + lineP1[index1 + 0];
                    int32_t sumG = lineP2[index2 + 1] - lineP1[index2 + 1] -
                               lineP2[index1 + 1] + lineP1[index1 + 1];
                    int32_t sumR = lineP2[index2 + 2] - lineP1[index2 + 2] -
                               lineP2[index1 + 2] + lineP1[index1 + 2];

                    const int32_t pixelCount = (x2 - x1) * (y2 - y1);
                    linePD[0] = (sumB + (pixelCount >> 1)) / pixelCount;
                    linePD[1] = (sumG + (pixelCount >> 1)) / pixelCount;
                    linePD[2] = (sumR + (pixelCount >> 1)) / pixelCount;
                    uint8_t* alpha = lineImageHeader + (x << 2);
                    linePD[3] = alpha[3];
                    linePD += 4;
                }
            }
        }
    }

    /*
     * BoxBlurAlpha - Apply box blur to the alpha channel only, leaving RGB unchanged.
     *
     * Complements BoxBlur which skips the alpha channel. Uses a single-channel
     * integral image (O(N) complexity) with OpenMP parallelization.
     *
     * Algorithm:
     *   1. Build a single-channel integral image of alpha values.
     *      integral[y+1][x+1] = Σ alpha(0..y, 0..x).
     *      First row/column are zero-padded for boundary safety.
     *   2. For each pixel (y,x), compute the alpha sum in the surrounding
     *      (2*R)×(2*R) window via 4-corner lookup:
     *        sum = I[y2][x2] - I[y1][x2] - I[y2][x1] + I[y1][x1]
     *      where y1=max(0,y-R), y2=min(H,y+R+1), x1=max(0,x-R), x2=min(W,x+R+1).
     *   3. Replace alpha with the rounded average:
     *        newAlpha = (sum + pixelCount/2) / pixelCount
     *      The +pixelCount/2 implements round-half-up, preventing systematic
     *      darkening from integer truncation.
     */
    template <class Img>
    void BoxBlurAlpha(Img& img, uint16_t radius, int32_t stride)
    {
        if (radius < 1) {
            return;
        }
        int32_t width = img.GetWidth();
        int32_t height = img.GetHeight();
        /* Reuse integral_ buffer; reallocate if image dimensions changed.
           Note: alpha integral is single-channel, so buffer size differs
           from the 4-channel RGBA integral used by BoxBlur. */
        bool isRecompute = false;
        if (alphaIntegral_ == nullptr || (alphaWidth_ * alphaHeight_) != (width * height)) {
            int64_t totalBytes = (static_cast<int64_t>(width) + 1) *
                (static_cast<int64_t>(height) + 1) * sizeof(int32_t);
            if ((totalBytes <= 0) || (totalBytes > SIZE_MAX)) {
                return;
            }
            if (alphaIntegral_ != nullptr) {
                free(alphaIntegral_);
            }
            alphaIntegral_ = (int32_t*)malloc(static_cast<size_t>(totalBytes));
            if (alphaIntegral_ == nullptr) {
                return;
            }
            alphaWidth_ = width;
            alphaHeight_ = height;
            isRecompute = true;
        }
        if (isRecompute) {
            GetAlphaIntegralImage(reinterpret_cast<uint8_t*>(img.PixValuePtr(0, 0)), width, height, stride);
        }
#ifndef __ICCARM__
#pragma omp parallel for
#endif
        for (int32_t y = 0; y < height; y++) {
            int32_t y1 = MATH_MAX(y - radius, 0);
            int32_t y2 = MATH_MIN(y + radius + 1, height);
            uint8_t* lineData = reinterpret_cast<uint8_t*>(img.PixValuePtr(0, 0)) + y * stride;
            int32_t* lineP1 = alphaIntegral_ + y1 * (width + 1);
            int32_t* lineP2 = alphaIntegral_ + y2 * (width + 1);
 
            for (int32_t x = 0; x < width; x++) {
                int32_t x1 = MATH_MAX(x - radius, 0);
                int32_t x2 = MATH_MIN(x + radius + 1, width);
                int32_t pixelCount = (x2 - x1) * (y2 - y1);
                int32_t sum = lineP2[x2] - lineP1[x2] - lineP2[x1] + lineP1[x1];
                lineData[x * ARGB_BYTES_PER_PIXEL + ARGB_ALPHA_BYTE_OFFSET] =
                    static_cast<uint8_t>((sum + pixelCount / HALF_DIVISOR) / pixelCount);
            }
        }
    }

private:
    void GetRGBAIntegralImage(uint8_t* src, uint16_t width, uint16_t height, uint16_t stride)
    {
        int32_t channel = FOUR_TIMES;
        if (integral_ != nullptr && ((imageWidth_ * imageHeight_) != (width * height))) {
            int32_t integralSize = (width + 1) * channel * sizeof(int32_t);
            memset_s(integral_, integralSize, 0, integralSize);
        }
        for (int y = 0; y < height; y++) {
            uint8_t* linePS = src + y * stride;
            //	last position
            int32_t* linePL = integral_ + y * (width + 1) * channel + channel;
            //	curretn position£¬waring the first column of every line row is zero
            int32_t* linePD = integral_ + (y + 1) * (width + 1) * channel + channel;
            //	the first column is 0
            linePD[-4] = 0;
            linePD[-3] = 0;
            linePD[-2] = 0;
            linePD[-1] = 0;
            for (int x = 0, sumB = 0, sumG = 0, sumR = 0, sumA = 0; x < width; x++) {
                sumB += linePS[0];
                sumG += linePS[1];
                sumR += linePS[2];
                sumA += linePS[3];
                linePD[0] = linePL[0] + sumB;
                linePD[1] = linePL[1] + sumG;
                linePD[2] = linePL[2] + sumR;
                linePD[3] = linePL[3] + sumA;
                linePS += channel;
                linePL += channel;
                linePD += channel;
            }
        }
    }

    /*
     * Build a single-channel integral image for the alpha channel.
     * integral[y+1][x+1] = cumulative sum of alpha from (0,0) to (y,x).
     * First row/column are zero-padded to simplify boundary calculations.
     */
    void GetAlphaIntegralImage(uint8_t* src, int32_t width, int32_t height, int32_t stride)
    {
        int32_t integralSize = (width + 1) * sizeof(int32_t);
        if (alphaIntegral_ != nullptr) {
            memset_s(alphaIntegral_, integralSize, 0, integralSize);
        }
        for (int y = 0; y < height; y++) {
            uint8_t* linePS = src + y * stride;
            int32_t* linePL = alphaIntegral_ + y * (width + 1) + 1;
            int32_t* linePD = alphaIntegral_ + (y + 1) * (width + 1) + 1;
            linePD[-1] = 0;
            int32_t sum = 0;
            for (int x = 0; x < width; x++) {
                sum += linePS[x * ARGB_BYTES_PER_PIXEL + ARGB_ALPHA_BYTE_OFFSET];
                linePD[0] = linePL[0] + sum;
                linePL += 1;
                linePD += 1;
            }
        }
    }

    int32_t* integral_;
    int32_t imageWidth_;
    int32_t imageHeight_;
    int32_t* alphaIntegral_;
    int32_t alphaWidth_;
    int32_t alphaHeight_;
#endif
};
} // namespace OHOS
#endif
