/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: WeatherPageLunarPhase
 * Create: 2025-03-15
 */

#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "common/image_cache_manager.h"
#include "components/ui_list_nested.h"
#include "components/ui_scroll_view_nested.h"
#include "ui_resource_weather.h"
#include "wearable_log.h"
#include "weather/WeatherModel.h"
#include "weather/WeatherPresenter.h"
#include "weather/WeatherView.h"
#include "weather/WeatherPageLunarPhase.h"

namespace OHOS {
static constexpr uint8_t WEATHER_ITEM_FONT_SIZE = 24;
static constexpr uint8_t WEATHER_ITEM_FONT_SIZE_TWO = 30;
static constexpr int16_t WEATHER_TITLE_POSITION_X = 198;
static constexpr int16_t WEATHER_TITLE_POSITION_Y = 41;
static constexpr int16_t WEATHER_TITLE_SIZE_WIDTH = 454;
static constexpr int16_t WEATHER_TITLE_SIZE_HEIGHT = 49;
static constexpr int16_t WEATHER_LUNAR_POSITION_X = 169;
static constexpr int16_t WEATHER_LUNAR_POSITION_Y = 290;
static constexpr int16_t WEATHER_BUTTON_LEFT_POSITION_X = 76;
static constexpr int16_t WEATHER_BUTTON_RIGHT_POSITION_X = 306;
static constexpr int16_t WEATHER_BUTTON_POSITION_Y = 50;
static constexpr int16_t WEATHER_BUTTON_SIZE = 69;
static constexpr int16_t WEATHER_IMAGE_POSITION_X = 166;
static constexpr int16_t WEATHER_IMAGE_POSITION_Y = 160;
static constexpr int16_t WEATHER_LABEL_DAY_POSITION_X = 184;
static constexpr int16_t WEATHER_LABEL_DAY_SIZE_WIDTH = 89;
static constexpr int16_t WEATHER_LABEL_DAY_SIZE_HEIGHT = 30;
static constexpr int16_t WEATHER_PHASE_SIZE_WIDTH = 117;
static constexpr int16_t WEATHER_PHASE_SIZE_HEIGHT = 49;
static constexpr int16_t WEATHER_LIGHT_SIZE_WIDTH = 173;
static constexpr int16_t WEATHER_LIGHT_SIZE_HEIGHT = 39;
static constexpr int16_t WEATHER_LIGHT_POSITION_X = 141;

REGIST_SLICE_PAGE(VIEW_WEATHER, WEATHER_PAGES::WEATHER_LUNARPHASE_PAGE, WeatherPageLunarPhase, false);

WeatherPageLunarPhase::WeatherPageLunarPhase()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WeatherPageLunarPhase::WeatherPageLunarPhase");
}

WeatherPageLunarPhase::~WeatherPageLunarPhase()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }

    if (bgImg_ != nullptr) {
        delete bgImg_;
        bgImg_ = nullptr;
    }

    if (drawImg_ != nullptr) {
        delete drawImg_;
        drawImg_ = nullptr;
    }

    if (labelTitle_ != nullptr) {
        delete labelTitle_;
        labelTitle_ = nullptr;
    }

    if (labelDay_ != nullptr) {
        delete labelDay_;
        labelDay_ = nullptr;
    }

    if (labelLunarPhase_ != nullptr) {
        delete labelLunarPhase_;
        labelLunarPhase_ = nullptr;
    }

    if (labelMonthLight_ != nullptr) {
        delete labelMonthLight_;
        labelMonthLight_ = nullptr;
    }

    if (labelMonthDay_ != nullptr) {
        delete labelMonthDay_;
        labelMonthDay_ = nullptr;
    }

    if (buttonChangeLeft_ != nullptr) {
        delete buttonChangeLeft_;
        buttonChangeLeft_ = nullptr;
    }

    if (buttonChangeRight_ != nullptr) {
        delete buttonChangeRight_;
        buttonChangeRight_ = nullptr;
    }

    ImageCacheManager::GetInstance().UnloadAllInMultiRes(WEATHER_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WeatherPageLunarPhase::~WeatherPageLunarPhase");
}

void WeatherPageLunarPhase::OnStart(void *data)
{
    group_ = new UIViewGroup();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "group_ new fail");
        return;
    }
    group_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    group_->SetViewId("phaseGroup");
    group_->SetOnDragListener(this);
    group_->SetDraggable(true);
    group_->SetTouchable(true);
    phase_ = WeatherModel::GetInstance().GetLunarPhaseData();
    InitImage();
    InitLabel();
    InitButton();
    InitMonthLabel();
    AddViewToPageContainer(group_);
}

void WeatherPageLunarPhase::InitImage()
{
    bgImg_ = new UIImageView();
    if (bgImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "bgImg_ new image fail");
        return;
    }
    bgImg_->SetPosition(0, 0);
    bgImg_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    WeatherModel::GetInstance().LoadImageOfDayNight(bgImg_, IMAGE_WEATHER_BG1, IMAGE_WEATHER_BG2);
    drawImg_ = new UIImageView();
    if (drawImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "drawImg_ new image fail");
        return;
    }
    drawImg_->SetPosition(WEATHER_IMAGE_POSITION_X, WEATHER_IMAGE_POSITION_Y);
    drawImg_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    LOADIMG::LoadImageViewImg(drawImg_, WEATHER_IMAGE, IMAGE_WEATHER_MOON1);
    group_->Add(bgImg_);
    group_->Add(drawImg_);
}

void WeatherPageLunarPhase::InitLabel()
{
    labelTitle_ = new UILabel();
    if (labelTitle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "labelTitle_ is nullptr");
        return;
    }
    labelTitle_->SetPosition(WEATHER_TITLE_POSITION_X, WEATHER_TITLE_POSITION_Y);
    labelTitle_->SetText("月相");
    labelTitle_->Resize(WEATHER_TITLE_SIZE_WIDTH, WEATHER_TITLE_SIZE_HEIGHT);
    labelTitle_->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    labelTitle_->SetFont(BOLD_VECTOR_FONT_FILENAME, WEATHER_ITEM_FONT_SIZE_TWO);
    labelTitle_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelTitle_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    labelTitle_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    labelTitle_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    labelDay_ = new UILabel();
    if (labelDay_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "labelDay_ is nullptr");
        return;
    }
    labelDay_->SetPosition(WEATHER_LABEL_DAY_POSITION_X, WEATHER_TITLE_POSITION_Y + labelTitle_->GetTextHeight());
    labelDay_->SetText("今天");
    labelDay_->Resize(WEATHER_LABEL_DAY_SIZE_WIDTH, WEATHER_LABEL_DAY_SIZE_HEIGHT);
    labelDay_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, WEATHER_ITEM_FONT_SIZE);
    labelDay_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    WeatherModel::GetInstance().LoadTextColorOfDayNight(labelDay_, Color::White().full, Color::Gray().full);
    labelDay_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    labelDay_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    group_->Add(labelTitle_);
    group_->Add(labelDay_);
}

void WeatherPageLunarPhase::InitMonthLabel()
{
    labelLunarPhase_ = new UILabel();
    if (labelLunarPhase_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "labelLunarPhase_ is nullptr");
        return;
    }
    labelLunarPhase_->SetPosition(WEATHER_LUNAR_POSITION_X, WEATHER_LUNAR_POSITION_Y);
    labelLunarPhase_->Resize(WEATHER_PHASE_SIZE_WIDTH, WEATHER_PHASE_SIZE_HEIGHT);
    labelLunarPhase_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, WEATHER_ITEM_FONT_SIZE_TWO);
    labelLunarPhase_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelLunarPhase_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    labelLunarPhase_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    labelLunarPhase_->SetLineBreakMode(UILabel::LINE_BREAK_ELLIPSIS);
    labelLunarPhase_->SetText(phase_[0].phase);

    labelMonthLight_ = new UILabel();
    if (labelMonthLight_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "labelMonthLight_ is nullptr");
        return;
    }
    labelMonthLight_->SetPosition(WEATHER_LIGHT_POSITION_X, labelLunarPhase_->GetY()
                                    + labelLunarPhase_->GetTextHeight());
    labelMonthLight_->Resize(WEATHER_LIGHT_SIZE_WIDTH, WEATHER_LIGHT_SIZE_HEIGHT);
    labelMonthLight_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, WEATHER_ITEM_FONT_SIZE);
    labelMonthLight_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelMonthLight_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    WeatherModel::GetInstance().LoadTextColorOfDayNight(labelMonthLight_, Color::White().full, Color::Gray().full);
    labelMonthLight_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    labelMonthLight_->SetLineBreakMode(UILabel::LINE_BREAK_ELLIPSIS);
    labelMonthLight_->SetText(phase_[0].lightTime);

    labelMonthDay_ = new UILabel();
    if (labelMonthDay_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "labelMonthDay_ is nullptr");
        return;
    }
    labelMonthDay_->SetPosition(WEATHER_LIGHT_POSITION_X, labelMonthLight_->GetY() + labelMonthLight_->GetTextHeight());
    labelMonthDay_->Resize(WEATHER_LIGHT_SIZE_WIDTH, WEATHER_LIGHT_SIZE_HEIGHT);
    labelMonthDay_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, WEATHER_ITEM_FONT_SIZE);
    labelMonthDay_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelMonthDay_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    WeatherModel::GetInstance().LoadTextColorOfDayNight(labelMonthDay_, Color::White().full, Color::Gray().full);
    labelMonthDay_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    labelMonthDay_->SetLineBreakMode(UILabel::LINE_BREAK_ELLIPSIS);
    labelMonthDay_->SetText(phase_[0].monthDay);
    group_->Add(labelLunarPhase_);
    group_->Add(labelMonthLight_);
    group_->Add(labelMonthDay_);
}

void WeatherPageLunarPhase::InitButton()
{
    buttonChangeLeft_ = new UIButton();
    if (buttonChangeLeft_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "buttonChangeLeft_ is nullptr");
        return;
    }
    buttonChangeLeft_->SetPosition(WEATHER_BUTTON_LEFT_POSITION_X, WEATHER_BUTTON_POSITION_Y);
    buttonChangeLeft_->Resize(WEATHER_BUTTON_SIZE, WEATHER_BUTTON_SIZE);
    buttonChangeLeft_->SetViewId("buttonLeft");
    buttonChangeLeft_->SetOnClickListener(this);
    buttonChangeLeft_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::RELEASED);
    buttonChangeLeft_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    buttonChangeLeft_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::INACTIVE);
    buttonChangeLeft_->SetVisible(true);
    LOADIMG::LoadBtnImage(buttonChangeLeft_, WEATHER_IMAGE, IMAGE_WEATHER_PREV_GRAY, IMAGE_WEATHER_PREV_GRAY);
    buttonChangeRight_ = new UIButton();

    if (buttonChangeRight_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "buttonChangeRight_ new fail");
        return;
    }
    buttonChangeRight_->SetPosition(WEATHER_BUTTON_RIGHT_POSITION_X, WEATHER_BUTTON_POSITION_Y);
    buttonChangeRight_->Resize(WEATHER_BUTTON_SIZE, WEATHER_BUTTON_SIZE);
    buttonChangeRight_->SetViewId("buttonRight");
    buttonChangeRight_->SetOnClickListener(this);
    buttonChangeRight_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::RELEASED);
    buttonChangeRight_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    buttonChangeRight_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::INACTIVE);
    buttonChangeRight_->SetVisible(true);
    LOADIMG::LoadBtnImage(buttonChangeRight_, WEATHER_IMAGE, IMAGE_WEATHER_NEXT, IMAGE_WEATHER_NEXT);
    buttonChangeLeft_->Disable();
    group_->Add(buttonChangeLeft_);
    group_->Add(buttonChangeRight_);
}

bool WeatherPageLunarPhase::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().SwitchPageInSlice(WEATHER_PAGES::WEATHER_MONTHRISE_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    } else if (event.GetDragDirection() == DragEvent::DIRECTION_TOP_TO_BOTTOM) {
        NativeAbility::GetInstance().SwitchPageInSlice(WEATHER_PAGES::WEATHER_MAIN_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool WeatherPageLunarPhase::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);

    if (strcmp(view.GetViewId(), "buttonLeft") == 0) {
        if (index_ > 0) {
            labelDay_->SetText(phase_[--index_].weekTime);
            labelLunarPhase_->SetText(phase_[index_].phase);
            labelMonthLight_->SetText(phase_[index_].lightTime);
            labelMonthDay_->SetText(phase_[index_].monthDay);
            LOADIMG::LoadImageViewImg(drawImg_, WEATHER_IMAGE, phase_[index_].image);
            if (index_ == 0) {
                LOADIMG::LoadBtnImage(buttonChangeLeft_, WEATHER_IMAGE, IMAGE_WEATHER_PREV_GRAY,
                                      IMAGE_WEATHER_PREV_GRAY);
                buttonChangeLeft_->Disable();
            } else {
                LOADIMG::LoadBtnImage(buttonChangeLeft_, WEATHER_IMAGE, IMAGE_WEATHER_PREV, IMAGE_WEATHER_PREV);
                LOADIMG::LoadBtnImage(buttonChangeRight_, WEATHER_IMAGE, IMAGE_WEATHER_NEXT, IMAGE_WEATHER_NEXT);
                buttonChangeLeft_->Enable();
                buttonChangeRight_->Enable();
            }
        }
    } else if (strcmp(view.GetViewId(), "buttonRight") == 0) {
        if (index_ < BUTTON_CHANGE_END) {
            labelDay_->SetText(phase_[++index_].weekTime);
            labelLunarPhase_->SetText(phase_[index_].phase);
            labelMonthLight_->SetText(phase_[index_].lightTime);
            labelMonthDay_->SetText(phase_[index_].monthDay);
            LOADIMG::LoadImageViewImg(drawImg_, WEATHER_IMAGE, phase_[index_].image);
            if (index_ == BUTTON_CHANGE_END) {
                LOADIMG::LoadBtnImage(buttonChangeRight_, WEATHER_IMAGE, IMAGE_WEATHER_NEXT_GRAY,
                                      IMAGE_WEATHER_NEXT_GRAY);
                buttonChangeRight_->Disable();
            } else {
                LOADIMG::LoadBtnImage(buttonChangeLeft_, WEATHER_IMAGE, IMAGE_WEATHER_PREV, IMAGE_WEATHER_PREV);
                LOADIMG::LoadBtnImage(buttonChangeRight_, WEATHER_IMAGE, IMAGE_WEATHER_NEXT, IMAGE_WEATHER_NEXT);
                buttonChangeLeft_->Enable();
                buttonChangeRight_->Enable();
            }
        }
    }
    return true;
}
}