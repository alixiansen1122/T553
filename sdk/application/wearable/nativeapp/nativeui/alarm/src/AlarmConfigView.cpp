/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Alarm
 * Author:
 * Create: 2021-09-11
 */

#include "alarm/AlarmConfigView.h"
#include "UiConfig.h"
#include <string>
#include "alarm/AlarmPresenter.h"
#include <sstream>
#include "color.h"

namespace OHOS {
#define SET_ALARM "编辑闹钟"
#define TIME "9:00:00"
#define SET_THIS_TIME "更改时间"
#define WEEK_MON_TUE "一 二"
#define SET_DATA "更改星期"
#define CHANGE_NAME "闹铃"
#define SET_NAME "更改闹铃"
#define DELETE_INTERFACES "删除"
#define EVERY_DAY "每天"
#define DO_NOT_REPEAT "不重复"

static AlarmConfigView *g_pAlarmConfigView = nullptr;
constexpr int16_t CONTAINER_X = 0;
constexpr int16_t CONTAINER_Y = 0;
constexpr int16_t SET_ONE = 1;
constexpr int16_t SET_FOUR = 4;
constexpr int16_t TITLE_X_LEN = 100;
constexpr int16_t MAX_TITLE_X_LEN = 160;
constexpr int16_t MAX_TITLE_Y_LEN = 45;
constexpr int16_t MAX_DATA_X = 65;
constexpr int16_t CLOCK_X = 165;
constexpr int16_t LABEL_WID = 250;
constexpr int16_t MAX_LABEL_H_LEN = 80;
constexpr int16_t MAX_DATA_W_LEN = 275;
constexpr int16_t DIGITAL_CLOCK_FONT = 45;
constexpr int16_t MAX_LABEL_TIME_Y_LEN = 195;
constexpr int16_t LABEL_SET_TIME_Y = 245;
constexpr int16_t MAX_LABEL_DAY_Y_LEN = 320;
constexpr int16_t MAX_LABEL_SET_WEEK_Y_LEN = 380;
constexpr int16_t MAX_LABEL_LABEL_CHANGE_NAME_Y_LEN = 460;
constexpr int16_t MAX_LABEL_SET_NAME_Y_LEN = 505;
constexpr int16_t MAX_DEFAULT_FONT_LEN = 34;
constexpr int16_t LITTLE_LABEL_FONT = 30;
constexpr int16_t MAX_DEFAULT_FONT_2_LEN = 35;
constexpr int16_t MAX_DEFAULT_LIT_FONT2_LEN = 40;
constexpr int16_t MAX_BACK_BUTTON_HEIGHT_LEN = 64;
constexpr int16_t MAX_FONT_DEFAULT_SIZE_LEN = 24;
constexpr int16_t MAX_DELETE_BUTTON_W_LEN = 320;
constexpr int16_t LABEL_DAY_WIDTH = 320;
constexpr int16_t MAX_DELETE_BUTTON_H_LEN = 100;
constexpr int16_t MAX_DELETE_BUTTON_X_LEN = 63;
constexpr int16_t MAX_DELETE_BUTTON_Y_LEN = 365;
constexpr int16_t MAX_IMAGE_CHANGE_X_LEN = 370;
constexpr int16_t MAX_IMAGE_CHANGE1_Y_LEN = 210;
constexpr int16_t MAX_IMAGE_CHANGE2_Y_LEN = 350;
constexpr int16_t MAX_IMAGE_CHANGE3_Y_LEN = 470;
constexpr int16_t MAX_IMAGE_CHANGE_WIDTH = 50;
constexpr int16_t MAX_IMAGE_CHANGE_HEIGHT = 60;
constexpr int16_t CONFIG_SCROLL_SIZE = 50;
constexpr int16_t LABEL_SEMITRANSPARENT = 168;

AlarmConfigView::AlarmConfigView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmConfigView::AlarmConfigView");
    g_pAlarmConfigView = this;
}

AlarmConfigView::~AlarmConfigView()
{
    group->RemoveAll();
    configScroll->RemoveAll();
    delete editLabel;
    editLabel = nullptr;
    delete deleteImage;
    deleteImage = nullptr;
    delete configScroll;
    configScroll = nullptr;
    delete labelTime;
    labelTime = nullptr;
    delete labelSetTime;
    labelSetTime = nullptr;
    delete labelDay;
    labelDay = nullptr;
    delete labelSetWeek;
    labelSetWeek = nullptr;
    delete labelChangeName;
    labelChangeName = nullptr;
    delete labelSetName;
    labelSetName = nullptr;
    delete changeIcon1;
    changeIcon1 = nullptr;
    delete changeIcon2;
    changeIcon2 = nullptr;
    delete changeIcon3;
    changeIcon3 = nullptr;
    delete group;
    group = nullptr;
    g_pAlarmConfigView = nullptr;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmConfigView::~AlarmConfigView");
}

AlarmConfigView *AlarmConfigView::GetInstance(void)
{
    return g_pAlarmConfigView;
}

UILabel *AlarmConfigView::CreateConfigLabel(const char *buttonText, int16_t labelFont, int16_t x, int16_t y, int16_t width)
{
    if (buttonText == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmConfigView:: buttonText nullptr!");
        return nullptr;
    }
    UILabel *label = nullptr;
    label = new UILabel();
    if (label == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmConfigView:: CreateConfigLabel return nullptr");
        return nullptr;
    }
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, labelFont);
    label->SetText(buttonText);
    label->SetPosition(x, y, width, MAX_LABEL_H_LEN);
    return label;
}

void AlarmConfigView::CreateChangeIcons(void)
{
    changeIcon1 = new UIImageView();
    if (changeIcon1 == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmConfigView:: new changeIcon1 fail");
        return;
    }
    changeIcon1->SetPosition(MAX_IMAGE_CHANGE_X_LEN, MAX_IMAGE_CHANGE1_Y_LEN, MAX_IMAGE_CHANGE_WIDTH, MAX_IMAGE_CHANGE_HEIGHT);
    changeIcon1->SetSrc(PNG_SETTTING_BUTTON_IMAGE);
    configScroll->Add(changeIcon1);

    changeIcon2 = new UIImageView();
    if (changeIcon2 == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmConfigView:: new changeIcon2 fail");
        return;
    }
    changeIcon2->SetPosition(MAX_IMAGE_CHANGE_X_LEN, MAX_IMAGE_CHANGE2_Y_LEN, MAX_IMAGE_CHANGE_WIDTH, MAX_IMAGE_CHANGE_HEIGHT);
    changeIcon2->SetSrc(PNG_SETTTING_BUTTON_IMAGE);
    configScroll->Add(changeIcon2);

    changeIcon3 = new UIImageView();
    if (changeIcon3 == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmConfigView:: new changeIcon3 fail");
        return;
    }
    changeIcon3->SetPosition(MAX_IMAGE_CHANGE_X_LEN, MAX_IMAGE_CHANGE3_Y_LEN, MAX_IMAGE_CHANGE_WIDTH, MAX_IMAGE_CHANGE_HEIGHT);
    changeIcon3->SetSrc(PNG_SETTTING_BUTTON_IMAGE);
    configScroll->Add(changeIcon3);

    UIView::OnDragListener *configDragListener = (UIView::OnDragListener *)OHOS::AlarmPresenter::GetInstance();
    if (configDragListener == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmConfigView:: new configDragListener fail");
        return;
    }
    group->SetOnDragListener(configDragListener);
}

void AlarmConfigView::CreateConfigButton(void)
{
    editLabel = CreateConfigLabel(SET_ALARM, MAX_DEFAULT_FONT_LEN, TITLE_X_LEN, MAX_TITLE_Y_LEN, LABEL_WID);
    if (editLabel == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmConfigView:: new editLabel fail");
        return;
    }
    editLabel->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER, UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
    configScroll->Add(editLabel);
    UIView::OnClickListener *clickListener = (UIView::OnClickListener *)OHOS::AlarmPresenter::GetInstance();
    if (clickListener == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmConfigView::OnStart clickListener nullptr");
        return;
    }
    labelTime = new UIDigitalClock();
    if (labelTime == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmConfigView:: new labelTime fail");
        return;
    }
    labelTime->SetViewId(SET_TIME);
    labelTime->SetPosition(CLOCK_X, MAX_LABEL_TIME_Y_LEN, LABEL_WID, MAX_LABEL_H_LEN);
    labelTime->SetFont(DEFAULT_VECTOR_FONT_FILENAME, DIGITAL_CLOCK_FONT);
    labelTime->SetTouchable(true);
    labelTime->SetDisplayMode(UIDigitalClock::DISPLAY_24_HOUR_NO_SECONDS);
    labelTime->SetOnClickListener(clickListener);
    configScroll->Add(labelTime);

    labelSetTime = CreateConfigLabel(SET_THIS_TIME, LITTLE_LABEL_FONT, MAX_TITLE_X_LEN, LABEL_SET_TIME_Y, LABEL_WID);
    if (labelSetTime == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmConfigView:: new labelSetTime fail");
        return;
    }
    labelSetTime->SetStyle(STYLE_TEXT_OPA, LABEL_SEMITRANSPARENT);
    configScroll->Add(labelSetTime);
    labelDay = CreateConfigLabel(WEEK_MON_TUE, MAX_DEFAULT_FONT_LEN, MAX_DATA_X, MAX_LABEL_DAY_Y_LEN, LABEL_DAY_WIDTH);
    if (labelDay == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmConfigView:: new labelDay fail");
        return;
    }
    labelDay->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER, UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
    configScroll->Add(labelDay);
    labelDay->SetViewId(SET_WEEK);
    labelDay->SetTouchable(true);
    labelDay->SetOnClickListener(clickListener);
    labelSetWeek = CreateConfigLabel(SET_DATA, LITTLE_LABEL_FONT, MAX_TITLE_X_LEN, MAX_LABEL_SET_WEEK_Y_LEN, LABEL_WID);
    if (labelSetWeek == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmConfigView:: new labelSetWeek fail");
        return;
    }
    labelSetWeek->SetStyle(STYLE_TEXT_OPA, LABEL_SEMITRANSPARENT);
    configScroll->Add(labelSetWeek);
}

UIScrollView *AlarmConfigView::InitConfigView(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmConfigView::OnStart");
    group = new UIScrollView();
    if (group == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetWeekView:: new group fail");
        return nullptr;
    }
    group->SetPosition(CONTAINER_X, CONTAINER_Y, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);

    configScroll = new UIScrollView();
    if (configScroll == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmConfigView:: new configScroll fail");
        return nullptr;
    }
    configScroll->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION - MAX_BACK_BUTTON_HEIGHT_LEN);
    configScroll->SetReboundSize(CONFIG_SCROLL_SIZE);
    configScroll->SetHorizontalScrollState(false);
    CreateConfigButton();
    labelChangeName = CreateConfigLabel(CHANGE_NAME, MAX_DEFAULT_FONT_LEN, MAX_TITLE_X_LEN,
    MAX_LABEL_LABEL_CHANGE_NAME_Y_LEN, LABEL_WID);
    if (labelChangeName == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmConfigView:: new labelChangeName fail");
        return nullptr;
    }
    configScroll->Add(labelChangeName);
    labelSetName = CreateConfigLabel(SET_NAME, LITTLE_LABEL_FONT, MAX_TITLE_X_LEN, MAX_LABEL_SET_NAME_Y_LEN, LABEL_WID);
    if (labelSetName == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmConfigView:: new labelSetName fail");
        return nullptr;
    }
    labelSetName->SetStyle(STYLE_TEXT_OPA, LABEL_SEMITRANSPARENT);
    configScroll->Add(labelSetName);
    deleteImage = new UILabelButton();
    if (deleteImage == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmConfigView:: new deleteImage fail");
        return nullptr;
    }
    deleteImage->SetPosition(MAX_DELETE_BUTTON_X_LEN, MAX_DELETE_BUTTON_Y_LEN, MAX_DELETE_BUTTON_W_LEN, MAX_DELETE_BUTTON_H_LEN);
    deleteImage->SetText(DELETE_INTERFACES);
    deleteImage->SetTouchable(true);
    deleteImage->SetViewId(ALARM_CLOCK_DELETE);
    deleteImage->SetTextColor(Color::Red());
    deleteImage->SetFont(DEFAULT_VECTOR_FONT_FILENAME, MAX_DEFAULT_LIT_FONT2_LEN);
    UIView::OnClickListener *clickListener = (UIView::OnClickListener *)OHOS::AlarmPresenter::GetInstance();
    deleteImage->SetOnClickListener(clickListener);
    deleteImage->SetImageSrc(PNG_NEW_DEVICE, PNG_NEW_DEVICE);
    
    CreateChangeIcons();
    group->Add(configScroll);
    group->Add(deleteImage);
    return group;
}

void AlarmConfigView::RefreshConfig(const uint32_t time, uint8_t week, uint16_t alarmNum)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmConfigView::RefreshStep");
    uint8_t hour = time >> 16; // 闹钟存储小时位需右移16位
    uint8_t min = time & 0x00FF; // 闹钟存储分钟的位数右边16位需要全部置0
    uint16_t ret;
    std::string weekStr;
    if (hour > 24) { // 小时数最大为24，加入判断若小时的参数超过24则错误
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmConfigView::RefreshStep hour > 24 error!");
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmConfigView::week = %u!", week);
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
    if (labelTime == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmConfigView::RefreshStep labelTime error!");
        return;
    }
    if (alarmNum == 0) {
        deleteImage->SetVisible(false);
    } else if (alarmNum >= SET_ONE && alarmNum <= SET_FOUR) {
        deleteImage->SetVisible(true);
    }
    labelTime->SetTime24Hour(hour, min, 0);
    labelDay->SetText(weekStr.c_str());
}
}