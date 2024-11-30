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
#if defined(GRAPHIC_ENABLE_LINEJOIN_FLAG) && GRAPHIC_ENABLE_LINEJOIN_FLAG
          lineJoin_(LineJoin::ROUND_JOIN),
#endif
#if defined(GRAPHIC_ENABLE_LINECAP_FLAG) && GRAPHIC_ENABLE_LINECAP_FLAG
          lineCap_(LineCap::BUTT_CAP),
#endif
#if defined(GRAPHIC_ENABLE_DASH_GENERATE_FLAG) && GRAPHIC_ENABLE_DASH_GENERATE_FLAG
          isDashMode_(false),
          dashOffset_(0),
          dashArray_(nullptr),
          ndashes_(0),
#endif
#if defined(GRAPHIC_ENABLE_LINEJOIN_FLAG) && GRAPHIC_ENABLE_LINEJOIN_FLAG
          miterLimit_(0),
#endif
#if defined(GRAPHIC_ENABLE_GRADIENT_FILL_FLAG) && GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
          linearGradientPoint_({0, 0, 0, 0}),
          radialGradientPoint_({0, 0, 0, 0, 0, 0}),
          stopAndColors_({}),
          gradientflag_(Linear),
#endif
#if defined(GRAPHIC_ENABLE_PATTERN_FILL_FLAG) && GRAPHIC_ENABLE_PATTERN_FILL_FLAG
          patternRepeat_(REPEAT),
#endif
#if defined(GRAPHIC_ENABLE_PATTERN_FILL_FLAG) && GRAPHIC_ENABLE_PATTERN_FILL_FLAG
          image_(nullptr),
#endif
#if defined(GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG) && GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG
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

    void InitDash(const Paint& paint);

    void Init(const Paint& paint);


    /**
     * @brief A destructor used to delete the <b>Paint</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual ~Paint();

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
    void SetStrokeStyle(ColorType color);

    /**
     * @brief Sets fill style.
     *
     * @param color value.
     * @since 1.0
     * @version 1.0
     */
    void SetFillStyle(ColorType color);

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
    void SetStrokeColor(ColorType color);

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
    void SetFillColor(ColorType color);

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

#if defined(GRAPHIC_ENABLE_LINECAP_FLAG) && GRAPHIC_ENABLE_LINECAP_FLAG
    /**
     * @brief Sets the cap type.
     * @see GetLineCap
     * @since 1.0
     * @version 1.0
     */
    void SetLineCap(LineCap lineCap);
#endif

#if defined(GRAPHIC_ENABLE_LINECAP_FLAG) && GRAPHIC_ENABLE_LINECAP_FLAG
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

#if defined(GRAPHIC_ENABLE_LINEJOIN_FLAG) && GRAPHIC_ENABLE_LINEJOIN_FLAG
    /**
     * @brief Sets the style at the path connection of the pen.
     * @see GetLineJoin
     * @since 1.0
     * @version 1.0
     */
    void SetLineJoin(LineJoin lineJoin);
#endif

#if defined(GRAPHIC_ENABLE_LINEJOIN_FLAG) && GRAPHIC_ENABLE_LINEJOIN_FLAG
    /**
     * @brief Sets the spacing limit for sharp corners at path connections.
     * @see GetMiterLimit
     * @since 1.0
     * @version 1.0
     */
    void SetMiterLimit(float miterLimit);
#endif

#if defined(GRAPHIC_ENABLE_LINEJOIN_FLAG) && GRAPHIC_ENABLE_LINEJOIN_FLAG
    float GetMiterLimit() const
    {
        return miterLimit_;
    }
#endif

#if defined(GRAPHIC_ENABLE_LINEJOIN_FLAG) && GRAPHIC_ENABLE_LINEJOIN_FLAG
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

#if defined(GRAPHIC_ENABLE_DASH_GENERATE_FLAG) && GRAPHIC_ENABLE_DASH_GENERATE_FLAG
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
    void SetLineDash(float* lineDashs, const uint32_t ndash);

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
    void SetLineDashOffset(float offset);

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
    void ClearLineDash(void);
#endif

#if defined(GRAPHIC_ENABLE_GRADIENT_FILL_FLAG) && GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
    void createLinearGradient(float startx, float starty, float endx, float endy);

    void addColorStop(float stop, ColorType color);

    void createRadialGradient(float start_x, float start_y, float start_r, float end_x, float end_y, float end_r);

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

#if defined(GRAPHIC_ENABLE_PATTERN_FILL_FLAG) && GRAPHIC_ENABLE_PATTERN_FILL_FLAG
    /*
     * Set hatch patterns for elements
     * @param img Represents the pattern of the hatch，text Represents a fill pattern
     */
    void CreatePattern(const char* img, PatternRepeatMode patternRepeat);

    const char* GetPatternImage() const
    {
        return image_;
    }

    PatternRepeatMode GetPatternRepeatMode() const
    {
        return patternRepeat_;
    }
#endif

#if defined(GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG) && GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG
    /**
     * @brief Sets the shadow blur level.
     * @since 1.0
     * @version 1.0
     */
    void SetShadowBlur(uint16_t radius);

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
    void SetShadowOffsetX(float offset);
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
    void SetShadowOffsetY(float offset);
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
    void SetShadowColor(ColorType color);
    bool HaveShadow() const
    {
        return haveShadow_;
    }
#endif
    /**
     * @brief Sets the alpha of the current drawing.
     */
    void SetGlobalAlpha(float alphaPercentage);

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
    void SetGlobalCompositeOperation(GlobalCompositeOperation globalCompositeOperation);

    /**
     * @brief Get blend mode
     */
    GlobalCompositeOperation GetGlobalCompositeOperation() const
    {
        return globalCompositeOperation_;
    }

    /* Zooms the current drawing to a larger or smaller size */
    void Scale(float scaleX, float scaleY);

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
    void Rotate(float angle);

    /**
     * @brief Rotate current drawing
     * @param angle rotate angle value.
     * @param x translate x coordinate.
     * @param y translate y coordinate.
     * @since 1.0
     * @version 1.0
     */
    void Rotate(float angle, int16_t x, int16_t y);

    /**
     * @brief Remap the (x, y) position on the canvas
     * @param x translate x coordinate.
     * @param y translate y coordinate.
     * @since 1.0
     * @version 1.0
     */
    void Translate(int16_t x, int16_t y);

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
    void SetTransform(float scaleX, float shearX, float shearY, float scaleY, int16_t transLateX, int16_t transLateY);

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
    void Transform(float scaleX, float shearX, float shearY, float scaleY, int16_t transLateX, int16_t transLateY);

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

#if defined(GRAPHIC_ENABLE_BLUR_EFFECT_FLAG) && GRAPHIC_ENABLE_BLUR_EFFECT_FLAG
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
#if defined(GRAPHIC_ENABLE_LINEJOIN_FLAG) && GRAPHIC_ENABLE_LINEJOIN_FLAG
    LineJoin lineJoin_;
#endif

#if defined(GRAPHIC_ENABLE_LINECAP_FLAG) && GRAPHIC_ENABLE_LINECAP_FLAG
    LineCap lineCap_;
#endif
#if defined(GRAPHIC_ENABLE_DASH_GENERATE_FLAG) && GRAPHIC_ENABLE_DASH_GENERATE_FLAG
    bool isDashMode_;  // Is it a dash mode segment.
    float dashOffset_; // dash Point offset.
    float* dashArray_; // dash Point array.
    uint32_t ndashes_; // Length of dasharray
#endif
#if defined(GRAPHIC_ENABLE_LINEJOIN_FLAG) && GRAPHIC_ENABLE_LINEJOIN_FLAG
    float miterLimit_; // Sets the spacing limit for sharp corners at path connections
#endif
#if defined(GRAPHIC_ENABLE_GRADIENT_FILL_FLAG) && GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
    LinearGradientPoint linearGradientPoint_;
    RadialGradientPoint radialGradientPoint_;
    List<StopAndColor> stopAndColors_;
    Gradient gradientflag_;
#endif
#if defined(GRAPHIC_ENABLE_PATTERN_FILL_FLAG) && GRAPHIC_ENABLE_PATTERN_FILL_FLAG
    PatternRepeatMode patternRepeat_;
#endif
#if defined(GRAPHIC_ENABLE_PATTERN_FILL_FLAG) && GRAPHIC_ENABLE_PATTERN_FILL_FLAG
    const char* image_;
#endif
#if defined(GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG) && GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG
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
