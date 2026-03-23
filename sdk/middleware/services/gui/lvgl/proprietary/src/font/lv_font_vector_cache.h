/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#ifndef LV_FONT_VECTOR_CACHE_H
#define LV_FONT_VECTOR_CACHE_H

#include "lv_conf.h"

#if LV_USE_FONT_VECTOR
#include "lv_font_vector.h"

#ifdef __cplusplus
extern "C" {
#endif

void LvInitFontVectorCache(void);
LvGlyphPath* LvGetFontVectorCacheSpace(uint32_t unicode, uint32_t cmdSize, uint32_t dataSize);
LvGlyphPath* LvGetFontVectorCachedData(uint32_t unicode);
void LvClearFontVectorCache(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
#endif