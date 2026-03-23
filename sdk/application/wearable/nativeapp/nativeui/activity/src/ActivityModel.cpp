/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: ActivityModel view
 * Author:
 * Create: 2022-03-02
 */

#include "activity/ActivityModel.h"
#include <fstream>
#include "iostream"
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include "wearable_log.h"
#include "ohos_timer.h"
#include "graphic_timer.h"
#include "securec.h"
#include "broadcast_service.h"
#include "clock/MainClockView.h"
#include "activity/ActivityWeekView.h"
#include "graphic_service.h"

#ifdef __cplusplus
extern "C" {
#endif

using namespace OHOS;

static void Notify(void *args, const Topic topic, Request* request);

Subscriber subscirber = {
    .identity = nullptr,
    .args = nullptr,
    .Notify = Notify,
};

void StepSubscribeTopic(Topic topic)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "StepSubscribeTopic, topic=%u", topic);
    BroadcastApi *broadcastApi = nullptr;
    IUnknown *api = SAMGR_GetInstance()->GetFeatureApi(BROADCAST_SERVICE_NAME, BROADCAST_FEATURE_NAME);
    if (api == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "StepSubscribeTopic, GetFeatureApi failed");
        return;
    }
    int result = api->QueryInterface(api, DEFAULT_VERSION, (void **)&broadcastApi);
    if (result != 0 || broadcastApi == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "StepSubscribeTopic, QueryInterface failed");
        return;
    }
    broadcastApi->SubscribeTopic((IUnknown *)broadcastApi, topic, &subscirber);
    broadcastApi->Release((IUnknown *)broadcastApi);
}

void StepUnsubscribeTopic(Topic topic)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "StepUnsubscribeTopic, topic=%u", topic);
    BroadcastApi *broadcastApi = nullptr;
    IUnknown *api = SAMGR_GetInstance()->GetFeatureApi(BROADCAST_SERVICE_NAME, BROADCAST_FEATURE_NAME);
    if (api == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "StepUnsubscribeTopic, GetFeatureApi failed");
        return;
    }
    int result = api->QueryInterface(api, DEFAULT_VERSION, (void **)&broadcastApi);
    if (result != 0 || broadcastApi == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "StepUnsubscribeTopic, QueryInterface failed");
        return;
    }
    broadcastApi->UnsubscribeTopic((IUnknown *)broadcastApi, topic, &subscirber);
    broadcastApi->Release((IUnknown *)broadcastApi);
}

static void RefreshStepControls(const uint32_t &value, const uint16 &msgId)
{
    MainClockView *mainClockView = OHOS::MainClockView::GetInstance();
    if (mainClockView != nullptr) {
        GraphicService::GetInstance()->PostGraphicEvent([value]() {
            if (OHOS::MainClockView::GetInstance() != nullptr) {
                OHOS::MainClockView::GetInstance()->RefreshStep(value);
            }
        });
    }

    ActivityWeekView *activityWeekView = OHOS::ActivityWeekView::GetInstance();
    if (activityWeekView == nullptr) {
        return;
    }
    GraphicService::GetInstance()->PostGraphicEvent([value]() {
        if (OHOS::ActivityWeekView::GetInstance() != nullptr) {
            OHOS::ActivityWeekView::GetInstance()->RefreshSteps(value);
        }
    });

    if (msgId == TOPIC_EVENT_STEPDATA_UPDATE) {
        GraphicService::GetInstance()->PostGraphicEvent([value]() {
            if (OHOS::ActivityWeekView::GetInstance() != nullptr) {
                OHOS::ActivityWeekView::GetInstance()->RefreshTodayView(value);
            }
        });
    } else if (msgId == TOPIC_EVENT_NEXTDAY_UPDATE) {
        GraphicService::GetInstance()->PostGraphicEvent([]() {
            if (OHOS::ActivityWeekView::GetInstance() != nullptr) {
                OHOS::ActivityWeekView::GetInstance()->InitAvgStep();
            }
        });
        if (OHOS::ActivityModel::GetInstance()->GetWeekToday() == MONDAY_STEP_COUNTER) {
            GraphicService::GetInstance()->PostGraphicEvent([]() {
                if (OHOS::ActivityWeekView::GetInstance() != nullptr) {
                    OHOS::ActivityWeekView::GetInstance()->InitWeekView();
                }
            });
        }
    }
}

static void Notify(void *args, const Topic topic, Request* request)
{
    UNUSED(args);
    UNUSED(topic);
    if (request == nullptr) {
        return;
    }

    if (request->msgId == TOPIC_EVENT_STEPDATA_UPDATE) {
        if (request->data == nullptr) {
            return;
        }
        // 计步数据累加
        uint32 counter = OHOS::ActivityModel::GetInstance()->GetStepCounter();
        counter += *(uint32_t *)request->data;
        OHOS::ActivityModel::GetInstance()->ReviseStepCounter(counter);

        GraphicService::GetInstance()->PostGraphicEvent([counter]() {
            if (ActivityModel::GetInstance() != nullptr) {
                ActivityModel::GetInstance()->SetStepData(counter);
            }
        });
        RefreshStepControls(counter, TOPIC_EVENT_STEPDATA_UPDATE);
    }

    if (request->msgId == TOPIC_EVENT_NEXTDAY_UPDATE) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "STEP:TOPIC_EVENT_NEXTDAY_UPDATE");
        uint32_t refreshValue = OHOS::ActivityModel::GetInstance()->ZeroHourStepHandle();
        RefreshStepControls(refreshValue, TOPIC_EVENT_NEXTDAY_UPDATE);
    }
}

#ifdef __cplusplus
}
#endif

namespace OHOS {
StepAllData g_stepData1 = {10050, 13421, 12312, 4252, 8993, 12314, 3423};

static constexpr const char *STEP_DATA_PATH = "/user/data/step_";
static constexpr uint32_t SECONDS_ONEDAY = 86400;
static constexpr uint16_t ONT_MIN_STEP_CONTER = 120;

GraphicTimer* g_saveEveryMinHandle = nullptr;
void SaveEveryMinCallback(void* data);

ActivityModel::ActivityModel()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ActivityModel::ActivityModel");
}

ActivityModel::~ActivityModel()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ActivityModel::~ActivityModel");
    StepUnsubscribeTopic(TOPIC_EVENT_STEPDATA_UPDATE);
    StepUnsubscribeTopic(TOPIC_EVENT_NEXTDAY_UPDATE);
    if (g_saveEveryMinHandle != nullptr) {
        delete g_saveEveryMinHandle;
        g_saveEveryMinHandle = nullptr;
    }
}

ActivityModel *ActivityModel::GetInstance(void)
{
    static ActivityModel instance;
    return &instance;
}

void ActivityModel::Init()
{
    StepSubscribeTopic(TOPIC_EVENT_STEPDATA_UPDATE);
    StepSubscribeTopic(TOPIC_EVENT_NEXTDAY_UPDATE);
    InitStepData();
}

uint32 ActivityModel::GetStepCounter(void)
{
    return stepCounter;
}

void ActivityModel::ReviseStepCounter(uint32 &counter)
{
    stepCounter = counter;
}

void ActivityModel::SaveStepToFile(struct tm *tmData)
{
}

void ActivityModel::GetStepData(const StepDataDistribute &type, uint32_t *value)
{
    *value = g_stepData1.weekSteps[type];
    return;
}

static void SaveEveryMinCallbackProc(void* data)
{
    UNUSED(data);
    int today = ActivityModel::GetInstance()->GetWeekToday();
    uint32 savedValue = ActivityModel::GetInstance()->GetLastSavedValue();
    if (savedValue != g_stepData1.weekSteps[today]) {
        struct tm curTime;
        time_t second = time(nullptr);
        (void)gmtime_r((const time_t *)&second, &curTime);
        ActivityModel::GetInstance()->SaveStepToFile(&curTime);
    }
}

void SaveEveryMinCallback(void* data)
{
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(SaveEveryMinCallbackProc, data));
}

void ActivityModel::SetStepData(const uint32_t &value)
{
    g_stepData1.weekSteps[weekToday] = value;
    // 第一次上报数据后打开定时器
    if (g_saveEveryMinHandle != nullptr) {
        return;
    }
    uint32 tick = GetOSTick(60000); // 60000: seconds of a minute
    g_saveEveryMinHandle = new GraphicTimer(tick, SaveEveryMinCallback, nullptr, true);
    if (g_saveEveryMinHandle == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ActivityModel::SetStepData new GraphicTimer failed!!");
        return;
    }
    bool retTimer = g_saveEveryMinHandle->Start();
    if (!retTimer) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ActivityModel::SetStepData Start failed!!");
        if (g_saveEveryMinHandle != nullptr) {
            delete g_saveEveryMinHandle;
            g_saveEveryMinHandle = nullptr;
        }
        return;
    }
    return;
}

int ActivityModel::GetWeekToday(void)
{
    return weekToday;
}

uint32 ActivityModel::GetLastSavedValue(void)
{
    return lastSavedValue;
}

bool ActivityModel::InitStepData(void)
{
    stepCounter = g_stepData1.weekSteps[weekToday];
    lastSavedValue = stepCounter;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "TodayStep: %d", g_stepData1.weekSteps[weekToday]);
    return true;
}

uint32_t ActivityModel::ZeroHourStepHandle(void)
{
    g_stepData1.weekSteps[weekToday] = stepCounter;
    struct tm historyTime;
    time_t second = time(nullptr) - SECONDS_ONEDAY;
    (void)gmtime_r((const time_t *)&second, &historyTime);
    SaveStepToFile(&historyTime);
    stepCounter = 0;
    if (weekToday == SUNDAY_STEP_COUNTER) {
        weekToday = MONDAY_STEP_COUNTER;
        (void)memset_s(&g_stepData1.weekSteps, sizeof(g_stepData1.weekSteps), 0, sizeof(g_stepData1.weekSteps));
    } else {
        ++weekToday;
    }
    return stepCounter;
}
}
