/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "lv_low_power_manager.h"
#include "graphic_service_wrapper.h"
#include "lv_anim.h"
#include "misc/lv_gc.h"

#if LV_USE_LOW_POWER_MANAGER

static bool IsPendingRefr(void)
{
    lv_disp_t* disp = _lv_ll_get_head(&LV_GC_ROOT(_lv_disp_ll));
    while (disp != NULL) {
        if (disp->inv_p != 0) {
            return true;
        }
        disp = _lv_ll_get_next(&LV_GC_ROOT(_lv_disp_ll), disp);
    }
    return false;
}

void TryToEnterLowPower(void)
{
    if (lv_anim_count_running() != 0) {
        return;
    }

    // 2: lvgl's timer: anim, refr.
    if ((_lv_ll_get_len(&LV_GC_ROOT(_lv_timer_ll)) - _lv_ll_get_len(&LV_GC_ROOT(_lv_indev_ll))) > 2) {
        return;
    }

    if (!LvIsTouchEventEmpty() || !LvIsKeyEventEmpty() || !LvIsRotateEventEmpty() ||
        !LvIsTouchInjectorEmpty() || !LvIsKeyInjectorEmpty() || !LvIsRotateInjectorEmpty()) {
        return;
    }

#if LV_USE_SURFACE_VIEW
    if (LvObtainSurfaceView() != NULL) {
        return;
    }
#endif

    if (IsPendingRefr()) {
        return;
    }

    if (LvHasPendingFrame()) {
        return;
    }
    SetFrequence(1);
}

void ExitLowPower(void)
{
    SetFrequence(60); // 60: fps
}

#endif