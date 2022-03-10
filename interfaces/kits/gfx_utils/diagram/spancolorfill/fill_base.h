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
 * @file fill_base.h
 * @brief Defines Scan line distributor and converter
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_FILL_BASE_H
#define GRAPHIC_LITE_FILL_BASE_H

#include "gfx_utils/diagram/vertexprimitive/geometry_plaindata_array.h"

namespace OHOS {
template <class ColorT>
class FillBase {
public:
    /**
     * @brief Resize Redistribution calculation spans_ length
     * @param span_len Scan line length
     * @return First address of spans
     */
    inline ColorT* Resize(uint32_t span_len)
    {
        if (span_len > spans_.GetSize()) {
            spans_.Resize(((span_len + MAX_COLOR_NUM) >> BYTE_LENGTH) << BYTE_LENGTH);
        }
        return &spans_[0];
    }

private:
    GeometryPlainDataArray<ColorT> spans_;
};
} // namespace OHOS
#endif
