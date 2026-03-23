/*
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alarm notification implement
 * Author: CompanyName
 * Create: 2023-08-11
 */

#include "notification_manager.h"
#include "wearable_log.h"
#include "UiConfig.h"
#include "alarm/AlarmPopUpPresenter.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"

namespace OHOS {
static constexpr const char* RING_AGAIN = "响铃将在十分钟后再次响起";
constexpr int32_t NOTIFY_THREE_SECONDS = 3000;
constexpr int32_t NOTIFY_ONE_SECOND = 1000;
constexpr int32_t NOTIFY_THIRTY_SECONDS = 30000;
constexpr int32_t NOTIFY_ONE_MINUTE = 60000;
constexpr int32_t NOTOFY_NO_OPERATE_TIME = 35000;
GraphicTimer* g_notifyStopHandle = nullptr; // timer of close view and alarm ring view
GraphicTimer* g_alarmRing = nullptr; // timer of alarm view

UIViewGroup* g_alarmView = nullptr;
UIViewGroup* g_closeView = nullptr;
UIViewGroup* g_ringView = nullptr;
UIImageView* g_imageClose = nullptr;
UIImageView* g_imageRing = nullptr;
UIImageView* g_imageDelete = nullptr;
UIImageView* g_imageClock = nullptr;
UILabel* g_closeLable = nullptr;
UILabel* g_ringLable = nullptr;
UILabel* g_mainLable = nullptr;
UIDigitalClock* g_dClock = nullptr;

// alarm close view
constexpr int16_t MAX_SET_ZERO_LEN = 0;
constexpr int16_t MAX_SET_SRC_LEN = 90;
constexpr int16_t MAX_IMAGE_X_LEN = 165;
constexpr int16_t MAX_IMAGE_Y_LEN = 150;
constexpr int16_t MAX_LABEL_X_LEN = 187;
constexpr int16_t MAX_LABEL_Y_LEN = 300;
constexpr int16_t MAX_LABEL_LENGTH_LEN = 350;
constexpr int16_t MAX_LABEL_HEIGHT_LEN = 40;
constexpr int16_t MAX_LABEL_FONT_LEN = 35;

// alarm main view
constexpr int16_t CONTAINER_X = 0;
constexpr int16_t CONTAINER_Y = 0;
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

// alarm delay view
constexpr int16_t SET_ZERO = 0;
constexpr int16_t SET_SRC = 150;
constexpr int16_t DELAY_IMAGE_X = 150;
constexpr int16_t DELAY_IMAGE_Y = 140;
constexpr int16_t DELAY_LABEL_X = 80;
constexpr int16_t DELAY_LABEL_Y = 300;
constexpr int16_t DELAY_LABEL_LENGTH = 350;
constexpr int16_t DELAY_LABEL_HEIGHT = 35;
constexpr int16_t DELAY_LABEL_FONT = 25;

void StopNotifyCallback(void* data)
{
    (void)data;
    NotificationManager::GetInstance()->StopNotify();
}

void NoOperateCallback(void* data)
{
    (void)data;
    NotificationManager::GetInstance()->StopNotify();
    AlarmPopUpPresenter::GetInstance()->DelayAlarmRing();
}

void StartAlarmTimer(AlarmTimerType type, int32_t interval)
{
    uint32_t tick;
    bool retTimer;
    if (type >= AlarmTimerType::NOTIFY_ALARM_UNKNOWN) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "StartAlarmTimer type:%d invalid!!", type);
        return;
    }
    switch (type) {
        case AlarmTimerType::NOTIFY_ALARM_CLOSE:
        case AlarmTimerType::NOTIFY_ALARM_DELAY:
            tick = GetOSTick(interval);
            g_notifyStopHandle = new GraphicTimer(tick, StopNotifyCallback, nullptr, true);
            retTimer = g_notifyStopHandle->Start();
            if (!retTimer && g_notifyStopHandle != nullptr) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP,
                    "StartAlarmTimer osTimerStart failed, type= %d!!", type);
                delete g_notifyStopHandle;
                g_notifyStopHandle = nullptr;
                return;
            }
            break;
        case AlarmTimerType::NOTIFY_ALARM_RING:
            tick = GetOSTick(interval);
            g_alarmRing = new GraphicTimer(tick, NoOperateCallback, nullptr, true);
            retTimer = g_alarmRing->Start();
            if (!retTimer && g_alarmRing != nullptr) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP,
                    "StartAlarmTimer osTimerStart failed, type= %d!!", type);
                delete g_alarmRing;
                g_alarmRing = nullptr;
                return;
            }
            break;
        default:
            break;
    }
}

void DisableAlarmTimer(AlarmTimerType type)
{
    if (type >= AlarmTimerType::NOTIFY_ALARM_UNKNOWN) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "DisableAlarmTimer type:%d invalid!!", type);
        return;
    }
    switch (type) {
        case AlarmTimerType::NOTIFY_ALARM_CLOSE:
        case AlarmTimerType::NOTIFY_ALARM_DELAY:
            if (g_notifyStopHandle != nullptr) {
                delete g_notifyStopHandle;
                g_notifyStopHandle = nullptr;
            }
            break;
        case AlarmTimerType::NOTIFY_ALARM_RING:
            if (g_alarmRing != nullptr) {
                delete g_alarmRing;
                g_alarmRing = nullptr;
            }
            break;
        default:
            break;
    }
}

UIViewGroup *CreateAlarmCloseView()
{
    g_closeView = new UIViewGroup();
    if (g_closeView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CreateAlarmCloseView g_closeView null!");
        return nullptr;
    }
    g_closeView->SetPosition(MAX_SET_ZERO_LEN, MAX_SET_ZERO_LEN);
    g_closeView->SetWidth(Screen::GetInstance().GetWidth());
    g_closeView->SetHeight(Screen::GetInstance().GetHeight());
    g_imageClose = new UIImageView();
    if (g_imageClose == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CreateAlarmCloseView g_imageClose null!");
        delete g_closeView;
        g_closeView = nullptr;
        return nullptr;
    }
    g_imageClose->SetPosition(MAX_IMAGE_X_LEN, MAX_IMAGE_Y_LEN);
    g_imageClose->SetWidth(MAX_SET_SRC_LEN);
    g_imageClose->SetHeight(MAX_SET_SRC_LEN);
    g_imageClose->SetSrc(ALARM_CLOCK_CLOSE_ADD_PATH);
    g_imageClose->SetTouchable(true);
    g_closeView->Add(g_imageClose);

    g_closeLable = new UILabel();
    if (g_closeLable == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CreateAlarmCloseView g_closeLable null!");
        delete g_closeView;
        g_closeView = nullptr;
        delete g_imageClose;
        g_imageClose = nullptr;
        return nullptr;
    }
    g_closeLable->SetPosition(MAX_LABEL_X_LEN, MAX_LABEL_Y_LEN, MAX_LABEL_LENGTH_LEN, MAX_LABEL_HEIGHT_LEN);
    g_closeLable->SetFont(DEFAULT_VECTOR_FONT_FILENAME, MAX_LABEL_FONT_LEN);
    g_closeLable->SetText("CLOSE");
    g_closeView->Add(g_closeLable);
    g_closeView->SetViewId(ALARM_VIEW_CLOSE);
    UIView::OnDragListener *listener = static_cast<UIView::OnDragListener*>(AlarmViewEventListener::GetInstance());
    g_closeView->SetOnDragListener(listener);
    g_closeView->SetDraggable(true);
    g_closeView->SetTouchable(true);
    NotificationManager::GetInstance()->RegisterNotifyCleanupFunction(FreeAlarmCloseView);
    return g_closeView;
}

void FreeAlarmCloseView(void)
{
    if (g_closeView != nullptr) {
        DisableAlarmTimer(AlarmTimerType::NOTIFY_ALARM_CLOSE);
        g_closeView->RemoveAll();
        delete g_imageClose;
        g_imageClose = nullptr;
        delete g_closeLable;
        g_closeLable = nullptr;
        RootView::GetInstance()->Remove(g_closeView);
        delete g_closeView;
        g_closeView = nullptr;
    }
}

UIViewGroup *CreateAlarmRingView()
{
    g_ringView = new UIViewGroup();
    if (g_ringView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CreateAlarmRingView g_ringView null!");
        return nullptr;
    }
    g_ringView->SetPosition(SET_ZERO, SET_ZERO);
    g_ringView->SetWidth(Screen::GetInstance().GetWidth());
    g_ringView->SetHeight(Screen::GetInstance().GetHeight());
    g_imageRing = new UIImageView();
    if (g_imageRing == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CreateAlarmRingView g_imageRing null!");
        delete g_ringView;
        g_ringView = nullptr;
        return nullptr;
    }
    g_imageRing->SetPosition(DELAY_IMAGE_X, DELAY_IMAGE_Y);
    g_imageRing->SetWidth(SET_SRC);
    g_imageRing->SetHeight(SET_SRC);
    g_imageRing->SetSrc(ALARM_CLOCK_POP_ADD_PATH);
    g_ringView->Add(g_imageRing);
    g_ringLable = new UILabel();
    if (g_ringLable == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmRingView::g_ringLable null!");
        delete g_ringView;
        g_ringView = nullptr;
        delete g_imageRing;
        return nullptr;
    }
    g_ringLable->SetPosition(DELAY_LABEL_X, DELAY_LABEL_Y, DELAY_LABEL_LENGTH, DELAY_LABEL_HEIGHT);
    g_ringLable->SetFont(DEFAULT_VECTOR_FONT_FILENAME, DELAY_LABEL_FONT);
    g_ringLable->SetText(RING_AGAIN);
    g_ringView->Add(g_ringLable);
    g_ringView->SetViewId(ALARM_VIEW_DELAY);
    UIView::OnDragListener *listener = static_cast<UIView::OnDragListener*>(AlarmViewEventListener::GetInstance());
    g_ringView->SetOnDragListener(listener);
    g_ringView->SetDraggable(true);
    g_ringView->SetTouchable(true);
    NotificationManager::GetInstance()->RegisterNotifyCleanupFunction(FreeAlarmRingView);
    return g_ringView;
}

void FreeAlarmRingView(void)
{
    if (g_ringView != nullptr) {
        DisableAlarmTimer(AlarmTimerType::NOTIFY_ALARM_DELAY);
        g_ringView->RemoveAll();
        delete g_imageRing;
        g_imageRing = nullptr;
        delete g_ringLable;
        g_ringLable = nullptr;
        RootView::GetInstance()->Remove(g_ringView);
        delete g_ringView;
        g_ringView = nullptr;
    }
}

static bool CreateAlarmViewImage(void)
{
    g_imageDelete = new UIImageView();
    if (g_imageDelete == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "showAlarmNotify g_imageDelete null!");
        return false;
    }
    g_imageDelete->SetPosition(IMAGE_DELETE_X, IMAGE_DELETE_Y);
    g_imageDelete->SetWidth(MAX_SET_SRC_LEN);
    g_imageDelete->SetHeight(MAX_SET_SRC_LEN);
    g_imageDelete->SetViewId(ALARM_IMAGE_DELETE);
    g_imageDelete->SetSrc(ALARM_DELETE_ADD_PATH);
    g_imageDelete->SetTouchable(true);
    UIView::OnClickListener *closeListener = static_cast<UIView::OnClickListener *>\
        (AlarmViewEventListener::GetInstance());
    g_imageDelete->SetOnClickListener(closeListener);

    g_imageClock = new UIImageView();
    if (g_imageClock == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "showAlarmNotify g_imageClock null!");
        delete g_imageDelete;
        return false;
    }
    g_imageClock->SetPosition(IMAGE_CLOCK_X, IMAGE_CLOCK_Y);
    g_imageClock->SetWidth(MAX_SET_SRC_LEN);
    g_imageClock->SetHeight(MAX_SET_SRC_LEN);
    g_imageClock->SetViewId(ALARM_IMAGE_CLOCK);
    g_imageClock->SetSrc(ALARM_CLOCK_ADD_PATH);
    g_imageClock->SetTouchable(true);
    g_imageClock->SetTouchable(true);
    UIView::OnClickListener *clockListener = static_cast<UIView::OnClickListener *>\
        (OHOS::AlarmViewEventListener::GetInstance());
    g_imageClock->SetOnClickListener(clockListener);
    return true;
}

static void FreeAlarmViewImage(void)
{
    delete g_imageDelete;
    g_imageDelete = nullptr;
    delete g_imageClock;
    g_imageClock = nullptr;
}

static bool CreateAlarmViewLabel(void)
{
    struct tm localTime;
    int16 thisHour;
    int16 thisMin;

    g_mainLable = new UILabel();
    if (g_mainLable == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CreateAlarmViewLabel g_mainLable is nullptr");
        return false;
    }
    g_mainLable->SetPosition(LABEL_X, LABEL_Y, LABEL_WIDTH, LABEL_HEIGHT);
    g_mainLable->SetText("闹钟");
    g_mainLable->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT);

    g_dClock = new UIDigitalClock();
    if (g_dClock == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CreateAlarmViewLabel g_dClock is nullptr");
        delete g_mainLable;
        return false;
    }
    g_dClock->SetPosition(CLOCK_X, CLOCK_Y);
    g_dClock->Resize(DIGITAL_CLOCK_WIDTH, DIGITAL_CLOCK_HEIGHT);
    g_dClock->SetFont(DEFAULT_VECTOR_FONT_FILENAME, CLOCK_FONT);
    time_t seconds = time(nullptr);
    if (localtime_r(&seconds, &localTime) == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CreateAlarmViewLabel get localtime error!");
        delete g_mainLable;
        delete g_dClock;
        return false;
    }
    AlarmClockModel::GetInstance()->GetPopTimeData(&thisHour, &thisMin);
    g_dClock->SetDisplayMode(UIDigitalClock::DISPLAY_24_HOUR_NO_SECONDS);
    g_dClock->SetTime24Hour(thisHour, thisMin, 0);
    return true;
}

static void FreeAlarmViewLabel(void)
{
    delete g_mainLable;
    g_mainLable = nullptr;
    delete g_dClock;
    g_dClock = nullptr;
}

UIViewGroup* CreateAlarmView(void)
{
    AlarmClockModel::GetInstance()->AlarmGetClockNum();
    g_alarmView = new UIViewGroup();
    if (g_alarmView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CreateAlarmView g_alarmView null!");
        return nullptr;
    }
    g_alarmView->SetPosition(CONTAINER_X, CONTAINER_Y,
                             Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    g_alarmView->SetViewId(ALARM_VIEW_POPUP);

    if (!CreateAlarmViewLabel()) {
        delete g_alarmView;
        g_alarmView = nullptr;
        return nullptr;
    }

    if (!CreateAlarmViewImage()) {
        delete g_alarmView;
        g_alarmView = nullptr;
        FreeAlarmViewLabel();
        return nullptr;
    }
    g_alarmView->Add(g_mainLable);
    g_alarmView->Add(g_dClock);
    g_alarmView->Add(g_imageDelete);
    g_alarmView->Add(g_imageClock);

    g_alarmView->SetDraggable(true);
    g_alarmView->SetTouchable(true);
    UIView::OnDragListener *listener = static_cast<UIView::OnDragListener*>(AlarmViewEventListener::GetInstance());
    g_alarmView->SetOnDragListener(listener);
    StartAlarmTimer(AlarmTimerType::NOTIFY_ALARM_RING, NOTOFY_NO_OPERATE_TIME);
    NotificationManager::GetInstance()->RegisterNotifyCleanupFunction(FreeAlarmView);
    return g_alarmView;
}
void FreeAlarmView(void)
{
    if (g_alarmView != nullptr) {
        DisableAlarmTimer(AlarmTimerType::NOTIFY_ALARM_RING);
        g_alarmView->RemoveAll();
        FreeAlarmViewLabel();
        FreeAlarmViewImage();
        RootView::GetInstance()->Remove(g_alarmView);
        delete g_alarmView;
        g_alarmView = nullptr;
    }
}

AlarmViewEventListener::AlarmViewEventListener()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmViewEventListener :%p", this);
}

AlarmViewEventListener::~AlarmViewEventListener()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "~AlarmViewEventListener");
}

AlarmViewEventListener *AlarmViewEventListener::GetInstance(void)
{
    static AlarmViewEventListener alarmViewEventListener;
    return &alarmViewEventListener;
}

bool AlarmViewEventListener::OnDrag(UIView& view, const DragEvent& event)
{
    return true;
}

bool AlarmViewEventListener::OnDragEnd(UIView& view, const DragEvent& event)
{
    if (event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) {
        if (strcmp(view.GetViewId(), ALARM_VIEW_POPUP) == 0) {
            DisableAlarmTimer(AlarmTimerType::NOTIFY_ALARM_RING);
            AlarmPopUpPresenter::GetInstance()->DelayAlarmRing();
            UIViewGroup* alarmRingView = CreateAlarmRingView();
            NotificationManager::GetInstance()->screenContainer_->SetVisible(false);
            RootView::GetInstance()->Add(alarmRingView);
            alarmRingView->SetVisible(true);
            StartAlarmTimer(AlarmTimerType::NOTIFY_ALARM_DELAY, NOTIFY_THREE_SECONDS);
        } else if (strcmp(view.GetViewId(), ALARM_VIEW_CLOSE) == 0 ||
                   strcmp(view.GetViewId(), ALARM_VIEW_DELAY) == 0) {
            NotificationManager::GetInstance()->StopNotify();
        }
    }
    return true;
}

bool AlarmViewEventListener::OnClick(UIView& view, const ClickEvent& event)
{
    UNUSED(event);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlamNotify ClickListener %s", view.GetViewId());
    if (strcmp(view.GetViewId(), ALARM_IMAGE_DELETE) == 0) {
        DisableAlarmTimer(AlarmTimerType::NOTIFY_ALARM_RING);
        AlarmPopUpPresenter::GetInstance()->CancelAlarmRing();
        UIViewGroup* alarmCloseView = CreateAlarmCloseView();
        NotificationManager::GetInstance()->screenContainer_->SetVisible(false);
        RootView::GetInstance()->Add(alarmCloseView);
        alarmCloseView->SetVisible(true);
        StartAlarmTimer(AlarmTimerType::NOTIFY_ALARM_CLOSE, NOTIFY_THREE_SECONDS);
        return true;
    } else if (strcmp(view.GetViewId(), ALARM_IMAGE_CLOCK) == 0) {
        DisableAlarmTimer(AlarmTimerType::NOTIFY_ALARM_RING);
        AlarmPopUpPresenter::GetInstance()->DelayAlarmRing();
        UIViewGroup* alarmRingView = CreateAlarmRingView();
        NotificationManager::GetInstance()->screenContainer_->SetVisible(false);
        RootView::GetInstance()->Add(alarmRingView);
        alarmRingView->SetVisible(true);
        StartAlarmTimer(AlarmTimerType::NOTIFY_ALARM_DELAY, NOTIFY_THREE_SECONDS);
    }
    return true;
}
}  // namespace OHOS
