/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: TimerSettingPage
 * Create: 2025-06-09
 */

#include <unistd.h>
#include <string>
#include "components/ui_arc_label.h"
#include "wearable_log.h"
#include "SlicePageFactory.h"
#include "common/image_cache_manager.h"
#include "timers/TimerView.h"
#include "timers/TimerPresenter.h"
#include "timers/TimerSettingPage.h"

namespace OHOS {
static constexpr uint8_t BACKGROUND_FONT = 40;
static constexpr uint8_t HIGHTLIGHT_FONT = 50;
static constexpr uint16_t PADDING = 3;
static constexpr uint16_t BORDER_RADIUS = 5;
static constexpr uint16_t TEXT_FONT = 24;
static constexpr uint16_t SETTING_TITLE_POSITION_X = 100;
static constexpr uint16_t SETTING_TITLE_POSITION_Y = 31;
static constexpr uint16_t SETTING_TITLE_SIZE_WIDTH = 253;
static constexpr uint16_t SETTING_TITLE_SIZE_HEIGHT = 53;
static constexpr uint16_t SETTING_TITLE_TEXT_SIZE = 38;
static constexpr uint16_t SETTING_TIMERLABEL_POSITION_X = 86;
static constexpr uint16_t SETTING_TIMERLABEL_POSITION_Y = 88;
static constexpr uint16_t SETTING_TIMERLABEL_SIZE_WIDTH = 48;
static constexpr uint16_t SETTING_TIMERLABEL_SIZE_HEIGHT = 42;
static constexpr uint16_t SETTING_TIMERLABEL_FONT = 18;
static constexpr uint16_t SETTING_START_POSITION_X = 112;
static constexpr uint16_t SETTING_START_POSITION_Y = 347;
static constexpr uint16_t SETTING_START_SIZE_WIDTH = 232;
static constexpr uint16_t SETTING_START_SIZE_HEIGHT = 76;
static constexpr uint16_t TIMERPICKER_POSX1 = 61;
static constexpr uint16_t TIMERPICKER_POSX2 = 183;
static constexpr uint16_t TIMERPICKER_POSX3 = 305;
static constexpr uint16_t TIMERPICKER_POSY = 119;
static constexpr uint16_t SETTING_TIMERPICKER_WIDTH = 88;
static constexpr uint16_t SETTING_TIMERPICKER_HEIGHT = 189;
static constexpr uint16_t TIMERPICKER_RADIUS = 11;
static constexpr uint16_t SETTING_TIMERPICKER_ITEM_HEIGHT = 70;
static constexpr uint16_t SETTING_TIMERPICKER_HOUR = 24;
static constexpr uint16_t SETTING_TIMERPICKER_HOUR_END = 23;
static constexpr uint16_t SETTING_TIMERPICKER_MIN = 60;
static constexpr uint16_t SETTING_TIMERPICKER_MIN_END = 59;
static constexpr uint16_t HOUR_TIME = 3600;
static constexpr uint16_t MIN_TIME = 60;
static constexpr uint16_t POX1_MIN = 86;
static constexpr uint16_t POX2_MIN = 206;
static constexpr uint16_t POX3_MIN = 332;
static constexpr uint16_t INDEX_2 = 2;
static constexpr char *START_TIMING_BUTTON  = "startCountDownButton";
static constexpr char *HOUR_PICKER = "hourPicker";
static constexpr char *MIN_PICKER = "minPicker";
static constexpr char *SEC_PICKER = "secPicker";
// 时间选择器分钟与秒钟列
static const char *g_uiPickerMinAndSecData[] = {"01", "02", "03", "04", "05", "06", "07", "08", "09", "10",
                                                "11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
                                                "21", "22", "23", "24", "25", "26", "27", "28", "29", "30",
                                                "31", "32", "33", "34", "35", "36", "37", "38", "39", "40",
                                                "41", "42", "43", "44", "45", "46", "47", "48", "49", "50",
                                                "51", "52", "53", "54", "55", "56", "57", "58", "59", "00"};

// 时间选择器小时列
static const char *g_uiPickerHourData[] = {"01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12",
                                           "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "00"};

REGIST_SLICE_PAGE(VIEW_TIMER, TIMER_PAGES::TIMER_SETTING_PAGE, TimerSettingPage, false);

TimerSettingPage::TimerSettingPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "TimerSettingPage::TimerSettingPage");
}

TimerSettingPage::~TimerSettingPage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }

    if (labelTitle_ != nullptr) {
        delete labelTitle_;
        labelTitle_ = nullptr;
    }

    for (int i = 0; i < PICKER_COUNT; i++) {
        if (timerPicker_[i] != nullptr) {
            delete timerPicker_[i];
            timerPicker_[i] = nullptr;
        }
    }

    if (startImg_ != nullptr) {
        delete startImg_;
        startImg_ = nullptr;
    }

    if (labelUnit_ != nullptr) {
        delete labelUnit_;
        labelUnit_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(TIMER_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "TimerSettingPage::~TimerSettingPage");
}

void TimerSettingPage::OnStart(void *data)
{
    labelTitle_ = new UILabel();
    if (labelTitle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelTitle_ fail");
        return;
    }
    labelTitle_->SetPosition(SETTING_TITLE_POSITION_X, SETTING_TITLE_POSITION_Y);
    labelTitle_->Resize(SETTING_TITLE_SIZE_WIDTH, SETTING_TITLE_SIZE_HEIGHT);
    labelTitle_->SetText("设置时间");
    labelTitle_->SetFont(BOLD_VECTOR_FONT_FILENAME, SETTING_TITLE_TEXT_SIZE);
    labelTitle_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelTitle_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    labelTitle_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    labelTitle_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    labelTitle_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);

    if (group_ == nullptr) {
    }
    group_ = new UIScrollView();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new group_ fail");
        return;
    }
    group_->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    group_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    group_->SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
    group_->SetThrowDrag(true);
    CreateLabel();
    CreateStartButton();
    CreatePicker();
    group_->Add(labelTitle_);
    group_->SetOnDragListener(this);
    AddViewToPageContainer(group_);
}

void TimerSettingPage::CreateLabel(void)
{
    // 用来显示picker的类型单位：小时、分、秒 同一时间只显示一个picker的类型单位
    labelUnit_ = new UILabel();
    if (labelUnit_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelUnit_ fail");
        return;
    }
    labelUnit_->SetPosition(SETTING_TIMERLABEL_POSITION_X, SETTING_TIMERLABEL_POSITION_Y);
    labelUnit_->Resize(SETTING_TIMERLABEL_SIZE_WIDTH, SETTING_TIMERLABEL_SIZE_HEIGHT);
    labelUnit_->SetStyle(STYLE_BACKGROUND_COLOR, 0xFFFFBF00);
    labelUnit_->SetStyle(STYLE_BACKGROUND_OPA, 0xef);
    labelUnit_->SetStyle(STYLE_BORDER_RADIUS, BORDER_RADIUS);
    labelUnit_->SetStyle(STYLE_PADDING_TOP, PADDING);
    labelUnit_->SetStyle(STYLE_PADDING_BOTTOM, PADDING);
    labelUnit_->SetStyle(STYLE_TEXT_FONT, TEXT_FONT);
    labelUnit_->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);
    labelUnit_->SetTextColor(Color::GetColorFromRGBA(0x00, 0x00, 0x00, 0xff));
    labelUnit_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelUnit_->SetDirect(TEXT_DIRECT_MIXED);
    labelUnit_->SetRollSpeed(0);
    labelUnit_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_TIMERLABEL_FONT);
    labelUnit_->SetVisible(false);
    group_->Add(labelUnit_);
}

void TimerSettingPage::CreateStartButton(void)
{
    // 开始按钮
    startImg_ = new UIImageView();
    if (startImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new startImg_ fail");
        return;
    }
    startImg_->SetPosition(SETTING_START_POSITION_X, SETTING_START_POSITION_Y);
    startImg_->Resize(SETTING_START_SIZE_WIDTH, SETTING_START_SIZE_HEIGHT);
    startImg_->SetViewId(START_TIMING_BUTTON);
    startImg_->SetTouchable(false);
    startImg_->SetOnClickListener(this);
    LOADIMG::LoadImageViewImg(startImg_, TIMER_IMAGE, IMAGE_TIMER_BEGIN_GRAY);
    group_->Add(startImg_);
}

void TimerSettingPage::CreatePicker()
{
    int posX[PICKER_COUNT] = {TIMERPICKER_POSX1, TIMERPICKER_POSX2, TIMERPICKER_POSX3};
    int posY = TIMERPICKER_POSY;
    for (int i = 0; i < PICKER_COUNT; i++) {
        timerPicker_[i] = new UIPicker();
        if (timerPicker_[i] == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new timerPicker_[%d] fail", i);
            return;
        }
        timerPicker_[i]->SetPosition(posX[i], posY);
        timerPicker_[i]->Resize(SETTING_TIMERPICKER_WIDTH, SETTING_TIMERPICKER_HEIGHT);
        timerPicker_[i]->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
        timerPicker_[i]->SetStyle(STYLE_BORDER_WIDTH, PADDING);
        timerPicker_[i]->SetStyle(STYLE_BORDER_RADIUS, TIMERPICKER_RADIUS);
        timerPicker_[i]->SetStyle(STYLE_BORDER_COLOR, 0xffb9b9b9);
        timerPicker_[i]->SetItemHeight(SETTING_TIMERPICKER_ITEM_HEIGHT);
        timerPicker_[i]->SetLoopState(true);
        timerPicker_[i]->SetBackgroundFont(DEFAULT_VECTOR_FONT_FILENAME, BACKGROUND_FONT);
        timerPicker_[i]->SetHighlightFont(DEFAULT_VECTOR_FONT_FILENAME, HIGHTLIGHT_FONT);
        timerPicker_[i]->SetTextColor(Color::GetColorFromRGBA(0x64, 0x69, 0x6e, 0xff),
                                      Color::GetColorFromRGBA(0xff, 0xff, 0xff, 0xff));
        if (i == 0) {
            timerPicker_[i]->SetValues(g_uiPickerHourData, SETTING_TIMERPICKER_HOUR);
            timerPicker_[i]->SetSelected(SETTING_TIMERPICKER_HOUR_END);
        } else {
            timerPicker_[i]->SetValues(g_uiPickerMinAndSecData, SETTING_TIMERPICKER_MIN);
            timerPicker_[i]->SetSelected(SETTING_TIMERPICKER_MIN_END);
        }
        timerPicker_[i]->SetOnClickListener(this);
        timerPicker_[i]->SetDraggable(true);
        timerPicker_[i]->SetOnDragListener(this);
        timerPicker_[i]->RegisterSelectedListener(this);
        group_->Add(timerPicker_[i]);
    }

    timerPicker_[0]->SetViewId(HOUR_PICKER);
    timerPicker_[1]->SetViewId(MIN_PICKER);
    timerPicker_[INDEX_2]->SetViewId(SEC_PICKER);
}

bool TimerSettingPage::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    if (strcmp(view.GetViewId(), START_TIMING_BUTTON) == 0) {
        TimerModel::GetInstance().SetEclipseTime(true, 0);
        uint8_t hour = std::stoi(g_uiPickerHourData[timerPicker_[0]->GetSelected()]);
        uint8_t minute = std::stoi(g_uiPickerMinAndSecData[timerPicker_[1]->GetSelected()]);
        uint8_t second = std::stoi(g_uiPickerMinAndSecData[timerPicker_[INDEX_2]->GetSelected()]);
        uint64_t totalTime = (hour * HOUR_TIME) + (minute * MIN_TIME) + (second);
        TimerModel::GetInstance().SetTotalTime(totalTime);
        TimerModel::GetInstance().SetIsShowFragment(true);
        NativeAbility::GetInstance().SwitchPageInSlice(TIMER_PAGES::TIMER_MAIN_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    labelUnit_->Invalidate();
    return true;
}

bool TimerSettingPage::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().SwitchPageInSlice(TIMER_PAGES::TIMER_MAIN_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    if (strcmp(view.GetViewId(), HOUR_PICKER) == 0) {
        timerPicker_[0]->SetStyle(STYLE_BORDER_COLOR, Color::Orange().full);
        timerPicker_[1]->SetStyle(STYLE_BORDER_COLOR, Color::Gray().full);
        timerPicker_[INDEX_2]->SetStyle(STYLE_BORDER_COLOR, Color::Gray().full);
        timerPicker_[0]->Invalidate();
        timerPicker_[1]->Invalidate();
        timerPicker_[INDEX_2]->Invalidate();
        labelUnit_->SetVisible(true);
        labelUnit_->SetPosition(POX1_MIN, SETTING_TIMERLABEL_POSITION_Y);
        labelUnit_->SetText("小时");
        labelUnit_->Invalidate();
    } else if (strcmp(view.GetViewId(), MIN_PICKER) == 0) {
        timerPicker_[0]->SetStyle(STYLE_BORDER_COLOR, Color::Gray().full);
        timerPicker_[1]->SetStyle(STYLE_BORDER_COLOR, Color::Orange().full);
        timerPicker_[INDEX_2]->SetStyle(STYLE_BORDER_COLOR, Color::Gray().full);
        timerPicker_[0]->Invalidate();
        timerPicker_[1]->Invalidate();
        timerPicker_[INDEX_2]->Invalidate();
        labelUnit_->SetVisible(true);
        labelUnit_->SetPosition(POX2_MIN, SETTING_TIMERLABEL_POSITION_Y);
        labelUnit_->SetText("分钟");
        labelUnit_->Invalidate();
    } else if (strcmp(view.GetViewId(), SEC_PICKER) == 0) {
        timerPicker_[1]->SetStyle(STYLE_BORDER_COLOR, Color::Gray().full);
        timerPicker_[0]->SetStyle(STYLE_BORDER_COLOR, Color::Gray().full);
        timerPicker_[INDEX_2]->SetStyle(STYLE_BORDER_COLOR, Color::Orange().full);
        timerPicker_[0]->Invalidate();
        timerPicker_[1]->Invalidate();
        timerPicker_[INDEX_2]->Invalidate();
        labelUnit_->SetVisible(true);
        labelUnit_->SetPosition(POX3_MIN, SETTING_TIMERLABEL_POSITION_Y);
        labelUnit_->SetText("秒");
        labelUnit_->Invalidate();
    }
    return true;
}

void TimerSettingPage::OnPickerStoped(UIPicker &picker)
{
    if (strcmp(picker.GetViewId(), HOUR_PICKER) == 0) {
        uint16_t hour = picker.GetSelected();
        timerPicker_[0]->SetStyle(STYLE_BORDER_COLOR, Color::Orange().full);
        timerPicker_[1]->SetStyle(STYLE_BORDER_COLOR, Color::Gray().full);
        timerPicker_[INDEX_2]->SetStyle(STYLE_BORDER_COLOR, Color::Gray().full);
        timerPicker_[0]->Invalidate();
        timerPicker_[1]->Invalidate();
        timerPicker_[INDEX_2]->Invalidate();
        if ((hour == SETTING_TIMERPICKER_HOUR_END) && (timerPicker_[1]->GetSelected() == SETTING_TIMERPICKER_MIN_END) &&
            (timerPicker_[INDEX_2]->GetSelected() == SETTING_TIMERPICKER_MIN_END)) {
            startImg_->SetTouchable(false);
            LOADIMG::LoadImageViewImg(startImg_, TIMER_IMAGE, IMAGE_TIMER_BEGIN_GRAY);
        } else {
            startImg_->SetTouchable(true);
            LOADIMG::LoadImageViewImg(startImg_, TIMER_IMAGE, IMAGE_TIMER_BEGIN_ORANGE);
        }
        labelUnit_->SetVisible(true);
        labelUnit_->SetPosition(POX1_MIN, SETTING_TIMERLABEL_POSITION_Y);
        labelUnit_->SetText("小时");
        labelUnit_->Invalidate();
    } else if (strcmp(picker.GetViewId(), MIN_PICKER) == 0) {
        uint16_t minute = picker.GetSelected();
        timerPicker_[0]->SetStyle(STYLE_BORDER_COLOR, Color::Gray().full);
        timerPicker_[1]->SetStyle(STYLE_BORDER_COLOR, Color::Orange().full);
        timerPicker_[INDEX_2]->SetStyle(STYLE_BORDER_COLOR, Color::Gray().full);
        timerPicker_[0]->Invalidate();
        timerPicker_[1]->Invalidate();
        timerPicker_[INDEX_2]->Invalidate();
        if ((timerPicker_[0]->GetSelected() == SETTING_TIMERPICKER_HOUR_END) && (minute == SETTING_TIMERPICKER_MIN_END) &&
            (timerPicker_[INDEX_2]->GetSelected() == SETTING_TIMERPICKER_MIN_END)) {
            startImg_->SetTouchable(false);
            LOADIMG::LoadImageViewImg(startImg_, TIMER_IMAGE, IMAGE_TIMER_BEGIN_GRAY);
        } else {
            startImg_->SetTouchable(true);
            LOADIMG::LoadImageViewImg(startImg_, TIMER_IMAGE, IMAGE_TIMER_BEGIN_ORANGE);
        }
        labelUnit_->SetVisible(true);
        labelUnit_->SetPosition(POX2_MIN, SETTING_TIMERLABEL_POSITION_Y);
        labelUnit_->SetText("分钟");
        labelUnit_->Invalidate();
    } else if (strcmp(picker.GetViewId(), SEC_PICKER) == 0) {
        uint16_t sec = picker.GetSelected();
        timerPicker_[0]->SetStyle(STYLE_BORDER_COLOR, Color::Gray().full);
        timerPicker_[1]->SetStyle(STYLE_BORDER_COLOR, Color::Gray().full);
        timerPicker_[INDEX_2]->SetStyle(STYLE_BORDER_COLOR, Color::Orange().full);
        timerPicker_[0]->Invalidate();
        timerPicker_[1]->Invalidate();
        timerPicker_[INDEX_2]->Invalidate();
        if ((timerPicker_[0]->GetSelected() == SETTING_TIMERPICKER_HOUR_END) &&
            (timerPicker_[1]->GetSelected() == SETTING_TIMERPICKER_MIN_END) && (sec == SETTING_TIMERPICKER_MIN_END)) {
            startImg_->SetTouchable(false);
            LOADIMG::LoadImageViewImg(startImg_, TIMER_IMAGE, IMAGE_TIMER_BEGIN_GRAY);
        } else {
            startImg_->SetTouchable(true);
            LOADIMG::LoadImageViewImg(startImg_, TIMER_IMAGE, IMAGE_TIMER_BEGIN_ORANGE);
        }
        labelUnit_->SetVisible(true);
        labelUnit_->SetPosition(POX3_MIN, SETTING_TIMERLABEL_POSITION_Y);
        labelUnit_->SetText("秒");
        labelUnit_->Invalidate();
    }
}
}