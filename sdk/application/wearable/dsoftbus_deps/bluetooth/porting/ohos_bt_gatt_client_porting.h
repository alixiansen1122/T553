/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: dsoftbus ble gatt client adapter header file.
 * Author: CompanyName
 * Create:
 */
#ifndef OHOS_BT_GATT_CLIENT_PORTING_H
#define OHOS_BT_GATT_CLIENT_PORTING_H

#include "bts_le_gap.h"
#include "bts_gatt_client.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

void ble_gattc_notification_callback(uint8_t client_id, uint16_t conn_id, gattc_handle_value_t *data,
    errcode_t status);
void ble_gattc_discovery_service_complete_callback(uint8_t client_id, uint16_t conn_id,
    bt_uuid_t *uuid, errcode_t status);
void ble_gattc_mtu_changed_callback(uint8_t client_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status);
void ble_gattc_indication_callback(uint8_t client_id, uint16_t conn_id, gattc_handle_value_t *data,
    errcode_t status);
void ble_gattc_read_cfm_callback(uint8_t client_id, uint16_t conn_id, gattc_handle_value_t *read_result,
    gatt_status_t status);
void ble_gattc_read_by_uuid_complete_callback(uint8_t client_id, uint16_t conn_id,
    gattc_read_req_by_uuid_param_t *param, errcode_t status);
void ble_gattc_write_cfm_callback(uint8_t client_id, uint16_t conn_id, uint16_t handle, gatt_status_t status);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* OHOS_BT_GATT_CLIENT_PORTING_H */

