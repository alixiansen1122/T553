/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "lv_demo_scroll_text.h"
#include "lvgl.h"
#if LV_USE_FONT_VECTOR
#include "lv_font_vector.h"
#endif
#include "lv_demo_ext.h"

#if LV_USE_DEMO_SCROLL_TEXT

static const uint16_t VECTOR_FONT_SIZE = 36;
static const uint16_t ANI_TIME = 1000;
static lv_font_t* g_font36 = NULL;
static int8_t g_count = 0;
static int8_t g_scroll = -4;
static bool g_isInited = false;

static void ScrollCb(void* var, int32_t v)
{
    g_count++;
    if (g_count == 50) { // 50: limit
        g_count = 0;
        g_scroll *= -1;
    }
    lv_obj_scroll_by(var, 0, g_scroll, false);
}

void LvDemoScrollText(lv_obj_t* par)
{
    lv_obj_t* label = lv_label_create(par);
    lv_obj_set_scrollbar_mode(label, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_pos(label, 0, 0);
    lv_obj_set_size(label, lv_disp_get_hor_res(_lv_refr_get_disp_refreshing()),
        lv_disp_get_ver_res(_lv_refr_get_disp_refreshing()));

#if LV_USE_FONT_VECTOR
    LvInitFontExt();
    g_font36 = LvFontLoadExt(DEFAULT_VECTOR_FONT_FILENAME, VECTOR_FONT_SIZE);
    lv_obj_set_style_text_font(label, g_font36, 0);
#endif
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);

    lv_obj_set_style_bg_opa(par, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_opa(label, LV_OPA_TRANSP, 0);

#if defined(LV_USE_DEMO_SCROLL_TEXT_CN)
    lv_label_set_text(label,
        "36号全屏字体测试用例。请欣赏现代诗人汪国真发表的我微笑着走向生活。"
        "我微笑着走向生活，无论生活以什么方式回敬我。报我以平坦吗？我是一条欢乐奔流的小河。"
        "报我以崎岖吗？我是一座庄严思索的大山。报我以幸福吗？我是一只凌空飞翔的燕子。"
        "报我以不幸吗？我是一根劲竹经得起千击万磨。生活里不能没有笑声，没有笑声的世界该是多么寂寞。"
        "什么也改变不了我对生活的热爱，我微笑着走向火热的生活！"
        "现代诗词已全部展示完毕。");
#elif defined(LV_USE_DEMO_SCROLL_TEXT_EN)
    lv_label_set_text(label,
        "Do not go gentle into that good night,"
        "Old age should burn and rave at close of day;"
        "Rage, rage against the dying of the light."
        "Though wise men at their end know dark is right,"
        "Because their words had forked no lightning they"
        "Do not go gentle into that good night."
        "Good men, the last wave by, crying how bright."
        "Their frail deeds might have danced in a green bay,"
        "Rage, rage against the dying of the light."
        "Wild men who caught and sang the sun in flight,"
        "And learn, too late, they grieved it on its way,"
        "Do not go gentle into that good night."
        "Grave men, near death, who see with blinding sight");
#endif

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&a, ScrollCb);
    lv_anim_set_var(&a, label);
    lv_anim_set_time(&a, ANI_TIME);
    lv_anim_start(&a);
}

static void Init(lv_obj_t* par)
{
    if (g_isInited) {
        return;
    }
    g_isInited = true;
    LvDemoScrollText(par);
}

static void Deinit(lv_obj_t* par)
{
    if (!g_isInited) {
        return;
    }
    lv_obj_clean(par);
#if LV_USE_FONT_VECTOR
    LvFontFreeExt(g_font36);
    g_font36 = NULL;
#endif
    g_count = 0;
    g_scroll = -4; // -4: default value
    g_isInited = false;
}

static LvTestCaseInfo g_testCaseInfo = {
    "Scroll Text",
    Init,
    Deinit,
};

void LvRegisterDemoScrollText(void)
{
    RegisterTestCase(&g_testCaseInfo);
}
#endif