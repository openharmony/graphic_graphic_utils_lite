/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "gfx_utils/diagram/common/paint.h"
namespace OHOS {
void Paint::InitDash(const Paint& paint)
{
#if defined(GRAPHIC_ENABLE_DASH_GENERATE_FLAG) && GRAPHIC_ENABLE_DASH_GENERATE_FLAG
    if (isDashMode_ && ndashes_ > 0) {
        dashArray_ = new float[ndashes_];
        if (dashArray_) {
            if (memset_s(dashArray_, ndashes_ * sizeof(float), 0, ndashes_ * sizeof(float)) != EOF) {
            }
            for (uint32_t i = 0; i < ndashes_; i++) {
                dashArray_[i] = paint.dashArray_[i];
            }
        } else {
            ndashes_ = 0;
            dashOffset_ = 0;
            isDashMode_ = false;
        }
    } else {
        dashArray_ = nullptr;
    }
#endif
}

/*
 * Initialize data members.
 * style_:       paint style.
 * fillColor_:   Sets the fill color of the pen.
 * strokeColor_: Sets the line color of the pen.
 * opacity_:     Set transparency.
 * strokeWidth_: Set lineweight.
 * lineCap_:     Set pen cap.
 * lineJoin_:    Sets the style at the path connection of the pen.
 * miterLimit_:  Sets the spacing limit for sharp corners at path connections.
 * dashOffset:   dash Point offset.
 * isDrawDash:   Whether to draw dotted line.
 * dashArray:    dash Point group.
 * ndashes:      Number of dotted lines.
 * globalAlpha:  Set element Global alpha.
 * shadowBlurRadius:  Sets the shadow blur radius.
 * shadowOffsetX:     Sets the abscissa offset of the shadow.
 * shadowOffsetY:     Sets the shadow ordinate offset.
 * shadowColor:       Set shadow color.
 */
void Paint::Init(const Paint& paint)
{
    style_ = paint.style_;
    fillColor_ = paint.fillColor_;
    strokeColor_ = paint.strokeColor_;
    strokeWidth_ = paint.strokeWidth_;
    opacity_ = paint.opacity_;
#if defined(GRAPHIC_ENABLE_LINECAP_FLAG) && GRAPHIC_ENABLE_LINECAP_FLAG
    lineCap_ = paint.lineCap_;
#endif
#if defined(GRAPHIC_ENABLE_LINEJOIN_FLAG) && GRAPHIC_ENABLE_LINEJOIN_FLAG
    lineJoin_ = paint.lineJoin_;
#endif
#if defined(GRAPHIC_ENABLE_DASH_GENERATE_FLAG) && GRAPHIC_ENABLE_DASH_GENERATE_FLAG
    isDashMode_ = paint.isDashMode_;
    dashOffset_ = paint.dashOffset_;
    dashArray_ = paint.dashArray_;
    ndashes_ = paint.ndashes_;
#endif
    changeFlag_ = paint.changeFlag_;
    scaleRadioX_ = paint.scaleRadioX_;
    scaleRadioY_ = paint.scaleRadioY_;
    translationX_ = paint.translationX_;
    translationY_ = paint.translationY_;
    InitDash(paint);
#if defined(GRAPHIC_ENABLE_LINEJOIN_FLAG) && GRAPHIC_ENABLE_LINEJOIN_FLAG
    miterLimit_ = paint.miterLimit_;
#endif
#if defined(GRAPHIC_ENABLE_GRADIENT_FILL_FLAG) && GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
    linearGradientPoint_ = paint.linearGradientPoint_;
    radialGradientPoint_ = paint.radialGradientPoint_;
    stopAndColors_ = paint.stopAndColors_;
    gradientflag_ = paint.gradientflag_;
#endif
#if defined(GRAPHIC_ENABLE_PATTERN_FILL_FLAG) && GRAPHIC_ENABLE_PATTERN_FILL_FLAG
    patternRepeat_ = paint.patternRepeat_;
#endif
#if defined(GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG) && GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG
    shadowBlurRadius_ = paint.shadowBlurRadius_;
    shadowOffsetX_ = paint.shadowOffsetX_;
    shadowOffsetY_ = paint.shadowOffsetY_;
    shadowColor_ = paint.shadowColor_;
    haveShadow_ = paint.haveShadow_;
#endif
    globalAlpha_ = paint.globalAlpha_;
    globalCompositeOperation_ = paint.globalCompositeOperation_;
    rotateAngle_ = paint.rotateAngle_;
    transfrom_ = paint.transfrom_;
    haveComposite_ = paint.haveComposite_;
}

void Paint::SetStrokeStyle(ColorType color)
{
    SetStyle(Paint::STROKE_STYLE);
    SetStrokeColor(color);
}

void Paint::SetFillStyle(ColorType color)
{
    SetStyle(Paint::FILL_STYLE);
    SetFillColor(color);
}

void Paint::SetStrokeColor(ColorType color)
{
    strokeColor_ = color;
    changeFlag_ = true;
}

void Paint::SetFillColor(ColorType color)
{
    fillColor_ = color;
    changeFlag_ = true;
}

#if defined(GRAPHIC_ENABLE_LINECAP_FLAG) && GRAPHIC_ENABLE_LINECAP_FLAG
void Paint::SetLineCap(LineCap lineCap)
{
    lineCap_ = lineCap;
    changeFlag_ = true;
}
#endif

#if defined(GRAPHIC_ENABLE_LINEJOIN_FLAG) && GRAPHIC_ENABLE_LINEJOIN_FLAG
void Paint::SetLineJoin(LineJoin lineJoin)
{
    lineJoin_ = lineJoin;
    changeFlag_ = true;
}
#endif

#if defined(GRAPHIC_ENABLE_LINEJOIN_FLAG) && GRAPHIC_ENABLE_LINEJOIN_FLAG
void Paint::SetMiterLimit(float miterLimit)
{
    miterLimit_ = miterLimit;
    changeFlag_ = true;
}
#endif

#if defined(GRAPHIC_ENABLE_DASH_GENERATE_FLAG) && GRAPHIC_ENABLE_DASH_GENERATE_FLAG
void Paint::SetLineDash(float* lineDashs, const uint32_t ndash)
{
    ClearLineDash();
    if (lineDashs == nullptr || ndash == 0) {
        return;
    }
    ndashes_ = ndash;
    isDashMode_ = true;
    dashArray_ = new float[ndashes_];
    if (dashArray_) {
        if (memset_s(dashArray_, ndashes_ * sizeof(float), 0, ndashes_ * sizeof(float)) != EOF) {
        }
        for (uint32_t iIndex = 0; iIndex < ndashes_; iIndex++) {
            dashArray_[iIndex] = lineDashs[iIndex];
        }
    } else {
        ndashes_ = 0;
        dashOffset_ = 0;
        isDashMode_ = false;
    }
    changeFlag_ = true;
}

void Paint::SetLineDashOffset(float offset)
{
    dashOffset_ = offset;
    changeFlag_ = true;
    isDashMode_ = true;
}

void Paint::ClearLineDash(void)
{
    dashOffset_ = 0;
    ndashes_ = 0;
    isDashMode_ = false;
    if (dashArray_ != nullptr) {
        delete[] dashArray_;
        dashArray_ = nullptr;
    }
}
#endif

#if defined(GRAPHIC_ENABLE_GRADIENT_FILL_FLAG) && GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
void Paint::createLinearGradient(float startx, float starty, float endx, float endy)
{
    gradientflag_ = Linear;
    linearGradientPoint_.x0 = startx;
    linearGradientPoint_.y0 = starty;
    linearGradientPoint_.x1 = endx;
    linearGradientPoint_.y1 = endy;
    changeFlag_ = true;
}

void Paint::addColorStop(float stop, ColorType color)
{
    StopAndColor stopAndColor;
    stopAndColor.stop = stop;
    stopAndColor.color = color;
    stopAndColors_.PushBack(stopAndColor);
}

void Paint::createRadialGradient(float start_x, float start_y, float start_r, float end_x, float end_y, float end_r)
{
    gradientflag_ = Radial;
    radialGradientPoint_.x0 = start_x;
    radialGradientPoint_.y0 = start_y;
    radialGradientPoint_.r0 = start_r;
    radialGradientPoint_.x1 = end_x;
    radialGradientPoint_.y1 = end_y;
    radialGradientPoint_.r1 = end_r;
    changeFlag_ = true;
}
#endif

#if defined(GRAPHIC_ENABLE_PATTERN_FILL_FLAG) && GRAPHIC_ENABLE_PATTERN_FILL_FLAG
void Paint::CreatePattern(const char* img, PatternRepeatMode patternRepeat)
{
    image_ = img;
    patternRepeat_ = patternRepeat;
    changeFlag_ = true;
}
#endif

#if defined(GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG) && GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG
void Paint::SetShadowBlur(uint16_t radius)
{
    shadowBlurRadius_ = radius;
    changeFlag_ = true;
}

void Paint::SetShadowOffsetX(float offset)
{
    shadowOffsetX_ = offset;
    changeFlag_ = true;
}

void Paint::SetShadowOffsetY(float offset)
{
    shadowOffsetY_ = offset;
    changeFlag_ = true;
}

void Paint::SetShadowColor(ColorType color)
{
    shadowColor_ = color;
    changeFlag_ = true;
    haveShadow_ = true;
}
#endif

void Paint::SetGlobalAlpha(float alphaPercentage)
{
    if (alphaPercentage > 1) {
        globalAlpha_ = 1.0;
        return;
    }
    if (alphaPercentage < 0) {
        globalAlpha_ = 0.0;
        return;
    }
    globalAlpha_ = alphaPercentage;
    changeFlag_ = true;
}

void Paint::SetGlobalCompositeOperation(GlobalCompositeOperation globalCompositeOperation)
{
    globalCompositeOperation_ = globalCompositeOperation;
    changeFlag_ = true;
    if (globalCompositeOperation != SOURCE_OVER) {
        haveComposite_ = true;
    }
}

void Paint::Scale(float scaleX, float scaleY)
{
    this->scaleRadioX_ *= scaleX;
    this->scaleRadioY_ *= scaleX;
    if (rotateAngle_ > 0.0f || rotateAngle_ < 0) {
        transfrom_.Rotate(-rotateAngle_ * PI / BOXER);
        transfrom_.Scale(scaleX, scaleY);
        transfrom_.Rotate(rotateAngle_ * PI / BOXER);
    } else {
        transfrom_.Scale(scaleX, scaleY);
    }
    changeFlag_ = true;
}

void Paint::Rotate(float angle)
{
    changeFlag_ = true;
    transfrom_.Rotate(angle * PI / BOXER);
    rotateAngle_ += angle;
}

void Paint::Rotate(float angle, int16_t x, int16_t y)
{
    transfrom_.Translate(-x, -y);
    transfrom_.Rotate(angle * PI / BOXER);
    rotateAngle_ += angle;
    transfrom_.Translate(x, y);
    changeFlag_ = true;
}

void Paint::Translate(int16_t x, int16_t y)
{
    changeFlag_ = true;
    transfrom_.Translate(x, y);
    this->translationX_ += x;
    this->translationY_ += y;
}

void Paint::SetTransform(float scaleX, float shearX, float shearY,
                         float scaleY, int16_t transLateX, int16_t transLateY)
{
    transfrom_.Reset();
    rotateAngle_ = 0;
    Transform(scaleX, shearX, shearY, scaleY, transLateX, transLateY);
    changeFlag_ = true;
}

void Paint::Transform(float scaleX, float shearX, float shearY, float scaleY, int16_t transLateX, int16_t transLateY)
{
    changeFlag_ = true;
    this->translationX_ += transLateX;
    this->translationY_ += transLateY;
    transLateX += transfrom_.GetData()[2];
    transLateY += transfrom_.GetData()[5];
    transfrom_.Translate(-transfrom_.GetData()[2], -transfrom_.GetData()[5]);
    Scale(scaleX, scaleY);
    transfrom_.Translate(transLateX, transLateY);
    transfrom_.SetData(1, transfrom_.GetData()[1] + shearX);
    transfrom_.SetData(3, transfrom_.GetData()[3] + shearY);
}
} // namespace OHOS