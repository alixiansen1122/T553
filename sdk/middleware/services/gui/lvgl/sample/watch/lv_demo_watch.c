/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "lv_demo_watch.h"
#include "notification.h"
#include "control.h"
#include "tools.h"
#include "activity.h"
#include "app.h"
#include "lv_demo_watch_font.h"
#include "lv_cross_view.h"

#if LV_USE_DEMO_WATCH
#define LV_DEMO_WATCH_ZOOM_EFFECT

#ifdef LV_DEMO_WATCH_PAGE_FILP_EFFECT
#include "lv_page_flip_effect.h"
#elif defined(LV_DEMO_WATCH_TURN_PAGE_EFFECT)
#include "lv_turn_page_effect.h"
#elif defined(LV_DEMO_WATCH_ZOOM_EFFECT)
#include "lv_zoom_effect.h"
#endif

lv_obj_t* g_cv = NULL;

#define LV_DEMO_WATCH_USE_FULLSCREENBLUR
#ifdef LV_DEMO_WATCH_USE_FULLSCREENBLUR
#define MAX_BLUR_RADIUS 10
#define MIN_BLUR_RADIUS 0
static uint16_t GetBlurRadiusCb(lv_coord_t distance)
{
    uint16_t blurRadius = LV_ABS(distance) /
        (lv_obj_get_content_height(g_cv) / (MAX_BLUR_RADIUS - MIN_BLUR_RADIUS)) + MIN_BLUR_RADIUS;
    return blurRadius;
}
#endif

void InitMain(lv_obj_t* parent)
{
    g_cv = LvCrossViewCreate(parent);
    lv_obj_t* pageC = LvCrossViewAddHorPage(g_cv, GetClockPageInfo(), true);
    LvglCreateClock(pageC);

    lv_obj_t* pageR = LvCrossViewAddHorPage(g_cv, GetActivityPageInfo(), false);
    LvglCreateActivity(pageR);

    lv_obj_t* pageL = LvCrossViewInsertHorPage(g_cv, pageC, GetToolsPageInfo(), false);
    LvglCreateTools(pageL);

    lv_obj_t* pageT = LvCrossViewAddVerPage(g_cv, GetNotificationPageInfo(), true);
    LvglCreateNotification(pageT);

    lv_obj_t* pageB = LvCrossViewAddVerPage(g_cv, GetControlPageInfo(), false);
    LvglCreateControl(pageB);

#ifndef MEMORY_MINI
#ifdef LV_DEMO_WATCH_PAGE_FILP_EFFECT
    LvPageFlipScrollEffectInit();
    LvCrossViewSetScrollEffect(g_cv, LvGetPageFlipScrollEffect());
#elif defined(LV_DEMO_WATCH_TURN_PAGE_EFFECT)
    LvTurnPageScrollEffectInit();
    LvCrossViewSetScrollEffect(g_cv, LvGetTurnPageScrollEffect());
#elif defined(LV_DEMO_WATCH_ZOOM_EFFECT)
    LvCrossViewSetScrollEffect(g_cv, LvGetZoomScrollEffect());
#endif
    LvCrossViewSetScreenCap(g_cv, true);
#endif
    LvCrossViewSetHorActPage(g_cv, pageC);
    LvCrossViewSetVerBlur(g_cv, true);

#ifdef LV_DEMO_WATCH_USE_FULLSCREENBLUR
    LvCrossViewSetVerGlobalBlur(g_cv, GetBlurRadiusCb);
#endif
}

void LvDemoWatch(void)
{
    InitLvFontRes();
    InitMain(lv_scr_act());
}
#endif
