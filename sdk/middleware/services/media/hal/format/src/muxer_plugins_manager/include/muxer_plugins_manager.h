/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: muxer plugin manager interfaces
* Author: Media Software Group
* Create: 2021-04-21
*/

#ifndef DEMUXER_PLUGINS_MANAGER_H
#define DEMUXER_PLUGINS_MANAGER_H

#include "plugins_manager_common.h"
#include "media_hal_common.h"
#include "muxer_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

void MuxerPluginManagerInit(void);
void MuxerPluginManagerDeInit(void);
const MuxerPluginEntry *MuxerPluginManagerFindPlugin(const MuxerOutput *cfgParam, const ProtocolFun *protocol,
    HalHandle *muxerHdl);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  // MUXER_PLUGINS_MANAGER_H
/** @} */