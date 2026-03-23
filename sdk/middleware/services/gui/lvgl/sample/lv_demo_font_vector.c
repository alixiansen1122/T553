/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#include "lv_demo_font_vector.h"
#include "lv_demo_ext.h"

#if LV_USE_DEMO_FONT_VECTOR

static lv_font_t* g_font28 = NULL;
static lv_font_t* g_font35 = NULL;
static lv_font_t* g_font38 = NULL;
static lv_font_t* g_font40 = NULL;
static lv_coord_t g_width = 150;
static bool g_isInited = false;

static void SetUpSingleLabel(lv_obj_t* par, lv_font_t* font, lv_palette_t color, uint8_t opa, lv_text_align_t textAlign)
{
    lv_obj_t* label = lv_label_create(par);
    lv_obj_set_size(label, g_width, LV_SIZE_CONTENT);

    lv_label_set_text(label, "Test Vector!");
    lv_obj_set_style_text_color(label, lv_palette_main(color), 0);
    lv_obj_set_style_text_opa(label, opa, 0);
    lv_obj_set_style_text_align(label, textAlign, 0);
    lv_obj_set_style_bg_color(label, lv_palette_main(LV_PALETTE_DEEP_PURPLE), 0);
    lv_obj_set_style_bg_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(label, font, 0);
}

static void SetUpLabelRow(lv_obj_t* par, lv_font_t* font, lv_palette_t color, uint8_t opa)
{
    lv_text_align_t textAligns[2] = {LV_TEXT_ALIGN_CENTER, LV_TEXT_ALIGN_LEFT};
    for (uint8_t i = 0; i < 2; i++) { // 2: col size
        SetUpSingleLabel(par, font, color, opa, textAligns[i]);
    }
}

void LvDemoFontVector(lv_obj_t* par)
{
    lv_obj_set_style_pad_all(par, 50, 0); // 50: pad
    lv_obj_set_flex_flow(par, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(par, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_base_dir(par, LV_BASE_DIR_RTL, 0);

    lv_obj_set_style_bg_opa(par, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(par, lv_palette_main(LV_PALETTE_GREY), 0);

    g_font28 = LvFontLoadExt(DEFAULT_VECTOR_FONT_FILENAME, 28); // 28: font size
    g_font35 = LvFontLoadExt(DEFAULT_VECTOR_FONT_FILENAME, 35); // 35: font size
    g_font38 = LvFontLoadExt(DEFAULT_VECTOR_FONT_FILENAME, 38); // 38: font size
    g_font40 = LvFontLoadExt(DEFAULT_VECTOR_FONT_FILENAME, 40); // 40: font size

    lv_obj_t* ref = par;
    lv_palette_t colors[4] = {LV_PALETTE_RED, LV_PALETTE_RED, LV_PALETTE_BLUE, LV_PALETTE_BLUE};
    uint8_t opa[4] = {LV_OPA_COVER, LV_OPA_20, LV_OPA_COVER, LV_OPA_20};
    lv_font_t* fonts[4] = {g_font28, g_font35, g_font38, g_font40};

    for (int i = 0; i < 4; i++) { // 4: row len
        SetUpLabelRow(par, fonts[i], colors[i], opa[i]);
    }
}

static void Init(lv_obj_t* par)
{
    if (g_isInited) {
        return;
    }
    g_isInited = true;
    LvDemoFontVector(par);
}

static void Deinit(lv_obj_t* par)
{
    if (!g_isInited) {
        return;
    }
    lv_obj_clean(par);

    LvFontFreeExt(g_font28);
    LvFontFreeExt(g_font35);
    LvFontFreeExt(g_font38);
    LvFontFreeExt(g_font40);
    g_font28 = NULL;
    g_font35 = NULL;
    g_font38 = NULL;
    g_font40 = NULL;
    g_isInited = false;
}

static LvTestCaseInfo g_testCaseInfo = {
    "FontVector",
    Init,
    Deinit,
};

void LvRegisterDemoFontVector(void)
{
    RegisterTestCase(&g_testCaseInfo);
}
#endif
