/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "lv_demo_load_scr.h"
#include "lv_scr_load_anim.h"
#include "lv_demo_ext.h"

#if LV_USE_DEMO_LOAD_SCR

lv_obj_t* g_oriScr = NULL;
lv_obj_t* g_newScr = NULL;
lv_obj_t* g_loadBtn = NULL;
lv_font_t* g_font = NULL;
static const uint32_t ANIM_TIME = 500;
static const uint32_t ANIM_DELAY = 10;
static const lv_coord_t LABEL_W = 80;
static const lv_coord_t LABEL_H = 60;

#define ANIM_TYPE_NUM 2
static const char* g_animTypeStr[ANIM_TYPE_NUM] = {"Scale", "Flip"};
static uint8_t g_selectedAnimIndex = 0;
static lv_obj_t* g_animTypeBtn = NULL;
static lv_obj_t* g_animTypeLabel = NULL;
static bool g_isInited = false;

static void UnloadCb(lv_event_t * e)
{
    if (g_selectedAnimIndex == 0) {
        LvScrLoadAnimExt(g_oriScr, LV_SCR_LOAD_ANIM_OUT_SCALE, ANIM_TIME, ANIM_DELAY, false, (void*)&g_loadBtn->coords);
    } else {
        LvScrLoadAnimExt(g_oriScr, LV_SCR_LOAD_ANIM_FLIP, ANIM_TIME, ANIM_DELAY, false, NULL);
    }
}

static void AddScaledImg(lv_obj_t* par)
{
    lv_obj_t* img = LvImgExtCreate(par);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
    LvImgSetSrcExt(img, "/user/res/A094_051_8888_hfbc.bin");
    LvImgExtScale(img, (LvVector3){1.5f, 1.5f, 1.0f}, (LvVector3){110.0f, 110.0f, 0.0f});
}

static lv_obj_t* CreateNewScr(void)
{
    lv_obj_t* scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
    lv_obj_set_style_radius(scr, 0, 0);
    lv_obj_set_style_border_width(scr, 0, 0);
    lv_obj_add_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    AddScaledImg(scr);

    lv_obj_t* backBtn = lv_btn_create(scr);
    lv_obj_set_size(backBtn, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(backBtn, LV_ALIGN_BOTTOM_MID, 0, -20); // -20: y offset
    lv_obj_add_event_cb(backBtn, UnloadCb, LV_EVENT_RELEASED, NULL);
    lv_obj_set_style_radius(backBtn, 0, 0);
    lv_obj_set_style_border_width(backBtn, 0, 0);

    lv_obj_t* backLabel = lv_label_create(backBtn);
    lv_obj_set_size(backLabel, LABEL_W, LABEL_H);
    lv_label_set_text(backLabel, "Back");
    lv_obj_set_style_text_color(backLabel, lv_color_white(), 0);
    lv_obj_set_style_text_align(backLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(backLabel, g_font, 0);
    return scr;
}

static void LoadCb(lv_event_t * e)
{
    lv_obj_t* obj = (lv_obj_t*)lv_event_get_target(e);
    if (g_selectedAnimIndex == 0) {
        LvScrLoadAnimExt(g_newScr, LV_SCR_LOAD_ANIM_OVER_SCALE, ANIM_TIME, ANIM_DELAY, false, (void*)&obj->coords);
    } else {
        LvScrLoadAnimExt(g_newScr, LV_SCR_LOAD_ANIM_FLIP, ANIM_TIME, ANIM_DELAY, false, NULL);
    }
}

static void ChangeAnimCb(lv_event_t * e)
{
    lv_obj_t* obj = (lv_obj_t*)lv_event_get_target(e);
    g_selectedAnimIndex++;
    if (g_selectedAnimIndex >= ANIM_TYPE_NUM) {
        g_selectedAnimIndex = 0;
    }
    lv_obj_t* label = (lv_obj_t*)lv_event_get_user_data(e);
    lv_label_set_text(label, g_animTypeStr[g_selectedAnimIndex]);
    lv_obj_invalidate(label);
}

static void InitOrigScr(lv_obj_t* par)
{
    lv_obj_set_style_bg_color(par, lv_color_black(), 0);
    lv_obj_set_style_radius(par, 0, 0);
    lv_obj_set_style_border_width(par, 0, 0);

    g_loadBtn = lv_btn_create(par);
    lv_obj_set_size(g_loadBtn, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_center(g_loadBtn);
    lv_obj_add_event_cb(g_loadBtn, LoadCb, LV_EVENT_RELEASED, NULL);
    lv_obj_set_style_radius(g_loadBtn, 0, 0);
    lv_obj_set_style_border_width(g_loadBtn, 0, 0);

    lv_obj_t* loadLabel = lv_label_create(g_loadBtn);
    lv_obj_set_size(loadLabel, LABEL_W, LABEL_H);
    lv_label_set_text(loadLabel, "Load");
    lv_obj_set_style_text_color(loadLabel, lv_color_white(), 0);
    lv_obj_set_style_text_align(loadLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(loadLabel, g_font, 0);

    g_animTypeBtn = lv_btn_create(par);
    lv_obj_set_size(g_animTypeBtn, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align_to(g_animTypeBtn, g_loadBtn, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 50); // 50: offset
    lv_obj_set_style_radius(g_animTypeBtn, 0, 0);
    lv_obj_set_style_border_width(g_animTypeBtn, 0, 0);

    g_animTypeLabel = lv_label_create(g_animTypeBtn);
    lv_obj_set_size(g_animTypeLabel, LABEL_W, LABEL_H);
    lv_label_set_text(g_animTypeLabel, g_animTypeStr[g_selectedAnimIndex]);
    lv_obj_set_style_text_color(g_animTypeLabel, lv_color_white(), 0);
    lv_obj_set_style_text_align(g_animTypeLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(g_animTypeLabel, g_font, 0);

    lv_obj_add_event_cb(g_animTypeBtn, ChangeAnimCb, LV_EVENT_RELEASED, g_animTypeLabel);
}

void LvDemoLoadScr(lv_obj_t* par)
{
    LvInitFontExt();
    g_font = LvFontLoadExt(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: font size

    g_oriScr = lv_obj_get_screen(par);
    InitOrigScr(par);

    g_newScr = CreateNewScr();
}

static void Init(lv_obj_t* par)
{
    if (g_isInited) {
        return;
    }
    g_isInited = true;
    LvDemoLoadScr(par);
}

static void Deinit(lv_obj_t* par)
{
    if (!g_isInited) {
        return;
    }
    lv_obj_clean(par);
    lv_obj_del(g_newScr);
    g_newScr = NULL;
    g_oriScr = NULL;
    LvFontFreeExt(g_font);
    g_font = NULL;
    g_selectedAnimIndex = 0;
    g_isInited = false;
}

static LvTestCaseInfo g_testCaseInfo = {
    "LoadScr",
    Init,
    Deinit,
};

void LvRegisterDemoLoadScr(void)
{
    RegisterTestCase(&g_testCaseInfo);
}
#endif