/*
 * Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
 * Description: media hal common
 * Author: Media Software Group
 * Create: 2021-03-02
 */

#include "device_config.h"
#include "securec.h"
#include "audio_errors.h"
#include "audio_utils.h"

#define LOG_MODULE_NAME "AudioDeviceMgr"

#define PRIMARY_ADAPTER_NAME   "Primary"
#define BLUETOOTH_ADAPTER_NAME "Bluetooth"
#define MODEM_ADAPTER_NAME     "Modem"

/* primary adapter */
static AudioDeviceType g_primaryDevice[] = {
    OUT_BUILTIN_SPEAKER,
    IN_BUILTIN_MIC,
};

/* bluetoot adapter */
static AudioDeviceType g_bluetoothDevice[] = {
    OUT_BLUETOOTH_SCO,
    OUT_BLUETOOTH_A2DP,
    IN_BLUETOOTH_SCO,
    IN_BLUETOOTH_A2DP,
};

/* modem adapter */
static AudioDeviceType g_modemDevice[] = {
    OUT_MODEM,
    OUT_MODEM_HEADSET,
    IN_MODEM,
    IN_MODEM_HEADSET,
};

static int32_t ParseConfig(const char *fileName)
{
    AUDIO_UNUSED(fileName);
    ALOGW("config not exist, return failed use default");
    return AUDIO_ERROR;
}

static int32_t SetDefault(void)
{
    return AUDIO_SUCCESS;
}

int32_t GetAdapterListFromConfig(AdapterDeviceMap **adapterList, int32_t *adapterNum)
{
    if (adapterList == NULL || adapterNum == NULL) {
        return AUDIO_ERROR;
    }
    /* adapter list */
    static AdapterDeviceMap  s_adapterConfigList[] = {
        {
            .adapterName = PRIMARY_ADAPTER_NAME,
            .supportDeviceCount = sizeof(g_primaryDevice) / sizeof(AudioDeviceType),
            .supportDevice = g_primaryDevice,
        },
        {
            .adapterName = BLUETOOTH_ADAPTER_NAME,
            .supportDeviceCount = sizeof(g_bluetoothDevice) / sizeof(AudioDeviceType),
            .supportDevice = g_bluetoothDevice,
        },
        {
            .adapterName = MODEM_ADAPTER_NAME,
            .supportDeviceCount = sizeof(g_modemDevice) / sizeof(AudioDeviceType),
            .supportDevice = g_modemDevice,
        },
    };
    *adapterList = s_adapterConfigList;
    *adapterNum = (int32_t)(sizeof(s_adapterConfigList) / sizeof(AdapterDeviceMap));
    return AUDIO_SUCCESS;
}

int32_t LoadConfig(void)
{
    if (ParseConfig(NULL) == AUDIO_ERROR) {
        return SetDefault();
    }
    ALOGW("config parse success, need process");
    return AUDIO_SUCCESS;
}