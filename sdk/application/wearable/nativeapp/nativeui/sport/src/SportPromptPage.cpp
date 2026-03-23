/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportPromptPage
 * Create: 2025-06-06
 */

#include "wearable_log.h"
#include "UiConfig.h"
#include "main/LoadImg.h"
#include "ui_resource_sport.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "sport/SportView.h"
#include "sport/SportModel.h"
#include "sport/SportPromptPage.h"

namespace OHOS {
static constexpr uint16_t ICON_X = 173;
static constexpr uint16_t ICON_Y = 48;
static constexpr uint16_t LABEL_X = 24;
static constexpr uint16_t LABEL_Y = 177;
static constexpr uint16_t LABEL_W = 405;
static constexpr uint16_t LABEL_H = 100;
static constexpr uint16_t LABEL_FONT = 36;
static constexpr uint16_t BUTTON_X1 = 104;
static constexpr uint16_t BUTTON_X2 = 240;
static constexpr uint16_t BUTTON_Y = 296;
static constexpr uint16_t BUTTON_W = 109;
static constexpr uint16_t BUTTON_H = 109;
static const char *PROMPT_CONTENT = "本次运动距离过短，不会记录数据。是否结束？";

REGIST_SLICE_PAGE(VIEW_SPORT, SportPages::SPORT_PAGE_PROMPT, SportPromptPage, false);

SportPromptPage::~SportPromptPage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }
    if (promptLabel_ != nullptr) {
        delete promptLabel_;
        promptLabel_ = nullptr;
    }
    if (promptIcon_ != nullptr) {
        delete promptIcon_;
        promptIcon_ = nullptr;
    }
    if (cancelButton_ != nullptr) {
        delete cancelButton_;
        cancelButton_ = nullptr;
    }
    if (confirmButton_ != nullptr) {
        delete confirmButton_;
        confirmButton_ = nullptr;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SportPromptPage::~SportPromptPage");
}

void SportPromptPage::CreateUILabel()
{
    promptIcon_ = new UIImageView();
    if (promptIcon_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportPromptPage promptIcon_ new fail");
        return;
    }
    promptIcon_->SetPosition(ICON_X, ICON_Y);
    LOADIMG::LoadImageViewImg(promptIcon_, SPORT_IMAGE, IMAGE_SPORT_FACE);
    group_->Add(promptIcon_);

    promptLabel_ = new UILabel();
    if (promptLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportPromptPage promptLabel_ new fail");
        return;
    }
    promptLabel_->SetPosition(LABEL_X, LABEL_Y, LABEL_W, LABEL_H);
    promptLabel_->SetText(PROMPT_CONTENT);
    promptLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT);
    promptLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    group_->Add(promptLabel_);
}

void SportPromptPage::CreateUIButton()
{
    cancelButton_ = new UIButton();
    if (cancelButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportPromptPage cancelButton_ new fail");
        return;
    }
    cancelButton_->SetPosition(BUTTON_X1, BUTTON_Y, BUTTON_W, BUTTON_H);
    cancelButton_->SetViewId(SPORT_PROMPT_CANCEL_BUTTON);
    cancelButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::RELEASED);
    cancelButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    cancelButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::INACTIVE);
    LOADIMG::LoadBtnImage(cancelButton_, SPORT_IMAGE, IMAGE_SPORT_CANCEL, IMAGE_SPORT_CANCEL);
    cancelButton_->SetOnClickListener(this);
    group_->Add(cancelButton_);

    confirmButton_ = new UIButton();
    if (confirmButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportPromptPage confirmButton_ new fail");
        return;
    }
    confirmButton_->SetPosition(BUTTON_X2, BUTTON_Y, BUTTON_W, BUTTON_H);
    confirmButton_->SetViewId(SPORT_PROMPT_CONFIRM_BUTTON);
    confirmButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::RELEASED);
    confirmButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    confirmButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::INACTIVE);
    LOADIMG::LoadBtnImage(confirmButton_, SPORT_IMAGE, IMAGE_SPORT_CONFIRM, IMAGE_SPORT_CONFIRM);
    confirmButton_->SetOnClickListener(this);
    group_->Add(confirmButton_);
}

void SportPromptPage::OnStart(void *data)
{
    group_ = new UIViewGroup();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportPromptPage group_ new fail");
        return;
    }
    group_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    group_->SetOnDragListener(this);
    group_->SetDraggable(true);
    group_->SetTouchable(true);

    CreateUILabel();
    CreateUIButton();
    AddViewToPageContainer(group_);
}

bool SportPromptPage::OnDrag(UIView &view, const DragEvent &event)
{
    return true;
}

bool SportPromptPage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), SPORT_PROMPT_CANCEL_BUTTON) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_PAUSE,
            TransitionType::TRANSITION_INVALID, false);
    } else if (strcmp(view.GetViewId(), SPORT_PROMPT_CONFIRM_BUTTON) == 0) {
        SportModel::GetInstance().StopAndReport(true);
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_MENU,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}
}