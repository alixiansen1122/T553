/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingConnectNewPhone
 * Created: 2025-06-05
 */

#include "wearable_log.h"
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "NativeAbility.h"
#include "AppViewIDs.h"
#include "uiservice/ui_service.h"
#include "main/LoadImg.h"
#include "ui_resource_settings.h"
#include "settings/page/SettingBluetoothPage.h"
#include "settings/page/SettingConnectNewPhone.h"

namespace OHOS {
static constexpr int16_t SETTING_CONNECT_CANCEL_WIDTH = 85;
static constexpr int16_t SETTING_CONNECT_CANCEL_Y = 300;
static constexpr int16_t SETTING_CONNECT_CANCEL_X = 114;
static constexpr int16_t SETTING_CONNECT_CONFIRM_X = 254;
static constexpr int16_t SETTING_CONNECT_TEXTFIRST_Y = 100;
static constexpr int16_t SETTING_CONNECT_TEXTSECOND_Y = 150;
static constexpr int16_t SETTING_CONNECT_TEXTTHIRD_Y = 200;
static constexpr int16_t SETTING_UNPAIR_TEXT_HEIGHT = 50;
static constexpr int16_t SETTING_UNPAIR_TEXT_FONT = 34;
static constexpr char *CONFIRM_BUTTON = "confirmButton";
static constexpr char *CANCEL_BUTTON = "cancelButton";

void SettingConnectNewPhone::OnCreateView(void* data)
{
    InitNewPhoneFragment();
}

void SettingConnectNewPhone::OnDestroyView()
{
    fragmentView_.RemoveAll();
    if (textFirst_ != nullptr) {
        delete textFirst_;
        textFirst_ = nullptr;
    }
    if (textSecond_ != nullptr) {
        delete textSecond_;
        textSecond_ = nullptr;
    }
    if (textThird_ != nullptr) {
        delete textThird_;
        textThird_ = nullptr;
    }
    if (cancel_ != nullptr) {
        delete cancel_;
        cancel_ = nullptr;
    }
    if (confirm_ != nullptr) {
        delete confirm_;
        confirm_ = nullptr;
    }
}

void SettingConnectNewPhone::InitNewPhoneFragment()
{
    fragmentView_.SetTouchable(true);
    fragmentView_.SetDraggable(true);
    fragmentView_.SetOnDragListener(this);
    std::string str1 = "将断开已连接的手机，";
    std::string str2 = "同一账号的新手机会保留";
    std::string str3 = "个人数据，是否继续？";
    textFirst_ = new UILabel();
    if (textFirst_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitNewPhoneFragment textFirst_ new fail");
        return;
    }
    textFirst_->SetPosition(0, SETTING_CONNECT_TEXTFIRST_Y, RESOLUTION_WIDTH, SETTING_UNPAIR_TEXT_HEIGHT);
    textFirst_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    textFirst_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_UNPAIR_TEXT_FONT);
    textFirst_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    textFirst_->SetText(str1.c_str());

    textSecond_ = new UILabel();
    if (textSecond_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitNewPhoneFragment textSecond_ new fail");
        return;
    }
    textSecond_->SetPosition(0, SETTING_CONNECT_TEXTSECOND_Y, RESOLUTION_WIDTH, SETTING_UNPAIR_TEXT_HEIGHT);
    textSecond_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    textSecond_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_UNPAIR_TEXT_FONT);
    textSecond_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    textSecond_->SetText(str2.c_str());

    textThird_ = new UILabel();
    if (textThird_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitNewPhoneFragment textThird_ new fail");
        return;
    }
    textThird_->SetPosition(0, SETTING_CONNECT_TEXTTHIRD_Y, RESOLUTION_WIDTH, SETTING_UNPAIR_TEXT_HEIGHT);
    textThird_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    textThird_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_UNPAIR_TEXT_FONT);
    textThird_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    textThird_->SetText(str3.c_str());

    cancel_ = new UIImageView();
    if (cancel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitNewPhoneFragment cancel_ new fail");
        return;
    }
    LOADIMG::LoadImageViewImg(cancel_, SETTING_IMAGE, IMAGE_CANCEL);
    cancel_->SetPosition(SETTING_CONNECT_CANCEL_X,
        SETTING_CONNECT_CANCEL_Y, SETTING_CONNECT_CANCEL_WIDTH, SETTING_CONNECT_CANCEL_WIDTH);
    cancel_->SetTouchable(true);
    cancel_->SetViewId(CANCEL_BUTTON);
    cancel_->SetOnClickListener(this);
    cancel_->SetVisible(true);

    confirm_ = new UIButton();
    if (confirm_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitNewPhoneFragment confirm_ new fail");
        return;
    }
    confirm_->SetPosition(SETTING_CONNECT_CONFIRM_X,
        SETTING_CONNECT_CANCEL_Y, SETTING_CONNECT_CANCEL_WIDTH, SETTING_CONNECT_CANCEL_WIDTH);
    confirm_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Blue().full);
    confirm_->SetViewId(CONFIRM_BUTTON);
    confirm_->SetOnClickListener(this);
    confirm_->SetVisible(true);
    LOADIMG::LoadBtnImage(confirm_, SETTING_IMAGE, IMAGE_OK_BLUE, IMAGE_OK_BLUE);
    confirm_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Blue().full, UIButton::PRESSED);
    confirm_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Blue().full, UIButton::RELEASED);
    confirm_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Blue().full, UIButton::INACTIVE);

    fragmentView_.Add(textFirst_);
    fragmentView_.Add(textSecond_);
    fragmentView_.Add(textThird_);
    fragmentView_.Add(cancel_);
    fragmentView_.Add(confirm_);
}

bool SettingConnectNewPhone::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    if (strcmp(view.GetViewId(), CANCEL_BUTTON) == 0) {
        fragmentView_.SetVisible(false);
        SettingBluetoothPage::GetInstance()->ChangeView(BT_MAIN_PAGE);
    } else if (strcmp(view.GetViewId(), CONFIRM_BUTTON) == 0) {
        fragmentView_.SetVisible(false);
        SettingBluetoothPage::GetInstance()->ChangeView(CONNECT_OPEN_FRAGMENT);
        SendMsgToUIService(CONNECT_NEW_PHONE, 0, nullptr, 0);
    }
    return true;
}

bool SettingConnectNewPhone::OnDragStart(UIView& view, const DragEvent& event)
{
    return true;
}

bool SettingConnectNewPhone::OnDrag(UIView& view, const DragEvent& event)
{
    return true;
}

bool SettingConnectNewPhone::OnDragEnd(UIView& view, const DragEvent& event)
{
    return true;
}
}
