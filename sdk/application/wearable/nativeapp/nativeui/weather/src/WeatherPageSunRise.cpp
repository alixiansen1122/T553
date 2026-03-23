/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: WeatherPageSunRise
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
#include "weather/WeatherPageSunRise.h"

namespace OHOS {
static constexpr uint8_t WEATHER_TITLE_FONT_SIZE = 30;
static constexpr uint8_t WEATHER_ITEM_FONT_SIZE = 24;
static constexpr int16_t WEATHER_RESIZE_SIZE_WIDTH = 117;
static constexpr int16_t WEATHER_RESIZE_SIZE_HEIGHT = 38;
static constexpr int16_t WEATHER_TITLE_POSITION_X = 167;
static constexpr int16_t WEATHER_TITLE_POSITION_Y = 41;
static constexpr int16_t WEATHER_TITLE_DAY_POSITION_X = 49;
static constexpr int16_t WEATHER_TITLE_DAY_POSITION_Y = 318;
static constexpr int16_t WEATHER_TITLE_OFFSET = 250;
static constexpr int16_t WEATHER_BUTTON_LEFT_POSITION_X = 76;
static constexpr int16_t WEATHER_BUTTON_RIGHT_POSITION_X = 306;
static constexpr int16_t WEATHER_BUTTON_POSITION_Y = 50;
static constexpr int16_t WEATHER_BUTTON_SIZE = 69;
static constexpr int16_t WEATHER_IMAGE_POSITION_X = 0;
static constexpr int16_t WEATHER_IMAGE_POSITION_Y = 168;
static constexpr int16_t WEATHER_IMAGE_SUN_POSITION_X = 135;
static constexpr int16_t WEATHER_LABEL_DAY_POSITION_X = 184;
static constexpr int16_t WEATHER_LABEL_DAY_SIZE_WIDTH = 89;
static constexpr int16_t WEATHER_LABEL_DAY_SIZE_HEIGHT = 30;
static constexpr int16_t WEATHER_LABEL_SUN_SIZE_WIDTH = 113;
static constexpr int16_t WEATHER_LABEL_SUN_SIZE_HEIGHT = 30;

REGIST_SLICE_PAGE(VIEW_WEATHER, WEATHER_PAGES::WEATHER_SUNRISE_PAGE, WeatherPageSunRise, false);

WeatherPageSunRise::WeatherPageSunRise()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WeatherPageSunRise::WeatherPageSunRise");
}

WeatherPageSunRise::~WeatherPageSunRise()
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

    if (sunImg_ != nullptr) {
        delete sunImg_;
        sunImg_ = nullptr;
    }

    if (labelTitle_ != nullptr) {
        delete labelTitle_;
        labelTitle_ = nullptr;
    }

    if (labelDay_ != nullptr) {
        delete labelDay_;
        labelDay_ = nullptr;
    }

    for (int i = 0; i < SUNRISE_TIME_LABEL_COUNT; i++) {
        if (labelSun_[i] != nullptr) {
            delete labelSun_[i];
            labelSun_[i] = nullptr;
        }
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
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WeatherPageSunRise::~WeatherPageSunRise");
}

void WeatherPageSunRise::OnStart(void *data)
{
    group_ = new UIViewGroup();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Group new fail");
        return;
    }
    group_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    group_->SetViewId("weatherSwipeGroup_");
    group_->SetOnDragListener(this);
    group_->SetDraggable(true);
    group_->SetTouchable(true);
    sun_ = WeatherModel::GetInstance().GetSunData();
    InitImage();
    InitLabel();
    InitLabelSun();
    InitButton();
    index_ = 0;
    AddViewToPageContainer(group_);
}

void WeatherPageSunRise::InitImage()
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
    LOADIMG::LoadImageViewImg(drawImg_, WEATHER_IMAGE, IMAGE_WEATHER_SUNRISE);

    sunImg_ = new UIImageView();
    if (sunImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "sunImg_ new image fail");
        return;
    }
    sunImg_->SetPosition(WEATHER_IMAGE_SUN_POSITION_X, WEATHER_IMAGE_POSITION_Y);
    sunImg_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    LOADIMG::LoadImageViewImg(sunImg_, WEATHER_IMAGE, IMAGE_WEATHER_SUN);

    group_->Add(bgImg_);
    group_->Add(drawImg_);
    group_->Add(sunImg_);
}

void WeatherPageSunRise::InitLabel()
{
    labelTitle_ = new UILabel();
    if (labelTitle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "labelTitle_ is nullptr");
        return;
    }
    labelTitle_->SetPosition(WEATHER_TITLE_POSITION_X, WEATHER_TITLE_POSITION_Y);
    labelTitle_->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    labelTitle_->SetText("日出日落");
    labelTitle_->Resize(WEATHER_RESIZE_SIZE_WIDTH, WEATHER_RESIZE_SIZE_HEIGHT);
    labelTitle_->SetFont(BOLD_VECTOR_FONT_FILENAME, WEATHER_TITLE_FONT_SIZE);
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
    labelDay_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    WeatherModel::GetInstance().LoadTextColorOfDayNight(labelDay_, Color::White().full, Color::Gray().full);
    labelDay_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    labelDay_->SetText(sun_[0].weekTime);
    group_->Add(labelTitle_);
    group_->Add(labelDay_);
}

void WeatherPageSunRise::InitLabelSun()
{
    for (int i = 0; i < SUNRISE_TIME_LABEL_COUNT; i++) {
        labelSun_[i] = new UILabel();
        if (labelSun_[i] == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "labelSun_[%d] is nullptr", i);
            return;
        }
        labelSun_[i]->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
        labelSun_[i]->SetPosition(WEATHER_TITLE_DAY_POSITION_X + i * WEATHER_TITLE_OFFSET,
                                  WEATHER_TITLE_DAY_POSITION_Y);
        labelSun_[i]->Resize(WEATHER_LABEL_SUN_SIZE_WIDTH, WEATHER_LABEL_SUN_SIZE_HEIGHT);
        labelSun_[i]->SetFont(BOLD_VECTOR_FONT_FILENAME, WEATHER_ITEM_FONT_SIZE);
        labelSun_[i]->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        labelSun_[i]->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
        WeatherModel::GetInstance().LoadTextColorOfDayNight(labelSun_[i], Color::White().full, Color::Gray().full);
        labelSun_[i]->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
        group_->Add(labelSun_[i]);
    }
    std::string sunRise = "日出 " + std::string(sun_[0].sunRise);
    std::string sunSet = "日落 " + std::string(sun_[0].sunSet);
    labelSun_[0]->SetText(sunRise.c_str());
    labelSun_[1]->SetText(sunSet.c_str());
}

void WeatherPageSunRise::InitButton()
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
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "buttonChangeRight_ is nullptr");
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

bool WeatherPageSunRise::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().ChangeSliceToApplist();
    } else if (event.GetDragDirection() == DragEvent::DIRECTION_RIGHT_TO_LEFT) {
        NativeAbility::GetInstance().SwitchPageInSlice(WEATHER_PAGES::WEATHER_MONTHRISE_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    } else if (event.GetDragDirection() == DragEvent::DIRECTION_TOP_TO_BOTTOM) {
        NativeAbility::GetInstance().SwitchPageInSlice(WEATHER_PAGES::WEATHER_MAIN_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool WeatherPageSunRise::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    if (strcmp(view.GetViewId(), "buttonLeft") == 0) {
        if (index_ > 0) {
            labelDay_->SetText(sun_[--index_].weekTime);
            std::string sunRise = "日出 " + std::string(sun_[index_].sunRise);
            std::string sunSet = "日落 " + std::string(sun_[index_].sunSet);
            labelSun_[0]->SetText(sunRise.c_str());
            labelSun_[1]->SetText(sunSet.c_str());
            if (index_ == 0) {
                sunImg_->SetVisible(true);
                LOADIMG::LoadBtnImage(buttonChangeLeft_, WEATHER_IMAGE, IMAGE_WEATHER_PREV_GRAY,
                                      IMAGE_WEATHER_PREV_GRAY);
                buttonChangeLeft_->Disable();
            } else {
                sunImg_->SetVisible(false);
                LOADIMG::LoadBtnImage(buttonChangeLeft_, WEATHER_IMAGE, IMAGE_WEATHER_PREV, IMAGE_WEATHER_PREV);
                LOADIMG::LoadBtnImage(buttonChangeRight_, WEATHER_IMAGE, IMAGE_WEATHER_NEXT, IMAGE_WEATHER_NEXT);
                buttonChangeLeft_->Enable();
                buttonChangeRight_->Enable();
            }
        }
    } else if (strcmp(view.GetViewId(), "buttonRight") == 0) {
        if (index_ < BUTTON_CHANGE_END) {
            labelDay_->SetText(sun_[++index_].weekTime);
            std::string sunRise = "日出 " + std::string(sun_[index_].sunRise);
            std::string sunSet = "日落 " + std::string(sun_[index_].sunSet);
            labelSun_[0]->SetText(sunRise.c_str());
            labelSun_[1]->SetText(sunSet.c_str());
            sunImg_->SetVisible(false);
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