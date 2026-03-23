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

#include "display_layer.h"
#include <math.h>
#include <securec.h>
#include "display_common.h"
#include "display_layer_internal.h"
#include "soc_dpu_api.h"
#ifdef __LITEOS__
#include "los_memory.h"
#endif
#include "td_base.h"
#include "graphic_hardware_config.h"

static bool g_hdmiConnected = false;
static bool g_lastLayerVisible = true;

static void SetHdmiConnect(bool connect)
{
    g_hdmiConnected = connect;
}

static bool HdmiIsConnected(void)
{
    return g_hdmiConnected;
}

static LayerPrivate *GetLayerInstance(uint32_t devId, uint32_t layerId)
{
    static LayerPrivate layerPriv[DEV_ID_NUM][GRA_LAYER_MAX] = {   // [1][3]
        {
            {
                .fd = -1,
                .type = LAYER_TYPE_GRAPHIC,
                .layerName = "/dev/fb0",
                .pixFmt = PIXEL_FMT_RGBA_5551,
                .flushRect = {0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT},
                .width = DEFAULT_WIDTH,
                .height = DEFAULT_HEIGHT,
                .cap = {TD_TRUE, DEFAULT_WIDTH, DEFAULT_HEIGHT, TD_TRUE},
            },
            {
                .fd = -1,
                .type = LAYER_TYPE_GRAPHIC,
                .layerName = "/dev/fb1",
                .pixFmt = PIXEL_FMT_RGBA_5551,
                .flushRect = {0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT},
                .width = DEFAULT_WIDTH,
                .height = DEFAULT_HEIGHT,
                .cap = {TD_TRUE, DEFAULT_WIDTH, DEFAULT_HEIGHT, TD_TRUE},
            },
            {
                .fd = -1,
                .type = LAYER_TYPE_GRAPHIC,
                .layerName = "/dev/fb2",
                .pixFmt = PIXEL_FMT_RGBA_5551,
                .flushRect = {0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT},
                .width = DEFAULT_WIDTH,
                .height = DEFAULT_HEIGHT,
                .cap = {TD_TRUE, DEFAULT_WIDTH, DEFAULT_HEIGHT, TD_TRUE},
            },
        }
    };
    return &layerPriv[devId][layerId];
}

static void PrintLayerInfo(const LayerInfo *layerInfo)
{
    DISP_LOGI("layerInfo:\n");
    DISP_LOGI("type = %d\n", layerInfo->type);
    DISP_LOGI("width = %d\n", layerInfo->width);
    DISP_LOGI("height = %d\n", layerInfo->height);
    DISP_LOGI("bpp = %d\n", layerInfo->bpp);
    DISP_LOGI("pixFormat = %d\n", layerInfo->pixFormat);
}

static void Convert2DpuPixFmt(PixelFormat fmt, ext_gfx_fmt *gfxFmt)
{
    switch (fmt) {
        case PIXEL_FMT_RGBA_5551:
            *gfxFmt = EXT_GFX_FMT_ARGB1555;
            break;
        case PIXEL_FMT_RGB_565:
            *gfxFmt = EXT_GFX_FMT_RGB565;
            break;
        case PIXEL_FMT_RGB_888:
            *gfxFmt = EXT_GFX_FMT_RGB888;
            break;
        case PIXEL_FMT_RGBA_8888:
            *gfxFmt = EXT_GFX_FMT_ARGB8888;
            break;
        case PIXEL_FMT_YCBCR_420_SP:
            *gfxFmt = EXT_GFX_FMT_YUV420SP;
            break;
        default:
            DISP_LOGE("unsupported pixel format:%d\n", fmt);
            break;
    }
}

static uint32_t CalcStride(uint32_t width, PixelFormat fmt, ext_gfx_compress_mode cmpMode)
{
    uint32_t bpp;
    uint32_t stride;
    switch (fmt) {
        case PIXEL_FMT_RGBA_8888:
            if (cmpMode == EXT_GFX_COMPRESS_HFBC) {
                bpp = 12; /* 12 bits */
            } else if (cmpMode == EXT_GFX_COMPRESS_HFBC_ABYPASS) {
                bpp = 16; /* 16 bits */
            } else {
                bpp = 32; /* 32 bits */
            }
            break;
        case PIXEL_FMT_RGBA_5551:
        case PIXEL_FMT_RGB_565:
            bpp = 16; /* 16 bits */
            break;
        case PIXEL_FMT_RGB_888:
            bpp = (cmpMode != EXT_GFX_COMPRESS_NONE) ? 8 : 24; /* 8,24 bits */
            break;
        default:
            bpp = 32; /* 32 bits */
            break;
    }
    stride = uapi_byte_align(width, DISP_BYTE_ALIGNMENT) * bpp / DISP_STRIDE_DIVIDER;
    if (cmpMode != EXT_GFX_COMPRESS_NONE) {
        stride = stride * 4; /* 4: compress coef */
    }
    return stride;
}

static void SavePrivLayerInfo(uint32_t devId, uint32_t layerId, int32_t fd, const LayerInfo *info)
{
    LayerPrivate *priv = GetLayerInstance(devId, layerId);
    priv->pixFmt = info->pixFormat;
    priv->type = info->type;
    priv->fd = fd;
    priv->flushRect.w = info->width;
    priv->flushRect.h = info->height;
    priv->width = info->width;
    priv->height = info->height;
}

static int32_t Flush(uint32_t devId, uint32_t layerId, LayerBuffer *buffer)
{
    if ((devId == 0) && (layerId == 0) && (buffer == NULL)) {
        if (uapi_dpu_refresh_layers() < 0) {
            DISP_LOGE("uapi_dpu_refresh_layers failed\n");
            return DISPLAY_FAILURE;
        }
        return DISPLAY_SUCCESS;
    }

    CHECK_NULLPOINTER_RETURN_VALUE(buffer, DISPLAY_NULL_PTR);
    CHECK_DEVID_VALID(devId, DISPLAY_FAILURE);
    CHECK_GRAPHIC_LAYERID_VALID(layerId, DISPLAY_FAILURE);
    CHECK_LAYER_OPENED(devId, layerId, DISPLAY_FAILURE);
    LayerPrivate *priv = GetLayerInstance(devId, layerId);

    ext_dpu_surface surface;
    (void)memset_s(&surface, sizeof(ext_dpu_surface), 0, sizeof(ext_dpu_surface));
    Convert2DpuPixFmt(buffer->pixFormat, &surface.color_fmt);
    surface.phy_addr = (td_u32)(buffer->data.phyAddr);
    surface.width = (td_u32)(buffer->width);
    surface.height = (td_u32)(buffer->height);
    surface.stride = (td_u32)(buffer->pitch);
    if (surface.color_fmt == EXT_GFX_FMT_YUV420SP) {
        surface.uv_stride = (td_u32)(buffer->pitch);
        surface.uv_offset = surface.stride * surface.height;
    }
    ext_rect rect;
    rect.x = priv->flushRect.x;
    rect.y = priv->flushRect.y;
    rect.width = (td_u32)(priv->flushRect.w);
    rect.height = (td_u32)(priv->flushRect.h);
#if DOUBLE_BUFFER
    surface.update_rect = &rect;
#else
    surface.update_rect = NULL;
#endif

    if (uapi_dpu_set_layer_info((td_handle)(priv->fd), &surface) < 0) {
        DISP_LOGE("uapi_dpu_set_layer_info failed\n");
        return DISPLAY_FAILURE;
    }

    return DISPLAY_SUCCESS;
}

static int32_t SetLayerVisible(uint32_t devId, uint32_t layerId, bool visibled)
{
    CHECK_DEVID_VALID(devId, DISPLAY_FAILURE);
    CHECK_GRAPHIC_LAYERID_VALID(layerId, DISPLAY_FAILURE);
    CHECK_LAYER_OPENED(devId, layerId, DISPLAY_FAILURE);
    LayerPrivate *priv = GetLayerInstance(devId, layerId);

    if (visibled) {
        if (uapi_dpu_show_layer((td_handle)(priv->fd)) < 0) {
            DISP_LOGE("uapi_dpu_show_layer failed!\n");
            return DISPLAY_FAILURE;
        }
    } else {
        if (uapi_dpu_hide_layer((td_handle)(priv->fd)) < 0) {
            DISP_LOGE("uapi_dpu_hide_layer failed!\n");
            return DISPLAY_FAILURE;
        }
    }
    return DISPLAY_SUCCESS;
}

static int32_t WaitForVBlank(uint32_t devId, uint32_t layerId, int32_t timeOut)
{
    (void)devId;
    (void)layerId;
    (void)timeOut;
    if (uapi_dpu_wait_layer_vsync() < 0) {
        DISP_LOGE("uapi_dpu_wait_layer_vsync failed!\n");
        return DISPLAY_FAILURE;
    }
    return DISPLAY_SUCCESS;
}

static int32_t GetLayerAlpha(uint32_t devId, uint32_t layerId, LayerAlpha *alpha)
{
    CHECK_DEVID_VALID(devId, DISPLAY_FAILURE);
    CHECK_GRAPHIC_LAYERID_VALID(layerId, DISPLAY_FAILURE);
    CHECK_LAYER_OPENED(devId, layerId, DISPLAY_FAILURE);
    LayerPrivate *priv = GetLayerInstance(devId, layerId);

    if (uapi_dpu_get_layer_alpha((td_handle)(priv->fd), &(alpha->gAlpha)) < 0) {
        DISP_LOGE("uapi_dpu_get_layer_alpha failed\n");
        return DISPLAY_FAILURE;
    }
    return DISPLAY_SUCCESS;
}

static int32_t SetLayerAlpha(uint32_t devId, uint32_t layerId, LayerAlpha *alpha)
{
    CHECK_DEVID_VALID(devId, DISPLAY_FAILURE);
    CHECK_GRAPHIC_LAYERID_VALID(layerId, DISPLAY_FAILURE);
    CHECK_LAYER_OPENED(devId, layerId, DISPLAY_FAILURE);
    LayerPrivate *priv = GetLayerInstance(devId, layerId);

    if (uapi_dpu_set_layer_alpha((td_handle)(priv->fd), alpha->gAlpha) < 0) {
        DISP_LOGE("uapi_dpu_set_layer_alpha failed\n");
        return DISPLAY_FAILURE;
    }
    return DISPLAY_SUCCESS;
}

static int32_t OpenLayer(uint32_t devId, const LayerInfo *layerInfo, uint32_t *layerId)
{
    CHECK_DEVID_VALID(devId, DISPLAY_FAILURE);
    CHECK_NULLPOINTER_RETURN_VALUE(layerInfo, DISPLAY_NULL_PTR);
    CHECK_NULLPOINTER_RETURN_VALUE(layerId, DISPLAY_NULL_PTR);
    PrintLayerInfo(layerInfo);
    if (layerInfo->type != LAYER_TYPE_GRAPHIC) {
        DISP_LOGE("unsupported layer type: %d\n", layerInfo->type);
        return DISPLAY_NOT_SUPPORT;
    }

    ext_dpu_layer_info dpuInfo;
    (void)memset_s(&dpuInfo, sizeof(ext_dpu_layer_info), 0, sizeof(ext_dpu_layer_info));
    dpuInfo.height = (td_u32)(layerInfo->height);
    dpuInfo.width = (td_u32)(layerInfo->width);
    dpuInfo.compress_mode = EXT_GFX_COMPRESS_NONE;
    dpuInfo.buf_mode = EXT_DPU_BUFFER_MODE_NORMAL;
    Convert2DpuPixFmt(layerInfo->pixFormat, &dpuInfo.layer_fmt);

    int32_t index;
    LayerPrivate *priv = NULL;
    for (index = EXT_DPU_LAYER_ID1; index < EXT_DPU_LAYER_ID_MAX; index++) {
        priv = GetLayerInstance(devId, (uint32_t)(index));
        if (priv->fd == -1) {
            dpuInfo.layer_id = index;
            if (uapi_dpu_create_layer(&dpuInfo, layerId) < 0) {
                DISP_LOGE("uapi_dpu_create_layer failed\n");
                return DISPLAY_FAILURE;
            }
#if !DOUBLE_BUFFER
            if (index == EXT_DPU_LAYER_ID1 && uapi_dpu_set_layer_vblank(*layerId, true) < 0) {
                DISP_LOGE("uapi_dpu_set_layer_vblank failed\n");
                return DISPLAY_FAILURE;
            }
#endif
            SavePrivLayerInfo(devId, (uint32_t)(index), (int32_t)(*layerId), layerInfo);
            return DISPLAY_SUCCESS;
        }
    }
    return DISPLAY_FAILURE;
}

static int32_t CloseLayer(uint32_t devId, uint32_t layerId)
{
    CHECK_DEVID_VALID(devId, DISPLAY_FAILURE);
    CHECK_GRAPHIC_LAYERID_VALID(layerId, DISPLAY_FAILURE);
    CHECK_LAYER_OPENED(devId, layerId, DISPLAY_FAILURE);
    LayerPrivate *priv = GetLayerInstance(devId, layerId);

    if (uapi_dpu_destroy_layer((td_handle)(priv->fd)) < 0) {
        DISP_LOGE("uapi_dpu_destroy_layer failed\n");
        return DISPLAY_FAILURE;
    }
    priv->fd = -1;

    priv->canvas_vir = NULL;
    priv->canvas.phy_addr = 0;
    return DISPLAY_SUCCESS;
}

static int32_t SetLayerZorder(uint32_t devId, uint32_t layerId, uint32_t zorder)
{
    CHECK_DEVID_VALID(devId, DISPLAY_FAILURE);
    CHECK_GRAPHIC_LAYERID_VALID(layerId, DISPLAY_FAILURE);
    CHECK_LAYER_OPENED(devId, layerId, DISPLAY_FAILURE);
    LayerPrivate *priv = GetLayerInstance(devId, layerId);

    if (uapi_dpu_set_layer_zorder((td_handle)(priv->fd), zorder) < 0) {
        DISP_LOGE("uapi_dpu_set_layer_zorder failed\n");
        return DISPLAY_FAILURE;
    }
    return DISPLAY_SUCCESS;
}

static int32_t GetLayerZorder(uint32_t devId, uint32_t layerId, uint32_t *zorder)
{
    CHECK_DEVID_VALID(devId, DISPLAY_FAILURE);
    CHECK_GRAPHIC_LAYERID_VALID(layerId, DISPLAY_FAILURE);
    CHECK_LAYER_OPENED(devId, layerId, DISPLAY_FAILURE);
    LayerPrivate *priv = GetLayerInstance(devId, layerId);

    if (uapi_dpu_get_layer_zorder((td_handle)(priv->fd), zorder) < 0) {
        DISP_LOGE("uapi_dpu_get_layer_zorder failed\n");
        return DISPLAY_FAILURE;
    }
    return DISPLAY_SUCCESS;
}

static void ConvertToLayerBuffer(uint32_t devId, uint32_t layerId, LayerBuffer *buffer)
{
    LayerPrivate *priv = GetLayerInstance(devId, layerId);
    buffer->data.virAddr = priv->canvas_vir;
    buffer->data.phyAddr = priv->canvas.phy_addr;
    buffer->width = (int32_t)priv->canvas.width;
    buffer->height = (int32_t)priv->canvas.height;
    buffer->pitch = (int32_t)(priv->canvas.stride);
    buffer->pixFormat = priv->pixFmt;
}

static int32_t GetLayerBuffer(uint32_t devId, uint32_t layerId, LayerBuffer *buffer)
{
    return DISPLAY_FAILURE;
}

static int32_t InitDisplay(uint32_t devId)
{
    CHECK_DEVID_VALID(devId, DISPLAY_FAILURE);
    return DISPLAY_SUCCESS;
}

static int32_t DeinitDisplay(uint32_t devId)
{
    CHECK_DEVID_VALID(devId, DISPLAY_FAILURE);
    return DISPLAY_SUCCESS;
}

static int32_t SetLayerDirtyRegion(uint32_t devId, uint32_t layerId, IRect *region)
{
    CHECK_NULLPOINTER_RETURN_VALUE(region, DISPLAY_NULL_PTR);
    CHECK_DEVID_VALID(devId, DISPLAY_FAILURE);
    CHECK_GRAPHIC_LAYERID_VALID(layerId, DISPLAY_FAILURE);
    CHECK_LAYER_OPENED(devId, layerId, DISPLAY_FAILURE);
    LayerPrivate *priv = GetLayerInstance(devId, layerId);
    priv->flushRect.x = region->x;
    priv->flushRect.y = region->y;
    priv->flushRect.w = region->w;
    priv->flushRect.h = region->h;

    bool widthInc = false;
    bool heightInc = false;
    // 2: width aligned by 2
    if (priv->flushRect.w % 2 == 1) {
        priv->flushRect.w += 1;
        widthInc = true;
    }

    // 2: height aligned by 2
    if (priv->flushRect.h % 2 == 1) {
        priv->flushRect.h += 1;
        heightInc = true;
    }

    // 2: x aligned by 2
    if (priv->flushRect.x % 2 == 1) {
        priv->flushRect.x -= 1;
        if (!widthInc && (priv->flushRect.x + priv->flushRect.w + 2 <= priv->width)) { // 2: inc by 2
            priv->flushRect.w += 2; // 2: inc by 2
        }
    }

    // 2: y aligned by 2
    if (priv->flushRect.y % 2 == 1) {
        priv->flushRect.y -= 1;
        if (!heightInc && (priv->flushRect.y + priv->flushRect.h + 2 <= priv->height)) { // 2: inc by 2
            priv->flushRect.h += 2; // 2: inc by 2
        }
    }
    return DISPLAY_SUCCESS;
}

// Set layer position, width and height is not handled.
static int32_t SetLayerSize(uint32_t devId, uint32_t layerId, IRect *rect)
{
    CHECK_DEVID_VALID(devId, DISPLAY_FAILURE);
    CHECK_GRAPHIC_LAYERID_VALID(layerId, DISPLAY_FAILURE);
    LayerPrivate *priv = GetLayerInstance(devId, layerId);

    if (layerId == EXT_DPU_LAYER_ID2) {
        rect->x = (rect->x / 2) * 2; // 2: align by 2
        rect->y = (rect->y / 2) * 2; // 2: align by 2
        bool layerVisible = true;
        if ((rect->x >= RESOLUTION_WIDTH) || (rect->x <= -RESOLUTION_WIDTH) ||
            (rect->y >= RESOLUTION_HEIGHT) || (rect->y <= -RESOLUTION_HEIGHT)) {
            layerVisible = false;
        }
        if (g_lastLayerVisible != layerVisible) {
            g_lastLayerVisible = layerVisible;
            SetLayerVisible(devId, layerId, layerVisible);
        }
        if (uapi_dpu_set_layer_pos((td_handle)(priv->fd), rect->x, rect->y) < 0) {
            DISP_LOGE("uapi_dpu_set_layer_pos failed. x: %d, y: %d", rect->x, rect->y);
            return DISPLAY_FAILURE;
        }
    }
    return DISPLAY_SUCCESS;
}

static int32_t SetLayerColorKey(uint32_t devId, uint32_t layerId, bool enable, uint32_t key)
{
    CHECK_DEVID_VALID(devId, DISPLAY_FAILURE);
    CHECK_GRAPHIC_LAYERID_VALID(layerId, DISPLAY_FAILURE);
    LayerPrivate *priv = GetLayerInstance(devId, layerId);

    ext_dpu_colorkey colorKey;
    if (enable) {
        colorKey.key_en = true;
        colorKey.key_value = key;
    } else {
        colorKey.key_en = false;
    }
    if (uapi_dpu_set_layer_colorkey((td_handle)(priv->fd), &colorKey) < 0) {
        DISP_LOGE("uapi_dpu_set_layer_colorkey failed.");
        return DISPLAY_FAILURE;
    }
    return DISPLAY_SUCCESS;
}

static int32_t GetLayerColorKey(uint32_t devId, uint32_t layerId, bool *enable, uint32_t *key)
{
    CHECK_DEVID_VALID(devId, DISPLAY_FAILURE);
    CHECK_GRAPHIC_LAYERID_VALID(layerId, DISPLAY_FAILURE);
    LayerPrivate *priv = GetLayerInstance(devId, layerId);

    ext_dpu_colorkey colorKey;
    if (uapi_dpu_get_layer_colorkey((td_handle)(priv->fd), &colorKey) < 0) {
        DISP_LOGE("uapi_dpu_get_layer_colorkey failed.");
        return DISPLAY_FAILURE;
    }
    *enable = colorKey.key_en;
    *key = colorKey.key_value;
    return DISPLAY_SUCCESS;
}

static void LayerFuncInit(LayerFuncs *funcs)
{
    funcs->InitDisplay = InitDisplay;
    funcs->DeinitDisplay = DeinitDisplay;
    funcs->CreateLayer = OpenLayer;
    funcs->CloseLayer = CloseLayer;
    funcs->SetLayerVisible = SetLayerVisible;
    funcs->Flush = Flush;
    funcs->GetLayerBuffer = GetLayerBuffer;
    funcs->SetLayerDirtyRegion = SetLayerDirtyRegion;
    funcs->WaitForVBlank = WaitForVBlank;
    funcs->SetLayerZorder = SetLayerZorder;
    funcs->GetLayerZorder = GetLayerZorder;
    funcs->SetLayerAlpha = SetLayerAlpha;
    funcs->GetLayerAlpha = GetLayerAlpha;
    funcs->SetLayerColorKey = SetLayerColorKey;
    funcs->GetLayerColorKey = GetLayerColorKey;
    funcs->SetLayerSize = SetLayerSize;
}

int32_t LayerInitialize(LayerFuncs **funcs)
{
    CHECK_NULLPOINTER_RETURN_VALUE(funcs, DISPLAY_NULL_PTR);
    LayerFuncs *lFuncs = (LayerFuncs *)malloc(sizeof(LayerFuncs));
    if (lFuncs == NULL) {
        DISP_LOGE("lFuncs is null\n");
        return DISPLAY_NULL_PTR;
    }

    (void)memset_s(lFuncs, sizeof(LayerFuncs), 0, sizeof(LayerFuncs));
    LayerFuncInit(lFuncs);
    *funcs = lFuncs;
    DISP_LOGI("layer initialize success\n");
    return DISPLAY_SUCCESS;
}

int32_t LayerUninitialize(LayerFuncs *funcs)
{
    CHECK_NULLPOINTER_RETURN_VALUE(funcs, DISPLAY_NULL_PTR);
    free(funcs);
    funcs = NULL;
    DISP_LOGI("layer uninitialize success\n");
    return DISPLAY_SUCCESS;
}
