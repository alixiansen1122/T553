/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: ohos bt gatt server src file.
 * Author: CompanyName
 * Create:
 */
#include <stdio.h>
#include "securec.h"
#include "common_def.h"
#include "bts_le_gap.h"
#include "bts_gatt_server.h"
#include "wearable_log.h"
#include "ohos_bt_gatt_server_porting.h"
#include "ohos_bt_gatt_server.h"

static gatts_callbacks_t g_gatts_callback = {0};

int BleGattsRegister(BtUuid appUuid)
{
    bt_uuid_t app_id;
    if (memcpy_s(app_id.uuid, BT_UUID_MAX_LEN, appUuid.uuid, appUuid.uuidLen) != EOK) {
        return OHOS_BT_STATUS_NOMEM;
    }
    app_id.uuid_len = appUuid.uuidLen;

    uint8_t server_id = 0;
    errcode_t ret = gatts_register_server(&app_id, &server_id);
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gatts_register_server fail ret %d,", ret);
        return OHOS_BT_STATUS_FAIL;
    }
    return OHOS_BT_STATUS_SUCCESS;
}

int BleGattsUnRegister(int serverId)
{
    errcode_t ret = gatts_unregister_server(serverId);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gatts_unregister_server fail ret %d,", ret);
        return OHOS_BT_STATUS_FAIL;
    }
    return OHOS_BT_STATUS_SUCCESS;
}

int BleGattsRegisterCallbacks(BtGattServerCallbacks *func)
{
    unused(func);

    g_gatts_callback.add_service_cb = ble_gatts_add_service_callback;
    g_gatts_callback.add_characteristic_cb = ble_gatts_add_characteristic_callback;
    g_gatts_callback.add_descriptor_cb = ble_gatts_add_descriptor_callback;
    g_gatts_callback.start_service_cb = ble_gatts_start_service_callback;
    g_gatts_callback.stop_service_cb = ble_gatts_stop_service_callback;
    g_gatts_callback.delete_service_cb = ble_gatts_delete_all_service_callback;
    g_gatts_callback.read_request_cb = ble_gatts_read_request_callback;
    g_gatts_callback.write_request_cb = ble_gatts_write_request_callback;
    g_gatts_callback.mtu_changed_cb = ble_gatts_mtu_changed_callback;

    errcode_t ret = gatts_register_callbacks(&g_gatts_callback);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gatts_register_callbacks fail ret %d,", ret);
        return OHOS_BT_STATUS_FAIL;
    }
    return OHOS_BT_STATUS_SUCCESS;
}

int BleGattsAddService(int serverId, BtUuid srvcUuid, bool isPrimary, int number)
{
    unused(number);

    bt_uuid_t app_id;
    if (memcpy_s(app_id.uuid, BT_UUID_MAX_LEN, srvcUuid.uuid, srvcUuid.uuidLen) != EOK) {
        return OHOS_BT_STATUS_NOMEM;
    }
    app_id.uuid_len = srvcUuid.uuidLen;

    errcode_t ret = gatts_add_service(serverId, &app_id, isPrimary);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gatts_add_service fail ret %d,", ret);
        return OHOS_BT_STATUS_FAIL;
    }
    return OHOS_BT_STATUS_SUCCESS;
}

int BleGattsStartService(int serverId, int srvcHandle)
{
    errcode_t ret = gatts_start_service(serverId, srvcHandle);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gatts_start_service fail ret %d,", ret);
        return OHOS_BT_STATUS_FAIL;
    }
    return OHOS_BT_STATUS_SUCCESS;
}

int BleGattsStopService(int serverId, int srvcHandle)
{
    errcode_t ret = gatts_stop_service(serverId, srvcHandle);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gatts_stop_service fail ret %d,", ret);
        return OHOS_BT_STATUS_FAIL;
    }
    return OHOS_BT_STATUS_SUCCESS;
}

int BleGattsDeleteService(int serverId, int srvcHandle)
{
    errcode_t ret = gatts_delete_service(serverId, srvcHandle);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gatts_delete_service fail ret %d,", ret);
        return OHOS_BT_STATUS_FAIL;
    }
    return OHOS_BT_STATUS_SUCCESS;
}

int BleGattsAddCharacteristic(int serverId, int srvcHandle, BtUuid characUuid,
                              int properties, int permissions)
{
    bt_uuid_t app_id;
    if (memcpy_s(app_id.uuid, BT_UUID_MAX_LEN, characUuid.uuid, characUuid.uuidLen) != EOK) {
        return OHOS_BT_STATUS_NOMEM;
    }
    app_id.uuid_len = characUuid.uuidLen;

    gatts_add_chara_info_t chara_info = {
        .chara_uuid = app_id,
        .permissions = permissions,
        .properties = properties
    };

    errcode_t ret = gatts_add_characteristic(serverId, srvcHandle, &chara_info);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gatts_add_characteristic fail ret %d,", ret);
        return OHOS_BT_STATUS_FAIL;
    }
    return OHOS_BT_STATUS_SUCCESS;
}

int BleGattsAddDescriptor(int serverId, int srvcHandle, BtUuid descUuid, int permissions)
{
    bt_uuid_t app_id;
    if (memcpy_s(app_id.uuid, BT_UUID_MAX_LEN, descUuid.uuid, descUuid.uuidLen) != EOK) {
        return OHOS_BT_STATUS_NOMEM;
    }
    app_id.uuid_len = descUuid.uuidLen;

    gatts_add_desc_info_t desc_info = {
        .desc_uuid = app_id,
        .permissions = permissions
    };

    errcode_t ret = gatts_add_descriptor(serverId, srvcHandle, &desc_info);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gatts_add_descriptor fail ret %d,", ret);
        return OHOS_BT_STATUS_FAIL;
    }
    return OHOS_BT_STATUS_SUCCESS;
}

int BleGattsSendResponse(int serverId, GattsSendRspParam *param)
{
    if (param == NULL) {
        return OHOS_BT_STATUS_PARM_INVALID;
    }

    gatts_send_rsp_t response = {
        .status = param->status,
        .offset = param->attrHandle,
        .value_len = param->valueLen,
        .value = param->value
    };

    errcode_t ret = gatts_send_response(serverId, param->connectId, &response);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gatts_send_response fail ret %d,", ret);
        return OHOS_BT_STATUS_FAIL;
    }
    return OHOS_BT_STATUS_SUCCESS;
}

int BleGattsSendIndication(int serverId, GattsSendIndParam *param)
{
    if (param == NULL) {
        return OHOS_BT_STATUS_PARM_INVALID;
    }

    gatts_ntf_ind_t notify = {
        .attr_handle = param->attrHandle,
        .value_len = param->valueLen,
        .value = param->value
    };

    errcode_t ret = gatts_notify_indicate(serverId, param->connectId, &notify);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gatts_notify_indicate fail ret %d,", ret);
        return OHOS_BT_STATUS_FAIL;
    }
    return OHOS_BT_STATUS_SUCCESS;
}

int BleGattsDisconnect(int serverId, BdAddr bdAddr, int connId)
{
    unused(serverId);
    unused(connId);

    bd_addr_t bd_addr;
    if (memcpy_s(bd_addr.addr, BD_ADDR_LEN, bdAddr.addr, BD_ADDR_LEN) != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "BleGattsDisconnect memcpy fail");
        return OHOS_BT_STATUS_NOMEM;
    }

    errcode_t ret = gap_ble_disconnect_remote_device(&bd_addr);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gap_ble_disconnect_remote_device fail ret %d", ret);
        return OHOS_BT_STATUS_FAIL;
    }
    return OHOS_BT_STATUS_SUCCESS;
}