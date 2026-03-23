/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: network connection manage
 * Author: SoftwarePlatform Group
 * Create: 2025-11-12
 */

#include <list>
#include <mutex>
#include "settings/model/SettingBluetoothModel.h"
#include "msg_center_protocol.h"
#include "connection_manage.h"

typedef struct NetConnectionParam {
    uint8_t networkType;
    NetConnectionCallback callback;
} NetConnectionParam;

static std::list<NetConnectionParam *> g_callbackList;
static std::mutex g_callbackMutex;
static int g_registerCount = 0;


void NetConnectChange(uint8_t networkType, uint8_t netConnectState)
{
    g_callbackMutex.lock();
    for (auto iter = g_callbackList.begin(); iter != g_callbackList.end(); iter++) {
        NetConnectionCallback callback = (*iter)->callback;
        uint8_t type = (*iter)->networkType;

        if (type == networkType && callback != nullptr) {
            g_callbackMutex.unlock();
            callback(networkType, netConnectState);
            g_callbackMutex.lock();
        }
    }
    g_callbackMutex.unlock();
}

static void LwipNetConnectionChange(uint8_t netifType, uint8_t netConnectState)
{
    uint8_t networkType = NETWORK_TYPE_UNKNOWN;
    if (netifType == LWIP_NETIF_VOLTE) {
        networkType = NETWORK_TYPE_CELL_LTE_CAT1;
    }
    if (netifType == LWIP_NETIF_BT_PAN) {
        networkType = NETWORK_TYPE_BLUETOOTH_PAN;
    }
    NetConnectChange(networkType, netConnectState);
}

static void RegisterLwipNetListener(uint8_t networkType)
{
    if (networkType != NETWORK_TYPE_BLUETOOTH_PAN && networkType != NETWORK_TYPE_CELL_LTE_CAT1) {
        return;
    }
    if (g_registerCount == 0) {
        lwip_register_connect_listener(LwipNetConnectionChange);
    }
    g_registerCount++;
}

static void UnRegisterLwipNetListener(uint8_t networkType)
{
    if (networkType == NETWORK_TYPE_BLUETOOTH_PAN || networkType == NETWORK_TYPE_CELL_LTE_CAT1) {
        g_registerCount--;
        if (g_registerCount == 0) {
            lwip_unregister_connect_listener(LwipNetConnectionChange);
        }
    }
}

int RegisterConnectListener(uint8_t networkType, NetConnectionCallback callback)
{
    if (callback == nullptr) {
        return -1;
    }
    NetConnectionParam *param = (NetConnectionParam *)malloc(sizeof(NetConnectionParam));
    if (param == nullptr) {
        return -1;
    }
    param->networkType = networkType;
    param->callback = callback;

    g_callbackMutex.lock();
    for (auto iter = g_callbackList.begin(); iter != g_callbackList.end(); iter++) {
        if ((*iter)->callback == callback && (*iter)->networkType == networkType) {
            g_callbackMutex.unlock();
            free(param);
            return 0;
        }
    }
    g_callbackList.push_back(param);
    g_callbackMutex.unlock();
    RegisterLwipNetListener(networkType);
    return 0;
}

void UnregisterConnectListener(uint8_t networkType, NetConnectionCallback callback)
{
    if (callback == nullptr) {
        return;
    }
    g_callbackMutex.lock();
    for (auto iter = g_callbackList.begin(); iter != g_callbackList.end();) {
        if ((*iter)->networkType == networkType && (*iter)->callback == callback) {
            UnRegisterLwipNetListener((*iter)->networkType);
            free(*iter);
            *iter = nullptr;
            iter = g_callbackList.erase(iter);
        } else {
            iter++;
        }
    }
    g_callbackMutex.unlock();
}

uint8_t GetNetConnectState(uint8_t netType)
{
    uint8_t state = CONNECTION_STATUS_UNKNOWN;
    switch (netType) {
        case NETWORK_TYPE_WIFI:
            break;
        case NETWORK_TYPE_BLUETOOTH:
            state = OHOS::SettingBluetoothModel::GetInstance()->GetAclConnected() ? CONNECTION_STATUS_CONNECTED
                                                                           : CONNECTION_STATUS_DISCONNECTED;
            break;
        case NETWORK_TYPE_BLUETOOTH_LE:
            state = (msg_center_get_ble_conn_state() == GAP_BLE_STATE_CONNECTED) ? CONNECTION_STATUS_CONNECTED
                                                                                 : CONNECTION_STATUS_DISCONNECTED;
            break;
        case NETWORK_TYPE_BLUETOOTH_PAN:
            lwip_get_connect_status(LWIP_NETIF_BT_PAN, &state);
            break;
        case NETWORK_TYPE_CELL_LTE_CAT1:
            lwip_get_connect_status(LWIP_NETIF_VOLTE, &state);
            break;
        default:
            state = CONNECTION_STATUS_UNKNOWN;
            break;
    }
    return state;
}
