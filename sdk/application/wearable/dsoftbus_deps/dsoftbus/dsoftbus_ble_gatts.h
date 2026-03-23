/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: softbus ble server adapter header file.
 * Author: CompanyName
 * Create:
 */
#ifndef DSOFTBUS_BLE_GATTS_H
#define DSOFTBUS_BLE_GATTS_H

#include "bts_gatt_server.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

void softbus_ble_gatts_add_service_callback(uint8_t server_id, bt_uuid_t *uuid, uint16_t handle, errcode_t status);
void softbus_ble_gatts_start_service_callback(uint8_t server_id, uint16_t handle, errcode_t status);
void softbus_ble_gatts_stop_service_callback(uint8_t server_id, uint16_t handle, errcode_t status);
void softbus_ble_gatts_delete_all_service_callback(uint8_t server_id, errcode_t status);
void softbus_ble_gatts_add_characteristic_callback(uint8_t server_id, bt_uuid_t *uuid, uint16_t service_handle,
    gatts_add_character_result_t *result, errcode_t status);
void softbus_ble_gatts_add_descriptor_callback(uint8_t server_id, bt_uuid_t *uuid, uint16_t service_handle,
    uint16_t handle, errcode_t status);
void softbus_ble_gatts_read_request_callback(uint8_t server_id, uint16_t conn_id, gatts_req_read_cb_t *read_cb_para,
    errcode_t status);
void softbus_ble_gatts_write_request_callback(uint8_t server_id, uint16_t conn_id, gatts_req_write_cb_t *write_cb_para,
    errcode_t status);
void softbus_ble_gatts_mtu_changed_callback(uint8_t server_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* DSOFTBUS_BLE_GATTS_H */

