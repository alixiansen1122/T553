/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: AlarmClockTimer
 * Create: 2021-09-22
 */

#include <ctime>
#include <sys/time.h>
#include <cstdio>
#include "wearable_log.h"
#include "securec.h"
#include "broadcast_service.h"
#include "broadcast_feature.h"
#include "service_id_define.h"
#include "samgr_lite.h"
#include "thread_adapter.h"
#include "graphic_service.h"

#include "alarmclocktimerconfig/AlarmClockTimerConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

static AlarmCtrl g_alarmCtrA;
static AlarmCtrl g_alarmCtrB;
namespace OHOS {
GraphicTimer* g_AlarmHandle = nullptr;
}

void AlarmTimerEventPublish(uint16 topic, AlarmClockEventData data)
{
    int32 ret;
    uint32 size = sizeof(AlarmClockEventData);
    BroadcastApi *broadcastApi = nullptr;
    IUnknown *api = nullptr;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmTimerEventPublish topic=%u!!", topic);

    void *publishData = malloc(size);
    if (publishData == nullptr) {
        return;
    }
    ret = memcpy_s(publishData, size, (const void *)&data, size);
    if (ret != EOK) {
        free(publishData);
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmTimerEventPublish memcpy_s fail! ret = %d", ret);
        return;
    }

    Request request = {
        .msgId = topic,
        .len = size,
        .msgValue = 0,
        .data = publishData,
    };

    /* public request */
    api = SAMGR_GetInstance()->GetFeatureApi(BROADCAST_SERVICE_NAME, BROADCAST_FEATURE_NAME);
    if (api == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "GetFeatureApi failed");
        free(publishData);
        return;
    }
    ret = api->QueryInterface(api, DEFAULT_VERSION, (void **)&broadcastApi);
    if (ret != 0 || broadcastApi == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "QueryInterface failed");
        free(publishData);
        return;
    }
    broadcastApi->PublishTopic((IUnknown *)broadcastApi, &request);
    broadcastApi->Release((IUnknown *)broadcastApi);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmTimerEventPublish end!!");

    return;
}

static void AlarmCallBackProcA(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmCallBackProcA");
    AlarmClockEventData unuse;
    AlarmTimerEventPublish(TOPIC_EVENT_ALARM_RING, unuse);
}

static void AlarmCallBackProcB(void)
{
    AlarmClockEventData unuse;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmCallBackProcB");
    AlarmTimerEventPublish(TOPIC_EVENT_NEXTDAY_UPDATE, unuse);
    SetZeroAlarm();
}

void AlarmScan(void *arg)
{
    struct tm curTime;
    int32_t ret = AlarmGetCurrentTimeInfo(&curTime);
    if (ret != ERRCODE_SUCC) {
        return;
    }

    if (g_alarmCtrA.state == ALARM_STATUS_RUNNING) {
        if ((g_alarmCtrA.callback != nullptr) &&
            (g_alarmCtrA.info.hour == curTime.tm_hour) &&
            (g_alarmCtrA.info.minute == curTime.tm_min)) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmA Called %d %d", curTime.tm_hour, curTime.tm_min);
            g_alarmCtrA.state = ALARM_STATUS_CREATED;
            g_alarmCtrA.callback();
        }
    }

    if (g_alarmCtrB.state == ALARM_STATUS_RUNNING) {
        if ((g_alarmCtrB.callback != nullptr) &&
            (g_alarmCtrB.info.week == curTime.tm_wday) &&
            (g_alarmCtrB.info.hour == curTime.tm_hour) &&
            (g_alarmCtrB.info.minute == curTime.tm_min)) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmB Called %d %d", curTime.tm_hour, curTime.tm_min);
            g_alarmCtrB.state = ALARM_STATUS_CREATED;
            g_alarmCtrB.callback();
        }
    }
}

static int32_t AlarmCreate(AlarmCtrl *alarmCtrl, AlarmCallBack callback)
{
    if (alarmCtrl == nullptr) {
        return ERRCODE_FAIL;
    }
    if (alarmCtrl->state == ALARM_STATUS_UNUSED) {
        alarmCtrl->state = ALARM_STATUS_CREATED;
        alarmCtrl->callback = callback;
    }
    return ERRCODE_SUCC;
}

int32_t AlarmTimerInit(void)
{
    int32_t ret;

    g_alarmCtrA.state = ALARM_STATUS_UNUSED;
    g_alarmCtrB.state = ALARM_STATUS_UNUSED;
    OHOS::g_AlarmHandle = new OHOS::GraphicTimer(ALARM_INTERVAL_MS, AlarmScan, nullptr, true);
    bool timerStatus = OHOS::g_AlarmHandle->Start();
    if (!timerStatus) {
        return ERRCODE_FAIL;
    }
    ret = AlarmCreate(&g_alarmCtrA, AlarmCallBackProcA);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    ret = AlarmCreate(&g_alarmCtrB, AlarmCallBackProcB);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

int32_t SetAlarm(AlarmType type, uint8_t week, uint8_t hour, uint8_t minute)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SetAlarm %d %d %d %d", type, week, hour, minute);
    if (type == ALARM_A) {
        g_alarmCtrA.state = ALARM_STATUS_RUNNING;
        g_alarmCtrA.info.hour = hour;
        g_alarmCtrA.info.minute = minute;
    } else if (type == ALARM_B) {
        g_alarmCtrB.state = ALARM_STATUS_RUNNING;
        g_alarmCtrB.info.week = week;
        g_alarmCtrB.info.hour = hour;
        g_alarmCtrB.info.minute = minute;
    } else {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

int32_t SetZeroAlarm(void)
{
    struct tm curTime;
    int32_t ret = AlarmGetCurrentTimeInfo(&curTime);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    int32_t weekday = (++curTime.tm_wday) % 7;
    return SetAlarm(ALARM_B, weekday, 0, 0);
}

int32_t AlarmGetCurrentTimeInfo(struct tm *timeInfo)
{
    if (timeInfo == nullptr) {
        return ERRCODE_FAIL;
    }
    time_t sec = time(nullptr);
    localtime_r(&sec, timeInfo);
    return ERRCODE_SUCC;
}

#ifdef __cplusplus
}
#endif
