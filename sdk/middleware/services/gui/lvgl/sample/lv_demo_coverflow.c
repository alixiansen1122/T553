/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "lv_demo_coverflow.h"
#include "lv_img_ext.h"
#include "lv_demo_ext.h"

#if LV_USE_DEMO_COVERFLOW

#define FACE_NUM 3
static uint8_t g_centerIndex = FACE_NUM / 2;
static float g_maxAngle = 70.0f;
static float g_imgW = 200.0f;
static float g_imgHalf = 100.0f;
static float g_divisorTwo = 2.0f;
static lv_coord_t g_padW = 125;
static lv_coord_t g_objW = 250;
static bool g_isInited = false;
static lv_font_t* g_font = NULL;

typedef struct {
    const char* name;
    const char* path;
    const char* mirrorPath;
} FaceInfo;

static FaceInfo g_faceInfo[FACE_NUM] = {
    {"rainbow", "/user/res/rainbow.bin", "/user/res/rainbow.bin"},
    {"kaleidoscope", "/user/res/kaleidoscope.bin", "/user/res/kaleidoscope.bin"},
    {"planet", "/user/res/planet.bin", "/user/res/planet.bin"}
};

static void TransformFace(lv_obj_t* cont, uint8_t i, uint8_t imgIndex)
{
    lv_obj_t* child = lv_obj_get_child(cont, i);
    if (child == NULL) {
        return;
    }
    lv_obj_t* obj = lv_obj_get_child(child, imgIndex);
    if (obj == NULL) {
        return;
    }
    float halfWidth = lv_area_get_width(&cont->coords) / g_divisorTwo;
    lv_coord_t contMidX = (lv_coord_t)(cont->coords.x1 + halfWidth);
    lv_coord_t contMidY = (lv_coord_t)(cont->coords.y1 + lv_area_get_height(&cont->coords) / g_divisorTwo);

    lv_coord_t x  = obj->coords.x1;
    lv_coord_t x1 = obj->coords.x2;
    lv_coord_t y = obj->coords.y1;
    lv_coord_t xMid = (lv_coord_t)(x + (x1 - x + 1) / g_divisorTwo);
    bool isLeft = false;
    bool isCenter = false;
    if (xMid < contMidX) {
        isLeft = true;
    } else if (xMid > contMidX) {
        isLeft = false;
    } else {
        isCenter = true;
    }

    float angle = 0.0f;
    float offset = 0;
    if (isCenter) {
        LvImgExtRotate(obj, 0, (LvVector3){g_imgHalf, 0, 0}, (LvVector3){g_imgHalf, LV_VER_RES, 0});
    } else if (isLeft) {
        offset = contMidX - xMid;
        offset = LV_MIN(g_maxAngle * (offset / halfWidth), g_maxAngle);
        angle = 360.0f - offset;
        LvImgExtRotate(obj, angle, (LvVector3){g_imgW, 0, 0}, (LvVector3){g_imgW, LV_VER_RES, 0});
    } else {
        offset = xMid - contMidX;
        angle = LV_MIN(g_maxAngle * (offset / halfWidth), g_maxAngle);
        LvImgExtRotate(obj, angle, (LvVector3){0.0, 0, 0}, (LvVector3){0.0, LV_VER_RES, 0});
    }

    LvImgExtSetCamPosition(obj, (LvVector2){contMidX - x, contMidY - y});
}

static void ScrollEventCb(lv_event_t * e)
{
    lv_obj_t* cont = lv_event_get_target(e);
    lv_obj_update_layout(cont);
    lv_area_t contArea;
    lv_obj_get_coords(cont, &contArea);
    lv_coord_t contMidX = (lv_coord_t)(contArea.x1 + lv_area_get_width(&contArea) / g_divisorTwo);
    lv_obj_t* label = (lv_obj_t*)lv_event_get_user_data(e);

    lv_coord_t minOffset = (lv_coord_t)(lv_area_get_width(&contArea) / g_divisorTwo);
    uint8_t newCenterIndex = g_centerIndex;
    for (uint8_t i = 0; i < FACE_NUM; i++) {
        lv_coord_t x = lv_obj_get_child(cont, i)->coords.x1;
        lv_coord_t x1 = lv_obj_get_child(cont, i)->coords.x2;
        lv_coord_t xMid = (lv_coord_t)(x + (x1 - x + 1) / g_divisorTwo);
        lv_coord_t offset = LV_ABS(xMid - contMidX);
        if (offset < minOffset) {
            newCenterIndex = i;
            minOffset = offset;
        }
    }
    if (newCenterIndex != g_centerIndex) {
        g_centerIndex = newCenterIndex;
        lv_label_set_text(label, g_faceInfo[g_centerIndex].name);
    }

    /* Update Text Opa */
    lv_obj_t* centerObj = lv_obj_get_child(cont, g_centerIndex);
    lv_coord_t centerChildMid = (lv_coord_t)(centerObj->coords.x1 +
        lv_area_get_width(&centerObj->coords) / g_divisorTwo);
    lv_coord_t offset = LV_ABS(contMidX - centerChildMid);
    uint8_t alpha = (uint8_t)((1.0f - ((float)offset / (lv_area_get_width(&centerObj->coords) / g_divisorTwo))) *
        LV_OPA_COVER);
    lv_obj_set_style_text_opa(label, alpha, 0);

    /* Update Imgs */
    for (uint8_t i = 0; i < FACE_NUM; i++) {
        TransformFace(cont, i, 0);
        TransformFace(cont, i, 1);
    }
}

static void CreateFace(lv_obj_t* cont, uint8_t index)
{
    lv_obj_t* obj = lv_obj_create(cont);
    lv_obj_set_size(obj, g_objW, LV_VER_RES);
    lv_obj_set_pos(obj, g_objW * index, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_0, 0);
    lv_obj_set_style_border_opa(obj, LV_OPA_0, 0);
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(obj, LV_DIR_NONE);

    lv_obj_t* img = LvImgExtCreate(obj);
    lv_obj_center(img);
    LvImgSetSrcExt(img, g_faceInfo[index].path);

    lv_obj_t* mirrorImg = LvImgExtCreate(obj);
    lv_obj_align_to(mirrorImg, img, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
    LvImgSetSrcExt(mirrorImg, g_faceInfo[index].mirrorPath);
    lv_obj_set_style_img_opa(mirrorImg, LV_OPA_30, 0);

    lv_obj_set_scroll_dir(obj, LV_DIR_NONE);

    lv_obj_update_layout(obj);
}

void LvDemoCoverFlow(lv_obj_t* par)
{
    lv_obj_set_scrollbar_mode(par, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_color(par, lv_color_black(), 0);
    lv_obj_set_scroll_dir(par, LV_DIR_NONE);

    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_center(cont);
    lv_obj_set_style_bg_color(cont, lv_color_black(), 0);
    lv_obj_set_size(cont, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(cont, 0, 0);
    lv_obj_set_scroll_snap_x(cont, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_opa(cont, LV_OPA_0, 0);
    lv_obj_set_style_radius(cont, 0, 0);
    lv_obj_set_style_pad_hor(cont, g_padW, 0);
    lv_obj_set_scroll_dir(cont, LV_DIR_HOR);

    for (uint8_t i = 0; i < FACE_NUM; i++) {
        CreateFace(cont, i);
    }

    /* Init Img Transform */
    TransformFace(cont, 0, 0);
    TransformFace(cont, 0, 1);
    TransformFace(cont, FACE_NUM - 1, 0);
    TransformFace(cont, FACE_NUM - 1, 1);

    /* Init Title */
    LvInitFontExt();
    g_font = LvFontLoadExt(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: Font Size
    lv_obj_t* label = lv_label_create(par);
    lv_obj_set_pos(label, 127, 50); // 127: x, 50: y
    lv_obj_set_size(label, 200, 50); // 200: width, 50: height
    lv_label_set_text(label, g_faceInfo[g_centerIndex].name);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_font(label, g_font, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);

    /* Add Scroll Cb */
    lv_obj_add_event_cb(cont, ScrollEventCb, LV_EVENT_SCROLL, label);

    lv_obj_update_layout(cont);

    lv_obj_t* centerObj = lv_obj_get_child(cont, g_centerIndex);
    lv_coord_t scroll = centerObj->coords.x1 - (lv_area_get_width(&cont->coords) - g_objW) / 2; // 2: divisor
    lv_obj_scroll_by(cont, -scroll, 0, LV_ANIM_OFF);
    lv_point_t p;
    lv_indev_scroll_get_snap_dist(cont, &p);
    lv_obj_scroll_by(cont, p.x, 0, LV_ANIM_OFF);
}

static void Init(lv_obj_t* par)
{
    if (g_isInited) {
        return;
    }
    g_isInited = true;
    LvDemoCoverFlow(par);
}

static void Deinit(lv_obj_t* par)
{
    if (!g_isInited) {
        return;
    }
    lv_obj_clean(par);
    LvFontFreeExt(g_font);
    g_font = NULL;
    g_centerIndex = FACE_NUM / 2; // 2: divisor
    g_isInited = false;
}

static LvTestCaseInfo g_testCaseInfo = {
    "CoverFlow",
    Init,
    Deinit,
};

void LvRegisterDemoCoverFlow(void)
{
    RegisterTestCase(&g_testCaseInfo);
}
#endif