/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SleepTargetTime
 * Created: 2025-06-05
 */
#include <string>
#include "components/ui_list_nested.h"
#include "components/ui_scroll_view_nested.h"
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "common/image_cache_manager.h"
#include "sleep/SleepMainPage.h"
#include "sleep/SleepPresenter.h"
#include "sleep/SleepView.h"
#include "sleep/SleepTargetTime.h"

namespace OHOS {
static constexpr uint8_t BACKGROUND_FONT = 40;
static constexpr uint8_t HIGHTLIGHT_FONT = 55;
static constexpr int16_t TARGET_SLEEP_SETTARGET_X = 120;
static constexpr int16_t TARGET_SLEEP_SETTARGET_Y = 40;
static constexpr int16_t TARGET_SLEEP_SETTARGET_WIDTH = 200;
static constexpr int16_t TARGET_SLEEP_SETTARGET_HIGHT = 38;
static constexpr int16_t TARGET_SLEEP_SETTARGET_FONT_SIZE = 32;
static constexpr int16_t TARGET_SLEEP_START_X = 112;
static constexpr int16_t TARGET_SLEEP_START_Y = 347;
static constexpr int16_t TARGET_SLEEP_START_WIDTH = 232;
static constexpr int16_t TARGET_SLEEP_START_HIGHT = 76;
static constexpr int16_t TARGET_SLEEP_PICKER2_X = 270;
static constexpr int16_t TARGET_SLEEP_PICKER_Y = 132;
static constexpr int16_t TARGET_SLEEP_PICKER_WIDTH = 80;
static constexpr int16_t TARGET_SLEEP_PICKER_HIGHT = 185;
static constexpr int16_t TARGET_SLEEP_PICKER_BORDER = 3;
static constexpr int16_t TARGET_SLEEP_PICKER_RADIUS = 11;
static constexpr int16_t TARGET_SLEEP_PICKER_ITEM = 70;
static constexpr int16_t TARGET_SLEEP_PICKER_HOUR = 24;
static constexpr int16_t TARGET_SLEEP_PICKER_MINUTE = 60;
static constexpr int16_t TARGET_SLEEP_TIMERLABEL_X = 131;
static constexpr int16_t TARGET_SLEEP_TIMERLABEL2_X = 273;
static constexpr int16_t TARGET_SLEEP_TIMERLABEL_Y = 99;
static constexpr int16_t TARGET_SLEEP_TIMERLABEL2_Y = 90;
static constexpr int16_t TARGET_SLEEP_TIMERLABEL_WIDTH = 55;
static constexpr int16_t TARGET_SLEEP_TIMERLABEL_HEIGHT = 30;
static constexpr uint8_t TIMERLABEL_FONT = 24;
static constexpr int16_t TARGET_SLEEP_COLOR_R = 181;
static constexpr int16_t TARGET_SLEEP_COLOR_G = 402;
static constexpr int16_t TARGET_SLEEP_COLOR_B = 255;
static constexpr int16_t TARGET_SLEEP_PICKER1_X = 115;
static constexpr int16_t TARGET_SLEEP_PICKER2_Y = 255;
static constexpr char *START_BUTTON = "start";
static constexpr char *HOUR_PICKER = "hourPicker";
static constexpr char *MIN_PICKER = "minPicker";

// 时间选择器分钟列
static const char *g_uiPickerMinute[] = {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
                                         "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", 
                                         "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
                                         "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
                                         "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
                                         "50", "51", "52", "53", "54", "55", "56", "57", "58", "59"};

// 时间选择器小时列
static const char *g_uiPickerHour[] = {"00", "01", "02", "03", "04", "05", "06", "07",
                                       "08", "09", "10", "11", "12", "13", "14", "15",
                                       "16", "17", "18", "19", "20", "21", "22", "23"};

REGIST_SLICE_PAGE(VIEW_SLEEPING, SLEEPING_PAGES::SLEEPING_TARGET_PAGE, SleepTargetTime, false);

SleepTargetTime::SleepTargetTime()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SleepTargetTime::SleepTargetTime");
}

SleepTargetTime::~SleepTargetTime()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }

    if (targetTitleLabel_ != nullptr) {
        delete targetTitleLabel_;
        targetTitleLabel_ = nullptr;
    }

    if (timerUnitLabel_ != nullptr) {
        delete timerUnitLabel_;
        timerUnitLabel_ = nullptr;
    }

    if (startButton_ != nullptr) {
        delete startButton_;
        startButton_ = nullptr;
    }

    for (int i = 0; i < PICKER_COUNT; i++) {
        if (timerPicker_[i] != nullptr) {
            delete timerPicker_[i];
            timerPicker_[i] = nullptr;
        }
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(SLEEP_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SleepTargetTime::~SleepTargetTime");
}

void SleepTargetTime::OnStart(void *data)
{
    group_ = new UIScrollView();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "group_ is nullptr");
        return;
    }
    group_->SetPosition(0, 0);
    group_->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);

    targetTitleLabel_ = new UILabel();
    if (targetTitleLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "targetTitleLabel_ is nullptr");
        return;
    }
    targetTitleLabel_->SetPosition(TARGET_SLEEP_SETTARGET_X, TARGET_SLEEP_SETTARGET_Y);
    targetTitleLabel_->Resize(TARGET_SLEEP_SETTARGET_WIDTH, TARGET_SLEEP_SETTARGET_HIGHT);
    targetTitleLabel_->SetText("设置目标");
    targetTitleLabel_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    targetTitleLabel_->SetTextColor(Color::White());
    targetTitleLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, TARGET_SLEEP_SETTARGET_FONT_SIZE);
    targetTitleLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    group_->Add(targetTitleLabel_);

    InitStartButton();
    CreateLabel();
    CreatePicker();

    group_->SetOnDragListener(this);
    group_->SetDraggable(true);
    group_->SetTouchable(true);
    AddViewToPageContainer(group_);
}

void SleepTargetTime::InitStartButton()
{
    startButton_ = new UILabelButton();
    if (startButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "startButton_ is nullptr");
        return;
    }
    startButton_->SetPosition(TARGET_SLEEP_START_X, TARGET_SLEEP_START_Y);
    startButton_->Resize(TARGET_SLEEP_START_WIDTH, TARGET_SLEEP_START_HIGHT);
    startButton_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    startButton_->SetAlign(TEXT_ALIGNMENT_CENTER);
    startButton_->SetViewId(START_BUTTON);
    startButton_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TARGET_SLEEP_SETTARGET_FONT_SIZE);
    startButton_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Purple().full);
    startButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::White().full, UIButton::PRESSED);
    startButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::White().full, UIButton::RELEASED);
    startButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::INACTIVE);
    startButton_->SetText("开始");
    LOADIMG::LoadBtnImage(startButton_, SLEEP_IMAGE, IMAGE_SLEEP_START_GRAY, IMAGE_SLEEP_START_GRAY);
    startButton_->SetOnClickListener(this);
    startButton_->Disable();
    group_->Add(startButton_);
}

void SleepTargetTime::SetPickerTime()
{
    std::tuple<uint32_t, uint32_t> time = SleepModel::GetInstance().GetTargetTime();
    uint32_t timeHours = std::get<0>(time);
    uint32_t timeMinutes = std::get<1>(time);
    timerPicker_[0]->SetSelected(timeHours);
    timerPicker_[1]->SetSelected(timeMinutes);
}

void SleepTargetTime::CreatePicker()
{
    int posX[PICKER_COUNT] = {TARGET_SLEEP_PICKER1_X, TARGET_SLEEP_PICKER2_Y};
    int posY = TARGET_SLEEP_PICKER_Y;
    for (int i = 0; i < PICKER_COUNT; i++) {
        if (timerPicker_[i] == nullptr) {
            timerPicker_[i] = new UIPicker();
        }
        timerPicker_[i]->SetPosition(posX[i], posY);
        timerPicker_[i]->Resize(TARGET_SLEEP_PICKER_WIDTH, TARGET_SLEEP_PICKER_HIGHT);
        timerPicker_[i]->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
        timerPicker_[i]->SetStyle(STYLE_BORDER_WIDTH, TARGET_SLEEP_PICKER_BORDER);
        timerPicker_[i]->SetStyle(STYLE_BORDER_RADIUS, TARGET_SLEEP_PICKER_RADIUS);
        timerPicker_[i]->SetStyle(STYLE_BORDER_COLOR, Color::Gray().full);
        timerPicker_[i]->SetItemHeight(TARGET_SLEEP_PICKER_ITEM);
        timerPicker_[i]->SetLoopState(true);
        timerPicker_[i]->SetBackgroundFont(DEFAULT_VECTOR_FONT_FILENAME, BACKGROUND_FONT);
        timerPicker_[i]->SetHighlightFont(DEFAULT_VECTOR_FONT_FILENAME, HIGHTLIGHT_FONT);
        timerPicker_[i]->SetTextColor(Color::Gray(), Color::White());
        if (i == 0) {
            timerPicker_[i]->SetValues(g_uiPickerHour, TARGET_SLEEP_PICKER_HOUR);
        } else {
            timerPicker_[i]->SetValues(g_uiPickerMinute, TARGET_SLEEP_PICKER_MINUTE);
        }
        timerPicker_[i]->SetOnClickListener(this);
        timerPicker_[i]->SetOnDragListener(this);
        timerPicker_[i]->RegisterSelectedListener(this);
        timerPicker_[i]->SetDraggable(true);
        group_->Add(timerPicker_[i]);
    }

    timerPicker_[0]->SetViewId(HOUR_PICKER);
    timerPicker_[1]->SetViewId(MIN_PICKER);
    if (SleepModel::GetInstance().GetRecord() == true) {
        startButton_->SetTextColor(Color::White());
        startButton_->Enable();
        LOADIMG::LoadBtnImage(startButton_, SLEEP_IMAGE, IMAGE_SLEEP_SETTIMEBTN, IMAGE_SLEEP_SETTIMEBTN);
        SetPickerTime();
    } else {
        timerPicker_[0]->SetSelected(0);
        timerPicker_[1]->SetSelected(0);
    }
}

bool SleepTargetTime::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    if (strcmp(view.GetViewId(), START_BUTTON) == 0) {
        uint8_t hour = std::stoi(g_uiPickerHour[timerPicker_[0]->GetSelected()]);
        uint8_t minute = std::stoi(g_uiPickerMinute[timerPicker_[1]->GetSelected()]);
        SleepModel::GetInstance().SetTargetTime((hour * TARGET_SLEEP_PICKER_MINUTE) + minute);
        NativeAbility::GetInstance().SwitchPageInSlice(SLEEPING_PAGES::SLEEPING_SLEEP_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

void SleepTargetTime::CreateLabel()
{
    timerUnitLabel_ = new UILabel();
    if (timerUnitLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "timerUnitLabel_ is nullptr");
        return;
    }
    timerUnitLabel_->SetPosition(TARGET_SLEEP_TIMERLABEL_X, TARGET_SLEEP_TIMERLABEL_Y);
    timerUnitLabel_->Resize(TARGET_SLEEP_TIMERLABEL_WIDTH, TARGET_SLEEP_TIMERLABEL_HEIGHT);
    timerUnitLabel_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    timerUnitLabel_->SetText("小时");
    timerUnitLabel_->SetStyle(STYLE_BACKGROUND_COLOR, 0xFFB566FF);
    timerUnitLabel_->SetTextColor(Color::Black());
    timerUnitLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    timerUnitLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TIMERLABEL_FONT);
    timerUnitLabel_->SetVisible(false);
    group_->Add(timerUnitLabel_);
}

bool SleepTargetTime::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().SwitchPageInSlice(SLEEPING_PAGES::SLEEPING_MAIN_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    if (strcmp(view.GetViewId(), HOUR_PICKER) == 0) {
        timerPicker_[0]->SetStyle(
            STYLE_BORDER_COLOR,
            Color::GetColorFromRGB(TARGET_SLEEP_COLOR_R, TARGET_SLEEP_COLOR_G, TARGET_SLEEP_COLOR_B).full);
        timerPicker_[1]->SetStyle(STYLE_BORDER_COLOR, Color::Gray().full);
        timerPicker_[0]->Invalidate();
        timerPicker_[1]->Invalidate();
        timerUnitLabel_->SetVisible(true);
        timerUnitLabel_->SetPosition(TARGET_SLEEP_TIMERLABEL_X, TARGET_SLEEP_TIMERLABEL2_Y);
        timerUnitLabel_->SetText("小时");
        timerUnitLabel_->Invalidate();
    } else if (strcmp(view.GetViewId(), MIN_PICKER) == 0) {
        timerPicker_[1]->SetStyle(
            STYLE_BORDER_COLOR,
            Color::GetColorFromRGB(TARGET_SLEEP_COLOR_R, TARGET_SLEEP_COLOR_G, TARGET_SLEEP_COLOR_B).full);
        timerPicker_[0]->SetStyle(STYLE_BORDER_COLOR, Color::Gray().full);
        timerPicker_[0]->Invalidate();
        timerPicker_[1]->Invalidate();
        timerUnitLabel_->SetVisible(true);
        timerUnitLabel_->SetPosition(TARGET_SLEEP_TIMERLABEL2_X, TARGET_SLEEP_TIMERLABEL2_Y);
        timerUnitLabel_->SetText("分钟");
        timerUnitLabel_->Invalidate();
    }
    return true;
}

void SleepTargetTime::OnPickerStoped(UIPicker &picker)
{
    if (strcmp(picker.GetViewId(), HOUR_PICKER) == 0) {
        uint16_t hour = picker.GetSelected();
        picker.SetStyle(STYLE_BORDER_COLOR,
                        Color::GetColorFromRGB(TARGET_SLEEP_COLOR_R, TARGET_SLEEP_COLOR_G, TARGET_SLEEP_COLOR_B).full);
        picker.Invalidate();
        if ((hour == 0) && (timerPicker_[1]->GetSelected() == 0)) {
            startButton_->Disable();
            LOADIMG::LoadBtnImage(startButton_, SLEEP_IMAGE, IMAGE_SLEEP_START_GRAY, IMAGE_SLEEP_START_GRAY);
        } else {
            startButton_->SetTextColor(Color::White());
            startButton_->Enable();
            LOADIMG::LoadBtnImage(startButton_, SLEEP_IMAGE, IMAGE_SLEEP_SETTIMEBTN, IMAGE_SLEEP_SETTIMEBTN);
        }
        timerUnitLabel_->SetVisible(true);
        timerUnitLabel_->SetPosition(TARGET_SLEEP_TIMERLABEL_X, TARGET_SLEEP_TIMERLABEL2_Y);
        timerUnitLabel_->SetText("小时");
        timerUnitLabel_->Invalidate();
    } else if (strcmp(picker.GetViewId(), MIN_PICKER) == 0) {
        uint16_t minute = picker.GetSelected();
        picker.SetStyle(STYLE_BORDER_COLOR,
                        Color::GetColorFromRGB(TARGET_SLEEP_COLOR_R, TARGET_SLEEP_COLOR_G, TARGET_SLEEP_COLOR_B).full);
        picker.Invalidate();
        if ((minute == 0) && (timerPicker_[0]->GetSelected() == 0)) {
            startButton_->Disable();
            LOADIMG::LoadBtnImage(startButton_, SLEEP_IMAGE, IMAGE_SLEEP_START_GRAY, IMAGE_SLEEP_START_GRAY);
        } else {
            startButton_->SetTextColor(Color::White());
            startButton_->Enable();
            LOADIMG::LoadBtnImage(startButton_, SLEEP_IMAGE, IMAGE_SLEEP_SETTIMEBTN, IMAGE_SLEEP_SETTIMEBTN);
        }
        timerUnitLabel_->SetVisible(true);
        timerUnitLabel_->SetPosition(TARGET_SLEEP_TIMERLABEL2_X, TARGET_SLEEP_TIMERLABEL2_Y);
        timerUnitLabel_->SetText("分钟");
        timerUnitLabel_->Invalidate();
    }
}
}
