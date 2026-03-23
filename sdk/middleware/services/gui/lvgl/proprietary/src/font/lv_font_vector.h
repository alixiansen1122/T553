/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#ifndef LV_FONT_VECTOR_H
#define LV_FONT_VECTOR_H

#include "lv_conf.h"

#if LV_USE_FONT_VECTOR

#include "stdbool.h"
#include "lv_font.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    lv_font_glyph_dsc_t dsc;
    float scale;
    uint16_t cmdCount;
    uint16_t dataCount;
    uint8_t* cmd;
    float* data;
} LvGlyphPath;

bool LvInitFontVector(void);

bool LvDeinitFontVector(void);

bool LvRegisterFont(lv_font_t* font, const char* fontName, uint16_t size);

void LvFreeFont(lv_font_t* font);

void LvUnregisterFont(void);

bool LvGetGlyphPath(const lv_font_t* font, uint32_t unicode, LvGlyphPath* entry);

uint16_t LvGetVectorFontSize(const lv_font_t* font);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
#endif