/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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
 * @addtogroup UI_Utils
 * @{
 *
 * @brief Defines basic UI utils.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file color.h
 *
 * @brief Defines color attributes for the graphics system and implements common color functions.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_COLOR_H
#define GRAPHIC_LITE_COLOR_H

#include <cmath>
#include <cstdint>

#include "gfx_utils/diagram/common/common_gamma_lut.h"
#include "gfx_utils/heap_base.h"
#include "graphic_config.h"
#include "graphic_math.h"
#if ENABLE_ARM_MATH
#    include "arm_math.h"
#endif
namespace OHOS {
const float PURPLE_MIN = 380.0f;
const float PURPLE_MIDDLE = 420.0f;
const float PURPLE_MAX = 440.0f;
const float BLUE_MAX = 490.0f;
const float CYAN_MAX = 510.0f;
const float GREEN_MAX = 580.0f;
const float ORANGE_MAX = 645.0f;
const float RED_MIN = 700.0f;
const float RED_MAX = 780.0f;
const float FIXED_VALUE = 0.3f;
const float COLOR_CONVERT = 255.0f;
template <class Colorspace>
struct Rgba8T;
struct Linear {
};

struct StandardRgb {
};
using OpacityType = uint8_t;
using Rgba8 = Rgba8T<Linear>;
using Srgba8 = Rgba8T<StandardRgb>;
/**
 * @brief Enumerates opacity values.
 */
enum {
    /** The opacity is 0. */
    OPA_TRANSPARENT = 0,
    /** The opacity is 100%. */
    OPA_OPAQUE = 255,
};

/**
 * @brief Defines the color attribute when the color depth is <b>16</b>.
 */
typedef union {
    struct {
        /** Blue */
        uint16_t blue : 5;
        /** Green */
        uint16_t green : 6;
        /** Red */
        uint16_t red : 5;
    };
    /** Full RGB data */
    uint16_t full;
} Color16;

/**
 * @brief Defines the color attribute when the color depth is <b>24</b>.
 */
struct Color24 {
    /** Blue */
    uint8_t blue;
    /** Green */
    uint8_t green;
    /** Red */
    uint8_t red;
};

/**
 * @brief Defines the color attribute when the color depth is <b>32</b>.
 */
typedef union {
    struct {
        /** Blue */
        uint8_t blue;
        /** Green */
        uint8_t green;
        /** Red */
        uint8_t red;
        /** Alpha (how opaque each pixel is) */
        uint8_t alpha;
    };
    /** Full RGB data */
    uint32_t full;
} Color32;

#if COLOR_DEPTH == 16
using ColorType = Color16;
#elif COLOR_DEPTH == 32
using ColorType = Color32;
#else
#    error "Invalid COLOR_DEPTH, Set it to 16 or 32!"
#endif

struct OrderRgb {
    enum Rgb {
        RED = 0,
        GREEN = 1,
        BLUE = 2
    };
};

struct OrderBgr {
    enum Bgr {
        BLUE = 0,
        GREEN = 1,
        RED = 2
    };
};

struct OrderRgba {
    enum Rgba {
        RED = 0,
        GREEN = 1,
        BLUE = 2,
        ALPHA = 3
    };
};

struct OrderArgb {
    enum Argb {
        ALPHA = 0,
        RED = 1,
        GREEN = 2,
        BLUE = 3
    };
};
struct OrderAbgr {
    enum Abgr {
        ALPHA = 0,
        BLUE = 1,
        GREEN = 2,
        RED = 3
    };
};
struct OrderBgra {
    enum Bgra {
        BLUE = 0,
        GREEN = 1,
        RED = 2,
        ALPHA = 3
    };
};

/**
 * @brief Rgba.
 *
 * Color order: red, green, blue, transparency.
 * Note that the colors in this support floating-point processing.
 * @see Rgba
 * @since 1.0
 * @version 1.0
 */
struct Rgba {
    float redValue;
    float greenValue;
    float blueValue;
    float alphaValue;

    Rgba() {}

    /**
     * @brief Rgba Constructor.
     *
     * @param Red value, green value, blue value, alpha transparency.
     * @since 1.0
     * @version 1.0
     */
    Rgba(float red, float green, float blue, float alpha = 1.0f)
        : redValue(red), greenValue(green), blueValue(blue), alphaValue(alpha) {}

    /**
     * @brief Rgba Constructor.
     *
     * @param Color is the RGBA object, alpha, and transparency.
     * @since 1.0
     * @version 1.0.
     */
    Rgba(const Rgba& color, float alpha)
        : redValue(color.redValue),
          greenValue(color.greenValue),
          blueValue(color.blueValue),
          alphaValue(alpha) {}

    /**
     * @brief Clear, color transparency set to 0.
     *
     * @return Returns a reference to the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    Rgba& Clear()
    {
        redValue = 0;
        greenValue = 0;
        blueValue = 0;
        alphaValue = 0;
        return *this;
    }

    /**
     * @brief Fully transparent.
     *
     * @return Returns a reference to the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    Rgba& Transparent()
    {
        alphaValue = 0;
        return *this;
    }

    /**
     * @brief Set transparency.
     *
     * @param Alpha transparency.
     * @return Returns a reference to the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    Rgba& Opacity(float alpha)
    {
        if (alpha < 0) {
            alphaValue = 0;
        } else if (alpha > 1) {
            alphaValue = 1;
        } else {
            alphaValue = alpha;
        }
        return *this;
    }

    /**
     * @brief Get transparency.
     *
     * @return Return transparency.
     * @since 1.0
     * @version 1.0
     */
    float Opacity() const
    {
        return alphaValue;
    }

    /**
     * @brief Double subtraction.
     *
     * @return Returns a reference to the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    Rgba& Demultiply()
    {
        if (alphaValue == 0) {
            redValue = 0;
            greenValue = 0;
            blueValue = 0;
        } else {
            float alpha = 1.0f / alphaValue;
            redValue *= alpha;
            greenValue *= alpha;
            blueValue *= alpha;
        }
        return *this;
    }
#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
    /**
     * @brief Gradual change.
     *
     * @param RGBA is the RGBA object and K is the coefficient of variation.
     * @return Returns the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    Rgba Gradient(Rgba rgba, float k) const
    {
        Rgba ret;
        ret.redValue = redValue + (rgba.redValue - redValue) * k;
        ret.greenValue = greenValue + (rgba.greenValue - greenValue) * k;
        ret.blueValue = blueValue + (rgba.blueValue - blueValue) * k;
        ret.alphaValue = alphaValue + (rgba.alphaValue - alphaValue) * k;
        return ret;
    }
#endif
    /**
     * @brief Overloaded operator += function.
     *
     * @param RGBA is a reference to the RGBA object.
     * @return Returns a reference to the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    Rgba& operator+=(const Rgba& rgba)
    {
        redValue += rgba.redValue;
        greenValue += rgba.greenValue;
        blueValue += rgba.blueValue;
        alphaValue += rgba.alphaValue;
        return *this;
    }

    /**
     * @brief Overloaded operator *= function.
     *
     * @param Multiplyvalue is the coefficient of multiplication.
     * @return Returns a reference to the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    Rgba& operator*=(float multiplyValue)
    {
        redValue *= multiplyValue;
        greenValue *= multiplyValue;
        blueValue *= multiplyValue;
        alphaValue *= multiplyValue;
        return *this;
    }

    static Rgba NoColor()
    {
        return Rgba(0, 0, 0, 0);
    }

    /**
     * @brief Is the wavelength purple.
     *
     * @param waveLength wavelength.
     * @return Returns the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    inline static Rgba IsPurpleWave(float waveLength)
    {
        if (waveLength >= PURPLE_MIN && waveLength <= PURPLE_MAX) {
            return Rgba(-1.0f * (waveLength - PURPLE_MAX) / (PURPLE_MAX - PURPLE_MIN), 0.0f, 1.0f);
        } else {
            return Rgba(0.0f, 0.0f, 0.0f);
        }
    }
    /**
     * @brief Is the wavelength blue.
     *
     * @param waveLength wavelength.
     * @return Returns the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    inline static Rgba IsBlueWave(float waveLength)
    {
        if (waveLength >= PURPLE_MAX && waveLength <= BLUE_MAX) {
            return Rgba(0.0f, (waveLength - PURPLE_MAX) / (BLUE_MAX - PURPLE_MAX), 1.0f);
        } else {
            return Rgba(0.0f, 0.0f, 0.0f);
        }
    }
    /**
     * @brief Is the wavelength cyan.
     *
     * @param waveLength wavelength.
     * @return Returns the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    inline static Rgba IsCyanWave(float waveLength)
    {
        if (waveLength >= BLUE_MAX && waveLength <= CYAN_MAX) {
            return Rgba(0.0f, 1.0f, -1.0f * (waveLength - CYAN_MAX) / (CYAN_MAX - BLUE_MAX));
        } else {
            return Rgba(0.0f, 0.0f, 0.0f);
        }
    }
    /**
     * @brief Is the wavelength green.
     *
     * @param waveLength wavelength.
     * @return Returns the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    inline static Rgba IsGreenWave(float waveLength)
    {
        if (waveLength >= CYAN_MAX && waveLength <= GREEN_MAX) {
            return Rgba((waveLength - CYAN_MAX) / (GREEN_MAX - CYAN_MAX), 1.0f, 0.0f);
        } else {
            return Rgba(0.0f, 0.0f, 0.0f);
        }
    }
    /**
     * @brief Is the wavelength orange.
     *
     * @param waveLength wavelength.
     * @return Returns the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    inline static Rgba IsOrangeWave(float waveLength)
    {
        if (waveLength >= GREEN_MAX && waveLength <= ORANGE_MAX) {
            return Rgba(1.0f, -1.0f * (waveLength - ORANGE_MAX) / (ORANGE_MAX - GREEN_MAX), 0.0f);
        } else {
            return Rgba(0.0f, 0.0f, 0.0f);
        }
    }
    /**
     * @brief Is the wavelength red.
     *
     * @param waveLength wavelength.
     * @return Returns the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    inline static Rgba IsRedWave(float waveLength)
    {
        if (waveLength >= ORANGE_MAX && waveLength <= RED_MAX) {
            return Rgba(1.0f, 0.0f, 0.0f);
        } else {
            return Rgba(0.0f, 0.0f, 0.0f);
        }
    }
    /**
     * @brief Initialize color based on wavelength.
     *
     * @param waveLength wavelength.
     * @return Returns the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    static Rgba InitColorByWaveLength(float waveLength);
    /**
     * @brief The color value is calculated from the wavelength and gamma value.
     *
     * @param waveLength waveLength，gamma Gamma value.
     * @return Returns the RGBA object.
     * @since 1.0
     * @version 1.0
     */
    static Rgba FromWaveLength(float waveLength, float gamma = 1.0f);

    explicit Rgba(float wavelen, float gamma = 1.0f)
    {
        *this = FromWaveLength(wavelen, gamma);
    }
};

inline Rgba Rgba::InitColorByWaveLength(float waveLength)
{
    Rgba rgba(0.0f, 0.0f, 0.0f);
    rgba += IsPurpleWave(waveLength);
    rgba += IsBlueWave(waveLength);
    rgba += IsCyanWave(waveLength);
    rgba += IsGreenWave(waveLength);
    rgba += IsOrangeWave(waveLength);
    rgba += IsRedWave(waveLength);
    return rgba;
}
inline Rgba Rgba::FromWaveLength(float waveLength, float gamma)
{
    Rgba rgba(0.0f, 0.0f, 0.0f);
    rgba += rgba.InitColorByWaveLength(waveLength);
    float ratio = 1.0f;
    if (waveLength > RED_MIN) {
        ratio = FIXED_VALUE + COEFFICIENT * (RED_MAX - waveLength) / (RED_MAX - RED_MIN);
    } else if (waveLength < PURPLE_MIDDLE) {
        ratio = FIXED_VALUE + COEFFICIENT * (waveLength - PURPLE_MIN) / (PURPLE_MIDDLE - PURPLE_MIN);
    }
    return Rgba(std::pow(rgba.redValue * ratio, gamma), std::pow(rgba.greenValue * ratio, gamma),
        std::pow(rgba.blueValue * ratio, gamma));
}

/**
 * @brief Rgba8T color sequence conversion.
 *
 * Color order: red, green, blue, transparency.
 *
 * @see Rgba8T
 * @since 1.0
 * @version 1.0
 */
template <class Colorspace>
struct Rgba8T {
    uint8_t redValue;
    uint8_t greenValue;
    uint8_t blueValue;
    uint8_t alphaValue;

    enum BaseScale {
        BASE_SHIFT = 8,
        BASE_SCALE = 1 << BASE_SHIFT,
        BASE_MASK = BASE_SCALE - 1,
        BASE_MSB = 1 << (BASE_SHIFT - 1)
    };

    Rgba8T() {}

    /**
     * @brief Rgba8T Constructor
     *
     * @param Red value, green value, blue value, alpha transparency
     * @since 1.0
     * @version 1.0
     */
    Rgba8T(uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha = BASE_MASK)
        : redValue(uint8_t(red)),
        greenValue(uint8_t(green)),
        blueValue(uint8_t(blue)),
        alphaValue(uint8_t(alpha)) {}

    /**
     * @brief Rgba8T Constructor
     *
     * @param Color is a reference to the RGBA object
     * @since 1.0
     * @version 1.0
     */
    Rgba8T(const Rgba& color)
    {
        Convert(*this, color);
    }

    /**
     * @brief Rgba8T Constructor
     *
     * @param Color is the reference of the rgba8t object, and alpha is the transparency
     * @since 1.0
     * @version 1.0
     */
    Rgba8T(const Rgba8T& color, uint32_t alpha)
        : redValue(color.redValue),
        greenValue(color.greenValue),
        blueValue(color.blueValue),
        alphaValue(uint8_t(alpha)) {}

    /**
     * @brief Rgba8T Constructor
     *
     * @param Color is a reference to the rgba8t < T > object
     * @return Rgba8T
     * @since 1.0
     * @version 1.0
     */
    template <class T>
    Rgba8T(const Rgba8T<T>& color)
    {
        Convert(*this, color);
    }

    /**
     * @brief Overloaded RGBA function
     *
     * @return Returns the RGBA object
     * @since 1.0
     * @version 1.0
     */
    operator Rgba() const
    {
        Rgba color;
        Convert(color, *this);
        return color;
    }

    /**
     * @brief Assign the color value in RGBA to rgba8t <linear>
     *
     * @param DST is the reference of rgba8t <linear> object, and Src is the
     *  constant reference of RGBA object
     * @since 1.0
     * @version 1.0
     */
    static void Convert(Rgba8T<Linear>& dst, const Rgba& src)
    {
        dst.redValue = uint8_t(MATH_UROUND(src.redValue * BASE_MASK));
        dst.greenValue = uint8_t(MATH_UROUND(src.greenValue * BASE_MASK));
        dst.blueValue = uint8_t(MATH_UROUND(src.blueValue * BASE_MASK));
        dst.alphaValue = uint8_t(MATH_UROUND(src.alphaValue * BASE_MASK));
    }

    /**
     * @brief Assign the color value in rgba8t <sRGB> to RGBA
     *
     * @param DST is the reference of the RGBA object, and Src is the constant
     *  reference of the rgba8t <sRGB> object
     * @since 1.0
     * @version 1.0.
     */
    static void Convert(Rgba& dst, const Rgba8T<StandardRgb>& src)
    {
        dst.redValue = StandardRgbConv<float>::RgbFromSrgb(src.redValue);
        dst.greenValue = StandardRgbConv<float>::RgbFromSrgb(src.greenValue);
        dst.blueValue = StandardRgbConv<float>::RgbFromSrgb(src.blueValue);
        dst.alphaValue = StandardRgbConv<float>::AlphaFromSrgb(src.alphaValue);
    }

    static inline uint8_t FromFloat(float value)
    {
        return uint8_t(MATH_UROUND(value * BASE_MASK));
    }

    static inline uint8_t EmptyValue()
    {
        return 0;
    }

    inline bool IsTransparent() const
    {
        return alphaValue == 0;
    }

    inline bool IsOpaque() const
    {
        return alphaValue == BASE_MASK;
    }

    static inline uint8_t Multiply(uint8_t valueA, uint8_t valueB)
    {
#if ENABLE_ARM_MATH
        uint32_t uint32_t = __SMUAD(valueA, valueB) + BASE_MSB;
#else
        uint32_t uint32_t = valueA * valueB + BASE_MSB;
#endif
        return uint8_t(((uint32_t >> BASE_SHIFT) + uint32_t) >> BASE_SHIFT);
    }

    static uint8_t DividMultiply(uint8_t valueA, uint8_t valueB)
    {
        if (valueA * valueB == 0) {
            return 0;
        } else if (valueA >= valueB) {
            return BASE_MASK;
        } else {
            if (valueB == 0) {
                valueB = 1;
            }

#if ENABLE_ARM_MATH
            return uint8_t(__UDIV(__SMUAD(valueA, BASE_MASK) + (valueB >> 1), valueB));
#else
            return uint8_t((valueA * BASE_MASK + (valueB >> 1)) / valueB);
#endif
        }
    }

    template <typename T>
    static inline T Downshift(T value, uint32_t digit)
    {
        return value >> digit;
    }

    template <typename T>
    static inline T Downscale(T value)
    {
        return value >> BASE_SHIFT;
    }

    static inline uint8_t MultCover(uint8_t valueA, uint8_t coverValue)
    {
        return Multiply(valueA, coverValue);
    }

    static inline uint8_t ScaleCover(uint8_t coverValue, uint8_t value)
    {
        return Multiply(value, coverValue);
    }

    static inline uint8_t Prelerp(uint8_t valueP, uint8_t valueQ, uint8_t valueA)
    {
        return valueP + valueQ - Multiply(valueP, valueA);
    }

    static inline uint8_t Lerp(uint8_t valueP, uint8_t valueQ, uint8_t valueA)
    {
#if ENABLE_ARM_MATH
        int32_t t = __SMUAD((valueQ - valueP), valueA) + BASE_MSB - (valueP > valueQ);
#else
        int32_t t = (valueQ - valueP) * valueA + BASE_MSB - (valueP > valueQ);

#endif
        return uint8_t(valueP + (((t >> BASE_SHIFT) + t) >> BASE_SHIFT));
    }

    /**
     * @brief Set transparency
     *
     * @param Alpha is transparency
     * @return Returns a reference to the rgba8t object
     * @since 1.0
     * @version 1.0
     */
    Rgba8T& Opacity(float alpha)
    {
        if (alpha < 0) {
            alphaValue = 0;
        } else if (alpha > 1) {
            alphaValue = 1;
        } else {
            alphaValue = static_cast<uint8_t>(MATH_UROUND(alpha * float(BASE_MASK)));
        }
        return *this;
    }

    float Opacity() const
    {
        return alphaValue / BASE_MASK;
    }
#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
    /**
     * @brief Gradient, calculate the new rgba8t object according to the change coefficient
     *
     * @param Color is the reference of the rgba8t object, and K is the coefficient of variation
     * @return Returns the rgba8t object
     * @since 1.0
     * @version 1.0
     */
    inline Rgba8T Gradient(const Rgba8T& color, float k) const
    {
        Rgba8T ret;
        uint32_t increaseK = MATH_UROUND(k * BASE_MASK);
        ret.redValue = Lerp(redValue, color.redValue, increaseK);
        ret.greenValue = Lerp(greenValue, color.greenValue, increaseK);
        ret.blueValue = Lerp(blueValue, color.blueValue, increaseK);
        ret.alphaValue = Lerp(alphaValue, color.alphaValue, increaseK);
        return ret;
    }
#endif
    static Rgba8T NoColor()
    {
        return Rgba8T(0, 0, 0, 0);
    }
};

/**
 * @brief Converts colors in different formats and defines common colors.
 *
 * @since 1.0
 * @version 1.0
 */
class Color : public HeapBase {
public:
    /**
     * @brief Mixes two colors (color 1 and color 2) based on a specified opacity.
     *
     * @param c1 Indicates color 1.
     * @param c2 Indicates color 2.
     * @param mix Indicates the alpha, that is, how opaque each pixel is.

     * @return Returns the color data after mixing.
     * @since 1.0
     * @version 1.0
     */
    static ColorType GetMixColor(ColorType c1, ColorType c2, uint8_t mix);

    /**
     * @brief Obtains the color based on the RGB color value.
     *
     * @param r8 Indicates the intensity of red.
     * @param g8 Indicates the intensity of green.
     * @param b8 Indicates the intensity of blue.
     *
     * @return Returns the color data generated.
     * @since 1.0
     * @version 1.0
     */
    static ColorType GetColorFromRGB(uint8_t r8, uint8_t g8, uint8_t b8);

    /**
     * @brief Obtains the color based on the RGBA color value.
     *
     * @param r8 Indicates the intensity of red.
     * @param g8 Indicates the intensity of green.
     * @param b8 Indicates the intensity of blue.
     * @param alpha Indicates the alpha, that is, how opaque each pixel is.
     *
     * @return Returns the color data generated.
     * @since 1.0
     * @version 1.0
     */
    static ColorType GetColorFromRGBA(uint8_t r8, uint8_t g8, uint8_t b8, uint8_t alpha);

    /**
     * @brief Converts color data into the RGBA8888 format.
     *
     * The color data definition varies according to the color depth.
     *
     * @param color Indicates the color data, which is defined by {@link ColorType}.
     * @return Returns the RGBA8888 color data.
     * @since 1.0
     * @version 1.0
     */
    static uint32_t ColorTo32(ColorType color);

    /**
     * @brief Converts color data with the 16-bit color depth into the RGBA8888 format.
     *
     * @param color Indicates the color data with the 16-bit color depth, which is defined by {@link Color16}.
     * @param alpha Indicates the alpha, that is, how opaque each pixel is.
     * @return Returns the RGBA8888 color data.
     * @since 1.0
     * @version 1.0
     */
    static uint32_t ColorTo32(Color16 color, uint8_t alpha);

    /**
     * @brief Converts color data from the RGBA8888 format into the RGB565 format.
     *
     * @param color Indicates the color data with the 32-bit color depth, which is defined by {@link Color32}.
     * @return Returns the RGB565 color data.
     * @since 1.0
     * @version 1.0
     */
    static uint16_t ColorTo16(Color32 color);

    /**
     * @brief Obtains the color data of white.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType White();

    /**
     * @brief Obtains the color data of silver.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Silver();

    /**
     * @brief Obtains the color data of gray.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Gray();

    /**
     * @brief Obtains the color data of black.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Black();

    /**
     * @brief Obtains the color data of red.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Red();

    /**
     * @brief Obtains the color data of maroon.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Maroon();

    /**
     * @brief Obtains the color data of yellow.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Yellow();

    /**
     * @brief Obtains the color data of olive.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Olive();

    /**
     * @brief Obtains the color data of lime.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Lime();

    /**
     * @brief Obtains the color data of green.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Green();

    /**
     * @brief Obtains the color data of cyan.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Cyan();

    /**
     * @brief Obtains the color data of aqua.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Aqua();

    /**
     * @brief Obtains the color data of teal.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Teal();

    /**
     * @brief Obtains the color data of blue.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Blue();

    /**
     * @brief Obtains the color data of navy.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Navy();

    /**
     * @brief Obtains the color data of magenta.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Magenta();

    /**
     * @brief Obtains the color data of purple.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Purple();

    /**
     * @brief Obtains the color data of orange.
     *
     * @return Returns the color data.
     * @since 1.0
     * @version 1.0
     */
    static ColorType Orange();
};
} // namespace OHOS
#endif
