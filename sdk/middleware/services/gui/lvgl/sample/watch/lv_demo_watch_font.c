/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "lv_demo_watch_font.h"

#if LV_USE_DEMO_WATCH

lv_font_t* g_font14 = NULL;
lv_font_t* g_font16 = NULL;
lv_font_t* g_font22 = NULL;
lv_font_t* g_font32 = NULL;

void InitLvFontRes()
{
#if LV_USE_FONT_VECTOR
    LvInitFontExt();
    g_font32 = LvFontLoadExt(DEFAULT_VECTOR_FONT_FILENAME, 32); // 32: size
    g_font22 = LvFontLoadExt(DEFAULT_VECTOR_FONT_FILENAME, 22); // 22: size
    g_font16 = LvFontLoadExt(DEFAULT_VECTOR_FONT_FILENAME, 16); // 16: size
    g_font14 = LvFontLoadExt(DEFAULT_VECTOR_FONT_FILENAME, 14); // 14: size
#else
    g_font32 = &lv_font_montserrat_32;
    g_font22 = &lv_font_montserrat_22;
    g_font16 = &lv_font_montserrat_16;
    g_font14 = &lv_font_montserrat_14;
#endif
}

void DeinitLvFontRes()
{
#if LV_USE_FONT_VECTOR
    LvFontFreeExt(g_font14);
    g_font14 = NULL;
    LvFontFreeExt(g_font16);
    g_font16 = NULL;
    LvFontFreeExt(g_font22);
    g_font22 = NULL;
    LvFontFreeExt(g_font32);
    g_font32 = NULL;

    LvFontUnloadExt();
#endif
}

lv_font_t* GetLvFont(uint8_t size)
{
    switch (size) {
        case 14: // 14: size
            return g_font14;
        case 16: // 16: size
            return g_font16;
        case 22: // 22: size
            return g_font22;
        case 32: // 32: size
            return g_font32;
        default:
            return NULL;
    }
}
#endif