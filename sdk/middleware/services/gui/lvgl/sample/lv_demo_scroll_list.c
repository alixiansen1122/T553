/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "lv_demo_scroll_list.h"
#if LV_USE_FONT_VECTOR
#include "lv_font_vector.h"
#endif
#include "lv_demo_ext.h"

#if LV_USE_DEMO_SCROLL_TEXT

static lv_font_t* g_font36 = NULL;
static int8_t g_count = 0;
static int8_t g_scroll = -4;
static const uint16_t VECTOR_FONT_SIZE = 36;
static const uint16_t ANI_TIME = 1000;
static const char* DEFAULT_SRC = "/user/res/haha.bin";
static bool g_isInited = false;

static void ScrollCb(void* var, int32_t v)
{
    g_count++;
    if (g_count == 50) { // 50: limit
        g_count = 0;
        g_scroll *= -1;
    }
    lv_obj_scroll_by(var, 0, g_scroll, false);
}

static lv_obj_t* AddProfile(lv_obj_t* par, lv_obj_t* ref, lv_align_t align)
{
    lv_obj_t* img = lv_img_create(par);
    lv_obj_align_to(img, ref, align, 0, 50); // 50: offset
    LvImgSetSrcExt(img, DEFAULT_SRC);

    lv_obj_t* label = lv_label_create(par);
    lv_obj_align_to(label, img, LV_ALIGN_OUT_RIGHT_MID, 20, 0); // 20: offset
    lv_label_set_text(label, "Name");
#if LV_USE_FONT_VECTOR
    lv_obj_set_style_text_font(label, g_font36, 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
#endif

    return img;
}

void LvDemoScrollList(lv_obj_t* par)
{
    lv_obj_set_style_border_opa(par, 0, 0);
    lv_obj_set_style_radius(par, 0, 0);
    lv_obj_set_scrollbar_mode(par, LV_SCROLLBAR_MODE_OFF);

    lv_obj_t* img = lv_img_create(par);
    lv_obj_align(img, LV_ALIGN_TOP_MID, -40, 50); // -40: X offset, 50: y offset
    LvImgSetSrcExt(img, DEFAULT_SRC);

    lv_obj_t* label = lv_label_create(par);
    lv_obj_align_to(label, img, LV_ALIGN_OUT_RIGHT_MID, 20, 0); // 20: offset
    lv_label_set_text(label, "Name");
#if LV_USE_FONT_VECTOR
    LvInitFontExt();
    g_font36 = LvFontLoadExt(DEFAULT_VECTOR_FONT_FILENAME, VECTOR_FONT_SIZE);
    lv_obj_set_style_text_font(label, g_font36, 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
#endif

    lv_obj_t* prev = img;

    for (int i = 0; i < 30; i++) { // 30: size
        prev = AddProfile(par, prev, LV_ALIGN_OUT_BOTTOM_LEFT);
    }

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&a, ScrollCb);
    lv_anim_set_var(&a, par);
    lv_anim_set_time(&a, ANI_TIME);
    lv_anim_set_playback_time(&a, ANI_TIME);
    lv_anim_start(&a);
}

static void Init(lv_obj_t* par)
{
    if (g_isInited) {
        return;
    }
    g_isInited = true;
    LvDemoScrollList(par);
}

static void Deinit(lv_obj_t* par)
{
    if (!g_isInited) {
        return;
    }
    lv_obj_clean(par);
    lv_anim_del(par, NULL);
#if LV_USE_FONT_VECTOR
    LvFontFreeExt(g_font36);
    g_font36 = NULL;
#endif
    g_count = 0;
    g_scroll = -4; // -4: default value
    g_isInited = false;
}

static LvTestCaseInfo g_testCaseInfo = {
    "Scroll List",
    Init,
    Deinit,
};

void LvRegisterDemoScrollList(void)
{
    RegisterTestCase(&g_testCaseInfo);
}
#endif