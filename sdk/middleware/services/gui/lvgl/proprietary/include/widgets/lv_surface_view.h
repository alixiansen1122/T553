/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#ifndef LV_SURFACE_VIEW_H
#define LV_SURFACE_VIEW_H

#include "lv_conf.h"
#include "lvgl.h"
#include "lv_mutex.h"
#include "misc/lv_color.h"

#if LV_USE_SURFACE_VIEW

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t width;
    int32_t height;
    uint32_t stride;
    uint64_t addr;
    uint32_t size;
    lv_img_cf_t format;
} LvSurfaceBuffer;

typedef struct {
    lv_obj_t obj;
    lv_ll_t bufList;
    lv_ll_t freeList;
    lv_ll_t dirtyList;
    LvSurfaceBuffer* lastFlushedBuf;
    LvSurfaceBuffer* curBuf;
    uint8_t flushCount;
    uint32_t bufSize;
    lv_color_t color;
    bool isInited;
    bool isLayerInited;
    bool isStopped;
    LvMutex bufMutex;
    LvCondition bufCond;
    LvMutex surMutex;
} LvSurfaceView;

/**
 * @brief  Create a surface view object.
 * @param  [in]  parent    Pointer to a parent object.
 * @return Pointer to the created object.
 */
lv_obj_t* LvSurfaceViewCreate(lv_obj_t* parent);

/**
 * @brief  User should call this function once to init the surface view before using any other methods.
 *         This will set object's size and position,
 *         and user should not call generic object functions, such as lv_obj_set_pos or lv_obj_set_size and etc.
 * @param  [in]  obj    Pointer to a surface view object.
 * @param  [in]  coords    Coordinate.
 * @param  [in]  w    Width.
 * @param  [in]  h    Height.
 * @return Returns true if init successfully; and returns false otherwise.
 */
bool LvInitSurfaceView(lv_obj_t* obj, lv_point_t coords, int32_t w, int32_t h);

/**
 * @brief  Set a color used for colorkey.
 * @param  [in]  obj    Pointer to a surface view object.
 * @param  [in]  color    Color used for colorkey.
 * @return Returns true if set color successfully; and false otherwise.
 */
bool LvSetSurfaceColorkey(lv_obj_t* obj, lv_color_t color);

/**
 * @brief  For producer to obtain a buffer to write data.
 * @param  [in]  obj    Pointer to a surface view object.
 * @param  [in]  wait    Currently used to decide whether to wait until a buffer available.
 *                       If gonna wait, this should be set to a valid value rather than 0.
 * @return Returns the pointer to a buffer if the operation is successful; returns NULL otherwise.
 */
LvSurfaceBuffer* LvRequestBuffer(lv_obj_t* obj, uint8_t wait);

/**
 * @brief  For producer to append a dirty buffer to dirty list waiting for being flushed.
 * @param  [in]  obj    Pointer to a surface view object.
 * @param  [in]  buffer    Pointer to a dirty buffer.
 */
void LvFlushBuffer(lv_obj_t* obj, LvSurfaceBuffer* buffer);

/**
 * @brief  For producer to return a previously requested buffer which has not been changed.
 * @param  [in]  obj    Pointer to a surface view object.
 * @param  [in]  buffer    Pointer to a previously requested buffer.
 */
void LvCancelBuffer(lv_obj_t* obj, LvSurfaceBuffer* buffer);

/**
 * @brief  For consumer to flush a dirty buffer.
 * @param  [in]  obj    Pointer to a surface view object.
 */
void LvFlushSurfaceView(lv_obj_t* obj);

/**
 * @brief  For consumer to call after a frame flushed.
 * @param  [in]  obj    Pointer to a surface view object.
 */
void LvSurfacePostFlush(lv_obj_t* obj);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
#endif