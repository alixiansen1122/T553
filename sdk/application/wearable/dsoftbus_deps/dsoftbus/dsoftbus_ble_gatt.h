/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: dsoftbus ble gatt adapter header file.
 * Author: CompanyName
 * Create:
 */
#ifndef DSOFTBUS_BLE_GATT_H
#define DSOFTBUS_BLE_GATT_H

#include "bts_le_gap.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

uint16_t softbus_ble_gatt_get_conn_id(void);
bd_addr_t softbus_ble_gatt_get_conn_addr(void);
void softbus_ble_gatt_connect_change_callback(uint16_t conn_id, bd_addr_t *addr, gap_ble_conn_state_t conn_state,
    gap_ble_pair_state_t pair_state, gap_ble_disc_reason_t disc_reason);
void softbus_ble_gatt_start_adv_callback(uint8_t adv_id, adv_status_t status);
void softbus_ble_gatt_stop_adv_callback(uint8_t adv_id, adv_status_t status);
void softbus_ble_gatt_set_adv_data_callback(uint8_t adv_id, errcode_t status);
void softbus_ble_gatt_scan_result_callback(gap_scan_result_data_t *scan_result_data);
void softbus_ble_gatt_set_scan_param_callback(errcode_t status);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* DSOFTBUS_BLE_GATT_H */

