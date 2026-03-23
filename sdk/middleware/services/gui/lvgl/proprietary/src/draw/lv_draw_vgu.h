/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#ifndef LV_DRAW_VGU_H
#define LV_DRAW_VGU_H

#include "src/draw/sw/lv_draw_sw.h"
#include "lv_conf.h"
#include "lvgl.h"

#if LV_USE_VGU
#include "display_vgu.h"
#include "lv_matrix.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    lv_draw_sw_ctx_t baseDraw;

    /* Hardware acceleration for gaussian blur */
    bool (*Blur)(lv_draw_ctx_t* drawCtx, uint16_t blurRadius);
    /* Draw transformed img with a given transformation matrix */
    void (*DrawTransformWithMatrix)(lv_draw_ctx_t* drawCtx, const lv_area_t* dstArea,
        const void* srcBuf, const lv_area_t* srcArea, const lv_draw_img_dsc_t* dsc,
        const LvMatrix* matrix, lv_img_cf_t cf);
    void (*DrawTransformWithDsc)(lv_draw_ctx_t* drawCtx, lv_img_dsc_t* src,
        const lv_draw_img_dsc_t* dsc, lv_area_t* srcArea, const LvMatrix* matrix);
    /* Init a vgu path */
    uintptr_t (*InitPath)(lv_draw_ctx_t* drawCtx, bool enAlias);
    /* Deinit a vgu path */
    void (*DeinitPath)(lv_draw_ctx_t* drawCtx, uintptr_t path);
    /* Set a path's data */
    bool (*SetPathData)(lv_draw_ctx_t* drawCtx, uintptr_t path, uint32_t num, const uint8_t* cmd,
        uint32_t dataNum, const float* data);
    /* Render a path */
    void (*RenderPath)(lv_draw_ctx_t* drawCtx, uintptr_t path, const lv_area_t* mask,
        VGUStrokeAttr* strokeAttr, VGUFillAttr* fillAttr, LvMatrix* matrix);
    /* Set a drawing flag */
    void (*StartDraw)(void);
    /* Clip img by path */
    bool (*ClipImgByPath)(lv_img_dsc_t* dst, lv_img_dsc_t* src, const lv_area_t* srcArea,
        uintptr_t path, const LvMatrix* pathMatrix, const LvMatrix* imgMatrix);
} LvDrawVguCtx;

/**
 * @brief  Init LvDrawVguCtx struct.
 * @param  [in]  drv    _lv_disp_drv_t struct.
 * @param  [in]  drawCtx    LvDrawVguCtx struct to be initialized.
 */
void LvDrawVguCtxInit(struct _lv_disp_drv_t* drv, lv_draw_ctx_t* drawCtx);

/**
 * @brief  Deinit LvDrawVguCtx struct.
 * @param  [in]  drv    _lv_disp_drv_t struct.
 * @param  [in]  drawCtx    LvDrawVguCtx struct to be uninitialized.
 */
void LvDrawVguCtxDeinit(struct _lv_disp_drv_t* drv, lv_draw_ctx_t* drawCtx);

/**
 * @brief  Init VGU module. This should be called only once and before other functions are used.
 * @return If VGU module is initialized successfully.
 */
bool LvVguModuleInit(void);

/**
 * @brief  Deinit VGU module.
 */
void LvVguModuleDeinit(void);

/**
 * @brief  sync hardware draw.
 * @param  [in]  isSync    whether the operation is synchronous or asynchronous.
 */
void LvSyncHwDraw(bool isSync);

/**
 * @brief  wait until hardware draw finish.
 */
void LvRenderWaitDone(void);

/**
 * @brief  Convert format from lv_img_cf_t to VGUPixelFormat.
 * @return If converts format successfully.
 */
bool LvConvertVguCf(lv_img_cf_t cf, VGUPixelFormat* dstColorFmt);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // LV_USE_VGU

#endif // LV_DRAW_VGU_H