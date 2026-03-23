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

#include "display_gralloc.h"
#include <securec.h>
#include "display_gralloc_internal.h"
#include "display_layer_internal.h"
#ifdef __LITEOS__
#include "los_memory.h"
#endif
#include "mem_pool_los.h"
#include "mem_pool_types.h"
#include "graphic_hardware_config.h"
#if ENABLE_GMMU
#include "display_gmmu_internal.h"
#endif

#define MAX_MALLOC_SIZE 0x10000000L
#define FB_COUNT 2
#define YUV_BUFFER_COUNT 2

typedef struct {
    uint32_t numPlanes;
    uint32_t radio[MAX_PLANES];
} PlaneLayoutInfo;

typedef struct {
    uint32_t format;
    uint32_t bitsPerPixel; // bits per pixel for first plane
    const PlaneLayoutInfo *planes;
} FormatInfo;

static const PlaneLayoutInfo g_yuv420SPLayout = {
    .numPlanes = 2,
    .radio = { 4, 2 },
};

static const PlaneLayoutInfo g_yuv420PLayout = {
    .numPlanes = 3,
    .radio = { 4, 1, 1 },
};

typedef struct {
    bool isAllocated;
    uint64_t addr;
    uint64_t length;
} AddrInfo;

TryToFreeImgCallback g_tryToFreeImgCallback = NULL;
TryToFreeFontCacheCallback g_tryToFreeFontCacheCallback = NULL;

#if ENABLE_GMMU
static bool g_gmmuAllocated[FB_COUNT] = {false, false};
GmmuBuf g_gmmuMem = { 0 };
#else
static AddrInfo FBInfo[FB_COUNT] = {
    {false, MEM_POOL_UI_FB, MEM_POOL_UI_FB_SIZE / 2},
    {false, MEM_POOL_UI_FB + MEM_POOL_UI_FB_SIZE / 2, MEM_POOL_UI_FB_SIZE / 2},
};
#endif

static uint8_t *g_poolId = MEMORY_POOL_ID_INVALID;
static uint8_t *g_fontPoolId = MEMORY_POOL_ID_INVALID;

static bool InitMemPool(void)
{
    if (g_poolId != MEMORY_POOL_ID_INVALID) {
        DISP_LOGI("Memory Pool has been inited.\n");
        return true;
    }

    g_poolId = LosRegisterMemPool((uint8_t *)MEM_POOL_UI_OTHER, MEM_POOL_UI_OTHER_SIZE, 0, 0);
    if (g_poolId == MEMORY_POOL_ID_INVALID) {
        return false;
    }

    if (g_fontPoolId != MEMORY_POOL_ID_INVALID) {
        DISP_LOGI("Font Memory Pool has been inited.\n");
        return true;
    }

#if CAPABILITY_HWDRAW_LETTER_VECTOR || CAPABILITY_HWDRAW_ALPHA_BITMAP
    g_fontPoolId = LosRegisterMemPool((uint8_t *)MEM_POOL_UI_FONT, MEM_POOL_UI_FONT_SIZE, 0, 0);
    if (g_fontPoolId == MEMORY_POOL_ID_INVALID) {
        return false;
    }
#endif
    return true;
}

static void DeInitMemPool(void)
{
    CHECK_NULLPOINTER_RETURN(g_poolId);
    LosUnregisterMemPool(g_poolId);
}

static const FormatInfo *GetFormatInfo(uint32_t format)
{
    static const FormatInfo fmtInfos[] = {
        {PIXEL_FMT_RGBX_8888,  32, NULL},  {PIXEL_FMT_RGBA_8888, 32,  NULL},
        {PIXEL_FMT_BGRX_8888,  32, NULL},  {PIXEL_FMT_BGRA_8888, 32,  NULL},
        {PIXEL_FMT_RGB_888,    24, NULL},  {PIXEL_FMT_BGR_565,   16,  NULL},
        {PIXEL_FMT_RGBA_5551,  16, NULL},  {PIXEL_FMT_RGB_565,   16,  NULL},
        {PIXEL_FMT_BGRX_4444,  16, NULL},  {PIXEL_FMT_BGRA_4444, 16,  NULL},
        {PIXEL_FMT_RGBA_4444,  16, NULL},  {PIXEL_FMT_RGBX_4444, 16,  NULL},
        {PIXEL_FMT_BGRX_5551,  16, NULL},  {PIXEL_FMT_BGRA_5551, 16,  NULL},
        {PIXEL_FMT_YCBCR_420_SP, 8, &g_yuv420SPLayout}, {PIXEL_FMT_YCRCB_420_SP, 8, &g_yuv420SPLayout},
        {PIXEL_FMT_YCBCR_420_P, 8, &g_yuv420PLayout}, {PIXEL_FMT_YCRCB_420_P, 8, &g_yuv420PLayout},
    };

    for (uint32_t i = 0; i < sizeof(fmtInfos) / sizeof(FormatInfo); i++) {
        if (fmtInfos[i].format == format) {
            return &fmtInfos[i];
        }
    }
    DISP_LOGE("the format can not support %d\n", format);
    return NULL;
}

static int32_t InitBufferHandle(BufferHandle *buffer, const AllocInfo *info)
{
#if ENABLE_GMMU
    if (info->usage == HBM_USE_MEM_FB) {
        buffer->usage = info->usage;
        buffer->fd = -1;
        buffer->width = (int32_t)(info->width);
        buffer->height = (int32_t)(info->height);
        buffer->format = info->format;
        return DISPLAY_SUCCESS;
    }
#endif

    if ((info->usage & HBM_USE_ASSIGN_SIZE) == HBM_USE_ASSIGN_SIZE) {
        if (info->expectedSize <= 0) {
            DISP_LOGE("%s: alloc info is invalid, size is %u\n", __func__, info->expectedSize);
            return DISPLAY_FAILURE;
        }
        buffer->size = (int32_t)(info->expectedSize);
    } else {
        const FormatInfo *fmtInfo = GetFormatInfo(info->format);
        if (fmtInfo == NULL) {
            DISP_LOGE("%s: can not get format information : %d\n", __func__, info->format);
            return DISPLAY_FAILURE;
        }

        buffer->stride = ALIGN_UP(info->width, WIDTH_ALIGN) * (int32_t)(fmtInfo->bitsPerPixel) / BITS_PER_BYTE;
        buffer->size = (int32_t)(info->height) * buffer->stride;
        buffer->width = (int32_t)(info->width);
        buffer->height = (int32_t)(info->height);
    }
    buffer->usage = info->usage;
    buffer->fd = -1;
    buffer->format = info->format;
    return DISPLAY_SUCCESS;
}

static int32_t AllocMemForFB(BufferHandle *buffer)
{
#if ENABLE_GMMU
    for (int i = 0; i < FB_COUNT; i++) {
        if (!g_gmmuAllocated[i]) {
            g_gmmuAllocated[i] = true;
            buffer->virAddr = (void *)g_gmmuMem.buffer[i];
            buffer->phyAddr = (uint64_t)g_gmmuMem.buffer[i];
            buffer->stride = (int32_t)(g_gmmuMem.stride);
            buffer->size = (int32_t)(g_gmmuMem.size);
#ifndef UT_TEST
            memset_s(buffer->virAddr, g_gmmuMem.size, 0,  g_gmmuMem.size);
#endif
            return DISPLAY_SUCCESS;
        }
    }
    return DISPLAY_FAILURE;
#else
    for (int i = 0; i < FB_COUNT; i++) {
        if (!FBInfo[i].isAllocated && (buffer->size <= FBInfo[i].length)) {
            FBInfo[i].isAllocated = true;
            buffer->virAddr = (void *)FBInfo[i].addr;
            buffer->phyAddr = FBInfo[i].addr;
#ifndef UT_TEST
            memset_s(buffer->virAddr, (size_t)(FBInfo[i].length), 0, (size_t)(FBInfo[i].length));
#endif
            return DISPLAY_SUCCESS;
        }
    }
    return DISPLAY_FAILURE;
#endif
}

static int32_t AllocMem(const AllocInfo *info, BufferHandle **handle)
{
    CHECK_NULLPOINTER_RETURN_VALUE(info, DISPLAY_NULL_PTR);
    CHECK_NULLPOINTER_RETURN_VALUE(handle, DISPLAY_NULL_PTR);

    BufferHandle *buffer = (BufferHandle *)malloc(sizeof(BufferHandle));
    if (buffer == NULL) {
        DISP_LOGE("malloc mem for buffer failed.\n");
        return DISPLAY_NULL_PTR;
    }
    int32_t ret = InitBufferHandle(buffer, info);
    if (ret != DISPLAY_SUCCESS) {
        DISP_LOGE("InitBufferHandle failed.\n");
        goto END;
    }

    if ((info->usage & HBM_USE_MEM_FB) == HBM_USE_MEM_FB) {
        ret = AllocMemForFB(buffer);
        goto END;
    }

    uint8_t *pool = (((info->usage & HBM_USE_MEM_FONT) == HBM_USE_MEM_FONT) ||
        ((info->usage & HBM_USE_MEM_PATH) == HBM_USE_MEM_PATH)) ? g_fontPoolId : g_poolId;

    int32_t maxSize = ((pool == g_poolId) ? MEM_POOL_UI_OTHER_SIZE : MEM_POOL_UI_FONT_SIZE);
    if ((buffer->size <= 0) || (buffer->size > maxSize)) {
        DISP_LOGE("bufferSize[%lld] is invalid.\n", buffer->size);
        ret = DISPLAY_FAILURE;
        goto END;
    }

    uint8_t *addr = LosAllocMem(pool, (uint32_t)(buffer->size));
    if ((pool == g_poolId) && (g_tryToFreeImgCallback != NULL)) {
        while (addr == NULL) {
            if (g_tryToFreeImgCallback()) {
                DISP_LOGW("Success to free image during AllocMem size: %u\n", buffer->size);
                addr = LosAllocMem(pool, (uint32_t)(buffer->size));
            } else {
                DISP_LOGE("Failed to AllocMem size: %u, and no image can be free\n", buffer->size);
                break;
            }
        }
    } else if ((pool == g_fontPoolId) && (g_tryToFreeFontCacheCallback != NULL)) {
        while (addr == NULL) {
            if (g_tryToFreeFontCacheCallback()) {
                DISP_LOGW("Success to free font cache during AllocMem size: %u\n", buffer->size);
                addr = LosAllocMem(pool, (uint32_t)(buffer->size));
            } else {
                DISP_LOGE("Failed to AllocMem size: %u, and no font cache can be free\n", buffer->size);
                break;
            }
        }
    }
    ret = (addr == NULL) ? DISPLAY_FAILURE : DISPLAY_SUCCESS;
    buffer->virAddr = (void *)addr;
    buffer->phyAddr = (uint64_t)addr;

END:
    if ((ret != DISPLAY_SUCCESS) && (buffer != NULL)) {
        free(buffer);
        buffer = NULL;
    }
    *handle = buffer;
    return ret;
}

static void FreeMem(BufferHandle *handle)
{
    CHECK_NULLPOINTER_RETURN(handle);
    if ((handle->usage & HBM_USE_MEM_FB) == HBM_USE_MEM_FB) {
        for (int i = 0; i < FB_COUNT; i++) {
#if ENABLE_GMMU
            if (g_gmmuAllocated[i] && (handle->phyAddr == g_gmmuMem.buffer[i])) {
                g_gmmuAllocated[i] = false;
#ifndef UT_TEST
                memset_s(handle->virAddr, g_gmmuMem.size, 0,  g_gmmuMem.size);
#endif
            }
#else
            if (FBInfo[i].isAllocated && (handle->phyAddr == FBInfo[i].addr)) {
                FBInfo[i].isAllocated = false;
#ifndef UT_TEST
                memset_s(handle->virAddr, (size_t)(FBInfo[i].length), 0, (size_t)(FBInfo[i].length));
#endif
            }
#endif
        }
    } else if (((handle->usage & HBM_USE_MEM_FONT) == HBM_USE_MEM_FONT) ||
        ((handle->usage & HBM_USE_MEM_PATH) == HBM_USE_MEM_PATH)) {
        LosFreeMem(g_fontPoolId, handle->virAddr);
    } else {
        LosFreeMem(g_poolId, handle->virAddr);
    }
    handle->virAddr = NULL;
    handle->phyAddr = 0;
    free(handle);
}

static void RegisterTryToFreeImgCallback(TryToFreeImgCallback callback)
{
    g_tryToFreeImgCallback = callback;
}

static void RegisterTryToFreeFontCacheCallback(TryToFreeFontCacheCallback callback)
{
    g_tryToFreeFontCacheCallback = callback;
}

int32_t GrallocInitialize(GrallocFuncs **funcs)
{
    CHECK_NULLPOINTER_RETURN_VALUE(funcs, DISPLAY_NULL_PTR);
    GrallocFuncs *gFuncs = (GrallocFuncs *)malloc(sizeof(GrallocFuncs));
    if (gFuncs == NULL) {
        DISP_LOGE("gFuncs is null\n");
        return DISPLAY_NULL_PTR;
    }

    if (!InitMemPool()) {
        DISP_LOGE("InitMemPool failure\n");
        free(gFuncs);
        gFuncs = NULL;
        return DISPLAY_FAILURE;
    }

    (void)memset_s(gFuncs, sizeof(GrallocFuncs), 0, sizeof(GrallocFuncs));
    gFuncs->AllocMem = AllocMem;
    gFuncs->FreeMem = FreeMem;
    gFuncs->RegisterTryToFreeImgCallback = RegisterTryToFreeImgCallback;
    gFuncs->RegisterTryToFreeFontCacheCallback = RegisterTryToFreeFontCacheCallback;
    *funcs = gFuncs;

#if ENABLE_GMMU
    GmmuAttr gmmuAtr = {LAYER_BPP, RESOLUTION_WIDTH, RESOLUTION_HEIGHT};
    if (GmmuInit(&gmmuAtr) != DISPLAY_SUCCESS) {
        return DISPLAY_FAILURE;
    }
    GmmuAlloc(&g_gmmuMem);
#endif
    DISP_LOGI("gralloc initialize success\n");
    return DISPLAY_SUCCESS;
}

int32_t GrallocUninitialize(GrallocFuncs *funcs)
{
    CHECK_NULLPOINTER_RETURN_VALUE(funcs, DISPLAY_NULL_PTR);
    free(funcs);
    funcs = NULL;

    DeInitMemPool();
    DISP_LOGI("gralloc uninitialize success\n");

#if ENABLE_GMMU
    GmmuFree(&g_gmmuMem);
    GmmuDeinit();
#endif
    return DISPLAY_SUCCESS;
}
