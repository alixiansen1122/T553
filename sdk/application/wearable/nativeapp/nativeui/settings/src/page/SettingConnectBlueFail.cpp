/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingConnectBlueFail
 * Created: 2025-06-05
 */
#include "wearable_log.h"
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "NativeAbility.h"
#include "AppViewIDs.h"
#include "ui_resource_settings.h"
#include "main/LoadImg.h"
#include "settings/model/SettingBluetoothModel.h"
#include "settings/page/SettingBluetoothPage.h"
#include "settings/page/SettingConnectBlueFail.h"

namespace OHOS {
static constexpr int16_t SETTING_IMG_X = 100;
static constexpr int16_t SETTING_IMG_Y = -50;
static constexpr int16_t SETTING_WATCH_Y = 180;
static constexpr int16_t SETTING_WATCH_HEIGHT = 40;
static constexpr int16_t SETTING_WATCH_FONT = 36;
static constexpr int16_t SETTING_CONTENT_Y = 230;
static constexpr int16_t SETTING_CONYENT_HEIGHT = 40;
static constexpr int16_t SETTING_BTN_X = 65;
static constexpr int16_t SETTING_BTN_Y = 359;
static constexpr int16_t SETTING_BTN_WIDTH = 324;
static constexpr int16_t SETTING_BTN_HEIGHT = 101;
static constexpr char *CONFIRM_BUTTON = "confirmButton";

void SettingConnectBlueFail::OnCreateView(void* data)
{
    InitFailFragment();
}

void SettingConnectBlueFail::OnDestroyView()
{
    fragmentView_.RemoveAll();
    if (img_ != nullptr) {
        delete img_;
        img_ = nullptr;
    }
    if (watchNameLabel_ != nullptr) {
        delete watchNameLabel_;
        watchNameLabel_ = nullptr;
    }
    if (connectStatusLabel_ != nullptr) {
        delete connectStatusLabel_;
        connectStatusLabel_ = nullptr;
    }
    if (confirmButton_ != nullptr) {
        delete confirmButton_;
        confirmButton_ = nullptr;
    }
}

void SettingConnectBlueFail::InitFailFragment()
{
    fragmentView_.SetTouchable(true);
    fragmentView_.SetDraggable(true);
    fragmentView_.SetOnDragListener(this);
    img_ = new UIImageView();
    if (img_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitFailFragment img_ new fail");
        return;
    }
    LOADIMG::LoadImageViewImg(img_, SETTING_IMAGE, IMAGE_SURPRISE);
    img_->SetPosition(SETTING_IMG_X, SETTING_IMG_Y);

    watchNameLabel_ = new UILabel();
    if (watchNameLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitFailFragment watchNameLabel_ new fail");
        return;
    }
    watchNameLabel_->SetPosition(0, SETTING_WATCH_Y, RESOLUTION_WIDTH, SETTING_WATCH_HEIGHT);
    watchNameLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    watchNameLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_WATCH_FONT);
    watchNameLabel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    std::string str = SettingBluetoothModel::GetInstance()->GetDeviceName();
    watchNameLabel_->SetText(str.c_str());

    connectStatusLabel_ = new UILabel();
    if (connectStatusLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitFailFragment connectStatusLabel_ new fail");
        return;
    }
    connectStatusLabel_->SetPosition(0, SETTING_CONTENT_Y, RESOLUTION_WIDTH, SETTING_WATCH_HEIGHT);
    connectStatusLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    connectStatusLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_WATCH_FONT);
    connectStatusLabel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    connectStatusLabel_->SetText("连接失败,请稍后再试");

    confirmButton_ = new UILabelButton();
    if (confirmButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitFailFragment confirmButton_ new fail");
        return;
    }
    LOADIMG::LoadBtnImage(confirmButton_, SETTING_IMAGE, IMAGE_OK_DOWN, IMAGE_OK_DOWN);
    confirmButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    confirmButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::RELEASED);
    confirmButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::INACTIVE);
    confirmButton_->SetPosition(SETTING_BTN_X, SETTING_BTN_Y, SETTING_BTN_WIDTH, SETTING_BTN_HEIGHT);
    confirmButton_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    confirmButton_->SetAlign(TEXT_ALIGNMENT_CENTER);
    confirmButton_->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    confirmButton_->SetViewId(CONFIRM_BUTTON);
    confirmButton_->SetOnClickListener(this);
    confirmButton_->SetVisible(true);

    fragmentView_.Add(img_);
    fragmentView_.Add(watchNameLabel_);
    fragmentView_.Add(connectStatusLabel_);
    fragmentView_.Add(confirmButton_);
}

void SettingConnectBlueFail::RefreshFragment()
{
    if (watchNameLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "RefreshFragment fail");
        return;
    }
    std::string str = SettingBluetoothModel::GetInstance()->GetDeviceName();
    watchNameLabel_->SetText(str.c_str());
}

bool SettingConnectBlueFail::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    if (strcmp(view.GetViewId(), CONFIRM_BUTTON) == 0) {
        bool state = SettingBluetoothPage::GetInstance()->GetImageAnimatorState();
        if (state == false) {
            SettingBluetoothPage::GetInstance()->StartBtScan();
        }
        fragmentView_.SetVisible(false);
    }
    return true;
}

bool SettingConnectBlueFail::OnDragStart(UIView& view, const DragEvent& event)
{
    return true;
}

bool SettingConnectBlueFail::OnDrag(UIView& view, const DragEvent& event)
{
    return true;
}

bool SettingConnectBlueFail::OnDragEnd(UIView& view, const DragEvent& event)
{
    return true;
}
}
