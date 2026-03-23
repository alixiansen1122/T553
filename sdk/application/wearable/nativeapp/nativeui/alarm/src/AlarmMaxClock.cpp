/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Alarm
 * Author:
 * Create: 2021-09-11
 */

#include "alarm/AlarmMaxClock.h"
#include "UiConfig.h"
#include <string>
#include "ohos_timer.h"
#include "graphic_timer.h"

#include "alarm/AlarmPresenter.h"
#include "alarm/MainAlarmView.h"

namespace OHOS {
#define MAX_INETERFACES "当前闹钟个数已达上限"
#define INTERFACE_ID "maxclock"

GraphicTimer* g_stopAlarmMaxHandle = nullptr;
static AlarmMaxClock *g_AlarmMaxClock = nullptr;
static constexpr uint64_t MS_THREE_SECONDS = 3000;
void ScanTimerCallback(void* data);
void EndScanHandle(void);

static AlarmMaxClock *g_JudgeView = nullptr;
constexpr int16_t CONTAINER_X = 0;
constexpr int16_t CONTAINER_Y = 0;
constexpr int16_t MAX_LABEL_X_LEN = 80;
constexpr int16_t MAX_LABEL_Y_LEN = 180;
constexpr int16_t MAX_LABEL_LENGTH_LEN = 400;
constexpr int16_t MAX_LABEL_WIDTH_LEN = 50;
constexpr int16_t MAX_LABEL_FONT_LEN = 30;
constexpr int16_t MAX_IMAGE_DELETE_X_LEN = 90;
constexpr int16_t MAX_IMAGE_DELETE_Y_LEN = 300;
constexpr int16_t MAX_IMAGE_TRUE_X_LEN = 270;
constexpr int16_t MAX_IMAGE_TRUE_Y_LEN = 300;
constexpr int16_t MAX_IMAGE_DELETE_WIDTH_LEN = 150;
constexpr int16_t MAX_IMAGE_TRUE_WIDTH_LEN = 150;

AlarmMaxClock::AlarmMaxClock()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmMaxClock::AlarmMaxClock");
    g_AlarmMaxClock = this;
}

AlarmMaxClock::~AlarmMaxClock()
{
    container->RemoveAll();
    delete labelText;
    labelText = nullptr;
    delete container;
    container = nullptr;
    g_AlarmMaxClock = nullptr;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmMaxClock::~AlarmMaxClock");
}

AlarmMaxClock *AlarmMaxClock::GetInstance(void)
{
    return g_AlarmMaxClock;
}

UIScrollView *AlarmMaxClock::InitAlarmMax(void)
{
    container = new UIScrollView();
    if (container == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmMaxClock:: new container fail");
        return container;
    }
    container->SetPosition(CONTAINER_X, CONTAINER_Y);
    container->SetWidth(HORIZONTAL_RESOLUTION);
    container->SetHeight(VERTICAL_RESOLUTION);

    labelText = new UILabel();
    if (labelText == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmMaxClock:: new labelText fail");
        return container;
    }
    labelText->SetPosition(MAX_LABEL_X_LEN, MAX_LABEL_Y_LEN, MAX_LABEL_LENGTH_LEN, MAX_LABEL_WIDTH_LEN);
    labelText->SetText(MAX_INETERFACES);
    labelText->SetFont(DEFAULT_VECTOR_FONT_FILENAME, MAX_LABEL_FONT_LEN);

    container->SetTouchable(true);
    container->SetDraggable(true);
    UIView::OnDragListener *maxClockDragListener = (UIView::OnDragListener *)OHOS::AlarmPresenter::GetInstance();
    if (maxClockDragListener == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmMaxClock:: maxClockDragListener is nullptr");
        return container;
    }
    container->SetOnDragListener(maxClockDragListener);
    container->Add(labelText);

    return container;
}

void CancelScanHandle(void)
{
    if (g_stopAlarmMaxHandle != nullptr) {
		delete g_stopAlarmMaxHandle;

        g_stopAlarmMaxHandle = nullptr;
    }
}

void EndScanHandle(void)
{
    if (OHOS::MainAlarmView::GetInstance() == nullptr) {
        return;
    }
    OHOS::MainAlarmView::GetInstance()->ChangeView(CHANGE_INTERFACE_MAXCLOCK, MAIN_INTERFACE);
}

static void ScanTimerCallbackProc(void* data)
{
    (void)data;
    CancelScanHandle();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmMax scan end jumpView");
    EndScanHandle();
}

void ScanTimerCallback(void* data)
{
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(ScanTimerCallbackProc, data));
}

void AlarmMaxClock::AlarmMaxDelayExit(void)
{
    uint32 tick = GetOSTick(MS_THREE_SECONDS);
    g_stopAlarmMaxHandle = new GraphicTimer(tick, ScanTimerCallback, nullptr, true);
    if (g_stopAlarmMaxHandle == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmMaxClock::AlarmMaxDelayExit new GraphicTime failed!!");
        return;
    }
    bool retTimer = g_stopAlarmMaxHandle->Start();
    if (!retTimer) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmMaxClock::AlarmMaxDelayExit Start failed!!");
        if (g_stopAlarmMaxHandle != nullptr) {
            delete g_stopAlarmMaxHandle;

            g_stopAlarmMaxHandle = nullptr;
        }
        return;
    }
}
}