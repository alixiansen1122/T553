/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "lv_demo_zoom_img.h"
#include "lv_demo_ext.h"

#if LV_USE_DEMO_ZOOM_IMG
static const uint16_t ANI_TIME = 1000;
static const char* DEFAULT_SRC = "/user/res/haha.bin";
static bool g_isInited = false;

static void ImgZoomCb(void* var, int32_t v)
{
    lv_obj_invalidate(var);
    lv_img_set_zoom(var, (uint16_t)v);
}

void LvDemoZoomImg(lv_obj_t* par)
{
    lv_obj_t* img = lv_img_create(par);
    lv_obj_set_pos(img, 200, 200); // 200: pos
    LvImgSetSrcExt(img, DEFAULT_SRC);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_values(&a, 64, 1024); // 64, 1024: zoom
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&a, ImgZoomCb);
    lv_anim_set_var(&a, img);
    lv_anim_set_time(&a, ANI_TIME);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
    lv_anim_start(&a);
}

static void Init(lv_obj_t* par)
{
    if (g_isInited) {
        return;
    }
    g_isInited = true;
    LvDemoZoomImg(par);
}

static void Deinit(lv_obj_t* par)
{
    if (!g_isInited) {
        return;
    }
    lv_obj_clean(par);
    g_isInited = false;
}

static LvTestCaseInfo g_testCaseInfo = {
    "Zoom Img",
    Init,
    Deinit,
};

void LvRegisterDemoZoomImg(void)
{
    RegisterTestCase(&g_testCaseInfo);
}
#endif