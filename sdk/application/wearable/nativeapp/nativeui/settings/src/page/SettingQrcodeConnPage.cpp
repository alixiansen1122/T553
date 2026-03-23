/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingQrcodeConnPage
 * Created: 2025-06-05
 */

#include "wearable_log.h"
#include "SlicePageFactory.h"
#include "settings/model/SettingBluetoothModel.h"
#include "settings/common/SettingCommon.h"
#include "settings/model/SettingQrcodeConnModel.h"
#include "settings/page/SettingQrcodeConnPage.h"

namespace OHOS {
REGIST_SLICE_PAGE(VIEW_SETTING, SettingPageId::QRCODE_CONN_PAGE, SettingQrcodeConnPage, false);
static const uint32_t QRCODE_POS_X = 101;
static const uint32_t QRCODE_POS_Y = 85;
static const uint32_t QRCODE_WIDTH = 232;
static const uint32_t QRCODE_HEIGHT = 232;
static const uint32_t PAD_SIZE = 10;
static const uint32_t QRCODE_CONTENT_POS_Y = 350;
static const uint32_t QRCODE_CONTENT_HEIGHT = 50;
static const uint32_t QRCODE_CONTENT_FONT = 36;

SettingQrcodeConnPage::SettingQrcodeConnPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingQrcodeConnPage::SettingQrcodeConnPage");
}

SettingQrcodeConnPage::~SettingQrcodeConnPage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }
    if (qrcode_ != nullptr) {
        delete qrcode_;
        qrcode_ = nullptr;
    }
    if (content_ != nullptr) {
        delete content_;
        content_ = nullptr;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingQrcodeConnPage::~SettingQrcodeConnPage");
}

void SettingQrcodeConnPage::InitView()
{
    group_ = new UIScrollViewNested();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingQrcodeConnPage::InitView::group_ new fail");
        return;
    }
    group_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    group_->SetStyle(STYLE_BACKGROUND_OPA, 0);

    qrcode_ = new UIQrcode();
    if (qrcode_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingQrcodeConnPage::InitView::qrcode_ new fail");
        return;
    }
    qrcode_->SetPosition(QRCODE_POS_X, QRCODE_POS_Y, QRCODE_WIDTH, QRCODE_HEIGHT);
    qrcode_->SetQrcodeInfo(SettingBluetoothModel::GetInstance()->GetWatchName());
    qrcode_->SetStyle(STYLE_PADDING_LEFT, PAD_SIZE);
    qrcode_->SetStyle(STYLE_PADDING_RIGHT, PAD_SIZE);
    qrcode_->SetStyle(STYLE_PADDING_TOP, PAD_SIZE);
    qrcode_->SetStyle(STYLE_PADDING_BOTTOM, PAD_SIZE);
    group_->Add(qrcode_);

    content_ = new UILabel();
    if (content_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingQrcodeConnPage::InitView::content_ new fail");
        return;
    }
    content_->SetPosition(0, QRCODE_CONTENT_POS_Y, RESOLUTION_WIDTH, QRCODE_CONTENT_HEIGHT);
    content_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    content_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, QRCODE_CONTENT_FONT);
    content_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    content_->SetText("扫码连接设备");
    group_->Add(content_);
    AddViewToPageContainer(group_);
}

void SettingQrcodeConnPage::OnStart(void *data)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingQrcodeConnPage::OnStart");
    InitView();
}
}
