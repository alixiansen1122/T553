/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "lv_demo_kaleidoscope.h"
#include "lv_kaleidoscope_utils.h"
#include "lv_kaleidoscope.h"
#include "lv_img_ext.h"
#include "lv_demo_ext.h"

#if LV_USE_DEMO_KALEIDOSCOPE
#if DEMO_KALEIDOSCOPE_ANIM
static lv_anim_t g_anim = {0};
#else
static lv_obj_t* g_posBtn = NULL;
static lv_obj_t* g_negBtn = NULL;
static const lv_coord_t BTN_W = 227;
static const lv_coord_t BTN_H = 100;
#endif
static const int16_t ROTATE_VAL = 10;
static bool g_isInited = false;

#if DEMO_KALEIDOSCOPE_UTILS
static lv_obj_t* g_img = NULL;
static LvKaleidoscopeInfo g_info = {0};
#else
static lv_obj_t* g_kaleidoscope = NULL;
#endif
static lv_font_t* g_font = NULL;

static void KaleidoscopeAnimCb(void* var, int32_t value)
{
#if DEMO_KALEIDOSCOPE_UTILS
    LvUpdateKaleidoscopeInfo(&g_info, ROTATE_VAL);
    lv_obj_invalidate(g_img);
#else
    LvKaleidoscopeRotate(g_kaleidoscope, ROTATE_VAL);
#endif
}

#if !DEMO_KALEIDOSCOPE_ANIM
static void EventCb(lv_event_t * e)
{
    lv_obj_t* obj = lv_event_get_target(e);
#if DEMO_KALEIDOSCOPE_UTILS
    if (obj == g_posBtn) {
        LvUpdateKaleidoscopeInfo(&g_info, ROTATE_VAL);
    } else if (obj == g_negBtn) {
        LvUpdateKaleidoscopeInfo(&g_info, -ROTATE_VAL);
    }
    lv_obj_invalidate(g_img);
#else
    if (obj == g_posBtn) {
        LvKaleidoscopeRotate(g_kaleidoscope, ROTATE_VAL);
    } else if (obj == g_negBtn) {
        LvKaleidoscopeRotate(g_kaleidoscope, -ROTATE_VAL);
    }
#endif
}
#endif

void LvDemoKaleidoscope(lv_obj_t* par)
{
    uint32_t resId = LvLoadImg("/user/res/rabbit454.bin", true);

#if DEMO_KALEIDOSCOPE_UTILS
    LvInitKaleidoscopeInfo(&g_info, resId, NULL);
    LvUpdateKaleidoscopeInfo(&g_info, ROTATE_VAL);
    g_img = lv_img_create(par);
    lv_obj_center(g_img);
    lv_img_set_src(g_img, LvGetKaleidoscopeDsc(&g_info));
#else
    g_kaleidoscope = LvKaleidoscopeCreate(par);
    lv_obj_set_size(g_kaleidoscope, LV_HOR_RES, LV_VER_RES);
    lv_obj_center(g_kaleidoscope);
    LvKaleidoscopeSetSrc(g_kaleidoscope, resId);
#endif

#if DEMO_KALEIDOSCOPE_ANIM
    lv_anim_init(&g_anim);
    lv_anim_set_repeat_count(&g_anim, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&g_anim, KaleidoscopeAnimCb);
    lv_anim_set_time(&g_anim, 1000); // 1000: time
    lv_anim_start(&g_anim);
#else
    LvInitFontExt();
    g_font = LvFontLoadExt(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: font size

    g_posBtn = lv_label_create(par);
    lv_obj_set_size(g_posBtn, BTN_W, BTN_H);
    lv_obj_set_pos(g_posBtn, 0, 350); // 350: y
    lv_obj_add_flag(g_posBtn, LV_OBJ_FLAG_CLICKABLE);
    lv_label_set_text(g_posBtn, "POS");
    lv_obj_set_style_text_color(g_posBtn, lv_palette_main(LV_PALETTE_DEEP_ORANGE), 0);
    lv_obj_set_style_text_align(g_posBtn, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(g_posBtn, g_font, 0);
    lv_obj_add_event_cb(g_posBtn, EventCb, LV_EVENT_PRESSING, NULL);

    g_negBtn = lv_label_create(par);
    lv_obj_set_size(g_negBtn, BTN_W, BTN_H);
    lv_obj_set_pos(g_negBtn, 227, 350); // 227:x 350: y
    lv_obj_add_flag(g_negBtn, LV_OBJ_FLAG_CLICKABLE);
    lv_label_set_text(g_negBtn, "NEG");
    lv_obj_set_style_text_color(g_negBtn, lv_palette_main(LV_PALETTE_DEEP_ORANGE), 0);
    lv_obj_set_style_text_align(g_negBtn, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(g_negBtn, g_font, 0);
    lv_obj_add_event_cb(g_negBtn, EventCb, LV_EVENT_PRESSING, NULL);
#endif
}

static void Init(lv_obj_t* par)
{
    if (g_isInited) {
        return;
    }
    g_isInited = true;
    LvDemoKaleidoscope(par);
}

static void Deinit(lv_obj_t* par)
{
    if (!g_isInited) {
        return;
    }

#if DEMO_KALEIDOSCOPE_UTILS
#if LV_USE_IMG_CACHE_EXT
    LvImgCacheInvalidateSrc(lv_img_get_src(g_img));
#else
    lv_img_cache_invalidate_src(lv_img_get_src(g_img));
#endif
    g_img = NULL;
#else
    g_kaleidoscope = NULL;
#endif

#if DEMO_KALEIDOSCOPE_ANIM
    lv_anim_custom_del(&g_anim, NULL);
#else
    g_posBtn = NULL;
    g_negBtn = NULL;
    LvDeinitKaleidoscopeInfo(g_info);
#endif

    lv_obj_clean(par);
    LvFontFreeExt(g_font);
    g_font = NULL;
    g_isInited = false;
}

static LvTestCaseInfo g_testCaseInfo = {
    "Kaleidoscope",
    Init,
    Deinit,
};

void LvRegisterDemoKaleidoscope(void)
{
    RegisterTestCase(&g_testCaseInfo);
}
#endif // LV_USE_DEMO_KALEIDOSCOPE