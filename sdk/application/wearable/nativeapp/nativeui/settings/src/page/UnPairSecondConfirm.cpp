/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: UnPairSecondConfirm
 * Created: 2025-06-05
 */

#include "wearable_log.h"
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "NativeAbility.h"
#include "AppViewIDs.h"
#include "ui_resource_settings.h"
#include "main/LoadImg.h"
#include "settings/page/SettingBluetoothPage.h"
#include "settings/model/SettingBluetoothModel.h"
#include "settings/page/UnPairSecondConfirm.h"

namespace OHOS {
static constexpr int16_t SETTING_ADDR2 = 2;
static constexpr int16_t SETTING_ADDR3 = 3;
static constexpr int16_t SETTING_ADDR4 = 4;
static constexpr int16_t SETTING_ADDR5 = 5;
static constexpr int16_t SETTING_UNPAIR_TITLE_Y = 60;
static constexpr int16_t SETTING_UNPAIR_TITLE_HEIGHT = 40;
static constexpr int16_t SETTING_UNPAIR_TITLE_FONT = 38;
static constexpr int16_t SETTING_UNPAIR_CONTENT1_X = 30;
static constexpr int16_t SETTING_UNPAIR_CONTENT1_Y = 135;
static constexpr int16_t SETTING_UNPAIR_CONTENT2_Y = 180;
static constexpr int16_t SETTING_UNPAIR_CONTENT3_Y = 225;
static constexpr int16_t SETTING_UNPAIR_CONYENT_WIDTH = 394;
static constexpr int16_t SETTING_UNPAIR_CONYENT_HEIGHT = 50;
static constexpr int16_t SETTING_UNPAIR_CONTENT2_HEIGHT = 115;
static constexpr int16_t SETTING_UNPAIR_CONTENT1_FONT = 35;
static constexpr int16_t SETTING_UNPAIR_BTN_WIDTH = 85;
static constexpr int16_t SETTING_UNPAIR_CANCEL_Y = 300;
static constexpr int16_t SETTING_UNPAIR_CANCEL_X = 114;
static constexpr int16_t SETTING_UNPAIR_CONFIRM_X = 254;
static constexpr char *CONFIRM_BUTTON = "confirmButton";
static constexpr char *CANCEL_BUTTON = "cancelButton";

void UnPairSecondConfirm::OnCreateView(void* data)
{
    InitUnpairFragment();
}

void UnPairSecondConfirm::OnDestroyView()
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
    if (watchNameLabel_ != nullptr) {
        delete watchNameLabel_;
        watchNameLabel_ = nullptr;
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

void UnPairSecondConfirm::InitUnpairFragment()
{
    fragmentView_.SetTouchable(true);
    fragmentView_.SetDraggable(true);
    fragmentView_.SetOnDragListener(this);
    title_ = new UILabel();
    if (title_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "UnPairSecondConfirm::InitUnpairFragment title_ new fail");
        return;
    }
    title_->SetPosition(0, SETTING_UNPAIR_TITLE_Y, RESOLUTION_WIDTH, SETTING_UNPAIR_TITLE_HEIGHT);
    title_->SetText("确定取消配对?");
    title_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    title_->SetFont(BOLD_VECTOR_FONT_FILENAME, SETTING_UNPAIR_TITLE_FONT);
    title_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    std::string str1 = SettingBluetoothModel::GetInstance()->GetDeviceName();
    std::string str = "此操作将会取消您与";
    std::string str2 = "以下设备的配对:";
    contentFirst_ = new UILabel();
    if (contentFirst_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "UnPairSecondConfirm::InitUnpairFragment contentFirst_ new fail");
        return;
    }
    contentFirst_->SetPosition(0, SETTING_UNPAIR_CONTENT1_Y,
        RESOLUTION_WIDTH, SETTING_UNPAIR_CONYENT_HEIGHT);
    contentFirst_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    contentFirst_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_UNPAIR_CONTENT1_FONT);
    contentFirst_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    contentFirst_->SetText(str.c_str());

    contentSecond_ = new UILabel();
    if (contentSecond_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "UnPairSecondConfirm::InitUnpairFragment contentSecond_ new fail");
        return;
    }
    contentSecond_->SetPosition(SETTING_UNPAIR_CONTENT1_X, SETTING_UNPAIR_CONTENT2_Y,
        SETTING_UNPAIR_CONYENT_WIDTH, SETTING_UNPAIR_CONYENT_HEIGHT);
    contentSecond_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    contentSecond_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_UNPAIR_CONTENT1_FONT);
    contentSecond_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    contentSecond_->SetText(str2.c_str());

    watchNameLabel_ = new UILabel();
    if (watchNameLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "UnPairSecondConfirm::InitUnpairFragment watchNameLabel_ new fail");
        return;
    }
    watchNameLabel_->SetPosition(0, SETTING_UNPAIR_CONTENT3_Y,
        RESOLUTION_WIDTH, SETTING_UNPAIR_CONYENT_HEIGHT);
    watchNameLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    watchNameLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_UNPAIR_CONTENT1_FONT);
    watchNameLabel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    watchNameLabel_->SetText(str1.c_str());

    cancel_ = new UIImageView();
    if (cancel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "UnPairSecondConfirm::InitUnpairFragment cancel_ new fail");
        return;
    }
    LOADIMG::LoadImageViewImg(cancel_, SETTING_IMAGE, IMAGE_CANCEL);
    cancel_->SetPosition(SETTING_UNPAIR_CANCEL_X,
        SETTING_UNPAIR_CANCEL_Y, SETTING_UNPAIR_BTN_WIDTH, SETTING_UNPAIR_BTN_WIDTH);
    cancel_->SetTouchable(true);
    cancel_->SetViewId(CANCEL_BUTTON);
    cancel_->SetOnClickListener(this);
    cancel_->SetVisible(true);
    confirm_ = new UILabelButton();
    if (confirm_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "UnPairSecondConfirm::InitUnpairFragment confirm_ new fail");
        return;
    }
    confirm_->SetPosition(SETTING_UNPAIR_CONFIRM_X,
        SETTING_UNPAIR_CANCEL_Y, SETTING_UNPAIR_BTN_WIDTH, SETTING_UNPAIR_BTN_WIDTH);
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
    fragmentView_.Add(watchNameLabel_);
    fragmentView_.Add(cancel_);
    fragmentView_.Add(confirm_);
}

void UnPairSecondConfirm::RefreshFragment()
{
    if (watchNameLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "UnPairSecondConfirm::RefreshFragment fail");
        return;
    }
    std::string str1 = SettingBluetoothModel::GetInstance()->GetDeviceName();
    watchNameLabel_->SetText(str1.c_str());
}

bool UnPairSecondConfirm::OnClick(UIView &view, const ClickEvent &event)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "UnPairSecondConfirm::OnClick");
    UNUSED(event);
    if (strcmp(view.GetViewId(), CANCEL_BUTTON) == 0) {
        // 此处仅退出页面，耳机仍存在，需要通知主页面 此时存在耳机列表，蓝牙主页通过该变量做出对应的显示
        fragmentView_.SetVisible(false);
        SettingBluetoothPage::GetInstance()->ChangeView(BT_MAIN_PAGE);
    } else if (strcmp(view.GetViewId(), CONFIRM_BUTTON) == 0) {
        SettingBluetoothModel::GetInstance()->HandleUnpairRequest();
        bd_addr_t clickAddr;
        (void)memcpy_s(clickAddr.addr, BD_ADDR_LEN,
            SettingBluetoothModel::GetInstance()->GetSelectedDeviceInfo()->addr, BD_ADDR_LEN);
        std::list<BtCaseInfo> &bluetoothCaseList = SettingBluetoothModel::GetInstance()->GetScansDevicesList();
        for (auto it = bluetoothCaseList.begin(); it != bluetoothCaseList.end(); it++)  {
            if ((clickAddr.addr[0] == it->addr[0]) && (clickAddr.addr[1] == it->addr[1])
                && (clickAddr.addr[SETTING_ADDR2] == it->addr[SETTING_ADDR2])
                && (clickAddr.addr[SETTING_ADDR3] == it->addr[SETTING_ADDR3])
                && (clickAddr.addr[SETTING_ADDR4] == it->addr[SETTING_ADDR4])
                && (clickAddr.addr[SETTING_ADDR5] == it->addr[SETTING_ADDR5])) {
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "OnClick unpair device_name:%s", it->deviceName);
                bluetoothCaseList.erase(it);
            }
        }
        fragmentView_.SetVisible(false);
        SettingBluetoothPage::GetInstance()->LoadFindDeviceList();
        SettingBluetoothPage::GetInstance()->ChangeView(BT_MAIN_PAGE);
    }
    return true;
}

bool UnPairSecondConfirm::OnDragStart(UIView& view, const DragEvent& event)
{
    return true;
}

bool UnPairSecondConfirm::OnDrag(UIView& view, const DragEvent& event)
{
    return true;
}

bool UnPairSecondConfirm::OnDragEnd(UIView& view, const DragEvent& event)
{
    return true;
}
}
