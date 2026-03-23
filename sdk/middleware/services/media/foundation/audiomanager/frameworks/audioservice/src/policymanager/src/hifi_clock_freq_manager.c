/*
* Copyright (c) CompanyNameMagicTag. 2023-2023. All rights reserved.
* Description: hifi clock freq manager impl
* Author: Media Software Group
* Create: 2023-11-09
*/

#include "hifi_clock_freq_manager.h"
#include <limits.h>
#include <math.h>
#include "pm_clocks.h"
#include "devices_manager.h"
#include "interrupt_manager.h"
#include "audio_errors.h"
#include "audio_utils.h"
#include "am_list.h"
#include "securec.h"
#include "media_thread_adapt.h"

#define LOG_MODULE_NAME "HifiClockFreqMgr"

typedef struct {
    AudioStreamType streamType;
    AudioDeviceType deviceType;
    pm_hifi_mode_t hifiMode;
} HifiClockFreqContext;

static List g_availableHifiClockFreqContext;
static pm_hifi_mode_t g_hifiMode = HIFI_LPM_CLK_MAX;
static MediaMutexHandle g_hifiClockFreqMutex = NULL;
static HifiClockFreqContext  g_hifiClockFreqList[] = {
    {
        .streamType = AUDIO_STREAM_ALARM,
        .deviceType = OUT_BUILTIN_SPEAKER,
        .hifiMode = HIFI_LPM_CLK_LEVEL1,
    },
    {
        .streamType = AUDIO_STREAM_ALARM_SYSTEM,
        .deviceType = OUT_BUILTIN_SPEAKER,
        .hifiMode = HIFI_LPM_CLK_LEVEL1,
    },
    {
        .streamType = AUDIO_STREAM_ALARM_CLOCK,
        .deviceType = OUT_BUILTIN_SPEAKER,
        .hifiMode = HIFI_LPM_CLK_LEVEL1,
    },
    {
        .streamType = AUDIO_STREAM_ALARM_CLOCK,
        .deviceType = OUT_BLUETOOTH_A2DP,
        .hifiMode = HIFI_LPM_CLK_LOW_LEVEL,
    },
    {
        .streamType = AUDIO_STREAM_RING,
        .deviceType = OUT_BLUETOOTH_A2DP,
        .hifiMode = HIFI_LPM_CLK_LOW_LEVEL,
    },
    {
        .streamType = AUDIO_STREAM_RING,
        .deviceType = OUT_BUILTIN_SPEAKER,
        .hifiMode = HIFI_LPM_CLK_LEVEL1,
    },
    {
        .streamType = AUDIO_STREAM_VOICE_CALL_VOIP,
        .deviceType = UNKNOWN,
        .hifiMode = HIFI_LPM_CLK_LEVEL2,
    },
    {
        .streamType = AUDIO_STREAM_VOICE_CALL_BT_SCO,
        .deviceType = UNKNOWN,
        .hifiMode = HIFI_LPM_CLK_LEVEL2,
    },
    {
        .streamType = AUDIO_STREAM_VOICE_CALL_VOLTE_SPI,
        .deviceType = OUT_BUILTIN_SPEAKER,
        .hifiMode = HIFI_LPM_CLK_LEVEL2,
    },
    {
        .streamType = AUDIO_STREAM_VOICE_CALL_VOLTE_SPI,
        .deviceType = IN_BUILTIN_MIC,
        .hifiMode = HIFI_LPM_CLK_LEVEL2,
    },
    {
        .streamType = AUDIO_STREAM_VOICE_CALL_VOLTE,
        .deviceType = OUT_MODEM,
        .hifiMode = HIFI_LPM_CLK_LEVEL2,
    },
    {
        .streamType = AUDIO_STREAM_VOICE_CALL_VOLTE,
        .deviceType = IN_MODEM,
        .hifiMode = HIFI_LPM_CLK_LEVEL2,
    },
    {
        .streamType = AUDIO_STREAM_VOICE_CALL_VOLTE,
        .deviceType = IN_MODEM_HEADSET,
        .hifiMode = HIFI_LPM_CLK_LEVEL1,
    },
    {
        .streamType = AUDIO_STREAM_VOICE_CALL_VOLTE,
        .deviceType = OUT_MODEM_HEADSET,
        .hifiMode = HIFI_LPM_CLK_LEVEL1,
    },
    {
        .streamType = AUDIO_STREAM_VOICE_CALL_VOLTE,
        .deviceType = OUT_BUILTIN_SPEAKER,
        .hifiMode = HIFI_LPM_CLK_LEVEL2,
    },
    {
        .streamType = AUDIO_STREAM_VOICE_CALL_VOLTE,
        .deviceType = IN_BUILTIN_MIC,
        .hifiMode = HIFI_LPM_CLK_LEVEL2,
    },
    {
        .streamType = AUDIO_STREAM_VOICE_ASSISTANT,
        .deviceType = OUT_BUILTIN_SPEAKER,
        .hifiMode = HIFI_LPM_CLK_LEVEL1,
    },
    {
        .streamType = AUDIO_STREAM_VOICE_ASSISTANT,
        .deviceType = OUT_BLUETOOTH_A2DP,
        .hifiMode = HIFI_LPM_CLK_LOW_LEVEL,
    },
    {
        .streamType = AUDIO_STREAM_TTS,
        .deviceType = OUT_BUILTIN_SPEAKER,
        .hifiMode = HIFI_LPM_CLK_LEVEL1,
    },
    {
        .streamType = AUDIO_STREAM_TTS,
        .deviceType = OUT_BLUETOOTH_A2DP,
        .hifiMode = HIFI_LPM_CLK_LOW_LEVEL,
    },
    {
        .streamType = AUDIO_STREAM_NOTIFICATION,
        .deviceType = OUT_BUILTIN_SPEAKER,
        .hifiMode = HIFI_LPM_CLK_LEVEL1,
    },
    {
        .streamType = AUDIO_STREAM_NOTIFICATION,
        .deviceType = OUT_BLUETOOTH_A2DP,
        .hifiMode = HIFI_LPM_CLK_LOW_LEVEL,
    },
    {
        .streamType = AUDIO_STREAM_NOTIFICATION_SYSTEM,
        .deviceType = OUT_BUILTIN_SPEAKER,
        .hifiMode = HIFI_LPM_CLK_LEVEL1,
    },
    {
        .streamType = AUDIO_STREAM_NOTIFICATION_SYSTEM,
        .deviceType = OUT_BLUETOOTH_A2DP,
        .hifiMode = HIFI_LPM_CLK_LOW_LEVEL,
    },
    {
        .streamType = AUDIO_STREAM_NOTIFICATION_PROMPT,
        .deviceType = OUT_BUILTIN_SPEAKER,
        .hifiMode = HIFI_LPM_CLK_LEVEL1,
    },
    {
        .streamType = AUDIO_STREAM_NOTIFICATION_PROMPT,
        .deviceType = OUT_BLUETOOTH_A2DP,
        .hifiMode = HIFI_LPM_CLK_LOW_LEVEL,
    },
    {
        .streamType = AUDIO_STREAM_MUSIC,
        .deviceType = OUT_BUILTIN_SPEAKER,
        .hifiMode = HIFI_LPM_CLK_LEVEL1,
    },
    {
        .streamType = AUDIO_STREAM_MUSIC,
        .deviceType = OUT_BLUETOOTH_A2DP,
        .hifiMode = HIFI_LPM_CLK_LOW_LEVEL,
    },
    {
        .streamType = AUDIO_STREAM_FITNESS_VIDEO,
        .deviceType = OUT_BUILTIN_SPEAKER,
        .hifiMode = HIFI_LPM_CLK_LEVEL1,
    },
    {
        .streamType = AUDIO_STREAM_VOICE_RECORD,
        .deviceType = IN_BUILTIN_MIC,
        .hifiMode = HIFI_LPM_CLK_LEVEL1,
    },
    {
        .streamType = AUDIO_STREAM_VOICE_ASSISTANT,
        .deviceType = IN_BUILTIN_MIC,
        .hifiMode = HIFI_LPM_CLK_LEVEL2,
    },
    {
        .streamType = AUDIO_STREAM_A2DP_MUSIC,
        .deviceType = OUT_BUILTIN_SPEAKER,
        .hifiMode = HIFI_LPM_CLK_LEVEL1,
    },
    {
        .streamType = AUDIO_STREAM_A2DP_MUSIC,
        .deviceType = IN_BLUETOOTH_A2DP,
        .hifiMode = HIFI_LPM_CLK_LEVEL1,
    },
};

static uintptr_t HifiClockFreqListDup(uintptr_t ptr)
{
    if (ptr == 0) {
        return 0;
    }
    HifiClockFreqContext *hifiClockFreqInfo = (HifiClockFreqContext *)malloc(sizeof(HifiClockFreqContext));
    if (hifiClockFreqInfo == NULL) {
        ALOGE("deviceInfo malloc failed");
        return 0;
    }
    if (memcpy_s(hifiClockFreqInfo, sizeof(HifiClockFreqContext), (HifiClockFreqContext *)ptr,
        sizeof(HifiClockFreqContext)) != EOK) {
        ALOGE("memcpy_s  deviceInfo failed");
        free(hifiClockFreqInfo);
        return 0;
    }
    return (uintptr_t)hifiClockFreqInfo;
}

static void HifiClockFreqListFree(uintptr_t ptr)
{
    if (ptr == 0) {
        return;
    }
    HifiClockFreqContext *deviceInfo = (HifiClockFreqContext *)ptr;
    free(deviceInfo);
}


static void HifiClockFreqListInit(List *hifiClockFreqListHead)
{
    DupFreeFuncPair dataFunc;
    dataFunc.dupFunc = HifiClockFreqListDup;
    dataFunc.freeFunc = HifiClockFreqListFree;
    ListInit(hifiClockFreqListHead, &dataFunc);
}

static int32_t HifiClockFreqListPushFront(List *hifiClockFreqListHead, const HifiClockFreqContext *hifiContext)
{
    uint32_t ret = ListPushFront(hifiClockFreqListHead, (uintptr_t)hifiContext);
    return ret == LIST_OK ? AUDIO_SUCCESS : AUDIO_ERROR;
}

static int32_t HifiClockFreqListErase(List *hifiClockFreqListHead, const HifiClockFreqContext *hifiContext)
{
    bool found = false;
    for (ListIterator it = ListIterBegin(hifiClockFreqListHead); it != ListIterEnd(hifiClockFreqListHead);) {
        HifiClockFreqContext *hifiClockInfoTmp = (HifiClockFreqContext *)ListIterData(it);
        if (hifiClockInfoTmp->streamType == hifiContext->streamType &&
            hifiClockInfoTmp->deviceType == hifiContext->deviceType) {
            it = ListIterErase(hifiClockFreqListHead, it);
            found = true;
            break;
        } else {
            it = ListIterNext(hifiClockFreqListHead, it);
        }
    }
    return found ? AUDIO_SUCCESS : AUDIO_ERROR;
}

static pm_hifi_mode_t HifiClockFreqListFindMaxFreq(const List *deviceListHead)
{
    pm_hifi_mode_t hifiModeMax = HIFI_LPM_CLK_LOW_LEVEL;
    bool found = false;
    for (ListIterator it = ListIterBegin(deviceListHead); it != ListIterEnd(deviceListHead);
        it = ListIterNext(deviceListHead, it)) {
        HifiClockFreqContext *hifiClockInfo = (HifiClockFreqContext *)ListIterData(it);
        if (hifiClockInfo->hifiMode != HIFI_LPM_CLK_MAX &&
            hifiClockInfo->hifiMode >= hifiModeMax) {
            hifiModeMax = hifiClockInfo->hifiMode;
            found = true;
        }
    }
    if (!found) {
        hifiModeMax = HIFI_LPM_CLK_MAX;
    }
    return hifiModeMax;
}

static bool HifiClockFreqListFind(const List *deviceListHead, const HifiClockFreqContext *hifiContext)
{
    for (ListIterator it = ListIterBegin(deviceListHead); it != ListIterEnd(deviceListHead);
        it = ListIterNext(deviceListHead, it)) {
        HifiClockFreqContext *hifiClockInfo = (HifiClockFreqContext *)ListIterData(it);
        if (hifiClockInfo->streamType == hifiContext->streamType &&
            hifiClockInfo->deviceType == hifiContext->deviceType) {
            return true;
        }
    }
    return false;
}

void HifiClockFreqMgrInit(void)
{
    if (g_hifiClockFreqMutex == NULL) {
        g_hifiClockFreqMutex = MediaMutexCreate(NULL);
        if (g_hifiClockFreqMutex == NULL) {
            ALOGE("create hifi Clock Freq Mutex failed");
            return;
        }
    }
    MediaMutexLock(g_hifiClockFreqMutex);
    HifiClockFreqListInit(&g_availableHifiClockFreqContext);
    MediaMutexUnLock(g_hifiClockFreqMutex);
}

void HifiClockFreqMgrDeInit(void)
{
    MediaMutexLock(g_hifiClockFreqMutex);
    ListDeinit(&g_availableHifiClockFreqContext);
    g_hifiMode = HIFI_LPM_CLK_MAX;
    MediaMutexUnLock(g_hifiClockFreqMutex);
    MediaMutexDestroy(&g_hifiClockFreqMutex);
    g_hifiClockFreqMutex = NULL;
}

static int32_t GetSupportFreqIndexByStreamTypeAndDeviceType(AudioStreamType streamType, AudioDeviceType deviceType)
{
    int32_t contextNum = (int32_t)(sizeof(g_hifiClockFreqList) / sizeof(HifiClockFreqContext));
    int32_t index = -1;
    for (int32_t i = 0; i < contextNum; i++) {
        HifiClockFreqContext context = g_hifiClockFreqList[i];
        if (context.streamType != streamType) {
            continue;
        }
        if (context.deviceType == UNKNOWN ||
            context.deviceType == deviceType) {
            index = i;
            break;
        }
    }
    return index;
}

static void SetHifiClockFreqValue(void)
{
    pm_hifi_mode_t hifiMode = HifiClockFreqListFindMaxFreq(&g_availableHifiClockFreqContext);
    if (g_hifiMode != hifiMode && hifiMode != HIFI_LPM_CLK_MAX) {
        ALOGI("current hifi mode:%d, set hifi mode:%d", g_hifiMode, hifiMode);
        uapi_set_hifi_mode(hifiMode);
        g_hifiMode = hifiMode;
    }
}

int32_t SetHifiClockFreqByStreamTypeAndDeviceType(AudioStreamType streamType,
    AudioDeviceType deviceType, bool isNeedPush)
{
    MediaMutexLock(g_hifiClockFreqMutex);
    int32_t index = GetSupportFreqIndexByStreamTypeAndDeviceType(streamType, deviceType);
    if (index < 0) {
        ALOGE("can not find the support context by streamType:%x!", streamType);
        MediaMutexUnLock(g_hifiClockFreqMutex);
        return AUDIO_ERROR;
    }
    int32_t ret = 0;
    if (isNeedPush) {
        if (HifiClockFreqListFind(&g_availableHifiClockFreqContext, &g_hifiClockFreqList[index])) {
            ALOGI("the current streamType[%x] already exists and does not need to be recorded!",
                g_hifiClockFreqList[index].streamType);
            MediaMutexUnLock(g_hifiClockFreqMutex);
            return AUDIO_SUCCESS;
        }
        ret = HifiClockFreqListPushFront(&g_availableHifiClockFreqContext, &g_hifiClockFreqList[index]);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("HifiClockFreqList Push Front failed, ret:%x", ret);
            MediaMutexUnLock(g_hifiClockFreqMutex);
            return AUDIO_ERROR;
        }
    } else {
        if (!HifiClockFreqListFind(&g_availableHifiClockFreqContext, &g_hifiClockFreqList[index])) {
            ALOGI("the current streamType[%x] does not exist and does not need to be deleted!",
                g_hifiClockFreqList[index].streamType);
            MediaMutexUnLock(g_hifiClockFreqMutex);
            return AUDIO_SUCCESS;
        }
        ret = HifiClockFreqListErase(&g_availableHifiClockFreqContext, &g_hifiClockFreqList[index]);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("HifiClockFreqList Erase failed, ret:%x", ret);
            MediaMutexUnLock(g_hifiClockFreqMutex);
            return AUDIO_ERROR;
        }
    }
    SetHifiClockFreqValue();
    MediaMutexUnLock(g_hifiClockFreqMutex);
    return AUDIO_SUCCESS;
}