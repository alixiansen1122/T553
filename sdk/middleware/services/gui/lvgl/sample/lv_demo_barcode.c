/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "lv_demo_barcode.h"
#include "lv_demo_ext.h"
#include "lv_barcode.h"

#if LV_USE_DEMO_BARCODE

static lv_obj_t* g_barcode = NULL;
static int32_t g_index = -1;
static bool g_isInited = false;

typedef struct {
    lv_point_t size;
    const char* data;
} BarcodeTestInfo;

#define BARCODE_TEST_NUM 4
static BarcodeTestInfo testInfo[BARCODE_TEST_NUM] = {
    {(lv_point_t){400, 100}, "BARCODE1"},
    {(lv_point_t){300, 200}, "BARCODE1"},
    {(lv_point_t){400, 100}, "BARCODE2"},
    {(lv_point_t){400, 100}, "BARCODE321"},
};

static void BarcodeCb(void* var, int32_t v)
{
    if (v == g_index) {
        return;
    }

    g_index = v;
    printf("======Begin Test Barcode[%d]: char* = %s: w = %d, h = %d======\n",
        g_index, testInfo[g_index].data, testInfo[g_index].size.x, testInfo[g_index].size.y);
    LvBarcodeUpdate(g_barcode, testInfo[g_index].data, testInfo[g_index].size.x, testInfo[g_index].size.y);
}

void LvDemoBarcode(lv_obj_t* par)
{
    lv_obj_set_style_bg_color(par, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_style_bg_opa(par, LV_OPA_COVER, 0);

    g_barcode = LvBarcodeCreate(par);
    lv_obj_align(g_barcode, LV_ALIGN_CENTER, 0, 0);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_repeat_count(&a, 1);
    lv_anim_set_exec_cb(&a, BarcodeCb);
    lv_anim_set_var(&a, g_barcode);
    lv_anim_set_values(&a, 0, BARCODE_TEST_NUM - 1);
    lv_anim_set_path_cb(&a, &lv_anim_path_linear);
    lv_anim_set_time(&a, 20000); // 20000: anim time
    lv_anim_start(&a);
}

static void Init(lv_obj_t* par)
{
    if (g_isInited) {
        return;
    }

    g_isInited = true;
    LvDemoBarcode(par);
}

static void Deinit(lv_obj_t* par)
{
    if (!g_isInited) {
        return;
    }
    lv_anim_del(g_barcode, NULL);
    g_barcode = NULL;
    g_index = -1;
    lv_obj_clean(par);
    g_isInited = false;
}

static LvTestCaseInfo g_testCaseInfo = {
    "Barcode",
    Init,
    Deinit,
};

void LvRegisterDemoBarcode(void)
{
    RegisterTestCase(&g_testCaseInfo);
}

#endif