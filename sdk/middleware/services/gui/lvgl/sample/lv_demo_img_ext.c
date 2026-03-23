/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "lv_demo_img_ext.h"
#include "lv_img_ext.h"
#include "lv_demo_ext.h"

#if LV_USE_DEMO_IMG_EXT

static lv_obj_t* g_incBtn = NULL;
static lv_obj_t* g_decBtn = NULL;
static lv_obj_t* g_img = NULL;
static float g_angle = 0.0f;
static bool g_isInited = false;
static lv_font_t* g_font = NULL;

static void EventCb(lv_event_t * e)
{
    lv_obj_t* obj = lv_event_get_target(e);
    if (lv_event_get_code(e) == LV_EVENT_PRESSING) {
        if (obj == g_incBtn) {
            g_angle += 2.0f;
        } else if (obj == g_decBtn) {
            g_angle -= 2.0f;
        }
        if (g_angle > 360.0f) {
            g_angle = g_angle - 360.0f;
        } else if (g_angle < 0.0f) {
            g_angle = 360.0f - g_angle;
        }
        LvImgExtRotate(g_img, g_angle, (LvVector3){0.0f, 0.0f, 0.0f}, (LvVector3){220.0f, 220.0f, 1.0f});
    }
}

void LvDemoImgExt(lv_obj_t* par)
{
    g_img = LvImgExtCreate(par);
    lv_obj_align(g_img, LV_ALIGN_CENTER, 0, 0);
    LvImgSetSrcExt(g_img, "/user/res/A094_051_8888_hfbc.bin");

    LvImgExtScale(g_img, (LvVector3){1.5f, 1.5f, 1.0f}, (LvVector3){110.0f, 110.0f, 0.0f});

    LvInitFontExt();
    g_font = LvFontLoadExt(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: font size

    g_incBtn = lv_label_create(par);
    lv_obj_set_size(g_incBtn, 227, 100); // 227: width, 100: height
    lv_obj_set_pos(g_incBtn, 0, 350); // 350: y
    lv_obj_add_flag(g_incBtn, LV_OBJ_FLAG_CLICKABLE);
    lv_label_set_text(g_incBtn, "+ DEG");
    lv_obj_set_style_text_color(g_incBtn, lv_palette_main(LV_PALETTE_DEEP_ORANGE), 0);
    lv_obj_set_style_text_align(g_incBtn, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(g_incBtn, g_font, 0);
    lv_obj_add_event_cb(g_incBtn, EventCb, LV_EVENT_ALL, NULL);

    g_decBtn = lv_label_create(par);
    lv_obj_set_size(g_decBtn, 227, 100); // 227: width, 100: height
    lv_obj_set_pos(g_decBtn, 227, 350); // 227: x, 350: y
    lv_obj_add_flag(g_decBtn, LV_OBJ_FLAG_CLICKABLE);
    lv_label_set_text(g_decBtn, "- DEG");
    lv_obj_set_style_text_color(g_decBtn, lv_palette_main(LV_PALETTE_DEEP_ORANGE), 0);
    lv_obj_set_style_text_align(g_decBtn, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(g_decBtn, g_font, 0);
    lv_obj_add_event_cb(g_decBtn, EventCb, LV_EVENT_ALL, NULL);
}

static void Init(lv_obj_t* par)
{
    if (g_isInited) {
        return;
    }
    g_isInited = true;
    LvDemoImgExt(par);
}

static void Deinit(lv_obj_t* par)
{
    if (!g_isInited) {
        return;
    }
    lv_obj_clean(par);
    LvFontFreeExt(g_font);
    g_font = NULL;
    g_isInited = false;
    g_incBtn = NULL;
    g_decBtn = NULL;
    g_img = NULL;
    g_angle = 0.0f;
}

static LvTestCaseInfo g_testCaseInfo = {
    "ImgExt",
    Init,
    Deinit,
};

void LvRegisterDemoImgExt(void)
{
    RegisterTestCase(&g_testCaseInfo);
}
#endif
