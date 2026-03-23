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

#include <unordered_map>
#ifdef __LITEOS__
#include "los_memory_pri.h"
#endif
#include "display_type.h"
#include "gfx_utils/graphic_log.h"
#include "securec.h"
#include "common/image_cache_manager.h"
#include "font/ui_font.h"
#include "hals/gralloc_engines.h"

namespace OHOS {
static std::unordered_map<uintptr_t, BufferHandle*> g_buffers;

GrallocEngines* GrallocEngines::GetInstance()
{
    static GrallocEngines instance;
    return &instance;
}

static bool TryToFreeImage()
{
    return ImageCacheManager::GetInstance().TryToFreeImage();
}

static bool TryToFreeFontCache()
{
    return UIFont::GetInstance()->TryToFreeFontCache();
}

bool GrallocEngines::Init()
{
    if (grallocFuncs_ != nullptr) {
        GRAPHIC_LOGI("GrallocEngines has init succeed.");
        return true;
    }
    if (GrallocInitialize(&grallocFuncs_) != DISPLAY_SUCCESS) {
        return false;
    }
    grallocFuncs_->RegisterTryToFreeImgCallback(TryToFreeImage);
    grallocFuncs_->RegisterTryToFreeFontCacheCallback(TryToFreeFontCache);
    return true;
}

bool GrallocEngines::AllocBuffer(const AllocInfo& info, GrallocBuffer& buffer)
{
    if (grallocFuncs_ == nullptr) {
        GRAPHIC_LOGE("GrallocEngines not init!");
        return false;
    }
    BufferHandle* bufferHandle = nullptr;
    if (grallocFuncs_->AllocMem(&info, &bufferHandle) != DISPLAY_SUCCESS || bufferHandle == nullptr) {
        GRAPHIC_LOGE("GrallocEngines AllocMem size:%d usage:%x failed.", info.expectedSize, info.usage);
#if ENABLE_MEMORY_CHECK
        DumpMem();
#endif
#ifdef __LITEOS__
        OsMemInfoPrint(reinterpret_cast<void*>(MEM_POOL_UI_OTHER));
        OsMemInfoPrint(reinterpret_cast<void*>(MEM_POOL_UI_FONT));
#endif
        return false;
    }
    g_buffers.insert(std::make_pair(reinterpret_cast<uint64_t>(bufferHandle->virAddr), bufferHandle));
    buffer.phyAddr = bufferHandle->phyAddr;
    buffer.virAddr = bufferHandle->virAddr;
#if ENABLE_GMMU
    buffer.stride = static_cast<uint32_t>(bufferHandle->stride);
#endif
#if ENABLE_MEMORY_CHECK
    if (isMonitorEnabled_) {
        printf("[Alloc] addr = %p, size = %d, usage = %llu\n",
            bufferHandle->virAddr, bufferHandle->size, bufferHandle->usage);
    }
#endif
    if ((info.usage & HBM_USE_MEM_YUV) == HBM_USE_MEM_YUV) {
        yuvBufCnt_++;
        if (yuvBufCnt_ > 3) { // 3: yuv buf cnt
            GRAPHIC_LOGW("Count of yuv buffer is %d. It should not be greater than 3.", yuvBufCnt_);
        }
    }
    return true;
}

void GrallocEngines::FreeBuffer(uint8_t* virAddr)
{
    if (grallocFuncs_ == nullptr) {
        GRAPHIC_LOGE("GrallocEngines not init!");
        return;
    }

    if (virAddr == nullptr) {
        return;
    }

    uint64_t addr = reinterpret_cast<uint64_t>(virAddr);
    auto iter = g_buffers.find(addr);
    if (iter != g_buffers.end()) {
        BufferHandle* handle = iter->second;
        if (handle != nullptr) {
            if ((handle->usage & HBM_USE_MEM_YUV) == HBM_USE_MEM_YUV) {
                yuvBufCnt_--;
            }
            g_buffers.erase(iter);
#if ENABLE_MEMORY_CHECK
            if (isMonitorEnabled_) {
                printf("[Free] addr = %p, size = %d, usage = %llu\n", virAddr, handle->size, handle->usage);
            }
#endif
            grallocFuncs_->FreeMem(handle);
        }
    }
}

uint8_t* GrallocEngines::AllocBufferBySize(uint32_t size, uint64_t usage)
{
    AllocInfo info;
    info.expectedSize = size;
    info.usage = usage;
    GrallocBuffer buffer;
    if (!AllocBuffer(info, buffer)) {
        return nullptr;
    }
    return reinterpret_cast<uint8_t*>(buffer.virAddr);
}

#if ENABLE_MEMORY_CHECK
void GrallocEngines::DumpMem(bool dumpDetails)
{
    int32_t fontCacheSize = 0;
    int32_t imageCacheSize = 0;
    int32_t pathCacheSize = 0;
    int32_t fbSize = 0;
    int32_t mapBufferSize = 0;
    int32_t yuvCacheSize = 0;
    int32_t fontCount = 0;
    int32_t fontMax = 0;
    int32_t pathCnt = 0;
    printf("current gralloc allocate memory:\n");
    for (auto it : g_buffers) {
        if ((it.second->usage & HBM_USE_MEM_FONT) == HBM_USE_MEM_FONT) {
            fontCacheSize += it.second->size;
            if (fontMax < it.second->size) {
                fontMax = it.second->size;
            }
            fontCount++;
            continue;
        }
        if (it.second->usage == HBM_USE_ASSIGN_SIZE) {
            imageCacheSize += it.second->size;
            if (dumpDetails) {
                printf("  image addr=%p, size=%d\n", reinterpret_cast<void*>(it.first), it.second->size);
            }
            continue;
        }
        if ((it.second->usage & HBM_USE_MEM_MULTI_IMG) == HBM_USE_MEM_MULTI_IMG) {
            imageCacheSize += it.second->size;
            if (dumpDetails) {
                printf("  multi image addr=%p, size=%d\n", reinterpret_cast<void *>(it.first), it.second->size);
            }
            continue;
        }
        if ((it.second->usage & HBM_USE_MEM_PATH) == HBM_USE_MEM_PATH) {
            pathCacheSize += it.second->size;
            if (dumpDetails) {
                printf("  path addr=%p, size=%d\n", reinterpret_cast<void *>(it.first), it.second->size);
            }
            pathCnt++;
            continue;
        }
        if ((it.second->usage & HBM_USE_MEM_FB) == HBM_USE_MEM_FB) {
            fbSize += it.second->size;
            if (dumpDetails) {
                printf("  fb addr=%p, size=%d\n", reinterpret_cast<void *>(it.first), it.second->size);
            }
            continue;
        }
        if ((it.second->usage & HBM_USE_MEM_BUFMAP) == HBM_USE_MEM_BUFMAP) {
            mapBufferSize += it.second->size;
            if (dumpDetails) {
                printf("  mapbuffer addr=%p, size=%d\n", reinterpret_cast<void *>(it.first), it.second->size);
            }
            continue;
        }
        if ((it.second->usage & HBM_USE_MEM_YUV) == HBM_USE_MEM_YUV) {
            yuvCacheSize += it.second->size;
            if (dumpDetails) {
                printf("  yuv addr=%p, size=%d\n", reinterpret_cast<void *>(it.first), it.second->size);
            }
            continue;
        }
        if ((it.second->usage & HBM_USE_MEM_JPEG_DATA) == HBM_USE_MEM_JPEG_DATA) {
            yuvCacheSize += it.second->size;
            if (dumpDetails) {
                printf("  jpeg addr=%p, size=%d\n", reinterpret_cast<void *>(it.first), it.second->size);
            }
            continue;
        }
    }
    printf("total size: font %d (count: %d, max: %d), image %d, path %d (count: %d), fb %d, mapbuffer:%d, yuv %d\n",
        fontCacheSize, fontCount, fontMax, imageCacheSize, pathCacheSize, pathCnt, fbSize, mapBufferSize, yuvCacheSize);
}

void GrallocEngines::SetMemMonitor(bool enable)
{
    isMonitorEnabled_ = enable;
}

#endif
} // namespace OHOS
