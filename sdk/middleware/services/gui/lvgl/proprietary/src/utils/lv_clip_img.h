/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef LV_CLIP_IMG_H
#define LV_CLIP_IMG_H

#include "lv_conf.h"
#include "lv_canvas_ext.h"
#include "lv_img_buf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Clip img by closed path
 * @param  [in]  dst    Pointer to dst lv_img_dsc_t.
 * @param  [in]  src    Pointer to src lv_img_dsc_t.
 * @param  [in]  path    Path.
 * @param  [in]  pathMatrix    Path matrix.
 * @param  [in]  imgMatrix    Img matrix.
 * @return Return true if clip the img successfully; and false otherwise.
 */
bool LvClipImgByPath(lv_img_dsc_t* dst, lv_img_dsc_t* src, const LvPath* path,
    const LvMatrix* pathMatrix, const LvMatrix* imgMatrix);

/**
 * @brief  Clip img by closed path and a given rect.
 * @param  [in]  dst    Pointer to dst lv_img_dsc_t.
 * @param  [in]  src    Pointer to src lv_img_dsc_t.
 * @param  [in]  path    Path.
 * @param  [in]  pathMatrix    Path matrix.
 * @param  [in]  imgMatrix    Img matrix.
 * @param  [in]  srcArea    Src area.
 * @return Return true if clip the img successfully; and false otherwise.
 */
bool LvClipImgByPathAndRect(lv_img_dsc_t* dst, lv_img_dsc_t* src, const LvPath* path,
    const LvMatrix* pathMatrix, const LvMatrix* imgMatrix, const lv_area_t* srcArea);

/**
 * @brief  Clip img by circle.
 * @param  [in]  src    Pointer to lv_img_dsc_t.
 * @param  [in]  center    Center of circle.
 * @param  [in]  radius    Radius of circle.
 * @return Return a clipped img.
 */
lv_img_dsc_t* LvClipImgByCircle(lv_img_dsc_t* src, lv_point_t center, uint16_t radius);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
