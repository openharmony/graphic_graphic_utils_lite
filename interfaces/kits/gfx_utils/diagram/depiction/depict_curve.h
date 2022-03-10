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

/**
 * @file depict_curve.h
 *
 * @brief Defines Building curve transformation pipes
 * The curve drawing is usually approximated by a series of short line segments,
 * which is the only efficient method to draw the curve
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_DEPICT_CURVE_H
#define GRAPHIC_LITE_DEPICT_CURVE_H

#include "gfx_utils/diagram/common/common_basics.h"
#include "gfx_utils/diagram/vertexprimitive/geometry_curves.h"

namespace OHOS {
/**
 * @template<VertexSource,QuadraticBezierCurve,CubicBezierCurve> class DepictCurve
 * @brief By PATH_CMD_CURVE3 and PATH_CMD_CURVE4
 * The command calculates the generated curve points and
 * saves the generated points to the curve using the conversion pipe
 * move_to/line_to vertex sequence
 * @since 1.0
 * @version 1.0
 */
template <class VertexSource,
          class QuadraticBezierCurve = QuadraticBezierCurve,
          class CubicBezierCurve = CubicBezierCurve>
class DepictCurve {
public:
    using QuadraticBezierCurveType = QuadraticBezierCurve;
    using CubicBezierCurveType = CubicBezierCurve;
    using SelfType = DepictCurve<VertexSource, QuadraticBezierCurve, CubicBezierCurve>;

    /**
     * @brief DepictCurve Class constructor
     * The construction parameter is the vertexsource attribute, which determines the vertex source of the curve.
     * @since 1.0
     * @version 1.0
     */
    explicit DepictCurve(VertexSource& source)
        : source_(&source), lastX_(0), lastY_(0) {}

    void Attach(VertexSource& source)
    {
        source_ = &source;
    }

    /**
     * @brief Method for approximate estimation of curve
     * There are two methods for defining and estimating quadratic or cubic (Bessel) curves.
     * One is curve_ Inc, which is estimated by increasing the curve
     * The second is curve_ Div is by equally dividing the curve into n div segments
     * @since 1.0
     * @version 1.0
     */
    void ApproximationMethod(CurveApproximationMethod curvApproxiMethod)
    {
        quadraticBezier_.ApproximationMethod(curvApproxiMethod);
        cubicBezier_.ApproximationMethod(curvApproxiMethod);
    }

    CurveApproximationMethod ApproximationMethod() const
    {
        return cubicBezier_.ApproximationMethod();
    }

    /**
     * @brief Finally determine the accuracy of the estimate
     * In practical application, we need to convert the world coordinates of points to screen coordinates,
     * so there will always be a certain scaling factor.
     * Curves are usually processed in the world coordinate system and converted to pixel values when estimating.
     * It usually looks like this: m_curved.approximation_scale(m_transform.scale());
     * Here, m_transform is a matrix of affine mapping,
     * which contains all transformations,
     * including viewpoint and scaling.
     * @since 1.0
     * @version 1.0
     */
    void ApproximationScale(float aScale)
    {
        quadraticBezier_.SetApproximationScale(aScale);
        cubicBezier_.SetApproximationScale(aScale);
    }

    float ApproximationScale() const
    {
        return cubicBezier_.ApproximationScale();
    }

    /**
     * @brief Sets the angle estimate in radians. The less this value,
     * the more accurate the estimation at the turn of the curve.
     * However, if set to 0, it means that the angle condition is not considered at all.
     * @since 1.0
     * @version 1.0
     */
    void AngleTolerance(float angleRate)
    {
        quadraticBezier_.AngleTolerance(angleRate);
        cubicBezier_.AngleTolerance(angleRate);
    }

    float AngleTolerance() const
    {
        return cubicBezier_.AngleTolerance();
    }

    /**
     * @brief An angle set in radians. If 0,
     * Then only the real cusp will have a bevel cut. If greater than 0,
     * Then it will limit the curvature of the curve. The higher the value,
     * the less sharp corners of the curve will be cut.
     * Generally, this value should not be greater than 10-15 degrees.
     * @since 1.0
     * @version 1.0
     */
    void CuspLimit(float v)
    {
        quadraticBezier_.CuspLimit(v);
        cubicBezier_.CuspLimit(v);
    }

    float CuspLimit() const
    {
        return cubicBezier_.CuspLimit();
    }
    /**
     * Reset the status attribute of a path segment
     * @path_id Is a path ID, calculated from 0
     * @since 1.0
     * @version 1.0
     */
    void Rewind(uint32_t pathId);

    /**
     * According to PATH_CMD command returns the vertex coordinates generated in each stage
     * @since 1.0
     * @version 1.0
     */
    uint32_t GenerateVertex(float* x, float* y);

private:
    DepictCurve(const SelfType&);
    const SelfType& operator=(const SelfType&);

    VertexSource* source_;
    float lastX_;
    float lastY_;
    QuadraticBezierCurveType quadraticBezier_;
    CubicBezierCurveType cubicBezier_;
};

/**
 * Reset the status attribute of a path segment
 * @path_id is a path ID, calculated from 0
 * @since 1.0
 * @version 1.0
 */
template <class VertexSource, class QuadraticBezierCurve, class CubicBezierCurve>
void DepictCurve<VertexSource, QuadraticBezierCurve, CubicBezierCurve>::Rewind(uint32_t pathId)
{
    source_->Rewind(pathId);
    lastX_ = 0.0;
    lastY_ = 0.0;
    quadraticBezier_.Reset();
    cubicBezier_.Reset();
}

/**
 * According to PATH_CMD command returns the vertex coordinates generated in each stage
 * @since 1.0
 * @version 1.0
 */
template <class VertexSource, class QuadraticBezierCurve, class CubicBezierCurve>
uint32_t DepictCurve<VertexSource, QuadraticBezierCurve, CubicBezierCurve>::GenerateVertex(float* x, float* y)
{
    if (!IsStop(quadraticBezier_.GenerateVertex(x, y))) {
        lastX_ = *x;
        lastY_ = *y;
        return PATH_CMD_LINE_TO;
    }

    if (!IsStop(cubicBezier_.GenerateVertex(x, y))) {
        lastX_ = *x;
        lastY_ = *y;
        return PATH_CMD_LINE_TO;
    }

    float control2X = 0;
    float control2Y = 0;
    float endX = 0;
    float endY = 0;

    uint32_t cmd = source_->GenerateVertex(x, y);
    switch (cmd) {
        case PATH_CMD_CURVE3:
            source_->GenerateVertex(&endX, &endY);

            quadraticBezier_.Init(lastX_, lastY_, *x, *y, endX, endY);

            quadraticBezier_.GenerateVertex(x, y); // First call returns path_cmd_move_to
            quadraticBezier_.GenerateVertex(x, y); // This is the first vertex of the curve
            cmd = PATH_CMD_LINE_TO;
            break;

        case PATH_CMD_CURVE4:
            source_->GenerateVertex(&control2X, &control2Y);
            source_->GenerateVertex(&endX, &endY);

            cubicBezier_.Init(lastX_, lastY_, *x, *y, control2X, control2Y, endX, endY);

            cubicBezier_.GenerateVertex(x, y); // First call returns path_cmd_move_to
            cubicBezier_.GenerateVertex(x, y); // This is the first vertex of the curve
            cmd = PATH_CMD_LINE_TO;
            break;
    }
    lastX_ = *x;
    lastY_ = *y;
    return cmd;
}
} // namespace OHOS
#endif
