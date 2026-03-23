/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#include "lv_key_injector.h"
#include "lv_ll.h"
#if LV_USE_LOW_POWER_MANAGER
#include "lv_low_power_manager.h"
#endif
#include "misc/lv_assert.h"

static lv_ll_t g_keyInjectorDataLL;

void LvInitKeyInjector(void)
{
    _lv_ll_init(&g_keyInjectorDataLL, sizeof(lv_indev_data_t));
}

void LvKeyInjectorRead(lv_indev_drv_t* indevDrv, lv_indev_data_t* data)
{
    (void)indevDrv;
    if (_lv_ll_is_empty(&g_keyInjectorDataLL)) {
        return;
    }
    lv_indev_data_t* head = _lv_ll_get_head(&g_keyInjectorDataLL);
    data->key = head->key;
    data->state = head->state;
    _lv_ll_remove(&g_keyInjectorDataLL, head);
    lv_mem_free(head);
    return;
}

void LvKeyInjectorPush(uint32_t key, lv_indev_state_t state)
{
    lv_indev_data_t* data = _lv_ll_ins_tail(&g_keyInjectorDataLL);
    if (data == NULL) {
        LV_ASSERT_MALLOC(data);
        return;
    }
    lv_memset_00(data, sizeof(lv_indev_data_t));
    data->key = key;
    data->state = state;
#if LV_USE_LOW_POWER_MANAGER
    ExitLowPower();
#endif
    return;
}

bool LvIsKeyInjectorEmpty(void)
{
    return _lv_ll_is_empty(&g_keyInjectorDataLL);
}