/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "activity.h"
#include "lv_demo_watch_font.h"

#if LV_USE_DEMO_WATCH

#define CIRCLE_GROUP_WIDTH  (LV_HOR_RES / 2)
#define CIRCLE_GROUP_HEIGHT (LV_VER_RES / 3)

#define OUT_LABEL_W         (LV_HOR_RES / 2)
#define OUT_LABEL_H         (GROUP_DEFAULT_H / 4)

lv_obj_t* g_label = NULL;
lv_obj_t* g_label2 = NULL;

static LvPageInfo g_pageInfo;
static void ActivityPreloadCb(lv_obj_t* obj)
{
    LV_LOGI("callback %s func\n", __FUNCTION__);
}

static void ActivityUnloadCb(lv_obj_t* obj)
{
    LV_LOGI("callback %s func\n", __FUNCTION__);
}

static void ActivityScrollBeginCb(lv_obj_t* obj, bool isActive)
{
    LV_LOGI("callback[%s]: OBJ[%p] IsActive[%d]\n", __FUNCTION__, obj, isActive);
}

static void ActivityScrollEndCb(lv_obj_t* obj, bool isActive)
{
    LV_LOGI("callback[%s]: OBJ[%p] IsActive[%d]\n", __FUNCTION__, obj, isActive);
}

static void ActivityCoverBeginCb(lv_obj_t* obj, bool isCovered)
{
    LV_LOGI("callback[%s]: OBJ[%p] isCovered[%d]\n", __FUNCTION__, obj, isCovered);
}

static void ActivityCoverEndCb(lv_obj_t* obj, bool isCovered)
{
    LV_LOGI("callback[%s]: OBJ[%p] isCovered[%d]\n", __FUNCTION__, obj, isCovered);
}

LvPageInfo* GetActivityPageInfo()
{
    g_pageInfo.preload = ActivityPreloadCb;
    g_pageInfo.unload = ActivityUnloadCb;
    g_pageInfo.scrollBegin = ActivityScrollBeginCb;
    g_pageInfo.scrollEnd = ActivityScrollEndCb;
    g_pageInfo.coverBegin = ActivityCoverBeginCb;
    g_pageInfo.coverEnd = ActivityCoverEndCb;

    return &g_pageInfo;
}

static void SetOutAngle(lv_obj_t* obj, int32_t v)
{
    lv_arc_set_value(obj, v);
    lv_label_set_text_fmt(g_label, "%d/500", v * 500 / 100); // 500: max Calories value; 100: circle animator max value
}

static void SetInAngle(lv_obj_t* obj, int32_t v)
{
    lv_arc_set_value(obj, v);
    lv_label_set_text_fmt(g_label2, "%d/30", v * 30 / 100);  // 30: max timer value; 100: circle animator max value
}

static lv_obj_t* CreateArc(lv_obj_t* parent)
{
    if (parent == NULL) {
        return NULL;
    }
    lv_obj_t* circle = lv_arc_create(parent);
    if (circle == NULL) {
        return NULL;
    }
    lv_arc_set_rotation(circle, 270); // 270: angle
    lv_arc_set_bg_angles(circle, 0, 360); // 360: angle
    lv_arc_set_value(circle, 0);
    lv_obj_remove_style(circle, NULL, LV_PART_KNOB);
    lv_obj_center(circle);

    lv_obj_clear_flag(circle, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(circle, LV_OBJ_FLAG_SCROLLABLE);
    return circle;
}

static lv_obj_t* CreateArcTitleLabel(lv_obj_t* parent, const char* text)
{
    if (parent == NULL) {
        return NULL;
    }
    static lv_style_t sty;
    lv_style_init(&sty);
    lv_style_set_text_opa(&sty, LV_OPA_100);
    lv_style_set_text_color(&sty, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_text_font(&sty, GetLvFont(14)); // 14: size

    lv_obj_t* label = lv_label_create(parent);
    lv_obj_add_style(label, &sty, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_size(label, LV_HOR_RES / 2, CIRCLE_GROUP_HEIGHT / 4); // 2, 4: divider
    if (text != NULL) {
        lv_label_set_text(label, text);
    }
    return label;
}

static void InitAnimInfo(lv_obj_t* obj, void* var,
    lv_anim_exec_xcb_t execCb, uint32_t duration, uint32_t delay)
{
    lv_anim_init(obj);
    lv_anim_set_var(obj, var);
    lv_anim_set_exec_cb(obj, execCb);
    lv_anim_set_time(obj, duration);
    lv_anim_set_repeat_count(obj, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_repeat_delay(obj, delay);
    lv_anim_set_values(obj, 0, 100); // 100: end values
    return;
}

static void CreateCircleProgress(lv_obj_t* parent)
{
    static lv_style_t groupStyle;
    lv_style_init(&groupStyle);
    lv_style_set_bg_color(&groupStyle, lv_color_black());
    lv_style_set_bg_opa(&groupStyle, LV_OPA_100);
    lv_style_set_pad_all(&groupStyle, 0);
    lv_style_set_border_opa(&groupStyle, LV_OPA_0);
    lv_style_set_border_width(&groupStyle, 0);
    lv_style_set_pad_all(&groupStyle, 2); // 2: pad value

    lv_obj_t* circleGroup = lv_obj_create(parent);
    lv_obj_set_size(circleGroup, CIRCLE_GROUP_WIDTH, CIRCLE_GROUP_HEIGHT);
    lv_obj_add_style(circleGroup, &groupStyle, LV_STATE_DEFAULT);
    lv_obj_align_to(circleGroup, parent, LV_ALIGN_TOP_LEFT, 0, LV_VER_RES / 7); // 7: divider
    lv_obj_clear_flag(circleGroup, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(circleGroup, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t* outArc = CreateArc(circleGroup);
    lv_coord_t temp = CIRCLE_GROUP_WIDTH < CIRCLE_GROUP_HEIGHT ? CIRCLE_GROUP_WIDTH : CIRCLE_GROUP_HEIGHT;
    lv_obj_set_size(outArc, temp, temp);
    lv_obj_set_style_arc_color(outArc, lv_palette_main(LV_PALETTE_PINK), LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(outArc, lv_palette_main(LV_PALETTE_GREY), LV_PART_MAIN);

    g_label = lv_label_create(parent);
    lv_label_set_long_mode(g_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_size(g_label, LV_HOR_RES / 2, CIRCLE_GROUP_HEIGHT / 4); // 2, 4: divider
    lv_obj_align_to(g_label, outArc, LV_ALIGN_OUT_RIGHT_TOP, 10, CIRCLE_GROUP_HEIGHT / 4); // 10: offset; 4: divider
    lv_label_set_text_fmt(g_label, "%d/500", 0);

    lv_obj_t* titleLabel = CreateArcTitleLabel(parent, "Calories(KCAL)");
    lv_obj_align_to(titleLabel, outArc, LV_ALIGN_OUT_RIGHT_TOP, 10, 0); // 10: offset

    lv_anim_t a;
    InitAnimInfo(&a, outArc, SetOutAngle, 3000, 500); // 500 : delay time 3000: duration time
    lv_anim_start(&a);

    lv_obj_t* inArc = CreateArc(circleGroup);
    lv_obj_set_size(inArc, temp * 3 / 4, temp * 3 / 4); // 3, 4: ratio
    lv_obj_set_style_arc_color(inArc, lv_palette_main(LV_PALETTE_GREEN), LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(inArc, lv_palette_main(LV_PALETTE_GREY), LV_PART_MAIN);

    lv_obj_t* titleLabel2 = CreateArcTitleLabel(parent, "Time(MIN)");
    // 10: offset; 2, 4: ratio
    lv_obj_align_to(titleLabel2, outArc, LV_ALIGN_OUT_RIGHT_TOP, 10, CIRCLE_GROUP_HEIGHT * 2 / 4);

    g_label2 = lv_label_create(parent);
    lv_obj_set_style_text_color(g_label2, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_set_style_text_opa(g_label2, LV_OPA_100, 0);
    lv_label_set_long_mode(g_label2, LV_LABEL_LONG_WRAP);
    lv_obj_set_size(g_label2, LV_HOR_RES / 2, CIRCLE_GROUP_HEIGHT / 4); // 2, 4: ratio
    // 10: offset; 3, 4: ratio
    lv_obj_align_to(g_label2, outArc, LV_ALIGN_OUT_RIGHT_TOP, 10, CIRCLE_GROUP_HEIGHT * 3 / 4);
    lv_label_set_text_fmt(g_label2, "%d/30", 0);

    lv_anim_t b;
    InitAnimInfo(&b, inArc, SetInAngle, 5000, 500); // 500 : delay time 5000: duration time
    lv_anim_start(&b);
}

static lv_obj_t* CreateTitleLabel(lv_obj_t* parent)
{
    if (parent == NULL) {
        return NULL;
    }
    static lv_style_t titleLabelStyle;
    lv_style_init(&titleLabelStyle);
    lv_style_set_text_font(&titleLabelStyle, GetLvFont(22)); // 22: size
    lv_style_set_bg_opa(&titleLabelStyle, LV_OPA_0);
    lv_style_set_text_color(&titleLabelStyle, lv_color_white());

    lv_obj_t* activityLabel = lv_label_create(parent);
    lv_obj_add_style(activityLabel, &titleLabelStyle, 0);
    lv_label_set_text(activityLabel, "Activity");
    lv_obj_set_align(activityLabel, LV_ALIGN_TOP_LEFT);
    lv_obj_clear_flag(activityLabel, LV_OBJ_FLAG_SCROLLABLE);
    return activityLabel;
}

static lv_obj_t* CreateTimeLabel(lv_obj_t* parent)
{
    if (parent == NULL) {
        return NULL;
    }
    static lv_style_t timeLabelStyle;
    lv_style_init(&timeLabelStyle);
    lv_style_set_text_font(&timeLabelStyle, GetLvFont(22)); // 22: size
    lv_style_set_bg_opa(&timeLabelStyle, LV_OPA_0);
    lv_style_set_text_color(&timeLabelStyle, lv_color_white());
    lv_style_set_text_align(&timeLabelStyle, LV_TEXT_ALIGN_RIGHT);

    lv_obj_t* timeLabel = lv_label_create(parent);
    lv_obj_add_style(timeLabel, &timeLabelStyle, 0);
    lv_label_set_text(timeLabel, "10:03");
    lv_obj_set_align(timeLabel, LV_ALIGN_TOP_RIGHT);
    lv_obj_clear_flag(timeLabel, LV_OBJ_FLAG_SCROLLABLE);
    return timeLabel;
}

void LvglCreateActivity(lv_obj_t* parent)
{
    static lv_style_t rootViewStyle;
    lv_style_init(&rootViewStyle);
    lv_style_set_bg_color(&rootViewStyle, lv_color_black());
    lv_style_set_bg_opa(&rootViewStyle, LV_OPA_100);
    lv_style_set_pad_all(&rootViewStyle, 0);
    lv_style_set_border_opa(&rootViewStyle, LV_OPA_0);
    lv_style_set_border_width(&rootViewStyle, 0);
    lv_style_set_text_color(&rootViewStyle, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_text_font(&rootViewStyle, GetLvFont(22)); // 22: size
    lv_style_set_pad_left(&rootViewStyle, 12); // 12: padding
    lv_style_set_pad_right(&rootViewStyle, 12); // 12: padding
    lv_style_set_pad_top(&rootViewStyle, 6); // 6: padding
    lv_style_set_pad_bottom(&rootViewStyle, 6); // 6: padding
    lv_obj_add_style(parent, &rootViewStyle, LV_STATE_DEFAULT);

    CreateTitleLabel(parent);
    CreateTimeLabel(parent);
    CreateCircleProgress(parent);
}
#endif
