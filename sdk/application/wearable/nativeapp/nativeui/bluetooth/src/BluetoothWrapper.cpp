/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: BluetoothModel view
 * Author:
 * Create: 2021-10-13
 */

#include <algorithm>
#include <list>
#include "bluetooth/BluetoothWrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

static bool gatts_register_flag = false;
static bool gattc_register_flag = false;
static uint8_t gatts_server_id = 0;
static uint8_t gattc_client_id = 0;
std::list<gap_ble_callbacks_t> g_gap_ble_callback_list;
std::list<gatts_callbacks_t> g_gatts_callback_list;
std::list<gatts_callbacks_t> g_gatts_ohos_callback_list;
std::list<gattc_callbacks_t> g_gattc_callback_list;
std::list<bt_uuid_t> g_uuid_list;
std::list<uint16_t> g_svr_handle_list;

errcode_t wrapper_gap_ble_register_callbacks(gap_ble_callbacks_t *func)
{
    g_gap_ble_callback_list.push_back(*func);
    return ERRCODE_SUCC;
}

errcode_t wrapper_gatts_register_callbacks(gatts_callbacks_t *func)
{
    g_gatts_callback_list.push_back(*func);
    return ERRCODE_SUCC;
}

bool isGattsCallbacksDuplicate(const gatts_callbacks_t& a, const gatts_callbacks_t& b)
{
    return (a.add_service_cb == b.add_service_cb) &&
           (a.add_characteristic_cb == b.add_characteristic_cb) &&
           (a.add_descriptor_cb == b.add_descriptor_cb) &&
           (a.start_service_cb == b.start_service_cb) &&
           (a.stop_service_cb == b.stop_service_cb) &&
           (a.delete_service_cb == b.delete_service_cb) &&
           (a.read_request_cb == b.read_request_cb) &&
           (a.write_request_cb == b.write_request_cb) &&
           (a.mtu_changed_cb == b.mtu_changed_cb);
}

errcode_t wrapper_gatts_register_ohos_callbacks(gatts_callbacks_t *func)
{
    if (func == nullptr) {
        return ERRCODE_INVALID_PARAM;
    }

    auto it = std::find_if(
        g_gatts_ohos_callback_list.begin(),
        g_gatts_ohos_callback_list.end(),
        [&](const gatts_callbacks_t& existing) {
            return isGattsCallbacksDuplicate(existing, *func);
        }
    );

    if (it != g_gatts_ohos_callback_list.end()) {
        return ERRCODE_SUCC;
    }

    g_gatts_ohos_callback_list.push_back(*func);
    return ERRCODE_SUCC;
}

errcode_t wrapper_gattc_register_callbacks(gattc_callbacks_t *func)
{
    g_gattc_callback_list.push_back(*func);
    return ERRCODE_SUCC;
}

void wrapper_gap_ble_enable_callback(errcode_t status)
{
    for (auto callback : g_gap_ble_callback_list) {
        if (callback.ble_enable_cb != nullptr) {
            callback.ble_enable_cb(status);
        }
    }
}

void wrapper_gap_ble_disable_callback(errcode_t status)
{
    for (auto callback : g_gap_ble_callback_list) {
        if (callback.ble_disable_cb != nullptr) {
            callback.ble_disable_cb(status);
        }
    }
}

void wrapper_gap_ble_start_adv_callback(uint8_t adv_id, adv_status_t status)
{
    for (auto callback : g_gap_ble_callback_list) {
        if (callback.start_adv_cb) {
            callback.start_adv_cb(adv_id, status);
        }
    }
}

void wrapper_gap_ble_stop_adv_callback(uint8_t adv_id, adv_status_t status)
{
    for (auto callback : g_gap_ble_callback_list) {
        if (callback.stop_adv_cb) {
            callback.stop_adv_cb(adv_id, status);
        }
    }
}

void wrapper_gap_ble_set_adv_data_callback(uint8_t adv_id, errcode_t status)
{
    for (auto callback : g_gap_ble_callback_list) {
        if (callback.set_adv_data_cb) {
            callback.set_adv_data_cb(adv_id, status);
        }
    }
}

void wrapper_gap_ble_set_adv_param_callback(uint8_t adv_id, errcode_t status)
{
    for (auto callback : g_gap_ble_callback_list) {
        if (callback.set_adv_param_cb) {
            callback.set_adv_param_cb(adv_id, status);
        }
    }
}

void wrapper_gap_ble_scan_result_callback(gap_scan_result_data_t *scan_result_data)
{
    for (auto callback : g_gap_ble_callback_list) {
        if (callback.scan_result_cb) {
            callback.scan_result_cb(scan_result_data);
        }
    }
}

void wrapper_gap_ble_set_scan_param_callback(errcode_t status)
{
    for (auto callback : g_gap_ble_callback_list) {
        if (callback.set_scan_param_cb) {
            callback.set_scan_param_cb(status);
        }
    }
}

void wrapper_gap_ble_connect_state_changed_callback(uint16_t conn_id, bd_addr_t *addr, gap_ble_conn_state_t conn_state,
    gap_ble_pair_state_t pair_state, gap_ble_disc_reason_t disc_reason)
{
    for (auto callback : g_gap_ble_callback_list) {
        if (callback.conn_state_change_cb) {
            callback.conn_state_change_cb(conn_id, addr, conn_state, pair_state, disc_reason);
        }
    }
}

void wrapper_gap_ble_paired_complete_callback(uint16_t conn_id, const bd_addr_t *addr, errcode_t status)
{
    for (auto callback : g_gap_ble_callback_list) {
        if (callback.pair_result_cb) {
            callback.pair_result_cb(conn_id, addr, status);
        }
    }
}

void wrapper_gap_ble_terminate_adv_callback(uint8_t adv_id, adv_status_t status)
{
    for (auto callback : g_gap_ble_callback_list) {
        if (callback.terminate_adv_cb) {
            callback.terminate_adv_cb(adv_id, status);
        }
    }
}

void wrapper_gap_ble_read_rssi_callback(uint16_t conn_id, int8_t rssi, errcode_t status)
{
    for (auto callback : g_gap_ble_callback_list) {
        if (callback.read_rssi_cb) {
            callback.read_rssi_cb(conn_id, rssi, status);
        }
    }
}

void wrapper_gap_ble_auth_complete_callback(uint16_t conn_id, const bd_addr_t *addr,
    errcode_t status, const ble_auth_info_evt_t* evt)
{
    for (auto callback : g_gap_ble_callback_list) {
        if (callback.auth_complete_cb) {
            callback.auth_complete_cb(conn_id, addr, status, evt);
        }
    }
}

void wrapper_gap_ble_connect_param_update_callback(uint16_t conn_id, errcode_t status,
    const gap_ble_conn_param_update_t *param)
{
    for (auto callback : g_gap_ble_callback_list) {
        if (callback.conn_param_update_cb) {
            callback.conn_param_update_cb(conn_id, status, param);
        }
    }
}

void wrapper_gatts_add_service_callback(uint8_t server_id, bt_uuid_t *uuid, uint16_t handle, errcode_t status)
{
    g_svr_handle_list.push_back(handle);
    for (auto callback : g_gatts_callback_list) {
        if (callback.add_service_cb) {
            callback.add_service_cb(server_id, uuid, handle, status);
        }
    }
}
void wrapper_gatts_add_characteristic_callback(uint8_t server_id, bt_uuid_t *uuid, uint16_t service_handle,
    gatts_add_character_result_t *result, errcode_t status)
{
    for (auto callback : g_gatts_callback_list) {
        if (callback.add_characteristic_cb) {
            callback.add_characteristic_cb(server_id, uuid, service_handle, result, status);
        }
    }
}
void wrapper_gatts_add_descriptor_callback(uint8_t server_id, bt_uuid_t *uuid, uint16_t service_handle,
    uint16_t handle, errcode_t status)
{
    for (auto callback : g_gatts_callback_list) {
        if (callback.add_descriptor_cb) {
            callback.add_descriptor_cb(server_id, uuid, service_handle, handle, status);
        }
    }
}
void wrapper_gatts_start_service_callback(uint8_t server_id, uint16_t handle, errcode_t status)
{
    bool skip_flag = true;
    for (auto svr_handle : g_svr_handle_list) {
        if (svr_handle == handle) {
            skip_flag = false;
        }
    }
    if (!skip_flag) {
        for (auto callback : g_gatts_callback_list) {
            if (callback.start_service_cb) {
                callback.start_service_cb(server_id, handle, status);
            }
        }
        g_uuid_list.pop_front();
        wrapper_gatts_service_start();
    } else {
        for (auto callback : g_gatts_ohos_callback_list) {
            if (callback.start_service_cb) {
                callback.start_service_cb(server_id, handle, status);
            }
        }
    }
}

void wrapper_gatts_stop_service_callback(uint8_t server_id, uint16_t handle, errcode_t status)
{
    for (auto callback : g_gatts_callback_list) {
        if (callback.stop_service_cb) {
            callback.stop_service_cb(server_id, handle, status);
        }
    }
    for (auto callback : g_gatts_ohos_callback_list) {
        if (callback.stop_service_cb) {
            callback.stop_service_cb(server_id, handle, status);
        }
    }
}
void wrapper_gatts_delete_all_service_callback(uint8_t server_id, errcode_t status)
{
    for (auto callback : g_gatts_callback_list) {
        if (callback.delete_service_cb) {
            callback.delete_service_cb(server_id, status);
        }
    }
}
void wrapper_gatts_read_request_callback(uint8_t server_id, uint16_t conn_id, gatts_req_read_cb_t *read_cb_para, errcode_t status)
{
    for (auto callback : g_gatts_callback_list) {
        if (callback.read_request_cb) {
            callback.read_request_cb(server_id, conn_id, read_cb_para, status);
        }
    }
    for (auto callback : g_gatts_ohos_callback_list) {
        if (callback.read_request_cb) {
            callback.read_request_cb(server_id, conn_id, read_cb_para, status);
        }
    }
}
void wrapper_gatts_write_request_callback(uint8_t server_id, uint16_t conn_id, gatts_req_write_cb_t *write_cb_para, errcode_t status)
{
    for (auto callback : g_gatts_callback_list) {
        if (callback.write_request_cb) {
            callback.write_request_cb(server_id, conn_id, write_cb_para, status);
        }
    }
    for (auto callback : g_gatts_ohos_callback_list) {
        if (callback.write_request_cb) {
            callback.write_request_cb(server_id, conn_id, write_cb_para, status);
        }
    }
}

void wrapper_gatts_mtu_changed_callback(uint8_t server_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status)
{
    for (auto callback : g_gatts_callback_list) {
        if (callback.mtu_changed_cb) {
            callback.mtu_changed_cb(server_id, conn_id, mtu_size, status);
        }
    }
    for (auto callback : g_gatts_ohos_callback_list) {
        if (callback.mtu_changed_cb) {
            callback.mtu_changed_cb(server_id, conn_id, mtu_size, status);
        }
    }
}

void wrapper_gattc_discovery_svc_callback(uint8_t client_id, uint16_t conn_id,
    gattc_discovery_service_result_t *service, errcode_t status)
{
    for (auto callback : g_gattc_callback_list) {
        if (callback.discovery_svc_cb) {
            callback.discovery_svc_cb(client_id, conn_id, service, status);
        }
    }
}

void wrapper_gattc_discovery_svc_cmp_callback(uint8_t client_id, uint16_t conn_id,
    bt_uuid_t *uuid, errcode_t status)
{
    for (auto callback : g_gattc_callback_list) {
        if (callback.discovery_svc_cmp_cb) {
            callback.discovery_svc_cmp_cb(client_id, conn_id, uuid, status);
        }
    }
}

void wrapper_gattc_discovery_chara_callback(uint8_t client_id, uint16_t conn_id,
    gattc_discovery_character_result_t *character, errcode_t status)
{
    for (auto callback : g_gattc_callback_list) {
        if (callback.discovery_chara_cb) {
            callback.discovery_chara_cb(client_id, conn_id, character, status);
        }
    }
}

void wrapper_gattc_discovery_chara_cmp_callback(uint8_t client_id, uint16_t conn_id,
    gattc_discovery_character_param_t *param, errcode_t status)
{
    for (auto callback : g_gattc_callback_list) {
        if (callback.discovery_chara_cmp_cb) {
            callback.discovery_chara_cmp_cb(client_id, conn_id, param, status);
        }
    }
}

void wrapper_gattc_discovery_desc_callback(uint8_t client_id, uint16_t conn_id,
    gattc_discovery_descriptor_result_t *descriptor, errcode_t status)
{
    for (auto callback : g_gattc_callback_list) {
        if (callback.discovery_desc_cb) {
            callback.discovery_desc_cb(client_id, conn_id, descriptor, status);
        }
    }
}

void wrapper_gattc_discovery_desc_cmp_callback(uint8_t client_id, uint16_t conn_id,
    uint16_t character_handle, errcode_t status)
{
    for (auto callback : g_gattc_callback_list) {
        if (callback.discovery_desc_cmp_cb) {
            callback.discovery_desc_cmp_cb(client_id, conn_id, character_handle, status);
        }
    }
}

void wrapper_gattc_read_callback(uint8_t client_id, uint16_t conn_id,
    gattc_handle_value_t *read_result, gatt_status_t status)
{
    for (auto callback : g_gattc_callback_list) {
        if (callback.read_cb) {
            callback.read_cb(client_id, conn_id, read_result, status);
        }
    }
}

void wrapper_gattc_read_cmp_callback(uint8_t client_id, uint16_t conn_id,
    gattc_read_req_by_uuid_param_t *param, errcode_t status)
{
    for (auto callback : g_gattc_callback_list) {
        if (callback.read_cmp_cb) {
            callback.read_cmp_cb(client_id, conn_id, param, status);
        }
    }
}

void wrapper_gattc_write_callback(uint8_t client_id, uint16_t conn_id, uint16_t handle, gatt_status_t status)
{
    for (auto callback : g_gattc_callback_list) {
        if (callback.write_cb) {
            callback.write_cb(client_id, conn_id, handle, status);
        }
    }
}

void wrapper_gattc_mtu_changed_callback(uint8_t client_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status)
{
    for (auto callback : g_gattc_callback_list) {
        if (callback.mtu_changed_cb) {
            callback.mtu_changed_cb(client_id, conn_id, mtu_size, status);
        }
    }
}

void wrapper_gattc_notification_callback(uint8_t client_id, uint16_t conn_id, gattc_handle_value_t *data,
    errcode_t status)
{
    for (auto callback : g_gattc_callback_list) {
        if (callback.notification_cb) {
            callback.notification_cb(client_id, conn_id, data, status);
        }
    }
}

void wrapper_indication_callback(uint8_t client_id, uint16_t conn_id, gattc_handle_value_t *data,
    errcode_t status)
{
    for (auto callback : g_gattc_callback_list) {
        if (callback.indication_cb) {
            callback.indication_cb(client_id, conn_id, data, status);
        }
    }
}

static gap_ble_callbacks_t g_wrapper_gap_ble_callbacks = {
    .ble_enable_cb = wrapper_gap_ble_enable_callback,
    .ble_disable_cb = wrapper_gap_ble_disable_callback,
    .set_adv_data_cb = wrapper_gap_ble_set_adv_data_callback,
    .set_adv_param_cb = wrapper_gap_ble_set_adv_param_callback,
    .set_scan_param_cb = wrapper_gap_ble_set_scan_param_callback,
    .start_adv_cb = wrapper_gap_ble_start_adv_callback,
    .stop_adv_cb = wrapper_gap_ble_stop_adv_callback,
    .scan_result_cb = wrapper_gap_ble_scan_result_callback,
    .conn_state_change_cb = wrapper_gap_ble_connect_state_changed_callback,
    .pair_result_cb = wrapper_gap_ble_paired_complete_callback,
    .read_rssi_cb = wrapper_gap_ble_read_rssi_callback,
    .terminate_adv_cb = wrapper_gap_ble_terminate_adv_callback,
    .auth_complete_cb = wrapper_gap_ble_auth_complete_callback,
    .conn_param_update_cb = wrapper_gap_ble_connect_param_update_callback,
};

static gatts_callbacks_t g_wrapper_gatts_callbacks = {
    .add_service_cb = wrapper_gatts_add_service_callback,
    .add_characteristic_cb = wrapper_gatts_add_characteristic_callback,
    .add_descriptor_cb = wrapper_gatts_add_descriptor_callback,
    .start_service_cb = wrapper_gatts_start_service_callback,
    .stop_service_cb = wrapper_gatts_stop_service_callback,
    .delete_service_cb = wrapper_gatts_delete_all_service_callback,
    .read_request_cb = wrapper_gatts_read_request_callback,
    .write_request_cb = wrapper_gatts_write_request_callback,
    .mtu_changed_cb = wrapper_gatts_mtu_changed_callback,
};

static gattc_callbacks_t g_wrapper_gattc_callbacks = {
    .discovery_svc_cb = wrapper_gattc_discovery_svc_callback,
    .discovery_svc_cmp_cb = wrapper_gattc_discovery_svc_cmp_callback,
    .discovery_chara_cb = wrapper_gattc_discovery_chara_callback,
    .discovery_chara_cmp_cb = wrapper_gattc_discovery_chara_cmp_callback,
    .discovery_desc_cb = wrapper_gattc_discovery_desc_callback,
    .discovery_desc_cmp_cb = wrapper_gattc_discovery_desc_cmp_callback,
    .read_cb = wrapper_gattc_read_callback,
    .read_cmp_cb = wrapper_gattc_read_cmp_callback,
    .write_cb = wrapper_gattc_write_callback,
    .mtu_changed_cb = wrapper_gattc_mtu_changed_callback,
    .notification_cb = wrapper_gattc_notification_callback,
    .indication_cb = wrapper_indication_callback,
};

void wrapper_ble_register(void)
{
    gap_ble_register_callbacks(&g_wrapper_gap_ble_callbacks);
    gatts_register_callbacks(&g_wrapper_gatts_callbacks);
    gattc_register_callbacks(&g_wrapper_gattc_callbacks);
}

errcode_t wrapper_gattc_register_client(bt_uuid_t *uuid, uint8_t *clientId)
{
    errcode_t ret = ERRCODE_SUCC;
    if (gattc_register_flag) {
        *clientId = gattc_client_id;
        ret = ERRCODE_SUCC;
    } else {
        gattc_register_flag = true;
        ret = gattc_register_client(uuid, clientId);
        gattc_client_id = *clientId;
    }
    return ret;
}

errcode_t wrapper_gatts_register_server(bt_uuid_t *app_uuid, uint8_t *server_id)
{
    errcode_t ret = ERRCODE_SUCC;
    if (gatts_register_flag) {
        *server_id = gatts_server_id;
        ret = ERRCODE_SUCC;
    } else {
        gatts_register_flag = true;
        ret = gatts_register_server(app_uuid, server_id);
        gatts_server_id = *server_id;
    }
    return ret;
}

errcode_t wrapper_gatts_add_service(uint8_t server_id, bt_uuid_t *service_uuid, bool is_primary)
{
    g_uuid_list.push_back(*service_uuid);
    return ERRCODE_SUCC;
}

void wrapper_gatts_service_start(void)
{
    if (!g_uuid_list.empty()) {
        bt_uuid_t &uuid = g_uuid_list.front();
        // 服务异步添加，需要保证当前服务start后，再添加下一个服务
        (void)gatts_add_service(gatts_server_id, &uuid, true);
    } else {
        g_svr_handle_list.clear();
    }
}

#ifdef __cplusplus
}
#endif
