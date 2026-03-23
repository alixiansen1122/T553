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
#if ENABLE_MEMORY_CHECK
#include "gfx_utils/mem_check.h"
#endif

namespace OHOS {
#ifndef IMG_CACHE_MEMORY_CUSTOM
void* ImageCacheMalloc(ImageInfo& info)
{
#if ENABLE_MEMORY_CHECK
    void* buf = malloc(info.dataSize);
    if (buf != NULL) {
        MemCheck::GetInstance()->MemAdd(info.dataSize, (long)buf);
    }
    return buf;
#else
    return malloc(info.dataSize);
#endif
}

void ImageCacheFree(ImageInfo& info, bool syncHwDraw)
{
    uint8_t* buf = const_cast<uint8_t*>(info.data);
#if ENABLE_MEMORY_CHECK
    MemCheck::GetInstance()->MemDelete((long)buf);
#endif
    free(buf);
    info.data = nullptr;
    return;
}

#if !ENABLE_MEMORY_HOOKS
void* UIMalloc(uint32_t size)
{
#if ENABLE_MEMORY_CHECK
    void* buf = malloc(size);
    if (buf != NULL) {
        MemCheck::GetInstance()->MemAdd(size, (long)buf);
    }
    return buf;
#else
    return malloc(size);
#endif
}

void UIFree(void* buffer)
{
    if (buffer == nullptr) {
        return;
    }
#if ENABLE_MEMORY_CHECK
    MemCheck::GetInstance()->MemDelete((long)buffer);
#endif
    free(buffer);
    buffer = NULL;
}

void* UIRealloc(void* buffer, uint32_t size)
{
    return realloc(buffer, size);
}
#endif
#endif
}
