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
 * @file graphic_buffer.h
 *
 * @brief Defines the LCD buffer info for the graphics system.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_BUFFER_H
#define GRAPHIC_LITE_BUFFER_H

#include "gfx_utils/graphic_types.h"
#include "gfx_utils/rect.h"

namespace OHOS {
#define _ALIGN_UP(sz, align) (((sz) + (align - 1)) & (-(align)))
#define _ALIGN_DOWN(sz, align) ((sz) & (-(align)))
#define _ADDR_ALIGN(ptr, sz, align)                                  \
    do {                                                             \
        uint32_t _sz = _ALIGN_DOWN(sz, align);                       \
        (ptr) = decltype(ptr)((uintptr_t)(void*)(ptr) + ((sz)-_sz)); \
        (sz) = _sz;                                                  \
    } while (0);
#define _STRUCT_ALIGN(sz) alignas(sz)

#ifdef ALIGNMENT_BYTES
#if (ALIGNMENT_BYTES != 0) && ((ALIGNMENT_BYTES & (ALIGNMENT_BYTES - 1)) != 0)
#error ALIGNMENT_BYTES should be power of 2.
#endif
#define UI_ALIGN_UP(size) _ALIGN_UP((size), ALIGNMENT_BYTES)
#define UI_ALIGN_DOWN(size) _ALIGN_DOWN((size), ALIGNMENT_BYTES)
#define UI_ADDR_ALIGN(ptr, sz) _ADDR_ALIGN(ptr, sz, ALIGNMENT_BYTES)
#define UI_STRUCT_ALIGN _STRUCT_ALIGN(ALIGNMENT_BYTES)
#else
#define UI_ALIGN_UP(size) (size)
#define UI_ALIGN_DOWN(size) (size)
#define UI_ADDR_ALIGN(ptr, sz)
#define UI_STRUCT_ALIGN
#endif

/**
 * @brief buffer info for drawing.
 * @since 6.0
 * @version 6.0
 */
struct BufferInfo {
    Rect rect;
    int32_t stride;
    void* phyAddr;
    void* virAddr;
    uint16_t width;
    uint16_t height;
    ColorMode mode;
    uint32_t color;
};
} // namespace OHOS
#endif // GRAPHIC_LITE_TYPES_H
