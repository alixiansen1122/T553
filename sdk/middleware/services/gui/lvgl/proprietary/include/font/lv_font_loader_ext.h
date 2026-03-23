/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#ifndef LV_FONT_LOADER_EXT_H
#define LV_FONT_LOADER_EXT_H

#include "lv_conf.h"

#if LV_USE_FONT_VECTOR

#include "src/font/lv_font.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Init the extended font module. This function should be called before calling other functions.
 * @return Returns true if init the module successfully; otherwise, returns false.
 */
bool LvInitFontExt(void);

/**
 * @brief  Deinit the extended font module.
 * @return Returns true if deinit the module successfully; otherwise, returns false.
 */
bool LvDeinitFontExt(void);

/**
 * @brief  Loads a lv_font_t object from a ttf font file and a given font size.
 *         User can only load single font file in the context, but different sizes can be speicfied.
 * @param  [in]  fontPath    Path of a ttf font file.
 * @param  [in]  size    Font size.
 * @return A pointer to the font or NULL in case of error.
 */
lv_font_t* LvFontLoadExt(const char* fontPath, uint16_t size);

/**
 * @brief  Acquire the font size of a lv_font_t object.
 * @param  [in]  font    A lv_font_t object created by the load function.
 * @return The font size of the lv_font_t object.
 */
uint16_t LvFontGetSize(const lv_font_t* font);

/**
 * @brief  Unload the previously registered ttf font file in the context.
 *         This function should be called before loading other ttf font file.
 */
void LvFontUnloadExt(void);

/**
 * @brief  Frees the memory allocated by the load function.
 * @param  [in]  font    A lv_font_t object created by the load function.
 */
void LvFontFreeExt(lv_font_t* font);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
#endif /* LV_FONT_LOADER_EXT_H */