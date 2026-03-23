/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: primary port header
* Author: Media Software Group
* Create: 2021-09-28
*/

#ifndef AUDIO_PRIMARY_PORT_H
#define AUDIO_PRIMARY_PORT_H

#include "audio_port_plugin.h"

typedef const AudioPortPlugin *(*GetPrimaryPortPlugin)(AudioPluginType pluginType);

const AudioPortPlugin *GetPrimaryPortPluginFuncs(AudioPluginType pluginType);

#endif
