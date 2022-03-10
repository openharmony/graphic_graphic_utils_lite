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

#ifndef GRAPHIC_LITE_DEPICT_STROKE_H
#define GRAPHIC_LITE_DEPICT_STROKE_H

#include "depict_adaptor_vertex_generate.h"
#include "gfx_utils/diagram/common/common_basics.h"
#include "gfx_utils/diagram/vertexgenerate/vertex_generate_stroke.h"

namespace OHOS {
/**
 * @template<class VertexSource,class Markers> struct DepictStroke
 * @brief The structure is mainly a template structure for contour line transformation,
 * including dotted line transformation
 * @since 1.0
 * @version 1.0
 */
template <class VertexSource, class Markers = EmptyMarkers>
struct DepictStroke
    : public DepictAdaptorVertexGenerate<VertexSource, VertexGenerateStroke, Markers> {
    using BaseType = DepictAdaptorVertexGenerate<VertexSource, VertexGenerateStroke, Markers>;

    /**
     * @brief DepictStroke Class constructor
     * The construction parameter is the VertexSource property,
     * which determines the processing of expanding or shrinking contour lines.
     * @since 1.0
     * @version 1.0
     */
    DepictStroke(VertexSource& vs)
        : DepictAdaptorVertexGenerate<VertexSource, VertexGenerateStroke, Markers>(vs)
    {
    }

    /** Contour line mainly sets the lineweight of geometric lines */
    void SetWidth(float width)
    {
        BaseType::GetGenerator().SetWidth(width);
    }

    /**
     * @brief Finally determine the accuracy of the estimation.
     * In practical application, we need to convert the world coordinates of points to screen coordinates,
     * so there will always be a certain scaling factor.
     * Curves are usually processed in the world coordinate system and converted to pixel values when estimating.
     * It usually looks like this: m_curved.approximation_scale(m_transform.scale());
     * Here, m_transformis a matrix of affine mapping,
     * which contains all transformations,
     * including viewpoint and scaling.
     * @since 1.0
     * @version 1.0
     */
    void ApproximationScale(float aScale)
    {
        BaseType::GetGenerator().ApproximationScale(aScale);
    }
    // Contour lines mainly return the lineweight of geometric lines
    float Width() const
    {
        return BaseType::GetGenerator().Width();
    }

    /**
     * @brief Returns the precision of the final decision estimate
     * In practical application, we need to convert the world coordinates of points to screen coordinates,
     * so there will always be a certain scaling factor.
     * Curves are usually processed in the world coordinate system and converted to pixel values when estimating.
     * It usually looks like this: m_curved.approximation_scale(m_transform.scale());
     * Here, m_transformis a matrix of affine mapping,
     * which contains all transformations,
     * including viewpoint and scaling.
     * @since 1.0
     * @version 1.0
     */
    float ApproximationScale() const
    {
        return BaseType::GetGenerator().ApproximationScale();
    }

    void SetStrokeShorten(float strokeShorten)
    {
        BaseType::GetGenerator().SetStrokeShorten(strokeShorten);
    }
    float GetStrokeShorten() const
    {
        return BaseType::GetGenerator().GetStrokeShorten();
    }

private:
    DepictStroke(const DepictStroke<VertexSource, Markers>&);
    const DepictStroke<VertexSource, Markers>& operator= (const DepictStroke<VertexSource, Markers>&);
};
} // namespace OHOS
#endif
