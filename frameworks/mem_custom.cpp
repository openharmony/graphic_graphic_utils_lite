/*
 * Copyright (c) 2026 HiSilicon (Shanghai) Technologies Co., Ltd.
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

#include "gfx_utils/mem_api.h"
#include "gfx_utils/common_macros.h"
#ifdef IMG_CACHE_MEMORY_CUSTOM
#include "hals/gralloc_engines.h"
#include "gfx_utils/graphic_log.h"

namespace OHOS {
UI_WEAK_SYMBOL void* ImageCacheMalloc(const ImageInfo& info)
{
    AllocInfo allocInfo;
    allocInfo.expectedSize = info.dataSize;
    allocInfo.usage = HBM_USE_ASSIGN_SIZE | HBM_USE_MEM_MMZ;
    GrallocBuffer buffer;
    if (!GrallocEngines::GetInstance()->AllocBuffer(allocInfo, buffer)) {
        GRAPHIC_LOGE("AllocBuffer failed.");
        return nullptr;
    }
    uint8_t** buf = const_cast<uint8_t**>(&info.phyAddr);
    *buf = reinterpret_cast<uint8_t*>(buffer.phyAddr);
    buf = const_cast<uint8_t**>(&info.data);
    *buf = reinterpret_cast<uint8_t*>(buffer.virAddr);
    return buffer.virAddr;
}

UI_WEAK_SYMBOL void ImageCacheFree(ImageInfo& info)
{
    if (info.data == nullptr) {
        return;
    }
    GrallocEngines::GetInstance()->FreeBuffer(info.data);
    info.phyAddr = nullptr;
    info.data = nullptr;
    return;
}
}
#endif