/*
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: calendar notification implement
 * Author: CompanyName
 * Create: 2025-10-23
 */
 
#include "notification_manager.h"
#include "wearable_log.h"
#include "UiConfig.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "ability_manager.h"
 
namespace OHOS {
constexpr int32_t NOTIFY_THREE_SECONDS = 3000;
constexpr int32_t NOTIFY_ONE_SECOND = 1000;
constexpr int32_t NOTIFY_THIRTY_SECONDS = 30000;
constexpr int32_t NOTIFY_ONE_MINUTE = 60000;
constexpr int32_t NOTOFY_NO_OPERATE_TIME = 35000;
 
UIViewGroup* g_calendarView = nullptr;
UIImageView* g_calendarExit = nullptr;
UIImageView* g_calendarConfirm = nullptr;
UILabel* g_tooltip = nullptr;
// calendar close view
constexpr int16_t MAX_SET_SRC_LEN = 90;
char g_bundlename[256] = {0};
// calendar main view
constexpr int16_t CONTAINER_X = 0;
constexpr int16_t CONTAINER_Y = 0;
constexpr int16_t IMAGE_DELETE_X = 85;
constexpr int16_t IMAGE_DELETE_Y = 300;
constexpr int16_t IMAGE_CLOCK_X = 275;
constexpr int16_t IMAGE_CLOCK_Y = 300;
constexpr int16_t BASE_YEARS = 1900;
 
// calendar delay view
constexpr int16_t DELAY_LABEL_X = 80;
constexpr int16_t DELAY_LABEL_Y = 100;
constexpr int16_t DELAY_LABEL_LENGTH = 350;
constexpr int16_t DELAY_LABEL_HEIGHT = 35;
constexpr int16_t DELAY_LABEL_FONT = 25;
 
static bool CreateCalendarViewImage(void)
{
    g_calendarExit = new UIImageView();
    if (g_calendarExit == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "showCalendarNotify g_calendarExit null!");
        return false;
    }
    g_calendarExit->SetPosition(IMAGE_DELETE_X, IMAGE_DELETE_Y);
    g_calendarExit->SetWidth(MAX_SET_SRC_LEN);
    g_calendarExit->SetHeight(MAX_SET_SRC_LEN);
    g_calendarExit->SetViewId(CALENDAR_IMAGE_DELETE);
    g_calendarExit->SetSrc(ALARM_DELETE_ADD_PATH);
    g_calendarExit->SetTouchable(true);
    UIView::OnClickListener *closeListener = static_cast<UIView::OnClickListener *>\
        (CalendarViewEventListener::GetInstance());
    g_calendarExit->SetOnClickListener(closeListener);
    g_calendarConfirm = new UIImageView();
    if (g_calendarConfirm == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "showCalendarNotify g_calendarConfirm null!");
        delete g_calendarExit;
        return false;
    }
    g_calendarConfirm->SetPosition(IMAGE_CLOCK_X, IMAGE_CLOCK_Y);
    g_calendarConfirm->SetWidth(MAX_SET_SRC_LEN);
    g_calendarConfirm->SetHeight(MAX_SET_SRC_LEN);
    g_calendarConfirm->SetViewId(CALENDAR_IMAGE_CLOCK);
    g_calendarConfirm->SetSrc(ALARM_CLOCK_ADD_PATH);
    g_calendarConfirm->SetTouchable(true);
    UIView::OnClickListener *clockListener = static_cast<UIView::OnClickListener *>\
        (OHOS::CalendarViewEventListener::GetInstance());
    g_calendarConfirm->SetOnClickListener(clockListener);
    return true;
}
 
static void FreeCalendarViewImage(void)
{
    delete g_calendarExit;
    g_calendarExit = nullptr;
    delete g_calendarConfirm;
    g_calendarConfirm = nullptr;
}
 
static const char* GetFormatTimestamp(long long timestamp)
{
    static char buffer[20];
    
    time_t seconds = (time_t)(timestamp / NOTIFY_ONE_SECOND);
 
    struct tm *localTime = localtime(&seconds);
    if (localTime == NULL) {
        return NULL;
    }
 
    int size = sprintf_s(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d",
            localTime->tm_year + BASE_YEARS,
            localTime->tm_mon + 1,
            localTime->tm_mday,
            localTime->tm_hour,
            localTime->tm_min,
            localTime->tm_sec);
    if (size < 0) {
        return NULL;
    }
    return buffer;
}
 
UIViewGroup* CreateCalendarView(CalendarEvent *event)
{
    g_calendarView = new UIViewGroup();
    if (g_calendarView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CreateCalendarView g_calendarView null!");
        return nullptr;
    }
    g_calendarView->SetPosition(CONTAINER_X, CONTAINER_Y,
        Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    g_calendarView->SetViewId(CALENDAR_VIEW_POPUP);
 
    if (!CreateCalendarViewImage()) {
        delete g_calendarView;
        g_calendarView = nullptr;
        return nullptr;
    }
    g_tooltip = new UILabel();
    if (g_tooltip == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmRingView::g_tooltip null!");
        delete g_calendarView;
        g_calendarView = nullptr;
        FreeCalendarViewImage();
        return nullptr;
    }
    g_tooltip->SetPosition(DELAY_LABEL_X, DELAY_LABEL_Y, DELAY_LABEL_LENGTH, DELAY_LABEL_HEIGHT);
    g_tooltip->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);
    g_tooltip->SetFont(DEFAULT_VECTOR_FONT_FILENAME, DELAY_LABEL_FONT);
    char text[256] = {0};
    int size = sprintf_s(text, sizeof(text), "id = %d                                      bundle = %s eventContent = %s starttime = %s",
        event->id, event->bundlename, event->eventContent, GetFormatTimestamp(event->startTime));
    memset_s(g_bundlename, sizeof(g_bundlename), 0, sizeof(g_bundlename));
    int ret = memcpy_s(g_bundlename, sizeof(g_bundlename), event->bundlename, strlen(event->bundlename));
    if (size < 0 || ret != EOK) {
        FreeCalendarView();
        return nullptr;
    }
    g_tooltip->SetText(text);
    g_calendarView->Add(g_calendarExit);
    g_calendarView->Add(g_calendarConfirm);
    g_calendarView->Add(g_tooltip);
    g_calendarView->SetDraggable(true);
    g_calendarView->SetTouchable(true);
    UIView::OnDragListener *listener = static_cast<UIView::OnDragListener*>(CalendarViewEventListener::GetInstance());
    g_calendarView->SetOnDragListener(listener);
    NotificationManager::GetInstance()->RegisterNotifyCleanupFunction(FreeCalendarView);
    return g_calendarView;
}
void FreeCalendarView(void)
{
    if (g_calendarView != nullptr) {
        g_calendarView->RemoveAll();
        FreeCalendarViewImage();
        delete g_tooltip;
        g_tooltip = nullptr;
        RootView::GetInstance()->Remove(g_calendarView);
        delete g_calendarView;
        g_calendarView = nullptr;
    }
}
 
CalendarViewEventListener::CalendarViewEventListener()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "CalendarViewEventListener :%p", this);
}
 
CalendarViewEventListener::~CalendarViewEventListener()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "~CalendarViewEventListener");
}
 
CalendarViewEventListener *CalendarViewEventListener::GetInstance(void)
{
    static CalendarViewEventListener calendarViewEventListener;
    return &calendarViewEventListener;
}
 
bool CalendarViewEventListener::OnDrag(UIView& view, const DragEvent& event)
{
    return true;
}
 
bool CalendarViewEventListener::OnDragEnd(UIView& view, const DragEvent& event)
{
    if (event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) {
        if (strcmp(view.GetViewId(), CALENDAR_VIEW_POPUP) == 0) {
            NotificationManager::GetInstance()->screenContainer_->SetVisible(false);
            NotificationManager::GetInstance()->StopNotify();
        } else if (strcmp(view.GetViewId(), CALENDAR_VIEW_CLOSE) == 0 ||
                   strcmp(view.GetViewId(), CALENDAR_VIEW_DELAY) == 0) {
            NotificationManager::GetInstance()->screenContainer_->SetVisible(false);
            NotificationManager::GetInstance()->StopNotify();
        }
    }
    return true;
}
 
bool CalendarViewEventListener::OnClick(UIView& view, const ClickEvent& event)
{
    UNUSED(event);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlamNotify ClickListener %s", view.GetViewId());
    if (strcmp(view.GetViewId(), CALENDAR_IMAGE_DELETE) == 0) {
        NotificationManager::GetInstance()->screenContainer_->SetVisible(false);
        Want *want = new Want();
        memset_s(want, sizeof(Want), 0, sizeof(Want));
        ElementName startElement = {};
        startElement.abilityName = (char *)"default";
        startElement.bundleName = g_bundlename;
        startElement.deviceId = nullptr;
        SetWantElement(want, startElement);
        StartAbility(want);
        ClearWant(want);
        delete want;
        NotificationManager::GetInstance()->StopNotify();
    } else if (strcmp(view.GetViewId(), CALENDAR_IMAGE_CLOCK) == 0) {
        NotificationManager::GetInstance()->screenContainer_->SetVisible(false);
        NotificationManager::GetInstance()->StopNotify();
    }
    return true;
}
}  // namespace OHOS