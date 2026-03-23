/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: Audio plugins manager
* Author: Media Software Group
* Create: 2021-07-21
*/

#ifndef AUDIO_PLUGINS_MANAGER_H
#define AUDIO_PLUGINS_MANAGER_H

#include "audio_port_plugin.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

void AudioPluginManagerInit(void);
void AudioPluginManagerDeInit(void);
const void *AudioPluginManagerGetPlugin(AudioPluginType pluginType);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  // FORMAT_INTERFACE_H
/** @} */