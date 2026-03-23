/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#ifndef LV_DISPLAY_DEVICE_H
#define LV_DISPLAY_DEVICE_H

#include <stdint.h>
#include "display_type.h"
#include "misc/lv_area.h"
#include "lv_hal_disp.h"
#include "lv_obj.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LV_LAYER_0 = 0,
    LV_LAYER_1,
    LV_LAYER_MAX,
} LvLayerId;

typedef enum {
    LV_BUFFER_0 = 0,
    LV_BUFFER_1,
    LV_BUFFER_MAX,
} LvBufferId;

bool LvInitDisplayDevice(lv_coord_t width, lv_coord_t height);

void LvCloseDisplayDevice(void);

bool LvOpenLayer(LvLayerId layerId, LayerInfo* info, uint8_t bufCount);

bool LvAllocLayerBuffer(LvLayerId layerId);

void LvFreeLayerBuffer(LvLayerId layerId);

void LvFlush(LvLayerId layerId);

void LvWaitForVBlank(void);

void LvSetLayerDirtyRegion(LvLayerId layerId, const lv_area_t* dirtyArea);

void LvCloseLayer(LvLayerId layerId);

bool LvSwapBuffer(LvLayerId layerId);

const LayerBuffer* LvGetLayerBuffer(LvLayerId layerId, uint8_t* bufCount);

void LvSetHardwareLayerBuffer(uint8_t layerId, uint8_t bufferId, const LayerBuffer* layer);

void LvSetLayerColorKey(uint8_t layerId, bool enColorKey, uint32_t color);

void LvSetLayerPosition(uint8_t layerId, int16_t x, int16_t y);

void LvAsyncFlush(void);

void LvFlushHandler(lv_disp_drv_t* dispDrv, const lv_area_t* area);

bool LvIsUlpsEnabled(void);

void LvDecExitUlpsCnt(void);

void LvIncExitUlpsCnt(void);

uint8_t LvGetExistUlpsCnt(void);

void LvEnableAsyncMode(bool enable);

bool LvIsAsyncMode(void);

bool LvHasPendingFrame(void);

#if LV_USE_SURFACE_VIEW
void LvSetSurfaceView(lv_obj_t* obj);

lv_obj_t* LvObtainSurfaceView(void);

void LvAsyncInvalidateSurfaceView(void);

void LvPreprocessSurfaceView(void);
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif