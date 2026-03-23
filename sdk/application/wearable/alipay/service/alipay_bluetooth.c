/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay bluetooth module.
 * Author:
 * Create:
 */

#include "common_def.h"
#include "bts_def.h"
#include "bts_gatt_stru.h"
#include "bts_a2dp_source.h"
#include "bts_gatt_server.h"
#include "bts_le_gap.h"
#include "bts_br_gap.h"
#include "osal_task.h"
#include "alipay_feature.h"
#include "securec.h"
#include "cmsis_os2.h"
#ifdef SUPPORT_OHOSFWK
#include "bluetooth/BluetoothWrapper.h"
#endif

/* Gatt service UUID */
#define ALIPAYY_SERVICE_ID 0x0238
/* Gatt Characteristic UUID */
#define ALIPAYY_CHARACTERISTIC_ID 0x024A
/* Client Characteristic Configuration UUID */
#define ALIPAYY_BLE_UUID_CLIENT_CHARACTERISTIC_CONF 0x2902
/* Gatt Characteristic max length */
#define ALIPAY_CHARACTERISTIC_MAX_LENGTH 256
#define ALIPAY_BLE_ADV_ID 1
#define ALIPAY_BLE_ADV_DATA_MAX_LENGTH 31
#define ALIPAY_BLE_ADV_BLE_NAME_MAX_LENGTH 13 // 31-4-2-3-9 = 13
#define ALIPAY_BLE_NAME_LENGTH 20
#define ALIPAY_BLE_ADV_MAC_HEAD_LENTH 4
#define ALIPAY_BLE_ADV_MAC_LENTH 10
#define ALIPAY_BLE_ADV_TYPE_LENTH 3
#define ALIPAY_BLE_ADV_UUID_LENTH 4
#define ALIPAY_BLE_DEFAULT_NAME  "WatchDemo"
#define ALIPAY_BLE_DEFAULT_NAME_LEN 9
#define ALIPAY_DESC_HANDLE_NUM 2
#define UUID16_LEN 2

// Alipay 蓝牙模块全局变量
static gap_ble_callbacks_t g_alipay_gapble_cbs;
static gatts_callbacks_t g_alipay_gatts_cbs;
static bool g_alipay_adv_start_state = false;
static uint16_t g_alipay_server_id = 1;
static uint16_t g_alipay_characteristic_handle = 0;
static uint16_t g_alipay_descriptor_handle[ALIPAY_DESC_HANDLE_NUM] = { 0 };
static uint16_t g_alipay_svr_handle = 0;
static uint16_t g_alipay_conn_id = 0;
static uint8_t g_alipay_character_buf[ALIPAY_CHARACTERISTIC_MAX_LENGTH] = {0};
static bool g_alipay_ble_init_flag = false;
static const uint8_t alipay_adv_type[ALIPAY_BLE_ADV_TYPE_LENTH] = {2, 1, 2};
static const uint8_t alipay_adv_uuid[ALIPAY_BLE_ADV_UUID_LENTH] = {0x3, 0x3, 0x02, 0x38};

static bool svr_alipay_get_adv_start_state(void)
{
    return g_alipay_adv_start_state;
}
static void svr_alipay_set_adv_start_state(bool state)
{
    g_alipay_adv_start_state = state;
}

void svr_alipay_ble_adv_stop(void)
{
    gap_ble_stop_adv(ALIPAY_BLE_ADV_ID);
}

/* adapt ios adv data. */
static void svr_alipay_ble_adv_data_set(int adv_id)
{
    uint8_t ble_name[ALIPAY_BLE_NAME_LENGTH] = {0};
    uint8_t ble_name_len = ALIPAY_BLE_NAME_LENGTH;
    uint8_t adv_data[ALIPAY_BLE_ADV_DATA_MAX_LENGTH] = {0};
    uint8_t adv_data_len = 0;
    uint8_t ble_data_name[ALIPAY_BLE_ADV_BLE_NAME_MAX_LENGTH + 1 ] = {0}; // +TYPE LENGTH
    uint8_t ble_data_mac_switch[BD_ADDR_LEN] = {0};
    uint8_t adv_mac_head[ALIPAY_BLE_ADV_MAC_LENTH] = {0x9, 0x16, 0x02, 0x38};
    gap_ble_config_adv_data_t adv_data_info;
    memset_s(&adv_data_info, sizeof(gap_ble_config_adv_data_t), 0, sizeof(gap_ble_config_adv_data_t));

    int32_t ret = gap_ble_get_local_name(ble_name, &ble_name_len);
    if (ret != 0) {
        ALIPAY_PRINT_ERR(0, "gap_ble_get_local_name fail, name:%s,len:%u,ret:%u.", ble_name, ble_name_len, ret);
        memset_s(ble_name, ALIPAY_BLE_NAME_LENGTH, 0, ALIPAY_BLE_NAME_LENGTH);
        memcpy_s(ble_name, ALIPAY_BLE_NAME_LENGTH, ALIPAY_BLE_DEFAULT_NAME, ALIPAY_BLE_DEFAULT_NAME_LEN);
        ble_name_len = ALIPAY_BLE_DEFAULT_NAME_LEN + 1;
    }
    ALIPAY_PRINT_DEBUG(0, "gap_ble_get_local_name name:%s,len:%u,ret:%u.", ble_name, ble_name_len, ret);

    bd_addr_t addr_info;
    ret = gap_ble_get_local_addr(&addr_info);
    ALIPAY_PRINT_DEBUG(0, "gap_ble_get_local_addr, ret:%u.", ret);
    memcpy_s(adv_data, sizeof(alipay_adv_type), alipay_adv_type, sizeof(alipay_adv_type));
    adv_data_len = sizeof(alipay_adv_type);

    if (ble_name_len > ALIPAY_BLE_ADV_BLE_NAME_MAX_LENGTH) {
        ble_name_len = ALIPAY_BLE_ADV_BLE_NAME_MAX_LENGTH;
    }
    ble_data_name[0] = ble_name_len;
    ble_data_name[1] = 0x9;
    memcpy_s(&ble_data_name[2], ble_name_len - 1, ble_name, ble_name_len - 1);

    memcpy_s(adv_data + adv_data_len, ble_name_len + 1, ble_data_name, ble_name_len + 1);

    adv_data_len = adv_data_len + ble_name_len + 1;
    memcpy_s(adv_data + adv_data_len, sizeof(alipay_adv_uuid), alipay_adv_uuid, sizeof(alipay_adv_uuid));
    adv_data_len = adv_data_len + sizeof(alipay_adv_uuid);
    int k = BD_ADDR_LEN;
    for (int i = 0; i < BD_ADDR_LEN; i++) {
        ble_data_mac_switch[--k] = addr_info.addr[i];
    }
    ALIPAY_PRINT_DEBUG(0, "ble_info:%02x:%02x:%02x:%02x:%02x:%02x", ble_data_mac_switch[0], ble_data_mac_switch[1],
        ble_data_mac_switch[2], ble_data_mac_switch[3], ble_data_mac_switch[4], ble_data_mac_switch[5]);
    memcpy_s(adv_mac_head + ALIPAY_BLE_ADV_MAC_HEAD_LENTH, BD_ADDR_LEN, ble_data_mac_switch, BD_ADDR_LEN);

    memcpy_s(adv_data + adv_data_len, sizeof(adv_mac_head), adv_mac_head, sizeof(adv_mac_head));
    adv_data_len = adv_data_len + sizeof(adv_mac_head);

    adv_data_info.adv_data = adv_data;
    adv_data_info.adv_length = adv_data_len;
    ALIPAY_PRINT_DEBUG(0, "adv_data_info.adv_length:%u.", adv_data_len);

    gap_ble_set_adv_data(adv_id, &adv_data_info);
    return;
}

void svr_alipay_ble_adv_start(void)
{
    int adv_id = ALIPAY_BLE_ADV_ID;
    if (svr_alipay_get_adv_start_state()) {
        ALIPAY_PRINT_ERR(0, "BleAdvStart already started");
        gap_ble_stop_adv(adv_id);
        osDelay(50);
    }

    gap_ble_adv_params_t adv_para;
    memset_s(&adv_para, sizeof(gap_ble_adv_params_t), 0, sizeof(gap_ble_adv_params_t));

    adv_para.min_interval = 0x30;
    adv_para.max_interval = 0x60;
    adv_para.adv_type = GAP_BLE_ADV_CONN_SCAN_UNDIR;
    adv_para.channel_map = 0x7;
    adv_para.duration = 0;

    gap_ble_set_adv_param(adv_id, &adv_para);
    gap_ble_set_local_appearance(GAP_BLE_APPEARANCE_TYPE_GENERIC_WATCH);
    svr_alipay_ble_adv_data_set(adv_id);
    bool ret = gap_ble_start_adv(adv_id);
    if (ret) {
        ALIPAY_PRINT_ERR(0, "svr_alipay_ble_adv_start bt gap_ble_start_adv  ret = 0x%x", ret);
    }
    ALIPAY_PRINT_DEBUG(0, "svr_alipay_ble_adv_start bt gap_ble_start_adv  ret = 0x%x", ret);
}

static void svr_alipay_ble_connstate_change_cb(unsigned short conn_id, bd_addr_t *addr, gap_ble_conn_state_t conn_state,
    gap_ble_pair_state_t pair_state, gap_ble_disc_reason_t disc_reason)
{
    unused(addr);
    unused(pair_state);
    unused(disc_reason);
    ALIPAY_PRINT_DEBUG(0, "BleConnStateChangeCallback conn_id 0x%x, conn_state 0x%x, pair_state 0x%x", conn_id,
        conn_state, pair_state);
    if (GAP_BLE_STATE_DISCONNECTED == conn_state) {
        g_alipay_conn_id = 0;
    } else {
        g_alipay_conn_id = conn_id;
    }
}

static void svr_alipay_ble_adv_start_cb(uint8_t adv_id, adv_status_t status)
{
    unused(status);
    if (adv_id == ALIPAY_BLE_ADV_ID) {
        ALIPAY_PRINT_DEBUG(0, "BleAdvStartCallback adv_id: %d, status:%d\n", adv_id, status);
        svr_alipay_set_adv_start_state(true);
    }
}

static void svr_alipay_ble_set_adv_data_cb(uint8_t adv_id, errcode_t status)
{
    unused(adv_id);
    unused(status);
    ALIPAY_PRINT_DEBUG(0, "BleAdvSetDataCallback adv_id: %d, status:%d\n", adv_id, status);
}

static void svr_alipay_ble_terminate_adv_cb(uint8_t adv_id, adv_status_t status)
{
    unused(status);
    if (adv_id == ALIPAY_BLE_ADV_ID) {
        ALIPAY_PRINT_DEBUG(0, "BleTerminateAdvCallback adv_id: %d, status:%d\n", adv_id, status);
        svr_alipay_set_adv_start_state(false);
    }
}

static void svr_alipay_ble_adv_stop_cb(uint8_t adv_id, adv_status_t status)
{
    unused(status);
    if (adv_id == ALIPAY_BLE_ADV_ID) {
        ALIPAY_PRINT_DEBUG(0, "BleAdvStopCallback adv_id: %d, status:%d\n", adv_id, status);
        svr_alipay_set_adv_start_state(false);
    }
}

static void svr_alipay_bt_gatts_service_add_cb(uint8_t server_id, bt_uuid_t *uuid, uint16_t srv_handle,
    errcode_t status)
{
    unused(status);
    if ((uuid->uuid_len == sizeof(uint16_t)) && (*((uint16_t *)uuid->uuid) == ALIPAYY_SERVICE_ID)) {
        ALIPAY_PRINT_DEBUG(0, "BtGattsServiceAddCallback 0x%x 0x%x 0x%x 0x%x",
                           status, server_id, srv_handle, *((uint16_t *)uuid->uuid));
        uint16_t id = ALIPAYY_CHARACTERISTIC_ID;
        gatts_add_chara_info_t character;
        uint16_t value = 0;

        memset_s(&character, sizeof(gatts_add_chara_info_t), 0, sizeof(gatts_add_chara_info_t));
        character.chara_uuid.uuid_len = sizeof(id);
        memcpy_s(character.chara_uuid.uuid, sizeof(character.chara_uuid.uuid), (char *)&id, sizeof(id));
        character.permissions = GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE;
        character.properties =
            GATT_CHARACTER_PROPERTY_BIT_READ | GATT_CHARACTER_PROPERTY_BIT_WRITE | GATT_CHARACTER_PROPERTY_BIT_NOTIFY;
        character.value_len = ALIPAY_CHARACTERISTIC_MAX_LENGTH;
        character.value = g_alipay_character_buf;

        int ret = gatts_add_characteristic(server_id, srv_handle, &character);
        if (ret != ERRCODE_SUCC) {
            ALIPAY_PRINT_ERR(0, "gatts_add_characteristic fail, ret:%u.\r\n", ret);
        }
        g_alipay_svr_handle = srv_handle;
    }
}

/* 将uint16的uuid数字转化为bt_uuid_t */
static void bts_data_to_uuid_len2(uint16_t uuid_data, bt_uuid_t *out_uuid)
{
    out_uuid->uuid_len = UUID16_LEN;
    out_uuid->uuid[0] = (uint8_t)(uuid_data >> 8); /* 8: octet bit num */
    out_uuid->uuid[1] = (uint8_t)(uuid_data);
}

static void svr_alipay_bt_gatts_characteristic_add_cb(uint8_t server_id, bt_uuid_t *uuid, uint16_t srv_handle,
    gatts_add_character_result_t *result, errcode_t status)
{
    unused(status);
    if ((uuid->uuid_len == sizeof(uint16_t)) && (*((uint16_t *)uuid->uuid) == ALIPAYY_CHARACTERISTIC_ID)) {
        ALIPAY_PRINT_DEBUG(0, "BtGattsCharacteristicAddCallback 0x%x 0x%x 0x%x 0x%x 0x%x",
                           status, server_id, srv_handle, result->handle, result->value_handle);
        uint8_t bas_ccc_val[] = {
            0x01,
            0x00
        };
        bt_uuid_t ccc_uuid = { 0 };
        bts_data_to_uuid_len2(0x2902, &ccc_uuid);
        gatts_add_desc_info_t descriptor;
        descriptor.desc_uuid = ccc_uuid;
        descriptor.permissions = GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE;
        descriptor.value_len = (uint16_t)sizeof(bas_ccc_val);
        descriptor.value = bas_ccc_val;
        int ret = 0;
        ret = gatts_add_descriptor(server_id, srv_handle, &descriptor);
        if (ret != ERRCODE_SUCC) {
            ALIPAY_PRINT_ERR(0, "gatts_add_descriptor fail, ret:%u.\r\n", ret);
        }
        bd_addr_t addr_info;
        gatts_add_desc_info_t descriptor_ap;
        gap_ble_get_local_addr(&addr_info);
        bts_data_to_uuid_len2(0x4b02, &ccc_uuid);
        descriptor_ap.desc_uuid = ccc_uuid;
        descriptor_ap.permissions = GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE;
        descriptor_ap.value_len = BD_ADDR_LEN;
        uint8_t ble_data_mac_switch[BD_ADDR_LEN] = {0};
        int k = BD_ADDR_LEN;
        for (int i = 0; i < BD_ADDR_LEN; i++) {
            ble_data_mac_switch[--k] = addr_info.addr[i];
        }
        descriptor_ap.value = ble_data_mac_switch;
        ret = gatts_add_descriptor(server_id, srv_handle, &descriptor_ap);
        if (ret != ERRCODE_SUCC) {
            ALIPAY_PRINT_ERR(0, "gatts_add_descriptor second fail, ret:%u.\r\n", ret);
        }
        ret = gatts_start_service(server_id, srv_handle);
        if (ret != ERRCODE_SUCC) {
            ALIPAY_PRINT_ERR(0, "gatts_start_service fail, ret:%u.\r\n", ret);
        }
        g_alipay_characteristic_handle = result->value_handle;
    }
}

static void svr_alipay_bt_gatts_descriptor_add_cb(uint8_t server_id, bt_uuid_t *uuid, uint16_t srv_handle,
    uint16_t desc_handle, errcode_t status)
{
    unused(server_id);
    unused(uuid);
    unused(status);
    if (g_alipay_svr_handle == srv_handle) {
        ALIPAY_PRINT_DEBUG(0, "BtGattsDescriptorAddCallback 0x%x 0x%x 0x%x 0x%x", status, server_id, srv_handle,
            desc_handle);
        static uint8_t desc_handle_count = 0;
        desc_handle_count = desc_handle_count % ALIPAY_DESC_HANDLE_NUM;
        g_alipay_descriptor_handle[desc_handle_count] = desc_handle;
        desc_handle_count++;
    }
}

static void svr_alipay_bt_gatts_service_start_cb(uint8_t server_id, uint16_t srv_handle, errcode_t status)
{
    unused(server_id);
    unused(status);
    if (g_alipay_svr_handle == srv_handle) {
        ALIPAY_PRINT_DEBUG(0, "BtGattsServiceStartCallback 0x%x 0x%x 0x%x", status, server_id, srv_handle);
    }
}

static void svr_alipay_bt_gatts_service_stop_cb(uint8_t server_id, uint16_t srv_handle, errcode_t status)
{
    unused(server_id);
    unused(srv_handle);
    unused(status);
    ALIPAY_PRINT_DEBUG(0, "BtGattsServiceStopCallback 0x%x 0x%x 0x%x", status, server_id, srv_handle);
}

static void svr_alipay_bt_gatts_service_del_cb(uint8_t server_id, errcode_t status)
{
    unused(server_id);
    unused(status);
    ALIPAY_PRINT_DEBUG(0, "BtGattsServiceDeleteCallback 0x%x 0x%x", status, server_id);
}

static void svr_alipay_bt_gatts_req_read_cb(uint8_t server_id, uint16_t conn_id, gatts_req_read_cb_t *read_cb_para,
    errcode_t status)
{
    unused(server_id);
    unused(conn_id);
    unused(read_cb_para);
    unused(status);
    ALIPAY_PRINT_DEBUG(0, "BtGattsRequestReadCallback");
}

static void svr_alipay_bt_gatts_req_write_cb(uint8_t server_id, uint16_t conn_id, gatts_req_write_cb_t *write_cb_para,
    errcode_t status)
{
    unused(server_id);
    unused(conn_id);
    unused(status);
    if ((g_alipay_characteristic_handle == write_cb_para->handle) ||
        (g_alipay_descriptor_handle[0] == write_cb_para->handle) ||
        (g_alipay_descriptor_handle[1] == write_cb_para->handle)) {
        ALIPAY_PRINT_DEBUG(0, "write callback,handle:%u str:%s len:%u",
                           write_cb_para->handle, write_cb_para->value, write_cb_para->length);
        uint8_t *msg_data = write_cb_para->value;
        errcode_t ret = svr_alipay_msg_write(ALIPAY_MSG_BT, msg_data, write_cb_para->length, false);
        if (ret != ERRCODE_SUCC) {
            ALIPAY_PRINT_ERR(0, "svr_alipay_msg_write fail, ret:%u.\r\n", ret);
        }
    }
    return;
}

void svr_alipay_bt_gatts_noyify(uint8_t *data, uint16_t data_len)
{
    gatts_ntf_ind_t data_info;
    data_info.attr_handle = g_alipay_characteristic_handle;
    data_info.value = data;
    data_info.value_len = data_len;

    errcode_t ret = gatts_notify_indicate(g_alipay_server_id, g_alipay_conn_id, &data_info);
    ALIPAY_PRINT_DEBUG(0, "gatts_notify_indicate ret:0x%x. serverid:%u,conn_id:%u,handle:%u.", ret, g_alipay_server_id,
        g_alipay_conn_id, g_alipay_characteristic_handle);
}

static void svr_alipay_btcallback_reg(void)
{
    g_alipay_gapble_cbs.conn_state_change_cb = svr_alipay_ble_connstate_change_cb;
    g_alipay_gapble_cbs.start_adv_cb = svr_alipay_ble_adv_start_cb;
    g_alipay_gapble_cbs.stop_adv_cb = svr_alipay_ble_adv_stop_cb;
    g_alipay_gapble_cbs.set_adv_data_cb = svr_alipay_ble_set_adv_data_cb;
    g_alipay_gapble_cbs.terminate_adv_cb = svr_alipay_ble_terminate_adv_cb;
#ifndef SUPPORT_OHOSFWK
    gap_ble_register_callbacks(&g_alipay_gapble_cbs);
#else
    wrapper_gap_ble_register_callbacks(&g_alipay_gapble_cbs);
#endif

    g_alipay_gatts_cbs.add_service_cb = svr_alipay_bt_gatts_service_add_cb;
    g_alipay_gatts_cbs.add_characteristic_cb = svr_alipay_bt_gatts_characteristic_add_cb;
    g_alipay_gatts_cbs.add_descriptor_cb = svr_alipay_bt_gatts_descriptor_add_cb;
    g_alipay_gatts_cbs.start_service_cb = svr_alipay_bt_gatts_service_start_cb;
    g_alipay_gatts_cbs.stop_service_cb = svr_alipay_bt_gatts_service_stop_cb;
    g_alipay_gatts_cbs.delete_service_cb = svr_alipay_bt_gatts_service_del_cb;
    g_alipay_gatts_cbs.read_request_cb = svr_alipay_bt_gatts_req_read_cb;
    g_alipay_gatts_cbs.write_request_cb = svr_alipay_bt_gatts_req_write_cb;
    g_alipay_gatts_cbs.mtu_changed_cb = NULL;
#ifndef SUPPORT_OHOSFWK
    gatts_register_callbacks(&g_alipay_gatts_cbs);
#else
    wrapper_gatts_register_callbacks(&g_alipay_gatts_cbs);
#endif
}

static void svr_alipay_bt_init_gatts(void)
{
    uint16_t service_id = ALIPAYY_SERVICE_ID;
    uint8_t server_id = 0;
    bt_uuid_t uuid;
    int ret;

    uuid.uuid_len = sizeof(service_id);
    memcpy_s(uuid.uuid, sizeof(uuid.uuid), (char *)&service_id, sizeof(service_id));
#ifndef SUPPORT_OHOSFWK
    ret = gatts_register_server(&uuid, &server_id); /* 注册gatt server */
#else
    ret = wrapper_gatts_register_server(&uuid, &server_id); /* 注册gatt server */
#endif
    if (ret != ERRCODE_SUCC) {
        ALIPAY_PRINT_ERR(0, "gatts_register_server ret:%u.\r\n", ret);
    }
    ALIPAY_PRINT_DEBUG(0, "gatts_register_server ret: 0x%x server_id = 0x%x", ret, server_id);
    g_alipay_server_id = server_id;
    uuid.uuid_len = sizeof(service_id);
    memcpy_s(uuid.uuid, sizeof(uuid.uuid), (char *)&service_id, sizeof(service_id));
#ifndef SUPPORT_OHOSFWK
    ret = gatts_add_service(server_id, &uuid, true);
#else
    ret = wrapper_gatts_add_service(server_id, &uuid, true);
#endif
    if (ret != ERRCODE_SUCC) {
        ALIPAY_PRINT_ERR(0, "gatts_add_service ret:%u.\r\n", ret);
    }
    return;
}

void svr_alipay_bluetooth_init(void)
{
#ifndef SUPPORT_OHOSFWK
    bool ret = enable_bt_stack();
    if (ret != ERRCODE_SUCC) {
        ALIPAY_PRINT_ERR(0, "enable_bt_stack ret:%u.\r\n", ret);
    }
#endif
    if (g_alipay_ble_init_flag == true) {
        return;
    }
    svr_alipay_btcallback_reg();
    svr_alipay_bt_init_gatts();
    g_alipay_ble_init_flag = true;
    return;
}
