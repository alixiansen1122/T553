/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Alarm
 * Author:
 * Create: 2021-09-11
 */

#ifndef ALARM_PRESENTER_H
#define ALARM_PRESENTER_H

#include "Presenter.h"
#include "ChangeSliceListener.h"
#include "UiConfig.h"
#include "components/ui_label.h"
#include "common/screen.h"
#include "components/ui_label.h"
#include "components/root_view.h"
#include "components/ui_view_group.h"
#include "components/ui_checkbox.h"
#include "components/ui_image_view.h"
#include "gfx_utils/graphic_types.h"
#include "components/ui_button.h"
#include "components/ui_label_button.h"
#include "components/ui_time_picker.h"
#include "components/ui_digital_clock.h"
#include "gfx_utils/graphic_log.h"
#include "alarm/AlarmClockModel.h"
#include "wearable_log.h"
#include "color.h"

namespace OHOS {
constexpr char* EVERY_DAY = (char *)"每天";
constexpr char* DO_NOT_REPEAT = (char *)"不重复";

constexpr char* MON_DAY = (char *)"一 ";
constexpr char* TUE_DAY = (char *)"二 ";
constexpr char* WED_DAY = (char *)"三 ";
constexpr char* THUR_DAY = (char *)"四 ";
constexpr char* FRI_DAY = (char *)"五 ";
constexpr char* SAT_DAY = (char *)"六 ";
constexpr char* SUN_DAY = (char *)"日";

constexpr char* SET_TIME = (char *)"settime";
constexpr char* SET_WEEK = (char *)"setweek";
constexpr char* ALARM_CLOCK_DELETE = (char *)"alarmclockdelete";
constexpr char* ALARM_NEXT = (char *)"alarmnext";
constexpr char* ALARM_OK = (char *)"alarmok";
constexpr char* JUDGE_DELETE = (char *)"judgedelete";
constexpr char* JUDGE_TRUE = (char *)"judgetrue";
constexpr char* ADD_BUTTON = (char *)"addbutton";

constexpr char* MONDAY = (char *)"monday";
constexpr char* TUESDAY = (char *)"tuesday";
constexpr char* WEDNESDAY = (char *)"wednesday";
constexpr char* THURSDAY = (char *)"thursday";
constexpr char* FRIDAY = (char *)"friday";
constexpr char* SATURDAY = (char *)"saturday";
constexpr char* SUNDAY = (char *)"sunday";
constexpr char* ALARM_WEEK_OK = (char *)"alarmweekok";

constexpr char* ALARM_STATUS_ONE  = (char *)"alarmstatus0";
constexpr char* ALARM_STATUS_TWO  = (char *)"alarmstatus1";
constexpr char* ALARM_STATUS_THREE  = (char *)"alarmstatus2";
constexpr char* ALARM_STATUS_FOUR  = (char *)"alarmstatus3";
constexpr char* ALARM_STATUS_FIVE  = (char *)"alarmstatus4";
constexpr char* ALARM_STATUS_SIX  = (char *)"alarmstatus5";

constexpr char* ALARM_CLOCK_ONE = (char *)"alarmclock0";
constexpr char* ALARM_CLOCK_TWO = (char *)"alarmclock1";
constexpr char* ALARM_CLOCK_THREE = (char *)"alarmclock2";
constexpr char* ALARM_CLOCK_FOUR = (char *)"alarmclock3";
constexpr char* ALARM_CLOCK_FIVE = (char *)"alarmclock4";
constexpr char* ALARM_CLOCK_SIX = (char *)"alarmclock5";

constexpr int16_t SET_SIX = 6;

constexpr char* ALARM_STATUS[6] = {ALARM_STATUS_ONE, ALARM_STATUS_TWO, ALARM_STATUS_THREE, // 闹钟按钮最大个数为6个
    ALARM_STATUS_FOUR, ALARM_STATUS_FIVE, ALARM_STATUS_SIX};
constexpr char* ALARM_CLOCK[6] = {ALARM_CLOCK_ONE, ALARM_CLOCK_TWO, ALARM_CLOCK_THREE, // 闹钟个数最大个数为6个
    ALARM_CLOCK_FOUR, ALARM_CLOCK_FIVE, ALARM_CLOCK_SIX};
constexpr char* WEEK_CONTAINER[7] = {SUNDAY, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY}; // 日期按钮最大个数为7个

typedef enum {
    MAIN_INTERFACE = 0,
    CHANGE_INTERFACE_CONFIG,
    CHANGE_INTERFACE_SET_TIME,
    CHANGE_INTERFACE_SET_WEEK,
    CHANGE_INTERFACE_JUDGE,
    CHANGE_INTERFACE_NEW_TIME,
    CHANGE_INTERFACE_MAXCLOCK,
} InterfacesId;

class AlarmStatusBtnChangeListener : public OHOS::UICheckBox::OnChangeListener
{
public:
    explicit AlarmStatusBtnChangeListener(UIView *uiView)
    {
        std::string alarmStatusId = uiView->GetViewId();
        std::string alarmCmpId;
        for (int16_t num = 0; num < SET_SIX; num++) { // 闹钟按钮的最大个数为6个
            alarmCmpId = ALARM_STATUS[num];
            if (alarmStatusId == alarmCmpId) {
                alarmStatusNum = num;
            }
        }
    }

    ~AlarmStatusBtnChangeListener() {}
    bool OnChange(UICheckBox::UICheckBoxState state) override;
private:
    int16_t alarmStatusNum;
};

class MainAlarmView;
class AlarmPresenter : public Presenter<MainAlarmView>, public UIView::OnClickListener,
 public UITimePicker::SelectedListener, public UIView::OnDragListener {
public:
    AlarmPresenter();
    ~AlarmPresenter() override;
    static AlarmPresenter *GetInstance();
    static void PlayRing(void *arg);
    int16_t GetInterStatus(void);
    ColorType ShadowBlue(void);
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool ClickFuncId(const char *buttonName);
    bool ClickFuncView(UIView &view);
    void OnTimePickerStoped(UITimePicker &picker) override;
    void ClickBlue(UIView &view);
    void ClickBlack(UIView &view);
    void GetAlarmNum(uint16_t alarmNum);
    void ClickSetTime(uint16_t alarmNum);
    void ClickSetWeek(uint16_t alarmNum);
    void ClickAlarmClockDelete(uint16_t alarmNum);
    void ClickJudgeDelete(uint16_t alarmNum);
    void ClickJudgeTrue(uint16_t alarmNum);
    void ClickAlarmTimeOk(uint16_t alarmNum);
    void ClickAlarmClock(uint16_t alarmNum);
    void ClickAlarmWeekOk(uint16_t alarmNum);
    void ClickAlarmNewTime(uint16_t alarmNum);
    void ClickAlarmNext(uint16_t alarmNum);
    void OnTimePickerInaction(uint32_t hour, uint32_t min);
    uint8_t alarmWeek[7] = {1, 1, 1, 1, 1, 1, 1}; // 闹钟默认所有的日期都选中，因此初始值为1，7是指一个星期中七天
private:
    uint32_t timeHour{0};
    uint32_t timeMin{0};
    uint16_t setListener;
    uint16_t alarmListener{0};
    uint16_t clockQty{0};
    int16_t interListener;
    bool buttonStratus = true;
};
}
#endif