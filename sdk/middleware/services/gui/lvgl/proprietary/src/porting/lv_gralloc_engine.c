/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#include "lv_gralloc_engine.h"
#include <stdio.h>
#include "display_gralloc.h"
#include "lv_ll.h"
#include "lvgl.h"

typedef struct {
    uint64_t virAddr;
    BufferHandle* hdl;
} LvGrallocInfo;

static GrallocFuncs* g_grallocFuncs;
static lv_ll_t g_bufferInfoLL;
static bool isMonitorEnabled = false;

bool LvInitGrallocEngine(void)
{
    if (g_grallocFuncs != NULL) {
        return true;
    }
    if (GrallocInitialize(&g_grallocFuncs) != DISPLAY_SUCCESS) {
        return false;
    }
    _lv_ll_init(&g_bufferInfoLL, sizeof(LvGrallocInfo));
    return true;
}

bool LvAllocBuffer(const AllocInfo* info, LvGrallocBuffer* buffer)
{
    if (g_grallocFuncs == NULL) {
        LV_LOGW("GrallocEngines not init!\n");
        return false;
    }
    BufferHandle* bufferHandle = NULL;
    if (g_grallocFuncs->AllocMem(info, &bufferHandle) != DISPLAY_SUCCESS) {
        LV_LOGE("gralloc AllocMem failed!\n");
        return false;
    }

    LvGrallocInfo* data = _lv_ll_ins_tail(&g_bufferInfoLL);
    if (data == NULL) {
        LV_LOGE("data is nullptr.\n");
        return false;
    }
    data->virAddr = (uint64_t)bufferHandle->virAddr;
    data->hdl = bufferHandle;

    buffer->phyAddr = bufferHandle->phyAddr;
    buffer->virAddr = bufferHandle->virAddr;
#if ENABLE_GMMU
    buffer->stride = (uint32_t)bufferHandle->stride;
#endif

    if (isMonitorEnabled) {
        LV_LOGI("[Alloc] addr = %p, size = %d, usage = %llu\n",
            bufferHandle->virAddr, bufferHandle->size, bufferHandle->usage);
    }
    return true;
}

bool LvFreeBuffer(uint8_t* virAddr)
{
    if (g_grallocFuncs == NULL) {
        LV_LOGW("GrallocEngines not init!\n");
        return false;
    }

    if (virAddr == NULL) {
        return false;
    }

    if (_lv_ll_is_empty(&g_bufferInfoLL)) {
        return false;
    }

    LvGrallocInfo* info = _lv_ll_get_head(&g_bufferInfoLL);
    while (info != NULL) {
        if (info->virAddr == (uint64_t)virAddr) {
            _lv_ll_remove(&g_bufferInfoLL, info);
            if (isMonitorEnabled) {
                LV_LOGI("[Free] addr = %p, size = %d, usage = %llu\n",
                    info->hdl->virAddr, info->hdl->size, info->hdl->usage);
            }
            g_grallocFuncs->FreeMem(info->hdl);
            lv_mem_free(info);
            return true;
        }
        info = _lv_ll_get_next(&g_bufferInfoLL, info);
    }
    return false;
}

static uint8_t* AllocBufferByUsage(uint32_t size, uint64_t usage)
{
    AllocInfo allocInfo;
    allocInfo.expectedSize = size;
    allocInfo.usage = usage;
    LvGrallocBuffer buffer;
    if (!LvAllocBuffer(&allocInfo, &buffer)) {
        LV_LOGE("AllocBuffer failed.\n");
        return NULL;
    }
    return (uint8_t*)buffer.virAddr;
}

uint8_t* LvAllocBufferBySize(uint32_t size)
{
    return AllocBufferByUsage(size, HBM_USE_ASSIGN_SIZE);
}

uint8_t* LvAllocBufferForImg(uint32_t size)
{
    return AllocBufferByUsage(size, HBM_USE_ASSIGN_SIZE | HBM_USE_MEM_MULTI_IMG);
}

uint8_t* LvAllocBufferForPath(uint32_t size)
{
    return AllocBufferByUsage(size, HBM_USE_ASSIGN_SIZE | HBM_USE_MEM_PATH);
}

void LvSetMemMonitor(bool enable)
{
    isMonitorEnabled = enable;
}

void LvDumpMem(void)
{
    uint32_t miscSize = 0;
    uint32_t imgSize = 0;
    uint32_t fbSize = 0;
    uint32_t pathSize = 0;
    uint32_t yuvSize = 0;
    uint32_t fontSize = 0;
    uint32_t fontMaxSize = 0;
    uint32_t fontCount = 0;

    LV_LOGI("====================Current gralloc allocate memory:====================\n");
    LvGrallocInfo* cur = _lv_ll_get_head(&g_bufferInfoLL);
    while (cur != NULL) {
        if (cur->hdl->usage == HBM_USE_ASSIGN_SIZE) {
            miscSize += (uint32_t)cur->hdl->size;
            LV_LOGI("   [Misc] addr = %p, size = %d\n", cur->hdl->virAddr, cur->hdl->size);
        } else if ((cur->hdl->usage & HBM_USE_MEM_MULTI_IMG) == HBM_USE_MEM_MULTI_IMG) {
            imgSize += (uint32_t)cur->hdl->size;
            LV_LOGI("   [Img] addr = %p, size = %d\n", cur->hdl->virAddr, cur->hdl->size);
        } else if ((cur->hdl->usage & HBM_USE_MEM_PATH) == HBM_USE_MEM_PATH) {
            pathSize += (uint32_t)cur->hdl->size;
            LV_LOGI("   [Path] addr = %p, size = %d\n", cur->hdl->virAddr, cur->hdl->size);
        } else if ((cur->hdl->usage & HBM_USE_MEM_FB) == HBM_USE_MEM_FB) {
            fbSize += (uint32_t)cur->hdl->size;
            LV_LOGI("   [FB] addr = %p, size = %d\n", cur->hdl->virAddr, cur->hdl->size);
        } else if ((cur->hdl->usage & HBM_USE_MEM_YUV) == HBM_USE_MEM_YUV) {
            yuvSize += (uint32_t)cur->hdl->size;
            LV_LOGI("   [YUV] addr = %p, size = %d\n", cur->hdl->virAddr, cur->hdl->size);
        } else if ((cur->hdl->usage & HBM_USE_MEM_FONT) == HBM_USE_MEM_FONT) {
            fontSize += (uint32_t)cur->hdl->size;
            if (fontMaxSize < (uint32_t)cur->hdl->size) {
                fontMaxSize = (uint32_t)cur->hdl->size;
            }
            fontCount++;
            LV_LOGI("   [Font] addr = %p, size = %d\n", cur->hdl->virAddr, cur->hdl->size);
        }
        cur = _lv_ll_get_next(&g_bufferInfoLL, cur);
    }
    LV_LOGI("\n[TOTAL] %u\n", fbSize + miscSize + imgSize + pathSize + fontSize);
    LV_LOGI("   [Fb] %u\n   [Misc] %u\n   [Img] %u\n   [Path] %u\n   [Yuv] %u\n   [Font] %u (maxSize: %u, count: %u)\n",
        fbSize, miscSize, imgSize, pathSize, yuvSize, fontSize, fontMaxSize, fontCount);
}