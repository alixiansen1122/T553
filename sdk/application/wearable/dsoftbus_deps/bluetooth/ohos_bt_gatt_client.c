/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: ohos bt gatt client src file.
 * Author: CompanyName
 * Create:
 */
#include "securec.h"
#include "common_def.h"
#include "bts_br_gap.h"
#include "bts_le_gap.h"
#include "wearable_log.h"
#include "bts_gatt_client.h"
#include "ohos_bt_adapt.h"
#include "ohos_bt_gatt_porting.h"
#include "ohos_bt_gatt_client_porting.h"
#include "ohos_bt_gatt_client.h"

static int32_t g_client_id = 0;
static bt_uuid_t g_bt_uuid = {0};
static gattc_callbacks_t g_gattc_callback;

int32_t ble_gatt_get_client_id(void)
{
    return g_client_id;
}

bt_uuid_t ble_gatt_get_uuid(void)
{
    return g_bt_uuid;
}

/* ohos gattc注册函数返回client id */
int BleGattcRegister(BtUuid appUuid)
{
    bt_uuid_t app_id;
    if (memcpy_s(app_id.uuid, BT_UUID_MAX_LEN, appUuid.uuid, appUuid.uuidLen) != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "BleGattcRegister memcpy fail");
        return -1;
    }
    app_id.uuid_len = appUuid.uuidLen;

    int32_t client_id = 0;
    errcode_t ret = gattc_register_client(&app_id, (uint8_t*)&client_id);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gattc_register_client fail ret %d,", ret);
        return -1;
    }

    g_client_id = client_id;
    g_bt_uuid = app_id;
    return client_id;
}

int BleGattcUnRegister(int clientId)
{
    errcode_t ret = gattc_unregister_client(clientId);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gattc_unregister_client fail ret %d,", ret);
        return OHOS_BT_STATUS_FAIL;
    }
    return OHOS_BT_STATUS_SUCCESS;
}

int BleGattcConfigureMtuSize(int clientId, int mtuSize)
{
    uint16_t conn_id = ble_gatt_get_conn_id();
    errcode_t ret = gattc_exchange_mtu_req(clientId, conn_id, mtuSize);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gattc_exchange_mtu_req fail ret %d,", ret);
        return OHOS_BT_STATUS_FAIL;
    }
    return OHOS_BT_STATUS_SUCCESS;
}

int BleGattcWriteCharacteristic(int clientId, BtGattCharacteristic characteristic,
    BtGattWriteType writeType, int len, const char *value)
{
    unused(characteristic);
    unused(writeType);

    if (value == NULL || len <= 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "BleGattcWriteCharacteristic:: invalid value");
        return OHOS_BT_STATUS_PARM_INVALID;
    }

    gattc_handle_value_t param;
    param.data_len = len;
    param.data = value;

    uint16_t conn_id = ble_gatt_get_conn_id();
    errcode_t ret = gattc_write_req(clientId, conn_id, &param);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gattc_write_req fail ret %d,", ret);
        return OHOS_BT_STATUS_FAIL;
    }
    return OHOS_BT_STATUS_SUCCESS;
}

static errcode_t ble_gattc_register_callbacks(void)
{
    g_gattc_callback.read_cb = ble_gattc_read_cfm_callback;
    g_gattc_callback.read_cmp_cb = ble_gattc_read_by_uuid_complete_callback;
    g_gattc_callback.write_cb = ble_gattc_write_cfm_callback;
    g_gattc_callback.notification_cb = ble_gattc_notification_callback;
    g_gattc_callback.indication_cb = ble_gattc_indication_callback;
    g_gattc_callback.mtu_changed_cb = ble_gattc_mtu_changed_callback;
    g_gattc_callback.discovery_svc_cmp_cb = ble_gattc_discovery_service_complete_callback;
    return gattc_register_callbacks(&g_gattc_callback);
}

int BleGattcConnect(int clientId, BtGattClientCallbacks *func, const BdAddr *bdAddr,
    bool isAutoConnect, BtTransportType transport)
{
    unused(clientId);
    unused(isAutoConnect);
    unused(transport);
    unused(func);

    if (bdAddr == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "BleGattcConnect:: bdAddr NULL");
        return OHOS_BT_STATUS_PARM_INVALID;
    }

    /* 按照ohos处理,gattc回调注册是在客户端发起连接时 */
    errcode_t ret = ble_gattc_register_callbacks();
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ble_gattc_register_callbacks fail ret %d,", ret);
    }

    bd_addr_t bd_addr;
    if (memcpy_s(bd_addr.addr, BD_ADDR_LEN, bdAddr->addr, BD_ADDR_LEN) != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "BleGattcConnect memcpy fail");
        return OHOS_BT_STATUS_NOMEM;
    }
    bd_addr.type = BT_ADDRESS_TYPE_PUBLIC_DEVICE_ADDRESS;

    ret = gap_ble_connect_remote_device(&bd_addr);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gap_ble_connect_remote_device fail ret %d,", ret);
        return OHOS_BT_STATUS_FAIL;
    }
    return OHOS_BT_STATUS_SUCCESS;
}

int BleGattcDisconnect(int clientId)
{
    unused(clientId);

    bd_addr_t bd_addr = ble_gatt_get_conn_addr();
    errcode_t ret = gap_ble_disconnect_remote_device(&bd_addr);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gap_ble_disconnect_remote_device fail ret %d,", ret);
        return OHOS_BT_STATUS_FAIL;
    }
    return OHOS_BT_STATUS_SUCCESS;
}

int BleGattcSearchServices(int clientId)
{
    uint16_t conn_id = ble_gatt_get_conn_id();
    errcode_t ret = gattc_discovery_service(clientId, conn_id, &g_bt_uuid);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "gattc_discovery_service fail ret %d,", ret);
        return OHOS_BT_STATUS_FAIL;
    }
    return OHOS_BT_STATUS_SUCCESS;
}

int BleGattcSetPriority(int clientId, const BdAddr *bdAddr, BtGattPriority priority)
{
    return OHOS_BT_STATUS_SUCCESS;
}

int BleGattcSetFastestConn(int clientId, bool fastestConnFlag)
{
    return OHOS_BT_STATUS_UNSUPPORTED;
}

int BleGattcRegisterNotification(int clientId, BtGattCharacteristic characteristic, bool enable)
{
    return OHOS_BT_STATUS_SUCCESS;
}

bool BleGattcGetService(int clientId, BtUuid serviceUuid)
{
    return true;
}