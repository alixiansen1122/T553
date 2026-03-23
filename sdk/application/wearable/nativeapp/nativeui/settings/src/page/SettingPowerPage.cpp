/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingPowerPage
 * Create: 2025-06-01
 */
#include <sys/time.h>
#ifdef BUILD_IN_LINUX
#include "localtime.h"
#endif
#include "common/screen.h"
#include "components/ui_scroll_view_nested.h"
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "NativeAbility.h"
#include "settings/common/SettingCommon.h"
#include "settings/model/SettingPowerModel.h"
#include "settings/page/SettingPowerPage.h"

namespace OHOS {
REGIST_SLICE_PAGE(VIEW_SETTING, SettingPageId::POWER_PAGE, SettingPowerPage, false);

#define SETTING_POWER_LEVEL_ARRAY_SIZE 10
#define SETTING_VIEW_TEXT_SIZE 16
static constexpr int16_t SETTING_POWER_SCROLL_BLANK = 60;
static constexpr int16_t SETTING_POWER_TITLE_X = 100;
static constexpr int16_t SETTING_POWER_TITLE_Y = 39;
static constexpr int16_t SETTING_POWER_TITLE_WIDTH = 254;
static constexpr int16_t SETTING_POWER_TITLE_HEIGHT = 53;
static constexpr int16_t SETTING_POWER_TITLE_FSIZE = 38;
static const char* SETTING_POWER_TITLE_TEXT = "电量";

static constexpr int16_t SETTING_POWER_POWERDESC_X = 76;
static constexpr int16_t SETTING_POWER_POWERDESC_Y = 113;
static constexpr int16_t SETTING_POWER_POWERDESC_WIDTH = 193;
static constexpr int16_t SETTING_POWER_POWERDESC_HEIGHT = 55;
static constexpr int16_t SETTING_POWER_POWERDESC_FSIZE = 48;
static const char* SETTING_POWER_POWERDESC_TEXT = "当前电量";

static constexpr int16_t SETTING_POWER_BATTERYPERCENTAGE_X = 286;
static constexpr int16_t SETTING_POWER_BATTERYPERCENTAGE_Y = 113;
static constexpr int16_t SETTING_POWER_BATTERYPERCENTAGE_WIDTH = 122;
static constexpr int16_t SETTING_POWER_BATTERYPERCENTAGE_HEIGHT = 55;
static constexpr int16_t SETTING_POWER_BATTERYPERCENTAGE_FSIZE = 48;
#define SETTING_POWER_BATTERYPERCENTAGE_COLOR Color::GetColorFromRGB(9, 148, 69)

static constexpr int16_t SETTING_POWER_TIME_X = 145;
static constexpr int16_t SETTING_POWER_TIME_Y = 172;
static constexpr int16_t SETTING_POWER_TIME_WIDTH = 165;
static constexpr int16_t SETTING_POWER_TIME_HEIGHT = 55;
static constexpr int16_t SETTING_POWER_TIME_FSIZE = 36;

static constexpr int16_t SETTING_POWER_GROUP_X = 20;
static constexpr int16_t SETTING_POWER_GROUP_Y = 234;
static constexpr int16_t SETTING_POWER_GROUP_WIDTH = 414;
static constexpr int16_t SETTING_POWER_GROUP_HEIGHT = 91;
static constexpr int16_t SETTING_POWER_GROUP_RADIUS = 48;

static constexpr int16_t SETTING_POWER_GROUP_BATTERY_X = 34;
static constexpr int16_t SETTING_POWER_GROUP_BATTERY_Y = 29;
static constexpr int16_t SETTING_POWER_GROUP_BATTERY_WIDTH = 128;
static constexpr int16_t SETTING_POWER_GROUP_BATTERY_HEIGHT = 38;
static constexpr int16_t SETTING_POWER_GROUP_BATTERY_FSIZE = 32;
static const char* SETTING_POWER_GROUP_BATTERY_TEXT = "电池健康";

static constexpr int16_t SETTING_POWER_GROUP_BATTERY_LEVEL_X = 295;
static constexpr int16_t SETTING_POWER_GROUP_BATTERY_LEVEL_Y = 29;
static constexpr int16_t SETTING_POWER_GROUP_BATTERY_LEVEL_WIDTH = 84;
static constexpr int16_t SETTING_POWER_GROUP_BATTERY_LEVEL_HEIGHT = 38;
static constexpr int16_t SETTING_POWER_GROUP_BATTERY_LEVEL_FSIZE = 32;

static constexpr int16_t SETTING_POWER_DETAILSDESC_X = 59;
static constexpr int16_t SETTING_POWER_DETAILSDESC_Y = 365;
static constexpr int16_t SETTING_POWER_DETAILSDESC_WIDTH = 336;
static constexpr int16_t SETTING_POWER_DETAILSDESC_LINE_HEIGHT = 36;
static constexpr int16_t SETTING_POWER_DETAILSDESC_FSIZE = 24;

void SettingPowerPage::OnStart(void* data)
{
    scrollView_ = new UIScrollViewNested();
    if (scrollView_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingPowerPage::OnStart scrollView_ new fail");
        return;
    }
    scrollView_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    scrollView_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    scrollView_->SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
    scrollView_->SetScrollBlankSize(SETTING_POWER_SCROLL_BLANK, UIAbstractScroll::Direction::BOTTOM);
    scrollView_->SetThrowDrag(true);
    scrollView_->SetIntercept(true);
    InitPowerView();
    InitBatteryStatusView();
    InitNotify();
    batteryStatusGroup_->Add(batteryLabel_);
    batteryStatusGroup_->Add(batteryLevel_);
    scrollView_->Add(title_);
    scrollView_->Add(powerDesc_);
    scrollView_->Add(batteryPercentage_);
    scrollView_->Add(timeLabel_);
    scrollView_->Add(batteryStatusGroup_);
    scrollView_->Add(detailsDesc_);
    AddViewToPageContainer(scrollView_);
}

void SettingPowerPage::OnResume()
{
    char tempBuffer[SETTING_POWER_LEVEL_ARRAY_SIZE] = "";
    if (sprintf_s(tempBuffer, sizeof(tempBuffer), "%u%%",
        SettingPowerModel::GetInstance().GetBatteryPowerLevel()) > 0) {
        batteryPercentage_->SetText(tempBuffer);
        batteryLevel_->SetText(tempBuffer);
    }
    detailsDesc_->SetText(SettingPowerModel::GetInstance().GetPowerModelNotify());
    timeAnimator_->Start();
}

void SettingPowerPage::OnPause()
{
    timeAnimator_->Stop();
}

void SettingPowerPage::OnStop()
{
    if (scrollView_ != nullptr) {
        scrollView_->RemoveAll();
        delete scrollView_;
        scrollView_ = nullptr;
    }
    if (batteryStatusGroup_ != nullptr) {
        batteryStatusGroup_->RemoveAll();
        delete batteryStatusGroup_;
        batteryStatusGroup_ = nullptr;
    }
    if (title_ != nullptr) {
        delete title_;
        title_ = nullptr;
    }
    if (powerDesc_ != nullptr) {
        delete powerDesc_;
        powerDesc_ = nullptr;
    }
    if (batteryPercentage_ != nullptr) {
        delete batteryPercentage_;
        batteryPercentage_ = nullptr;
    }
    if (timeLabel_ != nullptr) {
        delete timeLabel_;
        timeLabel_ = nullptr;
    }
    if (batteryLabel_ != nullptr) {
        delete batteryLabel_;
        batteryLabel_ = nullptr;
    }
    if (batteryLevel_ != nullptr) {
        delete batteryLevel_;
        batteryLevel_ = nullptr;
    }
    if (detailsDesc_ != nullptr) {
        delete detailsDesc_;
        detailsDesc_ = nullptr;
    }
    if (timeAnimator_ != nullptr) {
        delete timeAnimator_;
        timeAnimator_ = nullptr;
    }
}

void SettingPowerPage::Callback(UIView* view)
{
    char temp[SETTING_VIEW_TEXT_SIZE];
    time_t seconds = time(nullptr);
    struct tm tm_data;
#if !defined(_WIN32)
    localtime_r(&seconds, &tm_data);
#else
    localtime_s(&tm_data, &seconds);
#endif
    if (sprintf_s(temp, sizeof(temp), "今天%02d:%02d", tm_data.tm_hour, tm_data.tm_min) > 0) {
        ((UILabel*)view)->SetText(temp);
    }
}

void SettingPowerPage::InitPowerView()
{
    title_ = new UILabel();
    if (title_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingPowerPage::InitPowerView title_ new fail");
        return;
    }
    title_->SetPosition(SETTING_POWER_TITLE_X, SETTING_POWER_TITLE_Y);
    title_->Resize(SETTING_POWER_TITLE_WIDTH, SETTING_POWER_TITLE_HEIGHT);
    title_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    title_->SetFont(BOLD_VECTOR_FONT_FILENAME, SETTING_POWER_TITLE_FSIZE);
    title_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    title_->SetText(SETTING_POWER_TITLE_TEXT);

    powerDesc_  = new UILabel();
    if (powerDesc_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingPowerPage::InitPowerView powerDesc_ new fail");
        return;
    }
    powerDesc_->SetPosition(SETTING_POWER_POWERDESC_X, SETTING_POWER_POWERDESC_Y);
    powerDesc_->Resize(SETTING_POWER_POWERDESC_WIDTH, SETTING_POWER_POWERDESC_HEIGHT);
    powerDesc_->SetAlign(TEXT_ALIGNMENT_RIGHT, TEXT_ALIGNMENT_CENTER);
    powerDesc_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_POWER_POWERDESC_FSIZE);
    powerDesc_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    powerDesc_->SetText(SETTING_POWER_POWERDESC_TEXT);

    batteryPercentage_ = new UILabel();
    if (batteryPercentage_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingPowerPage::InitPowerView batteryPercentage_ new fail");
        return;
    }
    batteryPercentage_->SetPosition(SETTING_POWER_BATTERYPERCENTAGE_X, SETTING_POWER_BATTERYPERCENTAGE_Y);
    batteryPercentage_->Resize(SETTING_POWER_BATTERYPERCENTAGE_WIDTH, SETTING_POWER_BATTERYPERCENTAGE_HEIGHT);
    batteryPercentage_->SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    batteryPercentage_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_POWER_BATTERYPERCENTAGE_FSIZE);
    batteryPercentage_->SetStyle(STYLE_TEXT_COLOR, SETTING_POWER_BATTERYPERCENTAGE_COLOR.full);

    timeLabel_ = new UILabel();
    if (timeLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingPowerPage::InitPowerView timeLabel_ new fail");
        return;
    }
    timeLabel_->SetPosition(SETTING_POWER_TIME_X, SETTING_POWER_TIME_Y);
    timeLabel_->Resize(SETTING_POWER_TIME_WIDTH, SETTING_POWER_TIME_HEIGHT);
    timeLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    timeLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_POWER_TIME_FSIZE);
    timeLabel_->SetStyle(STYLE_TEXT_COLOR, Color::Gray().full);
}

void SettingPowerPage::InitBatteryStatusView()
{
    batteryStatusGroup_ = new UIViewGroup();
    if (batteryStatusGroup_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingPowerPage::InitBatteryStatusView batteryStatusGroup_ new fail");
        return;
    }
    batteryStatusGroup_->SetPosition(SETTING_POWER_GROUP_X, SETTING_POWER_GROUP_Y);
    batteryStatusGroup_->Resize(SETTING_POWER_GROUP_WIDTH, SETTING_POWER_GROUP_HEIGHT);
    batteryStatusGroup_->SetStyle(STYLE_BACKGROUND_COLOR, SETTING_LIST_DEFAULT_BG_COLOR.full);
    batteryStatusGroup_->SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
    batteryStatusGroup_->SetStyle(STYLE_BORDER_RADIUS, SETTING_POWER_GROUP_RADIUS);

    batteryLabel_ = new UILabel();
    if (batteryLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingPowerPage::InitBatteryStatusView batteryLabel_ new fail");
        return;
    }
    batteryLabel_->SetPosition(SETTING_POWER_GROUP_BATTERY_X, SETTING_POWER_GROUP_BATTERY_Y);
    batteryLabel_->Resize(SETTING_POWER_GROUP_BATTERY_WIDTH, SETTING_POWER_GROUP_BATTERY_HEIGHT);
    batteryLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    batteryLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_POWER_GROUP_BATTERY_FSIZE);
    batteryLabel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    batteryLabel_->SetText(SETTING_POWER_GROUP_BATTERY_TEXT);

    batteryLevel_ = new UILabel();
    if (batteryLevel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingPowerPage::InitBatteryStatusView batteryLevel_ new fail");
        return;
    }
    batteryLevel_->SetPosition(SETTING_POWER_GROUP_BATTERY_LEVEL_X, SETTING_POWER_GROUP_BATTERY_LEVEL_Y);
    batteryLevel_->Resize(SETTING_POWER_GROUP_BATTERY_LEVEL_WIDTH, SETTING_POWER_GROUP_BATTERY_LEVEL_HEIGHT);
    batteryLevel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    batteryLevel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_POWER_GROUP_BATTERY_LEVEL_FSIZE);
    batteryLevel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
}

void SettingPowerPage::InitNotify()
{
    detailsDesc_ = new UILabel();
    if (detailsDesc_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingPowerPage::OnStart::batteryPercentage_ new fail");
        return;
    }
    detailsDesc_->SetPosition(SETTING_POWER_DETAILSDESC_X, SETTING_POWER_DETAILSDESC_Y);
    detailsDesc_->Resize(SETTING_POWER_DETAILSDESC_WIDTH, SETTING_POWER_DETAILSDESC_LINE_HEIGHT);
    detailsDesc_->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);
    detailsDesc_->SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    detailsDesc_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_POWER_DETAILSDESC_FSIZE);
    detailsDesc_->SetStyle(STYLE_LINE_HEIGHT, SETTING_POWER_DETAILSDESC_LINE_HEIGHT);
    detailsDesc_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);

    timeAnimator_ = new Animator(this, timeLabel_, 0, true);
    if (timeAnimator_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingPowerPage::OnStart::timeAnimator_ new fail");
        return;
    }
}
};