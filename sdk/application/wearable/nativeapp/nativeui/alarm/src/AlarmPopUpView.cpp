/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Alarm
 * Author:
 * Create: 2021-09-11
 */

#include "alarm/AlarmPopUpView.h"
#include "alarm/AlarmPopUpPresenter.h"
#include "common/screen.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#ifdef BUILD_IN_LINUX
#include "localtime.h"
#endif
#include <string>

namespace OHOS {
static AlarmPopUpView *g_pAlarmPopUpView = nullptr;

constexpr int16_t CONTAINER_X = 0;
constexpr int16_t CONTAINER_Y = 0;
constexpr int16_t SET_ZERO = 0;
constexpr int16_t SET_TWO = 2;
constexpr int16_t SET_SRC = 90;
constexpr int16_t LABEL_X = 185;
constexpr int16_t LABEL_Y = 100;
constexpr int16_t LABEL_WIDTH = 250;
constexpr int16_t LABEL_HEIGHT = 45;
constexpr int16_t LABEL_FONT = 40;
constexpr int16_t CLOCK_FONT = 70;
constexpr int16_t CLOCK_X = 140;
constexpr int16_t CLOCK_Y = 170;
constexpr int16_t IMAGE_DELETE_X = 85;
constexpr int16_t IMAGE_DELETE_Y = 300;
constexpr int16_t IMAGE_CLOCK_X = 275;
constexpr int16_t IMAGE_CLOCK_Y = 300;
constexpr int16_t DIGITAL_CLOCK_WIDTH = 350;
constexpr int16_t DIGITAL_CLOCK_HEIGHT = 75;
constexpr uint8_t MAX_HOUR = 24;

AlarmPopUpView::AlarmPopUpView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPopUpView::AlarmPopUpView");
    g_pAlarmPopUpView = this;
}

AlarmPopUpView::~AlarmPopUpView()
{
    delete labelPopup;
    labelPopup = nullptr;
    delete dClock;
    dClock = nullptr;
    delete imageDelete;
    imageDelete = nullptr;
    delete imageClock;
    imageClock = nullptr;
    delete closeRingView;
    closeRingView = nullptr;
    delete ringView;
    ringView = nullptr;
    delete group;
    group = nullptr;
    for (int alarmNum = SET_ZERO; alarmNum <= SET_TWO; alarmNum++) {
        container[alarmNum] = nullptr;
    }
    g_pAlarmPopUpView = nullptr;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPopUpView::~AlarmPopUpView");
}

AlarmPopUpView *AlarmPopUpView::GetInstance(void)
{
    return g_pAlarmPopUpView;
}

void AlarmPopUpView::AlarmCreateLabel(void)
{
    labelPopup = new UILabel();
    if (labelPopup == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPopUpView::labelPopup null!");
        return;
    }
    struct tm local_time;
    time_t seconds = time(nullptr);

    labelPopup->SetPosition(LABEL_X, LABEL_Y, LABEL_WIDTH, LABEL_HEIGHT);
    labelPopup->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT);
    labelPopup->SetText("闹钟");
    group->Add(labelPopup);

    dClock = new UIDigitalClock();
    if (dClock == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPopUpView::dClock null!");
        return;
    }
    dClock->SetPosition(CLOCK_X, CLOCK_Y);
    dClock->Resize(DIGITAL_CLOCK_WIDTH, DIGITAL_CLOCK_HEIGHT);
    dClock->SetFont(DEFAULT_VECTOR_FONT_FILENAME, CLOCK_FONT);
    if (localtime_r(&seconds, &local_time) == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPopUpView::AlarmCreateLabel get localtime error!");
        return;
    }

    int16_t thisHour;
    int16_t thisMin;
    AlarmClockModel::GetInstance()->GetPopTimeData(&thisHour, &thisMin);
    dClock->SetDisplayMode(UIDigitalClock::DISPLAY_24_HOUR_NO_SECONDS);
    dClock->SetTime24Hour(thisHour, thisMin, 0);
    group->Add(dClock);
}

void AlarmPopUpView::AlarmCreateImage(void)
{
    imageDelete = new UIImageView();
    if (imageDelete == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPopUpView::imageDelete null!");
        return;
    }
    imageDelete->SetPosition(IMAGE_DELETE_X, IMAGE_DELETE_Y);
    imageDelete->SetWidth(SET_SRC);
    imageDelete->SetHeight(SET_SRC);
    imageDelete->SetViewId(ALARM_DELETE);
    imageDelete->SetSrc(ALARM_DELETE_ADD_PATH);
    imageDelete->SetTouchable(true);
    group->Add(imageDelete);

    UIView::OnClickListener *clickListener = (UIView::OnClickListener *)AlarmPopUpPresenter::GetInstance();
    imageDelete->SetOnClickListener(clickListener);

    imageClock = new UIImageView();
    if (imageClock == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPopUpView::imageClock null!");
        return;
    }
    imageClock->SetPosition(IMAGE_CLOCK_X, IMAGE_CLOCK_Y);
    imageClock->SetWidth(SET_SRC);
    imageClock->SetHeight(SET_SRC);
    imageClock->SetViewId(ALARM_CLOCK);
    imageClock->SetSrc(ALARM_CLOCK_ADD_PATH);
    imageClock->SetTouchable(true);
    group->Add(imageClock);

    imageClock->SetOnClickListener(clickListener);
    UIView::OnDragListener *listener = static_cast<UIView::OnDragListener*>(AlarmPopUpPresenter::GetInstance());
    group->SetOnDragListener(listener);
    group->SetDraggable(true);
    group->SetTouchable(true);
}

void AlarmPopUpView::OnStart()
{

    group = new UIViewGroup();
    if (group == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPopUpView::group null!");
        return;
    }
    group->SetPosition(CONTAINER_X, CONTAINER_Y, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);

    AlarmCreateLabel();
    AlarmCreateImage();
    AlarmClockModel::GetInstance()->SetSliceId();
    container[SET_ZERO] = group;

    AddViewToRootContainer(container[SET_ZERO]);
}

UIView* AlarmPopUpView::GetPopUpView(int16_t viewIndex)
{
    return container[viewIndex];
}

void AlarmPopUpView::ChangeView(int16_t hideView, int16_t showView)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainAlarmView::True");
    container[hideView]->SetVisible(false);
    if (showView == CHANGE_INTERFACE_CLOSE_RING) {
        if (container[showView] == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainAlarmView::AlarmCloseRingView");
            closeRingView = new AlarmCloseRingView();
            if (closeRingView == nullptr) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainAlarmView:: new closeRingView fail");
                return;
            }
            container[showView] = closeRingView->InitCloseRingView();
            if (container[showView] == nullptr) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainAlarmView:: new container[%d] fail", showView);
                return;
            }
            AddViewToRootContainer(container[showView]);
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainAlarmView:: new contai ner[%d] success", showView);
            AlarmCloseRingView::GetInstance()->AlarmCloseShowTimer();
        }
        container[showView]->SetVisible(true);
        Draw();
    } else if (showView == CHANGE_INTERFACE_DELAY_RING) {
        if (container[showView] == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainAlarmView::AlarmRingView");
            ringView = new AlarmRingView();
            if (ringView == nullptr) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainAlarmView:: new ringView fail");
                return;
            }
            container[showView] = ringView->InitRingView();
            if (container[showView] == nullptr) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainAlarmView:: new container[%d] fail", showView);
                return;
            }
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainAlarmView:: new container[%d] success", showView);
            AddViewToRootContainer(container[showView]);
            AlarmRingView::GetInstance()->AlarmRingDelayShowtimer();
        }
        container[showView]->SetVisible(true);
        Draw();
    }
}

}
