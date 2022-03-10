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
 * @brief Defines function ShortenPath.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LTE_GEOMETRY_SHORTEN_PATH_H
#define GRAPHIC_LTE_GEOMETRY_SHORTEN_PATH_H

#include "gfx_utils/diagram/common/common_basics.h"
#include "gfx_utils/diagram/vertexprimitive/geometry_vertex_sequence.h"

namespace OHOS {
/**
 * @brief Long line is shortened to broken line (long line becomes short line, used in dash).
 *
 * @param vtxSeq data source,distence distance,closed is the path closed.
 * @since 1.0
 * @version 1.0
 */
template <class GraphicGeometryVertexSequence>
void ShortenPath(GraphicGeometryVertexSequence& vtxSeq, float distence, uint32_t closed = 0)
{
    using VertexType = typename GraphicGeometryVertexSequence::ValueType;

    if (vtxSeq.GetSize() > 1 && distence > 0.0f) {
        float vtxSeqDistance;
        int32_t nSize = int32_t(vtxSeq.GetSize() - TWO_STEP);
        while (nSize) {
            vtxSeqDistance = vtxSeq[nSize].vertexDistance;
            if (distence < vtxSeqDistance) {
                break;
            }
            vtxSeq.RemoveLast();
            distence = distence - vtxSeqDistance;
            --nSize;
        }
        if (vtxSeq.GetSize() > 1) {
            nSize = vtxSeq.GetSize() - 1;
            VertexType& prev = vtxSeq[nSize - 1];
            VertexType& last = vtxSeq[nSize];
            vtxSeqDistance = (prev.vertexDistance - distence) / prev.vertexDistance;
            float x = prev.vertexXCoord + (last.vertexXCoord - prev.vertexXCoord) * vtxSeqDistance;
            float y = prev.vertexYCoord + (last.vertexYCoord - prev.vertexYCoord) * vtxSeqDistance;
            last.vertexXCoord = x;
            last.vertexYCoord = y;
            if (!prev(last)) {       // Calculate whether the two vertices are close
                vtxSeq.RemoveLast(); // Delete points not close
            }
            vtxSeq.Close(closed != 0);
        } else {
            vtxSeq.RemoveAll();
        }
    }
}
} // namespace OHOS
#endif
