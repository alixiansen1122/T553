/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingAboutPage
 * Create: 2025-06-01
 */
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "NativeAbility.h"
#include "AppViewIDs.h"
#include "wearable_log.h"
#include "settings/model/SettingAboutModel.h"
#include "settings/page/SettingAboutPage.h"

namespace OHOS {
REGIST_SLICE_PAGE(VIEW_SETTING, SettingPageId::ABOUT_PAGE, SettingAboutPage, false);

static constexpr int16_t SETTING_ABOUT_DEFAULT_X = 78;
static constexpr int16_t SETTING_ABOUT_DEFAULT_WIDTH = 298;
static constexpr int16_t SETTING_ABOUT_DEFAULT_DATA_OPA = 168; // 66%

static const char* SETTING_ABOUT_TITLE_TEXT = "关于";
static const char* SETTING_ABOUT_DEVICENAME_TEXT = "设备名称";
static const char* SETTING_ABOUT_DEVICETYPE_TEXT = "型号";

static constexpr int16_t SETTING_ABOUT_TITLE_X = 100;
static constexpr int16_t SETTING_ABOUT_TITLE_Y = 39;
static constexpr int16_t SETTING_ABOUT_TITLE_WIDTH = 254;
static constexpr int16_t SETTING_ABOUT_TITLE_HEIGHT = 53;
static constexpr int16_t SETTING_ABOUT_TITLE_FSIZE = 38;

static constexpr int16_t SETTING_ABOUT_DEVICENAME_Y = 134;
static constexpr int16_t SETTING_ABOUT_DEVICENAME_HEIGHT = 54;
static constexpr int16_t SETTING_ABOUT_DEVICENAME_FSIZE = 38;

static constexpr int16_t SETTING_ABOUT_DEVICETYPE_Y = 268;
static constexpr int16_t SETTING_ABOUT_DEVICETYPE_HEIGHT = 54;
static constexpr int16_t SETTING_ABOUT_DEVICETYPE_FSIZE = 38;

static constexpr int16_t SETTING_ABOUT_DEVICENAME_DATA_Y = 192;
static constexpr int16_t SETTING_ABOUT_DEVICENAME_DATA_HEIGHT = 45;
static constexpr int16_t SETTING_ABOUT_DEVICENAME_DATA_FSIZE = 32;

static constexpr int16_t SETTING_ABOUT_DEVICETYPE_DATA_Y = 322;
static constexpr int16_t SETTING_ABOUT_DEVICETYPE_DATA_HEIGHT = 45;
static constexpr int16_t SETTING_ABOUT_DEVICETYPE_DATA_FSIZE = 32;

void SettingAboutPage::OnStart(void* data)
{
    InitTitle();
    InitDeviceNameView();
    InitDeviceTypeView();
    AddViewToPageContainer(title_);
    AddViewToPageContainer(deviceNameTitle_);
    AddViewToPageContainer(deviceTypeTitle_);
    AddViewToPageContainer(deviceName_);
    AddViewToPageContainer(deviceType_);
}

void SettingAboutPage::OnResume()
{
    deviceName_->SetText(SettingAboutModel::GetInstance().GetSystemName().c_str());
    deviceType_->SetText(SettingAboutModel::GetInstance().GetSystemType().c_str());
}

void SettingAboutPage::OnStop()
{
    if (title_ != nullptr) {
        delete title_;
        title_ = nullptr;
    }
    if (deviceNameTitle_ != nullptr) {
        delete deviceNameTitle_;
        deviceNameTitle_ = nullptr;
    }
    if (deviceTypeTitle_ != nullptr) {
        delete deviceTypeTitle_;
        deviceTypeTitle_ = nullptr;
    }
    if (deviceName_ != nullptr) {
        delete deviceName_;
        deviceName_ = nullptr;
    }
    if (deviceType_ != nullptr) {
        delete deviceType_;
        deviceType_ = nullptr;
    }
}

void SettingAboutPage::InitTitle()
{
    title_ = new UILabel();
    if (title_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingAboutPage::OnStart title_ new fail");
        return;
    }
    title_->SetPosition(SETTING_ABOUT_TITLE_X, SETTING_ABOUT_TITLE_Y);
    title_->Resize(SETTING_ABOUT_TITLE_WIDTH, SETTING_ABOUT_TITLE_HEIGHT);
    title_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    title_->SetFont(BOLD_VECTOR_FONT_FILENAME, SETTING_ABOUT_TITLE_FSIZE);
    title_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    title_->SetText(SETTING_ABOUT_TITLE_TEXT);
}

void SettingAboutPage::InitDeviceTypeView()
{
    deviceTypeTitle_ = new UILabel();
    if (deviceTypeTitle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingAboutPage::OnStart deviceTypeTitle_ new fail");
        return;
    }
    deviceTypeTitle_->SetPosition(SETTING_ABOUT_DEFAULT_X, SETTING_ABOUT_DEVICETYPE_Y);
    deviceTypeTitle_->Resize(SETTING_ABOUT_DEFAULT_WIDTH, SETTING_ABOUT_DEVICETYPE_HEIGHT);
    deviceTypeTitle_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    deviceTypeTitle_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_ABOUT_DEVICETYPE_FSIZE);
    deviceTypeTitle_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    deviceTypeTitle_->SetText(SETTING_ABOUT_DEVICETYPE_TEXT);

    deviceType_ = new UILabel();
    if (deviceType_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingAboutPage::OnStart deviceType_ new fail");
        return;
    }
    deviceType_->SetPosition(SETTING_ABOUT_DEFAULT_X, SETTING_ABOUT_DEVICETYPE_DATA_Y);
    deviceType_->Resize(SETTING_ABOUT_DEFAULT_WIDTH, SETTING_ABOUT_DEVICETYPE_DATA_HEIGHT);
    deviceType_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    deviceType_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_ABOUT_DEVICETYPE_DATA_FSIZE);
    deviceType_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    deviceType_->SetStyle(STYLE_TEXT_OPA, SETTING_ABOUT_DEFAULT_DATA_OPA);
}

void SettingAboutPage::InitDeviceNameView()
{
    deviceNameTitle_ = new UILabel();
    if (deviceNameTitle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingAboutPage::OnStart deviceNameTitle_ new fail");
        return;
    }
    deviceNameTitle_->SetPosition(SETTING_ABOUT_DEFAULT_X, SETTING_ABOUT_DEVICENAME_Y);
    deviceNameTitle_->Resize(SETTING_ABOUT_DEFAULT_WIDTH, SETTING_ABOUT_DEVICENAME_HEIGHT);
    deviceNameTitle_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    deviceNameTitle_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_ABOUT_DEVICENAME_FSIZE);
    deviceNameTitle_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    deviceNameTitle_->SetText(SETTING_ABOUT_DEVICENAME_TEXT);

    deviceName_ = new UILabel();
    if (deviceName_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingAboutPage::OnStart deviceName_ new fail");
        return;
    }
    deviceName_->SetPosition(SETTING_ABOUT_DEFAULT_X, SETTING_ABOUT_DEVICENAME_DATA_Y);
    deviceName_->Resize(SETTING_ABOUT_DEFAULT_WIDTH, SETTING_ABOUT_DEVICENAME_DATA_HEIGHT);
    deviceName_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    deviceName_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_ABOUT_DEVICENAME_DATA_FSIZE);
    deviceName_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    deviceName_->SetStyle(STYLE_TEXT_OPA, SETTING_ABOUT_DEFAULT_DATA_OPA);
}
}
