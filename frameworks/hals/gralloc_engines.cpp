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

#include "hals/gralloc_engines.h"
#include <map>
#include "display_type.h"
#include "gfx_utils/graphic_log.h"
#include "securec.h"
namespace OHOS {
static std::map<uint64_t, BufferHandle*> buffers;

GrallocEngines* GrallocEngines::GetInstance()
{
    static GrallocEngines instance;
    return &instance;
}

bool GrallocEngines::Init()
{
    if (grallocFuncs_ != nullptr) {
        GRAPHIC_LOGI("GrallocEngines has init succeed.");
        return true;
    }
#ifdef ENABLE_GFX_ENGINES
    if (GrallocInitialize(&grallocFuncs_) != DISPLAY_SUCCESS) {
        return false;
    }
#endif
    return true;
}

bool GrallocEngines::AllocBuffer(const AllocInfo& info, GrallocBuffer& buffer)
{
    if (grallocFuncs_ == nullptr) {
        GRAPHIC_LOGE("GrallocEngines not init!");
        return false;
    }
    BufferHandle* bufferHandle = nullptr;
    if (grallocFuncs_->AllocMem(&info, &bufferHandle) != DISPLAY_SUCCESS) {
        return false;
    }
    buffers.insert(std::make_pair(reinterpret_cast<uint64_t>(bufferHandle->virAddr), bufferHandle));
    buffer.phyAddr = bufferHandle->phyAddr;
    buffer.virAddr = bufferHandle->virAddr;
    return true;
}

void GrallocEngines::FreeBuffer(const uint8_t* virAddr)
{
    if (grallocFuncs_ == nullptr) {
        GRAPHIC_LOGE("GrallocEngines not init!");
        return;
    }

    if (virAddr == nullptr) {
        return;
    }

    uint64_t addr = reinterpret_cast<uint64_t>(virAddr);
    auto iter = buffers.find(addr);
    if (iter != buffers.end()) {
        BufferHandle* handle = iter->second;
        if (handle != nullptr) {
            buffers.erase(iter);
            grallocFuncs_->FreeMem(handle);
        }
    }
}
} // namespace OHOS