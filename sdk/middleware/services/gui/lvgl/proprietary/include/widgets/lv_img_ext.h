/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef LV_IMG_EXT_H
#define LV_IMG_EXT_H

#include "lv_transform.h"
#include "lvgl.h"
#include "lv_img.h"
#include "lv_obj_class.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_obj_class_t lv_img_ext_class;

typedef struct {
    lv_img_t base;
    LvTransform* transform;
} LvImgExt;

/**
 * @brief  Create imgext obj.
 */
lv_obj_t* LvImgExtCreate(lv_obj_t* parent);

/**
 * @brief  Rotate obj.
 */
void LvImgExtRotate(lv_obj_t* obj, float angle, LvVector3 pivot1, LvVector3 pivot2);

/**
 * @brief  Scale obj.
 */
void LvImgExtScale(lv_obj_t* obj, LvVector3 scale, LvVector3 pivot);

/**
 * @brief  Translate obj.
 */
void LvImgExtTranslate(lv_obj_t* obj, LvVector3 translate);

/**
 * @brief  Set camera distance.
 */
void LvImgExtSetCamDistance(lv_obj_t* obj, float distance);

/**
 * @brief  Set camera position.
 */
void LvImgExtSetCamPosition(lv_obj_t* obj, LvVector2 pos);

/**
 * @brief  Set transform.
 */
void LvImgExtSetTransform(lv_obj_t* obj, const LvTransform* transform);

/**
 * @brief  Get transform.
 */
LvTransform* LvImgExtGetTransform(lv_obj_t* obj);

/**
 * @brief  Set the image data to display on the object.
 * @param  [in]  obj    Pointer to an image object
 * @param  [in]  resId   resource id (look lv_img_cache_manager.h)
 */
bool LvImgSetSrcById(lv_obj_t* obj, uint32_t resId);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_IMG_EXT_H */
