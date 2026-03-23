
/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "app.h"
#include "lvgl.h"
#include "lv_demo_watch_font.h"

#if LV_USE_DEMO_WATCH

#define CONT_WIDTH LV_HOR_RES
#define CONT_HEIGHT (LV_VER_RES / 8)

static lv_style_t g_labelStyle;
static lv_obj_t* g_btn = NULL;

static lv_obj_t* AddPage(lv_obj_t* menu, const char* name, const char* iconSrc)
{
    lv_obj_t* page = lv_menu_cont_create(menu);

    lv_obj_t* icon = lv_img_create(page);
    if (iconSrc != NULL) {
#if LV_USE_IMAGE_CACHE_EXT
        LvImgSetSrcWithCacheFlag(icon, iconSrc, false);
#else
        LvImgSetSrcExt(icon, iconSrc);
#endif
        lv_img_set_zoom(icon, 128); // 128: zoom
        lv_obj_align(icon, LV_ALIGN_LEFT_MID, 0, 0);
    }
    lv_obj_add_style(icon, &g_labelStyle, LV_STATE_DEFAULT);

    lv_obj_t* label = lv_label_create(page);
    lv_label_set_text(label, name);
    lv_obj_add_style(label, &g_labelStyle, LV_STATE_DEFAULT);
    return page;
}

void SetBtnClickable(bool isClickable)
{
    if (isClickable) {
        lv_obj_add_flag(g_btn, LV_OBJ_FLAG_CLICKABLE);
    } else {
        lv_obj_clear_flag(g_btn, LV_OBJ_FLAG_CLICKABLE);
    }
}

static void EventCb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* appList = (lv_obj_t*)lv_event_get_user_data(e);
    if (code == LV_EVENT_CLICKED) {
        lv_obj_add_flag(appList, LV_OBJ_FLAG_HIDDEN);
        SetBtnClickable(false);
    }
}

lv_obj_t* LvglPageMenuStart(lv_obj_t* parent)
{
    lv_font_t* font22 = GetLvFont(22); // 22: size
    lv_obj_t* menu = lv_menu_create(parent);
    lv_obj_set_size(menu, LV_HOR_RES, LV_VER_RES);
    lv_obj_clear_flag(menu, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_center(menu);

    static lv_style_t mainMenu;
    lv_style_init(&mainMenu);
    lv_style_set_bg_opa(&mainMenu, LV_OPA_COVER);
    lv_style_set_bg_color(&mainMenu, lv_color_black());
    lv_obj_add_style(menu, &mainMenu, LV_STATE_DEFAULT);
    lv_style_set_border_opa(&mainMenu, LV_OPA_0);
    lv_style_set_border_width(&mainMenu, 0);

    lv_style_init(&g_labelStyle);
    lv_style_set_text_color(&g_labelStyle, lv_color_white());
    lv_style_set_text_font(&g_labelStyle, font22);
    lv_style_set_text_opa(&g_labelStyle, LV_OPA_COVER);
    lv_style_set_bg_opa(&g_labelStyle, LV_OPA_0);

    /* Create a main page */
    lv_obj_t* mainPage = lv_menu_page_create(menu, NULL);
    lv_obj_clear_flag(menu, LV_OBJ_FLAG_SCROLL_CHAIN);

    AddPage(mainPage, "Alarm", "/user/res/APPLIST_ALARM.bin");
    AddPage(mainPage, "Phone", "/user/res/APPLIST_CALLER_IMAGE.bin");
    AddPage(mainPage, "Compass", "/user/res/APPLIST_COMPASS_IMAGE.bin");
    AddPage(mainPage, "Setting", "/user/res/APPLIST_SETTING.bin");
    AddPage(mainPage, "Blue Tooth", "/user/res/BLUETOOTH_TITLE.bin");

    lv_menu_set_page(menu, mainPage);

    g_btn = lv_obj_create(parent);
    lv_obj_clear_flag(g_btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(g_btn, LV_PCT(20), LV_PCT(20)); // 20: PCT
    lv_obj_align(g_btn, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(g_btn, EventCb, LV_EVENT_ALL, menu);
    lv_obj_set_style_bg_opa(g_btn, LV_OPA_0, 0);
    lv_obj_set_style_border_opa(g_btn, LV_OPA_0, 0);

    return menu;
}
#endif