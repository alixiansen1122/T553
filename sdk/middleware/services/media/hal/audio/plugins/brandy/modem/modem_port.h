/*
* Copyright (c) @CompanyNameMagicTag. 2023-2023. All rights reserved.
* Description: modem port header
* Author: Media Software Group
* Create: 2023-07-05
*/

#ifndef MODEM_PORT_H
#define MODEM_PORT_H

#include "audio_port_plugin.h"

typedef const AudioPortPlugin *(*GetModemPortPlugin)(AudioPluginType pluginType);

const AudioPortPlugin *GetModemPortPluginFuncs(AudioPluginType pluginType);

#endif
