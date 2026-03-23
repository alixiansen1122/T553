/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: ohos bt gatt porting src file.
 * Author: CompanyName
 * Create:
 */
#include "dsoftbus_ble_gatt.h"
#include "dsoftbus_ble_gattc.h"
#include "ohos_bt_gatt_porting.h"

uint16_t ble_gatt_get_conn_id(void)
{
    return softbus_ble_gatt_get_conn_id();
}

bd_addr_t ble_gatt_get_conn_addr(void)
{
    return softbus_ble_gatt_get_conn_addr();
}

void ble_gatt_connect_change_callback(uint16_t conn_id, bd_addr_t *addr, gap_ble_conn_state_t conn_state,
    gap_ble_pair_state_t pair_state, gap_ble_disc_reason_t disc_reason)
{
    softbus_ble_gatt_connect_change_callback(conn_id, addr, conn_state, pair_state, disc_reason);
}

void ble_gatt_start_adv_callback(uint8_t adv_id, adv_status_t status)
{
    softbus_ble_gatt_start_adv_callback(adv_id, status);
}

void ble_gatt_stop_adv_callback(uint8_t adv_id, adv_status_t status)
{
    softbus_ble_gatt_stop_adv_callback(adv_id, status);
}

void ble_gatt_set_adv_data_callback(uint8_t adv_id, errcode_t status)
{
    softbus_ble_gatt_set_adv_data_callback(adv_id, status);
}

void ble_gatt_scan_result_callback(gap_scan_result_data_t *scan_result_data)
{
    softbus_ble_gatt_scan_result_callback(scan_result_data);
}

void ble_gatt_set_scan_param_callback(errcode_t status)
{
    softbus_ble_gatt_set_scan_param_callback(status);
}