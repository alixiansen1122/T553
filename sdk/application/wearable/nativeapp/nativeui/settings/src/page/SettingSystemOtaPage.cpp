/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingSystemOtaPage
 * Create: 2025-06-01
 */
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "NativeAbility.h"
#include "settings/model/SettingSystemModel.h"
#include "settings/page/SettingSystemOtaPage.h"

namespace OHOS {
REGIST_SLICE_PAGE(VIEW_SETTING, SettingPageId::SYSTEM_OTA_PAGE, SettingSystemOtaPage, false);

#define SETTING_MID_UPGRADE_LABEL_TEXT_SIZE 4
#define SETTING_SYS_OTA_MIDPRORESS_BG_LINE_COLOR Color::GetColorFromRGB(217, 217, 217)
#define SETTING_SYS_OTA_MIDPRORESS_FG_LINE_COLOR Color::GetColorFromRGB(68, 132, 255)
static constexpr int16_t SETTING_SYS_OTA_MAX_PERCENTER = 100;
static constexpr int16_t SETTING_SYS_OTA_PREBUTTON_X = 154;
static constexpr int16_t SETTING_SYS_OTA_PREBUTTON_Y = 130;
static constexpr int16_t SETTING_SYS_OTA_PREBUTTON_WIDTH = 146;
static constexpr int16_t SETTING_SYS_OTA_PREBUTTON_HEIGHT = 146;

static constexpr int16_t SETTING_SYS_OTA_PRELABEL_X = 20;
static constexpr int16_t SETTING_SYS_OTA_PRELABEL_Y = 326;
static constexpr int16_t SETTING_SYS_OTA_PRELABEL_WIDTH = 414;
static constexpr int16_t SETTING_SYS_OTA_PRELABEL_HEIGHT = 44;
static constexpr int16_t SETTING_SYS_OTA_PRELABEL_FSIZE = 36;

static constexpr int16_t SETTING_SYS_OTA_MIDPRORESS_X = 89;
static constexpr int16_t SETTING_SYS_OTA_MIDPRORESS_Y = 92;
static constexpr int16_t SETTING_SYS_OTA_MIDPRORESS_WIDTH = 276;
static constexpr int16_t SETTING_SYS_OTA_MIDPRORESS_HEIGHT = 276;
static constexpr int16_t SETTING_SYS_OTA_MIDPRORESS_LINEWIDTH = 20;
static constexpr int16_t SETTING_SYS_OTA_MIDPRORESS_CENTPOS_X = 138;
static constexpr int16_t SETTING_SYS_OTA_MIDPRORESS_CENTPOS_Y = 138;
static constexpr int16_t SETTING_SYS_OTA_MIDPRORESS_RADUIS = 138;

static constexpr int16_t SETTING_SYS_OTA_MIDLABEL_X = 160;
static constexpr int16_t SETTING_SYS_OTA_MIDLABEL_Y = 163;
static constexpr int16_t SETTING_SYS_OTA_MIDLABEL_WIDTH = 102;
static constexpr int16_t SETTING_SYS_OTA_MIDLABEL_HEIGHT = 72;
static constexpr int16_t SETTING_SYS_OTA_MIDLABEL_FSIZE = 60;

static constexpr int16_t SETTING_SYS_OTA_MIDPERCENT_X = 160+102;
static constexpr int16_t SETTING_SYS_OTA_MIDPERCENT_Y = 187;
static constexpr int16_t SETTING_SYS_OTA_MIDPERCENT_WIDTH = 32;
static constexpr int16_t SETTING_SYS_OTA_MIDPERCENT_HEIGHT = 40;
static constexpr int16_t SETTING_SYS_OTA_MIDPERCENT_FSIZE = 36;
static const char* SETTING_SYS_OTA_MIDPERCENT_TEXT = "%";

static constexpr int16_t SETTING_SYS_OTA_MIDMESSAGE_X = 15;
static constexpr int16_t SETTING_SYS_OTA_MIDMESSAGE_Y = 246;
static constexpr int16_t SETTING_SYS_OTA_MIDMESSAGE_WIDTH = 424;
static constexpr int16_t SETTING_SYS_OTA_MIDMESSAGE_HEIGHT = 44;
static constexpr int16_t SETTING_SYS_OTA_MIDMESSAGE_FSIZE = 36;

void SettingSystemOtaPage::OnStart(void* data)
{
    InitPreGroupView();
    InitMidCircleView();
    InitMidMessageView();
    preUpgradeGroup_->Add(preUpgradeButton_);
    preUpgradeGroup_->Add(preUpgradeLabel_);
    midUpgradeGroup_->Add(midUpgradeProress_);
    midUpgradeGroup_->Add(midUpgradeLabel_);
    midUpgradeGroup_->Add(midUpgradePercent_);
    midUpgradeGroup_->Add(midUpgradeMessage_);
    AddViewToPageContainer(preUpgradeGroup_);
    AddViewToPageContainer(midUpgradeGroup_);
}

void SettingSystemOtaPage::OnResume()
{
    // 判断是否正在升级中
    if (!SettingSystemModel::GetInstance().GetOtaIsUpgrade()) {
        preUpgradeGroup_->SetVisible(true);
        midUpgradeGroup_->SetVisible(false);
        // 检查OTA升级包是否存在
        if (SettingSystemModel::GetInstance().GetOtaPackageSize() != 0) {
            preUpgradeButton_->Enable();
            ImageInfo* image =
                ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_SYSTEM_OTA_UPDATE, SETTING_IMAGE);
            preUpgradeButton_->SetImageSrc(image, image);
            preUpgradeLabel_->SetText("点击开始升级");
            preUpgradeButton_->SetOnClickListener(this);
        } else {
            ImageInfo* image =
                ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_SYSTEM_OTA_UNUPDATE, SETTING_IMAGE);
            preUpgradeButton_->Disable();
            preUpgradeButton_->SetImageSrc(image, image);
            preUpgradeLabel_->SetText("暂无可升级镜像");
            preUpgradeButton_->SetOnClickListener(nullptr);
        }
    } else {
        preUpgradeGroup_->SetVisible(false);
        midUpgradeGroup_->SetVisible(true);
        upgradeAnimator_->Start();
    }
}

void SettingSystemOtaPage::OnPause()
{
    upgradeAnimator_->Stop();
}

void SettingSystemOtaPage::OnStop()
{
    if (upgradeAnimator_ != nullptr) {
        delete upgradeAnimator_;
        upgradeAnimator_ = nullptr;
    }
    if (preUpgradeGroup_ != nullptr) {
        preUpgradeGroup_->RemoveAll();
        delete preUpgradeGroup_;
        preUpgradeGroup_ = nullptr;
    }
    if (midUpgradeGroup_ != nullptr) {
        midUpgradeGroup_->RemoveAll();
        delete midUpgradeGroup_;
        midUpgradeGroup_ = nullptr;
    }
    if (preUpgradeButton_ != nullptr) {
        delete preUpgradeButton_;
        preUpgradeButton_ = nullptr;
    }
    if (preUpgradeLabel_ != nullptr) {
        delete preUpgradeLabel_;
        preUpgradeLabel_ = nullptr;
    }
    if (midUpgradeProress_ != nullptr) {
        delete midUpgradeProress_;
        midUpgradeProress_ = nullptr;
    }
    if (midUpgradeLabel_ != nullptr) {
        delete midUpgradeLabel_;
        midUpgradeLabel_ = nullptr;
    }
    if (midUpgradePercent_ != nullptr) {
        delete midUpgradePercent_;
        midUpgradePercent_ = nullptr;
    }
    if (midUpgradeMessage_ != nullptr) {
        delete midUpgradeMessage_;
        midUpgradeMessage_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadOneInMultiRes(IMAGE_SYSTEM_OTA_UPDATE, SETTING_IMAGE);
    ImageCacheManager::GetInstance().UnloadOneInMultiRes(IMAGE_SYSTEM_OTA_UNUPDATE, SETTING_IMAGE);
}

bool SettingSystemOtaPage::OnClick(UIView &view, const ClickEvent &event)
{
    preUpgradeGroup_->SetVisible(false);
    midUpgradeGroup_->SetVisible(true);
    SettingSystemModel::GetInstance().StartOtaNativeUpgrade();
    upgradeAnimator_->Start();
    return true;
}

void SettingSystemOtaPage::Callback(UIView* view)
{
    char temp[SETTING_MID_UPGRADE_LABEL_TEXT_SIZE] = "";
    uint8_t percent = SettingSystemModel::GetInstance().GetOtaNativeUpgradePercent();
    std::string message = SettingSystemModel::GetInstance().GetOtaNativeUpgradeMessage();
    if (sprintf_s(temp, sizeof(temp), "%u", percent) > 0) {
        midUpgradeLabel_->SetText(temp);
    }
    midUpgradeProress_->SetValue(percent);
    midUpgradeMessage_->SetText(message.c_str());
    if (percent >= SETTING_SYS_OTA_MAX_PERCENTER || !SettingSystemModel::GetInstance().GetOtaIsUpgrade()) {
        upgradeAnimator_->Stop();
    }
}

void SettingSystemOtaPage::InitPreGroupView()
{
    upgradeAnimator_ = new Animator(this, nullptr, 0, true);
    if (upgradeAnimator_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingSystemOtaPage::InitPreGroupView upgradeAnimator_ new fail");
        return;
    }
    preUpgradeGroup_ = new UIViewGroup();
    if (preUpgradeGroup_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingSystemOtaPage::InitPreGroupView preUpgradeGroup_ new fail");
        return;
    }
    preUpgradeGroup_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    preUpgradeGroup_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    preUpgradeGroup_->SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);

    preUpgradeButton_ = new UIButton();
    if (preUpgradeButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingSystemOtaPage::InitPreGroupView preUpgradeButton_ new fail");
        return;
    }
    preUpgradeButton_->SetPosition(SETTING_SYS_OTA_PREBUTTON_X, SETTING_SYS_OTA_PREBUTTON_Y);
    preUpgradeButton_->Resize(SETTING_SYS_OTA_PREBUTTON_WIDTH, SETTING_SYS_OTA_PREBUTTON_WIDTH);
    preUpgradeButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::RELEASED);
    preUpgradeButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);

    preUpgradeLabel_ = new UILabel();
    if (preUpgradeLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingSystemOtaPage::InitPreGroupView preUpgradeLabel_ new fail");
        return;
    }
    preUpgradeLabel_->SetPosition(SETTING_SYS_OTA_PRELABEL_X, SETTING_SYS_OTA_PRELABEL_Y);
    preUpgradeLabel_->Resize(SETTING_SYS_OTA_PRELABEL_WIDTH, SETTING_SYS_OTA_PRELABEL_HEIGHT);
    preUpgradeLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    preUpgradeLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_SYS_OTA_PRELABEL_FSIZE);
    preUpgradeLabel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
}

void SettingSystemOtaPage::InitMidCircleView()
{
    midUpgradeGroup_ = new UIViewGroup();
    if (midUpgradeGroup_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingSystemOtaPage::InitMidCircleView midUpgradeGroup_ new fail");
        return;
    }
    midUpgradeGroup_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    midUpgradeGroup_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    midUpgradeGroup_->SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);

    midUpgradeProress_ = new UICircleProgress();
    if (midUpgradeProress_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingSystemOtaPage::InitMidCircleView preUpgradeButton_ new fail");
        return;
    }
    midUpgradeProress_->SetPosition(SETTING_SYS_OTA_MIDPRORESS_X, SETTING_SYS_OTA_MIDPRORESS_Y);
    midUpgradeProress_->Resize(SETTING_SYS_OTA_MIDPRORESS_WIDTH, SETTING_SYS_OTA_MIDPRORESS_HEIGHT);
    midUpgradeProress_->SetBackgroundStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    midUpgradeProress_->SetBackgroundStyle(STYLE_LINE_COLOR, SETTING_SYS_OTA_MIDPRORESS_BG_LINE_COLOR.full);
    midUpgradeProress_->SetBackgroundStyle(STYLE_LINE_WIDTH, SETTING_SYS_OTA_MIDPRORESS_LINEWIDTH);
    midUpgradeProress_->SetForegroundStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    midUpgradeProress_->SetForegroundStyle(STYLE_LINE_COLOR, SETTING_SYS_OTA_MIDPRORESS_FG_LINE_COLOR.full);
    midUpgradeProress_->SetForegroundStyle(STYLE_LINE_WIDTH, SETTING_SYS_OTA_MIDPRORESS_LINEWIDTH);
    midUpgradeProress_->SetCenterPosition(SETTING_SYS_OTA_MIDPRORESS_CENTPOS_X, SETTING_SYS_OTA_MIDPRORESS_CENTPOS_Y);
    midUpgradeProress_->SetRadius(SETTING_SYS_OTA_MIDPRORESS_RADUIS);
}

void SettingSystemOtaPage::InitMidMessageView()
{
    midUpgradeLabel_ = new UILabel();
    if (midUpgradeLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingSystemOtaPage::InitMidMessageView preUpgradeLabel_ new fail");
        return;
    }
    midUpgradeLabel_->SetPosition(SETTING_SYS_OTA_MIDLABEL_X, SETTING_SYS_OTA_MIDLABEL_Y);
    midUpgradeLabel_->Resize(SETTING_SYS_OTA_MIDLABEL_WIDTH, SETTING_SYS_OTA_MIDLABEL_HEIGHT);
    midUpgradeLabel_->SetAlign(TEXT_ALIGNMENT_RIGHT, TEXT_ALIGNMENT_CENTER);
    midUpgradeLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_SYS_OTA_MIDLABEL_FSIZE);
    midUpgradeLabel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);

    midUpgradePercent_ = new UILabel();
    if (midUpgradePercent_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingSystemOtaPage::InitMidMessageView midUpgradePercent_ new fail");
        return;
    }
    midUpgradePercent_->SetPosition(SETTING_SYS_OTA_MIDPERCENT_X, SETTING_SYS_OTA_MIDPERCENT_Y);
    midUpgradePercent_->Resize(SETTING_SYS_OTA_MIDPERCENT_WIDTH, SETTING_SYS_OTA_MIDPERCENT_HEIGHT);
    midUpgradePercent_->SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    midUpgradePercent_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_SYS_OTA_MIDPERCENT_FSIZE);
    midUpgradePercent_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    midUpgradePercent_->SetText("%");

    midUpgradeMessage_ = new UILabel();
    if (midUpgradeMessage_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingSystemOtaPage::InitMidMessageView midUpgradeMessage_ new fail");
        return;
    }
    midUpgradeMessage_->SetPosition(SETTING_SYS_OTA_MIDMESSAGE_X, SETTING_SYS_OTA_MIDMESSAGE_Y);
    midUpgradeMessage_->Resize(SETTING_SYS_OTA_MIDMESSAGE_WIDTH, SETTING_SYS_OTA_MIDMESSAGE_HEIGHT);
    midUpgradeMessage_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    midUpgradeMessage_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_SYS_OTA_MIDMESSAGE_FSIZE);
    midUpgradeMessage_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
}
}