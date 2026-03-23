/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: available devices manager impl
* Author: Media Software Group
* Create: 2021-04-07
*/

#include "devices_manager.h"
#include <limits.h>
#include <math.h>
#include "device_manager.h"
#include "audio_errors.h"
#include "audio_utils.h"
#include "am_list.h"
#include "securec.h"

#define LOG_MODULE_NAME "AvailableDevicesMgr"

typedef struct {
    List  availableInputDevices;
    List  availableOutputDevices;
} AvlDevicesMgrContext;

static AvlDevicesMgrContext g_avlDevicesMgrCtx;

/* DeviceList */
static void DeviceListInit(List *deviceListHead);
static int32_t DeviceListPushFront(List *deviceListHead, const AudioDeviceInfo *deviceInfo);
static int32_t DeviceListErase(List *deviceListHead, const AudioDeviceInfo *deviceInfo);
static bool DeviceListFind(const List *deviceListHead, AudioDeviceType device);
static void DeviceListDeInit(List *deviceListHead);

static int32_t GetAvailableDevices(DeviceFlag flag);

int32_t AvlDevicesMgrInit(void)
{
    int32_t ret;
    DeviceListInit(&g_avlDevicesMgrCtx.availableOutputDevices);
    ret = GetAvailableDevices(OUTPUT_DEVICES_FLAG);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("GetAvailableDevices OUTPUT_DEVICES_FLAG failed");
        DeviceListDeInit(&g_avlDevicesMgrCtx.availableOutputDevices);
        return AUDIO_ERROR;
    }
    DeviceListInit(&g_avlDevicesMgrCtx.availableInputDevices);
    ret = GetAvailableDevices(INPUT_DEVICES_FLAG);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("GetAvailableDevices INPUT_DEVICES_FLAG failed");
        DeviceListDeInit(&g_avlDevicesMgrCtx.availableOutputDevices);
        DeviceListDeInit(&g_avlDevicesMgrCtx.availableInputDevices);
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

bool AvlDevicesMgrIsAvailable(AudioDeviceType device)
{
    List *deviceList = &g_avlDevicesMgrCtx.availableOutputDevices;
    if (DeviceListFind(deviceList, device)) {
        return true;
    }
    deviceList = &g_avlDevicesMgrCtx.availableInputDevices;
    if (DeviceListFind(deviceList, device)) {
        return true;
    }
    return false;
}

int32_t AvlDevicesMgrPushFront(const AudioDeviceInfo *deviceInfo)
{
    List *deviceList = NULL;
    if (deviceInfo == NULL) {
        return AUDIO_ERROR;
    }
    deviceList = deviceInfo->flag == INPUT_DEVICES_FLAG ? &g_avlDevicesMgrCtx.availableInputDevices :
        &g_avlDevicesMgrCtx.availableOutputDevices;
    return DeviceListPushFront(deviceList, deviceInfo);
}

int32_t AvlDevicesMgrPop(const AudioDeviceInfo *deviceInfo)
{
    List *deviceList = NULL;
    if (deviceInfo == NULL) {
        return AUDIO_ERROR;
    }
    deviceList = deviceInfo->flag == INPUT_DEVICES_FLAG ? &g_avlDevicesMgrCtx.availableInputDevices :
        &g_avlDevicesMgrCtx.availableOutputDevices;
    return DeviceListErase(deviceList, deviceInfo);
}

void AvlDevicesMgrDeInit(void)
{
    DeviceListDeInit(&g_avlDevicesMgrCtx.availableInputDevices);
    DeviceListDeInit(&g_avlDevicesMgrCtx.availableOutputDevices);
}

static int32_t GetAvailableDevices(DeviceFlag flag)
{
    List *availableDevices = NULL;
    if (flag == OUTPUT_DEVICES_FLAG) {
        availableDevices = &g_avlDevicesMgrCtx.availableOutputDevices;
    } else {
        availableDevices = &g_avlDevicesMgrCtx.availableInputDevices;
    }
    uint32_t deviceCnt = 0;
    int32_t ret = DeviceMgrGetAvailableDeviceCount(flag, &deviceCnt);
    if (ret != AUDIO_SUCCESS ||
        deviceCnt == 0 ||
        deviceCnt == UINT_MAX) {
        ALOGE("DeviceMgrGetAvailableDeviceCount flag :%x failed :%x", flag, ret);
        return ret;
    }
    AudioDeviceInfo *devices = (AudioDeviceInfo *)malloc(deviceCnt * sizeof(AudioDeviceInfo));
    if (devices == NULL) {
        ALOGE("GetAvailableDevices malloc failed");
        return AUDIO_NO_MEM;
    }
    ret = DeviceMgrGetAvailableDevices(flag, deviceCnt, devices);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("DeviceMgrGetAvailableDevices failed :%x", ret);
        free(devices);
        return ret;
    }
    for (uint32_t i = 0; i < deviceCnt; i++) {
        ret = DeviceListPushFront(availableDevices, &devices[i]);
        if (ret != AUDIO_SUCCESS) {
            free(devices);
            ALOGE("DeviceListAddAtHead devices[i] :%u failed :0x%x", i, ret);
            return ret;
        }
    }
    free(devices);
    return AUDIO_SUCCESS;
}

static uintptr_t DeviceListDup(uintptr_t ptr)
{
    if (ptr == 0) {
        return 0;
    }
    AudioDeviceInfo *deviceInfo = (AudioDeviceInfo *)malloc(sizeof(AudioDeviceInfo));
    if (deviceInfo == NULL) {
        ALOGE("deviceInfo malloc failed");
        return 0;
    }
    if (memcpy_s(deviceInfo, sizeof(AudioDeviceInfo), (AudioDeviceInfo *)ptr,
                 sizeof(AudioDeviceInfo)) != EOK) {
        ALOGE("memcpy_s  deviceInfo failed");
        free(deviceInfo);
        return 0;
    }
    return (uintptr_t)deviceInfo;
}

static void DeviceListFree(uintptr_t ptr)
{
    if (ptr == 0) {
        return;
    }
    AudioDeviceInfo *deviceInfo = (AudioDeviceInfo *)ptr;
    free(deviceInfo);
}


static void DeviceListInit(List *deviceListHead)
{
    DupFreeFuncPair dataFunc;
    dataFunc.dupFunc = DeviceListDup;
    dataFunc.freeFunc = DeviceListFree;
    ListInit(deviceListHead, &dataFunc);
}

static int32_t DeviceListPushFront(List *deviceListHead, const AudioDeviceInfo *deviceInfo)
{
    uint32_t ret = ListPushFront(deviceListHead, (uintptr_t)deviceInfo);
    return ret == LIST_OK ? AUDIO_SUCCESS : AUDIO_ERROR;
}

static int32_t DeviceListErase(List *deviceListHead, const AudioDeviceInfo *deviceInfo)
{
    bool found = false;
    for (ListIterator it = ListIterBegin(deviceListHead); it != ListIterEnd(deviceListHead);) {
        AudioDeviceInfo *deviceInfoTmp = (AudioDeviceInfo *)ListIterData(it);
        if (deviceInfoTmp->device == deviceInfo->device) {
            it = ListIterErase(deviceListHead, it);
            found = true;
            break;
        } else {
            it = ListIterNext(deviceListHead, it);
        }
    }
    return found ? AUDIO_SUCCESS : AUDIO_ERROR;
}

static bool DeviceListFind(const List *deviceListHead, AudioDeviceType device)
{
    for (ListIterator it = ListIterBegin(deviceListHead); it != ListIterEnd(deviceListHead);
        it = ListIterNext(deviceListHead, it)) {
        AudioDeviceInfo *deviceInfo = (AudioDeviceInfo *)ListIterData(it);
        if (deviceInfo->device == device) {
            return true;
        }
    }
    return false;
}

static void DeviceListDeInit(List *deviceListHead)
{
    ListDeinit(deviceListHead);
}
