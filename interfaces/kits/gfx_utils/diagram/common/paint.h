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
#ifndef GRAPHIC_LITE_PAINT_H
#define GRAPHIC_LITE_PAINT_H

#include "gfx_utils/diagram/imagefilter/filter_blur.h"
#include "gfx_utils/diagram/spancolorfill/fill_pattern_rgba.h"
#include "gfx_utils/diagram/vertexprimitive/geometry_math_stroke.h"
#include "gfx_utils/diagram/vertexprimitive/geometry_path_storage.h"
#include "gfx_utils/list.h"

namespace OHOS {

/**
 * @brief Defines the basic styles of graphs drawn on canvases.
 *
 * @since 1.0
 * @version 1.0
 */
class Paint : public HeapBase {
    const uint16_t DEFAULT_STROKE_WIDTH = 2;
public:
    /**
     * @brief A constructor used to create a <b>Paint</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    Paint()
        : style_(PaintStyle::STROKE_FILL_STYLE),
          fillColor_(Color::Black()),
          strokeColor_(Color::White()),
          opacity_(OPA_OPAQUE),
          strokeWidth_(DEFAULT_STROKE_WIDTH),
          changeFlag_(false),
#if GRAPHIC_ENABLE_LINEJOIN_FLAG
          lineJoin_(LineJoin::ROUND_JOIN),
#endif
#if GRAPHIC_ENABLE_LINECAP_FLAG
          lineCap_(LineCap::BUTT_CAP),
#endif
#if GRAPHIC_ENABLE_DASH_GENERATE_FLAG
          isDashMode_(false),
          dashOffset_(0),
          dashArray_(nullptr),
          ndashes_(0),
#endif
#if GRAPHIC_ENABLE_LINEJOIN_FLAG
          miterLimit_(0),
#endif
#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
          linearGradientPoint_({0, 0, 0, 0}),
          radialGradientPoint_({0, 0, 0, 0, 0, 0}),
          stopAndColors_({}),
          gradientflag_(Linear),
#endif
#if GRAPHIC_ENABLE_PATTERN_FILL_FLAG
          patternRepeat_(REPEAT),
#endif
#if GRAPHIC_ENABLE_PATTERN_FILL_FLAG
          image_(nullptr),
#endif
#if GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG
          shadowBlurRadius_(0),
          shadowOffsetX_(0.0f),
          shadowOffsetY_(0.0f),
          shadowColor_(Color::Black()),
          haveShadow_(false),
#endif
          globalAlpha_(1.0),
          globalCompositeOperation_(SOURCE_OVER),
          rotateAngle_(0),
          scaleRadioX_(1.0f),
          scaleRadioY_(1.0f),
          translationX_(0),
          translationY_(0),
          haveComposite_(false)
    {
    }

    Paint(const Paint& paint)
    {
        Init(paint);
    }
    void InitDash(const Paint& paint)
    {
#if GRAPHIC_ENABLE_DASH_GENERATE_FLAG
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
    void Init(const Paint& paint)
    {
        style_ = paint.style_;
        fillColor_ = paint.fillColor_;
        strokeColor_ = paint.strokeColor_;
        strokeWidth_ = paint.strokeWidth_;
        opacity_ = paint.opacity_;
#if GRAPHIC_ENABLE_LINECAP_FLAG
        lineCap_ = paint.lineCap_;
#endif
#if GRAPHIC_ENABLE_LINEJOIN_FLAG
        lineJoin_ = paint.lineJoin_;
#endif
#if GRAPHIC_ENABLE_DASH_GENERATE_FLAG
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
#if GRAPHIC_ENABLE_LINEJOIN_FLAG
        miterLimit_ = paint.miterLimit_;
#endif
#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
        linearGradientPoint_ = paint.linearGradientPoint_;
        radialGradientPoint_ = paint.radialGradientPoint_;
        stopAndColors_ = paint.stopAndColors_;
        gradientflag_ = paint.gradientflag_;
#endif
#if GRAPHIC_ENABLE_PATTERN_FILL_FLAG
        patternRepeat_ = paint.patternRepeat_;
#endif
#if GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG
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

    /**
     * @brief A destructor used to delete the <b>Paint</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual ~Paint() {}

    const Paint& operator=(const Paint& paint)
    {
        Init(paint);
        return *this;
    }
    /**
     * @brief Enumerates paint styles of a closed graph. The styles are invalid for non-closed graphs.
     */
    enum PaintStyle {
        /** Stroke only */
        STROKE_STYLE = 1,
        /** Fill only */
        FILL_STYLE,
        /** Stroke and fill */
        STROKE_FILL_STYLE,
        /** Gradual change */
        GRADIENT,
        /** Image mode */
        PATTERN
    };

    struct LinearGradientPoint {
        /**  Start point coordinate x  */
        float x0;
        /**  Start point coordinate y  */
        float y0;
        /**  End point coordinate x  */
        float x1;
        /**  End point coordinate y  */
        float y1;
    };

    struct RadialGradientPoint {
        /**  Start dot coordinate x  */
        float x0;
        /** Start dot coordinate y  */
        float y0;
        /**  Start circle radius r0  */
        float r0;
        /**  End dot coordinates x  */
        float x1;
        /**  End dot coordinates y  */
        float y1;
        /**  Start circle radius r0  */
        float r1;
    };

    struct StopAndColor {
        /** Values between 0.0 and 1.0 represent the position between the beginning and end of the ramp.  */
        float stop;
        /** The color value displayed at the end */
        ColorType color;
    };

    enum Gradient { Linear, Radial };

    /**
     * @brief Sets the paint style of a closed graph.
     *
     * @param style Indicates the paint style. Stroke and fill are set by default.
     * For details, see {@link PaintStyle}.
     * @see GetStyle
     * @since 1.0
     * @version 1.0
     */
    void SetStyle(PaintStyle style)
    {
        style_ = style;
    }

    /**
     * @brief Sets the paint style.
     *
     * @param color value.
     * @since 1.0
     * @version 1.0
     */
    void SetStrokeStyle(ColorType color)
    {
        SetStyle(Paint::STROKE_STYLE);
        SetStrokeColor(color);
    }

    /**
     * @brief Sets fill style.
     *
     * @param color value.
     * @since 1.0
     * @version 1.0
     */
    void SetFillStyle(ColorType color)
    {
        SetStyle(Paint::FILL_STYLE);
        SetFillColor(color);
    }

    /**
     * @brief Sets the paint stroke style of a closed graph.
     *
     * @param style Indicates the paint style. Stroke and fill are set by default.
     * @since 1.0
     * @version 1.0
     */
    void SetStrokeStyle(PaintStyle style)
    {
        SetStyle(style);
    }

    /**
     * @brief Sets the paint fill style of a closed graph.
     *
     * @param style Indicates the paint style. Stroke and fill are set by default.
     * @since 1.0
     * @version 1.0
     */
    void SetFillStyle(PaintStyle style)
    {
        SetStyle(style);
    }

    /**
     * @brief Obtains the paint style of a closed graph.
     *
     * @return Returns the paint style. For details, see {@link PaintStyle}.
     * @see SetStyle
     * @since 1.0
     * @version 1.0
     */
    PaintStyle GetStyle() const
    {
        return style_;
    }

    /**
     * @brief Sets the width of a line or border.
     *
     * @param width Indicates the line width when a line is drawn or the border width when a closed graph is drawn.
     *        The width is extended to both sides.
     * @see GetStrokeWidth
     * @since 1.0
     * @version 1.0
     */
    void SetStrokeWidth(uint16_t width)
    {
        strokeWidth_ = width;
    }

    /**
     * @brief Obtains the width of a line or border.
     *
     * @return Returns the line width if a line is drawn or the border width if a closed graph is drawn.
     * @see SetStrokeWidth
     * @since 1.0
     * @version 1.0
     */
    uint16_t GetStrokeWidth() const
    {
        return strokeWidth_;
    }

    /**
     * @brief Sets the color of a line or border.
     *
     * @param color Indicates the line color when a line is drawn or the border color when a closed graph is drawn.
     * @see GetStrokeColor
     * @since 1.0
     * @version 1.0
     */
    void SetStrokeColor(ColorType color)
    {
        strokeColor_ = color;
        changeFlag_ = true;
    }

    /**
     * @brief Obtains the color of a line or border.
     *
     * @return Returns the line color if a line is drawn or the border color if a closed graph is drawn.
     * @see SetStrokeWidth
     * @since 1.0
     * @version 1.0
     */
    ColorType GetStrokeColor() const
    {
        return strokeColor_;
    }

    /**
     * @brief Sets fill color.
     *
     * This function is valid only for closed graphs.
     *
     * @param color Indicates the fill color to set.
     * @see GetFillColor
     * @since 1.0
     * @version 1.0
     */
    void SetFillColor(ColorType color)
    {
        fillColor_ = color;
        changeFlag_ = true;
    }

    /**
     * @brief Obtains the fill color.
     *
     * @return Returns the fill color.
     * @see SetFillColor
     * @since 1.0
     * @version 1.0
     */
    ColorType GetFillColor() const
    {
        return fillColor_;
    }

    /**
     * @brief Sets the opacity.
     *
     * The setting takes effect for the entire graph, including the border, line color, and fill color.
     *
     * @param opacity Indicates the opacity. The value range is [0, 255].
     * @see GetOpacity
     * @since 1.0
     * @version 1.0
     */
    void SetOpacity(uint8_t opacity)
    {
        opacity_ = opacity;
    }

    /**
     * @brief Obtains the opacity.
     *
     * @return Returns the opacity.
     * @see SetOpacity
     * @since 1.0
     * @version 1.0
     */
    uint8_t GetOpacity() const
    {
        return opacity_;
    }

    bool GetChangeFlag() const
    {
        return changeFlag_;
    }
#if GRAPHIC_ENABLE_LINECAP_FLAG
    /**
     * @brief Sets the cap type.
     * @see GetLineCap
     * @since 1.0
     * @version 1.0
     */
    void SetLineCap(LineCap lineCap)
    {
        lineCap_ = lineCap;
        changeFlag_ = true;
    }
#endif

#if GRAPHIC_ENABLE_LINECAP_FLAG
    /**
     * @brief Gets the cap type.
     * @see SetLineCap
     * @since 1.0
     * @version 1.0
     */
    LineCap GetLineCap() const
    {
        return lineCap_;
    }
#endif
#if GRAPHIC_ENABLE_LINEJOIN_FLAG
    /**
     * @brief Sets the style at the path connection of the pen.
     * @see GetLineJoin
     * @since 1.0
     * @version 1.0
     */
    void SetLineJoin(LineJoin lineJoin)
    {
        lineJoin_ = lineJoin;
        changeFlag_ = true;
    }
#endif
#if GRAPHIC_ENABLE_LINEJOIN_FLAG
    /**
     * @brief Sets the spacing limit for sharp corners at path connections.
     * @see GetMiterLimit
     * @since 1.0
     * @version 1.0
     */
    void SetMiterLimit(float miterLimit)
    {
        miterLimit_ = miterLimit;
        changeFlag_ = true;
    }
#endif
#if GRAPHIC_ENABLE_LINEJOIN_FLAG
    float GetMiterLimit() const
    {
        return miterLimit_;
    }
#endif
#if GRAPHIC_ENABLE_LINEJOIN_FLAG
    /**
     * @brief Gets the style at the path connection of the pen.
     * @see SetLineJoin
     * @since 1.0
     * @version 1.0
     */
    LineJoin GetLineJoin() const
    {
        return lineJoin_;
    }
#endif
#if GRAPHIC_ENABLE_DASH_GENERATE_FLAG
    bool IsLineDash() const
    {
        return isDashMode_;
    }

    /**
     * @brief Sets the array and number of dashes.
     * @param lineDashs Represents an array of dotted lines,ndash Indicates the number of dotted lines
     * @since 1.0
     * @version 1.0
     */
    void SetLineDash(float* lineDashs, const uint32_t ndash)
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

    /**
     * @brief Get dash array
     * @return
     */
    float* GetLineDash() const
    {
        return dashArray_;
    }

    float GetLineDashOffset() const
    {
        return dashOffset_;
    }

    /**
     * @brief Sets the offset of the dash mode start point
     * @see GetLineDashOffset
     * @since 1.0
     * @version 1.0
     */
    void SetLineDashOffset(float offset)
    {
        dashOffset_ = offset;
        changeFlag_ = true;
        isDashMode_ = true;
    }

    /**
     * @brief Get dash array length
     * @return
     */
    uint32_t GetLineDashCount() const
    {
        return ndashes_;
    }

    /**
     * @brief Empty the dotted line and draw it instead.
     * @since 1.0
     * @version 1.0
     */
    void ClearLineDash(void)
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
#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
    void createLinearGradient(float startx, float starty, float endx, float endy)
    {
        gradientflag_ = Linear;
        linearGradientPoint_.x0 = startx;
        linearGradientPoint_.y0 = starty;
        linearGradientPoint_.x1 = endx;
        linearGradientPoint_.y1 = endy;
        changeFlag_ = true;
    }

    void addColorStop(float stop, ColorType color)
    {
        StopAndColor stopAndColor;
        stopAndColor.stop = stop;
        stopAndColor.color = color;
        stopAndColors_.PushBack(stopAndColor);
    }

    void createRadialGradient(float start_x, float start_y, float start_r, float end_x, float end_y, float end_r)
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

    List<StopAndColor> getStopAndColor() const
    {
        return stopAndColors_;
    }

    LinearGradientPoint GetLinearGradientPoint() const
    {
        return linearGradientPoint_;
    }

    RadialGradientPoint GetRadialGradientPoint() const
    {
        return radialGradientPoint_;
    }

    Gradient GetGradient() const
    {
        return gradientflag_;
    }
#endif
#if GRAPHIC_ENABLE_PATTERN_FILL_FLAG
    /*
     * Set hatch patterns for elements
     * @param img Represents the pattern of the hatch，text Represents a fill pattern
     */
    void CreatePattern(const char* img, PatternRepeatMode patternRepeat)
    {
        image_ = img;
        patternRepeat_ = patternRepeat;
        changeFlag_ = true;
    }

    const char* GetPatternImage() const
    {
        return image_;
    }

    PatternRepeatMode GetPatternRepeatMode() const
    {
        return patternRepeat_;
    }
#endif

#if GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG
    /**
     * @brief Sets the shadow blur level.
     * @since 1.0
     * @version 1.0
     */
    void SetShadowBlur(uint16_t radius)
    {
        shadowBlurRadius_ = radius;
        changeFlag_ = true;
    }

    /**
     * @brief Gets the shadow blur level.
     * @since 1.0
     * @version 1.0
     */
    uint16_t GetShadowBlur() const
    {
        return shadowBlurRadius_;
    }

    /**
     * @brief Gets the abscissa offset of the shadow.
     * @since 1.0
     * @version 1.0
     */
    float GetShadowOffsetX() const
    {
        return shadowOffsetX_;
    }
    /**
     * @brief Sets the abscissa offset of the shadow.
     * @since 1.0
     * @version 1.0
     */
    void SetShadowOffsetX(float offset)
    {
        shadowOffsetX_ = offset;
        changeFlag_ = true;
    }
    /**
     * @brief Gets the shadow ordinate offset.
     * @since 1.0
     * @version 1.0
     */
    float GetShadowOffsetY() const
    {
        return shadowOffsetY_;
    }
    /**
     * @brief Sets the shadow ordinate offset.
     * @since 1.0
     * @version 1.0
     */
    void SetShadowOffsetY(float offset)
    {
        shadowOffsetY_ = offset;
        changeFlag_ = true;
    }
    /**
     * @brief Gets the color value of the shadow.
     * @since 1.0
     * @version 1.0
     */
    ColorType GetShadowColor() const
    {
        return shadowColor_;
    }
    /**
     * @brief Sets the color value of the shadow.
     * @since 1.0
     * @version 1.0
     */
    void SetShadowColor(ColorType color)
    {
        shadowColor_ = color;
        changeFlag_ = true;
        haveShadow_ = true;
    }
    bool HaveShadow() const
    {
        return haveShadow_;
    }
#endif
    /**
     * @brief Sets the alpha of the current drawing.
     */
    void SetGlobalAlpha(float alphaPercentage)
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

    /**
     * @brief get the alpha of the current drawing
     * @return Returns the alpha of the current drawing
     * @since 1.0
     * @version 1.0
     */
    float GetGlobalAlpha() const
    {
        return globalAlpha_;
    }

    /**
     * @brief Set blend mode
     */
    void SetGlobalCompositeOperation(GlobalCompositeOperation globalCompositeOperation)
    {
        globalCompositeOperation_ = globalCompositeOperation;
        changeFlag_ = true;
        if (globalCompositeOperation != SOURCE_OVER) {
            haveComposite_ = true;
        }
    }

    /**
     * @brief Get blend mode
     */
    GlobalCompositeOperation GetGlobalCompositeOperation() const
    {
        return globalCompositeOperation_;
    }

    /* Zooms the current drawing to a larger or smaller size */
    void Scale(float scaleX, float scaleY)
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

    /**
     * @brief get the x coordinate scale value
     * @since 1.0
     * @version 1.0
     */
    float GetScaleX() const
    {
        return this->scaleRadioX_;
    }

    /**
     * @brief get the y coordinate scale value
     * @since 1.0
     * @version 1.0
     */
    float GetScaleY() const
    {
        return this->scaleRadioY_;
    }

    /**
     * @brief Rotate current drawing
     * @param angle rotate angle value.
     * @since 1.0
     * @version 1.0
     */
    void Rotate(float angle)
    {
        changeFlag_ = true;
        transfrom_.Rotate(angle * PI / BOXER);
        rotateAngle_ += angle;
    }

    /**
     * @brief Rotate current drawing
     * @param angle rotate angle value.
     * @param x translate x coordinate.
     * @param y translate y coordinate.
     * @since 1.0
     * @version 1.0
     */
    void Rotate(float angle, int16_t x, int16_t y)
    {
        transfrom_.Translate(-x, -y);
        transfrom_.Rotate(angle * PI / BOXER);
        rotateAngle_ += angle;
        transfrom_.Translate(x, y);
        changeFlag_ = true;
    }

    /**
     * @brief Remap the (x, y) position on the canvas
     * @param x translate x coordinate.
     * @param y translate y coordinate.
     * @since 1.0
     * @version 1.0
     */
    void Translate(int16_t x, int16_t y)
    {
        changeFlag_ = true;
        transfrom_.Translate(x, y);
        this->translationX_ += x;
        this->translationY_ += y;
    }

    /**
     * @brief Gets the x position on the remapping canvas
     * @since 1.0
     * @version 1.0
     */
    int16_t GetTranslateX() const
    {
        return this->translationX_;
    }

    /**
     * @brief Gets the Y position on the remapping canvas
     * @since 1.0
     * @version 1.0
     */
    int16_t GetTranslateY() const
    {
        return this->translationY_;
    }

    /**
     * @brief Resets the current conversion to the identity matrix. Then run transform ()
     * @param scaleX scale x value.
     * @param shearX shear x value.
     * @param shearY shear y value.
     * @param scaleY scale y value
     * @param transLateX translate x coordinate.
     * @param transLateY translate y coordinate.
     * @since 1.0
     * @version 1.0
     */
    void SetTransform(float scaleX, float shearX, float shearY, float scaleY, int16_t transLateX, int16_t transLateY)
    {
        transfrom_.Reset();
        rotateAngle_ = 0;
        Transform(scaleX, shearX, shearY, scaleY, transLateX, transLateY);
        changeFlag_ = true;
    }

    /**
     * @brief Resets the current conversion to the identity matrix. Then run transform ()
     * @param scaleX scale x value.
     * @param shearX shear x value.
     * @param shearY shear y value.
     * @param scaleY scale y value
     * @param transLateX translate x coordinate.
     * @param transLateY translate y coordinate.
     * @since 1.0
     * @version 1.0
     */
    void Transform(float scaleX, float shearX, float shearY, float scaleY, int16_t transLateX, int16_t transLateY)
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

    /**
     * @brief Gets the Trans Affine
     * @since 1.0
     * @version 1.0
     */
    TransAffine GetTransAffine() const
    {
        return transfrom_;
    }

    /**
     * @brief Gets the Rotate Angle
     * @since 1.0
     * @version 1.0
     */
    float GetRotateAngle() const
    {
        return rotateAngle_;
    }

    bool HaveComposite() const
    {
        return haveComposite_;
    }

#if GRAPHIC_ENABLE_BLUR_EFFECT_FLAG
    Filterblur drawBlur;
    Filterblur GetDrawBoxBlur() const
    {
        return drawBlur;
    }
#endif

private:
    PaintStyle style_;
    ColorType fillColor_;
    ColorType strokeColor_;
    uint8_t opacity_;
    uint16_t strokeWidth_;
    bool changeFlag_;
#if GRAPHIC_ENABLE_LINEJOIN_FLAG
    LineJoin lineJoin_;
#endif

#if GRAPHIC_ENABLE_LINECAP_FLAG
    LineCap lineCap_;
#endif
#if GRAPHIC_ENABLE_DASH_GENERATE_FLAG
    bool isDashMode_;  // Is it a dash mode segment.
    float dashOffset_; // dash Point offset.
    float* dashArray_; // dash Point array.
    uint32_t ndashes_; // Length of dasharray
#endif
#if GRAPHIC_ENABLE_LINEJOIN_FLAG
    float miterLimit_; // Sets the spacing limit for sharp corners at path connections
#endif
#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
    LinearGradientPoint linearGradientPoint_;
    RadialGradientPoint radialGradientPoint_;
    List<StopAndColor> stopAndColors_;
    Gradient gradientflag_;
#endif
#if GRAPHIC_ENABLE_PATTERN_FILL_FLAG
    PatternRepeatMode patternRepeat_;
#endif
#if GRAPHIC_ENABLE_PATTERN_FILL_FLAG
    const char* image_;
#endif
#if GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG
    uint16_t shadowBlurRadius_; // Sets the shadow blur radius.
    float shadowOffsetX_;       // Sets the abscissa offset of the shadow.
    float shadowOffsetY_;       // Sets the shadow ordinate offset.
    ColorType shadowColor_;     // Set shadow color.
    bool haveShadow_;           // Is there a shadow currently.
#endif
    float globalAlpha_;                                 // The transparency of the current drawing is 0-1 percent
    GlobalCompositeOperation globalCompositeOperation_; // Mixed image mode
    float rotateAngle_;                                 // Rotation angle in degrees
    float scaleRadioX_;
    float scaleRadioY_;
    int32_t translationX_;
    int32_t translationY_;
    TransAffine transfrom_; // matrix.
    bool haveComposite_;
};
} // namespace OHOS

#endif // GRAPHIC_LITE_PAINT_H
