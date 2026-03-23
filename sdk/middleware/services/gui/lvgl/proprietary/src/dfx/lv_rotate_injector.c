/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "lv_rotate_injector.h"
#include "lv_ll.h"
#if LV_USE_LOW_POWER_MANAGER
#include "lv_low_power_manager.h"
#endif

static lv_ll_t g_rotateInjectorDataLL;

void LvInitRotateInjector(void)
{
    _lv_ll_init(&g_rotateInjectorDataLL, sizeof(lv_indev_data_t));
}

void LvRotateInjectorRead(lv_indev_drv_t* indevDrv, lv_indev_data_t* data)
{
    (void)indevDrv;
    if (_lv_ll_is_empty(&g_rotateInjectorDataLL)) {
        return;
    }
    lv_indev_data_t* head = _lv_ll_get_head(&g_rotateInjectorDataLL);
    data->enc_diff = head->enc_diff;
    _lv_ll_remove(&g_rotateInjectorDataLL, head);
    lv_mem_free(head);
    return;
}

void LvRotateInjectorPush(int16_t rotate)
{
    lv_indev_data_t* data = _lv_ll_ins_tail(&g_rotateInjectorDataLL);
    if (data == NULL) {
        LV_ASSERT_MALLOC(data);
        return;
    }
    lv_memset_00(data, sizeof(lv_indev_data_t));
    data->enc_diff = rotate;
#if LV_USE_LOW_POWER_MANAGER
    ExitLowPower();
#endif
    return;
}

bool LvIsRotateInjectorEmpty(void)
{
    return _lv_ll_is_empty(&g_rotateInjectorDataLL);
}
