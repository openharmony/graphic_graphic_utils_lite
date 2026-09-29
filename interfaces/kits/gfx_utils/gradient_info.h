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
 * @file gradient_info.h
 * @brief Core data contract for linear gradients, shared across repositories.
 *
 * This header defines every data type required by the linear gradient feature
 * of the lite graphic system. It is the single data contract between the ACE
 * engine layer (CSS parsing) and the UI drawing layer (rendering).
 *
 * Adaptation notes:
 *   - Colors reuse the existing ui_lite ColorType union instead of a bespoke
 *     Color class, so no conversion is required at the repository boundary.
 *   - ColorType layout: union { uint32_t full; struct { uint8_t alpha, red,
 *     green, blue; }; }
 *   - Relationship with the legacy GradientColor union (declared in style.h):
 *     * GradientColor is the compact union consumed by the low level drawing
 *       code and its linear part only carries two colors.
 *     * GradientInfo (this file) is the declarative upper layer carrier and
 *       supports an arbitrary number of color stops.
 *     * The adapter layer converts GradientInfo into drawing commands.
 *
 * Types provided by this header:
 *   - GradientType              gradient family (linear / radial / sweep)
 *   - CssGradientDirection      the eight CSS keywords plus a custom angle
 *   - GradientColorStop         one color stop (ColorType + normalized offset)
 *   - GradientInfo              the gradient payload passed across repositories
 *
 * Shared helpers provided by this header (single source of truth, previously
 * duplicated in app_gradient_parser.cpp and linear_gradient_builder.cpp):
 *   - CssGradientDirectionToAngle()  direction keyword -> CSS angle
 *   - NormalizeCssAngleDegrees()     wrap an arbitrary angle into [0, 360)
 *   - ResolveCssGradientAngle()      resolve the effective angle of a gradient
 */

#ifndef GRAPHIC_UI_GRADIENT_INFO_H
#define GRAPHIC_UI_GRADIENT_INFO_H

#include <cmath>
#include <cstdint>
#include <cstring>
#include <new>
#include "gfx_utils/color.h"
/*
 * Error reporting for the inline helpers below. graphic_log.h lives in the very
 * same gfx_utils public config as the already required color.h (and color.h
 * itself pulls graphic_config.h, the only extra dependency of graphic_log.h),
 * so every existing consumer of this header keeps building unchanged.
 */
#include "gfx_utils/graphic_log.h"

namespace OHOS {

/* ==========================================================================
 * Gradient type
 * ========================================================================== */

/**
 * @brief Gradient family.
 *
 * Only LINEAR is implemented in the current version. RADIAL and SWEEP are
 * reserved so that future extensions only need to add parsing and drawing
 * logic without touching the shared enumeration values.
 */
enum class GradientType : uint8_t {
    LINEAR = 0, /* Linear gradient (implemented) */
    RADIAL = 1, /* Radial gradient (reserved) */
    SWEEP  = 2, /* Sweep gradient (reserved) */
};

/* ==========================================================================
 * Linear gradient direction
 * ========================================================================== */

/**
 * @brief CSS standard linear gradient directions.
 *
 * Covers every direction keyword accepted by CSS linear-gradient(). The value
 * in parentheses is the equivalent CSS angle (0 degrees points up, the angle
 * grows clockwise):
 *
 *   TO_TOP (0)             bottom  -> top
 *   TO_TOP_RIGHT (45)      bottom left -> top right
 *   TO_RIGHT (90)          left    -> right
 *   TO_BOTTOM_RIGHT (135)  top left -> bottom right
 *   TO_BOTTOM (180)        top     -> bottom (CSS default)
 *   TO_BOTTOM_LEFT (225)   top right -> bottom left
 *   TO_LEFT (270)          right   -> left
 *   TO_TOP_LEFT (315)      bottom right -> top left
 *   CUSTOM_ANGLE           explicit angle such as 72deg
 *   NONE                   no gradient (default value)
 *
 * The numeric values 0..7 intentionally match the legacy
 * GradientColor.direction field so that GetDirectionValue() is a plain cast.
 */
enum class CssGradientDirection : uint8_t {
    TO_TOP          = 0,
    TO_TOP_RIGHT    = 1,
    TO_RIGHT        = 2,
    TO_BOTTOM_RIGHT = 3,
    TO_BOTTOM       = 4,
    TO_BOTTOM_LEFT  = 5,
    TO_LEFT         = 6,
    TO_TOP_LEFT     = 7,
    CUSTOM_ANGLE    = 8,
    NONE            = 255,
};

/* ==========================================================================
 * Named constants (replace the angle / channel / count magic numbers that used
 * to be scattered over the parser, the adapter and the drawing path)
 *
 * G.INC.10-CPP: a header must not define internal (non externally visible)
 * symbols, so none of the constants below carries the `static` specifier any
 * more. They stay in this header as `constexpr` values instead of moving to a
 * .cpp file because:
 *   1. They are used in constant expressions (default member initializers,
 *      switch/compare against enum-like limits); an `extern const` defined in a
 *      translation unit could no longer serve that purpose.
 *   2. This header is the cross repository data contract - ui_lite consumers
 *      (linear_gradient_builder.cpp, ui_view.cpp, ui_button.cpp) include it
 *      without linking the ace_engine_lite gradient_parser library, so a
 *      definition living in a .cpp of this module would break their link step.
 * `inline constexpr` is deliberately not used either: the module is built with
 * -std=c++14 (see BUILD.gn) and ui_lite with c++11, where inline variables do
 * not exist yet. Dropping `static` keeps the exact same compile time semantics.
 * ========================================================================== */

/** CSS angle of each direction keyword, in degrees. */
constexpr float CSS_ANGLE_TO_TOP          = 0.0f;
constexpr float CSS_ANGLE_TO_TOP_RIGHT    = 45.0f;
constexpr float CSS_ANGLE_TO_RIGHT        = 90.0f;
constexpr float CSS_ANGLE_TO_BOTTOM_RIGHT = 135.0f;
constexpr float CSS_ANGLE_TO_BOTTOM       = 180.0f;
constexpr float CSS_ANGLE_TO_BOTTOM_LEFT  = 225.0f;
constexpr float CSS_ANGLE_TO_LEFT         = 270.0f;
constexpr float CSS_ANGLE_TO_TOP_LEFT     = 315.0f;

/** Angle used whenever a direction cannot be resolved (CSS default). */
constexpr float CSS_ANGLE_DEFAULT = CSS_ANGLE_TO_BOTTOM;

/** One full revolution in degrees, used to wrap arbitrary angles. */
constexpr float CSS_ANGLE_FULL_TURN = 360.0f;

/** Right angle in degrees, used by the CSS-to-math angle conversion. */
constexpr float CSS_ANGLE_QUARTER_TURN = 90.0f;

/** Half revolution in degrees, used by the radian-to-degree conversion. */
constexpr float CSS_ANGLE_HALF_TURN = 180.0f;

/** A gradient needs at least a start color and an end color. */
constexpr uint8_t GRADIENT_MIN_COLOR_STOP_COUNT = 2;

/**
 * @brief Painted range of the gradient line.
 *
 * W3C CSS Images Module Level 3, 3.4.1: 0.0 is the starting point of the
 * gradient line and 1.0 its ending point, and only that segment covers the
 * box. Authored color stop positions are NOT restricted to this range: a stop
 * may sit at a negative position or beyond 100%, in which case it lives on the
 * (infinite) gradient line outside the box and still drives the interpolation
 * of the visible segment.
 */
constexpr float GRADIENT_OFFSET_MIN = 0.0f;
constexpr float GRADIENT_OFFSET_MAX = 1.0f;

/** Sentinel offset meaning "not specified by CSS, infer it during normalization". */
constexpr float GRADIENT_OFFSET_UNSPECIFIED = -1.0f;

/**
 * @brief Tolerance used to recognise the GRADIENT_OFFSET_UNSPECIFIED sentinel.
 *
 * Since out of range positions became legal, a plain "offset < 0" test is no
 * longer able to tell "no position authored" from a real negative position
 * such as -20%. Every consumer must go through IsGradientOffsetUnspecified().
 */
constexpr float GRADIENT_OFFSET_SENTINEL_EPSILON = 1e-6f;

/**
 * @brief Largest magnitude accepted for an authored out of range position.
 *
 * CSS does not bound the percentage, but a ratio of +/-100 (i.e. +/-10000%)
 * already makes the visible segment a flat color, so anything beyond that is
 * clamped to keep the interpolation arithmetic well conditioned.
 */
constexpr float GRADIENT_OFFSET_EXTENT_LIMIT = 100.0f;

/**
 * @brief Whether @p offset is the "position omitted in CSS" sentinel.
 *
 * @param offset raw stop offset produced by the parser.
 * @return true when the position still has to be inferred by
 *         CssGradientParser::NormalizeColorStops().
 */
inline bool IsGradientOffsetUnspecified(float offset)
{
    return (offset > (GRADIENT_OFFSET_UNSPECIFIED - GRADIENT_OFFSET_SENTINEL_EPSILON)) &&
           (offset < (GRADIENT_OFFSET_UNSPECIFIED + GRADIENT_OFFSET_SENTINEL_EPSILON));
}

/** Maximum value of a single 8 bit color channel. */
constexpr uint8_t COLOR_CHANNEL_MAX = 0xFF;

/** Fully transparent ColorType payload. */
constexpr uint32_t COLOR_FULL_TRANSPARENT = 0u;

/**
 * @brief Maximum number of color stops of a linear gradient.
 *
 * Limited to 16 stops because:
 *   1. Memory protection on lite devices (GradientColorStop is 8 bytes, so 16
 *      stops cost 128 bytes).
 *   2. Real world CSS rarely declares more than 8 stops.
 *   3. It keeps a sane ratio with RADIAL_GRADIENT_COLOR_NUM.
 */
constexpr uint8_t LINEAR_GRADIENT_MAX_COLORS = 16;

/* ==========================================================================
 * Shared angle helpers
 * ========================================================================== */

/**
 * @brief Wrap an arbitrary CSS angle into the [0, 360) range;
 *
 * Also acts as the guard against non finite input: NaN is detected with the
 * self-comparison trick (portable across toolchains that build without the
 * full <cmath> NaN macros) and degrades to the CSS default angle.
 *
 * @param angleDeg raw angle in degrees, may be negative, huge or NaN.
 * @return an angle inside [0, 360).
 */
inline float NormalizeCssAngleDegrees(float angleDeg)
{
    /* NaN never equals itself; fall back to the CSS default direction. */
    if (angleDeg != angleDeg) {
        return CSS_ANGLE_DEFAULT;
    }
    float normalized = fmodf(angleDeg, CSS_ANGLE_FULL_TURN);
    if (normalized < 0.0f) {
        normalized += CSS_ANGLE_FULL_TURN;
    }
    return normalized;
}

/**
 * @brief Map a CSS direction keyword to its CSS angle in degrees.
 *
 * Single source of truth for the direction-keyword -> angle mapping, replacing
 * the switch statements that previously lived in both repositories.
 *
 * @param dir direction keyword.
 * @return CSS angle in degrees; CSS_ANGLE_DEFAULT for CUSTOM_ANGLE, NONE and
 *         any unknown value, because those cannot be expressed as a keyword.
 * @note CUSTOM_ANGLE and NONE are legitimate enum values and downgrade silently;
 *       only a corrupt/garbage enum (neither a keyword nor CUSTOM_ANGLE/NONE)
 *       hits the fallback branch and emits an error log.
 */
constexpr float CSS_DIRECTION_ANGLE_TABLE[] = {
    CSS_ANGLE_TO_TOP,          /* TO_TOP = 0 */
    CSS_ANGLE_TO_TOP_RIGHT,    /* TO_TOP_RIGHT = 1 */
    CSS_ANGLE_TO_RIGHT,        /* TO_RIGHT = 2 */
    CSS_ANGLE_TO_BOTTOM_RIGHT, /* TO_BOTTOM_RIGHT = 3 */
    CSS_ANGLE_TO_BOTTOM,       /* TO_BOTTOM = 4 */
    CSS_ANGLE_TO_BOTTOM_LEFT,  /* TO_BOTTOM_LEFT = 5 */
    CSS_ANGLE_TO_LEFT,         /* TO_LEFT = 6 */
    CSS_ANGLE_TO_TOP_LEFT,     /* TO_TOP_LEFT = 7 */
    CSS_ANGLE_DEFAULT,         /* CUSTOM_ANGLE = 8*/
};
constexpr std::size_t CSS_DIRECTION_ANGLE_TABLE_SIZE =
    sizeof(CSS_DIRECTION_ANGLE_TABLE) / sizeof(CSS_DIRECTION_ANGLE_TABLE[0]);

inline float CssGradientDirectionToAngle(CssGradientDirection dir)
{
    const uint8_t idx = static_cast<uint8_t>(dir);
    if (idx < CSS_DIRECTION_ANGLE_TABLE_SIZE) {
        return CSS_DIRECTION_ANGLE_TABLE[idx];
    }
    if (dir != CssGradientDirection::CUSTOM_ANGLE && dir != CssGradientDirection::NONE) {
        GRAPHIC_LOGE("CssGradientDirectionToAngle: unknown dir=%u (status=fallback, default=%.1f)",
                     static_cast<unsigned>(dir), CSS_ANGLE_DEFAULT);
    }
    return CSS_ANGLE_DEFAULT;
}

/* ==========================================================================
 * Color stop
 * ========================================================================== */

/**
 * @brief One gradient color stop.
 *
 * Fields:
 *   - color  : color value, reusing the ui_lite ColorType union so that both
 *              .full (uint32_t) and .red/.green/.blue/.alpha are available.
 *   - offset : normalized position on the gradient line, where 0.0 is the start
 *              point and 1.0 is the end point. While parsing, the value may sit
 *              outside [0.0, 1.0] because CSS allows positions such as -20% or
 *              150%; CssGradientParser::ClipColorStopsToGradientLine() projects
 *              the ramp back onto [0.0, 1.0] before the payload is published,
 *              so a GradientInfo handed to the drawing layer always carries
 *              offsets inside that range.
 *              GRADIENT_OFFSET_UNSPECIFIED marks a stop whose position must
 *              still be interpolated by NormalizeColorStops(); test it with
 *              IsGradientOffsetUnspecified(), never with "offset < 0".
 *
 * Example: linear-gradient(red 0%, green 50%, blue 100%)
 *   -> [{RED, 0.0}, {GREEN, 0.5}, {BLUE, 1.0}]
 */
struct GradientColorStop {
    ColorType color; /* Color value (ColorType union, identical to style.h) */
    float offset;    /* Normalized offset in [0.0, 1.0] */

    /** Default constructor: transparent black at offset 0. */
    GradientColorStop()
    {
        color.full = COLOR_FULL_TRANSPARENT;
        offset = GRADIENT_OFFSET_MIN;
    }

    /** Construct from an already built ColorType. */
    GradientColorStop(ColorType c, float o) : color(c), offset(o) {}

    /**
     * @brief Convenience constructor taking separate RGBA channels.
     * @param r red   [0, 255]
     * @param g green [0, 255]
     * @param b blue  [0, 255]
     * @param a alpha [0, 255] (255 means fully opaque)
     * @param o offset [0.0, 1.0]
     */
    GradientColorStop(uint8_t r, uint8_t g, uint8_t b, uint8_t a, float o)
    {
        color.red = r;
        color.green = g;
        color.blue = b;
        color.alpha = a;
        offset = o;
    }
};

/* ==========================================================================
 * Gradient payload
 * ========================================================================== */

/**
 * @brief Complete description of a linear gradient.
 *
 * This is the payload exchanged between repositories. Data flow:
 *   ace_engine_lite (CSS parsing)
 *     -> Component (ownership)
 *       -> UIView (owns its own copy, used while drawing)
 *         -> DrawCanvas (rasterization)
 *
 * Relationship with the legacy GradientColor union:
 *   - GradientColor (style.h) is the compact low level format whose linear part
 *     only supports two colors.
 *   - GradientInfo is the extended upper layer format supporting many stops.
 *   - When colorCount == 2 the payload maps directly onto
 *     GradientColor.colorBegin / .colorEnd, see LinearGradientBuilder::ConvertToLegacy().
 *   - When colorCount > 2 the adapter layer performs a multi stop fill.
 *
 * Memory ownership rules:
 *   1. colorStops is heap allocated by SetColorStops() and released by the
 *      destructor.
 *   2. Copy construction and copy assignment are deleted to avoid double free.
 *   3. Move construction and move assignment transfer ownership cheaply.
 *   4. Crossing a repository boundary must go through DeepCopy().
 *
 * Usage example:
 * @code
 *   GradientInfo info;
 *   info.direction = CssGradientDirection::TO_BOTTOM;
 *   GradientColorStop stops[2];
 *   stops[0] = GradientColorStop(0xe6, 0x64, 0x65, 0xff, 0.0f);
 *   stops[1] = GradientColorStop(0x91, 0x98, 0xe5, 0xff, 1.0f);
 *   info.SetColorStops(stops, 2);
 *   // info.isValid == true
 * @endcode
 */
struct GradientInfo {
    GradientType type;              /* Gradient family (currently LINEAR only) */
    CssGradientDirection direction; /* Direction keyword or CUSTOM_ANGLE */
    float angle;                    /* Explicit angle, only valid for CUSTOM_ANGLE */
    GradientColorStop* colorStops;  /* Heap allocated array of color stops */
    uint8_t colorCount;             /* Number of stops (2 = two color, >=3 = multi color) */
    bool isValid;                   /* Whether the payload can be drawn */

    /** Default constructor: an explicitly invalid payload. */
    GradientInfo()
        : type(GradientType::LINEAR),
          direction(CssGradientDirection::NONE),
          angle(CSS_ANGLE_TO_TOP),
          colorStops(nullptr),
          colorCount(0),
          isValid(false) {}

    /** Destructor: release the heap allocated stop array. */
    ~GradientInfo()
    {
        Clear();
    }

    /* Copying is forbidden, use DeepCopy() instead. */
    GradientInfo(const GradientInfo&) = delete;
    GradientInfo& operator=(const GradientInfo&) = delete;

    /** Move constructor: transfer ownership of the stop array. */
    GradientInfo(GradientInfo&& other) noexcept
        : type(other.type),
          direction(other.direction),
          angle(other.angle),
          colorStops(other.colorStops),
          colorCount(other.colorCount),
          isValid(other.isValid)
    {
        /* Detach the source so that its destructor does not free the array. */
        other.colorStops = nullptr;
        other.colorCount = 0;
        other.isValid = false;
    }

    /**
     * @brief Move assignment: release our own buffer, then steal the source one.
     *
     * Without this operator `a = std::move(b)` would fail to compile because the
     * copy assignment operator is deleted, which forced callers to use awkward
     * Clear() + SetColorStops() sequences.
     */
    GradientInfo& operator=(GradientInfo&& other) noexcept
    {
        if (this == &other) {
            return *this;
        }
        Clear();
        type = other.type;
        direction = other.direction;
        angle = other.angle;
        colorStops = other.colorStops;
        colorCount = other.colorCount;
        isValid = other.isValid;

        other.colorStops = nullptr;
        other.colorCount = 0;
        other.isValid = false;
        return *this;
    }

    /**
     * @brief Release every owned resource and reset to the invalid state.
     *
     * Called by the destructor, before replacing the color stops, and whenever
     * the gradient effect is removed from a view.
     */
    void Clear()
    {
        if (colorStops != nullptr) {
            delete[] colorStops;
            colorStops = nullptr;
        }
        colorCount = 0;
        isValid = false;
        direction = CssGradientDirection::NONE;
    }

    /**
     * @brief Replace the color stops with a deep copy of the input array.
     *
     * @param stopsArray source stops, may live on the stack or in a container.
     * @param count      number of entries in @p stopsArray.
     *
     * Steps:
     *   1. Remember the direction and angle, because Clear() resets them.
     *   2. Drop the previous stops.
     *   3. Validate the input; a gradient needs at least two stops.
     *   4. Clamp the count to LINEAR_GRADIENT_MAX_COLORS.
     *   5. Allocate and deep copy, then restore the direction and angle.
     *
     * Precondition: offsets are expected to be already normalized (monotonically
     * increasing inside [0, 1]); normalization is the parser's responsibility,
     * see CssGradientParser::NormalizeColorStops().
     */
    void SetColorStops(const GradientColorStop* stopsArray, uint8_t count)
    {
        /*
         * Clear() resets direction to NONE, which used to silently discard a
         * previously parsed custom angle. Snapshot both fields and restore them
         * once the new stops are in place.
         */
        const CssGradientDirection savedDirection = direction;
        const float savedAngle = angle;

        Clear();

        /* A gradient is undefined with fewer than two stops. */
        if (stopsArray == nullptr || count < GRADIENT_MIN_COLOR_STOP_COUNT) {
            direction = savedDirection;
            angle = savedAngle;
            return;
        }

        /* Lite device protection: never allocate more than the hard limit. */
        if (count > LINEAR_GRADIENT_MAX_COLORS) {
            count = LINEAR_GRADIENT_MAX_COLORS;
        }

        colorStops = new (std::nothrow) GradientColorStop[count];
        if (colorStops == nullptr) {
            /* Out of memory: stay invalid but keep the direction for diagnostics. */
            GRAPHIC_LOGE("GradientInfo::SetColorStops: alloc failed for count=%u "
                         "(status=invalid, bytes=%u, direction=%u)",
                         static_cast<unsigned>(count),
                         static_cast<unsigned>(count * sizeof(GradientColorStop)),
                         static_cast<unsigned>(savedDirection));
            isValid = false;
            direction = savedDirection;
            angle = savedAngle;
            return;
        }

        /* GradientColorStop is a POD-like aggregate, plain assignment is enough. */
        for (uint8_t i = 0; i < count; i++) {
            colorStops[i] = stopsArray[i];
        }
        colorCount = count;
        isValid = true;

        direction = savedDirection;
        angle = savedAngle;
    }

    /**
     * @brief Duplicate this gradient into a freshly allocated object.
     *
     * @return a new GradientInfo owned by the caller, or nullptr when the source
     *         is invalid or the allocation failed.
     *
     * Used when Component hands the gradient over to UIView so that both sides
     * own an independent copy with independent lifetimes.
     */
    GradientInfo* DeepCopy() const
    {
        /* Refuse to clone a payload that cannot be drawn anyway. */
        if (!isValid || colorStops == nullptr || colorCount < GRADIENT_MIN_COLOR_STOP_COUNT) {
            GRAPHIC_LOGE("GradientInfo::DeepCopy: source unusable isValid=%d stops=%s count=%u "
                         "(status=null returned, required>=%u, direction=%u)",
                         static_cast<int>(isValid), (colorStops == nullptr) ? "null" : "valid",
                         static_cast<unsigned>(colorCount),
                         static_cast<unsigned>(GRADIENT_MIN_COLOR_STOP_COUNT),
                         static_cast<unsigned>(direction));
            return nullptr;
        }

        GradientInfo* copy = new (std::nothrow) GradientInfo();
        if (copy == nullptr) {
            GRAPHIC_LOGE("GradientInfo::DeepCopy: alloc failed for the payload "
                         "(status=null returned, count=%u, direction=%u)",
                         static_cast<unsigned>(colorCount), static_cast<unsigned>(direction));
            return nullptr;
        }

        copy->type = type;
        copy->direction = direction;
        copy->angle = angle;

        /* SetColorStops performs the deep copy of the stop array. */
        copy->SetColorStops(colorStops, colorCount);

        /*
         * SetColorStops can fail on allocation. Detect it here so that an
         * unusable object is never handed to the drawing layer.
         */
        if (!copy->isValid || copy->colorStops == nullptr) {
            GRAPHIC_LOGE("GradientInfo::DeepCopy: stop array copy failed "
                         "(status=rolled back, count=%u, direction=%u)",
                         static_cast<unsigned>(colorCount), static_cast<unsigned>(direction));
            delete copy;
            return nullptr;
        }

        return copy;
    }

    /**
     * @brief Effective CSS angle of this gradient, already normalized.
     *
     * Resolution order:
     *   - CUSTOM_ANGLE: use `angle`, wrapped into [0, 360) and NaN guarded.
     *   - any keyword : use the shared keyword table.
     *
     * Centralizing this logic keeps the ui_lite adapter and any future consumer
     * from re-implementing the same branch.
     */
    float ResolveCssAngle() const
    {
        if (direction == CssGradientDirection::CUSTOM_ANGLE) {
            return NormalizeCssAngleDegrees(angle);
        }
        return CssGradientDirectionToAngle(direction);
    }

    /**
     * @brief First stop color, compatible with GradientColor.colorBegin.
     * @return the first stop color, or transparent black when unavailable.
     */
    ColorType GetBeginColor() const
    {
        ColorType result;
        result.full = COLOR_FULL_TRANSPARENT;
        if (colorStops != nullptr && colorCount > 0) {
            result = colorStops[0].color;
        }
        return result;
    }

    /**
     * @brief Last stop color, compatible with GradientColor.colorEnd.
     * @return the last stop color, or transparent black when unavailable.
     */
    ColorType GetEndColor() const
    {
        ColorType result;
        result.full = COLOR_FULL_TRANSPARENT;
        if (colorStops != nullptr && colorCount > 0) {
            result = colorStops[colorCount - 1].color;
        }
        return result;
    }

    /**
     * @brief Direction value compatible with GradientColor.direction.
     *
     * @return the raw enum value; CUSTOM_ANGLE degrades to TO_BOTTOM because the
     *         compact union cannot express an arbitrary angle.
     */
    uint8_t GetDirectionValue() const
    {
        if (direction == CssGradientDirection::CUSTOM_ANGLE) {
            return static_cast<uint8_t>(CssGradientDirection::TO_BOTTOM);
        }
        return static_cast<uint8_t>(direction);
    }
};

} // namespace OHOS

#endif // GRAPHIC_UI_GRADIENT_INFO_H
