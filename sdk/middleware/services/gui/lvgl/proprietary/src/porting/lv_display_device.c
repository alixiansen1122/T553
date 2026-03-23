/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#include "lv_display_device.h"
#include <stdio.h>
#include "display_layer.h"
#include "lv_gralloc_engine.h"
#include "lv_conf_ext.h"
#if LV_USE_DFX_CMD && LV_USE_DFX_TRACE
#include "lv_dfx_trace.h"
#endif

typedef struct {
    uint32_t layerHdl;
    LayerInfo layerInfo;
    uint8_t bufferCount;
    LayerBuffer* buffer;
    lv_point_t layerPosition;
} LvLayer;

static uint8_t g_currentBufferIndex[LV_LAYER_MAX];
static LvLayer g_layers[LV_LAYER_MAX];
/* default dev id */
static uint32_t g_devId = 0;

/* LayerInfo struct default value */
static LayerType g_layerType = LAYER_TYPE_GRAPHIC;
static LayerFuncs* g_layerFuncs = NULL;
static uint8_t g_bufCount = 2;
static bool g_isInit = false;

#if LV_USE_SURFACE_VIEW
static lv_obj_t* g_surfaceView = NULL;
static uint8_t g_surfaceInvalidateCnt = 0;
#endif

#if LV_USE_ASYNC_MODE
static bool g_isAsyncMode = true;
#else
static bool g_isAsyncMode = false;
#endif

typedef struct {
    lv_area_t area;
    bool hasPendingFrame;
} AsyncInfo;

static AsyncInfo g_asyncInfo = {(lv_area_t){0, 0, 0, 0}, false};
static uint8_t g_exitUlpsCnt = 0;

#if LV_USE_SURFACE_VIEW
void LvSetSurfaceView(lv_obj_t* obj)
{
    g_surfaceView = obj;
    if (obj == NULL) {
#if LV_USE_ASYNC_MODE
        LvEnableAsyncMode(true);
#endif
        g_surfaceInvalidateCnt = 0;
    } else {
#if LV_USE_ASYNC_MODE
        LvEnableAsyncMode(false);
#endif
    }
}

lv_obj_t* LvObtainSurfaceView(void)
{
    return g_surfaceView;
}

void LvAsyncInvalidateSurfaceView(void)
{
    if (g_surfaceView != NULL) {
        g_surfaceInvalidateCnt++;
    }
}

void LvPreprocessSurfaceView(void)
{
    if (g_surfaceView == NULL) {
        return;
    }
    if (g_surfaceInvalidateCnt > 0) {
        lv_obj_invalidate(g_surfaceView);
        g_surfaceInvalidateCnt--;
    }
}
#endif

static bool LvCheckLayerFuncs(void)
{
    if (g_layerFuncs == NULL) {
        LV_LOGE("layerFuncs is not init.\n");
        return false;
    }
    if ((g_layerFuncs->InitDisplay == NULL) || (g_layerFuncs->CreateLayer == NULL) ||
        (g_layerFuncs->SetLayerDirtyRegion == NULL) || (g_layerFuncs->Flush == NULL) ||
        (g_layerFuncs->SetLayerColorKey == NULL) || (g_layerFuncs->CloseLayer == NULL) ||
        (g_layerFuncs->WaitForVBlank == NULL) || (g_layerFuncs->SetLayerSize == NULL)) {
        LV_LOGE("related funcs is not init.\n");
        return false;
    }
    return true;
}

static uint32_t LvCalcStride(uint32_t width, PixelFormat fmt)
{
    uint32_t bytePerPixel;
    uint32_t stride;
    switch (fmt) {
        case PIXEL_FMT_RGBA_8888:
            bytePerPixel = 4; // 4: byte per pixel
            break;
        case PIXEL_FMT_RGBA_5551:
        case PIXEL_FMT_RGB_565:
            bytePerPixel = 2; // 2: byte per pixel
            break;
        case PIXEL_FMT_RGB_888:
            bytePerPixel = 3; // 3: byte per pixel
            break;
        default:
            LV_LOGE("Unsupported PixelFormat: %d, just return 0.\n", fmt);
            return 0;
    }
    stride = LV_ALIGN_BYTE(width, LV_BYTE_ALIGNMENT) * bytePerPixel;
    return stride;
}

bool LvInitDisplayDevice(lv_coord_t width, lv_coord_t height)
{
    if (g_isInit) {
        LV_LOGW("display device is already init.\n");
        return false;
    }
    LvInitGrallocEngine();
    /* obtain pointers to layer functions */
    LayerInitialize(&g_layerFuncs);
    if (!LvCheckLayerFuncs()) {
        LV_LOGE("LayerInitialize failed.\n");
        return false;
    }

    /* init display */
    g_layerFuncs->InitDisplay(g_devId);

    /* open default Layer 0 */
    LayerInfo layerInfo;
    layerInfo.width = (int32_t)width;
    layerInfo.height = (int32_t)height;
    layerInfo.pixFormat = DEFAULT_PIX_FMT;
    layerInfo.bpp = DEFAULT_PIX_BPP;
    layerInfo.type = g_layerType;
    if (!LvOpenLayer(LV_LAYER_0, &layerInfo, g_bufCount)) {
        LV_LOGE("OpenLayer failed.\n");
        return false;
    }

    /* alloc buffer for default layer 0 */
    if (!LvAllocLayerBuffer(LV_LAYER_0)) {
        LvCloseLayer(LV_LAYER_0);
        LV_LOGE("AllocLayerBuffer failed.\n");
        return false;
    }

    IRect dirtyRect = {0, 0, g_layers[LV_LAYER_0].layerInfo.width - 1, g_layers[LV_LAYER_0].layerInfo.height - 1};
    g_layerFuncs->SetLayerDirtyRegion(g_devId, g_layers[LV_LAYER_0].layerHdl, &dirtyRect);

    g_isInit = true;
    return true;
}

void LvCloseDisplayDevice(void)
{
    if (!g_isInit) {
        LV_LOGW("dispaly device is not init.\n");
        return;
    }
    LvFreeLayerBuffer(LV_LAYER_0);
    LvCloseLayer(LV_LAYER_0);
    if (g_layerFuncs->DeinitDisplay(g_devId) != DISPLAY_SUCCESS) {
        LV_LOGE("DeinitDisplay failed.\n");
        return;
    }
    if (LayerUninitialize(g_layerFuncs) != DISPLAY_SUCCESS) {
        LV_LOGE("LayerUninitialize failed.\n");
        return;
    }
    g_isInit = false;
}

bool LvSwapBuffer(LvLayerId layerId)
{
    if ((layerId < LV_LAYER_0) || (layerId >= LV_LAYER_MAX)) {
        LV_LOGW("Invalid layerId[%d].\n", layerId);
        return false;
    }
    if (g_layers[layerId].bufferCount < 2) { // 2: buffer count
        LV_LOGW("layer[%d] is not opened or only has 1 buffer.\n", layerId);
        return false;
    }
    uint8_t currentBufferId = g_currentBufferIndex[layerId];
    if (currentBufferId + 1 < g_layers[layerId].bufferCount) {
        g_currentBufferIndex[layerId] = currentBufferId + 1;
    } else {
        g_currentBufferIndex[layerId] = 0;
    }
    return true;
}

void LvWaitForVBlank(void)
{
    if (g_layerFuncs->WaitForVBlank(g_devId, g_layers[LV_LAYER_0].layerHdl, 0) != DISPLAY_SUCCESS) {
        LV_LOGE("WaitForVBlank failed.\n");
    }
}

void LvCloseLayer(LvLayerId layerId)
{
    if ((layerId < LV_LAYER_0) || (layerId >= LV_LAYER_MAX)) {
        LV_LOGW("Invalid layerId[%d].\n", layerId);
        return;
    }

    if (g_layers[layerId].bufferCount == 0) {
        LV_LOGW("layer[%d] is already closed.\n", layerId);
        return;
    }

    if (g_layerFuncs->CloseLayer(g_devId, g_layers[layerId].layerHdl) != DISPLAY_SUCCESS) {
        LV_LOGE("close layer[%d] failed.\n", layerId);
        return;
    }

    free(g_layers[layerId].buffer);
    g_layers[layerId].buffer = NULL;
    g_layers[layerId].layerHdl = 0;
    g_layers[layerId].bufferCount = 0;
    g_layers[layerId].layerPosition.x = 0;
    g_layers[layerId].layerPosition.y = 0;
    (void)memset_s(&g_layers[layerId].layerInfo, sizeof(LayerInfo), 0, sizeof(LayerInfo));
}

void LvFreeLayerBuffer(LvLayerId layerId)
{
    if ((layerId < LV_LAYER_0) || (layerId >= LV_LAYER_MAX)) {
        LV_LOGW("Invalid layerId[%d].\n", layerId);
        return;
    }

    for (uint8_t i = 0; i < g_layers[layerId].bufferCount; i++) {
        LvFreeBuffer(g_layers[layerId].buffer[i].data.virAddr);
    }
}

static void LvClearLayerBuffer(LayerBuffer* buffer)
{
    buffer->fenceId = 0;
    buffer->width = 0;
    buffer->height = 0;
    buffer->pitch = 0;
    buffer->pixFormat = PIXEL_FMT_BUTT;
    buffer->data.phyAddr = 0;
    buffer->data.virAddr = NULL;
}

bool LvOpenLayer(LvLayerId layerId, LayerInfo* info, uint8_t bufCount)
{
    if ((layerId < LV_LAYER_0) || (layerId >= LV_LAYER_MAX) || (bufCount == 0) || (bufCount > LV_BUFFER_MAX)) {
        LV_LOGW("Invalid layerId[%d] or invalid bufCount[%u].\n", layerId, bufCount);
        return false;
    }

    if (g_layers[layerId].bufferCount != 0) {
        LV_LOGW("layer[%d] is already opened with BufferNo[%u].\n", layerId, g_layers[layerId].bufferCount);
        return false;
    }

    size_t size = bufCount * sizeof(LayerBuffer);
    g_layers[layerId].buffer = (LayerBuffer*)malloc(size);
    (void)memset_s(g_layers[layerId].buffer, size, 0, size);
    if (g_layers[layerId].buffer == NULL) {
        LV_LOGE("new LayerBuffer failed.\n");
        return false;
    }

    g_layers[layerId].layerInfo.width = info->width;
    g_layers[layerId].layerInfo.height = info->height;
    g_layers[layerId].layerInfo.type = info->type;
    g_layers[layerId].layerInfo.bpp = info->bpp;
    g_layers[layerId].layerInfo.pixFormat = info->pixFormat;
    g_layers[layerId].bufferCount = bufCount;
    g_layers[layerId].layerPosition.x = 0;
    g_layers[layerId].layerPosition.y = 0;
    if (g_layerFuncs->CreateLayer(g_devId, &g_layers[layerId].layerInfo, &g_layers[layerId].layerHdl) !=
        DISPLAY_SUCCESS) {
        free(g_layers[layerId].buffer);
        g_layers[layerId].buffer = NULL;
        g_layers[layerId].layerHdl = 0;
        g_layers[layerId].bufferCount = 0;
        (void)memset_s(&g_layers[layerId].layerInfo, sizeof(LayerInfo), 0, sizeof(LayerInfo));
        LV_LOGE("OpenLayer failed. LayerId: %d.\n", layerId);
        return false;
    }
    return true;
}

bool LvAllocLayerBuffer(LvLayerId layerId)
{
    if ((layerId < LV_LAYER_0) || (layerId >= LV_LAYER_MAX) || (g_layers[layerId].bufferCount == 0)) {
        LV_LOGW("Invalid layerId[%d] or layer is not opened.\n", layerId);
        return false;
    }

    if (g_layers[layerId].buffer[0].data.virAddr != NULL) {
        LV_LOGW("layer[%d] : Memory of buffers is already allocated.\n", layerId);
        return false;
    }

#if !ENABLE_GMMU
    uint32_t stride = LvCalcStride(g_layers[layerId].layerInfo.width, g_layers[layerId].layerInfo.pixFormat);
    if (stride == 0) {
        return false;
    }
#endif

    uint8_t index = 0;
    while (index < g_layers[layerId].bufferCount) {
        AllocInfo info;
#if ENABLE_GMMU
        info.usage = HBM_USE_MEM_FB;
#else
        info.expectedSize = stride * (uint16_t)g_layers[layerId].layerInfo.height;
        info.usage = HBM_USE_ASSIGN_SIZE | HBM_USE_MEM_FB;
#endif
        LvGrallocBuffer buffer;
        if (!LvAllocBuffer(&info, &buffer)) {
            LV_LOGE("AllocMem failed.\n");
            goto ERROR;
        }

        g_layers[layerId].buffer[index].data.virAddr = buffer.virAddr;
        g_layers[layerId].buffer[index].data.phyAddr = (uint64_t)buffer.virAddr;
        g_layers[layerId].buffer[index].width = g_layers[layerId].layerInfo.width;
        g_layers[layerId].buffer[index].height = g_layers[layerId].layerInfo.height;
#if ENABLE_GMMU
        g_layers[layerId].buffer[index].pitch = (int32_t)buffer.stride;
#else
        g_layers[layerId].buffer[index].pitch = (int32_t)stride;
#endif
        g_layers[layerId].buffer[index].pixFormat = g_layers[layerId].layerInfo.pixFormat;
        ++index;
    }
    return true;
ERROR:
    for (uint8_t i = 0; i < index; i++) {
        LvFreeBuffer((uint8_t*)g_layers[layerId].buffer[i].data.virAddr);
        LvClearLayerBuffer(&g_layers[layerId].buffer[i]);
    }
    return false;
}

static void FlushOnSyncMode(lv_disp_drv_t* dispDrv, const lv_area_t* area)
{
    if (area == NULL) {
        LvAsyncFlush();
        return;
    }

    if (g_asyncInfo.hasPendingFrame) {
        g_asyncInfo.hasPendingFrame = false;
    }

#if LV_USE_DFX_CMD && LV_USE_DFX_TRACE
    PeriodTraceStart(PERIOD_TRACE_FLUSH);
#endif
    LvSetLayerDirtyRegion(LV_LAYER_0, area);
    LvFlush(LV_LAYER_0);

#if LV_USE_SURFACE_VIEW
    if (g_surfaceView != NULL) {
        LvFlushSurfaceView(g_surfaceView);
    }
#endif

    LvFlush(LV_LAYER_MAX);
    lv_disp_flush_ready(dispDrv);
    LvSwapBuffer(LV_LAYER_0);

#if LV_USE_SURFACE_VIEW
    if (g_surfaceView != NULL) {
        LvSurfacePostFlush(g_surfaceView);
    }
#endif
#if LV_USE_DFX_CMD && LV_USE_DFX_TRACE
    PeriodTraceEnd(PERIOD_TRACE_FLUSH);
#endif
}

bool LvHasPendingFrame(void)
{
    return g_asyncInfo.hasPendingFrame;
}

void LvAsyncFlush(void)
{
    if (!g_asyncInfo.hasPendingFrame) {
        return;
    }

    if (LvIsUlpsEnabled()) {
        if (LvGetExistUlpsCnt() == 0) {
            uapi_mipi_tx_exit_ulps();
            LvIncExitUlpsCnt();
            uapi_tcxo_delay_ms(1);
        }
    }
    LvRenderWaitDone();
#if LV_USE_DFX_CMD && LV_USE_DFX_TRACE
    PeriodTraceStart(PERIOD_TRACE_FLUSH);
#endif
    LvSwapBuffer(LV_LAYER_0);
    LvSetLayerDirtyRegion(LV_LAYER_0, &g_asyncInfo.area);
    LvFlush(LV_LAYER_0);
    LvFlush(LV_LAYER_MAX);
    LvSwapBuffer(LV_LAYER_0);
    g_asyncInfo.hasPendingFrame = false;
#if LV_USE_DFX_CMD && LV_USE_DFX_TRACE
    PeriodTraceEnd(PERIOD_TRACE_FLUSH);
#endif
}

void LvFlushHandler(lv_disp_drv_t* dispDrv, const lv_area_t* area)
{
    if (!LvIsAsyncMode()) {
        FlushOnSyncMode(dispDrv, area);
        return;
    }

    // Flush On Async Mode
    LvAsyncFlush();

    if (area == NULL) {
        return;
    }

    LvSyncHwDraw(false);
    g_asyncInfo.area = *area;
    g_asyncInfo.hasPendingFrame = true;
    lv_disp_flush_ready(dispDrv);
    LvSwapBuffer(LV_LAYER_0);
}

void LvFlush(LvLayerId layerId)
{
    if ((layerId < LV_LAYER_0) || (layerId > LV_LAYER_MAX)) {
        LV_LOGW("Invalid layerId[%d].\n", layerId);
        return;
    }
    if (g_layerFuncs->Flush != NULL) {
        if (layerId == LV_LAYER_MAX) {
            if (g_layerFuncs->Flush(0, 0, NULL) != DISPLAY_SUCCESS) {
                LV_LOGE("flush failed.\n");
            }
            if (LvIsUlpsEnabled()) {
                LvDecExitUlpsCnt();
            }
            return;
        }
        uint8_t currentBufferIndex = g_currentBufferIndex[layerId];
        if (g_layerFuncs->Flush(g_devId, g_layers[layerId].layerHdl, &g_layers[layerId].buffer[currentBufferIndex]) !=
            DISPLAY_SUCCESS) {
            LV_LOGE("flush failed.\n");
            return;
        }
    }
}

void LvSetLayerDirtyRegion(LvLayerId layerId, const lv_area_t* dirtyArea)
{
    if ((layerId < LV_LAYER_0) || (layerId >= LV_LAYER_MAX)) {
        LV_LOGW("Invalid layerId[%d].\n", layerId);
        return;
    }
    IRect rect = {dirtyArea->x1, dirtyArea->y1, lv_area_get_width(dirtyArea), lv_area_get_height(dirtyArea)};
    if (g_layerFuncs->SetLayerDirtyRegion(g_devId, g_layers[layerId].layerHdl, &rect) !=
        DISPLAY_SUCCESS) {
        LV_LOGE("SetLayerDirtyRegion failed. LayerId: %d.\n", layerId);
    }
}

const LayerBuffer* LvGetLayerBuffer(LvLayerId layerId, uint8_t* bufCount)
{
    if ((layerId < LV_LAYER_0) || (layerId >= LV_LAYER_MAX)) {
        LV_LOGW("Invalid layerId[%d].\n", layerId);
        return NULL;
    }
    *bufCount = g_layers[layerId].bufferCount;
    return g_layers[layerId].buffer;
}

void LvSetHardwareLayerBuffer(uint8_t layerId, uint8_t bufferId, const LayerBuffer* layer)
{
    if ((layerId != LV_LAYER_1) || (bufferId >= g_layers[layerId].bufferCount)) {
        return;
    }
    g_layers[LV_LAYER_1].buffer[bufferId] = *layer;
}

void LvSetLayerColorKey(uint8_t layerId, bool enColorKey, uint32_t color)
{
    if ((layerId < LV_LAYER_0) || (layerId >= LV_LAYER_MAX)) {
        LV_LOGW("Invalid layerId[%u].\n", layerId);
        return;
    }

    if (g_layerFuncs->SetLayerColorKey(g_devId, g_layers[layerId].layerHdl, enColorKey, color) !=
        DISPLAY_SUCCESS) {
        LV_LOGE("SetLayerColorKey failed. LayerId: %u, enColorKey: %d.", layerId, enColorKey);
    }
}

void LvSetLayerPosition(uint8_t layerId, int16_t x, int16_t y)
{
    if ((layerId < LV_LAYER_0) || (layerId >= LV_LAYER_MAX)) {
        LV_LOGW("Invalid layerId[%u].\n", layerId);
        return;
    }

    if (g_layers[layerId].layerPosition.x == x && g_layers[layerId].layerPosition.y == y) {
        return;
    }

    g_layers[layerId].layerPosition.x = x;
    g_layers[layerId].layerPosition.y = y;
    IRect rect = {x, y, 0, 0};
    if (g_layerFuncs->SetLayerSize(g_devId, g_layers[layerId].layerHdl, &rect) != DISPLAY_SUCCESS) {
        LV_LOGE("SetLayerSize failed.");
    }
}

bool LvIsUlpsEnabled(void)
{
#if ENABLE_ULPS
    return true;
#else
    return false;
#endif
}

void LvDecExitUlpsCnt(void)
{
    g_exitUlpsCnt--;
}

void LvIncExitUlpsCnt(void)
{
    g_exitUlpsCnt++;
}

uint8_t LvGetExistUlpsCnt(void)
{
    return g_exitUlpsCnt;
}

void LvEnableAsyncMode(bool enable)
{
    if (g_isAsyncMode == enable) {
        return;
    }
    if (enable && (LvObtainSurfaceView() != NULL)) {
        return;
    }
    if (!enable && g_asyncInfo.hasPendingFrame) {
        LvRenderWaitDone();
    }
    g_isAsyncMode = enable;
}

bool LvIsAsyncMode(void)
{
    return g_isAsyncMode;
}