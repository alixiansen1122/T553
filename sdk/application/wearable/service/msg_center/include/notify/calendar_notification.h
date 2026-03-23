/*
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: calendar notification api
 * Author: CompanyName
 * Create: 2025-10-23
 */
 
#ifndef CALENDAR_NOTIFICATION_H
#define CALENDAR_NOTIFICATION_H
 
static constexpr const char* CALENDAR_IMAGE_DELETE = "calendar_delete";
static constexpr const char* CALENDAR_IMAGE_CLOCK = "calendar_clock";
static constexpr const char* CALENDAR_VIEW_CLOSE = "calendar_close";
static constexpr const char* CALENDAR_VIEW_DELAY = "calendar_delay";
static constexpr const char* CALENDAR_VIEW_POPUP = "calendar_popup";
 
enum class CalendarTimerType {
    NOTIFY_CALENDAR_CLOSE = 0,
    NOTIFY_CALENDAR_DELAY = 1,
    NOTIFY_CALENDAR_RING = 2,
    NOTIFY_CALENDAR_UNKNOWN = 10,
};
 
typedef struct {
    int id;
    int type;
    int64_t startTime;
    int64_t endTime;
    bool isAllDay;
    bool hasReminder;
    bool isRepeat;
    int64_t recurrenceFrequency;
    char bundlename[64]; // 外部传入字符串，内部不负责释放
    char eventContent[256];
} CalendarEvent;
 
 
namespace OHOS {
UIViewGroup* CreateCalendarView(CalendarEvent *event);
void FreeCalendarView(void);
 
class CalendarViewEventListener : public OHOS::UIView::OnClickListener,
                               public OHOS::UIView::OnLongPressListener,
                               public OHOS::UIView::OnTouchListener,
                               public OHOS::UIView::OnDragListener {
public:
    CalendarViewEventListener();
    virtual ~CalendarViewEventListener();
    static CalendarViewEventListener *GetInstance(void);
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragEnd(OHOS::UIView& view, const OHOS::DragEvent& event) override;
    bool OnClick(OHOS::UIView& view, const OHOS::ClickEvent& event) override;
    bool OnPress(OHOS::UIView& view, const OHOS::PressEvent& event) override
    {
        return true;
    }
    bool OnLongPress(OHOS::UIView& view, const OHOS::LongPressEvent& event) override
    {
        return true;
    }
    bool OnRelease(OHOS::UIView& view, const OHOS::ReleaseEvent& event) override
    {
        return true;
    }
 
    bool OnDragStart(OHOS::UIView& view, const OHOS::DragEvent& event) override
    {
        return true;
    }
};
}; // namespace OHOS
 
#endif /* CALENDAR_NOTIFICATION_H */