/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: dsoftbus cpp function adapt src file.
 * Author: CompanyName
 * Create:
 */
#include "lnn_ohos_account.h"
#include "lnn_settingdata_event_monitor.h"

#include "stdbool.h"
#include "softbus_errcode.h"

int32_t LnnGetSettingDeviceName(char *deviceName, uint32_t len)
{
    (void)deviceName;
    (void)len;
    return SOFTBUS_ERR;
}

int32_t LnnInitGetDeviceName(LnnDeviceNameHandler handler)
{
    (void)handler;
    return SOFTBUS_OK;
}

int32_t LnnGetOhosAccountInfo(uint8_t *accountHash, uint32_t len)
{
    (void)accountHash;
    (void)len;
    return SOFTBUS_OK;
}

int32_t LnnInitOhosAccount(void)
{
    return SOFTBUS_OK;
}

void LnnOnOhosAccountChanged(void)
{
}

bool LnnIsDefaultOhosAccount(void)
{
    return false;
}

int32_t LnnInitDeviceNameMonitorImpl(void)
{
    return SOFTBUS_OK;
}

void RegisterNameMonitor(void)
{
}

int32_t LnnInitCommonEventMonitorImpl(void)
{
    return SOFTBUS_OK;
}

int32_t LnnInitWifiServiceMonitorImpl(void)
{
    return SOFTBUS_OK;
}