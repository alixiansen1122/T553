/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef LV_IMG_CACHE_EXT_H
#define LV_IMG_CACHE_EXT_H

#include "lv_conf.h"
#include "misc/lv_color.h"
#include "draw/lv_img_cache.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LV_USE_IMG_CACHE_EXT

/**
 * @brief  Open an image using the image decoder interface and cache it by a two-level LRU rule.
 *         The image will be left open meaning if the image decoder open callback allocated memory then it will remain.
 *         The image is closed if a new image is opened and the new image takes its place in the cache.
 *         When full entries or out of memory, short-term cached entry will be firstly removed than long-term one.
 * @param  [in]  src    source of the image. Path to file or pointer to an `lv_img_dsc_t` variable
 * @param  [in]  color    The color of the image with `LV_IMG_CF_ALPHA_...`
 * @param  [in]  frameId    The index of the frame. Used only with animated images, set 0 for normal images
 * @param  [in]  isLongTermCached    Whether the image will be cached in long-term.
 * @return Pointer to the cached _lv_img_cache_entry_t or NULL if failed.
 */
_lv_img_cache_entry_t* LvImgCacheOpen(const void* src, lv_color_t color, int32_t frameId, bool isLongTermCached);

/**
 * @brief  Set the number of images to be cached.
 * @param  [in]  size    total size of cache enties.
 */
void LvImgCacheSetSize(uint16_t size);

/**
 * @brief  Invalidate an image source in the cache.
 *         Useful if the image source is updated therefore it needs to be cached again.
 * @param  [in]  src    an image source path to a file or pointer to an `lv_img_dsc_t` variable.
 */
void LvImgCacheInvalidateSrc(const void* src);

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_IMG_CACHE_EXT_H */
