/*
 * Copyright (c) 2022 CompanyNameMagicTag.
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
#ifdef IMG_CACHE_MEMORY_CUSTOM
#include "hals/gralloc_engines.h"
#include "gfx_utils/graphic_log.h"
#if ENABLE_MEMORY_CHECK
#include "gfx_utils/mem_check.h"
#endif
#include "engines/gfx/lite_m_gfx_engine.h"

namespace OHOS {
void* ImageCacheMalloc(ImageInfo& info)
{
    uint8_t* addr = GrallocEngines::GetInstance()->AllocBufferBySize(info.dataSize);
    if (addr == nullptr) {
        GRAPHIC_LOGE("ImageCacheMalloc failed! size = %u", info.dataSize);
        return nullptr;
    }
    info.phyAddr = addr;
    info.data = addr;
    info.fileLen = 0;
    info.resId = 0;
    return reinterpret_cast<void*>(addr);
}

void ImageCacheFree(ImageInfo& info, bool syncHwDraw)
{
    if (info.data == nullptr) {
        return;
    }

    if (syncHwDraw) {
        LiteMGfxEngine::GetInstance()->SyncHwDraw();
    }
    GrallocEngines::GetInstance()->FreeBuffer(const_cast<uint8_t*>(info.data));
    info.phyAddr = nullptr;
    info.data = nullptr;
    return;
}

void* UIMalloc(uint32_t size)
{
#if ENABLE_MEMORY_CHECK
    void* buf = malloc(size);
    if (buf != nullptr) {
        MemCheck::GetInstance()->MemAdd(size, (uintptr_t)buf);
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
    MemCheck::GetInstance()->MemDelete((uintptr_t)buffer);
#endif
    free(buffer);
    buffer = nullptr;
}

void* UIRealloc(void* buffer, uint32_t size)
{
#if ENABLE_MEMORY_CHECK
    MemCheck::GetInstance()->MemDelete((uintptr_t)buffer);
    void* buf = realloc(buffer, size);
    MemCheck::GetInstance()->MemAdd(size, (uintptr_t)buf);
    return buf;
#else
    return realloc(buffer, size);
#endif
}
}
#endif