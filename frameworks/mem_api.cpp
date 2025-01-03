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

#include "gfx_utils/mem_api.h"
#include "gfx_utils/common_macros.h"

namespace OHOS {
UI_WEAK_SYMBOL void* ImageCacheMalloc(const ImageInfo& info)
{
    return malloc(info.dataSize);
}

UI_WEAK_SYMBOL void ImageCacheFree(ImageInfo& info)
{
    uint8_t* buf = const_cast<uint8_t*>(info.data);
    free(buf);
    info.data = nullptr;
    return;
}

UI_WEAK_SYMBOL void* UIMalloc(uint32_t size)
{
    return malloc(size);
}

UI_WEAK_SYMBOL void UIFree(void* buffer)
{
    free(buffer);
}

UI_WEAK_SYMBOL void* UIRealloc(void* buffer, uint32_t size)
{
    return realloc(buffer, size);
}
} // namespace OHOS
