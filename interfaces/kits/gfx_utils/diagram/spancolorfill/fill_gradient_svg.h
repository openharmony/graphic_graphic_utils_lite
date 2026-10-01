/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

/**
 * @file fill_gradient_svg.h
 * @brief Defines SVG-specific scanline gradient helpers
 * @since 3.0
 * @version 5.0
 */

#ifndef GRAPHIC_LITE_FILL_GRADIENT_SVG_H
#define GRAPHIC_LITE_FILL_GRADIENT_SVG_H

#include "gfx_utils/color.h"
#include "gfx_utils/diagram/common/common_basics.h"
#include "gfx_utils/diagram/spancolorfill/fill_base.h"
#include "gfx_utils/diagram/spancolorfill/fill_gradient_lut.h"
#include "gfx_utils/diagram/spancolorfill/fill_interpolator.h"
#include "gfx_utils/graphic_math.h"

namespace OHOS {
#if defined(FEATURE_COMPONENT_SVG) && FEATURE_COMPONENT_SVG
/* 4x4 Bayer matrix used for ordered dithering of 8-bit gradient output.
 * Normalised threshold is (value + 0.5) / 16, i.e. inside [0, 1). */
static constexpr int16_t GRADIENT_BAYER_4X4[4][4] = {
    {0,  8, 2,  10},
    {12, 4, 14, 6},
    {3,  11, 1, 9},
    {15, 7, 13, 5}
};

static inline uint8_t DitherColorChannel(float value, float threshold)
{
    /* Upper bound of an 8-bit color channel; MAX_COLOR_NUM is unsigned, so keep it
       int32_t here to compare against the clamped value without a sign mismatch. */
    constexpr int32_t COLOR_CHANNEL_MAX = static_cast<int32_t>(MAX_COLOR_NUM);
    int32_t v = static_cast<int32_t>(value + threshold);
    if (v < 0) {
        v = 0;
    } else if (v > COLOR_CHANNEL_MAX) {
        v = COLOR_CHANNEL_MAX;
    }
    return static_cast<uint8_t>(v);
}

static constexpr int32_t SVG_GRADIENT_SUBPIXEL_SCALE = FillInterpolator::SUBPIXEL_SCALE;

/**
 * SVG-only gradient index interface.
 *
 * Kept separate from the baseline Gradient interface so that enabling SVG does not
 * change the virtual signature or behavior used by non-SVG components.
 */
class GradientSvg {
public:
    /* Linear gradients pass the raw projected coordinate which can be much larger
       than int16_t after raising subpixel precision to 8 bit. */
    virtual int32_t Calculate(int32_t x, int32_t y, int32_t startRadius, int32_t endRadius, int32_t size) = 0;
    /**
     * @brief Floating-point gradient index before LUT quantization.
     * Used to interpolate between adjacent LUT entries and eliminate banding.
     */
    virtual float CalculateFloat(int32_t x, int32_t y, int32_t startRadius, int32_t endRadius, int32_t size)
    {
        return static_cast<float>(Calculate(x, y, startRadius, endRadius, size));
    }
};

/**
 * SVG-specific gradient scanline fill with LUT interpolation and Bayer dithering.
 */
class FillGradientSvg : public SpanBase {
public:
    FillGradientSvg() {}

    FillGradientSvg(FillInterpolator& inter, GradientSvg& GradientFunction,
                    FillGradientLut& ColorFunction, float distance1, float distance2)
        : interpolator_(&inter),
          gradientFunction_(&GradientFunction),
          colorFunction_(&ColorFunction),
          distance1_(static_cast<int32_t>(distance1 * SVG_GRADIENT_SUBPIXEL_SCALE)),
          distance2_(static_cast<int32_t>(distance2 * SVG_GRADIENT_SUBPIXEL_SCALE)) {}

    void Prepare() {}

    void Generate(Rgba8T* span, int32_t x, int32_t y, uint32_t len)
    {
        constexpr int32_t downscaleShift = 0;
        interpolator_->Begin(x, y, len);
        int32_t lutSize = static_cast<int32_t>(colorFunction_->GetSize());
        constexpr int32_t GRADIENT_LUT_INTERP_UPPER_OFFSET = 2;
        for (; len; --len, ++(*interpolator_), span++) {
            interpolator_->Coordinates(&x, &y);
            float indexF = gradientFunction_->CalculateFloat(x >> downscaleShift, y >> downscaleShift,
                                                             distance1_, distance2_, lutSize);
            int32_t index = static_cast<int32_t>(indexF);
            if (index < 0) {
                index = 0;
            } else if (index >= lutSize - 1) {
                index = lutSize - GRADIENT_LUT_INTERP_UPPER_OFFSET;
            }
            int32_t i0 = index;
            float fraction = indexF - static_cast<float>(i0);
            const Rgba8T& c0 = (*colorFunction_)[i0];
            const Rgba8T& c1 = (*colorFunction_)[i0 + 1];
            uint32_t bx = static_cast<uint32_t>(x >> FillInterpolator::SUBPIXEL_SHIFT) & 3;
            uint32_t by = static_cast<uint32_t>(y >> FillInterpolator::SUBPIXEL_SHIFT) & 3;
            float threshold = (static_cast<float>(GRADIENT_BAYER_4X4[by][bx]) + 0.5f) / 16.0f;
            float r = c0.red + (c1.red - c0.red) * fraction;
            float g = c0.green + (c1.green - c0.green) * fraction;
            float b = c0.blue + (c1.blue - c0.blue) * fraction;
            float a = c0.alpha + (c1.alpha - c0.alpha) * fraction;
            span->red = DitherColorChannel(r, threshold);
            span->green = DitherColorChannel(g, threshold);
            span->blue = DitherColorChannel(b, threshold);
            span->alpha = DitherColorChannel(a, threshold);
        }
    }

private:
    FillInterpolator* interpolator_;
    GradientSvg* gradientFunction_;
    FillGradientLut* colorFunction_;
    int32_t distance1_;
    int32_t distance2_;
};

/**
 * SVG radial gradient index calculator.
 */
class GradientRadialCalculateSvg : public virtual GradientSvg {
public:
    GradientRadialCalculateSvg()
        : endRadius_(HUNDRED_TIMES * SVG_GRADIENT_SUBPIXEL_SCALE), dx_(0), dy_(0)
    {
        UpdateValues();
    }

    GradientRadialCalculateSvg(float endRadius, float dx, float dy)
        : endRadius_(static_cast<int32_t>(endRadius * SVG_GRADIENT_SUBPIXEL_SCALE)),
          dx_(static_cast<int32_t>(dx * SVG_GRADIENT_SUBPIXEL_SCALE)),
          dy_(static_cast<int32_t>(dy * SVG_GRADIENT_SUBPIXEL_SCALE))
    {
        UpdateValues();
    }

    int32_t Calculate(int32_t x, int32_t y, int32_t startRadius, int32_t endRadius, int32_t size) override
    {
        float dx = static_cast<float>(x - dx_);
        float dy = static_cast<float>(y - dy_);
        float distanceRadius = dx * static_cast<float>(dy_) - dy * static_cast<float>(dx_);
        float radiusDistance = endRadiusSquare_ * (dx * dx + dy * dy)
                               - distanceRadius * distanceRadius;
        float deltaRadius = static_cast<float>(endRadius - startRadius); // Difference of radius
        if (deltaRadius < 1.0f) {
            deltaRadius = 1.0f;
        }
        int32_t index = static_cast<int32_t>((((dx * static_cast<float>(dx_) + dy * static_cast<float>(dy_) +
                        Sqrt(fabs(radiusDistance)))
                        * mul_ - static_cast<float>(startRadius)) * static_cast<float>(size)) / deltaRadius);
        if (index < 0) {
            index = 0;
        }
        if (index >= size) {
            index = size - 1;
        }
        return index;
    }

    /**
     * @brief Floating-point radial gradient index used for inter-LUT interpolation.
     */
    float CalculateFloat(int32_t x, int32_t y, int32_t startRadius, int32_t endRadius, int32_t size) override
    {
        float dx = static_cast<float>(x - dx_);
        float dy = static_cast<float>(y - dy_);
        float distanceRadius = dx * static_cast<float>(dy_) - dy * static_cast<float>(dx_);
        float radiusDistance = endRadiusSquare_ * (dx * dx + dy * dy)
                               - distanceRadius * distanceRadius;
        float deltaRadius = static_cast<float>(endRadius - startRadius);
        if (deltaRadius < 1.0f) {
            deltaRadius = 1.0f;
        }
        if (size <= 1) {
            return 0.0f;
        }
        float t = (((dx * static_cast<float>(dx_) + dy * static_cast<float>(dy_) +
                     Sqrt(fabs(radiusDistance))) * mul_ - static_cast<float>(startRadius)) *
                   static_cast<float>(size)) / deltaRadius;
        return t;
    }

private:
    void UpdateValues()
    {
        endRadiusSquare_ = float(endRadius_) * float(endRadius_);
        float dxSquare_ = float(dx_) * float(dx_);
        float dySquare_ = float(dy_) * float(dy_);
        float dRadius = (endRadiusSquare_ - (dxSquare_ + dySquare_));
        if (dRadius == 0.0f) {
            if (dx_) {
                if (dx_ < 0.0f) {
                    ++dx_;
                } else {
                    --dx_;
                }
            }
            if (dy_) {
                if (dy_ < 0.0f) {
                    ++dy_;
                } else {
                    --dy_;
                }
            }
            dxSquare_ = float(dx_) * float(dx_);
            dySquare_ = float(dy_) * float(dy_);
            dRadius = (endRadiusSquare_ - (dxSquare_ + dySquare_));
        }
        mul_ = endRadius_ / dRadius;
    }

    int32_t endRadius_;
    /** In the x-axis direction, the distance from the end circle center to the start circle center */
    int32_t dx_;
    /** In the y-axis direction, the distance from the end circle center to the start circle center */
    int32_t dy_;
    float endRadiusSquare_;
    float mul_;
};

/**
 * SVG linear gradient index calculator.
 */
class GradientLinearCalculateSvg : public virtual GradientSvg {
public:
    int32_t Calculate(int32_t x, int32_t, int32_t, int32_t distance, int32_t size) override
    {
        if (distance < 1) {
            distance = 1;
        }
        int32_t index = (x * size) / distance;
        if (index < 0) {
            index = 0;
        }
        if (index >= size) {
            index = size - 1;
        }
        return index;
    }

    float CalculateFloat(int32_t x, int32_t, int32_t, int32_t distance, int32_t size) override
    {
        if (distance < 1) {
            distance = 1;
        }
        if (size <= 1) {
            return 0.0f;
        }
        return static_cast<float>(x * size) / static_cast<float>(distance);
    }
};
#endif // FEATURE_COMPONENT_SVG
} // namespace OHOS
#endif // GRAPHIC_LITE_FILL_GRADIENT_SVG_H
