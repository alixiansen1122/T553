/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "watch.h"
#include "sys/time.h"
#include "lvgl.h"
#include "lv_demo_watch_font.h"

#if LV_USE_DEMO_WATCH

#define TIME_OBJ_WIDTH   lv_disp_get_hor_res(_lv_refr_get_disp_refreshing())
#define TIME_OBJ_HEIGHT  lv_disp_get_ver_res(_lv_refr_get_disp_refreshing()) / 3

typedef struct {
    lv_obj_t* timeLabel;
    lv_obj_t* dateLabel;
    lv_obj_t* weekdayLabel;
} LvClock;

static bool g_isPressed = false;
static bool g_isAppListActive = false;
static lv_obj_t* g_appList = NULL;
#define MIN_TO_HOUR 60
#define HOURLY_SYSTEM 24

static LvPageInfo g_pageInfo;
static void ClockPreloadCb(lv_obj_t* obj)
{
    LV_LOGI("callback %s func\n", __FUNCTION__);
}

static void ClockUnloadCb(lv_obj_t* obj)
{
    LV_LOGI("callback %s func\n", __FUNCTION__);
}

static void ClockScrollBeginCb(lv_obj_t* obj, bool isActive)
{
    LV_LOGI("callback[%s]: OBJ[%p] IsActive[%d]\n", __FUNCTION__, obj, isActive);
}

static void ClockScrollEndCb(lv_obj_t* obj, bool isActive)
{
    LV_LOGI("callback[%s]: OBJ[%p] IsActive[%d]\n", __FUNCTION__, obj, isActive);
}

static void ClockCoverBeginCb(lv_obj_t* obj, bool isCovered)
{
    LV_LOGI("callback[%s]: OBJ[%p] isCovered[%d]\n", __FUNCTION__, obj, isCovered);
}

static void ClockCoverEndCb(lv_obj_t* obj, bool isCovered)
{
    LV_LOGI("callback[%s]: OBJ[%p] isCovered[%d]\n", __FUNCTION__, obj, isCovered);
}

LvPageInfo* GetClockPageInfo()
{
    g_pageInfo.preload = ClockPreloadCb;
    g_pageInfo.unload = ClockUnloadCb;
    g_pageInfo.scrollBegin = ClockScrollBeginCb;
    g_pageInfo.scrollEnd = ClockScrollEndCb;
    g_pageInfo.coverBegin = ClockCoverBeginCb;
    g_pageInfo.coverEnd = ClockCoverEndCb;

    return &g_pageInfo;
}

static void ClockDateTaskCb(lv_timer_t* timer)
{
    // 7: day count a week
    static const char* weekDay[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
    static time_t unixTime;
    static struct tm* timeInfo;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t second = tv.tv_sec;
    int minutes = second / MIN_TO_HOUR;
    int hour = minutes / MIN_TO_HOUR;
    int day = hour / HOURLY_SYSTEM;
    int weekday = (day / 7) % 7;      // 7: day to weekday
    int realHour = hour % HOURLY_SYSTEM;
    int realMinute = minutes % MIN_TO_HOUR;
    int realMilliSecond = second % MIN_TO_HOUR;
    int realDay = day % HOURLY_SYSTEM + 1;

    if (timer != NULL && timer->user_data != NULL) {
        LvClock* clock = (LvClock*)(timer->user_data);
        if (clock->timeLabel != NULL) {
            lv_label_set_text_fmt(clock->timeLabel, "%02d:%02d:%02d", realHour, realMinute, realMilliSecond);
            lv_obj_align_to(clock->timeLabel, lv_obj_get_parent(clock->timeLabel), LV_ALIGN_CENTER, 0, 0);
        }

        if (clock->weekdayLabel != NULL) {
            lv_label_set_text_fmt(clock->weekdayLabel, "%02d %s", realDay, weekDay[weekday]);
            // -2: offset
            lv_obj_align_to(clock->weekdayLabel, lv_obj_get_parent(clock->weekdayLabel), LV_ALIGN_BOTTOM_MID, -2, 0);
        }
    }
}

static lv_obj_t* CreateData(lv_obj_t* parent)
{
    static lv_style_t dateStyle;
    lv_style_init(&dateStyle);
    lv_style_set_bg_opa(&dateStyle, LV_OPA_0);
    lv_style_set_border_width(&dateStyle, 0);
    lv_style_set_radius(&dateStyle, 5); // 5: radius
    lv_style_set_bg_color(&dateStyle, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_pad_left(&dateStyle, 0);
    lv_style_set_pad_right(&dateStyle, 0);

    /* Date display */
    lv_obj_t* dateObj = lv_obj_create(parent);
    lv_obj_clear_flag(dateObj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(dateObj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(dateObj, TIME_OBJ_WIDTH, TIME_OBJ_HEIGHT);
    lv_obj_align_to(dateObj, parent, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style(dateObj, &dateStyle, LV_STATE_DEFAULT);
    return dateObj;
}

static lv_obj_t* CreateTime(lv_obj_t* parent)
{
    static lv_style_t timeStyle;
    lv_style_init(&timeStyle);
    lv_style_set_bg_opa(&timeStyle, LV_OPA_0);
    lv_style_set_border_width(&timeStyle, 0);
    lv_style_set_radius(&timeStyle, 0);
    lv_style_set_pad_all(&timeStyle, 0);

    /* Time display */
    lv_obj_t* timeObj = lv_obj_create(parent);
    lv_obj_clear_flag(timeObj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(timeObj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(timeObj, TIME_OBJ_WIDTH, TIME_OBJ_HEIGHT);
    lv_obj_align_to(timeObj, parent, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style(timeObj, &timeStyle, LV_STATE_DEFAULT);
    return timeObj;
}

static lv_obj_t* CreateWatchBg(lv_obj_t* parent)
{
    static lv_style_t dateTimeClockStyle;
    lv_style_init(&dateTimeClockStyle);
    lv_style_set_bg_opa(&dateTimeClockStyle, LV_OPA_COVER);
    lv_style_set_border_width(&dateTimeClockStyle, 0);
    lv_style_set_bg_color(&dateTimeClockStyle, lv_color_black());
    lv_style_set_pad_left(&dateTimeClockStyle, 1);
    lv_style_set_pad_right(&dateTimeClockStyle, 1);
    lv_style_set_pad_top(&dateTimeClockStyle, 0);
    lv_style_set_pad_bottom(&dateTimeClockStyle, 0);

    /* Time & Date */
    lv_obj_t* timeDateObj = lv_obj_create(parent);
    lv_obj_clear_flag(timeDateObj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(timeDateObj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(timeDateObj, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL) * 3 / 5); // 3, 5: ratio
    lv_obj_add_style(timeDateObj, &dateTimeClockStyle, LV_STATE_DEFAULT);
    lv_obj_center(timeDateObj);
    return timeDateObj;
}

static void ShowAppList(lv_obj_t* parent)
{
    if (g_appList == NULL) {
        g_appList = LvglPageMenuStart(parent);
    } else {
        lv_obj_clear_flag(g_appList, LV_OBJ_FLAG_HIDDEN);
        SetBtnClickable(true);
    }
}

static void EventClickCb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* container = (lv_obj_t*)lv_event_get_user_data(e);
    if (code == LV_EVENT_CLICKED) {
        ShowAppList(container);
    }
}

lv_obj_t* CreateListBtn(lv_obj_t* parent)
{
    lv_obj_t* btn = lv_obj_create(parent);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(btn, LV_PCT(20), LV_PCT(20)); // 20: PCT
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(btn, EventClickCb, LV_EVENT_ALL, parent);
    lv_obj_set_style_bg_opa(btn, LV_OPA_0, 0);
    lv_obj_set_style_border_opa(btn, LV_OPA_0, 0);
    return btn;
}

void LvglCreateClock(lv_obj_t* subMenu)
{
    /* Time font */
    static lv_style_t timeLabelStyle;
    lv_style_init(&timeLabelStyle);
    lv_style_set_text_color(&timeLabelStyle, lv_color_white());
    lv_style_set_text_font(&timeLabelStyle, GetLvFont(32)); // 32: size
    lv_style_set_text_opa(&timeLabelStyle, LV_OPA_COVER);
    lv_style_set_bg_opa(&timeLabelStyle, LV_OPA_0);

    /* Date font */
    static lv_style_t dateLabelStyle;
    lv_style_init(&dateLabelStyle);
    lv_style_set_text_opa(&dateLabelStyle, LV_OPA_COVER);
    lv_style_set_bg_opa(&dateLabelStyle, LV_OPA_0);
    lv_style_set_text_color(&dateLabelStyle, lv_color_white());
    lv_style_set_text_font(&dateLabelStyle, GetLvFont(16)); // 16: size

    /* Week font */
    static lv_style_t weekLabelStyle;
    lv_style_init(&weekLabelStyle);
    lv_style_set_text_opa(&weekLabelStyle, LV_OPA_COVER);
    lv_style_set_bg_opa(&weekLabelStyle, LV_OPA_0);
    lv_style_set_text_color(&weekLabelStyle, lv_color_white());
    lv_style_set_text_font(&weekLabelStyle, GetLvFont(16)); // 16: size

    lv_obj_t* timeDateObj = CreateWatchBg(subMenu);
    lv_obj_t* timeObj = CreateTime(timeDateObj);
    lv_obj_t* dateObj = CreateData(timeDateObj);
    lv_obj_t* btn = CreateListBtn(subMenu);

    static LvClock lvClock = { 0 };
    lvClock.timeLabel = lv_label_create(timeObj);
    lv_obj_clear_flag(lvClock.timeLabel, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(lvClock.timeLabel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(lvClock.timeLabel, &timeLabelStyle, LV_STATE_DEFAULT);
    /* Week display */
    lvClock.weekdayLabel = lv_label_create(dateObj);
    lv_obj_clear_flag(lvClock.weekdayLabel, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(lvClock.weekdayLabel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(lvClock.weekdayLabel, &weekLabelStyle, LV_STATE_DEFAULT);

    lv_obj_align_to(lvClock.timeLabel, lv_obj_get_parent(lvClock.timeLabel), LV_ALIGN_CENTER, 0, 0);
    lv_obj_align_to(lvClock.weekdayLabel, lvClock.timeLabel, LV_ALIGN_OUT_BOTTOM_MID, -2, 0); // -2: offset

    lv_timer_create(ClockDateTaskCb, 200, (void*)&lvClock); // 200: period
}
#endif