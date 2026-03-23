/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: dsoftbus ble gatt server adapt src file.
 * Author: CompanyName
 * Create:
 */
#include "securec.h"
#include "wearable_log.h"
#include "ohos_bt_def.h"
#include "ohos_bt_gatt_server.h"
#include "softbus_adapter_ble_gatt_server.h"
#include "dsoftbus_ble_gatts.h"

static uint16_t g_service_handle;
static SoftBusGattsCallback *g_gatts_callback = NULL;

void softbus_ble_gatts_add_service_callback(uint8_t server_id, bt_uuid_t *uuid, uint16_t handle, errcode_t status)
{
    if (uuid == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "add service cbk: uuid NULL");
        return;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "add service cbk: server: %d, status: %d, handle: %d, uuid_len: %d",
        server_id, status, handle, uuid->uuid_len);

    BtUuid app_id;
    if (memcpy_s(app_id.uuid, BT_UUID_MAX_LEN, uuid->uuid, uuid->uuid_len) != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "BleGattcRegister memcpy fail");
        return;
    }
    app_id.uuidLen = uuid->uuid_len;
    g_service_handle = handle;

    SoftBusBleServiceAddCallback(status, &app_id, handle);
}

void softbus_ble_gatts_start_service_callback(uint8_t server_id, uint16_t handle, errcode_t status)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "start service cbk: server: %d, status: %d, handle: %d",
        server_id, status, handle);

    SoftBusBleServiceStartCallback(status, handle);
}

void softbus_ble_gatts_stop_service_callback(uint8_t server_id, uint16_t handle, errcode_t status)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "stop service cbk: server: %d, status: %d, handle: %d",
        server_id, status, handle);

    SoftBusBleServiceStopCallback(status, handle);
}

void softbus_ble_gatts_delete_all_service_callback(uint8_t server_id, errcode_t status)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "del service cbk: server: %d, status: %d", server_id, status);

    SoftBusBleServiceDeleteCallback(status, g_service_handle);
}

void softbus_ble_gatts_add_characteristic_callback(uint8_t server_id, bt_uuid_t *uuid, uint16_t service_handle,
    gatts_add_character_result_t *result, errcode_t status)
{
    if (uuid == NULL || result == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "add characteristic cbk: uuid NULL or result NULL");
        return;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "add characteristic cbk: server: %d, status: %d", server_id, status);

    BtUuid app_id;
    if (memcpy_s(app_id.uuid, BT_UUID_MAX_LEN, uuid->uuid, uuid->uuid_len) != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "add characteristic cbk memcpy fail");
        return;
    }
    app_id.uuidLen = uuid->uuid_len;

    SoftBusBleCharacteristicAddCallback(status, &app_id, service_handle, result->handle);
}

void softbus_ble_gatts_add_descriptor_callback(uint8_t server_id, bt_uuid_t *uuid, uint16_t service_handle,
    uint16_t handle, errcode_t status)
{
    if (uuid == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "add descriptor cbk: uuid NULL");
        return;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "add descriptor cbk: server: %d, status: %d, handle: %d",
        server_id, status, handle);

    BtUuid app_id;
    if (memcpy_s(app_id.uuid, BT_UUID_MAX_LEN, uuid->uuid, uuid->uuid_len) != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "add descriptor cbk memcpy fail");
        return;
    }
    app_id.uuidLen = uuid->uuid_len;

    SoftBusBleDescriptorAddCallback(status, &app_id, service_handle, handle);
}

void softbus_ble_gatts_read_request_callback(uint8_t server_id, uint16_t conn_id, gatts_req_read_cb_t *read_cb_para,
    errcode_t status)
{
    if (read_cb_para == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "read request cbk: read_cb_para NULL");
        return;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "read request cbk: server: %d, status: %d, conn_id: %d",
        server_id, status, conn_id);

    BtReqReadCbPara req = {0};
    req.connId = conn_id;
    // req.btAddr = ; 待确认
    req.transId = read_cb_para->request_id;
    req.attrHandle = read_cb_para->handle;
    req.offset = read_cb_para->offset;
    req.isLong = read_cb_para->is_long;

    SoftBusBleRequestReadCallback(req);
}

void softbus_ble_gatts_write_request_callback(uint8_t server_id, uint16_t conn_id, gatts_req_write_cb_t *write_cb_para,
    errcode_t status)
{
    if (write_cb_para == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "write request cbk: write_cb_para NULL");
        return;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "write request cbk: server: %d, status: %d, conn_id: %d",
        server_id, status, conn_id);

    BtReqWriteCbPara req = {0};
    req.connId = conn_id;
    req.transId = write_cb_para->request_id;
    // req.btAddr
    req.attrHandle = write_cb_para->handle;
    req.offset = write_cb_para->offset;
    req.length = write_cb_para->length;
    req.needRsp = write_cb_para->need_rsp;
    req.isPrep = write_cb_para->is_prep;
    req.value = write_cb_para->value;

    SoftBusBleRequestWriteCallback(req);
}

void softbus_ble_gatts_mtu_changed_callback(uint8_t server_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "mtu change cbk: conn_id: %d, mtu_size: %d", conn_id, status);

    SoftBusBleMtuChangeCallback(conn_id, mtu_size);
}
