/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Alarm
 * Author:
 * Create: 2021-09-11
 */

#include "alarm/MainAlarmView.h"
#include "UiConfig.h"
#include "ui_label_button.h"
#include "phoneservice/PhoneService.h"

namespace OHOS {
static MainAlarmView *g_MainAlarmView = nullptr;

constexpr int16_t SET_ZERO = 0;
constexpr int16_t SET_ONE = 1;
constexpr int16_t SET_TWO = 2;
constexpr int16_t SET_THREE = 3;
constexpr int16_t SET_FOUR = 4;
constexpr int16_t SET_FIVE = 5;
constexpr int16_t INTERFACES_MAX = 6;
constexpr int16_t CONTAINER_X = 0;
constexpr int16_t CONTAINER_Y = 0;
constexpr int16_t ALARM_CLOCK_WIDTH = 250;
constexpr int16_t ALARM_CLOCK_HEIGHT = 60;
constexpr int16_t LABEL_HEIGHT = 30;
constexpr int16_t WEEK_LABEL_HEIGHT = 85;
constexpr int16_t ADD_BUTTON_X = 182;
constexpr int16_t ADD_BUTTON_Y = 15;
constexpr int16_t ADD_BUTTON_W = 100;
constexpr int16_t ADD_BUTTON_H = 100;
constexpr int16_t ALARM_ONE_Y = 360;
constexpr int16_t ALARM_TWO_Y = 510;
constexpr int16_t ALARM_THREE_Y = 660;
constexpr int16_t ALARM_FOUR_Y = 810;
constexpr int16_t LABEL_ONE_Y = 420;
constexpr int16_t LABEL_TWO_Y = 570;
constexpr int16_t LABEL_THREE_Y = 720;
constexpr int16_t LABEL_FOUR_Y = 870;
constexpr int16_t STATUS_BUTTON_X = 320;
constexpr int16_t STATUS_BUTTON_Y = 210;
constexpr int16_t STATUS_BUTTON_W = 90;
constexpr int16_t STATUS_BUTTON_H = 90;
constexpr int16_t D_CLOCK_X = 80;
constexpr int16_t D_CLOCK_FONT_SIZE = 55;
constexpr int16_t D_CLOCK_LABEL_FONT = 25;
constexpr int16_t POSITION_ADD_Y = 110;
constexpr int16_t POSITION_BUTTON_ADD_Y = 85;
constexpr int16_t LABEL_SEMITRANSPARENT = 128;
constexpr uint16_t FIRST_CLOCK = 0;
constexpr uint32_t ALARM_CLOCK_NO_MODEL = 0;
constexpr int16_t CLOCK_MAX_NUM = 5;
constexpr uint8_t MAX_HOUR = 24;

MainAlarmView::MainAlarmView()
{
    g_MainAlarmView = this;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainAlarmView::MainAlarmView");
}

MainAlarmView::~MainAlarmView()
{
    alarmHour = 0;
    alarmMin = 0;
    dClockNum = 0;
    weekStatus = 0;
    group->RemoveAll();
    delete addButton;
    addButton = nullptr;
    delete group;
    group = nullptr;
    delete configView;
    configView = nullptr;
    delete setTimeView;
    setTimeView = nullptr;
    delete setWeekView;
    setWeekView = nullptr;
    delete judgeView;
    judgeView = nullptr;
    delete newTimeView;
    newTimeView = nullptr;
    for (int alarmNum = SET_ZERO; alarmNum <= SET_FIVE; alarmNum++) {
        if (dClock[alarmNum] != nullptr) {
            delete dClock[alarmNum];
            dClock[alarmNum] = nullptr;
        }
        if (statusButton[alarmNum] != nullptr) {
            delete statusButton[alarmNum];
            statusButton[alarmNum] = nullptr;
        }
        if (dLabel[alarmNum] != nullptr) {
            delete dLabel[alarmNum];
            dLabel[alarmNum] = nullptr;
        }
        container[alarmNum] = nullptr;
    }
    g_MainAlarmView = nullptr;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainAlarmView::~MainAlarmView");
}

MainAlarmView *MainAlarmView::GetInstance(void)
{
    return g_MainAlarmView;
}

void MainAlarmView::CreateImage(void)
{
    UIView::OnClickListener *clickListener = (UIView::OnClickListener *)OHOS::AlarmPresenter::GetInstance();

    addButton = new UIImageView();
    if (addButton == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainAlarmView:: new addButton fail");
        return;
    }
    addButton->SetPosition(ADD_BUTTON_X, ADD_BUTTON_Y);
    addButton->SetWidth(ADD_BUTTON_W);
    addButton->SetHeight(ADD_BUTTON_H);
    addButton->SetSrc(ALARM_ADD_BUTTON_PATH);
    addButton->SetViewId(ADD_BUTTON);
    addButton->SetTouchable(true);
    addButton->SetOnClickListener(clickListener);
    group->Add(addButton);
}

void MainAlarmView::NewClockCtl(int16_t alarmNum)
{
    UIView::OnClickListener *clockListener = (UIView::OnClickListener *)OHOS::AlarmPresenter::GetInstance();
    dClock[alarmNum] = new UIDigitalClock();
    if (dClock[alarmNum] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainAlarmView:: new dClock fail");
        return;
    }
    dClock[alarmNum]->SetViewId(ALARM_CLOCK[alarmNum]);
    dClock[alarmNum]->SetTouchable(true);
    dClock[alarmNum]->SetOnClickListener(clockListener);
    if (alarmNum == 0) {
        dClock[0]->SetPosition(D_CLOCK_X, alarmPositionY, ALARM_CLOCK_WIDTH, ALARM_CLOCK_HEIGHT);
    } else {
        dClock[alarmNum]->SetPosition(D_CLOCK_X, dClock[0]->GetY() + (alarmInterval * alarmNum), ALARM_CLOCK_WIDTH, ALARM_CLOCK_HEIGHT);
    }
    dClock[alarmNum]->SetFont(DEFAULT_VECTOR_FONT_FILENAME, D_CLOCK_FONT_SIZE);
    dClock[alarmNum]->SetDisplayMode(UIDigitalClock::DISPLAY_24_HOUR_NO_SECONDS);
    group->Add(dClock[alarmNum]);

    dLabel[alarmNum] = new UILabel();
    if (dLabel[alarmNum] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainAlarmView:: new dLabel fail");
        return;
    }
    if (alarmNum == 0) {
        dLabel[0]->SetPosition(D_CLOCK_X, labelPositionY, ALARM_CLOCK_WIDTH, WEEK_LABEL_HEIGHT);
    } else {
        dLabel[alarmNum]->SetPosition(D_CLOCK_X, dLabel[0]->GetY() + (alarmInterval * alarmNum), ALARM_CLOCK_WIDTH, WEEK_LABEL_HEIGHT);
    }
    group->Add(dLabel[alarmNum]);

    statusButton[alarmNum] = new UIToggleButton();
    if (statusButton[alarmNum] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainAlarmView:: new statusButton[alarmNum] fail");
        return;
    }
    statusButton[alarmNum]->SetViewId(ALARM_STATUS[alarmNum]);
    if (alarmNum == 0) {
        statusButton[0]->SetPosition(STATUS_BUTTON_X, alarmPositionY, STATUS_BUTTON_W, STATUS_BUTTON_H);
    } else {
        statusButton[alarmNum]->SetPosition(STATUS_BUTTON_X, statusButton[0]->GetY() + (alarmInterval * alarmNum), STATUS_BUTTON_W, STATUS_BUTTON_H);
    }
    statusButton[alarmNum]->SetStyle(STYLE_BACKGROUND_OPA, alarmNum);
    UICheckBox::OnChangeListener *statusListener{nullptr};
    statusListener = new AlarmStatusBtnChangeListener(reinterpret_cast<UIView*>(statusButton[alarmNum]));
    if (statusListener == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainAlarmView::NewStatusButton statusListener nullptr!");
        return;
    }
    statusButton[alarmNum]->SetOnChangeListener((UICheckBox::OnChangeListener *)statusListener);
    group->Add(statusButton[alarmNum]);
}

void MainAlarmView::LabelGetWeek(uint8_t week, int16_t alarmNum)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainAlarmView::week = %u!", week);
    std::string weekStr;
    if ((week & 0x7f) == 0x7f) { // 判断0~6位是否都为1，如若都为1，则一周7天闹钟都响应
        weekStr = EVERY_DAY;
    } else if (week == 0) { // 判断0~6位是否都为0，如若都为0，则一周7天都不响应，闹钟为不重复开启
        weekStr = DO_NOT_REPEAT;
    } else {
        if ((week & 0x02) == 0x02) { // 判断第1位是否为1，如若为1，则星期一响应
            weekStr += MON_DAY;
        }
        if ((week & 0x04) == 0x04) { // 判断第2位是否为1，如若为1，则星期二响应
            weekStr += TUE_DAY;
        }
        if ((week & 0x08) == 0x08) { // 判断第3位是否为1，如若为1，则星期三响应
            weekStr += WED_DAY;
        }
        if ((week & 0x10) == 0x10) { // 判断第4位是否为1，如若为1，则星期四响应
            weekStr += THUR_DAY;
        }
        if ((week & 0x20) == 0x20) { // 判断第5位是否为1，如若为1，则星期五响应
            weekStr += FRI_DAY;
        }
        if ((week & 0x40) == 0x40) { // 判断第6位是否为1，如若为1，则星期六响应
            weekStr += SAT_DAY;
        }
        if ((week & 0x01) == 0x01) { // 判断第0位是否为1，如若为1，则星期日响应
            weekStr += SUN_DAY;
        }
    }
    dLabel[alarmNum]->SetFont(DEFAULT_VECTOR_FONT_FILENAME, D_CLOCK_LABEL_FONT);
    dLabel[alarmNum]->SetText(weekStr.c_str());
    dLabel[alarmNum]->SetStyle(STYLE_TEXT_OPA, LABEL_SEMITRANSPARENT);
}

bool MainAlarmView::GetRefreshStatus(void)
{
    return reflashStatus;
}

void MainAlarmView::RefreshAlarmStatus(uint8_t alarmEnable, int16_t num)
{
    reflashStatus = true;
    if (alarmEnable == 0) {
        statusButton[num]->SetState(false);
    } else if (alarmEnable == 1) {
        statusButton[num]->SetState(true);
    }
    reflashStatus = false;
    dClock[num]->SetVisible(true);
    statusButton[num]->SetVisible(true);
}

void MainAlarmView::OnStart()
{

    group = new UIScrollView();
    if (group == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainAlarmView:: new group fail");
        return;
    }
    group->SetPosition(CONTAINER_X, CONTAINER_Y, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    UIView::OnClickListener *clickListener = (UIView::OnClickListener *)OHOS::AlarmPresenter::GetInstance();
    CreateImage();

    group->SetTouchable(true);
    group->SetDraggable(true);
    UIView::OnDragListener *alarmDragListener = (UIView::OnDragListener *)OHOS::AlarmPresenter::GetInstance();
    if (alarmDragListener == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView:: new alarmDragListener fail");
        return;
    }
    group->SetOnDragListener(alarmDragListener);
    RefreshAlarm();
    if (group != nullptr) {
        container[0] = group;
        AddViewToRootContainer(container[0]);
    }
}

void MainAlarmView::RefreshAlarm(void)
{
    uint32_t alarmTime = 0;
    uint8_t alarmWeek = 0;
    uint8_t alarmEnable = 0;
    bool alarmStatus;
    int16_t i;
    for (i = 0; i < CLOCK_MAX_NUM; i++) {
        alarmStatus = AlarmClockModel::GetInstance()->GetClockTime(i, &alarmTime);
        if (!alarmStatus) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainAlarmView:: GetClockTime fail! i = %d", i);
            break;
        }
        alarmStatus = AlarmClockModel::GetInstance()->GetClockWeek(i, &alarmWeek);
        if (!alarmStatus) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainAlarmView:: GetClockWeek fail! i = %d", i);
            break;
        }
        alarmStatus = AlarmClockModel::GetInstance()->GetClockStatus(i, &alarmEnable);
        if (!alarmStatus) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainAlarmView:: GetClockStatus fail! i = %d", i);
            break;
        }
        uint8_t refreshHour = alarmTime >> 16; // 闹钟存储小时位需要右移16位
        if (refreshHour > MAX_HOUR) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainAlarmView:: Hour > 24 error!");
            return;
        }
        uint8_t refreshMin = alarmTime & 0x00FF; // 得出闹钟时间的分钟位需要将左置位16位清零
        if (i > dClockNum) {
            NewClockCtl(i);
        }
        LabelGetWeek(alarmWeek, i);
        RefreshAlarmStatus(alarmEnable, i);
        dClock[i]->SetTime24Hour(refreshHour, refreshMin, 0);
    }
    group->ScrollBy(0, ADD_BUTTON_Y - addButton->GetY());
    if (dClockNum > (i - 1)) {
        group->Remove(dClock[dClockNum]);
        delete dClock[dClockNum];
        dClock[dClockNum] = nullptr;

        group->Remove(statusButton[dClockNum]);
        delete statusButton[dClockNum];
        statusButton[dClockNum] = nullptr;

        group->Remove(dLabel[dClockNum]);
        delete dLabel[dClockNum];
        dLabel[dClockNum] = nullptr;
    }
    dClockNum = i - 1;
}

bool MainAlarmView::ChangeMainAlarm(int16_t thisView)
{
    if (container[thisView] == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainAlarmView::MainAlarmView");
        if (group == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainAlarmView:: new container[%d] fail", thisView);
            return false;
        }
        AddViewToRootContainer(container[thisView]);
    }
    return true;
}

bool MainAlarmView::ChangeConfig(int16_t thisView)
{
    if (container[thisView] == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainAlarmView::AlarmConfigView");
        configView = new AlarmConfigView();
        if (configView == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainAlarmView:: new container[%d] fail", thisView);
            return false;
        }
        container[thisView] = configView->InitConfigView();
        AddViewToRootContainer(container[thisView]);
    }
    return true;
}

bool MainAlarmView::ChangeSetTime(int16_t thisView)
{
    if (container[thisView] == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainAlarmView::AlarmSetTimeView");
        setTimeView = new AlarmSetTimeView();
        if (setTimeView == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainAlarmView:: new container[%d] fail", thisView);
            return false;
        }
        container[thisView] = setTimeView->InitSetTimeView();
        AddViewToRootContainer(container[thisView]);
    }

    return true;
}

bool MainAlarmView::ChangeSetWeek(int16_t thisView)
{
    if (container[thisView] == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainAlarmView::AlarmSetWeekView");
        setWeekView = new AlarmSetWeekView();
        if (setWeekView == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainAlarmView:: new container[%d] fail", thisView);
            return false;
        }
        container[thisView] = setWeekView->InitSetWeekView();
        AddViewToRootContainer(container[thisView]);
    }
    return true;
}

bool MainAlarmView::ChangeJudge(int16_t thisView)
{
    if (container[thisView] == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainAlarmView::AlarmSetWeekView");
        judgeView = new AlarmJudgeView();
        if (judgeView == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainAlarmView:: new container[%d] fail", thisView);
            return false;
        }
        container[thisView] = judgeView->InitJudgeView();
        AddViewToRootContainer(container[thisView]);
    }
    return true;
}

bool MainAlarmView::ChangeNewTime(int16_t thisView)
{
    if (container[thisView] == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainAlarmView::AlarmNewTimeView");
        newTimeView = new AlarmNewTimeView();
        if (newTimeView == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainAlarmView:: new container[%d] fail", thisView);
            return false;
        }
        container[thisView] = newTimeView->InitNewTimeView();
        AddViewToRootContainer(container[thisView]);
    }
    return true;
}

bool MainAlarmView::ChangeMaxClock(int16_t thisView)
{
    if (container[thisView] == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainAlarmView::AlarmMaxClock");
        maxClockView = new AlarmMaxClock();
        if (maxClockView == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainAlarmView:: new container[%d] fail", thisView);
            return false;
        }
        container[thisView] = maxClockView->InitAlarmMax();
        AddViewToRootContainer(container[thisView]);
    }
    AlarmMaxClock::GetInstance()->AlarmMaxDelayExit();
    return true;
}

void MainAlarmView::DropView(UIView& view)
{
    if (&view == container[CHANGE_INTERFACE_CONFIG]) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainAlarmView::DropView::container[CHANGE_INTERFACE_CONFIG]");
        ChangeView(CHANGE_INTERFACE_CONFIG, MAIN_INTERFACE);
    } else if (&view == container[CHANGE_INTERFACE_SET_TIME]) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainAlarmView::DropView::container[CHANGE_INTERFACE_SET_TIME]");
        ChangeView(CHANGE_INTERFACE_SET_TIME, CHANGE_INTERFACE_CONFIG);
    } else if (&view == container[CHANGE_INTERFACE_SET_WEEK]) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainAlarmView::DropView::container[CHANGE_INTERFACE_SET_WEEK]");
        weekStatus = AlarmPresenter::GetInstance()->GetInterStatus();
        if (weekStatus == 1) {
            ChangeView(CHANGE_INTERFACE_SET_WEEK, CHANGE_INTERFACE_CONFIG);
        } else if (weekStatus == 0) {
            ChangeView(CHANGE_INTERFACE_SET_WEEK, CHANGE_INTERFACE_NEW_TIME);
        }
    } else if (&view == container[CHANGE_INTERFACE_NEW_TIME]) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainAlarmView::DropView::container[CHANGE_INTERFACE_NEW_TIME]");
        ChangeView(CHANGE_INTERFACE_NEW_TIME, MAIN_INTERFACE);
        if (AlarmNewTimeView::GetInstance() == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainAlarmView::DropView new AlarmNewTimeView::GetInstance() error");
            return;
        }
        AlarmNewTimeView::GetInstance()->RefreshNewTime();
    } else if (&view == container[CHANGE_INTERFACE_JUDGE]) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainAlarmView::DropView::container[CHANGE_INTERFACE_JUDGE]");
        ChangeView(CHANGE_INTERFACE_JUDGE, CHANGE_INTERFACE_CONFIG);
    } else if (&view == container[CHANGE_INTERFACE_MAXCLOCK]) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainAlarmView::DropView::container[CHANGE_INTERFACE_MAXCLOCK]");
        ChangeView(CHANGE_INTERFACE_MAXCLOCK, MAIN_INTERFACE);
        CancelScanHandle();
    } else if (&view == container[MAIN_INTERFACE]) {
        uint16_t preSlice = AlarmClockModel::GetInstance()->JudgePreSlice();
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainAlarmView::DropView::container[MAIN_INTERFACE]");
        NativeAbility::GetInstance().ChangeSlice(preSlice);
    }
}

struct AlarmViewMapper {
    const int16_t viewId;
    bool (MainAlarmView::*func)(int16_t thisView);
};

static const AlarmViewMapper ViewMapper[] = {
    {MAIN_INTERFACE, &MainAlarmView::ChangeMainAlarm},
    {CHANGE_INTERFACE_CONFIG, &MainAlarmView::ChangeConfig},
    {CHANGE_INTERFACE_SET_TIME, &MainAlarmView::ChangeSetTime},
    {CHANGE_INTERFACE_SET_WEEK, &MainAlarmView::ChangeSetWeek},
    {CHANGE_INTERFACE_JUDGE, &MainAlarmView::ChangeJudge},
    {CHANGE_INTERFACE_NEW_TIME, &MainAlarmView::ChangeNewTime},
    {CHANGE_INTERFACE_MAXCLOCK, &MainAlarmView::ChangeMaxClock},
};

bool MainAlarmView::ClickFunc(int16_t interf)
{
    int funcSize = sizeof(ViewMapper) / sizeof(ViewMapper[0]);
    for (int i = 0; i < funcSize; i++) {
        if (interf == ViewMapper[i].viewId) {
            bool jug = (this->*(ViewMapper[i].func))(interf);
            if (!jug) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainAlarmView:: ChangeInterFace[%d] false", interf);
                return false;
            }
            return true;
        }
    }
    return false;
}

void MainAlarmView::ChangeView(int16_t hideView, int16_t showView)
{
    if (hideView > INTERFACES_MAX) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainAlarmView:: ChangeSetTime Overstep ones bounds");
        return;
    }
    container[hideView]->SetVisible(false);
    ClickFunc(showView);
    container[showView]->SetVisible(true);
    Draw();
}
}