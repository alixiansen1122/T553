/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Alarm
 * Author:
 * Create: 2021-09-11
 */


#include "alarm/AlarmPopUpPresenter.h"
#include "alarm/MainAlarmView.h"
#include "ChangeSliceListener.h"
#include "phoneservice/PhoneService.h"
#include "player/PlayersModel.h"
#include "NativeRegisterManager.h"

namespace OHOS {

REGIST_SLICE(VIEW_ALARM_POP_UP, AlarmPopUpView, AlarmPopUpPresenter);

static AlarmPopUpPresenter *g_pAlarmPresenter = nullptr;

constexpr int16_t SET_ZERO = 0;
constexpr int16_t SET_ONE = 0;

static constexpr uint16_t RENDER_FINISH = 2;

AlarmPopUpPresenter::AlarmPopUpPresenter()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPopUpPresenter new");
    g_pAlarmPresenter = this;
}

AlarmPopUpPresenter::~AlarmPopUpPresenter()
{
    g_pAlarmPresenter = nullptr;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPopUpPresenter::~AlarmPopUpPresenter");
}

AlarmPopUpPresenter *AlarmPopUpPresenter::GetInstance()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter GetInstance");
    return g_pAlarmPresenter;
}

void AlarmPopUpPresenter::CancelAlarmRing()
{
    uint8_t alarmId = 0;
    bool alarmStatus;
    AlarmClockModel::GetInstance()->PopInterCancelMusic();
    alarmStatus = AlarmClockModel::GetInstance()->GetListenClockId(&alarmId);
    if (!alarmStatus) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPopUpPresenter::CancelAlarmRing get alarm Id false!");
        return;
    }
    AlarmClockModel::GetInstance()->ResetRingClockStatus();
    AlarmClockModel::GetInstance()->SetCancelAlarm(alarmId);
}

void AlarmPopUpPresenter::DelayAlarmRing()
{
    uint8_t alarmId = 0;
    bool alarmStatus;
    AlarmClockModel::GetInstance()->PopInterCancelMusic();
    alarmStatus = AlarmClockModel::GetInstance()->GetListenClockId(&alarmId);
    if (!alarmStatus) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPopUpPresenter::DelayAlarmRing get alarm Id false!");
        return;
    }
    AlarmClockModel::GetInstance()->SetDelayAlarm(alarmId);
}

bool AlarmPopUpPresenter::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "StatusButtonClickListener %s", view.GetViewId());
    if (strcmp(view.GetViewId(), ALARM_DELETE) == 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmRingPresenter delete clock");
        CancelAlarmRing();
        AlarmPopUpView::GetInstance()->ChangeView(CHANGE_POP_UP_INTERFACE, CHANGE_INTERFACE_CLOSE_RING);
        return true;
    } else if (strcmp(view.GetViewId(), ALARM_CLOCK) == 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmRingPresenter delay clock");
        DelayAlarmRing();
        AlarmPopUpView::GetInstance()->ChangeView(CHANGE_POP_UP_INTERFACE, CHANGE_INTERFACE_DELAY_RING);
        return true;
    }
    return true;
}

bool AlarmPopUpPresenter::OnDrag(UIView& view, const DragEvent& event)
{
    return true;
}

bool AlarmPopUpPresenter::OnDragEnd(UIView& view, const DragEvent& event)
{
    if (event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "pop view DIRECTION_LEFT_TO_RIGHT");
        if (AlarmPopUpView::GetInstance()->GetPopUpView(CHANGE_POP_UP_INTERFACE) == &view) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "pop view cancel");
            // 此处跳转到延迟界面
            DelayAlarmRing();
            AlarmPopUpView::GetInstance()->ChangeView(CHANGE_POP_UP_INTERFACE, CHANGE_INTERFACE_DELAY_RING);
            return true;
        } else if (AlarmPopUpView::GetInstance()->GetPopUpView(CHANGE_INTERFACE_CLOSE_RING) == &view) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "pop CHANGE_INTERFACE_CLOSE_RING");
            if (AlarmCloseRingView::GetInstance() == nullptr) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmRingPresenter delay clock");
                return false;
            }
            AlarmCloseRingView::GetInstance()->CancelCloseHandle();
        } else if (AlarmPopUpView::GetInstance()->GetPopUpView(CHANGE_INTERFACE_DELAY_RING) == &view) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "pop CHANGE_INTERFACE_DELAY_RING");
            AlarmRingView::GetInstance()->CancelTimerHandle();
        }
        uint16_t preSlice = AlarmClockModel::GetInstance()->JudgePreSlice();
        NativeAbility::GetInstance().ChangeSlice(preSlice);
    }
    return true;
}

void AlarmPopUpPresenter::Callback()
{
    if (isStartCalcu) {
        if (calcuNotifyNums == RENDER_FINISH) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPlayer init and start!!");
            AlarmClockModel::GetInstance()->AlarmPlayerStart();
            calcuNotifyNums = 0;
            isStartCalcu = false;
        }
        ++calcuNotifyNums;
    }
}

void AlarmPopUpPresenter::OnResume()
{
    Init();
}

void AlarmPopUpPresenter::OnPause()
{
    Deinit();
}
}