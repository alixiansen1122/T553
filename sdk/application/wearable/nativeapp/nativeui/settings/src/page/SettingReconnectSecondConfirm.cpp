/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingReconnectSecondConfirm
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
#include "settings/page/SettingReconnectSecondConfirm.h"

namespace OHOS {
static constexpr int16_t SETTING_ADDR2 = 2;
static constexpr int16_t SETTING_ADDR3 = 3;
static constexpr int16_t SETTING_ADDR4 = 4;
static constexpr int16_t SETTING_ADDR5 = 5;
static constexpr int16_t SETTING_RECO_TITLE_Y = 60;
static constexpr int16_t SETTING_RECO_TITLE_HEIGHT = 40;
static constexpr int16_t SETTING_RECO_TITLE_FONT = 38;
static constexpr int16_t SETTING_RECO_CONTENT1_Y = 135;
static constexpr int16_t SETTING_RECO_CONTENT2_X = 30;
static constexpr int16_t SETTING_RECO_CONTENT2_Y = 180;
static constexpr int16_t SETTING_RECO_CONTENT3_Y = 225;
static constexpr int16_t SETTING_RECO_CONTENT_Y = 130;
static constexpr int16_t SETTING_RECO_CONYENT_HEIGHT = 115;
static constexpr int16_t SETTING_RECO_CONYENT1_HEIGHT = 50;
static constexpr int16_t SETTING_RECO_CONTENT_FONT = 35;
static constexpr int16_t SETTING_RECO_CONYENT_WIDTH = 394;
static constexpr int16_t SETTING_RECO_BTN_WIDTH = 85;
static constexpr int16_t SETTING_RECO_BTN_Y = 300;
static constexpr int16_t SETTING_RECO_BTN_X1 = 114;
static constexpr int16_t SETTING_RECO_BTN_X2 = 254;
static constexpr char *CONFIRM_BUTTON = "confirmButton";
static constexpr char *CANCEL_BUTTON = "cancelButton";

void SettingReconnectSecondConfirm::OnCreateView(void* data)
{
    InitSecondConfirmFragment();
}

void SettingReconnectSecondConfirm::OnDestroyView()
{
    fragmentView_.RemoveAll();
    if (title_ != nullptr) {
        delete title_;
        title_ = nullptr;
    }
    if (contentFirst_ != nullptr) {
        delete contentFirst_;
        contentFirst_ = nullptr;
    }
    if (contentSecond_ != nullptr) {
        delete contentSecond_;
        contentSecond_ = nullptr;
    }
    if (contentThird_ != nullptr) {
        delete contentThird_;
        contentThird_ = nullptr;
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

void SettingReconnectSecondConfirm::InitSecondConfirmFragment()
{
    fragmentView_.SetTouchable(true);
    fragmentView_.SetDraggable(true);
    fragmentView_.SetOnDragListener(this);

    title_ = new UILabel();
    if (title_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitSecondConfirmFragment title_ new fail");
        return;
    }
    title_->SetPosition(0, SETTING_RECO_TITLE_Y, RESOLUTION_WIDTH, SETTING_RECO_TITLE_HEIGHT);
    if (SettingBluetoothModel::GetInstance()->GetBlueToothConnectStatus()) {
        title_->SetText("确定断开连接?");
    } else {
        title_->SetText("确定重新连接?");
    }

    title_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    title_->SetFont(BOLD_VECTOR_FONT_FILENAME, SETTING_RECO_TITLE_FONT);
    title_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    std::string str1 = SettingBluetoothModel::GetInstance()->GetDeviceName();
    std::string str = "";
    std::string str2 = "";
    if (SettingBluetoothModel::GetInstance()->GetBlueToothConnectStatus()) {
        str = "此操作将会断开您与";
        str2 = "以下设备的连接:";
    } else {
        str = "此操作您将会与以下";
        str2 = "设备进行重新连接:";
    }
    contentFirst_ = new UILabel();
    if (contentFirst_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitSecondConfirmFragment contentFirst_ new fail");
        return;
    }
    contentFirst_->SetPosition(0, SETTING_RECO_CONTENT1_Y,
        RESOLUTION_WIDTH, SETTING_RECO_CONYENT1_HEIGHT);
    contentFirst_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    contentFirst_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_RECO_CONTENT_FONT);
    contentFirst_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    contentFirst_->SetText(str.c_str());

    contentSecond_ = new UILabel();
    if (contentSecond_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitSecondConfirmFragment contentSecond_ new fail");
        return;
    }
    contentSecond_->SetPosition(SETTING_RECO_CONTENT2_X, SETTING_RECO_CONTENT2_Y,
        SETTING_RECO_CONYENT_WIDTH, SETTING_RECO_CONYENT1_HEIGHT);
    contentSecond_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    contentSecond_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_RECO_CONTENT_FONT);
    contentSecond_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    contentSecond_->SetText(str2.c_str());

    contentThird_ = new UILabel();
    if (contentThird_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitSecondConfirmFragment contentThird_ new fail");
        return;
    }
    contentThird_->SetPosition(0, SETTING_RECO_CONTENT3_Y,
        RESOLUTION_WIDTH, SETTING_RECO_CONYENT1_HEIGHT);
    contentThird_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    contentThird_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_RECO_CONTENT_FONT);
    contentThird_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    contentThird_->SetText(str1.c_str());

    cancel_ = new UIImageView();
    if (cancel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitSecondConfirmFragment cancel_ new fail");
        return;
    }
    LOADIMG::LoadImageViewImg(cancel_, SETTING_IMAGE, IMAGE_CANCEL);
    cancel_->SetPosition(SETTING_RECO_BTN_X1, SETTING_RECO_BTN_Y, SETTING_RECO_BTN_WIDTH, SETTING_RECO_BTN_WIDTH);
    cancel_->SetTouchable(true);
    cancel_->SetViewId(CANCEL_BUTTON);
    cancel_->SetOnClickListener(this);
    cancel_->SetVisible(true);
    confirm_ = new UILabelButton();
    if (confirm_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitSecondConfirmFragment confirm_ new fail");
        return;
    }
    confirm_->SetPosition(SETTING_RECO_BTN_X2, SETTING_RECO_BTN_Y, SETTING_RECO_BTN_WIDTH, SETTING_RECO_BTN_WIDTH);
    confirm_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Blue().full);
    confirm_->SetViewId(CONFIRM_BUTTON);
    confirm_->SetOnClickListener(this);
    confirm_->SetVisible(true);
    LOADIMG::LoadBtnImage(confirm_, SETTING_IMAGE, IMAGE_OK_BLUE, IMAGE_OK_BLUE);
    confirm_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Blue().full, UIButton::PRESSED);
    confirm_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Blue().full, UIButton::RELEASED);
    confirm_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Blue().full, UIButton::INACTIVE);

    fragmentView_.Add(title_);
    fragmentView_.Add(contentFirst_);
    fragmentView_.Add(contentSecond_);
    fragmentView_.Add(contentThird_);
    fragmentView_.Add(cancel_);
    fragmentView_.Add(confirm_);
}

bool SettingReconnectSecondConfirm::OnClick(UIView &view, const ClickEvent &event)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingReconnectSecondConfirm OnClick");
    UNUSED(event);
    if (strcmp(view.GetViewId(), CANCEL_BUTTON) == 0) {
        fragmentView_.SetVisible(false);
        SettingBluetoothPage::GetInstance()->ChangeView(BT_MAIN_PAGE);
    } else if (strcmp(view.GetViewId(), CONFIRM_BUTTON) == 0) {
        bd_addr_t addr;
        BtCaseInfo *info = SettingBluetoothModel::GetInstance()->GetSelectedDeviceInfo();
        if (memcpy_s(addr.addr, BD_ADDR_LEN, info->addr, BD_ADDR_LEN) != EOK) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "memcpy_s failed");
            return false;
        }
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingReconnectSecondConfirmPage addr: %02x%02x%02x%02x%02x%02x",
            addr.addr[SETTING_ADDR5], addr.addr[SETTING_ADDR4], addr.addr[SETTING_ADDR3],
            addr.addr[SETTING_ADDR2], addr.addr[1], addr.addr[0]);
        if (SettingBluetoothModel::GetInstance()->GetBlueToothConnectStatus()) {
            errcode_t retVal = gap_disconnect_remote_device(&addr); // 断开连接
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Disconnect SelectedDevice retVal =  %d", retVal);
            if (retVal == 0) {
                SettingBluetoothModel::GetInstance()->UpdateDisconnectStatusForDevice(addr);
                SettingBluetoothModel::GetInstance()->SetConnectResultPage(false);
                SettingBluetoothModel::GetInstance()->SetBlueToothConnectStatus(false);
            }
        } else {
            if (SettingBluetoothModel::GetInstance()->ConnectRemoteDevice(&addr) == 0) {
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "connect SelectedDevice");
                SettingBluetoothModel::GetInstance()->SetLastConnedAddr(&addr);
                SettingBluetoothModel::GetInstance()->UpdateConnectStatusForDevice();
                SettingBluetoothModel::GetInstance()->SetConnectResultPage(true);
                SettingBluetoothModel::GetInstance()->SetBlueToothConnectStatus(true);
            }  else {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ConnectRemoteDevice failed");
            }
        }
        fragmentView_.SetVisible(false);
        SettingBluetoothPage::GetInstance()->ChangeView(BT_MAIN_PAGE);
    }
    return true;
}

void SettingReconnectSecondConfirm::RefreshFragment()
{
    if (contentFirst_ == nullptr || contentSecond_ == nullptr || contentThird_ == nullptr ||
        title_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "RefreshFragment fail");
        return;
    }
    std::string str1 = SettingBluetoothModel::GetInstance()->GetDeviceName();
    std::string str = "";
    std::string str2 = "";
    bool status = SettingBluetoothModel::GetInstance()->GetBlueToothConnectStatus();
    if (status) {
        str = "此操作将会断开您与";
        str2 = "以下设备的连接:";
        title_->SetText("确定断开连接?");
    } else {
        str = "此操作您将会与以下";
        str2 = "设备进行重新连接:";
        title_->SetText("确定重新连接?");
    }
    contentFirst_->SetText(str.c_str());
    contentSecond_->SetText(str2.c_str());
    contentThird_->SetText(str1.c_str());
}


bool SettingReconnectSecondConfirm::OnDragStart(UIView& view, const DragEvent& event)
{
    return true;
}

bool SettingReconnectSecondConfirm::OnDrag(UIView& view, const DragEvent& event)
{
    return true;
}

bool SettingReconnectSecondConfirm::OnDragEnd(UIView& view, const DragEvent& event)
{
    return true;
}
}
