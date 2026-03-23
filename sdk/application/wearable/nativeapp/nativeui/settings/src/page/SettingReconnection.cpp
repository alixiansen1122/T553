/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingReconnection
 * Created: 2025-06-05
 */

#include "wearable_log.h"
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "NativeAbility.h"
#include "AppViewIDs.h"
#include "main/LoadImg.h"
#include "settings/model/SettingBluetoothModel.h"
#include "settings/page/SettingBluetoothPage.h"
#include "settings/page/SettingReconnection.h"

namespace OHOS {
static constexpr int16_t SETTING_WATCH_Y = 105;
static constexpr int16_t SETTING_WATCH_HEIGHT = 40;
static constexpr int16_t SETTING_WATCH_FONT = 36;
static constexpr int16_t SETTING_CONTENT_Y = 155;
static constexpr int16_t SETTING_CONYENT_HEIGHT = 40;
static constexpr int16_t SETTING_RECO_X = 111;
static constexpr int16_t SETTING_RECO1_Y = 230;
static constexpr int16_t SETTING_RECO2_Y = 321;
static constexpr int16_t SETTING_RECO_WIDTH = 232;
static constexpr int16_t SETTING_RECO_HEIGHT = 76;
static constexpr int16_t SETTING_RECO_FONT = 32;
static constexpr int16_t SETTING_RECO_BTN_WIDTH = 90;
static constexpr int16_t SETTING_RECO_BTN_Y = 320;
static constexpr int16_t SETTING_RECO_BTN_X1 = 85;
static constexpr int16_t SETTING_RECO_BTN_X2 = 279;
static constexpr char *RECONNECT_BUTTON = "reconnectionButton";
static constexpr char *UNPAIR_BUTTON = "unpairButton";

void SettingReconnection::OnCreateView(void* data)
{
    InitReconnectionFragment();
}

void SettingReconnection::OnDestroyView()
{
    fragmentView_.RemoveAll();
    if (watchNameLabel_ != nullptr) {
        delete watchNameLabel_;
        watchNameLabel_ = nullptr;
    }
    if (connectStatusLabel_ != nullptr) {
        delete connectStatusLabel_;
        connectStatusLabel_ = nullptr;
    }
    if (reconnectionButton_ != nullptr) {
        delete reconnectionButton_;
        reconnectionButton_ = nullptr;
    }
    if (unpair_ != nullptr) {
        delete unpair_;
        unpair_ = nullptr;
    }
}

void SettingReconnection::InitReconnectionFragment()
{
    fragmentView_.SetTouchable(true);
    fragmentView_.SetDraggable(true);
    fragmentView_.SetOnDragListener(this);
    watchNameLabel_ = new UILabel();
    if (watchNameLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitReconnectionFragment watchNameLabel_ new fail");
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
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitReconnectionFragment connectStatusLabel_ new fail");
        return;
    }
    connectStatusLabel_->SetPosition(0, SETTING_CONTENT_Y, RESOLUTION_WIDTH, SETTING_CONYENT_HEIGHT);
    connectStatusLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    connectStatusLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_WATCH_FONT);
    connectStatusLabel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    if (SettingBluetoothModel::GetInstance()->GetBlueToothConnectStatus()) {
        connectStatusLabel_->SetText("已连接");
    } else {
        connectStatusLabel_->SetText("未连接");
    }

    reconnectionButton_ = new UILabelButton();
    if (reconnectionButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitReconnectionFragment recButton_ new fail");
        return;
    }
    reconnectionButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Blue().full, UIButton::PRESSED);
    reconnectionButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Blue().full, UIButton::RELEASED);
    reconnectionButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Blue().full, UIButton::INACTIVE);
    reconnectionButton_->SetPosition(SETTING_RECO_X, SETTING_RECO1_Y, SETTING_RECO_WIDTH, SETTING_RECO_HEIGHT);
    if (SettingBluetoothModel::GetInstance()->GetBlueToothConnectStatus()) {
        reconnectionButton_->SetText("断开连接");
    } else {
        reconnectionButton_->SetText("重新连接");
    }

    reconnectionButton_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_RECO_FONT);
    reconnectionButton_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    reconnectionButton_->SetViewId(RECONNECT_BUTTON);
    reconnectionButton_->SetOnClickListener(this);
    reconnectionButton_->SetVisible(true);

    unpair_ = new UIImageView();
    if (unpair_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitReconnectionFragment unpair_ new fail");
        return;
    }
    unpair_->SetPosition(SETTING_RECO_X, SETTING_RECO2_Y, SETTING_RECO_WIDTH, SETTING_RECO_HEIGHT);
    LOADIMG::LoadImageViewImg(unpair_, SETTING_IMAGE, IMAGE_UNPAIR);
    unpair_->SetTouchable(true);
    unpair_->SetViewId(UNPAIR_BUTTON);
    unpair_->SetOnClickListener(this);

    fragmentView_.Add(watchNameLabel_);
    fragmentView_.Add(connectStatusLabel_);
    fragmentView_.Add(reconnectionButton_);
    fragmentView_.Add(unpair_);
}

void SettingReconnection::RefreshFragment()
{
    if (reconnectionButton_ == nullptr || connectStatusLabel_ == nullptr || watchNameLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "RefreshFragment fail");
        return;
    }
    bool status = SettingBluetoothModel::GetInstance()->GetBlueToothConnectStatus();
    if (status) {
        reconnectionButton_->SetText("断开连接");
    } else {
        reconnectionButton_->SetText("重新连接");
    }
    if (status) {
        connectStatusLabel_->SetText("已连接");
    } else {
        connectStatusLabel_->SetText("未连接");
    }
    std::string str = SettingBluetoothModel::GetInstance()->GetDeviceName();
    watchNameLabel_->SetText(str.c_str());
}

bool SettingReconnection::OnClick(UIView &view, const ClickEvent &event)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "OnClick");
    UNUSED(event);
    if (strcmp(view.GetViewId(), RECONNECT_BUTTON) == 0) {
        fragmentView_.SetVisible(false);
        SettingBluetoothPage::GetInstance()->ChangeView(RECONNECT_SECOND_CONFIRM_FRAGMENT);
    } else if (strcmp(view.GetViewId(), UNPAIR_BUTTON) == 0) {
        fragmentView_.SetVisible(false);
        SettingBluetoothPage::GetInstance()->ChangeView(UNPAIR_CONFIRM_FRAGMENT);
    }
    return true;
}

bool SettingReconnection::OnDragStart(UIView& view, const DragEvent& event)
{
    return true;
}

bool SettingReconnection::OnDrag(UIView& view, const DragEvent& event)
{
    return true;
}

bool SettingReconnection::OnDragEnd(UIView& view, const DragEvent& event)
{
    fragmentView_.SetVisible(false);
    SettingBluetoothPage::GetInstance()->ChangeView(BT_MAIN_PAGE);
    return true;
}
}
