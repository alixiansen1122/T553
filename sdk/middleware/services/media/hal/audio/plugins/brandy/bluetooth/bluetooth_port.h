/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: bluetooth port header
* Author: Media Software Group
* Create: 2021-09-28
*/

#ifndef BLUETOOTH_PORT_H
#define BLUETOOTH_PORT_H

#include "audio_port_plugin.h"

typedef const AudioPortPlugin *(*GetBluetoothPortPlugin)(AudioPluginType pluginType);

const AudioPortPlugin *GetBluetoothPortPluginFuncs(AudioPluginType pluginType);

#endif
