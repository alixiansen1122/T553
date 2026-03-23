/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: ohos bt gatt src file.
 * Author: CompanyName
 * Create:
 */
#include <stdio.h>
#include "securec.h"
#include "common_def.h"
#include "bts_le_gap.h"
#include "wearable_log.h"
#include "ohos_bt_gatt_porting.h"
#include "ohos_bt_gatt.h"

#define BLE_SCAN_INTERVAL 48
#define BLE_SCAN_WINDOW   48

static gap_ble_callbacks_t g_gap_ble_callback;

int BleGattRegisterCallbacks(BtGattCallbacks *func)
{
    unused(func);

    g_gap_ble_callback.start_adv_cb = ble_gatt_start_adv_callback;
    g_gap_ble_callback.stop_adv_cb = ble_gatt_stop_adv_callback;
    g_gap_ble_callback.set_adv_data_cb = ble_gatt_set_adv_data_callback;
    g_gap_ble_callback.scan_result_cb = ble_gatt_scan_result_callback;
    g_gap_ble_callback.set_scan_param_cb = ble_gatt_set_scan_param_callback;
    g_gap_ble_callback.conn_state_change_cb = ble_gatt_connect_change_callback;

    errcode_t ret = gap_ble_register_callbacks(&g_gap_ble_callback);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gap_ble_register_callbacks fail ret %d", ret);
        return OHOS_BT_STATUS_FAIL;
    }
    return OHOS_BT_STATUS_SUCCESS;
}

static unsigned char convert_adv_type(unsigned char adv_type)
{
    switch (adv_type) {
        case OHOS_BLE_ADV_IND:
            return GAP_BLE_ADV_CONN_SCAN_UNDIR;
        case OHOS_BLE_ADV_NONCONN_IND:
            return GAP_BLE_ADV_NONCONN_SCAN_UNDIR;
        default:
            return GAP_BLE_ADV_CONN_SCAN_UNDIR;
    }
    return GAP_BLE_ADV_CONN_SCAN_UNDIR;
}

static unsigned char convert_adv_filter(unsigned char adv_filter)
{
    switch (adv_filter) {
        case OHOS_BLE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY:
            return GAP_BLE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;
        case OHOS_BLE_ADV_FILTER_ALLOW_SCAN_WLST_CON_ANY:
            return GAP_BLE_ADV_FILTER_ALLOW_SCAN_WLST_CON_ANY;
        case OHOS_BLE_ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST:
            return GAP_BLE_ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST;
        case OHOS_BLE_ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST:
            return GAP_BLE_ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST;
        default:
            return GAP_BLE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;
    }
}

static void convert_adv_data(const StartAdvRawData *src, gap_ble_config_adv_data_t *dst)
{
    if (src == NULL || dst == NULL) {
        return;
    }

    dst->adv_length = src->advDataLen;
    dst->adv_data = (uint8_t*)src->advData;
    dst->scan_rsp_length = src->rspDataLen;
    dst->scan_rsp_data = (uint8_t*)src->rspData;
}

static void convert_adv_param(BleAdvParams *src, gap_ble_adv_params_t *dst)
{
    if (src == NULL || dst == NULL) {
        return;
    }

    dst->min_interval = src->minInterval;
    dst->max_interval = src->maxInterval;
    dst->adv_type = convert_adv_type(src->advType);
    dst->own_addr.type = 0x00;
    dst->peer_addr.type = 0x00;
    if (memcpy_s(dst->peer_addr.addr, BD_ADDR_LEN, src->peerAddr.addr, BD_ADDR_LEN) != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "convert_adv_param memcpy fail");
        return;
    }
    dst->channel_map = src->channelMap;
    dst->adv_filter_policy = convert_adv_filter(src->advFilterPolicy);
    dst->tx_power = src->txPower;
    dst->duration = src->duration;
}

int BleStartAdvEx(int *advId, const StartAdvRawData rawData, BleAdvParams advParam)
{
    if (advId == NULL) {
        return OHOS_BT_STATUS_PARM_INVALID;
    }

    /* 此处暂使用设置广播参数+设置广播数据+启动广播实现OHOS BleStartAdvEx接口功能 */
    gap_ble_adv_params_t dst_param;
    convert_adv_param(&advParam, &dst_param);
    errcode_t ret = gap_ble_set_adv_param(*advId, &dst_param);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gap_ble_set_adv_param fail ret = %d", ret);
        return OHOS_BT_STATUS_FAIL;
    }

    gap_ble_config_adv_data_t adv_data;
    convert_adv_data(&rawData, &adv_data);
    ret = gap_ble_set_adv_data(*advId, &adv_data);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gap_ble_set_adv_data fail ret = %d", ret);
        return OHOS_BT_STATUS_FAIL;
    }

    ret = gap_ble_start_adv(*advId);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gap_ble_start_adv fail ret = %d", ret);
        return OHOS_BT_STATUS_FAIL;
    }

    return OHOS_BT_STATUS_SUCCESS;
}

int BleStopAdv(int advId)
{
    errcode_t ret = gap_ble_stop_adv(advId);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gap_ble_stop_adv fail ret = %d", ret);
        return OHOS_BT_STATUS_FAIL;
    }
    return OHOS_BT_STATUS_SUCCESS;
}

int BleStartScanEx(BleScanConfigs *configs, BleScanNativeFilter *filter, unsigned int filterSize)
{
    unused(configs);
    unused(filter);
    unused(filterSize);

    /* OHOS原BleStartScanEx接口中filter设置3321不支持,此处暂只设置基本扫描参数 */
    gap_ble_scan_params_t scan_para = {0};
    scan_para.scan_phy = GAP_BLE_PHY_1M;
    scan_para.scan_interval = BLE_SCAN_INTERVAL;
    scan_para.scan_window = BLE_SCAN_WINDOW;
    scan_para.scan_type = GAP_BLE_SCAN_TYPE_ACTIVE;
    scan_para.scan_filter_policy = GAP_BLE_SCAN_FILTER_POLICY_ACCEPT_ALL;
    errcode_t ret = gap_ble_set_scan_parameters(&scan_para);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gap_ble_set_scan_parameters fail ret = %d", ret);
        return OHOS_BT_STATUS_FAIL;
    }

    ret = gap_ble_start_scan();
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gap_ble_start_scan fail ret = %d", ret);
        return OHOS_BT_STATUS_FAIL;
    }
    return OHOS_BT_STATUS_SUCCESS;
}

int BleStopScan(void)
{
    errcode_t ret = gap_ble_stop_scan();
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gap_ble_stop_scan fail ret = %d", ret);
        return OHOS_BT_STATUS_FAIL;
    }
    return OHOS_BT_STATUS_SUCCESS;
}

