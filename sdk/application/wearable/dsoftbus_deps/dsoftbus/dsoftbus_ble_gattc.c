/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: dsoftbus ble adapt src file.
 * Author: CompanyName
 * Create:
 */
#include "securec.h"
#include "wearable_log.h"
#include "ohos_bt_adapt.h"
#include "ohos_bt_gatt_client.h"
#include "softbus_adapter_ble_gatt_client.h"
#include "dsoftbus_ble_gattc.h"

void softbus_ble_gattc_notification_callback(uint8_t client_id, uint16_t conn_id, gattc_handle_value_t *data,
    errcode_t status)
{
    if (data == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "softbus_ble_gattc_notification_callback data NULL");
        return;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "notification callback: client_id = %d, conn_id = %d, status = %d\n",
        client_id, conn_id, status);

    bt_uuid_t app_id = ble_gatt_get_uuid();
    BtGattReadData notify;
    notify.data = data->data;
    notify.dataLen = data->data_len;
    notify.attribute.characteristic.characteristicUuid.uuidLen = app_id.uuid_len;
    if (memcpy_s(notify.attribute.characteristic.characteristicUuid.uuid, BT_UUID_MAX_LEN,
        app_id.uuid, app_id.uuid_len) != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "softbus_ble_gattc_notification_callback memcpy fail");
        return ;
    }

    SoftBusGattcNotificationCallback(client_id, &notify, status);
}

void softbus_ble_gattc_discovery_service_complete_callback(uint8_t client_id, uint16_t conn_id,
    bt_uuid_t *uuid, errcode_t status)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "discover callback: client_id = %d, conn_id = %d, status = %d\n",
        client_id, conn_id, status);

    SoftBusGattcSearchServiceCompleteCallback(client_id, status);
}

void softbus_ble_gattc_mtu_changed_callback(uint8_t client_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "mtu change callback:client_id = %d, conn_id = %d, status = %d, size = %d\n",
        client_id, conn_id, status, mtu_size);

    SoftBusGattcConfigureMtuSizeCallback(client_id, mtu_size, status);
}

void softbus_ble_gattc_indication_callback(uint8_t client_id, uint16_t conn_id, gattc_handle_value_t *data,
    errcode_t status)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "indication callback: client_id = %d, conn_id = %d, status = %d\n",
        client_id, conn_id, status);
}

void softbus_ble_gattc_read_cfm_callback(uint8_t client_id, uint16_t conn_id, gattc_handle_value_t *read_result,
    gatt_status_t status)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "read cfm callback: client_id = %d, conn_id = %d, status = %d\n",
        client_id, conn_id, status);
}

void softbus_ble_gattc_read_by_uuid_complete_callback(uint8_t client_id, uint16_t conn_id,
    gattc_read_req_by_uuid_param_t *param, errcode_t status)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "read complete callback: client_id = %d, conn_id = %d, status = %d\n",
        client_id, conn_id, status);
}

void softbus_ble_gattc_write_cfm_callback(uint8_t client_id, uint16_t conn_id, uint16_t handle, gatt_status_t status)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "write cfm callback: client_id = %d, conn_id = %d, status = %d, hdl = %d\n",
        client_id, conn_id, status, handle);
}