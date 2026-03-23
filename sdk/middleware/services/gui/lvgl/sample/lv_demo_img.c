/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "lv_demo_img.h"
#include "lvgl.h"
#include "lv_demo_ext.h"

#if LV_USE_DEMO_IMG
static const lv_coord_t ALIGN_GAP_H = 50;
static lv_ll_t g_styleLL = {0};
static bool g_isInited = false;

static void BasicImgCreate(lv_obj_t* img, lv_blend_mode_t blendMode, uint8_t opa, lv_img_cf_t cf)
{
    if (cf == LV_IMG_CF_RGBA8888) {
        LvImgSetSrcExt(img, "/user/res/A094_051_8888_hfbc.bin");
    } else if (cf == LV_IMG_CF_RGB888) {
        LvImgSetSrcExt(img, "/user/res/A094_051_888_abypass.bin");
    } else {
        LvImgSetSrcExt(img, "/user/res/A094_051_565_abypass.bin");
    }

    lv_style_t* style = _lv_ll_ins_tail(&g_styleLL);
    lv_style_init(style);
    lv_style_set_img_opa(style, opa);
    lv_style_set_blend_mode(style, blendMode);
    lv_obj_add_style(img, style, 0);
}

static void TransformImg(lv_obj_t* img, int16_t angle, uint16_t zoom)
{
    lv_img_set_zoom(img, zoom);
    lv_img_set_angle(img, angle);
}

static lv_obj_t* ImgFmtTest(lv_obj_t * par, lv_obj_t* alignRef, lv_align_t refAlignment)
{
    lv_obj_t* img1 = lv_img_create(par);
    BasicImgCreate(img1, LV_BLEND_MODE_SRC_OVER, LV_OPA_COVER, LV_IMG_CF_RGBA8888);
    lv_obj_align_to(img1, alignRef, refAlignment, 0, ALIGN_GAP_H);
    lv_obj_t* label1 = lv_label_create(par);
    lv_obj_align_to(label1, img1, LV_ALIGN_OUT_TOP_MID, 0, 0);
    lv_label_set_text(label1, "ARGB8888");

    lv_obj_t* img2 = lv_img_create(par);
    BasicImgCreate(img2, LV_BLEND_MODE_SRC_OVER, LV_OPA_COVER, LV_IMG_CF_RGB888);
    lv_obj_align_to(img2, img1, LV_ALIGN_OUT_BOTTOM_MID, 0, ALIGN_GAP_H);
    lv_obj_t* label2 = lv_label_create(par);
    lv_obj_align_to(label2, img2, LV_ALIGN_OUT_TOP_MID, 0, 0);
    lv_label_set_text(label2, "RGB888");

    lv_obj_t* img3 = lv_img_create(par);
    BasicImgCreate(img3, LV_BLEND_MODE_SRC, LV_OPA_COVER, LV_IMG_CF_RGB565);
    lv_obj_align_to(img3, img2, LV_ALIGN_OUT_BOTTOM_MID, 0, ALIGN_GAP_H);
    lv_obj_t* label3 = lv_label_create(par);
    lv_obj_align_to(label3, img3, LV_ALIGN_OUT_TOP_MID, 0, 0);
    lv_label_set_text(label3, "RGB565");

    return img3;
}

static lv_obj_t* ImgAlphaBlendTest(lv_obj_t* par, lv_obj_t* alignRef, lv_align_t refAlignment)
{
    lv_obj_t* img1 = lv_img_create(par);
    BasicImgCreate(img1, LV_BLEND_MODE_SRC_OVER, LV_OPA_COVER, LV_IMG_CF_RGBA8888);
    lv_obj_align_to(img1, alignRef, refAlignment, 0, ALIGN_GAP_H);
    lv_obj_t* label1 = lv_label_create(par);
    lv_obj_align_to(label1, img1, LV_ALIGN_OUT_TOP_MID, 0, 0);
    lv_label_set_text(label1, "opa255 srcover");

    lv_obj_t* img2 = lv_img_create(par);
    BasicImgCreate(img2, LV_BLEND_MODE_SRC_OVER, LV_OPA_20, LV_IMG_CF_RGBA8888);
    lv_obj_align_to(img2, img1, LV_ALIGN_OUT_BOTTOM_MID, 0, ALIGN_GAP_H);
    lv_obj_t* label2 = lv_label_create(par);
    lv_obj_align_to(label2, img2, LV_ALIGN_OUT_TOP_MID, 0, 0);
    lv_label_set_text(label2, "OPA_20 srcover");

    lv_obj_t* img3 = lv_img_create(par);
    BasicImgCreate(img3, LV_BLEND_MODE_SRC, LV_OPA_20, LV_IMG_CF_RGBA8888);
    lv_obj_align_to(img3, img2, LV_ALIGN_OUT_BOTTOM_MID, 0, ALIGN_GAP_H);
    lv_obj_t* label3 = lv_label_create(par);
    lv_obj_align_to(label3, img3, LV_ALIGN_OUT_TOP_MID, 0, 0);
    lv_label_set_text(label3, "OPA_20 src");

    lv_obj_t* img4 = lv_img_create(par);
    BasicImgCreate(img4, LV_BLEND_MODE_SRC_IN, LV_OPA_20, LV_IMG_CF_RGBA8888);
    lv_obj_align_to(img4, img3, LV_ALIGN_OUT_BOTTOM_MID, 0, ALIGN_GAP_H);
    lv_obj_t* label4 = lv_label_create(par);
    lv_obj_align_to(label4, img4, LV_ALIGN_OUT_TOP_MID, 0, 0);
    lv_label_set_text(label4, "OPA_20 srcin");

    lv_obj_t* img5 = lv_img_create(par);
    BasicImgCreate(img5, LV_BLEND_MODE_DST_OVER, LV_OPA_20, LV_IMG_CF_RGBA8888);
    lv_obj_align_to(img5, img4, LV_ALIGN_OUT_BOTTOM_MID, 0, ALIGN_GAP_H);
    lv_obj_t* label5 = lv_label_create(par);
    lv_obj_align_to(label5, img5, LV_ALIGN_OUT_TOP_MID, 0, 0);
    lv_label_set_text(label5, "OPA_20 dstover");

    lv_obj_t* img6 = lv_img_create(par);
    BasicImgCreate(img6, LV_BLEND_MODE_DST_IN, LV_OPA_20, LV_IMG_CF_RGBA8888);
    lv_obj_align_to(img6, img5, LV_ALIGN_OUT_BOTTOM_MID, 0, ALIGN_GAP_H);
    lv_obj_t* label6 = lv_label_create(par);
    lv_obj_align_to(label6, img6, LV_ALIGN_OUT_TOP_MID, 0, 0);
    lv_label_set_text(label6, "OPA_20 dstin");

    return img6;
}

static lv_obj_t* ImgTransformTest(lv_obj_t* par, lv_obj_t* alignRef, lv_align_t refAlignment)
{
    lv_obj_t* img1 = lv_img_create(par);
    BasicImgCreate(img1, LV_BLEND_MODE_SRC_OVER, LV_OPA_60, LV_IMG_CF_RGBA8888);
    TransformImg(img1, 0, 128); // 128: zoom
    lv_obj_align_to(img1, alignRef, refAlignment, 0, ALIGN_GAP_H);
    lv_obj_t* label1 = lv_label_create(par);
    lv_obj_align_to(label1, img1, LV_ALIGN_OUT_TOP_MID, 0, 0);
    lv_label_set_text(label1, "zoom 128");

    lv_obj_t* img2 = lv_img_create(par);
    BasicImgCreate(img2, LV_BLEND_MODE_SRC_OVER, LV_OPA_60, LV_IMG_CF_RGBA8888);
    TransformImg(img2, 0, 512); // 512: zoom
    lv_obj_align_to(img2, img1, LV_ALIGN_OUT_BOTTOM_MID, 0, ALIGN_GAP_H);
    lv_obj_t* label2 = lv_label_create(par);
    lv_obj_align_to(label2, img2, LV_ALIGN_OUT_TOP_MID, 0, 0);
    lv_label_set_text(label2, "zoom 512");

    lv_obj_t* img3 = lv_img_create(par);
    BasicImgCreate(img3, LV_BLEND_MODE_SRC_OVER, LV_OPA_60, LV_IMG_CF_RGBA8888);
    TransformImg(img3, 900, 256); // 900: angle, 256: zoom
    lv_obj_align_to(img3, img2, LV_ALIGN_OUT_BOTTOM_MID, 0, ALIGN_GAP_H);
    lv_obj_t* label3 = lv_label_create(par);
    lv_obj_align_to(label3, img3, LV_ALIGN_OUT_TOP_MID, 0, 0);
    lv_label_set_text(label3, "rotate 900");

    lv_obj_t* img4 = lv_img_create(par);
    BasicImgCreate(img4, LV_BLEND_MODE_SRC_OVER, LV_OPA_60, LV_IMG_CF_RGBA8888);
    TransformImg(img4, 900, 128); // 900: angle, 128: zoom
    lv_obj_align_to(img4, img3, LV_ALIGN_OUT_BOTTOM_MID, 0, ALIGN_GAP_H);
    lv_obj_t* label4 = lv_label_create(par);
    lv_obj_align_to(label4, img4, LV_ALIGN_OUT_TOP_MID, 0, 0);
    lv_label_set_text(label4, "rotate 900 zoom 128");

    return img4;
}

#if LV_USE_HW_JPEG_DECODER
static lv_obj_t* JpegTest(lv_obj_t* par, lv_obj_t* alignRef, lv_align_t refAlignment)
{
    lv_obj_t* img1 = lv_img_create(par);
    LvImgSetSrcExt(img1, "/user/res/lele.jpg");
    lv_img_set_zoom(img1, 512); // 512: scaler
    lv_obj_align_to(img1, alignRef, refAlignment, 0, ALIGN_GAP_H);
    lv_obj_t* label1 = lv_label_create(par);
    lv_obj_align_to(label1, img1, LV_ALIGN_OUT_TOP_MID, 0, -30); // -30: y offset
    lv_label_set_text(label1, "JPG 60*60 & zoom 512");

    lv_obj_t* img2 = lv_img_create(par);
    LvImgSetSrcExt(img2, "/user/res/clock.jpg");
    lv_obj_align_to(img2, img1, LV_ALIGN_OUT_BOTTOM_MID, 0, ALIGN_GAP_H);
    lv_obj_t* label2 = lv_label_create(par);
    lv_obj_align_to(label2, img2, LV_ALIGN_OUT_TOP_MID, 0, 0);
    lv_label_set_text(label2, "JPG 454*454");
    return img2;
}
#endif

void LvDemoImg(lv_obj_t* par)
{
    _lv_ll_init(&g_styleLL, sizeof(lv_style_t));

    lv_obj_t* panel = lv_obj_create(par);
    lv_obj_set_height(panel, LV_SIZE_CONTENT);
    lv_obj_set_width(panel, RESOLUTION_WIDTH);
    lv_obj_set_style_bg_color(panel, lv_palette_main(LV_PALETTE_YELLOW), LV_PART_MAIN);

#if LV_USE_HW_JPEG_DECODER
    lv_obj_t* jpegLast = JpegTest(panel, panel, LV_ALIGN_TOP_MID);
    lv_obj_t* fmtLast = ImgFmtTest(panel, jpegLast, LV_ALIGN_OUT_BOTTOM_MID);
#else
    lv_obj_t* fmtLast = ImgFmtTest(panel, panel, LV_ALIGN_TOP_MID);
#endif
    lv_obj_t* blendLast = ImgAlphaBlendTest(panel, fmtLast, LV_ALIGN_OUT_BOTTOM_MID);
    lv_obj_t* transLast = ImgTransformTest(panel, blendLast, LV_ALIGN_OUT_BOTTOM_MID);
}

static void Init(lv_obj_t* par)
{
    if (g_isInited) {
        return;
    }
    LvDemoImg(par);
    g_isInited = true;
}

static void ClearStyles(void)
{
    lv_style_t* cur = _lv_ll_get_head(&g_styleLL);
    lv_style_t* next = NULL;

    while (cur != NULL) {
        next = _lv_ll_get_next(&g_styleLL, cur);
        _lv_ll_remove(&g_styleLL, cur);
        lv_style_reset(cur);
        lv_mem_free(cur);
        cur = next;
    }
}

static void Deinit(lv_obj_t* par)
{
    if (!g_isInited) {
        return;
    }

    lv_obj_clean(par);
    ClearStyles();
    g_isInited = false;
}

static LvTestCaseInfo g_testCaseInfo = {
    "Img",
    Init,
    Deinit,
};

void LvRegisterDemoImg(void)
{
    RegisterTestCase(&g_testCaseInfo);
}

#endif