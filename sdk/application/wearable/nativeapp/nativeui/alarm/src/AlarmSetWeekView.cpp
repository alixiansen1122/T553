/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Alarm
 * Author:
 * Create: 2021-09-11
 */

#include "alarm/AlarmSetWeekView.h"
#include "common/screen.h"
#include "components/ui_digital_clock.h"

namespace OHOS {
#define MON "星期一"
#define TUE "星期二"
#define WED "星期三"
#define THUR "星期四"
#define FRI "星期五"
#define SAT "星期六"
#define SUN "星期天"
#define RESPEAT "重复"
#define SURE "确认"

#define MONDAY "monday"
#define TUESDAY "tuesday"
#define WEDNESDAY "wednesday"
#define THURSDAY "thursday"
#define FRIDAY "friday"
#define SATURDAY "saturday"
#define SUNDAY "sunday"

static AlarmSetWeekView *g_pAlarmSetWeekView = nullptr;
constexpr int16_t CONTAINER_X = 0;
constexpr int16_t CONTAINER_Y = 0;
constexpr int16_t DEFAULT_FONT = 40;
constexpr int16_t REBOUND_SIZE = 50;
constexpr int16_t BORDER_WIDTH = 20;
constexpr int16_t SET_ZERO = 0;
constexpr int16_t TITLE_X = 185;
constexpr int16_t TITLE_Y = 30;
constexpr int16_t LABEL_POSITION_SIZE = 150;
constexpr int16_t VIEW_BORDER_SIZE = 2;
constexpr int16_t BORDER_RADIUS_SIZE = 200;
constexpr int16_t VIEW_BUTTON_X = 100;
constexpr int16_t VIEW_BUTTON_Y = 70;
constexpr int16_t BUTTON_FONT_SIZE = 18;
constexpr int16_t TIME_LABEL_BUTTON_X = 63;
constexpr int16_t TIME_LABEL_BUTTON_Y = 360;
constexpr int16_t BUTTON_WIDHT3 = 320;
constexpr int16_t BUTTON_HEIGHT3 = 101;
constexpr int16_t LABEL_WEEK_FONT = 40;
constexpr int16_t BUTTON_WIDHT = 100;
constexpr int16_t BUTTON_HEIGHT = 60;
constexpr int16_t BUTTON_FONT = 20;
constexpr int16_t VIEW_POSITION_X = 60;
constexpr int16_t VIEW_POSITION_X2 = 12;
constexpr int16_t WEEK_Y1 = 130;
constexpr int16_t WEEK_Y2 = 210;
constexpr int16_t WEEK_GAP_X = 112;
constexpr int16_t BUTTON_STYLE_BORDER_RADIUS_VALUE = 30;
constexpr int16_t FONT_DEFAULT_SIZE = 30;
constexpr int16_t WEEK_MON = 1;
constexpr int16_t WEEK_TUE = 2;
constexpr int16_t WEEK_WED = 3;
constexpr int16_t WEEK_THUR = 4;
constexpr int16_t WEEK_FRI = 5;
constexpr int16_t WEEK_SAT = 6;
constexpr int16_t WEEK_SUN = 0;

AlarmSetWeekView::AlarmSetWeekView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView::AlarmSetWeekView");
    g_pAlarmSetWeekView = this;
}

AlarmSetWeekView::~AlarmSetWeekView()
{
    group->RemoveAll();
    delete labelRespeat;
    labelRespeat = nullptr;
    delete weekImage;
    weekImage = nullptr;
    for (int16_t alarmNum = 0; alarmNum < 7; alarmNum++) {
        delete buttonWeek[alarmNum];
        buttonWeek[alarmNum] = nullptr;
    }
    delete group;
    group = nullptr;
    g_pAlarmSetWeekView = nullptr;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView::~AlarmSetWeekView");
}

AlarmSetWeekView *AlarmSetWeekView::GetInstance(void)
{
    return g_pAlarmSetWeekView;
}

UILabelButton *AlarmSetWeekView::CreateUILabelButton(const char *buttonText, int16_t x, int16_t y)
{
    if (buttonText == nullptr) {
        return nullptr;
    }

    UILabelButton *labelButton = new UILabelButton();
    if (labelButton == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView:: new labelButton fail");
        return nullptr;
    }

    labelButton->SetPosition(x, y);
    labelButton->SetText(buttonText);
    labelButton->SetViewId(buttonText);
    labelButton->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    labelButton->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    labelButton->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
    labelButton->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
    labelButton->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::RELEASED);
    labelButton->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    labelButton->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::INACTIVE);
    labelButton->SetStyleForState(STYLE_BORDER_WIDTH, VIEW_BORDER_SIZE, UIButton::RELEASED);
    labelButton->SetStyleForState(STYLE_BORDER_WIDTH, VIEW_BORDER_SIZE, UIButton::PRESSED);
    labelButton->SetStyleForState(STYLE_BORDER_WIDTH, VIEW_BORDER_SIZE, UIButton::INACTIVE);
    labelButton->SetStyleForState(STYLE_BORDER_COLOR, Color::White().full, UIButton::RELEASED);
    labelButton->SetStyleForState(STYLE_BORDER_COLOR, Color::White().full, UIButton::PRESSED);
    labelButton->SetStyleForState(STYLE_BORDER_COLOR, Color::White().full, UIButton::INACTIVE);
    labelButton->SetStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(0x6c, 0xB8, 0xFD).full);
    labelButton->SetStyle(STYLE_BORDER_WIDTH, VIEW_BORDER_SIZE);
    labelButton->SetStyle(STYLE_BORDER_COLOR, Color::White().full);
    labelButton->Resize(BUTTON_WIDHT, BUTTON_HEIGHT);

    return labelButton;
}

void AlarmSetWeekView::CreateWeekButton1(void)
{
    int viewPositionX = VIEW_POSITION_X;   // 60 150
    UIView::OnClickListener *clickListener = (UIView::OnClickListener *)OHOS::AlarmPresenter::GetInstance();
    buttonWeek[WEEK_MON] = CreateUILabelButton(MON, viewPositionX, WEEK_Y1);
    if (buttonWeek[WEEK_MON] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView:: new buttonMon fail");
        return;
    }
    group->Add(buttonWeek[WEEK_MON]);
    buttonWeek[WEEK_MON]->SetOnClickListener(clickListener);
    buttonWeek[WEEK_MON]->SetViewId(MONDAY);
    viewPositionX += WEEK_GAP_X;

    buttonWeek[WEEK_TUE] = CreateUILabelButton(TUE, viewPositionX, WEEK_Y1);
    if (buttonWeek[WEEK_TUE] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView:: new buttonTue fail");
        return;
    }
    group->Add(buttonWeek[WEEK_TUE]);
    buttonWeek[WEEK_TUE]->SetOnClickListener(clickListener);
    buttonWeek[WEEK_TUE]->SetViewId(TUESDAY);
    viewPositionX += WEEK_GAP_X;

    buttonWeek[WEEK_WED] = CreateUILabelButton(WED, viewPositionX, WEEK_Y1);
    if (buttonWeek[WEEK_WED] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView:: new buttonWed fail");
        return;
    }
    group->Add(buttonWeek[WEEK_WED]);
    buttonWeek[WEEK_WED]->SetOnClickListener(clickListener);
    buttonWeek[WEEK_WED]->SetViewId(WEDNESDAY);
}

void AlarmSetWeekView::CreateWeekButton2(void)
{
    int viewPositionX2 = VIEW_POSITION_X2; // 12 210
    UIView::OnClickListener *clickListener = (UIView::OnClickListener *)OHOS::AlarmPresenter::GetInstance();

    buttonWeek[WEEK_THUR] = CreateUILabelButton(THUR, viewPositionX2, WEEK_Y2);
    if (buttonWeek[WEEK_THUR] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView:: new buttonThur fail");
        return;
    }
    group->Add(buttonWeek[WEEK_THUR]);
    buttonWeek[WEEK_THUR]->SetOnClickListener(clickListener);
    buttonWeek[WEEK_THUR]->SetViewId(THURSDAY);
    viewPositionX2 += WEEK_GAP_X;

    buttonWeek[WEEK_FRI] = CreateUILabelButton(FRI, viewPositionX2, WEEK_Y2);
    if (buttonWeek[WEEK_FRI] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView:: new buttonFri fail");
        return;
    }
    group->Add(buttonWeek[WEEK_FRI]);
    buttonWeek[WEEK_FRI]->SetOnClickListener(clickListener);
    buttonWeek[WEEK_FRI]->SetViewId(FRIDAY);
    viewPositionX2 += WEEK_GAP_X;

    buttonWeek[WEEK_SAT] = CreateUILabelButton(SAT, viewPositionX2, WEEK_Y2);
    if (buttonWeek[WEEK_SAT] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView:: new buttonSat fail");
        return;
    }
    group->Add(buttonWeek[WEEK_SAT]);
    buttonWeek[WEEK_SAT]->SetOnClickListener(clickListener);
    buttonWeek[WEEK_SAT]->SetViewId(SATURDAY);
    viewPositionX2 += WEEK_GAP_X;

    buttonWeek[WEEK_SUN] = CreateUILabelButton(SUN, viewPositionX2, WEEK_Y2);
    if (buttonWeek[WEEK_SUN] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView:: new buttonSun fail");
        return;
    }
    group->Add(buttonWeek[WEEK_SUN]);
    buttonWeek[WEEK_SUN]->SetOnClickListener(clickListener);
    buttonWeek[WEEK_SUN]->SetViewId(SUNDAY);
}

void AlarmSetWeekView::SetLabelButton(void)
{
    UIView::OnClickListener *clickListener = (UIView::OnClickListener *)OHOS::AlarmPresenter::GetInstance();
    if (clickListener == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView:: new clickListener fail");
        return;
    }
    weekImage = new UILabelButton();
    if (weekImage == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmConfigView:: new weekImage fail");
        return;
    }
    weekImage->SetPosition(TIME_LABEL_BUTTON_X, TIME_LABEL_BUTTON_Y, BUTTON_WIDHT3, BUTTON_HEIGHT3);
    weekImage->SetText(SURE);
    weekImage->SetTouchable(true);
    weekImage->SetViewId(ALARM_WEEK_OK);
    weekImage->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_WEEK_FONT);
    if (OHOS::AlarmPresenter::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmNewTimeView:: new ShadowBlue fail");
        return;
    }
    weekImage->SetTextColor(OHOS::AlarmPresenter::GetInstance()->ShadowBlue());
    weekImage->SetOnClickListener(clickListener);
    weekImage->SetImageSrc(PNG_NEW_DEVICE, PNG_NEW_DEVICE);
    group->Add(weekImage);
}

UIScrollView *AlarmSetWeekView::InitSetWeekView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView::InitSetWeekView");

    group = new UIScrollView();
    if (group == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView:: new group fail");
        return group;
    }

    group->SetPosition(CONTAINER_X, CONTAINER_Y, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    labelRespeat = new UILabel();
    if (labelRespeat == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView:: new labelRespeat fail");
        return group;
    }
    group->Add(labelRespeat);
    labelRespeat->SetText(RESPEAT);
    labelRespeat->SetFont(DEFAULT_VECTOR_FONT_FILENAME, DEFAULT_FONT);
    labelRespeat->SetPosition(TITLE_X, TITLE_Y, HORIZONTAL_RESOLUTION / 2, LABEL_POSITION_SIZE);

    CreateWeekButton1();
    CreateWeekButton2();
    SetLabelButton();

    group->SetTouchable(true);
    group->SetDraggable(true);
    UIView::OnDragListener *setWeekDragListener = (UIView::OnDragListener *)OHOS::AlarmPresenter::GetInstance();
    if (setWeekDragListener == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView:: new setWeekDragListener fail");
        return group;
    }
    group->SetOnDragListener(setWeekDragListener);
    return group;
}

void AlarmSetWeekView::RefreshSetWeek1(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView::RefreshStep1");
    if (buttonWeek[WEEK_MON] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView::RefreshStep1 buttonMon nullptr error!");
        return;
    }
    buttonWeek[WEEK_MON]->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    buttonWeek[WEEK_MON]->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    if (buttonWeek[WEEK_TUE] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView::RefreshStep1 buttonTue nullptr error!");
        return;
    }
    buttonWeek[WEEK_TUE]->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    buttonWeek[WEEK_TUE]->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    if (buttonWeek[WEEK_WED] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView::RefreshStep1 buttonWed nullptr error!");
        return;
    }
    buttonWeek[WEEK_WED]->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    buttonWeek[WEEK_WED]->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    if (buttonWeek[WEEK_THUR] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView::RefreshStep1 buttonThur nullptr error!");
        return;
    }
    buttonWeek[WEEK_THUR]->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    buttonWeek[WEEK_THUR]->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    if (buttonWeek[WEEK_FRI] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView::RefreshStep1 buttonFri nullptr error!");
        return;
    }
    buttonWeek[WEEK_FRI]->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    buttonWeek[WEEK_FRI]->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    if (buttonWeek[WEEK_SAT] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView::RefreshStep1 buttonSat nullptr error!");
        return;
    }
    buttonWeek[WEEK_SAT]->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    buttonWeek[WEEK_SAT]->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    if (buttonWeek[WEEK_SUN] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView::RefreshStep1 buttonSun nullptr error!");
        return;
    }
    buttonWeek[WEEK_SUN]->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    buttonWeek[WEEK_SUN]->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
}

void AlarmSetWeekView::RefreshSetWeek2(uint8_t week)
{
    for (int16_t alarmNum = 0; alarmNum < 7; alarmNum++) {
        if (buttonWeek[alarmNum] == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView::RefreshStep2 week[%d] nullptr error!", alarmNum);
            return;
        }
    }
    WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView:: week = %u", week);
    if ((week & 0x01) == 1) { // 判断星期天的位数是否为1
        buttonWeek[WEEK_SUN]->SetStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(0x6c, 0xB8, 0xFD).full);
        AlarmPresenter::GetInstance()->alarmWeek[WEEK_SUN] = 1;
    }
    if ((week & 0x02) == 0x02) { // 判断星期一的位数是否为1
        buttonWeek[WEEK_MON]->SetStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(0x6c, 0xB8, 0xFD).full);
        AlarmPresenter::GetInstance()->alarmWeek[WEEK_MON] = 1;
    }
    if ((week & 0x04) == 0x04) { // 判断星期二的位数是否为1
        buttonWeek[WEEK_TUE]->SetStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(0x6c, 0xB8, 0xFD).full);
        AlarmPresenter::GetInstance()->alarmWeek[WEEK_TUE] = 1;
    }
    if ((week & 0x08) == 0x08) { // 判断星期三的位数是否为1
        buttonWeek[WEEK_WED]->SetStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(0x6c, 0xB8, 0xFD).full);
        AlarmPresenter::GetInstance()->alarmWeek[WEEK_WED] = 1;
    }
    if ((week & 0x10) == 0x10) { // 判断星期四的位数是否为1
        buttonWeek[WEEK_THUR]->SetStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(0x6c, 0xB8, 0xFD).full);
        AlarmPresenter::GetInstance()->alarmWeek[WEEK_THUR] = 1;
    }
    if ((week & 0x20) == 0x20) { // 判断星期五的位数是否为1
        buttonWeek[WEEK_FRI]->SetStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(0x6c, 0xB8, 0xFD).full);
        AlarmPresenter::GetInstance()->alarmWeek[WEEK_FRI] = 1;
    }
    if ((week & 0x40) == 0x40) { // 判断星期六的位数是否为1
        buttonWeek[WEEK_SAT]->SetStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(0x6c, 0xB8, 0xFD).full);
        AlarmPresenter::GetInstance()->alarmWeek[WEEK_SAT] = 1;
    }
}
}