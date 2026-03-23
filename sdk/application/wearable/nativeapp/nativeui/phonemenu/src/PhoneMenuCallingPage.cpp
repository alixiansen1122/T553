/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneMenuCallingPage
 * Create: 2025-06-06
 */

#include "AppViewIDs.h"
#include "SlicePageFactory.h"
#include "NativeAbility.h"
#include "UiConfig.h"
#include "main/LoadImg.h"
#include "wearable_log.h"
#include "ui_resource_phonemenu.h"
#include "phonemenu/PhoneMenuView.h"
#include "phonemenu/PhoneMenuCallerLogModel.h"
#include "phonemenu/PhoneMenuCallingPage.h"

namespace OHOS {
static constexpr uint16_t PHONE_LABEL_X = 74;
static constexpr uint16_t PHONE_LABEL_Y = 143;
static constexpr uint16_t PHONE_LABEL_W = 320;
static constexpr uint16_t PHONE_LABEL_H = 46;
static constexpr uint16_t PHONE_LABEL_FONT = 46;
static constexpr uint16_t STATE_LABEL_X1 = 185;
static constexpr uint16_t STATE_LABEL_X2 = 125;
static constexpr uint16_t STATE_LABEL_Y = 204;
static constexpr uint16_t STATE_LABEL_W = 238;
static constexpr uint16_t STATE_LABEL_H = 46;
static constexpr uint16_t STATE_LABEL_FONT = 34;
static constexpr uint16_t ICON_X = 142;
static constexpr uint16_t ICON_Y = 214;
static constexpr uint16_t BUTTON_X1 = 109;
static constexpr uint16_t BUTTON_X2 = 249;
static constexpr uint16_t BUTTON_X3 = 186;
static constexpr uint16_t BUTTON_Y = 300;
static constexpr uint16_t BUTTON_W = 85;
static constexpr uint16_t BUTTON_H = 85;
static const char *HANG_UP_BUTTON_ID = "callingHangUp";
static const char *ANSWER_BUTTON_ID = "callingAnswer";
static const char *VOLUME_BUTTON_ID = "microphoneChange";
static PhoneMenuCallingPage *g_phoneMenuCallingPage = nullptr;

REGIST_SLICE_PAGE(VIEW_PHONE_MENU, PhoneMenuPages::PHONE_MENU_CALLING_PAGE, PhoneMenuCallingPage, false);

PhoneMenuCallingPage::PhoneMenuCallingPage()
{
    g_phoneMenuCallingPage = this;
}

PhoneMenuCallingPage::~PhoneMenuCallingPage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }
    if (icon_ != nullptr) {
        delete icon_;
        icon_ = nullptr;
    }
    if (phoneNumber_ != nullptr) {
        delete phoneNumber_;
        phoneNumber_ = nullptr;
    }
    if (stateLabel_ != nullptr) {
        delete stateLabel_;
        stateLabel_ = nullptr;
    }
    if (answerButton_ != nullptr) {
        delete answerButton_;
        answerButton_ = nullptr;
    }
    if (hangUpButton_ != nullptr) {
        delete hangUpButton_;
        hangUpButton_ = nullptr;
    }
    if (volumeButton_ != nullptr) {
        delete volumeButton_;
        volumeButton_ = nullptr;
    }
    g_phoneMenuCallingPage = nullptr;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallingPage::~PhoneMenuCallingPage");
}

PhoneMenuCallingPage *PhoneMenuCallingPage::GetInstance()
{
    return g_phoneMenuCallingPage;
}

UILabel *PhoneMenuCallingPage::GetCallingPhoneNumberLabel()
{
    return phoneNumber_;
}

UILabel *PhoneMenuCallingPage::GetCallingStateLabel()
{
    return stateLabel_;
}

PhoneMenuCallingPage::CallingState PhoneMenuCallingPage::GetCallingState()
{
    return state_;
}

void PhoneMenuCallingPage::CreateUILabel()
{
    phoneNumber_ = new UILabel();
    if (phoneNumber_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallingPage phoneNumber_ new fail");
        return;
    }
    phoneNumber_->SetPosition(PHONE_LABEL_X, PHONE_LABEL_Y, PHONE_LABEL_W, PHONE_LABEL_H);
    phoneNumber_->SetText(PhoneMenuCallerLogModel::GetInstance()->GetPhoneNumber().c_str());
    phoneNumber_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, PHONE_LABEL_FONT);
    phoneNumber_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    group_->Add(phoneNumber_);

    stateLabel_ = new UILabel();
    if (stateLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallingPage stateLabel_ new fail");
        return;
    }
    stateLabel_->SetPosition(STATE_LABEL_X1, STATE_LABEL_Y, STATE_LABEL_W, STATE_LABEL_H);
    stateLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, STATE_LABEL_FONT);
    stateLabel_->SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    stateLabel_->SetTextColor(Color::Green());
    group_->Add(stateLabel_);

    icon_ = new UIImageView();
    if (icon_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallingPage icon_ new fail");
        return;
    }
    icon_->SetPosition(ICON_X, ICON_Y);
    group_->Add(icon_);
}

void PhoneMenuCallingPage::CreateUIButton()
{
    answerButton_ = new UIButton();
    if (answerButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallingPage answerButton_ new fail");
        return;
    }
    answerButton_->SetPosition(BUTTON_X2, BUTTON_Y, BUTTON_W, BUTTON_H);
    answerButton_->SetViewId(ANSWER_BUTTON_ID);
    answerButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::RELEASED);
    answerButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    answerButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::INACTIVE);
    LOADIMG::LoadBtnImage(answerButton_, PHONEMENU_IMAGE, IMAGE_PHONEMENU_ANSWER, IMAGE_PHONEMENU_ANSWER);
    answerButton_->SetOnClickListener(this);
    group_->Add(answerButton_);

    hangUpButton_ = new UIButton();
    if (hangUpButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallingPage hangUpButton_ new fail");
        return;
    }
    hangUpButton_->SetViewId(HANG_UP_BUTTON_ID);
    hangUpButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::RELEASED);
    hangUpButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    hangUpButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::INACTIVE);
    LOADIMG::LoadBtnImage(hangUpButton_, PHONEMENU_IMAGE, IMAGE_PHONEMENU_HANGUP, IMAGE_PHONEMENU_HANGUP);
    hangUpButton_->SetOnClickListener(this);
    group_->Add(hangUpButton_);

    volumeButton_ = new UIButton();
    if (volumeButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallingPage volumeButton_ new fail");
        return;
    }
    volumeButton_->SetPosition(BUTTON_X2, BUTTON_Y, BUTTON_W, BUTTON_H);
    volumeButton_->SetViewId(VOLUME_BUTTON_ID);
    volumeButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::RELEASED);
    volumeButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    volumeButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::INACTIVE);
    bool isMicMute = PhoneMenuCallerLogModel::GetInstance()->GetMicMute();
    if (isMicMute) {
        LOADIMG::LoadBtnImage(volumeButton_, PHONEMENU_IMAGE, IMAGE_PHONEMENU_MIC_MUTE, IMAGE_PHONEMENU_MIC_MUTE);
    } else {
        LOADIMG::LoadBtnImage(volumeButton_, PHONEMENU_IMAGE, IMAGE_PHONEMENU_MIC, IMAGE_PHONEMENU_MIC);
    }
    volumeButton_->SetOnClickListener(this);
    group_->Add(volumeButton_);
}

void PhoneMenuCallingPage::OnStart(void *data)
{
    group_ = new UIViewGroup();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallingPage group_ new fail");
        return;
    }
    group_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    group_->SetOnDragListener(this);
    group_->SetDraggable(true);
    group_->SetTouchable(true);

    CreateUILabel();
    CreateUIButton();
    AddViewToPageContainer(group_);

    if (PhoneMenuCallerLogModel::GetInstance()->GetCallStatus()) {
        state_ = CallingState::SESSION;
        stateLabel_->SetPosition(STATE_LABEL_X2, STATE_LABEL_Y, STATE_LABEL_W, STATE_LABEL_H);
        stateLabel_->SetText(PhoneMenuCallerLogModel::GetInstance()->DisplayCallDuration().c_str());
        icon_->SetVisible(false);
        hangUpButton_->SetVisible(true);
        hangUpButton_->SetPosition(BUTTON_X1, BUTTON_Y, BUTTON_W, BUTTON_H);
        answerButton_->SetVisible(false);
        volumeButton_->SetVisible(true);
    } else if (PhoneMenuCallerLogModel::GetInstance()->GetWaitingStatus()) {
        state_ = CallingState::WAITING;
        stateLabel_->SetText("新来电...");
        icon_->SetVisible(true);
        LOADIMG::LoadImageViewImg(icon_, PHONEMENU_IMAGE, IMAGE_PHONEMENU_CALLIN_ICON);
        hangUpButton_->SetVisible(true);
        hangUpButton_->SetPosition(BUTTON_X3, BUTTON_Y, BUTTON_W, BUTTON_H);
        answerButton_->SetVisible(false);
        volumeButton_->SetVisible(false);
    } else if (PhoneMenuCallerLogModel::GetInstance()->GetPhoneStatus()) {
        state_ = CallingState::OUTGOING;
        stateLabel_->SetText("正在呼叫...");
        icon_->SetVisible(true);
        LOADIMG::LoadImageViewImg(icon_, PHONEMENU_IMAGE, IMAGE_PHONEMENU_CALLOUT_ICON);
        hangUpButton_->SetVisible(true);
        hangUpButton_->SetPosition(BUTTON_X3, BUTTON_Y, BUTTON_W, BUTTON_H);
        answerButton_->SetVisible(false);
        volumeButton_->SetVisible(false);
    } else {
        state_ = CallingState::INCOMING;
        stateLabel_->SetText("手机来电");
        icon_->SetVisible(true);
        LOADIMG::LoadImageViewImg(icon_, PHONEMENU_IMAGE, IMAGE_PHONEMENU_CALLIN_ICON);
        hangUpButton_->SetVisible(true);
        hangUpButton_->SetPosition(BUTTON_X1, BUTTON_Y, BUTTON_W, BUTTON_H);
        answerButton_->SetVisible(true);
        volumeButton_->SetVisible(false);
    }

    PhoneMenuCallerLogModel::GetInstance()->SetVolumeScreenStatus(false);
    if (PhoneMenuCallerLogModel::GetInstance()->GetMulitplePhoneStatus()) {
        MulitplePhoneTextReplaces();
        PhoneMenuCallerLogModel::GetInstance()->SetMulitplePhoneStatus(false);
    }
}

bool PhoneMenuCallingPage::OnDrag(UIView &view, const DragEvent &event)
{
    return false;
}

bool PhoneMenuCallingPage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), VOLUME_BUTTON_ID) == 0) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallingPage VOLUME_BUTTON_ID");
        bool isMicMute = PhoneMenuCallerLogModel::GetInstance()->GetMicMute();
        if (!isMicMute) {
            LOADIMG::LoadBtnImage(volumeButton_, PHONEMENU_IMAGE, IMAGE_PHONEMENU_MIC_MUTE, IMAGE_PHONEMENU_MIC_MUTE);
        } else {
            LOADIMG::LoadBtnImage(volumeButton_, PHONEMENU_IMAGE, IMAGE_PHONEMENU_MIC, IMAGE_PHONEMENU_MIC);
        }
        PhoneMenuCallerLogModel::GetInstance()->SetMicMute(!isMicMute);
        SetMicMute(!isMicMute);
        return true;
    } else if (strcmp(view.GetViewId(), ANSWER_BUTTON_ID) == 0) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallingPage ANSWER_BUTTON_ID");
        if (AcceptIncomingCall() != 0) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallingPage AcceptIncomingCall() fail");
            return false;
        }
        PhoneMenuCallerLogModel::GetInstance()->SetCallStatus(true);
    } else if (strcmp(view.GetViewId(), HANG_UP_BUTTON_ID) == 0) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallingPage HANG_UP_BUTTON_ID");
        if (state_ == CallingState::WAITING) {
            if (RejectIncomingCall() != 0) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallingPage RejectIncomingCall error");
                return false;
            }
            PhoneMenuCallerLogModel::GetInstance()->SetWaitingStatus(false);
            PhoneMenuCallerLogModel::GetInstance()->SetCallStatus(true);
            NativeAbility::GetInstance().SwitchPageInSlice(PhoneMenuPages::PHONE_MENU_CALLING_PAGE,
                TransitionType::TRANSITION_INVALID, false);
        } else if (state_ == CallingState::INCOMING) {
            if (RejectIncomingCall() != 0) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallingPage RejectIncomingCall fail");
                return false;
            }
            PhoneMenuCallerLogModel::GetInstance()->BackToPrePage();
        } else {
            if (FinishCall() != 0) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallingPage FinishCall fail");
                return false;
            }
            PhoneMenuCallerLogModel::GetInstance()->SetMakePhoneStatus(false);
            PhoneMenuCallerLogModel::GetInstance()->SetCallStatus(false);
            PhoneMenuCallerLogModel::GetInstance()->BackToPrePage();
        }
    }
    return true;
}
}