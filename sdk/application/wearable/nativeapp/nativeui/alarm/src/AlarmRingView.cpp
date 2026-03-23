/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Alarm
 * Author:
 * Create: 2021-09-11
 */

#include "alarm/AlarmRingView.h"
#include "common/screen.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include <string>
#include "alarm/AlarmPopUpPresenter.h"
#include "ohos_timer.h"
#include "graphic_timer.h"
#include "phoneservice/PhoneService.h"

namespace OHOS {
#define RING_AGAIN "响铃将在十分钟后再次响起"

GraphicTimer* g_stopAlarmRingHandle = nullptr;

static AlarmRingView *g_pAlarmRingView = nullptr;
constexpr int16_t SET_ZERO = 0;
constexpr int16_t SET_SRC = 150;
constexpr int16_t IMAGE_X = 150;
constexpr int16_t IMAGE_Y = 140;
constexpr int16_t LABEL_X = 80;
constexpr int16_t LABEL_Y = 300;
constexpr int16_t LABEL_LENGTH = 350;
constexpr int16_t LABEL_HEIGHT = 35;
constexpr int16_t LABEL_FONT = 25;

static constexpr uint64_t RING_THREE_SECONDS = 3000;
void RingTimerCallbacks(void* data);

AlarmRingView::AlarmRingView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmRingView::AlarmRingView");
    g_pAlarmRingView = this;
}

AlarmRingView::~AlarmRingView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmRingView::~AlarmRingView");
    group->RemoveAll();
    delete imageRingClock;
    imageRingClock = nullptr;
    delete labelRing;
    labelRing = nullptr;
    delete group;
    group = nullptr;
    g_pAlarmRingView = nullptr;
}

AlarmRingView *AlarmRingView::GetInstance(void)
{
    return g_pAlarmRingView;
}

UIViewGroup *AlarmRingView::InitRingView(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmRingView::InitRingView");
    group = new UIViewGroup();
    if (group == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmRingView::group null!");
        return nullptr;
    }
    group->SetPosition(SET_ZERO, SET_ZERO);
    group->SetWidth(HORIZONTAL_RESOLUTION);
    group->SetHeight(VERTICAL_RESOLUTION);

    imageRingClock = new UIImageView();
    if (imageRingClock == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmRingView::imageRingClock null!");
        return nullptr;
    }
    imageRingClock->SetPosition(IMAGE_X, IMAGE_Y);
    imageRingClock->SetWidth(SET_SRC);
    imageRingClock->SetHeight(SET_SRC);
    imageRingClock->SetSrc(ALARM_CLOCK_POP_ADD_PATH);
    group->Add(imageRingClock);

    labelRing = new UILabel();
    if (labelRing == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmRingView::labelRing null!");
        return nullptr;
    }
    labelRing->SetPosition(LABEL_X, LABEL_Y, LABEL_LENGTH, LABEL_HEIGHT);
    labelRing->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT);
    labelRing->SetText(RING_AGAIN);
    group->Add(labelRing);
    UIView::OnDragListener *listener = static_cast<UIView::OnDragListener*>(AlarmPopUpPresenter::GetInstance());
    group->SetOnDragListener(listener);
    group->SetDraggable(true);
    group->SetTouchable(true);
    return group;
}

void AlarmRingView::CancelTimerHandle(void)
{
    if (g_stopAlarmRingHandle != nullptr) {
        delete g_stopAlarmRingHandle;
        g_stopAlarmRingHandle = nullptr;
    }
}

static void RingTimerCallbacksProc(void* data)
{
    (void)data;
    AlarmRingView::GetInstance()->CancelTimerHandle();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmRing scan end jumpView");
    uint16_t preSlice = AlarmClockModel::GetInstance()->JudgePreSlice();
    NativeAbility::GetInstance().ChangeSlice(preSlice);
}

void RingTimerCallbacks(void* data)
{
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(RingTimerCallbacksProc, data));
}

void AlarmRingView::AlarmRingDelayShowtimer(void)
{
    uint32 tick = GetOSTick(RING_THREE_SECONDS);
    g_stopAlarmRingHandle = new GraphicTimer(tick, RingTimerCallbacks, nullptr, true);
    if (g_stopAlarmRingHandle == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmMaxClock::AlarmRingDelayShowtimer new GraphicTimer failed!!");
        return;
    }
    bool retTimer = g_stopAlarmRingHandle->Start();
    if (!retTimer) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmMaxClock::AlarmRingDelayShowtimer Start failed!!");
        if (g_stopAlarmRingHandle != nullptr) {
            delete g_stopAlarmRingHandle;
            g_stopAlarmRingHandle = nullptr;
        }
        return;
    }
}
} // namespace OHOS
