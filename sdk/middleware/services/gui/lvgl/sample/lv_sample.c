/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#include "cmsis_os.h"
#include "lvgl.h"
#include "lv_display_device.h"
#include "lv_port_disp.h"
#include "graphic_service_wrapper.h"
#include "lv_sample.h"

#if LV_USE_DEMO_EXT
#include "lv_demo_ext.h"
#endif
#if LV_USE_DEMO_SURFACE
#include "lv_demo_surface.h"
#endif
#if LV_USE_DEMO_WATCH
#include "lv_demo_watch.h"
#endif
#if defined(LV_USE_DEMO_VIDEO_PLAY) || defined(LV_USE_DEMO_VIDEO_DIAL)
#include "video_player_sample_wrapper.h"
#endif

static bool g_initialized = false;

static void LvglDemoCase(void* userData)
{
    (void)userData;
#if LV_USE_DEMO_EXT
    LvDemoExt();
    return;
#endif
#if LV_USE_DEMO_WATCH
    LvDemoWatch();
    return;
#endif
#if LV_USE_DEMO_SURFACE
    LvDemoSurface();
    return;
#endif
#if LV_USE_DEMO_VIDEO_PLAY
    LvDemoVideoPlay();
    return;
#endif
#if LV_USE_DEMO_VIDEO_DIAL
    LvDemoVideoDial();
    return;
#endif

#if LV_USE_DEMO_WIDGETS
    printf("===Begin Demo Widgets===\n");
    lv_demo_widgets();
#elif LV_USE_DEMO_KEYPAD_AND_ENCODER
    printf("===Begin Demo Encoder===\n");
    lv_demo_keypad_encoder();
#elif LV_USE_DEMO_BENCHMARK
    printf("===Begin Demo Benchmark===\n");
    lv_demo_benchmark();
#elif LV_USE_DEMO_STRESS
    printf("===Begin Demo Stress===\n");
    lv_demo_stress();
#elif LV_USE_DEMO_MUSIC
    printf("===Begin Demo Music===\n");
    lv_demo_music();
#endif
}

void LvglSample(void)
{
    if (g_initialized) {
        return;
    }
    printf("===Begin lvglSample===\n");
    GraphicStartMain();
    PostGraphicEvent(LvglDemoCase, NULL, true);
    g_initialized = true;
}