/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportTargetSettingPage
 * Create: 2025-06-06
 */

#include "UiConfig.h"
#include "main/LoadImg.h"
#include "ui_resource_sport.h"
#include "wearable_log.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "sport/SportView.h"
#include "sport/SportTargetSettingPage.h"

namespace OHOS {
static constexpr uint16_t LABELS_X = 100;
static constexpr uint16_t LABELS_Y = 39;
static constexpr uint16_t LABELS_W = 253;
static constexpr uint16_t LABELS_H = 53;
static constexpr uint16_t LABELS_FONT = 38;
static constexpr uint16_t PICKER_X = 103;
static constexpr uint16_t PICKER_Y = 122;
static constexpr uint16_t PICKER_W = 247;
static constexpr uint16_t PICKER_H = 212;
static constexpr uint16_t BACKGROUND_FONT = 38;
static constexpr uint16_t HIGHTLIGHT_FONT = 44;
static constexpr uint16_t PICKER_ITEM_H = 65;
static constexpr uint16_t BUTTON_X = 63;
static constexpr uint16_t BUTTON_Y = 361;
static constexpr uint16_t BUTTON_W = 328;
static constexpr uint16_t BUTTON_H = 93;

REGIST_SLICE_PAGE(VIEW_SPORT, SportPages::SPORT_PAGE_TARGET_SETTING, SportTargetSettingPage, false);

SportTargetSettingPage::~SportTargetSettingPage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }
    if (titleLabel_ != nullptr) {
        delete titleLabel_;
        titleLabel_ = nullptr;
    }
    if (picker_ != nullptr) {
        delete picker_;
        picker_ = nullptr;
    }
    if (confirmButton_ != nullptr) {
        delete confirmButton_;
        confirmButton_ = nullptr;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SportTargetSettingPage::~SportTargetSettingPage");
}

void SportTargetSettingPage::CreateUILabel()
{
    titleLabel_ = new UILabel();
    if (titleLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportTargetSettingPage titleLabel_ new fail");
        return;
    }
    titleLabel_->SetPosition(LABELS_X, LABELS_Y, LABELS_W, LABELS_H);
    titleLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    titleLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, LABELS_FONT);
    group_->Add(titleLabel_);
}

void SportTargetSettingPage::CreateUIPicker()
{
    picker_ = new UIPicker();
    if (picker_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportTargetSettingPage picker_ new fail");
        return;
    }
    picker_->SetPosition(PICKER_X, PICKER_Y, PICKER_W, PICKER_H);
    picker_->SetItemHeight(PICKER_ITEM_H);
    picker_->SetBackgroundFont(DEFAULT_VECTOR_FONT_FILENAME, BACKGROUND_FONT);
    picker_->SetHighlightFont(DEFAULT_VECTOR_FONT_FILENAME, HIGHTLIGHT_FONT);
    group_->Add(picker_);
}

void SportTargetSettingPage::CreateUIButton()
{
    confirmButton_ = new UILabelButton();
    if (confirmButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportTargetSettingPage confirmButton_ new fail");
        return;
    }
    confirmButton_->SetPosition(BUTTON_X, BUTTON_Y, BUTTON_W, BUTTON_H);
    confirmButton_->SetViewId(TARGET_SETTING_CONFIRM_BUTTON);
    confirmButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::RELEASED);
    confirmButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    confirmButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::INACTIVE);

    sport_info_t *sportInfo = SportModel::GetInstance().GetCurrSportInfo();
    if (sportInfo->setType == SPORT_SET_TYPE_TARGET) {
        if (sportInfo->targetType == SPORT_TARGET_DISTANCE) {
            LOADIMG::LoadBtnImage(confirmButton_, SPORT_IMAGE, IMAGE_SPORT_OK1, IMAGE_SPORT_OK1);
        } else if (sportInfo->targetType == SPORT_TARGET_TIME) {
            LOADIMG::LoadBtnImage(confirmButton_, SPORT_IMAGE, IMAGE_SPORT_OK3, IMAGE_SPORT_OK3);
        } else if (sportInfo->targetType == SPORT_TARGET_CALORIE) {
            LOADIMG::LoadBtnImage(confirmButton_, SPORT_IMAGE, IMAGE_SPORT_OK2, IMAGE_SPORT_OK2);
        }
    } else if (sportInfo->setType == SPORT_SET_TYPE_REMIND) {
        if (sportInfo->remindType == SPORT_REMIND_INTERVAL_DISTANCE) {
            LOADIMG::LoadBtnImage(confirmButton_, SPORT_IMAGE, IMAGE_SPORT_OK1, IMAGE_SPORT_OK1);
        } else if (sportInfo->remindType == SPORT_REMIND_INTERVAL_TIME) {
            LOADIMG::LoadBtnImage(confirmButton_, SPORT_IMAGE, IMAGE_SPORT_OK3, IMAGE_SPORT_OK3);
        }
    }

    confirmButton_->SetOnClickListener(this);
    group_->Add(confirmButton_);
}

void SportTargetSettingPage::OnStart(void *data)
{
    group_ = new UIViewGroup();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportTargetSettingPage group_ new fail");
        return;
    }
    group_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    group_->SetOnDragListener(this);
    group_->SetDraggable(true);
    group_->SetTouchable(true);

    CreateUILabel();
    CreateUIPicker();
    CreateUIButton();
    AddViewToPageContainer(group_);

    sport_info_t *sportInfo = SportModel::GetInstance().GetCurrSportInfo();
    if (sportInfo->setType == SPORT_SET_TYPE_TARGET) {
        UpdateViewByTargetType();
    } else if (sportInfo->setType == SPORT_SET_TYPE_REMIND) {
        UpdateViewByRemindType();
    }
}

bool SportTargetSettingPage::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        sport_info_t *sportInfo = SportModel::GetInstance().GetCurrSportInfo();
        if (sportInfo->setType == SPORT_SET_TYPE_TARGET) {
            sportInfo->targetType = sportInfo->targetTypeTemp;
            NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_TARGET,
                TransitionType::TRANSITION_INVALID, false);
        } else if (sportInfo->setType == SPORT_SET_TYPE_REMIND) {
            sportInfo->remindType = sportInfo->remindTypeTemp;
            NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_REMIND,
                TransitionType::TRANSITION_INVALID, false);
        }
    }
    return true;
}

bool SportTargetSettingPage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), TARGET_SETTING_CONFIRM_BUTTON) == 0) {
        uint16_t index = picker_->GetSelected();
        sport_info_t *sportInfo = SportModel::GetInstance().GetCurrSportInfo();
        if (sportInfo->setType == SPORT_SET_TYPE_TARGET) {
            SportModel::GetInstance().SetTargetValue(index);
            sportInfo->isTargetSetted = true;
            SportModel::GetInstance().SaveConfig(SPORT_CONFIG_INFO);
            NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_TARGET,
                TransitionType::TRANSITION_INVALID, false);
        } else if (sportInfo->setType == SPORT_SET_TYPE_REMIND) {
            SportModel::GetInstance().SetRemindValue(index);
            sportInfo->isRemindSetted = true;
            SportModel::GetInstance().SaveConfig(SPORT_CONFIG_INFO);
            NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_REMIND,
                TransitionType::TRANSITION_INVALID, false);
        }
    }
    return true;
}

void SportTargetSettingPage::UpdateViewByTargetType()
{
    sport_info_t *sportInfo = SportModel::GetInstance().GetCurrSportInfo();
    if (sportInfo->targetType == SPORT_TARGET_DISTANCE) {
        SportType sportType = SportModel::GetInstance().GetSportType();
        if (sportType == SPORT_TYPE_INDOOR_SWIMMING) {
            titleLabel_->SetText("趟数");
            const char *uiLapData[] = {"1趟", "2趟", "4趟", "6趟", "8趟", "10趟", "12趟", "16趟", "20趟", "30趟"};
            picker_->SetTextColor(Color::Gray(), Color::GetColorFromRGB(0x52, 0xe4, 0xeb));
            picker_->SetValues(uiLapData, sizeof(uiLapData) / sizeof(uiLapData[0]));
        } else {
            titleLabel_->SetText("距离");
            const char *uiDistanceData[] = {"1km", "3km", "5km", "7km", "10km", "15km", "20km", "25km", "30km", "40km"};
            picker_->SetTextColor(Color::Gray(), Color::GetColorFromRGB(0x52, 0xe4, 0xeb));
            picker_->SetValues(uiDistanceData, sizeof(uiDistanceData) / sizeof(uiDistanceData[0]));
        }
    } else if (sportInfo->targetType == SPORT_TARGET_TIME) {
        titleLabel_->SetText("时间");
        const char *uiTimeData[] = {"10分钟", "20分钟", "30分钟", "60分钟", "120分钟", "180分钟", "200分钟"};
        picker_->SetTextColor(Color::Gray(), Color::GetColorFromRGB(0xff, 0x75, 0x00));
        picker_->SetValues(uiTimeData, sizeof(uiTimeData) / sizeof(uiTimeData[0]));
    } else if (sportInfo->targetType == SPORT_TARGET_CALORIE) {
        titleLabel_->SetText("热量");
        const char *uiCalorieData[] = {"100千卡", "200千卡", "300千卡", "500千卡", "600千卡", "800千卡", "1000千卡"};
        picker_->SetTextColor(Color::Gray(), Color::GetColorFromRGB(0xff, 0x4d, 0x4f));
        picker_->SetValues(uiCalorieData, sizeof(uiCalorieData) / sizeof(uiCalorieData[0]));
    }
    titleLabel_->Invalidate();
    picker_->Invalidate();
    confirmButton_->Invalidate();
}

void SportTargetSettingPage::UpdateViewByRemindType()
{
    sport_info_t *sportInfo = SportModel::GetInstance().GetCurrSportInfo();
    if (sportInfo->remindType == SPORT_REMIND_INTERVAL_DISTANCE) {
        SportType sportType = SportModel::GetInstance().GetSportType();
        if (sportType == SPORT_TYPE_INDOOR_SWIMMING) {
            titleLabel_->SetText("趟数");
            const char *uiLapData[] = {"1趟", "2趟", "4趟", "6趟", "8趟", "10趟", "12趟", "16趟", "20趟", "30趟"};
            picker_->SetTextColor(Color::Gray(), Color::GetColorFromRGB(0x52, 0xe4, 0xeb));
            picker_->SetValues(uiLapData, sizeof(uiLapData) / sizeof(uiLapData[0]));
        } else {
            titleLabel_->SetText("距离");
            const char *uiDistanceData[] = {"1km", "3km", "5km", "7km", "10km", "15km", "20km", "25km", "30km", "40km"};
            picker_->SetTextColor(Color::Gray(), Color::GetColorFromRGB(0x52, 0xe4, 0xeb));
            picker_->SetValues(uiDistanceData, sizeof(uiDistanceData) / sizeof(uiDistanceData[0]));
        }
    } else if (sportInfo->remindType == SPORT_REMIND_INTERVAL_TIME) {
        titleLabel_->SetText("时间");
        const char *uiTimeData[] = {"10分钟", "20分钟", "30分钟", "60分钟", "120分钟", "180分钟", "200分钟"};
        picker_->SetTextColor(Color::Gray(), Color::GetColorFromRGB(0xff, 0x75, 0x00));
        picker_->SetValues(uiTimeData, sizeof(uiTimeData) / sizeof(uiTimeData[0]));
    }
    titleLabel_->Invalidate();
    picker_->Invalidate();
    confirmButton_->Invalidate();
}
}