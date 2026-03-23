/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Alarm
 * Author:
 * Create: 2021-09-11
 */


#include "alarm/AlarmPresenter.h"
#include "alarm/MainAlarmView.h"
#include "NativeRegisterManager.h"
#include "ChangeSliceListener.h"
#include "string"
#include "iostream"
#include "sstream"

namespace OHOS {

REGIST_MENU(VIEW_MAIN_ALARM, MainAlarmView, AlarmPresenter, PNG_APPLIST_ALARM_IMAGE, PNG_APPLIST_DEFAULT_IMG, "闹钟");

static AlarmPresenter *g_pAlarmPresenter = nullptr;

constexpr int16_t CLOSE_SWITCHES = 0;
constexpr int16_t OPEN_SWITCHES = 1;
constexpr int16_t SET_ZERO = 0;
constexpr int16_t NEW_TIME_LISTENER = 0;
constexpr int16_t SET_TIME_LISTENER = 1;
constexpr uint16_t THE_FIRST_ALARM = 0;
constexpr uint16_t SECOND_ALARM = 1;
constexpr uint16_t THIRD_ALARM = 2;
constexpr uint16_t FOURTH_ALARM = 3;
constexpr uint16_t FIVE_ALARM = 4;
constexpr uint16_t SIX_ALARM = 5;
constexpr uint16_t UNUSED_NUM = 6;
constexpr int16_t FIRST_ALARM = 0;
constexpr uint8_t WEEK_BIT6 = 6;
constexpr uint8_t WEEK_BIT5 = 5;
constexpr uint8_t WEEK_BIT4 = 4;
constexpr uint8_t WEEK_BIT3 = 3;
constexpr uint8_t WEEK_BIT2 = 2;
constexpr uint8_t WEEK_BIT1 = 1;
constexpr uint8_t WEEK_BIT0 = 0;
constexpr uint16_t ALARM_MAX_NUM = 4;
constexpr int32_t BUTTON_MAX_NUM = 7;
constexpr uint16_t CLOCK_NUM = 5;
constexpr uint8_t CANCEL_DATA = 0;
constexpr uint8_t SELECT_DATA = 1;
constexpr uint8_t SLIDING_LENGTH = 30;

AlarmPresenter::AlarmPresenter()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter new");
    g_pAlarmPresenter = this;
}

AlarmPresenter::~AlarmPresenter()
{
    g_pAlarmPresenter = nullptr;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter::~AlarmPresenter");
}

ColorType AlarmPresenter::ShadowBlue()
{
    return Color::GetColorFromRGB(0x17, 0x8F, 0xF8);
}

AlarmPresenter *AlarmPresenter::GetInstance()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter GetInstance");
    return g_pAlarmPresenter;
}

void AlarmPresenter::OnTimePickerStoped(UITimePicker &picker)
{
    std::string hour(picker.GetSelectHour());
    std::string minute(picker.GetSelectMinute());
    timeHour = stoi(hour);
    timeMin = stoi(minute);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter::OnTimePickerStoped the hour is %u,and the minute is %u", timeHour, timeMin);
}

void AlarmPresenter::OnTimePickerInaction(uint32_t hour, uint32_t min)
{
    timeHour = hour;
    timeMin = min;
}

void AlarmPresenter::PlayRing(void *arg)
{
    UNUSED(arg);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainAlarmView::playRing");
}

int16_t AlarmPresenter::GetInterStatus(void)
{
    return interListener;
}

void AlarmPresenter::ClickSetTime(uint16_t num)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter VIEW_ALARM_SET_TIME");
    UNUSED(num);
    uint32_t clockTime;
    if (!(AlarmClockModel::GetInstance()->GetClockTime(alarmListener, &clockTime))) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter ClickSetTime error");
        return;
    }
    MainAlarmView::GetInstance()->ChangeView(CHANGE_INTERFACE_CONFIG, CHANGE_INTERFACE_SET_TIME);
    if (MainAlarmView::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPresenter::ClickSetTime new MainAlarmView::GetInstance() error");
        return;
    }
    AlarmSetTimeView::GetInstance()->RefreshSetTime(clockTime);
    if (AlarmSetTimeView::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPresenter::ClickSetTime new AlarmSetTimeView::GetInstance() error");
        return;
    }
}

void AlarmPresenter::ClickSetWeek(uint16_t num)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter VIEW_ALARM_SET_WEEK");
    UNUSED(num);
    uint8_t clockWeek;
    if (!(AlarmClockModel::GetInstance()->GetClockWeek(alarmListener, &clockWeek))) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter ClickSetWeek error");
        return;
    }
    interListener = SET_TIME_LISTENER;
    MainAlarmView::GetInstance()->ChangeView(CHANGE_INTERFACE_CONFIG, CHANGE_INTERFACE_SET_WEEK);
    if (MainAlarmView::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPresenter::ClickSetWeek new MainAlarmView::GetInstance() error");
        return;
    }
    memset_s(alarmWeek, sizeof(alarmWeek), 0, sizeof(alarmWeek));
    AlarmSetWeekView::GetInstance()->RefreshSetWeek1();
    if (AlarmSetWeekView::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPresenter::ClickSetWeek new AlarmSetWeekView::GetInstance() error");
        return;
    }
    AlarmSetWeekView::GetInstance()->RefreshSetWeek2(clockWeek);
}

void AlarmPresenter::ClickAlarmClockDelete(uint16_t num)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter ClickAlarmClockDelete");
    UNUSED(num);
    MainAlarmView::GetInstance()->ChangeView(CHANGE_INTERFACE_CONFIG, CHANGE_INTERFACE_JUDGE);
    if (MainAlarmView::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPresenter::ClickAlarmClockDelete new MainAlarmView::GetInstance() error");
        return;
    }
}

void AlarmPresenter::ClickJudgeDelete(uint16_t num)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter JUDGE_DELETE");
    UNUSED(num);
    MainAlarmView::GetInstance()->ChangeView(CHANGE_INTERFACE_JUDGE, MAIN_INTERFACE);
    if (MainAlarmView::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPresenter::ClickJudgeDelete new MainAlarmView::GetInstance() error");
        return;
    }
}

void AlarmPresenter::ClickJudgeTrue(uint16_t num)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter JUDGE_DELETE");
    UNUSED(num);
    uint32_t clockTime;
    AlarmClockModel::GetInstance()->DeleteClockModel(alarmListener);
    if (MainAlarmView::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPresenter::ClickJudgeTrue new MainAlarmView::GetInstance() error");
        return;
    }
    MainAlarmView::GetInstance()->RefreshAlarm();
    clockQty--;
    for (uint16_t alarmNum = 0; alarmNum < clockQty; alarmNum++) {
        if (!(AlarmClockModel::GetInstance()->GetClockTime(alarmNum, &clockTime))) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter ClickJudgeTrue error");
            return;
        }
        MainAlarmView::GetInstance()->RefreshAlarm();
    }
    AlarmClockModel::GetInstance()->SetSerialRdc();
    MainAlarmView::GetInstance()->ChangeView(CHANGE_INTERFACE_JUDGE, MAIN_INTERFACE);
}

void AlarmPresenter::ClickAlarmTimeOk(uint16_t num)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter ALARM_OK");
    UNUSED(num);
    AlarmClockModel::GetInstance()->SetAlarmTime(timeHour, timeMin, alarmListener);
    uint32_t clockTime = ((timeHour << 16) | timeMin); // 闹钟时间小时的存储位需要右移16位
    uint8_t clockWeek;
    if (!(AlarmClockModel::GetInstance()->GetClockWeek(alarmListener, &clockWeek))) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter ClickAlarmTimeOk error");
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter time = %u", clockTime);
    AlarmClockModel::GetInstance()->DelAlarmDelay(alarmListener);

    if (MainAlarmView::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPresenter::ClickAlarmTimeOk new MainAlarmView::GetInstance() error");
        return;
    }
    AlarmClockModel::GetInstance()->ButtonStatusRing(alarmListener, true);
    MainAlarmView::GetInstance()->RefreshAlarm();
    if (AlarmConfigView::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPresenter::ClickAlarmTimeOk new AlarmConfigView::GetInstance() error");
        return;
    }
    AlarmConfigView::GetInstance()->RefreshConfig(clockTime, clockWeek, UNUSED_NUM);
    MainAlarmView::GetInstance()->ChangeView(CHANGE_INTERFACE_SET_TIME, CHANGE_INTERFACE_CONFIG);
}

void AlarmPresenter::ClickAlarmClock(uint16_t num)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter AlarmClock1");
    clockQty = AlarmClockModel::GetInstance()->GetAlarmNum();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "------AlarmPresenter alarmNumbel = %u", clockQty);
    setListener = SET_TIME_LISTENER;
    alarmListener = num;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter alarmListener = %u", alarmListener);
    uint32_t clockTime;
    uint8_t clockWeek;
    if (!(AlarmClockModel::GetInstance()->GetClockTime(alarmListener, &clockTime))) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter ClickAlarmClock error");
        return;
    }
    if (!(AlarmClockModel::GetInstance()->GetClockWeek(alarmListener, &clockWeek))) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter ClickAlarmClock error");
        return;
    }
    if (MainAlarmView::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPresenter::ClickAlarmClock new MainAlarmView::GetInstance() error");
        return;
    }
    MainAlarmView::GetInstance()->ChangeView(MAIN_INTERFACE, CHANGE_INTERFACE_CONFIG);
    if (AlarmConfigView::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPresenter::ClickAlarmClock new AlarmConfigView::GetInstance() error");
        return;
    }
    AlarmConfigView::GetInstance()->RefreshConfig(clockTime, clockWeek, num);
}

void AlarmPresenter::ClickAlarmWeekOk(uint16_t num)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter ALARM_WEEK_OK");
    UNUSED(num);
    uint32_t clockTime;
    uint8_t clockWeek = ((alarmWeek[WEEK_BIT6] << WEEK_BIT6) | (alarmWeek[WEEK_BIT5] << WEEK_BIT5) | (alarmWeek[WEEK_BIT4] << WEEK_BIT4) |
     (alarmWeek[WEEK_BIT3] << WEEK_BIT3) | (alarmWeek[WEEK_BIT2] << WEEK_BIT2) | (alarmWeek[WEEK_BIT1] << WEEK_BIT1) | alarmWeek[WEEK_BIT0]);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ClickAlarmWeekOk setListener = %u!", setListener);
    
    if (setListener == SET_TIME_LISTENER) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ClickAlarmWeekOk set time!");
        if (!(AlarmClockModel::GetInstance()->GetClockTime(alarmListener, &clockTime))) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter ClickAlarmWeekOk error");
            return;
        }
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ClickAlarmWeekOk  time = %d", clockTime);
        AlarmClockModel::GetInstance()->SetAlarmWeek(clockWeek, alarmListener);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ClickAlarmWeekOk  week = %u", clockWeek);
        AlarmConfigView::GetInstance()->RefreshConfig(clockTime, clockWeek, UNUSED_NUM);
        if (MainAlarmView::GetInstance() == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPresenter::ClickAlarmWeekOk new MainAlarmView::GetInstance() error");
            return;
        }
        AlarmClockModel::GetInstance()->ButtonStatusRing(alarmListener, true);
        MainAlarmView::GetInstance()->RefreshAlarm();
        MainAlarmView::GetInstance()->ChangeView(CHANGE_INTERFACE_SET_WEEK, CHANGE_INTERFACE_CONFIG);
    } else if (setListener == NEW_TIME_LISTENER) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ClickAlarmWeekOk new time!");
        clockQty++;
        alarmListener = clockQty;
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "******************ClickAlarmWeekOk  alarmListener = %d***************", alarmListener);
        AlarmClockModel::GetInstance()->SetSerialAdd();
        AlarmClockModel::GetInstance()->SetAlarmInfo(timeHour, timeMin, clockWeek, alarmListener, setListener);
        if (!(AlarmClockModel::GetInstance()->GetClockTime(alarmListener, &clockTime))) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter ClickAlarmWeekOk error");
            return;
        }
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ClickAlarmWeekOk  time = %d", clockTime);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ClickAlarmWeekOk  week = %u", clockWeek);
        if (MainAlarmView::GetInstance() == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPresenter::ClickAlarmWeekOk new MainAlarmView::GetInstance() error");
            return;
        }
        MainAlarmView::GetInstance()->RefreshAlarm();
        MainAlarmView::GetInstance()->ChangeView(CHANGE_INTERFACE_SET_WEEK, MAIN_INTERFACE);
    }
}

void AlarmPresenter::ClickAlarmNewTime(uint16_t num)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter ALARM_NEW_TIME");
    clockQty = AlarmClockModel::GetInstance()->GetAlarmNum();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter alarmNumbel = %u", clockQty);
    UNUSED(num);
    if (clockQty >= ALARM_MAX_NUM) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter new alarm max");
        MainAlarmView::GetInstance()->ChangeView(MAIN_INTERFACE, CHANGE_INTERFACE_MAXCLOCK);
        return;
    }
    setListener = NEW_TIME_LISTENER;
    timeHour = 0;
    timeMin = 0;
    if (MainAlarmView::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPresenter::ClickAlarmNewTime new MainAlarmView::GetInstance() error");
        return;
    }
    MainAlarmView::GetInstance()->ChangeView(MAIN_INTERFACE, CHANGE_INTERFACE_NEW_TIME);
    if (AlarmNewTimeView::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPresenter::ClickAlarmNewTime new AlarmNewTimeView::GetInstance() error");
        return;
    }
    AlarmNewTimeView::GetInstance()->RefreshNewTime();
}

void AlarmPresenter::ClickAlarmNext(uint16_t num)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter ALARM_NEXT");
    UNUSED(num);
    for (int i = 0; i < BUTTON_MAX_NUM; i++) {
        alarmWeek[i] = 0;
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "alarmWeek[%d] = %u", i, alarmWeek[i]);
    }
    interListener = NEW_TIME_LISTENER;
    if (MainAlarmView::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPresenter::ClickAlarmNext new MainAlarmView::GetInstance() error");
        return;
    }
    MainAlarmView::GetInstance()->ChangeView(CHANGE_INTERFACE_NEW_TIME, CHANGE_INTERFACE_SET_WEEK);
    if (AlarmNewTimeView::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPresenter::ClickAlarmNext new AlarmNewTimeView::GetInstance() error");
        return;
    }
    if (AlarmSetWeekView::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPresenter::ClickAlarmNext new AlarmSetWeekView::GetInstance() error");
        return;
    }
    AlarmSetWeekView::GetInstance()->RefreshSetWeek1();
}

bool AlarmPresenter::OnDrag(UIView& view, const DragEvent& event)
{
    if (event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) {
        int16_t deltaX;
        int16_t deltaY;
        deltaX = event.GetDeltaX();
        deltaY = event.GetDeltaY();
        if (MATH_ABS(deltaX) >= MATH_ABS(deltaY) && MATH_ABS(deltaX) > SLIDING_LENGTH) {
            MainAlarmView::GetInstance()->DropView(view);
        }
    }
    return true;
}

struct AlarmPresentMapper1 {
    const char *viewId;
    void (AlarmPresenter::*func)(uint16_t);
};

static const AlarmPresentMapper1 PresentMapper[] = {
    {SET_TIME, &AlarmPresenter::ClickSetTime},
    {SET_WEEK, &AlarmPresenter::ClickSetWeek},
    {ALARM_CLOCK_DELETE, &AlarmPresenter::ClickAlarmClockDelete},
    {JUDGE_DELETE, &AlarmPresenter::ClickJudgeDelete},
    {JUDGE_TRUE, &AlarmPresenter::ClickJudgeTrue},
    {ALARM_OK, &AlarmPresenter::ClickAlarmTimeOk},
    {ADD_BUTTON, &AlarmPresenter::ClickAlarmNewTime},
    {ALARM_NEXT, &AlarmPresenter::ClickAlarmNext},
    {ALARM_WEEK_OK, &AlarmPresenter::ClickAlarmWeekOk},
    {ALARM_CLOCK[THE_FIRST_ALARM], &AlarmPresenter::ClickAlarmClock},
    {ALARM_CLOCK[SECOND_ALARM], &AlarmPresenter::ClickAlarmClock},
    {ALARM_CLOCK[THIRD_ALARM], &AlarmPresenter::ClickAlarmClock},
    {ALARM_CLOCK[FOURTH_ALARM], &AlarmPresenter::ClickAlarmClock},
    {ALARM_CLOCK[FIVE_ALARM], &AlarmPresenter::ClickAlarmClock},
    {ALARM_CLOCK[SIX_ALARM], &AlarmPresenter::ClickAlarmClock},
};

bool AlarmPresenter::ClickFuncId(const char *buttonName)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "buttonName = %s", buttonName);
    uint16_t num;
    for (uint16_t i = 0; i < sizeof(PresentMapper) / sizeof(PresentMapper[0]); i++) {
        for (num = 0; num < CLOCK_NUM; num++) {
            if (strcmp(buttonName, ALARM_CLOCK[num]) == 0) {
                break;
            }
        }
        if (strcmp(buttonName, PresentMapper[i].viewId) == 0) {
            (this->*(PresentMapper[i].func))(num);
            return true;
        }
    }
    return false;
}

bool AlarmPresenter::ClickFuncView(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ClickFuncView view = %s", view.GetViewId());
    for (int num = 0; num < BUTTON_MAX_NUM; num++) {
        if (strcmp(view.GetViewId(), WEEK_CONTAINER[num]) == 0) {
            alarmWeek[num] = alarmWeek[num] ? CANCEL_DATA: SELECT_DATA;
            if (alarmWeek[num] == CANCEL_DATA) {
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "alarmWeek[num] = %u", alarmWeek[num]);
                view.SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
                dynamic_cast<UIButton &>(view).SetStyleForState(STYLE_BACKGROUND_COLOR,
                                                                Color::Black().full, UIButton::PRESSED);
            } else if (alarmWeek[num] == SELECT_DATA) {
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "alarmWeek[num] = %u", alarmWeek[num]);
                view.SetStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(0x6c, 0xB8, 0xFD).full);
                dynamic_cast<UIButton &>(view).SetStyleForState(STYLE_BACKGROUND_COLOR,
                                                                Color::GetColorFromRGB(0x6c, 0xB8, 0xFD).full, UIButton::PRESSED);
            }
        }
    }
    view.Invalidate();
    return true;
}

bool AlarmPresenter::OnClick(UIView &view, const ClickEvent &event)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPresenter::OnClick ");
    bool jug = ClickFuncId(view.GetViewId());
    if (!jug) {
        ClickFuncView(view);
    }
    return true;
}

bool AlarmStatusBtnChangeListener::OnChange(UICheckBox::UICheckBoxState state)
{
    if (MainAlarmView::GetInstance()->GetRefreshStatus()) {
        return true;
    }
    switch (state) {
        case UICheckBox::SELECTED:
            AlarmClockModel::GetInstance()->ButtonStatusRing(alarmStatusNum, true);
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmStatusBtnChangeListener::OnChange true");
            break;

        case UICheckBox::UNSELECTED:
            AlarmClockModel::GetInstance()->ButtonStatusRing(alarmStatusNum, false);
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmStatusBtnChangeListener::OnChange false");
            break;
        default:
            break;
    }
    return true;
}

}