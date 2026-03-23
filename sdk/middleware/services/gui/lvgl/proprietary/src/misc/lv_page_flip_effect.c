/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */
#include "lv_page_flip_effect.h"
#include "lv_matrix.h"
#include "lv_img_ext.h"

#define CAMERA_DISTANCE 600
#define EASE_RANGE 100
static lv_obj_t* g_bg = NULL;
static lv_obj_t* g_image = NULL;
static lv_obj_t* g_visiblePage = NULL;
static bool g_beginScroll = false;
static bool g_isInit = false;
static void LvPageFlipEffectBegin(void)
{
    int16_t middleWidth = LV_HOR_RES / 2;  // 2: half of width
    int16_t middleHeight = LV_VER_RES / 2;  // 2: half of height
    lv_obj_move_foreground(g_bg);
    lv_obj_clear_flag(g_bg, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(g_image);
    lv_obj_clear_flag(g_image, LV_OBJ_FLAG_HIDDEN);

    LvVector2 pos;
    pos.x = (float)middleWidth;
    pos.y = (float)middleHeight;
    LvImgExtSetCamPosition(g_image, pos);
    LvImgExtSetCamDistance(g_image, CAMERA_DISTANCE);
}

static void LvPageFlipEffectEnd(void)
{
    if (!g_beginScroll) {
        return;
    }
    g_visiblePage = NULL;
    lv_obj_add_flag(g_image, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(g_bg, LV_OBJ_FLAG_HIDDEN);
    g_beginScroll = false;
}

static void LvPageFlipEffect(lv_obj_t* leftPage, lv_obj_t* rightPage, lv_coord_t xOffset)
{
    if (!g_beginScroll) {
        LvPageFlipEffectBegin();
        g_beginScroll = true;
    }

    int16_t middleWidth = LV_HOR_RES / 2;  // 2: half of width
    int16_t middleHeight = LV_VER_RES / 2;  // 2: half of height
    float factor;
    if (LV_ABS(xOffset) < middleWidth) {
        factor = (LV_ABS(xOffset) * EASE_RANGE) / middleWidth;
    } else {
        factor = ((LV_HOR_RES - LV_ABS(xOffset)) * EASE_RANGE) / middleWidth;
    }

    float scale = 1.0 - 0.2 * factor / EASE_RANGE; // 0.2: scale range
    float angle = 90 * factor / EASE_RANGE;        // 90: quarter cycle degrees

    lv_obj_t* tmpCard = NULL;
    if (xOffset > 0) {
        if (xOffset < middleWidth) {
            angle = -angle;
            tmpCard = leftPage;
        } else {
            tmpCard = rightPage;
        }
    } else {
        if (LV_ABS(xOffset) < middleWidth) {
            tmpCard = rightPage;
        } else {
            angle = -angle;
            tmpCard = leftPage;
        }
    }

    if (tmpCard == NULL) {
        return;
    }

    if (g_visiblePage != tmpCard && xOffset != 0) {
        lv_img_set_src(g_image, lv_img_get_src(tmpCard));
        g_visiblePage = tmpCard;
    }

    LvImgExtScale(g_image, (LvVector3){scale, scale, 0}, (LvVector3){middleWidth, middleHeight, 0});
    LvImgExtRotate(g_image, angle, (LvVector3){middleWidth, 0, 0}, (LvVector3){middleWidth, LV_VER_RES, 0});
}

void LvPageFlipScrollEffectInit(void)
{
    if (g_isInit) {
        return;
    }
    if (g_bg == NULL) {
        g_bg = lv_obj_create(lv_scr_act());
        lv_obj_add_flag(g_bg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_pos(g_bg, 0, 0);
        lv_obj_set_size(g_bg, LV_HOR_RES, LV_VER_RES);
        lv_obj_set_style_bg_color(g_bg, lv_color_black(), 0);
        lv_obj_set_style_bg_opa(g_bg, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(g_bg, 0, 0);
        lv_obj_set_style_radius(g_bg, 0, 0);
    }
    if (g_image == NULL) {
        g_image = LvImgExtCreate(lv_scr_act());
        lv_obj_add_flag(g_image, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_pos(g_image, 0, 0);
        lv_obj_set_size(g_image, LV_HOR_RES, LV_VER_RES);
    }
    g_isInit = true;
}

void LvPageFlipScrollEffectDeinit(void)
{
    if (!g_isInit) {
        return;
    }
    if (g_bg != NULL) {
        lv_obj_del(g_bg);
        g_bg = NULL;
    }
    if (g_image != NULL) {
        lv_obj_del(g_image);
        g_image = NULL;
    }
    g_isInit = false;
}

ScrollEffectFunc LvGetPageFlipScrollEffect(void)
{
    static ScrollEffectFunc func;
    func.scrollEffect = LvPageFlipEffect;
    func.scrollEffectEnd = LvPageFlipEffectEnd;
    return func;
}