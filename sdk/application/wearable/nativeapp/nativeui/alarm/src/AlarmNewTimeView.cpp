/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Alarm
 * Author:
 * Create: 2021-09-11
 */

#include "alarm/AlarmNewTimeView.h"
#include "common/screen.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#ifdef BUILD_IN_LINUX
#include "localtime.h"
#endif
#include <string>
#include <iostream>
#include <sstream>

namespace OHOS {
#ifndef NEW_TIME
#define NEW_TIME "设置时间"
#endif
#define NEXT "下一步"

static AlarmNewTimeView *g_pAlarmNewTimeView = nullptr;
constexpr int16_t CONTAINER_X = 0;
constexpr int16_t CONTAINER_Y = 0;
constexpr int16_t TITLE_X = 153;
constexpr int16_t TITLE_Y = 30;
constexpr int16_t TIME_PICKER_X = 75;
constexpr int16_t TIME_PICKER_WIDTH = 250;
constexpr int16_t TIME_ITEM_HEIGHT = 80;
constexpr int16_t TIME_LABEL_BUTTON_X = 63;
constexpr int16_t TIME_LABEL_BUTTON_Y = 360;
constexpr int16_t LABEL_X = 0;
constexpr int16_t LABEL_Y = 45;
constexpr int16_t LABEL_BLANK = 38;
constexpr int16_t LIST_W = 300;
constexpr int16_t LIST_H = 260;
constexpr int16_t TITLE_LABEL_DEFAULT_HEIGHT = 50;
constexpr int16_t BUTTON_WIDHT3 = 324;
constexpr int16_t BUTTON_HEIGHT3 = 101;
constexpr int16_t PICKER_FONT_SIZE = 25;
constexpr uint8_t BACKGROUND_FONT = 40;
constexpr uint8_t HIGHTLIGHT_FONT = 50;
constexpr uint16_t LABEL_FONT = 40;

AlarmNewTimeView::AlarmNewTimeView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmNewTimeView::AlarmNewTimeView");
    g_pAlarmNewTimeView = this;
}

AlarmNewTimeView::~AlarmNewTimeView()
{
    group->RemoveAll();
    delete labelButton;
    labelButton = nullptr;
    delete picker;
    picker = nullptr;
    delete title;
    title = nullptr;
    delete group;
    group = nullptr;
    g_pAlarmNewTimeView = nullptr;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmNewTimeView::~AlarmNewTimeView");
}

AlarmNewTimeView *AlarmNewTimeView::GetInstance(void)
{
    return g_pAlarmNewTimeView;
}

UILabel *AlarmNewTimeView::GetTitleLabel(const char *titleName)
{
    if (titleName == nullptr) {
        return nullptr;
    }
    UILabel *label = new UILabel();
    if (label == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmNewTimeView:: new label fail");
        return nullptr;
    }
    // 2: half of screen width
    label->SetPosition(LABEL_X, LABEL_Y, HORIZONTAL_RESOLUTION / 2, TITLE_LABEL_DEFAULT_HEIGHT);
    label->SetText(titleName);
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT);
    return label;
}

void AlarmNewTimeView::CreateLabelTime(void)
{
    title = GetTitleLabel(NEW_TIME);
    if (title == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CreateLabelTime Set title fail");
        return;
    }
    group->Add(title);
    title->SetPosition(TITLE_X, TITLE_Y); // 672: x-coordinate
    title->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    picker = new UITimePicker();
    if (picker == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CreateLabelTime:: new picker fail");
        return;
    }

    UITimePicker::SelectedListener *selectedListener =
        (UITimePicker::SelectedListener *)OHOS::AlarmPresenter::GetInstance();
    if (selectedListener == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CreateLabelTime:: selectedListener is nullptr");
        return;
    }
    picker->EnableSecond(false);
    picker->SetPosition(TIME_PICKER_X, LABEL_Y + LABEL_BLANK, LIST_W, LIST_H); // 652: x-coordinate
    picker->SetItemHeight(TIME_ITEM_HEIGHT);
    picker->SetLoopState(0, true);
    picker->SetLoopState(1, true);
    picker->SetBackgroundFont(DEFAULT_VECTOR_FONT_FILENAME, BACKGROUND_FONT);
    picker->SetHighlightFont(DEFAULT_VECTOR_FONT_FILENAME, HIGHTLIGHT_FONT);
    picker->RegisterSelectedListener(selectedListener);
    group->Add(picker);
}

UIScrollView *AlarmNewTimeView::InitNewTimeView(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmNewTimeView::OnStart");
    group = new UIScrollView();
    if (group == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmNewTimeView:: new group fail");
        return nullptr;
    }
    group->SetPosition(CONTAINER_X, CONTAINER_Y);
    group->SetWidth(HORIZONTAL_RESOLUTION);
    group->SetHeight(VERTICAL_RESOLUTION);

    CreateLabelTime();

    labelButton = new UILabelButton();
    if (labelButton == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmNewTimeView:: new labelButton fail");
        return group;
    }
    labelButton->Resize(BUTTON_WIDHT3, BUTTON_HEIGHT3);
    labelButton->SetPosition(TIME_LABEL_BUTTON_X, TIME_LABEL_BUTTON_Y);
    labelButton->SetText(NEXT);
    labelButton->SetViewId(ALARM_NEXT);
    labelButton->SetTouchable(true);
    labelButton->SetImageSrc(PNG_NEW_DEVICE, PNG_NEW_DEVICE);
    labelButton->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT);
    if (OHOS::AlarmPresenter::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmNewTimeView:: new ShadowBlue fail");
        return group;
    }
    labelButton->SetTextColor(OHOS::AlarmPresenter::GetInstance()->ShadowBlue());
    UIView::OnClickListener *clickListener = (UIView::OnClickListener *)OHOS::AlarmPresenter::GetInstance();
    labelButton->SetOnClickListener(clickListener);
    group->Add(labelButton);

    UIView::OnDragListener *newTimeDragListener = (UIView::OnDragListener *)OHOS::AlarmPresenter::GetInstance();
    if (newTimeDragListener == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmNewTimeView:: new newTimeDragListener fail");
        return group;
    }
    group->SetOnDragListener(newTimeDragListener);

    return group;
}

void AlarmNewTimeView::RefreshNewTime(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmNewTimeView::RefreshStep");
    struct tm local_time;
    time_t seconds = time(nullptr);
    if (localtime_r(&seconds, &local_time) == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::AlarmGetClockNum get localtime error!");
        return;
    }
    std::ostringstream ostrHour;
    std::ostringstream ostMin;
    ostrHour << local_time.tm_hour;
    ostMin << local_time.tm_min;
    std::string weekStr = ostrHour.str() + ":" + ostMin.str();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "time = %s!", weekStr.c_str());
    picker->SetSelected(weekStr.c_str()); // 添加的时间默认初始时间为0点
    uint32_t timeHour = stoi(ostrHour.str());
    uint32_t timeMin = stoi(ostMin.str());
    AlarmPresenter::GetInstance()->OnTimePickerInaction(timeHour, timeMin);
}
} // namespace OHOS
