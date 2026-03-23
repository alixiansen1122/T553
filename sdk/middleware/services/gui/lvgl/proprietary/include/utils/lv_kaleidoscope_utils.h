/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef LV_KALEIDOSCOPE_UTILS_H
#define LV_KALEIDOSCOPE_UTILS_H

#include <stdbool.h>
#include "lv_conf.h"
#include "lv_img_buf.h"
#include "lv_canvas_ext.h"

#if LV_USE_KALEIDOSCOPE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    lv_img_dsc_t dsc;
    uint32_t stride;
} LvImgDscExt;

typedef struct {
    uint32_t resId;
    LvImgDscExt* kaleidoscopeDsc;
    LvImgDscExt* partialDsc;
    LvPath sectorPath;
    LvPath imgPath;
    float thetaVal;
    float radius;
    lv_point_t point;
    lv_point_t startPoint;
    bool isInternalMem;
} LvKaleidoscopeInfo;

/**
 * @brief  Init a pointer to LvKaleidoscopeInfo.
 * @param  [in]  info    Pointer to LvKaleidoscopeInfo.
 * @param  [in]  resId    Img resId, which is returned by LoadImg API. It is the source img for kaleidoscope img.
 * @param  [in]  kaleidoscopeDsc    Pointer to kaleidoscopeDsc.
                                    If it is NULL, it will be created and managed internally.
                                    Otherwise, it will be used to store kaleidoscope img.
 * @return Return true if init successfully; and false otherwise.
 */
bool LvInitKaleidoscopeInfo(LvKaleidoscopeInfo* info, uint32_t resId, const LvImgDscExt* kaleidoscopeDsc);

/**
 * @brief  Update the kaleidoscope img of LvKaleidoscopeInfo by a rotate value.
 * @param  [in]  info    Pointer to LvKaleidoscopeInfo.
 * @param  [in]  rotate    Rotate value. Currently only the sign matters.
 * @return Return true if update successfully; and false otherwise.
 */
bool LvUpdateKaleidoscopeInfo(LvKaleidoscopeInfo* info, int16_t rotate);

/**
 * @brief  Update the address of kaleidoscope data, when init the LvKaleidoscopeInfo with an externally managed dsc.
 * @param  [in]  info    Pointer to LvKaleidoscopeInfo.
 * @param  [in]  addr    Address of kaleidoscope data.
 * @return Return true if update successfully; and false otherwise.
 */
bool LvUpdateKaleidoscopeDscAddr(LvKaleidoscopeInfo* info, const uint8_t* addr);

/**
 * @brief  Deinit a pointer to LvKaleidoscopeInfo.
 * @param  [in]  info    Pointer to LvKaleidoscopeInfo.
 */
void LvDeinitKaleidoscopeInfo(LvKaleidoscopeInfo* info);

/**
 * @brief  Obtain kaleidoscope img.
 * @param  [in]  info    Pointer to LvKaleidoscopeInfo.
 * @return Return pointer to kaleidoscope img.
 */
lv_img_dsc_t* LvGetKaleidoscopeDsc(LvKaleidoscopeInfo* info);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
#endif