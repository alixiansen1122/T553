/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: protocol plugin manager
* Author: Media Software Group
* Create: 2021-04-21
*/

#ifndef PROTOCOL_PLUGINS_MANAGER_H
#define PROTOCOL_PLUGINS_MANAGER_H

#include "plugins_manager_common.h"
#include "protocol_plugins_manager.h"
#include "protocol_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

void ProtocolPluginManagerInit(void);
void ProtocolPluginManagerDeInit(void);
int32_t ProtocolPluginManagerFindPlugin(const char *url, int32_t len, ProtocolFun *func);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  // PROTOCOL_PLUGINS_MANAGER_H
/** @} */
