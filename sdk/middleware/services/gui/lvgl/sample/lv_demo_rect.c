/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "lv_demo_rect.h"
#include "lv_demo_ext.h"

#if LV_USE_DEMO_RECT
typedef struct {
    lv_opa_t borderOpa;
    lv_coord_t borderWidth;
    lv_coord_t radius;
} RectTest;

static RectTest g_rectTestList[32] = { // 32: testcase size
    {LV_OPA_COVER, 2, 7}, // 2, 7: borderWidth and radius
    {LV_OPA_COVER, 2, 10}, // 2, 10: borderWidth and radius
    {LV_OPA_COVER, 2, 20}, // 2, 20: borderWidth and radius
    {LV_OPA_COVER, 2, 30}, // 2, 30: borderWidth and radius
    {LV_OPA_COVER, 2, 40}, // 2, 40: borderWidth and radius
    {LV_OPA_COVER, 2, 50}, // 2, 50: borderWidth and radius
    {LV_OPA_COVER, 1, 50}, // 1, 50: borderWidth and radius
    {LV_OPA_COVER, 3, 50}, // 3, 50: borderWidth and radius
    {LV_OPA_COVER, 4, 50}, // 4, 50: borderWidth and radius
    {LV_OPA_COVER, 48, 50}, // 48, 50: borderWidth and radius
    {LV_OPA_COVER, 49, 50}, // 49, 50: borderWidth and radius
    {LV_OPA_COVER, 50, 50}, // 50, 50: borderWidth and radius
    {LV_OPA_COVER, 50, 100}, // 50, 100: borderWidth and radius
    {LV_OPA_COVER, 0, 7}, // 0, 7: borderWidth and radius
    {LV_OPA_COVER, 0, 50}, // 0, 50: borderWidth and radius
    {LV_OPA_COVER, 0, 100}, // 0, 100: borderWidth and radius
    {LV_OPA_50, 2, 7}, // 2, 7: borderWidth and radius
    {LV_OPA_50, 2, 10}, // 2, 10: borderWidth and radius
    {LV_OPA_50, 2, 20}, // 2, 20: borderWidth and radius
    {LV_OPA_50, 2, 30}, // 2, 30: borderWidth and radius
    {LV_OPA_50, 2, 40}, // 2, 40: borderWidth and radius
    {LV_OPA_50, 2, 50}, // 2, 50: borderWidth and radius
    {LV_OPA_50, 1, 50}, // 1, 50: borderWidth and radius
    {LV_OPA_50, 3, 50}, // 3, 50: borderWidth and radius
    {LV_OPA_50, 4, 50}, // 4, 50: borderWidth and radius
    {LV_OPA_50, 48, 50}, // 48, 50: borderWidth and radius
    {LV_OPA_50, 49, 50}, // 49, 50: borderWidth and radius
    {LV_OPA_50, 50, 50}, // 50, 50: borderWidth and radius
    {LV_OPA_50, 50, 100}, // 50, 100: borderWidth and radius
    {LV_OPA_50, 0, 7}, // 0, 7: borderWidth and radius
    {LV_OPA_50, 0, 50}, // 0, 50: borderWidth and radius
    {LV_OPA_50, 0, 100}, // 0, 100: borderWidth and radius
};

static uint32_t g_curTestIndex = 0;
static uint32_t g_curTime = 0;
static bool g_isInited = false;

static void RectCb(void* var, int32_t v)
{
    if (g_curTime == 0) {
        g_curTime = lv_tick_get();
    }
    if (lv_tick_elaps(g_curTime) > 3000) { // 3000: 3s, time elaps
        lv_obj_t* rect = (lv_obj_t*)var;
        lv_obj_set_style_border_width(rect, g_rectTestList[g_curTestIndex].borderWidth, 0);
        lv_obj_set_style_border_opa(rect, g_rectTestList[g_curTestIndex].borderOpa, 0);
        lv_obj_set_style_radius(rect, g_rectTestList[g_curTestIndex].radius, 0);
        printf("Test Rect[%u]: borderWidth = %d, borderOpa = %u, radius = %d\n",
            g_curTestIndex,
            g_rectTestList[g_curTestIndex].borderWidth,
            g_rectTestList[g_curTestIndex].borderOpa,
            g_rectTestList[g_curTestIndex].radius);
        g_curTestIndex++;
        if (g_curTestIndex == 32) { // 32: testcase size
            g_curTestIndex = 0;
            printf("===============END Test==============\n");
        }
        g_curTime = lv_tick_get();
    }
}

void LvDemoRect(lv_obj_t* par)
{
    lv_obj_t* rect = lv_obj_create(par);
    lv_obj_set_size(rect, 200, 200); // 200: width and height
    lv_obj_set_pos(rect, 100, 100); // 100: size
    lv_obj_set_style_bg_color(rect, lv_palette_main(LV_PALETTE_ORANGE), 0);
    lv_obj_set_style_border_post(rect, false, 0);
    lv_obj_set_style_border_opa(rect, 150, 0); // 150: opa
    lv_obj_set_style_border_color(rect, lv_palette_main(LV_PALETTE_BLUE), 0);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&a, RectCb);
    lv_anim_set_var(&a, rect);
    lv_anim_set_time(&a, 1000); // 1000: time
    lv_anim_start(&a);
}

static void Init(lv_obj_t* par)
{
    if (g_isInited) {
        return;
    }
    g_isInited = true;
    LvDemoRect(par);
}

static void Deinit(lv_obj_t* par)
{
    if (!g_isInited) {
        return;
    }
    lv_obj_clean(par);
    g_curTestIndex = 0;
    g_curTime = 0;
    g_isInited = false;
}

static LvTestCaseInfo g_testCaseInfo = {
    "Rect",
    Init,
    Deinit,
};

void LvRegisterDemoRect(void)
{
    RegisterTestCase(&g_testCaseInfo);
}
#endif