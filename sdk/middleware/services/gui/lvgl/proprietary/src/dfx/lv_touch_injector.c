/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#include "lv_touch_injector.h"
#include "lv_ll.h"
#if LV_USE_LOW_POWER_MANAGER
#include "lv_low_power_manager.h"
#endif

static lv_ll_t g_touchInjectorDataLL;

void LvInitTouchInjector(void)
{
    _lv_ll_init(&g_touchInjectorDataLL, sizeof(lv_indev_data_t));
}

void LvTouchInjectorRead(lv_indev_drv_t* indevDrv, lv_indev_data_t* data)
{
    (void)indevDrv;
    if (_lv_ll_is_empty(&g_touchInjectorDataLL)) {
        return;
    }
    lv_indev_data_t* head = _lv_ll_get_head(&g_touchInjectorDataLL);
    data->point = head->point;
    data->state = head->state;
    _lv_ll_remove(&g_touchInjectorDataLL, head);
    lv_mem_free(head);
    return;
}

void LvTouchInjectorPush(lv_point_t point, lv_indev_state_t state)
{
    lv_indev_data_t* data = _lv_ll_ins_tail(&g_touchInjectorDataLL);
    if (data == NULL) {
        LV_ASSERT_MALLOC(data);
        return;
    }
    lv_memset_00(data, sizeof(lv_indev_data_t));
    data->point = point;
    data->state = state;
#if LV_USE_LOW_POWER_MANAGER
    ExitLowPower();
#endif
    return;
}

bool LvIsTouchInjectorEmpty(void)
{
    return _lv_ll_is_empty(&g_touchInjectorDataLL);
}
