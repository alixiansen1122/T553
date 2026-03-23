/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */
#ifndef LV_IMG_CACHE_MANAGER_INTERNAL_H
#define LV_IMG_CACHE_MANAGER_INTERNAL_H
/*********************
 *      INCLUDES
 *********************/

#include "lvgl.h"
#include "draw/lv_img_cache.h"

#ifdef __cplusplus
extern "C" {
#endif
bool GetImgCacheEntry(uint32_t resId, _lv_img_cache_entry_t* cacheEntry);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif