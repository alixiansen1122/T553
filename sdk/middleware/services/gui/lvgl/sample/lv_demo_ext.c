/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "lv_demo_ext.h"
#include "lv_conf_ext.h"
#include "lv_ll.h"

#if LV_USE_DEMO_FONT_VECTOR
#include "lv_demo_font_vector.h"
#endif
#if LV_USE_DEMO_RECT
#include "lv_demo_rect.h"
#endif
#if LV_USE_DEMO_IMG
#include "lv_demo_img.h"
#endif
#if LV_USE_DEMO_IMG_EXT
#include "lv_demo_img_ext.h"
#endif
#if LV_USE_DEMO_COVERFLOW
#include "lv_demo_coverflow.h"
#endif
#if LV_USE_DEMO_LOAD_SCR
#include "lv_demo_load_scr.h"
#endif
#if LV_USE_DEMO_KALEIDOSCOPE
#include "lv_demo_kaleidoscope.h"
#endif
#if LV_USE_DEMO_RAINBOW
#include "lv_demo_rainbow.h"
#endif
#if LV_USE_DEMO_SCROLL_LIST
#include "lv_demo_scroll_list.h"
#endif
#if LV_USE_DEMO_SCROLL_TEXT
#include "lv_demo_scroll_text.h"
#endif
#if LV_USE_DEMO_ZOOM_IMG
#include "lv_demo_zoom_img.h"
#endif
#if LV_USE_DEMO_BARCODE
#include "lv_demo_barcode.h"
#endif
#if LV_USE_DEMO_IMG_CACHE_MANAGER
#include "lv_demo_img_cache_manager.h"
#endif

static lv_ll_t g_testCaseLL = {0};
static lv_obj_t* g_mainPage = NULL;
static lv_obj_t* g_mainScr = NULL;
static lv_obj_t* g_backBtn = NULL;
static lv_font_t* g_font = NULL;

typedef struct {
    lv_obj_t* page;
    LvTestCaseInfo info;
} LvTestPageInfo;

static void PrepareTestCase(void)
{
    _lv_ll_init(&g_testCaseLL, sizeof(LvTestPageInfo));
#if LV_USE_DEMO_BARCODE
    LvRegisterDemoBarcode();
#endif
#if LV_USE_DEMO_IMG_CACHE_MANAGER
    LvRegisterDemoImgCacheManager();
#endif
#if LV_USE_DEMO_RAINBOW
    LvRegisterDemoRainbow();
#endif
#if LV_USE_DEMO_KALEIDOSCOPE
    LvRegisterDemoKaleidoscope();
#endif
#if LV_USE_DEMO_COVERFLOW
    LvRegisterDemoCoverFlow();
#endif
#if LV_USE_DEMO_LOAD_SCR
    LvRegisterDemoLoadScr();
#endif
#if LV_USE_DEMO_CANVAS_EXT
    LvRegisterDemoCanvasExt();
#endif
#if LV_USE_DEMO_ZOOM_IMG
    LvRegisterDemoZoomImg();
#endif
#if LV_USE_DEMO_SCROLL_LIST
    LvRegisterDemoScrollList();
#endif
#if LV_USE_DEMO_SCROLL_TEXT
    LvRegisterDemoScrollText();
#endif
#if LV_USE_DEMO_IMG_EXT
    LvRegisterDemoImgExt();
#endif
#if LV_USE_DEMO_IMG
    LvRegisterDemoImg();
#endif
#if LV_USE_DEMO_RECT
    LvRegisterDemoRect();
#endif
#if LV_USE_DEMO_FONT_VECTOR
    LvRegisterDemoFontVector();
#endif
}

static void UnloadPage(lv_event_t * e)
{
    lv_obj_t* obj = lv_event_get_target(e);
    LvTestPageInfo* node = lv_event_get_user_data(e);
    lv_obj_add_flag(g_backBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_event_cb(g_backBtn, NULL);
    lv_scr_load(g_mainScr);
    if (node->info.Deinit != NULL) {
        node->info.Deinit(node->page);
    }
}

static void LoadPage(lv_event_t * e)
{
    LvTestPageInfo* node = lv_event_get_user_data(e);
    lv_scr_load(node->page);
    if (node->info.Init != NULL) {
        node->info.Init(node->page);
    }
    lv_obj_clear_flag(g_backBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(g_backBtn, UnloadPage, LV_EVENT_CLICKED, node);
}

void RegisterTestCase(LvTestCaseInfo* info)
{
    LvTestPageInfo* node = _lv_ll_ins_tail(&g_testCaseLL);
    memcpy_s(&node->info, sizeof(LvTestCaseInfo), info, sizeof(LvTestCaseInfo));

    node->page = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(node->page, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(node->page, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(node->page, 0, 0);
    lv_obj_set_style_radius(node->page, 0, 0);

    lv_obj_t* label = lv_label_create(g_mainPage);
    lv_obj_set_size(label, LV_HOR_RES, 80); // 80: height
    lv_obj_set_scroll_dir(g_mainPage, LV_DIR_VER);
    lv_label_set_text(label, node->info.name);
    lv_obj_set_style_text_font(label, g_font, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_center(label);
    lv_obj_add_flag(label, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_add_event_cb(label, LoadPage, LV_EVENT_CLICKED, node);
}

void LvDemoExt(void)
{
    LvInitFontExt();
    g_font = LvFontLoadExt(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: fontSize

    g_mainPage = lv_obj_create(lv_scr_act());
    lv_obj_set_scrollbar_mode(g_mainPage, LV_SCROLLBAR_MODE_OFF);

    g_mainScr = lv_scr_act();
    lv_obj_set_style_bg_color(g_mainPage, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(g_mainPage, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(g_mainPage, 0, 0);
    lv_obj_set_style_radius(g_mainPage, 0, 0);
    lv_obj_set_flex_flow(g_mainPage, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_grow(g_mainPage, 1);
    lv_obj_set_size(g_mainPage, LV_HOR_RES, LV_VER_RES);
    lv_obj_center(g_mainPage);

    g_backBtn = lv_obj_create(lv_layer_top());
    lv_obj_set_size(g_backBtn, 50, 50); // 50: size
    lv_obj_align(g_backBtn, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_add_flag(g_backBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_radius(g_backBtn, 0, 0);
    lv_obj_set_style_border_width(g_backBtn, 0, 0);
    lv_obj_set_style_border_opa(g_backBtn, LV_OPA_0, 0);
    lv_obj_set_style_bg_opa(g_backBtn, LV_OPA_0, 0);

    PrepareTestCase();
}