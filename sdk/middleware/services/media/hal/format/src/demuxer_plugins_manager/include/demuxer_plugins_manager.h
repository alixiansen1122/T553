/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: Demuxer Plugin Manager interfaces
* Author: Media Software Group
* Create: 2021-04-21
*/

#ifndef DEMUXER_PLUGINS_MANAGER_H
#define DEMUXER_PLUGINS_MANAGER_H

#include "plugins_manager_common.h"
#include "demuxer_interface.h"
#include "media_hal_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

bool HasM3U8(const char *url);
void DemuxerPluginManagerInit(void);
void DemuxerPluginManagerDeInit(void);
const DemuxerPluginEntry *DemuxerPluginManagerFindPlugin(const DemuxerSource *source, const ProtocolFun *protocol,
    HalHandle *demuxerHdl);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  // FORMAT_INTERFACE_H
/** @} */