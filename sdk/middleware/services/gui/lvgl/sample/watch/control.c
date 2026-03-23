/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "control.h"
#include "lv_demo_watch_font.h"

#if LV_USE_DEMO_WATCH

#define CONTROL_BTN_GROUP_W  LV_HOR_RES
#define CONTROL_BTN_GROUP_H  (LV_VER_RES * 2 / 3)

#define CONTROL_BTN_S    10
#define CONTROL_BTN_W    (CONTROL_BTN_GROUP_W / 2 - CONTROL_BTN_S * 2)
#define CONTROL_BTN_H    (CONTROL_BTN_GROUP_H / 3 - CONTROL_BTN_S * 2)
static LvPageInfo g_pageInfo;
static void ControlPreloadCb(lv_obj_t* obj)
{
    LV_LOGI("callback %s func\n", __FUNCTION__);
}

static void ControlUnloadCb(lv_obj_t* obj)
{
    LV_LOGI("callback %s func\n", __FUNCTION__);
}

static void ControlScrollBeginCb(lv_obj_t* obj, bool isActive)
{
    LV_LOGI("callback[%s]: OBJ[%p] IsActive[%d]\n", __FUNCTION__, obj, isActive);
}

static void ControlScrollEndCb(lv_obj_t* obj, bool isActive)
{
    LV_LOGI("callback[%s]: OBJ[%p] IsActive[%d]\n", __FUNCTION__, obj, isActive);
}

static void ControlCoverBeginCb(lv_obj_t* obj, bool isCovered)
{
    LV_LOGI("callback[%s]: OBJ[%p] isCovered[%d]\n", __FUNCTION__, obj, isCovered);
}

static void ControlCoverEndCb(lv_obj_t* obj, bool isCovered)
{
    LV_LOGI("callback[%s]: OBJ[%p] isCovered[%d]\n", __FUNCTION__, obj, isCovered);
}

LvPageInfo* GetControlPageInfo()
{
    g_pageInfo.preload = ControlPreloadCb;
    g_pageInfo.unload = ControlUnloadCb;
    g_pageInfo.scrollBegin = ControlScrollBeginCb;
    g_pageInfo.scrollEnd = ControlScrollEndCb;
    g_pageInfo.coverBegin = ControlCoverBeginCb;
    g_pageInfo.coverEnd = ControlCoverEndCb;

    return &g_pageInfo;
}

static void EventHandlerBtn1(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        lv_obj_t* btn = lv_event_get_target(e);
        lv_obj_t* label = (lv_obj_t*)lv_event_get_user_data(e);
        if (lv_obj_has_state(btn, LV_STATE_USER_3)) {
            lv_obj_clear_state(btn, LV_STATE_USER_3);
            lv_obj_add_state(btn, LV_STATE_USER_1);
            lv_label_set_text(label, "9%");
        } else if (lv_obj_has_state(btn, LV_STATE_USER_1)) {
            lv_obj_clear_state(btn, LV_STATE_USER_1);
            lv_obj_add_state(btn, LV_STATE_USER_2);
            lv_label_set_text(label, "17%");
        } else if (lv_obj_has_state(btn, LV_STATE_USER_2)) {
            lv_obj_clear_state(btn, LV_STATE_USER_2);
            lv_obj_add_state(btn, LV_STATE_USER_3);
            lv_label_set_text(label, "90%");
        }
    }
}

static void EventHandlerBtn2(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        lv_obj_t* btn = lv_event_get_target(e);
        lv_obj_t* label = (lv_obj_t*)lv_event_get_user_data(e);
        if (!lv_obj_has_state(btn, LV_STATE_USER_1)) {
            lv_obj_add_state(btn, LV_STATE_USER_1);
            lv_label_set_text(label, "FlashLight OFF");
        } else {
            lv_obj_clear_state(btn, LV_STATE_USER_1);
            lv_label_set_text(label, "FlashLight ON");
        }
    }
}

static void EventHandlerBtn3(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        lv_obj_t* btn = lv_event_get_target(e);
        lv_obj_t* img = (lv_obj_t*)lv_event_get_user_data(e);
        lv_state_t state = lv_obj_get_state(btn);
        if (!lv_obj_has_state(btn, LV_STATE_USER_1)) {
            lv_obj_add_state(btn, LV_STATE_USER_1);
#if LV_USE_IMAGE_CACHE_EXT
            LvImgSetSrcWithCacheFlag(img, "/user/res/MUTE_BUTTON.bin", false);
#else
            LvImgSetSrcExt(img, "/user/res/MUTE_BUTTON.bin");
#endif
            lv_img_set_zoom(img, 64); // 64: zoom
        } else {
            lv_obj_clear_state(btn, LV_STATE_USER_1);
#if LV_USE_IMAGE_CACHE_EXT
            LvImgSetSrcWithCacheFlag(img, "/user/res/INCREASE_BUTTON.bin", false);
#else
            LvImgSetSrcExt(img, "/user/res/INCREASE_BUTTON.bin");
#endif
            lv_img_set_zoom(img, 64); // 64: zoom
        }
    }
}

static void EventHandlerBtn4(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        lv_obj_t* btn = lv_event_get_target(e);
        lv_obj_t* label = (lv_obj_t*)lv_event_get_user_data(e);
        if (!lv_obj_has_state(btn, LV_STATE_USER_1)) {
            lv_obj_add_state(btn, LV_STATE_USER_1);
            lv_label_set_text(label, "DND");
        } else {
            lv_obj_clear_state(btn, LV_STATE_USER_1);
            lv_label_set_text(label, "SLEEP");
        }
    }
}

static lv_obj_t* CreateBtn1(lv_obj_t* parent)
{
    static lv_style_t greenStyle;
    lv_style_init(&greenStyle);
    lv_style_set_bg_opa(&greenStyle, LV_OPA_100);
    lv_style_set_radius(&greenStyle, CONTROL_BTN_H / 4); // 4: divider
    lv_style_set_text_font(&greenStyle, GetLvFont(22)); // 22: size
    lv_style_set_bg_color(&greenStyle, lv_palette_darken(LV_PALETTE_GREEN, 2)); // 2: lvl
    lv_style_set_text_color(&greenStyle, lv_color_white());

    static lv_style_t yellowStyle;
    lv_style_init(&yellowStyle);
    lv_style_set_radius(&yellowStyle, CONTROL_BTN_H / 4); // 4: divider
    lv_style_set_text_font(&yellowStyle, GetLvFont(22)); // 22: size
    lv_style_set_bg_opa(&yellowStyle, LV_OPA_100);
    lv_style_set_bg_color(&yellowStyle, lv_palette_darken(LV_PALETTE_YELLOW, 2)); // 2: lvl
    lv_style_set_text_color(&yellowStyle, lv_color_white());

    static lv_style_t redStyle;
    lv_style_init(&redStyle);
    lv_style_set_radius(&redStyle, CONTROL_BTN_H / 4); // 4: divider
    lv_style_set_text_font(&redStyle, GetLvFont(22)); // 22: size
    lv_style_set_bg_opa(&redStyle, LV_OPA_100);
    lv_style_set_bg_color(&redStyle, lv_palette_darken(LV_PALETTE_RED, 2)); // 2: lvl
    lv_style_set_text_color(&redStyle, lv_color_white());

    lv_obj_t* btnObj = lv_btn_create(parent);
    lv_obj_remove_style_all(btnObj);
    lv_obj_add_style(btnObj, &redStyle, LV_STATE_USER_1);
    lv_obj_add_style(btnObj, &yellowStyle, LV_STATE_USER_2);
    lv_obj_add_style(btnObj, &greenStyle, LV_STATE_USER_3);
    lv_obj_set_size(btnObj, CONTROL_BTN_W, CONTROL_BTN_H);
    lv_obj_add_state(btnObj, LV_STATE_USER_1);

    lv_obj_t* labelObj = lv_label_create(btnObj);
    lv_obj_add_event_cb(btnObj, EventHandlerBtn1, LV_EVENT_CLICKED, (void*)labelObj);
    lv_label_set_text(labelObj, "17%");
    lv_obj_center(labelObj);
    lv_obj_clear_flag(btnObj, LV_OBJ_FLAG_SCROLLABLE);
    return btnObj;
}

static lv_obj_t* CreateBtn2(lv_obj_t* parent, lv_style_t* mainStyle, lv_style_t* userStyle)
{
    lv_obj_t* btnObj = lv_btn_create(parent);
    lv_obj_remove_style_all(btnObj);
    lv_obj_add_style(btnObj, mainStyle, 0);
    lv_obj_add_style(btnObj, userStyle, LV_STATE_USER_1);
    lv_obj_set_size(btnObj, CONTROL_BTN_W, CONTROL_BTN_H);

    lv_obj_t* labelObj = lv_label_create(btnObj);
    lv_obj_add_event_cb(btnObj, EventHandlerBtn2, LV_EVENT_CLICKED, (void*)labelObj);
    lv_label_set_text(labelObj, "FlashLight ON");
    lv_obj_center(labelObj);
    lv_obj_clear_flag(btnObj, LV_OBJ_FLAG_SCROLLABLE);
    return btnObj;
}

static lv_obj_t* CreateBtn3(lv_obj_t* parent, lv_style_t* mainStyle, lv_style_t* userStyle)
{
    lv_obj_t* btnObj = lv_btn_create(parent);
    lv_obj_remove_style_all(btnObj);
    lv_obj_add_style(btnObj, mainStyle, 0);
    lv_obj_add_style(btnObj, userStyle, LV_STATE_USER_1);
    lv_obj_set_size(btnObj, CONTROL_BTN_W, CONTROL_BTN_H);

    lv_obj_t* icon = lv_img_create(btnObj);
#if LV_USE_IMAGE_CACHE_EXT
    LvImgSetSrcWithCacheFlag(icon, "/user/res/INCREASE_BUTTON.bin", false);
#else
    LvImgSetSrcExt(icon, "/user/res/INCREASE_BUTTON.bin");
#endif
    lv_img_set_zoom(icon, 64); // 64: zoom
    lv_obj_add_event_cb(btnObj, EventHandlerBtn3, LV_EVENT_CLICKED, (void*)icon);
    lv_obj_center(icon);
    lv_obj_clear_flag(btnObj, LV_OBJ_FLAG_SCROLLABLE);
    return btnObj;
}

static lv_obj_t* CreateBtn4(lv_obj_t* parent)
{
    static lv_style_t defineStyle;
    lv_style_init(&defineStyle);
    lv_style_set_radius(&defineStyle, CONTROL_BTN_H / 4); // 4: divider
    lv_style_set_bg_opa(&defineStyle, LV_OPA_50);
    lv_style_set_text_font(&defineStyle, GetLvFont(22)); // 22: size
    lv_style_set_bg_color(&defineStyle, lv_palette_darken(LV_PALETTE_GREY, 2)); // 2: lvl
    lv_style_set_text_color(&defineStyle, lv_color_white());
    lv_style_set_border_opa(&defineStyle, LV_OPA_0);
    lv_style_set_border_width(&defineStyle, 0);

    static lv_style_t yellowStyle;
    lv_style_init(&yellowStyle);
    lv_style_set_radius(&yellowStyle, CONTROL_BTN_H / 4); // 4: divider
    lv_style_set_text_font(&yellowStyle, GetLvFont(22)); // 22: size
    lv_style_set_bg_opa(&yellowStyle, LV_OPA_100);
    lv_style_set_bg_color(&yellowStyle, lv_palette_darken(LV_PALETTE_YELLOW, 2)); // 2: lvl
    lv_style_set_text_color(&yellowStyle, lv_color_white());

    /* Init the pressed style */
    static lv_style_t pressStyle;
    lv_style_init(&pressStyle);
    lv_style_set_bg_opa(&pressStyle, LV_OPA_100);
    lv_style_set_bg_color(&pressStyle, lv_palette_darken(LV_PALETTE_GREY, 2)); // 2: lvl

    lv_obj_t* btnObj = lv_btn_create(parent);
    lv_obj_remove_style_all(btnObj);
    lv_obj_add_style(btnObj, &defineStyle, 0);
    lv_obj_add_style(btnObj, &yellowStyle, LV_STATE_USER_1);
    lv_obj_add_style(btnObj, &pressStyle, LV_STATE_PRESSED);
    lv_obj_set_size(btnObj, CONTROL_BTN_W, CONTROL_BTN_H);

    lv_obj_t* labelObj = lv_label_create(btnObj);
    lv_obj_add_event_cb(btnObj, EventHandlerBtn4, LV_EVENT_CLICKED, (void*)labelObj);
    lv_label_set_text(labelObj, "SLEEP");
    lv_obj_center(labelObj);
    lv_obj_clear_flag(btnObj, LV_OBJ_FLAG_SCROLLABLE);
    return btnObj;
}

static void CreateBtnGroup(lv_obj_t* parent)
{
    static lv_style_t groupStyle;
    lv_style_init(&groupStyle);
    lv_style_set_bg_opa(&groupStyle, LV_OPA_0);
    lv_style_set_pad_all(&groupStyle, 0);
    lv_style_set_border_opa(&groupStyle, LV_OPA_0);
    lv_style_set_border_width(&groupStyle, 0);

    lv_obj_t* btnGroup = lv_obj_create(parent);
    lv_obj_set_size(btnGroup, CONTROL_BTN_GROUP_W, CONTROL_BTN_GROUP_H);
    lv_obj_center(btnGroup);
    lv_obj_add_style(btnGroup, &groupStyle, LV_STATE_DEFAULT);
    lv_obj_clear_flag(btnGroup, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(btnGroup, LV_OBJ_FLAG_CLICKABLE);

    static lv_style_t onStyle;
    lv_style_init(&onStyle);
    lv_style_set_radius(&onStyle, CONTROL_BTN_H / 4); // 4: divider
    lv_style_set_bg_opa(&onStyle, LV_OPA_100);
    lv_style_set_text_font(&onStyle, GetLvFont(14)); // 14: size
    lv_style_set_bg_color(&onStyle, lv_palette_darken(LV_PALETTE_GREEN, 2)); // 2: lvl
    lv_style_set_text_color(&onStyle, lv_color_white());
    lv_style_set_border_opa(&onStyle, LV_OPA_0);
    lv_style_set_border_width(&onStyle, 0);

    static lv_style_t offStyle;
    lv_style_init(&offStyle);
    lv_style_set_radius(&offStyle, CONTROL_BTN_H / 4); // 4: divider
    lv_style_set_bg_opa(&offStyle, LV_OPA_100);
    lv_style_set_text_font(&offStyle, GetLvFont(14)); // 14: size
    lv_style_set_bg_color(&offStyle, lv_palette_darken(LV_PALETTE_RED, 2)); // 2: lvl
    lv_style_set_text_color(&offStyle, lv_color_white());
    lv_style_set_border_opa(&offStyle, LV_OPA_0);
    lv_style_set_border_width(&offStyle, 0);

    lv_obj_t* btn1 = CreateBtn1(btnGroup);
    lv_obj_align(btn1, LV_ALIGN_TOP_LEFT, CONTROL_BTN_S, CONTROL_BTN_S);

    lv_obj_t* btn2 = CreateBtn2(btnGroup, &onStyle, &offStyle);
    lv_obj_align_to(btn2, btn1, LV_ALIGN_OUT_RIGHT_MID, CONTROL_BTN_S, 0);

    lv_obj_t* btn3 = CreateBtn3(btnGroup, &onStyle, &offStyle);
    lv_obj_align_to(btn3, btn1, LV_ALIGN_OUT_BOTTOM_MID, 0, CONTROL_BTN_S);

    lv_obj_t* btn4 = CreateBtn4(btnGroup);
    lv_obj_align_to(btn4, btn3, LV_ALIGN_OUT_RIGHT_MID, CONTROL_BTN_S, 0);
}

void LvglCreateControl(lv_obj_t* subMenu)
{
    static lv_style_t mainStyle;
    lv_style_init(&mainStyle);
    lv_style_set_bg_color(&mainStyle, lv_color_black());
    lv_style_set_bg_opa(&mainStyle, LV_OPA_100);
    lv_style_set_pad_all(&mainStyle, 0);
    lv_style_set_border_opa(&mainStyle, LV_OPA_0);
    lv_style_set_border_width(&mainStyle, 0);
    lv_obj_add_style(subMenu, &mainStyle, LV_STATE_DEFAULT);

    CreateBtnGroup(subMenu);
}
#endif
