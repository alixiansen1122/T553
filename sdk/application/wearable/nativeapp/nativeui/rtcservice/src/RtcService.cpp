/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: RtcService
 * Create: 2021-09-22
 */

#include <cstdio>
#include <ctime>
#include "memory_adapter.h"
#include "ohos_init.h"
#include "samgr_lite.h"
#include "service_id_define.h"
#include "wearable_log.h"
#include "securec.h"
#include "broadcast_service.h"

#include "alarmclockmanage/AlmmgrAlarmClockManage.h"
#include "rtcservice/RtcService.h"

#ifdef __cplusplus
extern "C" {
#endif

static const char *GetServiceName(Service *service);
static BOOL ServiceInitialize(Service *service, Identity identity);
static BOOL ServiceMessageHandle(Service *service, Request *request);
static TaskConfig GetServiceTaskConfig(Service *service);
static void SubscribeTopic(Topic topic);
void UnsubscribeTopic(Topic topic);
static void Notify(void *args, const Topic topic, Request* req);

static void Init();

static RtcService g_rtcService = {
    .GetName = GetServiceName,
    .Initialize = ServiceInitialize,
    .MessageHandle = ServiceMessageHandle,
    .GetTaskConfig = GetServiceTaskConfig,
    .subscirber = {
        .identity = nullptr,
        .args = &g_rtcService,
        .Notify = Notify,
    },
};

const int STACK_SIZE = 0xC00;
const int QUEUE_SIZE = 10;

static void Init()
{
    SamgrLite *sm = SAMGR_GetInstance();
    BOOL result = sm->RegisterService((Service *)&g_rtcService);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "RtcService starts %s!!", result ? "successfully" : "unsuccessfully");
    (void)result;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Init: SubscribeTopic!!");
    AlmmgrInitManagement();
    SubscribeTopic(TOPIC_EVENT_ALARM_RING);
    SubscribeTopic(TOPIC_EVENT_UTCTIME_UPDATE);
    SubscribeTopic(TOPIC_EVENT_NEXTDAY_UPDATE);
    return;
}
SYSEX_SERVICE_INIT(Init);

static const char *GetServiceName(Service *service)
{
    (void)service;
    return RTC_SERVICE;
}

static BOOL ServiceInitialize(Service *service, Identity identity)
{
    (void)service;
    (void)identity;

    return TRUE;
}

static BOOL ServiceMessageHandle(Service *service, Request *request)
{
    (void)service;
    if (request == nullptr) {
        return FALSE;
    }
    return TRUE;
}

static TaskConfig GetServiceTaskConfig(Service *service)
{
    (void)service;
    TaskConfig config = {LEVEL_HIGH, PRI_NORMAL, STACK_SIZE, QUEUE_SIZE, SINGLE_TASK};
    return config;
}

static void SubscribeTopic(Topic topic)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Rtc Subscribe Topic=%u!!", topic);
    BroadcastApi *broadcastApi = nullptr;

    IUnknown *api = SAMGR_GetInstance()->GetFeatureApi(BROADCAST_SERVICE_NAME, BROADCAST_FEATURE_NAME);
    if (api == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SubscribeTopic GetFeatureApi %s %s failed!!",
                      BROADCAST_SERVICE_NAME, BROADCAST_FEATURE_NAME);
        return;
    }
    int result = api->QueryInterface(api, DEFAULT_VERSION, (void **)&broadcastApi);
    if (result != 0 || broadcastApi == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SubscribeTopic QueryInterface %s %s failed!!",
                      BROADCAST_SERVICE_NAME, BROADCAST_FEATURE_NAME);
        return;
    }

    broadcastApi->SubscribeTopic((IUnknown *)broadcastApi, topic, &g_rtcService.subscirber);
    broadcastApi->Release((IUnknown *)broadcastApi);
    return;
}

void UnsubscribeTopic(Topic topic)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Rtc Unsubscribe Topic=%u!!", topic);
    BroadcastApi *broadcastApi = nullptr;

    IUnknown *api = SAMGR_GetInstance()->GetFeatureApi(BROADCAST_SERVICE_NAME, BROADCAST_FEATURE_NAME);
    if (api == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "UnsubscribeTopic GetFeatureApi %s %s failed!!",
                      BROADCAST_SERVICE_NAME, BROADCAST_FEATURE_NAME);
        return;
    }
    int result = api->QueryInterface(api, DEFAULT_VERSION, (void **)&broadcastApi);
    if (result != 0 || broadcastApi == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "UnsubscribeTopic QueryInterface %s %s failed!!",
                      BROADCAST_SERVICE_NAME, BROADCAST_FEATURE_NAME);
        return;
    }

    broadcastApi->UnsubscribeTopic((IUnknown *)broadcastApi, topic, &g_rtcService.subscirber);
    broadcastApi->Release((IUnknown *)broadcastApi);
    return;
}

static void Notify(void *args, const Topic topic, Request* req)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Rtc Notify:");
    AlarmClockQueueMsg eventMsg;

    if (req == nullptr || args == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Notify para invalid!!");
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "args=0x%p, topic=%d", args, topic);

    (void)memset_s(&eventMsg, sizeof(AlarmClockQueueMsg), 0, sizeof(AlarmClockQueueMsg));
    if (topic == TOPIC_EVENT_ALARM_RING) {
        eventMsg.eventType = EVENT_ALARM_RING;
        AlmmgrAlarmClockEventProcess(&eventMsg);
    } else if (topic == TOPIC_EVENT_NEXTDAY_UPDATE) {
        UpdataListAndFirstAlarmToday();
    } else if (topic == TOPIC_EVENT_UTCTIME_UPDATE) {
        UpdataListAndFirstAlarmToday();
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "topic invalid!!");
    }

    return;
}

#ifdef __cplusplus
}
#endif