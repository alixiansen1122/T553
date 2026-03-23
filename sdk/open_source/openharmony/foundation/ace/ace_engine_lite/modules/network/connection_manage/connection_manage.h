/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: network connection manage
 * Author: SoftwarePlatform Group
 * Create: 2025-11-12
 */


#include "lwip_public.h"
#include "network_listener.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines the callback function net connection status change.
 *
 * @param networkType network type, refer to #NetworkType.
 * @param netConnectState net connection status, refer to #ConnectionState.
 */
typedef void (*NetConnectionCallback)(const uint8_t networkType, const uint8_t netConnectState);

/**
 * @brief This function register network connection status listener.
 * @param[in] networkType network type, refer to #NetworkType.
 * @param[in] callback   net connection callback.
 * @return 0 if success; non-0 otherwise.
 */
int RegisterConnectListener(uint8_t networkType, NetConnectionCallback callback);

/**
 * @brief Defines the callback unregister function for listening net connection status change.
 * @param[in] networkType network type, refer to #NetworkType.
 * @param[in] callback   net connection callback.
 * @return 0 if success; non-0 otherwise.
 */
void UnregisterConnectListener(uint8_t networkType, NetConnectionCallback callback);

/**
 * @brief This function get current network connection status.
 * @param[in] networkType network type, refer to #NetworkType.
 * @return netConnectState net connection status, refer to #ConnectionState.
 */
uint8_t GetNetConnectState(uint8_t networkType);

void NetConnectChange(uint8_t networkType, uint8_t netConnectState);

#ifdef __cplusplus
}
#endif