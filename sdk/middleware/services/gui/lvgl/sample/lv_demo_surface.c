/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#include "lv_demo_surface.h"

#if LV_USE_DEMO_SURFACE
#include "lv_surface_view.h"

static LvSurfaceView* g_surface = NULL;
static bool g_isThreadStarted = false;

static void FlushCb(lv_event_t * e)
{
    if (g_surface == NULL) {
        return;
    }
    lv_obj_t* obj = lv_event_get_target(e);
    if (lv_obj_has_state(obj, LV_STATE_PRESSED)) {
        LvSurfaceBuffer* buffer = LvRequestBuffer(g_surface, 1);
        printf("[%s: %d] flush buffer(addr = %p)\n", __FUNCTION__, __LINE__, (uint8_t*)buffer->addr);
        LvFlushBuffer(g_surface, buffer);
    }
}

static void SurfaceSetUp(lv_obj_t* parent)
{
    lv_obj_t* obj = LvSurfaceViewCreate(parent);
    LvSurfaceView* surface = (LvSurfaceView*)obj;
    g_surface = surface;
#if LV_USE_DEMO_SURFACE_FULL_SIZE
    LvInitSurfaceView(surface, (lv_point_t){0, 0}, 454, 454); // 454: width and height
#else
    LvInitSurfaceView(surface, (lv_point_t){0, 0}, 256, 256); // 256: width and height
#endif
}

static void SurfaceTestThread(void* args)
{
    while (1) {
        LvSurfaceBuffer* buffer = LvRequestBuffer(g_surface, 1);
        printf("[%s: %d] buffer addr = %p\n", __FUNCTION__, __LINE__, (uint8_t*)buffer->addr);
        LvFlushBuffer(g_surface, buffer);
        usleep(15 * 1000); // 15: ms, 1000: unit conversion
    }
}

static void ThreadCb(lv_event_t * e)
{
    if ((g_surface == NULL) || (g_isThreadStarted)) {
        return;
    }
    lv_obj_t* obj = lv_event_get_target(e);
    if (lv_obj_has_state(obj, LV_STATE_PRESSED)) {
        g_isThreadStarted = true;
        printf("=======Start Surface Thread=======\n");
        osThreadAttr_t osAttr = {0};
        osAttr.stack_size = 0x1000u; // 0x1000u: stack size
        osAttr.name = "surface test";
        osAttr.priority = osPriorityHigh;
        osThreadNew((osThreadFunc_t)SurfaceTestThread, NULL, &osAttr);
    }
}

static void DeleteCb(lv_event_t * e)
{
    if ((g_surface == NULL) || (g_isThreadStarted)) {
        return;
    }
    lv_obj_t* obj = lv_event_get_target(e);
    if (lv_obj_has_state(obj, LV_STATE_PRESSED)) {
        printf("=======Delete Surface View=======\n");
        lv_obj_del((lv_obj_t*)g_surface);
        g_surface = NULL;
    }
}

static void TestBtnSetUp(lv_obj_t* parent)
{
    /* request and flush once when the btn is clicked */
    lv_obj_t* btn = lv_btn_create(parent);
    lv_obj_set_height(btn, LV_SIZE_CONTENT);

    lv_obj_t* label = lv_label_create(btn);
    lv_obj_set_size(label, 100, 50); // 100, 50: width and height
    lv_label_set_text(label, "flush");
    lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_ORANGE), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_align_to(btn, parent, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(btn, FlushCb, LV_EVENT_PRESSED, NULL);

    /* start a thread which will constantly request and flush buffer */
    lv_obj_t* btn1 = lv_btn_create(parent);
    lv_obj_set_height(btn1, LV_SIZE_CONTENT);

    lv_obj_t* label1 = lv_label_create(btn1);
    lv_obj_set_size(label1, 100, 50); // 100, 50: width and height
    lv_label_set_text(label1, "Start Thread");
    lv_obj_set_style_text_color(label1, lv_palette_main(LV_PALETTE_ORANGE), 0);
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_align_to(btn1, btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 10); // 10: gap
    lv_obj_add_event_cb(btn1, ThreadCb, LV_EVENT_PRESSED, NULL);

    /* delete surface */
    lv_obj_t* btn2 = lv_btn_create(parent);
    lv_obj_set_height(btn2, LV_SIZE_CONTENT);

    lv_obj_t* label2 = lv_label_create(btn2);
    lv_obj_set_size(label2, 100, 50); // 100, 50: width and height
    lv_label_set_text(label2, "Delete Surface");
    lv_obj_set_style_text_color(label2, lv_palette_main(LV_PALETTE_ORANGE), 0);
    lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_align_to(btn2, btn, LV_ALIGN_OUT_RIGHT_MID, 10, 0); // 10: gap
    lv_obj_add_event_cb(btn2, DeleteCb, LV_EVENT_PRESSED, NULL);
}

void LvDemoSurface()
{
    printf("=======Start Demo Surface=======\n");
    lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(lv_scr_act(), lv_palette_main(LV_PALETTE_GREY), 0);
    SurfaceSetUp(lv_scr_act());
    TestBtnSetUp(lv_scr_act());
}
#endif
