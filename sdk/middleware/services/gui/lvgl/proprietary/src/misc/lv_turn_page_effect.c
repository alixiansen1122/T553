/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */
#include "lv_turn_page_effect.h"
#include "lv_matrix.h"
#include "lv_img_ext.h"
#include "lv_draw_vgu.h"
#define EASE_RANGE 100
static lv_obj_t* g_bg = NULL;
static lv_obj_t* g_imgLL = NULL;
static lv_obj_t* g_imgLR = NULL;
static lv_obj_t* g_imgRL = NULL;
static lv_obj_t* g_imgRR = NULL;
static lv_obj_t* g_visiblePage = NULL;
static bool g_isInit = false;
static bool g_beginScroll = false;
static lv_obj_t* g_preLeftPage = false;
static lv_obj_t* g_preRightPage = false;

static lv_img_dsc_t* g_preImgDscLR = NULL;
static lv_img_dsc_t* g_imgDscLR = NULL;
static lv_img_dsc_t* g_imgDscRR = NULL;
static lv_draw_ctx_t* g_drawCtx = NULL;

static bool LvInitImgDsc(lv_img_dsc_t* imgDsc, lv_coord_t width, lv_coord_t height, uint32_t cf)
{
    if (imgDsc == NULL) {
        return false;
    }
    uint32_t buffSize = LV_ALIGN_BYTE((uint32_t)width, LV_BYTE_ALIGNMENT) * height *
        (lv_img_cf_get_px_size((lv_img_cf_t)cf) >> 3); // 3: shift by 3 bits
    uint8_t* buf = (uint8_t*)LvAllocBufferForImg(buffSize);
    if (buf == NULL) {
        LV_LOGE("[%s: %d] AllocBuffer failed, size = %u.\n", __FUNCTION__, __LINE__, buffSize);
        return false;
    }

    imgDsc->data = buf;
    imgDsc->header.w = (uint32_t)width;
    imgDsc->header.h = (uint32_t)height;
    imgDsc->header.cf = cf;
    imgDsc->data_size = buffSize;
    imgDsc->header.always_zero = 0;
    imgDsc->header.reserved = 0;
    return true;
}

static bool LvGetSrcByArea(lv_obj_t* dstObj, lv_img_dsc_t* dstDsc, lv_img_dsc_t* srcDsc, lv_area_t srcArea)
{
    if (dstObj == NULL || srcDsc == NULL) {
        return false;
    }
    if (dstDsc->data == NULL &&
        !LvInitImgDsc(dstDsc, lv_obj_get_width(dstObj), lv_obj_get_height(dstObj), srcDsc->header.cf)) {
        return false;
    }

    lv_memset_00((void*)dstDsc->data, dstDsc->data_size);
    if (g_drawCtx == NULL) {
        lv_disp_t* disp = _lv_refr_get_disp_refreshing();
        g_drawCtx = lv_mem_alloc(disp->driver->draw_ctx_size);
        if (g_drawCtx == NULL) {
            LV_LOGE("[%s: %d] alloc g_drawCtx failed.\n", __FUNCTION__, __LINE__);
            return false;
        }
        disp->driver->draw_ctx_init(NULL, g_drawCtx);
    }

    lv_area_t bufArea = {0, 0, dstDsc->header.w - 1, dstDsc->header.h - 1};
    g_drawCtx->clip_area = &bufArea;
    g_drawCtx->buf_area = &bufArea;
    g_drawCtx->buf = (void*)dstDsc->data;
    g_drawCtx->buf_stride = LV_ALIGN_BYTE((uint32_t)dstDsc->header.w, LV_BYTE_ALIGNMENT) *
        (lv_img_cf_get_px_size(dstDsc->header.cf) >> 3); // 3: shift by 3 bits
    g_drawCtx->cf = dstDsc->header.cf;

    lv_draw_img_dsc_t drawDsc;
    lv_draw_img_dsc_init(&drawDsc);
    drawDsc.blend_mode = LV_BLEND_MODE_SRC;
    drawDsc.stride = LV_ALIGN_BYTE((uint32_t)srcDsc->header.w, LV_BYTE_ALIGNMENT) *
        (lv_img_cf_get_px_size(srcDsc->header.cf) >> 3); // 3: shift by 3 bits

    LvMatrix matrix = {0};
    LvInitIdentityMatrix(&matrix);
    LvDrawVguCtx* vguCtx = (LvDrawVguCtx*)g_drawCtx;
    vguCtx->DrawTransformWithDsc(g_drawCtx, srcDsc, &drawDsc, &srcArea, &matrix);
    g_drawCtx->wait_for_finish(g_drawCtx);
    return true;
}

static void LvSetLeftSrc(lv_obj_t* leftPage)
{
    if (leftPage == NULL) {
        return;
    }
    lv_img_dsc_t* dscL = (lv_img_dsc_t*)lv_img_get_src(leftPage);
    lv_img_set_src(g_imgLL, dscL);
    if (lv_obj_has_flag(g_imgLL, LV_OBJ_FLAG_HIDDEN)) {
        lv_obj_clear_flag(g_imgLL, LV_OBJ_FLAG_HIDDEN);
    }

    lv_area_t areaL = {dscL->header.w / 2 - 1, 0, dscL->header.w - 2, dscL->header.h - 1}; // 2: half of width
    if (LvGetSrcByArea(g_imgLR, g_imgDscLR, dscL, areaL)) {
        lv_img_set_src(g_imgLR, g_imgDscLR);
        lv_obj_invalidate(g_imgLR);
    }
    LvImgExtSetCamPosition(g_imgLR, (LvVector2){0, (float)(LV_VER_RES / 2)}); // 2: half of height
}

static void LvSetRightSrc(lv_obj_t* rightPage)
{
    if (rightPage == NULL) {
        return;
    }
    lv_img_dsc_t* dscR = (lv_img_dsc_t*)lv_img_get_src(rightPage);
    lv_img_set_src(g_imgRL, dscR);

    lv_area_t areaR = {dscR->header.w / 2, 0, dscR->header.w - 1, dscR->header.h - 1}; // 2: half of width
    if (LvGetSrcByArea(g_imgRR, g_imgDscRR, dscR, areaR)) {
        lv_img_set_src(g_imgRR, g_imgDscRR);
        lv_obj_invalidate(g_imgRR);
    }
    LvImgExtSetCamPosition(g_imgRL, (LvVector2){(float)(LV_HOR_RES / 2), (float)(LV_VER_RES / 2)}); // 2: divisor
    if (lv_obj_has_flag(g_imgRR, LV_OBJ_FLAG_HIDDEN)) {
        lv_obj_clear_flag(g_imgRR, LV_OBJ_FLAG_HIDDEN);
    }
}

static void LvTurnPageEffectBegin(lv_obj_t* leftPage, lv_obj_t* rightPage)
{
    LvSetLeftSrc(leftPage);
    LvSetRightSrc(rightPage);
    if (leftPage == NULL || rightPage == NULL) {
        lv_obj_move_foreground(g_bg);
        if (lv_obj_has_flag(g_bg, LV_OBJ_FLAG_HIDDEN)) {
            lv_obj_clear_flag(g_bg, LV_OBJ_FLAG_HIDDEN);
        }
    }

    if (leftPage != NULL) {
        lv_obj_move_foreground(g_imgLL);
    }
    if (rightPage != NULL) {
        lv_obj_move_foreground(g_imgRR);
    }
    if (leftPage != NULL) {
        lv_obj_move_foreground(g_imgLR);
    }
    if (rightPage != NULL) {
        lv_obj_move_foreground(g_imgRL);
    }
    if (g_preLeftPage != NULL) {
        lv_obj_set_style_img_opa(g_preLeftPage, LV_OPA_COVER, 0);
    }
    if (g_preRightPage != NULL) {
        lv_obj_set_style_img_opa(g_preRightPage, LV_OPA_COVER, 0);
    }
    g_preLeftPage = leftPage;
    g_preRightPage = rightPage;
    if (g_preLeftPage != NULL) {
        lv_obj_set_style_img_opa(g_preLeftPage, LV_OPA_0, 0);
    }
    if (g_preRightPage != NULL) {
        lv_obj_set_style_img_opa(g_preRightPage, LV_OPA_0, 0);
    }
}

static void LvTurnPageEffectEnd(void)
{
    if (!g_beginScroll) {
        return;
    }
    lv_obj_add_flag(g_bg, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(g_imgLL, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(g_imgLR, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(g_imgRL, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(g_imgRR, LV_OBJ_FLAG_HIDDEN);

    if (g_preLeftPage != NULL) {
        lv_obj_set_style_img_opa(g_preLeftPage, LV_OPA_COVER, 0);
    }
    if (g_preRightPage != NULL) {
        lv_obj_set_style_img_opa(g_preRightPage, LV_OPA_COVER, 0);
    }
    g_visiblePage = NULL;
    g_preLeftPage = NULL;
    g_preRightPage = NULL;
    g_beginScroll = false;
}

static void LvTurnPageEffect(lv_obj_t* leftPage, lv_obj_t* rightPage, lv_coord_t xOffset)
{
    if (!g_beginScroll) {
        LvTurnPageEffectBegin(leftPage, rightPage);
        g_beginScroll = true;
    }

    if (g_preLeftPage != leftPage || g_preRightPage != rightPage) {
        LvTurnPageEffectBegin(leftPage, rightPage);
    }

    int16_t middleWidth = LV_HOR_RES / 2;  // 2: half of width
    float factor;
    if (LV_ABS(xOffset) < middleWidth) {
        factor = (LV_ABS(xOffset) * EASE_RANGE) / (float)middleWidth;
    } else {
        factor = ((LV_HOR_RES - LV_ABS(xOffset)) * EASE_RANGE) / (float)middleWidth;
    }
    float angle = 90.0f * factor / EASE_RANGE;        // 90.0f: quarter cycle degrees
    LvVector3 rotateStart = {middleWidth, 0, 0};
    LvVector3 rotateEnd = {middleWidth, LV_VER_RES, 0};

    lv_obj_t* tempCard = NULL;
    if (xOffset > 0) {
        if (xOffset < middleWidth) {
            angle = -angle;
            rotateStart.x = 0.0;
            rotateEnd.x = 0.0;
            tempCard = g_imgLR;
        } else {
            tempCard = g_imgRL;
        }
    } else if (xOffset < 0) {
        if (LV_ABS(xOffset) < middleWidth) {
            tempCard = g_imgRL;
        } else {
            angle = -angle;
            rotateStart.x = 0.0;
            rotateEnd.x = 0.0;
            tempCard = g_imgLR;
        }
    } else {
        if ((rightPage == NULL) || ((leftPage != NULL) && g_preLeftPage == leftPage)) {
            tempCard = g_imgLR;
        } else if ((leftPage == NULL) || ((rightPage != NULL) && g_preRightPage == rightPage)) {
            tempCard = g_imgRL;
        }
    }

    if (g_visiblePage != tempCard) {
        if (g_visiblePage != NULL && !lv_obj_has_flag(g_visiblePage, LV_OBJ_FLAG_HIDDEN)) {
            lv_obj_add_flag(g_visiblePage, LV_OBJ_FLAG_HIDDEN);
        }

        if (lv_obj_has_flag(tempCard, LV_OBJ_FLAG_HIDDEN)) {
            lv_obj_clear_flag(tempCard, LV_OBJ_FLAG_HIDDEN);
        }
        g_visiblePage = tempCard;
    }

    LvImgExtRotate(g_visiblePage, angle, rotateStart, rotateEnd);
}

static void CreateImgToParent(lv_obj_t* parent, lv_area_t rect, lv_obj_t** img)
{
    if (img == NULL || *img != NULL) {
        return;
    }
    lv_obj_t* tempObj = LvImgExtCreate(parent);
    lv_obj_add_flag(tempObj, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_pos(tempObj, rect.x1, rect.y1);
    lv_obj_set_size(tempObj, rect.x2, rect.y2);
    lv_obj_update_layout(tempObj);
    *img = tempObj;
}

void LvTurnPageScrollEffectInit(void)
{
    if (g_isInit) {
        return;
    }
    g_isInit = true;
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
    int16_t middleWidth = LV_HOR_RES / 2;  // 2: half of width
    CreateImgToParent(lv_scr_act(), (lv_area_t){0, 0, middleWidth, LV_VER_RES}, &g_imgLL);
    CreateImgToParent(lv_scr_act(), (lv_area_t){middleWidth - 1, 0, middleWidth, LV_VER_RES}, &g_imgLR);
    CreateImgToParent(lv_scr_act(), (lv_area_t){0, 0, middleWidth, LV_VER_RES}, &g_imgRL);
    CreateImgToParent(lv_scr_act(), (lv_area_t){middleWidth, 0, middleWidth, LV_VER_RES}, &g_imgRR);

    g_imgDscLR = (lv_img_dsc_t*)lv_mem_alloc(sizeof(lv_img_dsc_t));
    if (g_imgDscLR == NULL) {
        LV_LOGE("[%s: %d] alloc failed.\n", __FUNCTION__, __LINE__);
        LvTurnPageScrollEffectDeinit();
        return;
    }
    lv_memset_00(g_imgDscLR, sizeof(lv_img_dsc_t));

    g_imgDscRR = (lv_img_dsc_t*)lv_mem_alloc(sizeof(lv_img_dsc_t));
    if (g_imgDscRR == NULL) {
        LvTurnPageScrollEffectDeinit();
        LV_LOGE("[%s: %d] alloc failed.\n", __FUNCTION__, __LINE__);
        return;
    }
    lv_memset_00(g_imgDscRR, sizeof(lv_img_dsc_t));
}

void LvTurnPageScrollEffectDeinit(void)
{
    if (!g_isInit) {
        return;
    }
    g_isInit = false;
    if (g_bg != NULL) {
        lv_obj_del(g_imgLL);
        g_bg = NULL;
    }
    if (g_drawCtx != NULL) {
        lv_disp_t* disp = _lv_refr_get_disp_refreshing();
        disp->driver->draw_ctx_deinit(NULL, g_drawCtx);
        lv_mem_free(g_drawCtx);
        g_drawCtx = NULL;
    }
    if (g_imgDscLR != NULL) {
        LvFreeBuffer(g_imgDscLR->data);
        lv_mem_free(g_imgDscLR);
        g_imgDscLR = NULL;
    }

    if (g_imgDscRR != NULL) {
        LvFreeBuffer(g_imgDscRR->data);
        lv_mem_free(g_imgDscRR);
        g_imgDscRR = NULL;
    }
    if (g_imgLL != NULL) {
        lv_obj_del(g_imgLL);
        g_imgLL = NULL;
    }
    if (g_imgLR != NULL) {
        lv_obj_del(g_imgLR);
        g_imgLR = NULL;
    }
    if (g_imgRL != NULL) {
        lv_obj_del(g_imgRL);
        g_imgRL = NULL;
    }
    if (g_imgRR != NULL) {
        lv_obj_del(g_imgRR);
        g_imgRR = NULL;
    }
}

ScrollEffectFunc LvGetTurnPageScrollEffect(void)
{
    static ScrollEffectFunc func;
    func.scrollEffect = LvTurnPageEffect;
    func.scrollEffectEnd = LvTurnPageEffectEnd;
    return func;
}