/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "notification.h"
#include "lvgl.h"
#include "lv_demo_watch_font.h"

#if LV_USE_DEMO_WATCH

#define BTN_DEFAULT_WIDTH    (LV_HOR_RES * 11 / 12)
#define BTN_DEFAULT_HEIGHT   (LV_VER_RES / 6)

#define NOTE_DEFAULT_WIDTH    LV_HOR_RES
#define NOTE_DEFAULT_HEIGHT   (LV_VER_RES / 3)

#define NOTE_BG_DEFAULT_WIDTH    NOTE_DEFAULT_WIDTH
#define NOTE_BG_DEFAULT_HEIGHT   (NOTE_DEFAULT_HEIGHT * 3 / 4)

#define NOTE_TITLE_DEFAULT_WIDTH    (NOTE_BG_DEFAULT_WIDTH / 2)
#define NOTE_TITLE_DEFAULT_HEIGHT   (NOTE_BG_DEFAULT_HEIGHT * 1 / 3)

#define NOTE_TIME_DEFAULT_WIDTH   (NOTE_BG_DEFAULT_WIDTH / 8)
#define NOTE_TIME_DEFAULT_HEIGHT   (NOTE_BG_DEFAULT_HEIGHT * 1 / 3)

#define NOTE_TEXT_DEFAULT_Y   (NOTE_BG_DEFAULT_HEIGHT * 1 / 3)
#define NOTE_TEXT_DEFAULT_WIDTH   NOTE_DEFAULT_WIDTH
#define NOTE_TEXT_DEFAULT_HEIGHT   (NOTE_BG_DEFAULT_HEIGHT * 7 / 12)

static LvPageInfo g_pageInfo;
static void NotificationPreloadCb(lv_obj_t* obj)
{
    LV_LOGI("callback %s func\n", __FUNCTION__);
}

static void NotificationUnloadCb(lv_obj_t* obj)
{
    LV_LOGI("callback %s func\n", __FUNCTION__);
}

static void NotificationScrollBeginCb(lv_obj_t* obj, bool isActive)
{
    LV_LOGI("callback[%s]: OBJ[%p] IsActive[%d]\n", __FUNCTION__, obj, isActive);
}

static void NotificationScrollEndCb(lv_obj_t* obj, bool isActive)
{
    LV_LOGI("callback[%s]: OBJ[%p] IsActive[%d]\n", __FUNCTION__, obj, isActive);
}

static void NotificationCoverBeginCb(lv_obj_t* obj, bool isCovered)
{
    LV_LOGI("callback[%s]: OBJ[%p] isCovered[%d]\n", __FUNCTION__, obj, isCovered);
}

static void NotificationCoverEndCb(lv_obj_t* obj, bool isCovered)
{
    LV_LOGI("callback[%s]: OBJ[%p] isCovered[%d]\n", __FUNCTION__, obj, isCovered);
}

LvPageInfo* GetNotificationPageInfo()
{
    g_pageInfo.preload = NotificationPreloadCb;
    g_pageInfo.unload = NotificationUnloadCb;
    g_pageInfo.scrollBegin = NotificationScrollBeginCb;
    g_pageInfo.scrollEnd = NotificationScrollEndCb;
    g_pageInfo.coverBegin = NotificationCoverBeginCb;
    g_pageInfo.coverEnd = NotificationCoverEndCb;

    return &g_pageInfo;
}

static lv_obj_t* CreateClearBtn(lv_obj_t* main)
{
    lv_font_t* font22 = GetLvFont(22); // 22: size
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_reset(&style);
    lv_style_set_radius(&style, BTN_DEFAULT_HEIGHT / 2); // 2: divider
    lv_style_set_bg_opa(&style, LV_OPA_50);
    lv_style_set_text_font(&style, font22);
    lv_style_set_bg_color(&style, lv_palette_darken(LV_PALETTE_GREY, 2)); // 2: lvl
    lv_style_set_text_color(&style, lv_color_white());
    lv_style_set_border_opa(&style, LV_OPA_0);
    lv_style_set_border_width(&style, 0);

    /* Init the pressed style */
    static lv_style_t stylePr;
    lv_style_init(&stylePr);
    lv_style_reset(&stylePr);
    lv_style_init(&stylePr);
    lv_style_set_bg_opa(&stylePr, LV_OPA_100);
    lv_style_set_bg_color(&stylePr, lv_palette_darken(LV_PALETTE_GREY, 2)); // 2: lvl

    lv_obj_t* clearBtn = lv_btn_create(main);
    lv_obj_remove_style_all(clearBtn);
    lv_obj_add_style(clearBtn, &style, 0);
    lv_obj_add_style(clearBtn, &stylePr, LV_STATE_PRESSED);
    lv_obj_set_size(clearBtn, BTN_DEFAULT_WIDTH, BTN_DEFAULT_HEIGHT);
    lv_obj_set_align(clearBtn, LV_ALIGN_TOP_MID);

    lv_obj_t* label = lv_label_create(clearBtn);
    lv_label_set_text(label, "Clear All");
    lv_obj_center(label);
    return clearBtn;
}

static lv_obj_t* CreateNoteTime(lv_obj_t* parent)
{
    static lv_style_t timeStyle;
    lv_style_init(&timeStyle);
    lv_style_set_text_color(&timeStyle, lv_palette_darken(LV_PALETTE_GREY, 2)); // 2: lvl
    lv_style_set_text_font(&timeStyle, GetLvFont(16)); // 16: size
    lv_style_set_text_opa(&timeStyle, LV_OPA_100);
    lv_style_set_bg_opa(&timeStyle, LV_OPA_0);
    lv_style_set_border_opa(&timeStyle, LV_OPA_0);
    lv_style_set_border_width(&timeStyle, 0);

    lv_obj_t* time = lv_label_create(parent);
    lv_obj_add_style(time, &timeStyle, LV_STATE_DEFAULT);
    lv_obj_set_size(time, NOTE_TIME_DEFAULT_WIDTH, NOTE_TIME_DEFAULT_HEIGHT);
    lv_label_set_long_mode(time, LV_LABEL_LONG_CLIP);
    lv_label_set_text(time, "now");
    return time;
}

static lv_obj_t* CreateNoteTitle(lv_obj_t* parent)
{
    static lv_style_t titleStyle;
    lv_style_init(&titleStyle);
    lv_style_set_text_color(&titleStyle, lv_palette_lighten(LV_PALETTE_GREY, 2)); // 2: lvl
    lv_style_set_text_font(&titleStyle, GetLvFont(22)); // 22: size
    lv_style_set_text_opa(&titleStyle, LV_OPA_COVER);
    lv_style_set_bg_opa(&titleStyle, LV_OPA_0);
    lv_style_set_border_opa(&titleStyle, LV_OPA_0);
    lv_style_set_border_width(&titleStyle, 0);

    lv_obj_t* title = lv_label_create(parent);
    lv_obj_add_style(title, &titleStyle, LV_STATE_DEFAULT);
    lv_obj_set_size(title, NOTE_TITLE_DEFAULT_WIDTH, NOTE_TITLE_DEFAULT_HEIGHT);
    lv_label_set_long_mode(title, LV_LABEL_LONG_DOT);
    lv_label_set_text(title, "Title Note");
    lv_obj_set_align(title, LV_ALIGN_TOP_MID);
    return title;
}

static lv_obj_t* CreateNoteMessge(lv_obj_t* parent)
{
    static lv_style_t labelStyle;
    lv_style_init(&labelStyle);
    lv_style_reset(&labelStyle);
    lv_style_set_radius(&labelStyle, NOTE_BG_DEFAULT_HEIGHT / 4); // 4: divider
    lv_style_set_text_color(&labelStyle, lv_color_white());
    lv_style_set_text_font(&labelStyle, GetLvFont(16)); // 16: size
    lv_style_set_text_opa(&labelStyle, LV_OPA_COVER);
    lv_style_set_bg_opa(&labelStyle, LV_OPA_0);
    lv_style_set_border_opa(&labelStyle, LV_OPA_0);
    lv_style_set_border_width(&labelStyle, 0);

    lv_obj_t* message = lv_label_create(parent);
    lv_obj_add_style(message, &labelStyle, LV_STATE_DEFAULT);
    // 4: divider
    lv_obj_set_size(message, NOTE_TEXT_DEFAULT_WIDTH - NOTE_BG_DEFAULT_HEIGHT / 4, NOTE_TEXT_DEFAULT_HEIGHT);
    lv_label_set_long_mode(message, LV_LABEL_LONG_DOT);
    lv_label_set_text(message,
        "In all time before now and in all time to come, there has never been and will never be anyone just");
    lv_obj_set_pos(message, 0, NOTE_TEXT_DEFAULT_Y);
    lv_obj_clear_flag(message, LV_OBJ_FLAG_SCROLLABLE);
    return message;
}

static lv_obj_t* CreateBgGroup(lv_obj_t* parent)
{
    static lv_style_t bgStyle;
    lv_style_init(&bgStyle);
    lv_style_reset(&bgStyle);
    lv_style_set_radius(&bgStyle, NOTE_BG_DEFAULT_HEIGHT / 4); // 4: divider
    lv_style_set_bg_opa(&bgStyle, LV_OPA_50);
    lv_style_set_bg_color(&bgStyle, lv_palette_darken(LV_PALETTE_GREY, 2)); // 2: lvl
    lv_style_set_border_opa(&bgStyle, LV_OPA_0);
    lv_style_set_border_width(&bgStyle, 0);

    lv_obj_t* bgView = lv_obj_create(parent);
    lv_obj_set_align(bgView, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_size(bgView, NOTE_BG_DEFAULT_WIDTH, NOTE_BG_DEFAULT_HEIGHT);
    lv_obj_add_style(bgView, &bgStyle, LV_STATE_DEFAULT);
    lv_obj_clear_flag(bgView, LV_OBJ_FLAG_SCROLLABLE);
    return bgView;
}

static lv_obj_t* CreateNote(lv_obj_t* parent)
{
    static lv_style_t note_main_style;
    lv_style_init(&note_main_style);
    lv_style_reset(&note_main_style);
    lv_style_set_bg_opa(&note_main_style, LV_OPA_0);
    lv_style_set_bg_color(&note_main_style, lv_color_black());
    lv_style_set_border_opa(&note_main_style, LV_OPA_0);
    lv_style_set_border_width(&note_main_style, 0);

    lv_obj_t* noteView = lv_obj_create(parent);
    lv_obj_set_size(noteView, NOTE_DEFAULT_WIDTH, NOTE_DEFAULT_HEIGHT);
    lv_obj_center(noteView);
    lv_obj_add_style(noteView, &note_main_style, LV_STATE_DEFAULT);
    lv_obj_clear_flag(noteView, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* bgView = CreateBgGroup(noteView);
    lv_obj_t* message = CreateNoteMessge(bgView);
    lv_obj_t* title = CreateNoteTitle(bgView);
    lv_obj_t* time = CreateNoteTime(bgView);
    lv_obj_align_to(time, title, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

    lv_obj_t* img = lv_img_create(noteView);
#if LV_USE_IMAGE_CACHE_EXT
    LvImgSetSrcWithCacheFlag(img, "/user/res/APPLIST_ALARM.bin", false);
#else
    LvImgSetSrcExt(img, "/user/res/APPLIST_ALARM.bin");
#endif
    lv_img_set_zoom(img, 100); // 100: zoom
    lv_img_set_pivot(img, 0, 0);
    lv_obj_set_align(img, LV_ALIGN_TOP_LEFT);
    return noteView;
}

void LvglCreateNotification(lv_obj_t* subMenu)
{
    lv_obj_t* btn = CreateClearBtn(subMenu);
    lv_obj_t* note1 = CreateNote(subMenu);
    lv_obj_align_to(note1, subMenu, LV_ALIGN_TOP_MID, 0, lv_obj_get_y(btn) + lv_obj_get_height(btn));
}
#endif