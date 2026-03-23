/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: BluetoothWrapper
 * Author:
 * Create: 2021-10-13
 */
#ifndef BLUETOOTH_WRAPPER_H
#define BLUETOOTH_WRAPPER_H

#include "common_def.h"
#include "bts_def.h"
#include "bts_gatt_stru.h"
#include "bts_gatt_server.h"
#include "bts_gatt_client.h"
#include "bts_le_gap.h"

#ifdef __cplusplus
extern "C" {
#endif

errcode_t wrapper_gap_ble_register_callbacks(gap_ble_callbacks_t *func);
errcode_t wrapper_gattc_register_client(bt_uuid_t *uuid, uint8_t *clientId);
errcode_t wrapper_gatts_register_callbacks(gatts_callbacks_t *func);
errcode_t wrapper_gatts_register_ohos_callbacks(gatts_callbacks_t *func);
errcode_t wrapper_gattc_register_callbacks(gattc_callbacks_t *func);
void wrapper_ble_register(void);
errcode_t wrapper_gatts_register_server(bt_uuid_t *app_uuid, uint8_t *server_id);
errcode_t wrapper_gatts_add_service(uint8_t server_id, bt_uuid_t *service_uuid, bool is_primary);
void wrapper_gatts_service_start(void);

#ifdef __cplusplus
}
#endif

#endif