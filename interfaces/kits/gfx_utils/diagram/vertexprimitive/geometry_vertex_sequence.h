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
 * @addtogroup GraphicGeometry
 * @{
 *
 * @brief Defines GeometryVertexSequence VertexDist VertexDistCmd.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LTE_GEOMETRY_VERTEX_SEQUENCE_H
#define GRAPHIC_LTE_GEOMETRY_VERTEX_SEQUENCE_H

#include "gfx_utils/diagram/common/common_basics.h"
#include "gfx_utils/diagram/common/common_math.h"
#include "geometry_plaindata_blockvector.h"
namespace OHOS {
/**
 * @file geometry_vertex_sequence.h
 *
 * @brief Defines Define the GeometryVertexSequence class.
 *
 * @since 1.0
 * @version 1.0
 */
template <class T, uint32_t S = BLOCK_SHIFT_SIZE>
class GeometryVertexSequence : public GeometryPlainDataBlockVector<T, S> {
public:
    using BaseType = GeometryPlainDataBlockVector<T, S>;
    /**
     * @brief Closed vertex source.
     *
     * @param removeFlag Is it closed.
     * @since 1.0
     * @version 1.0
     */
    void Close(bool removeFlag);

    /**
     * @brief Add a point.
     *
     * @param val vertex.
     * @since 1.0
     * @version 1.0
     */
    void Add(const T& val);

    /**
     * @brief Modify the last vertex.
     *
     * @param t vertex.
     * @since 1.0
     * @version 1.0
     */
    void ModifyLast(const T& val);
};

template <class T, uint32_t S>
void GeometryVertexSequence<T, S>::Close(bool closed)
{
    while (BaseType::GetSize() > 1) {
        if ((*this)[BaseType::GetSize() - TWO_STEP]((*this)[BaseType::GetSize() - 1])) {
            break;
        }
        T t = (*this)[BaseType::GetSize() - 1];
        BaseType::RemoveLast();
        ModifyLast(t);
    }

    if (closed) {
        while (BaseType::GetSize() > 1) {
            if ((*this)[BaseType::GetSize() - 1]((*this)[0])) { // Calculate the distance between two vertices
                break;
            }
            BaseType::RemoveLast();
        }
    }
}

template <class T, uint32_t S>
void GeometryVertexSequence<T, S>::Add(const T& val)
{
    if (BaseType::GetSize() > 1) {
        if (!(*this)[BaseType::GetSize() - TWO_STEP]((*this)[BaseType::GetSize() - 1])) {
            BaseType::RemoveLast();
        }
    }
    BaseType::Add(val);
}

template <class T, uint32_t S>
void GeometryVertexSequence<T, S>::ModifyLast(const T& val)
{
    BaseType::RemoveLast();
    Add(val);
}

struct VertexDist {
    float vertexXCoord;
    float vertexYCoord;
    float vertexDistance;

    VertexDist() {}
    /**
     * @brief Construct vertexdist.
     *
     * @param x_,y_ Vertex coordinates.
     * @since 1.0
     * @version 1.0
     */
    VertexDist(float x, float y)
        : vertexXCoord(x), vertexYCoord(y), vertexDistance(0.0f) {}
    /**
     * @brief Calculate whether the distance between the two points is very close.
     *
     * @param vertex.
     * @return If the two points are close, false is returned.
     * @since 1.0
     * @version 1.0
     */
    bool operator()(const VertexDist& val)
    {
        vertexDistance = CalcDistance(vertexXCoord, vertexYCoord, val.vertexXCoord, val.vertexYCoord);
        bool ret = vertexDistance > VERTEX_DIST_EPSILON;
        if (!ret) {
            vertexDistance = 1.0f / VERTEX_DIST_EPSILON;
        }
        return ret;
    }
};
} // namespace OHOS
#endif
