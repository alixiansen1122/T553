/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Alarm
 * Author:
 * Create: 2021-09-11
 */

#include "alarm/AlarmSetTimeView.h"
#include "common/screen.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include <string>

namespace OHOS {
#define SET_THIS_TIME "设置时间"
#define SURE "确认"

static AlarmSetTimeView *g_pAlarmSetTimeView = nullptr;
constexpr int16_t CONTAINER_X = 0;
constexpr int16_t CONTAINER_Y = 0;
constexpr int16_t TITLE_X = 153;
constexpr int16_t TITLE_Y = 30;
constexpr int16_t LABEL_X = 0;
constexpr int16_t LABEL_Y = 30;
constexpr int16_t LABEL_FONT = 40;
constexpr int16_t TIME_PICKER_X = 75;
constexpr int16_t TIME_PICKER_WIDTH = 250;
constexpr int16_t TIME_ITEM_HEIGHT = 80;
constexpr int16_t TIME_LABEL_BUTTON_X = 63;
constexpr int16_t TIME_LABEL_BUTTON_Y = 360;
constexpr int16_t LABEL_BLANK = 38;
constexpr int16_t LIST_W = 300;
constexpr int16_t LIST_H = 250;
constexpr int16_t TITLE_LABEL_DEFAULT_HEIGHT = 50;
constexpr int16_t BUTTON_WIDHT3 = 320;
constexpr int16_t BUTTON_HEIGHT3 = 101;
constexpr int16_t PICKER_FONT_SIZE = 25;
constexpr int16_t LABEL_TIME_FONT = 40;
constexpr uint8_t BACKGROUND_FONT = 40;
constexpr uint8_t HIGHTLIGHT_FONT = 50;

AlarmSetTimeView::AlarmSetTimeView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmSetTimeView::AlarmSetTimeView");
    g_pAlarmSetTimeView = this;
}

AlarmSetTimeView::~AlarmSetTimeView()
{
    group->RemoveAll();
    delete timeImage;
    timeImage = nullptr;
    delete picker;
    picker = nullptr;
    delete title;
    title = nullptr;
    delete group;
    group = nullptr;
    g_pAlarmSetTimeView = nullptr;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmSetTimeView::~AlarmSetTimeView");
}

AlarmSetTimeView *AlarmSetTimeView::GetInstance(void)
{
    return g_pAlarmSetTimeView;
}

UILabel *AlarmSetTimeView::GetTitleLabel(const char *titleName)
{
    if (titleName == nullptr) {
        return nullptr;
    }
    UILabel *label = new UILabel();
    if (label == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmSetTimeView:: new label fail");
        return nullptr;
    }
    // 2: half of screen width
    label->SetPosition(TITLE_X, TITLE_Y, HORIZONTAL_RESOLUTION / 2, TITLE_LABEL_DEFAULT_HEIGHT);
    label->SetText(titleName);
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT);
    return label;
}

void AlarmSetTimeView::SetLabelButton(void)
{
    timeImage = new UILabelButton();
    if (timeImage == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmConfigView:: new timeImage fail");
        return;
    }
    timeImage->SetPosition(TIME_LABEL_BUTTON_X, TIME_LABEL_BUTTON_Y, BUTTON_WIDHT3, BUTTON_HEIGHT3);
    timeImage->SetText(SURE);
    timeImage->SetTouchable(true);
    timeImage->SetViewId(ALARM_OK);
    if (OHOS::AlarmPresenter::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmNewTimeView:: new ShadowBlue fail");
        return;
    }
    timeImage->SetTextColor(OHOS::AlarmPresenter::GetInstance()->ShadowBlue());
    timeImage->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_TIME_FONT);
    UIView::OnClickListener *clickListener = (UIView::OnClickListener *)OHOS::AlarmPresenter::GetInstance();
    timeImage->SetOnClickListener(clickListener);
    timeImage->SetImageSrc(PNG_NEW_DEVICE, PNG_NEW_DEVICE);
    group->Add(timeImage);
}

UIScrollView *AlarmSetTimeView::InitSetTimeView(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmSetTimeView::OnStart");
    group = new UIScrollView();
    if (group == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetTimeView:: new group fail");
        return nullptr;
    }
    group->SetPosition(CONTAINER_X, CONTAINER_Y);
    group->SetWidth(HORIZONTAL_RESOLUTION);
    group->SetHeight(VERTICAL_RESOLUTION);

    title = GetTitleLabel(SET_THIS_TIME);
    if (title == nullptr) {
        return nullptr;
    }
    group->Add(title);
    title->SetPosition(TITLE_X, TITLE_Y); // 672: x-coordinate
    title->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    picker = new UITimePicker();
    if (picker == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetTimeView:: new picker fail");
        return nullptr;
    }
    UITimePicker::SelectedListener *selectedListener =
        (UITimePicker::SelectedListener *)OHOS::AlarmPresenter::GetInstance();
    if (selectedListener == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetTimeView:: selectedListener is nullptr");
        return nullptr;
    }
    picker->EnableSecond(false);
    picker->SetPosition(TIME_PICKER_X, LABEL_Y + LABEL_BLANK, LIST_W, LIST_H); // 652: x-coordinate
    picker->SetItemHeight(TIME_ITEM_HEIGHT);
    picker->SetBackgroundFont(DEFAULT_VECTOR_FONT_FILENAME, BACKGROUND_FONT);
    picker->SetHighlightFont(DEFAULT_VECTOR_FONT_FILENAME, HIGHTLIGHT_FONT);
    picker->SetLoopState(0, true);
    picker->SetLoopState(1, true);
    picker->SetTouchable(true);
    picker->RegisterSelectedListener(selectedListener);
    group->Add(picker);

    SetLabelButton();

    group->SetTouchable(true);
    group->SetDraggable(true);
    UIView::OnDragListener *setTimeDragListener = (UIView::OnDragListener *)OHOS::AlarmPresenter::GetInstance();
    if (setTimeDragListener == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetTimeView:: setTimeDragListener is nullptr");
        return group;
    }
    group->SetOnDragListener(setTimeDragListener);
    return group;
}

void AlarmSetTimeView::RefreshSetTime(uint32_t time)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmSetTimeView::RefreshStep");
    std::string hour = std::to_string(time >> 16); // 闹钟时间存储小时位需要右移16位
    std::string min = std::to_string(time & 0x00FF); // 闹钟分钟位全部清除
    std::string pickerTime = hour + ":" + min;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmSetTimeView:: picker:%s", pickerTime.c_str());
    if (picker == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetTimeView::RefreshStep picker nullptr error!");
        return;
    }
    WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetTimeView::RefreshStep 35353535!");
    picker->SetSelected(pickerTime.c_str());
    if (!picker->SetSelected(pickerTime.c_str())) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmSetTimeView::RefreshStep Timepicker error!");
        return;
    }
    uint32_t timeHour = stoi(hour);
    uint32_t timeMin = stoi(min);
    AlarmPresenter::GetInstance()->OnTimePickerInaction(timeHour, timeMin);
}
} // namespace OHOS
