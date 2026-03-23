/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "tools.h"
#include "lv_demo_watch_font.h"
#include "lv_img_cache_manager.h"
#if LV_USE_DEMO_WATCH

#define TOOLS_GROUP_W (LV_HOR_RES * 0.6)
#define TOOLS_GROUP_H (LV_VER_RES / 2)

static LvPageInfo g_pageInfo;
static uint32_t g_resId = 0;
static lv_obj_t* g_img = NULL;
static void ToolsPreloadCb(lv_obj_t* obj)
{
    LV_LOGI("callback %s func\n", __FUNCTION__);
    g_resId = LvLoadImg("/user/res/APPLIST_VOCASSIST_IMAGE.bin", true);
    LvImgSetSrcById(g_img, g_resId);
}

static void ToolsUnloadCb(lv_obj_t* obj)
{
    LV_LOGI("callback %s func\n", __FUNCTION__);
    LvUnloadImg(g_resId);
}

static void ToolsScrollBeginCb(lv_obj_t* obj, bool isActive)
{
    LV_LOGI("callback[%s]: OBJ[%p] IsActive[%d]\n", __FUNCTION__, obj, isActive);
}

static void ToolsScrollEndCb(lv_obj_t* obj, bool isActive)
{
    LV_LOGI("callback[%s]: OBJ[%p] IsActive[%d]\n", __FUNCTION__, obj, isActive);
}

static void ToolsCoverBeginCb(lv_obj_t* obj, bool isCovered)
{
    LV_LOGI("callback[%s]: OBJ[%p] isCovered[%d]\n", __FUNCTION__, obj, isCovered);
}

static void ToolsCoverEndCb(lv_obj_t* obj, bool isCovered)
{
    LV_LOGI("callback[%s]: OBJ[%p] isCovered[%d]\n", __FUNCTION__, obj, isCovered);
}

LvPageInfo* GetToolsPageInfo()
{
    g_pageInfo.preload = ToolsPreloadCb;
    g_pageInfo.unload = ToolsUnloadCb;
    g_pageInfo.scrollBegin = ToolsScrollBeginCb;
    g_pageInfo.scrollEnd = ToolsScrollEndCb;
    g_pageInfo.coverBegin = ToolsCoverBeginCb;
    g_pageInfo.coverEnd = ToolsCoverEndCb;

    return &g_pageInfo;
}

static void LvglCreateGroup(lv_obj_t* parent)
{
    lv_font_t* font22 = GetLvFont(22); // 22: size
    static lv_style_t tileStyle;
    lv_style_init(&tileStyle);
    lv_style_set_bg_color(&tileStyle, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_bg_opa(&tileStyle, LV_OPA_COVER);
    lv_style_set_radius(&tileStyle, TOOLS_GROUP_H / 5); // 5: divider
    lv_style_set_border_opa(&tileStyle, LV_OPA_0);
    lv_style_set_pad_all(&tileStyle, 0);
    lv_style_set_text_color(&tileStyle, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_text_font(&tileStyle, font22);

    lv_obj_t* tileGroup = lv_obj_create(parent);
    lv_obj_add_style(tileGroup, &tileStyle, 0);
    lv_obj_set_size(tileGroup, TOOLS_GROUP_W, TOOLS_GROUP_H);
    lv_obj_center(tileGroup);
    lv_obj_clear_flag(tileGroup, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(tileGroup, LV_OBJ_FLAG_SCROLLABLE);

    g_img = LvImgExtCreate(tileGroup);
    lv_obj_center(g_img);
    lv_obj_clear_flag(g_img, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(g_img, LV_OBJ_FLAG_CLICKABLE);
}

void LvglCreateTools(lv_obj_t* parent)
{
    lv_font_t* font22 = GetLvFont(22); // 22: size
    static lv_style_t mainStyle;
    lv_style_init(&mainStyle);
    lv_style_set_bg_color(&mainStyle, lv_color_black());
    lv_style_set_bg_opa(&mainStyle, LV_OPA_COVER);
    lv_style_set_border_opa(&mainStyle, LV_OPA_0);

    lv_obj_add_style(parent, &mainStyle, 0);
    lv_obj_set_style_bg_color(parent, lv_color_black(), LV_PART_MAIN);

    static lv_style_t titleLabelStyle;
    lv_style_init(&titleLabelStyle);
    lv_style_set_text_font(&titleLabelStyle, font22);
    lv_style_set_bg_opa(&titleLabelStyle, LV_OPA_0);
    lv_style_set_text_color(&titleLabelStyle, lv_color_white());

    lv_obj_t* toolLabel = lv_label_create(parent);
    if (toolLabel == NULL) {
        return;
    }
    lv_obj_add_style(toolLabel, &titleLabelStyle, 0);
    lv_label_set_text(toolLabel, "Tool");
    lv_obj_set_align(toolLabel, LV_ALIGN_TOP_LEFT);
    lv_obj_clear_flag(toolLabel, LV_OBJ_FLAG_SCROLLABLE);

    static lv_style_t timeLabelStyle;
    lv_style_init(&timeLabelStyle);
    lv_style_set_text_font(&timeLabelStyle, font22);
    lv_style_set_bg_opa(&timeLabelStyle, LV_OPA_0);
    lv_style_set_text_color(&timeLabelStyle, lv_color_white());
    lv_style_set_text_align(&timeLabelStyle, LV_TEXT_ALIGN_RIGHT);

    lv_obj_t* timeLabel = lv_label_create(parent);
    if (timeLabel == NULL) {
        return;
    }
    lv_obj_add_style(timeLabel, &timeLabelStyle, 0);
    lv_label_set_text(timeLabel, "10:03");
    lv_obj_set_align(timeLabel, LV_ALIGN_TOP_RIGHT);
    lv_obj_clear_flag(timeLabel, LV_OBJ_FLAG_SCROLLABLE);

    LvglCreateGroup(parent);
}
#endif
