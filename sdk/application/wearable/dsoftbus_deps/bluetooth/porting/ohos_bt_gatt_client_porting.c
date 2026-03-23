/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: ohos bt gatt client porting src file.
 * Author: CompanyName
 * Create:
 */
#include "dsoftbus_ble_gattc.h"
#include "ohos_bt_gatt_client_porting.h"

void ble_gattc_notification_callback(uint8_t client_id, uint16_t conn_id, gattc_handle_value_t *data, errcode_t status)
{
    softbus_ble_gattc_notification_callback(client_id, conn_id, data, status);
}

void ble_gattc_discovery_service_complete_callback(uint8_t client_id, uint16_t conn_id,
    bt_uuid_t *uuid, errcode_t status)
{
    softbus_ble_gattc_discovery_service_complete_callback(client_id, conn_id, uuid, status);
}

void ble_gattc_mtu_changed_callback(uint8_t client_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status)
{
    softbus_ble_gattc_mtu_changed_callback(client_id, conn_id, mtu_size, status);
}

void ble_gattc_indication_callback(uint8_t client_id, uint16_t conn_id, gattc_handle_value_t *data,
    errcode_t status)
{
    softbus_ble_gattc_indication_callback(client_id, conn_id, data, status);
}

void ble_gattc_read_cfm_callback(uint8_t client_id, uint16_t conn_id, gattc_handle_value_t *read_result,
    gatt_status_t status)
{
    softbus_ble_gattc_read_cfm_callback(client_id, conn_id, read_result, status);
}

void ble_gattc_read_by_uuid_complete_callback(uint8_t client_id, uint16_t conn_id,
    gattc_read_req_by_uuid_param_t *param, errcode_t status)
{
    softbus_ble_gattc_read_by_uuid_complete_callback(client_id, conn_id, param, status);
}

void ble_gattc_write_cfm_callback(uint8_t client_id, uint16_t conn_id, uint16_t handle, gatt_status_t status)
{
    softbus_ble_gattc_write_cfm_callback(client_id, conn_id, handle, status);
}