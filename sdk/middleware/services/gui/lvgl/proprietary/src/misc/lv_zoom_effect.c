/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */
#include "lv_zoom_effect.h"
#include "lv_matrix.h"
static void LvLeftPageZoom(lv_obj_t* leftPage, lv_coord_t xOffset)
{
    if (leftPage == NULL) {
        return;
    }
    float width = lv_disp_get_hor_res(NULL);
    float scale = 0.5 * xOffset / width; // 0.5: base scale value
    LvVector3 leftScalePivot;
    LvVector3 scaleVector;
    leftScalePivot.x = lv_obj_get_width(leftPage);
    leftScalePivot.y = lv_obj_get_height(leftPage) / 2; // 2: middle
    leftScalePivot.z = 0;

    if (xOffset < 0) {
        float scaleIn = 0.5 - scale; // 0.5: base scale value
        scaleVector.x = scaleIn;
        scaleVector.y = scaleIn;
        scaleVector.z = 1.0;
        LvImgExtScale(leftPage, scaleVector, leftScalePivot);
    } else {
        float scaleOut = 1 - scale;
        scaleVector.x = scaleOut;
        scaleVector.y = scaleOut;
        scaleVector.z = 1.0;
        LvImgExtScale(leftPage, scaleVector, leftScalePivot);
    }
}

static void LvRightPageZoom(lv_obj_t* rightPage, lv_coord_t xOffset)
{
    if (rightPage == NULL) {
        return;
    }
    float width = lv_disp_get_hor_res(NULL);
    float scale = 0.5 * xOffset / width; // 0.5: base scale value
    LvVector3 rightScalePivot;
    LvVector3 scaleVector;
    rightScalePivot.x = 0;
    rightScalePivot.y = lv_obj_get_height(rightPage) / 2.0f; // 2.0f: middle
    rightScalePivot.z = 0;

    if (xOffset <= 0) {
        float scaleOut = 1 + scale;
        scaleVector.x = scaleOut;
        scaleVector.y = scaleOut;
        scaleVector.z = 1.0;
        LvImgExtScale(rightPage, scaleVector, rightScalePivot);
    } else {
        float scaleIn = 0.5 + scale; // 0.5: base scale value (scale < 0)
        scaleVector.x = scaleIn;
        scaleVector.y = scaleIn;
        scaleVector.z = 1.0;
        LvImgExtScale(rightPage, scaleVector, rightScalePivot);
    }
}

static void LvZoomEffect(lv_obj_t* leftPage, lv_obj_t* rightPage, lv_coord_t xOffset)
{
    LvLeftPageZoom(leftPage, xOffset);
    LvRightPageZoom(rightPage, xOffset);
}

ScrollEffectFunc LvGetZoomScrollEffect(void)
{
    static ScrollEffectFunc func;
    func.scrollEffect = LvZoomEffect;
    func.scrollEffectEnd = NULL;
    return func;
}