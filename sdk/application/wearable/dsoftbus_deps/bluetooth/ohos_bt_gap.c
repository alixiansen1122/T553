/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: ohos bt gap src file.
 * Author: CompanyName
 * Create:
 */
#include <stdio.h>
#include "common_def.h"
#include "bts_le_gap.h"
#include "bts_br_gap.h"
#include "wearable_log.h"
#include "ohos_bt_gap.h"

static gap_call_backs_t g_gap_callback;

static void bt_gap_state_changed_callback(const int transport, const int status)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "transport %d, status %d", transport, status);
}

static void bt_gap_acl_state_changed_callback(const bd_addr_t *bd_addr, gap_acl_state_t state, unsigned int reason)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "acl state %d, reason %x succ or err", state, reason);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "addr: %02x%02x%02x%02x",
        bd_addr->addr[3], bd_addr->addr[2], bd_addr->addr[1], bd_addr->addr[0]); /* 3 2 1 0 idx */
}

static void bt_gap_pair_requested_callback(const bd_addr_t *bd_addr)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "bt_gap_pair_requested_callback addr: ****%02x%02x%02x%02x\n",
        bd_addr->addr[3], bd_addr->addr[2], bd_addr->addr[1], bd_addr->addr[0]); /* 3 2 1 0 idx */
}

static void bt_gap_pair_confirmed_callback(const bd_addr_t *bd_addr, int req_type, int number)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "addr: ****%02x%02x%02x%02x req_type: %d number: %d\n",
        bd_addr->addr[3], bd_addr->addr[2], bd_addr->addr[1], bd_addr->addr[0], req_type, number); /* 3 2 1 0 idx */
}

int GapRegisterCallbacks(BtGapCallBacks *func)
{
    unused(func);

    g_gap_callback.state_change_callback = bt_gap_state_changed_callback;
    g_gap_callback.acl_state_changed_callbak = bt_gap_acl_state_changed_callback;
    g_gap_callback.pair_requested_callback = bt_gap_pair_requested_callback;
    g_gap_callback.pair_confiremed_callback = bt_gap_pair_confirmed_callback;

    errcode_t ret = gap_register_callbacks(&g_gap_callback);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gap_register_callbacks fail,ret = %d", ret);
        return OHOS_BT_STATUS_FAIL;
    }
    return OHOS_BT_STATUS_SUCCESS;
}

bool IsBleEnabled()
{
    return true;
}

bool GetLocalAddr(unsigned char *mac, unsigned int len)
{
    errcode_t ret = bluetooth_get_local_addr(mac, len);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "bluetooth_get_local_addr fail,ret = %d", ret);
        return false;
    }
    return true;
}

bool SetDevicePairingConfirmation(const BdAddr *bdAddr, int transport, bool accept)
{
    return true;
}

bool PairRequestReply(const BdAddr *bdAddr, int transport, bool accept)
{
    return true;
}

