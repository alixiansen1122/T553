/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: dsoftbus ble gatt adapt src file.
 * Author: CompanyName
 * Create:
 */
#include "securec.h"
#include "wearable_log.h"
#include "ohos_bt_gatt_client.h"
#include "softbus_adapter_ble_gatt.h"
#include "softbus_adapter_ble_gatt_client.h"
#include "dsoftbus_ble_gatt.h"

static uint16_t g_ble_conn_id = 0;
static bd_addr_t g_ble_conn_addr = {0};

uint16_t softbus_ble_gatt_get_conn_id(void)
{
    return g_ble_conn_id;
}

bd_addr_t softbus_ble_gatt_get_conn_addr(void)
{
    return g_ble_conn_addr;
}

void softbus_ble_gatt_connect_change_callback(uint16_t conn_id, bd_addr_t *addr, gap_ble_conn_state_t conn_state,
    gap_ble_pair_state_t pair_state, gap_ble_disc_reason_t disc_reason)
{
    if (addr == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "connect state change addr NULL\n");
        return;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "connect state change conn_id: %d, status: %d, addr[0] = %x, reason %x\n",
        conn_id, conn_state, addr[0], disc_reason);

    g_ble_conn_id = conn_id;
    if (memcpy_s(g_ble_conn_addr.addr, BD_ADDR_LEN, addr->addr, BD_ADDR_LEN) != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "convert_adv_param memcpy fail");
        return;
    }

    BtConnectState ble_conn_state = (conn_state == GAP_BLE_STATE_CONNECTED) ? OHOS_STATE_CONNECTED :
        OHOS_STATE_DISCONNECTED;

    int32_t client_id = ble_gatt_get_client_id();
    SoftBusGattcConnectionStateChangedCallback(client_id, ble_conn_state, 0);
}

void softbus_ble_gatt_start_adv_callback(uint8_t adv_id, adv_status_t status)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "adv start adv_id: %d, status:%d\n", adv_id, status);

    /* 回调接口中status的含义应为retcode,0表示执行成功 */
    SoftBusAdvEnableCallback(adv_id, status);
}

void softbus_ble_gatt_stop_adv_callback(uint8_t adv_id, adv_status_t status)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "adv stop adv_id: %d, status:%d\n", adv_id, status);

    SoftBusAdvDisableCallback(adv_id, status);
}

void softbus_ble_gatt_set_adv_data_callback(uint8_t adv_id, errcode_t status)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "set adv data adv_id: %d, status:%d\n", adv_id, status);

    SoftBusAdvDataCallback(adv_id, status);
}

void softbus_ble_gatt_scan_result_callback(gap_scan_result_data_t *scan_result_data)
{
    if (scan_result_data == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "scan_result_data NULL\n");
        return;
    }

    /* ohos的scan result结构体与3321 scan reslut结构体中的地址成员类型不同 */
    BtScanResultData bt_scan_result_data;
    bt_scan_result_data.eventType = scan_result_data->event_type;
    bt_scan_result_data.dataStatus = scan_result_data->data_status;
    bt_scan_result_data.primaryPhy = scan_result_data->primary_phy;
    bt_scan_result_data.secondaryPhy = scan_result_data->secondary_phy;
    bt_scan_result_data.advSid = scan_result_data->adv_sid;
    bt_scan_result_data.txPower = scan_result_data->tx_power;
    bt_scan_result_data.rssi = scan_result_data->rssi;
    bt_scan_result_data.periodicAdvInterval = scan_result_data->periodic_adv_interval;
    bt_scan_result_data.advLen = scan_result_data->adv_len;
    bt_scan_result_data.advData = scan_result_data->adv_data;

    if (memcpy_s(bt_scan_result_data.addr.addr, BD_ADDR_LEN, scan_result_data->addr.addr, BD_ADDR_LEN) != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "softbus_ble_gatt_scan_result_callback memcpy addr fail");
        return;
    }

    if (memcpy_s(bt_scan_result_data.directAddr.addr, BD_ADDR_LEN,
        scan_result_data->direct_addr.addr, BD_ADDR_LEN) != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "convert_adv_param memcpy fail");
        return;
    }

    SoftBusScanResultCallback(&bt_scan_result_data);
}

void softbus_ble_gatt_set_scan_param_callback(errcode_t status)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "set scan param: status:%d\n", status);
}
