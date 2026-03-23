/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: ohos bt gatt server porting src file.
 * Author: CompanyName
 * Create:
 */
#include "dsoftbus_ble_gatts.h"
#include "ohos_bt_gatt_server_porting.h"

void ble_gatts_add_service_callback(uint8_t server_id, bt_uuid_t *uuid, uint16_t handle, errcode_t status)
{
    softbus_ble_gatts_add_service_callback(server_id, uuid, handle, status);
}

void ble_gatts_start_service_callback(uint8_t server_id, uint16_t handle, errcode_t status)
{
    softbus_ble_gatts_start_service_callback(server_id, handle, status);
}

void ble_gatts_stop_service_callback(uint8_t server_id, uint16_t handle, errcode_t status)
{
    softbus_ble_gatts_stop_service_callback(server_id, handle, status);
}

void ble_gatts_delete_all_service_callback(uint8_t server_id, errcode_t status)
{
    softbus_ble_gatts_delete_all_service_callback(server_id, status);
}

void ble_gatts_add_characteristic_callback(uint8_t server_id, bt_uuid_t *uuid, uint16_t service_handle,
    gatts_add_character_result_t *result, errcode_t status)
{
    softbus_ble_gatts_add_characteristic_callback(server_id, uuid, service_handle, result, status);
}

void ble_gatts_add_descriptor_callback(uint8_t server_id, bt_uuid_t *uuid, uint16_t service_handle,
    uint16_t handle, errcode_t status)
{
    softbus_ble_gatts_add_descriptor_callback(server_id, uuid, service_handle, handle, status);
}

void ble_gatts_read_request_callback(uint8_t server_id, uint16_t conn_id, gatts_req_read_cb_t *read_cb_para,
    errcode_t status)
{
    softbus_ble_gatts_read_request_callback(server_id, conn_id, read_cb_para, status);
}

void ble_gatts_write_request_callback(uint8_t server_id, uint16_t conn_id, gatts_req_write_cb_t *write_cb_para,
    errcode_t status)
{
    softbus_ble_gatts_write_request_callback(server_id, conn_id, write_cb_para, status);
}

void ble_gatts_mtu_changed_callback(uint8_t server_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status)
{
    softbus_ble_gatts_mtu_changed_callback(server_id, conn_id, mtu_size, status);
}
