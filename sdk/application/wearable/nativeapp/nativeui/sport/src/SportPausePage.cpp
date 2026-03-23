/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportPausePage
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
#include "sport/SportPausePage.h"

namespace OHOS {
static constexpr uint16_t NUM_THOUSAND = 1000;
static constexpr uint16_t TIME_LABEL_FONT = 48;
static constexpr uint16_t TIME_LABEL_X = 166;
static constexpr uint16_t TIME_LABEL_Y = 35;
static constexpr uint16_t TIME_LABEL_W = 122;
static constexpr uint16_t TIME_LABEL_H = 48;
static constexpr uint16_t TEXT_LABEL_FONT = 64;
static constexpr uint16_t TEXT_LABEL_X = 130;
static constexpr uint16_t TEXT_LABEL_Y = 151;
static constexpr uint16_t TEXT_LABEL_W = 194;
static constexpr uint16_t TEXT_LABEL_H = 80;
static constexpr uint16_t BUTTON_X1 = 54;
static constexpr uint16_t BUTTON_X2 = 217;
static constexpr uint16_t BUTTON_Y = 250;
static constexpr uint16_t BUTTON_W = 184;
static constexpr uint16_t BUTTON_H = 184;

REGIST_SLICE_PAGE(VIEW_SPORT, SportPages::SPORT_PAGE_PAUSE, SportPausePage, false);

void SportPausePage::SportPauseAnimatorCallback::Callback(UIView *view)
{
    UILabel *timeLabel = dynamic_cast<UILabel *>(view);
    if (timeLabel == nullptr) {
        return;
    }
    std::string currTime = GetCurrentTimeStr();
    timeLabel->SetText(currTime.c_str());
    timeLabel->Invalidate();
}

SportPausePage::~SportPausePage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }
    if (timeLabel_ != nullptr) {
        delete timeLabel_;
        timeLabel_ = nullptr;
    }
    if (textLabel_ != nullptr) {
        delete textLabel_;
        textLabel_ = nullptr;
    }
    if (continueButton_ != nullptr) {
        delete continueButton_;
        continueButton_ = nullptr;
    }
    if (finishButton_ != nullptr) {
        delete finishButton_;
        finishButton_ = nullptr;
    }
    if (animator_ != nullptr) {
        delete animator_;
        animator_ = nullptr;
    }
    if (callback_ != nullptr) {
        delete callback_;
        callback_ = nullptr;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SportPausePage::~SportPausePage");
}

void SportPausePage::CreateUILabel()
{
    timeLabel_ = new UILabel();
    if (timeLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportPausePage timeLabel_ new fail");
        return;
    }
    timeLabel_->SetPosition(TIME_LABEL_X, TIME_LABEL_Y, TIME_LABEL_W, TIME_LABEL_H);
    timeLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    timeLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TIME_LABEL_FONT);
    group_->Add(timeLabel_);

    textLabel_ = new UILabel();
    if (textLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportPausePage textLabel_ new fail");
        return;
    }
    textLabel_->SetPosition(TEXT_LABEL_X, TEXT_LABEL_Y, TEXT_LABEL_W, TEXT_LABEL_H);
    textLabel_->SetText("已暂停");
    textLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    textLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TEXT_LABEL_FONT);
    group_->Add(textLabel_);

    callback_ = new SportPauseAnimatorCallback();
    if (callback_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportPausePage::OnStart callback_ new fail");
        return;
    }

    animator_ = new Animator(callback_, timeLabel_, 0, true);
    if (animator_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportPausePage::OnStart new Animator animator_ fail");
        return;
    }
}

void SportPausePage::OnPause()
{
    if (animator_ != nullptr) {
        if (animator_->GetState() != Animator::STOP) {
            animator_->Stop();
        }
    }
}

void SportPausePage::OnResume()
{
    if (animator_ != nullptr) {
        if (animator_->GetState() == Animator::STOP) {
            animator_->Start();
        }
    }
}

void SportPausePage::CreateUIButton()
{
    finishButton_ = new UILabelButton();
    if (finishButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportPausePage finishButton_ new fail");
        return;
    }
    finishButton_->SetPosition(BUTTON_X1, BUTTON_Y, BUTTON_W, BUTTON_H);
    finishButton_->SetViewId(SPORT_PAUSE_FINISH_BUTTON);
    finishButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::RELEASED);
    finishButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    finishButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::INACTIVE);
    LOADIMG::LoadBtnImage(finishButton_, SPORT_IMAGE, IMAGE_SPORT_STOP, IMAGE_SPORT_STOP);
    finishButton_->SetOnClickListener(this);
    group_->Add(finishButton_);

    continueButton_ = new UILabelButton();
    if (continueButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportPausePage continueButton_ new fail");
        return;
    }
    continueButton_->SetPosition(BUTTON_X2, BUTTON_Y, BUTTON_W, BUTTON_H);
    continueButton_->SetViewId(SPORT_PAUSE_CONTINUE_BUTTON);
    continueButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::RELEASED);
    continueButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    continueButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::INACTIVE);
    LOADIMG::LoadBtnImage(continueButton_, SPORT_IMAGE, IMAGE_SPORT_START, IMAGE_SPORT_START);
    continueButton_->SetOnClickListener(this);
    group_->Add(continueButton_);
}

void SportPausePage::OnStart(void *data)
{
    group_ = new UIViewGroup();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportPausePage group_ new fail");
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

bool SportPausePage::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        SportModel::GetInstance().ResumeAndReport(true);
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_DATA,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool SportPausePage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), SPORT_PAUSE_CONTINUE_BUTTON) == 0) {
        SportModel::GetInstance().ResumeAndReport(true);
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_DATA,
            TransitionType::TRANSITION_INVALID, false);
    } else if (strcmp(view.GetViewId(), SPORT_PAUSE_FINISH_BUTTON) == 0) {
        sport_info_t *sportInfo = SportModel::GetInstance().GetCurrSportInfo();
        if (sportInfo->data.distance < NUM_THOUSAND) {
            NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_PROMPT,
                TransitionType::TRANSITION_INVALID, false);
        } else {
            SportModel::GetInstance().StopAndReport(true);
            SportModel::GetInstance().AddRecord();
            NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_MENU,
                TransitionType::TRANSITION_INVALID, false);
        }
    }
    return true;
}
}