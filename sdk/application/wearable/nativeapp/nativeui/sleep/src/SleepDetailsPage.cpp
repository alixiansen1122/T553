/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SleepDetailsPage
 * Created: 2025-06-05
 */
#include <sys/time.h>
#include <string>
#include "ohos_timer.h"
#include "wearable_log.h"
#include "SlicePageFactory.h"
#include "common/image_cache_manager.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "sleep/SleepView.h"
#include "sleep/SleepModel.h"
#include "sleep/SleepDetailsPage.h"

namespace OHOS {
static constexpr int16_t SLEEPDETAIL_SLEEP_X = 97;
static constexpr int16_t SLEEPDETAIL_SLEEP_Y = 31;
static constexpr int16_t SLEEPDETAIL_SLEEP_WIDTH = 260;
static constexpr int16_t SLEEPDETAIL_SLEEP_HEIGHT = 53;
static constexpr int16_t SLEEPDETAIL_SLEEP_FONT_SIZE = 38;
static constexpr int16_t SLEEPDETAIL_IMAGE1_X = 77;
static constexpr int16_t SLEEPDETAIL_IMAGE1_Y = 215;
static constexpr int16_t SLEEPDETAIL_SOBERINGTIME_X = 103;
static constexpr int16_t SLEEPDETAIL_SOBERINGTIME_Y = 205;
static constexpr int16_t SLEEPDETAIL_SOBERINGTIME_WIDTH = 80;
static constexpr int16_t SLEEPDETAIL_SOBERINGTIME_HEIGHT = 29;
static constexpr int16_t SLEEPDETAIL_SOBERINGTIME_FONT_SIZE = 28;
static constexpr int16_t SLEEPDETAIL_IMAGE2_Y = 254;
static constexpr int16_t SLEEPDETAIL_IMAGE3_Y = 291;
static constexpr int16_t SLEEPDETAIL_IMAGE4_Y = 330;
static constexpr int16_t SLEEPDETAIL_RAPID_Y = 244;
static constexpr int16_t SLEEPDETAIL_RAPID_WIDTH = 150;
static constexpr int16_t SLEEPDETAIL_RAPID_HEIGHT = 38;
static constexpr int16_t SLEEPDETAIL_LIGHT_Y = 283;
static constexpr int16_t SLEEPDETAIL_DEEP_Y = 322;
static constexpr int16_t SLEEPDETAIL_GRAYBG_X = 62;
static constexpr int16_t SLEEPDETAIL_GRAYBG_Y = 185;
static constexpr int16_t SLEEPDETAIL_ACTUALSLEEP_X = 180;
static constexpr int16_t SLEEPDETAIL_ACTUAL_NORECODE_X = 96;
static constexpr int16_t SLEEPDETAIL_ACTUALSLEEP_Y = 100;
static constexpr int16_t SLEEPDETAIL_ACTUALSLEEP_WIDTH = 200;
static constexpr int16_t SLEEPDETAIL_ACTUALSLEEP_HEIGHT = 35;
static constexpr int16_t SLEEPDETAIL_ACTUALSLEEP_FONT_SIZE = 32;
static constexpr int16_t SLEEPDETAIL_LABEL_WIDTH = 160;
static constexpr int16_t SLEEPDETAIL_PADDING_RIGHT = 30;
static constexpr int16_t SLEEPDETAIL_SOBERINGTEXT_X = 220;
static constexpr int16_t SLEEPDETAIL_ACTUALSLEEP_NORECORD_X = 179;
static constexpr int16_t SLEEPDETAIL_ACTUALSLEEP_NORECORD_Y = 110;
static constexpr int16_t SLEEPDETAIL_SOBERTIMETEXT_X = 300;
static constexpr int16_t SLEEPDETAIL_SOBERTIMETEXT_Y = 190;
static constexpr int16_t SLEEPDETAIL_RAPIDTIMETEXT_Y = 230;
static constexpr int16_t SLEEPDETAIL_LIGHTIMETEXT_Y = 270;
static constexpr int16_t SLEEPDETAIL_DEEPIMETEXT_Y = 310;
static constexpr int16_t SLEEPDETAIL_ACTUALSLEEPTIMEHOURS_FONT_SIZE = 40;
static constexpr int16_t SLEEPDETAIL_ACTUALSLEEPTIMINUTES_HEIGHT = 50;
static constexpr int16_t SLEEPDETAIL_ACTUALSLEEPTIMINUTES_FONT_SIZE = 40;
static constexpr int16_t SLEEPDETAIL_ACTUALMINUTE_X = 230;
static constexpr int16_t SLEEPDETAIL_ACTUALMINUTE_Y = 116;
static constexpr int16_t SLEEPDETAIL_ACTUALHOUR_X = 135;
static constexpr int16_t SLEEPDETAIL_ACTUALHOUR_Y = 100;
static constexpr int16_t SLEEPDETAIL_ACTUALHOURTEXT_X = 170;
static constexpr int16_t SLEEPDETAIL_ACTUALMINUTETEXT_X = 280;
static constexpr int16_t ACTUALHOURS_X = 140;
static constexpr int16_t ACTUALHOURSTEXT_X = 180;
static constexpr int16_t ACTUALMINUTESTEXT_X = 270;

REGIST_SLICE_PAGE(VIEW_SLEEPING, SLEEPING_PAGES::SLEEPING_DEATIL_PAGE, SleepDetailsPage, false);

SleepDetailsPage::SleepDetailsPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SleepDetailsPage::SleepDetailsPage");
}

SleepDetailsPage::~SleepDetailsPage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }

    if (bgImg_ != nullptr) {
        delete bgImg_;
        bgImg_ = nullptr;
    }

    if (sleepTitleLabel_ != nullptr) {
        delete sleepTitleLabel_;
        sleepTitleLabel_ = nullptr;
    }

    if (actualSleepNorecordTimeLabel_ != nullptr) {
        delete actualSleepNorecordTimeLabel_;
        actualSleepNorecordTimeLabel_ = nullptr;
    }

    if (actualSleepTimeHoursLabel_ != nullptr) {
        delete actualSleepTimeHoursLabel_;
        actualSleepTimeHoursLabel_ = nullptr;
    }

    if (actualSleepTimeHoursUnitLabel_ != nullptr) {
        delete actualSleepTimeHoursUnitLabel_;
        actualSleepTimeHoursUnitLabel_ = nullptr;
    }

    if (actualSleepTimeMinutesLabel_ != nullptr) {
        delete actualSleepTimeMinutesLabel_;
        actualSleepTimeMinutesLabel_ = nullptr;
    }

    if (actualSleepTimeMinutesUnitLabel_ != nullptr) {
        delete actualSleepTimeMinutesUnitLabel_;
        actualSleepTimeMinutesUnitLabel_ = nullptr;
    }

    if (grayBgImg_ != nullptr) {
        delete grayBgImg_;
        grayBgImg_ = nullptr;
    }

    if (redPointImg_ != nullptr) {
        delete redPointImg_;
        redPointImg_ = nullptr;
    }

    if (soberingTimeLabel_ != nullptr) {
        delete soberingTimeLabel_;
        soberingTimeLabel_ = nullptr;
    }

    if (soberingTimeUnitLabel_ != nullptr) {
        delete soberingTimeUnitLabel_;
        soberingTimeUnitLabel_ = nullptr;
    }

    if (orangePointImg_ != nullptr) {
        delete orangePointImg_;
        orangePointImg_ = nullptr;
    }

    if (rapidEyeMovementLabel_ != nullptr) {
        delete rapidEyeMovementLabel_;
        rapidEyeMovementLabel_ = nullptr;
    }

    if (rapidEyeMovementUnitLabel_ != nullptr) {
        delete rapidEyeMovementUnitLabel_;
        rapidEyeMovementUnitLabel_ = nullptr;
    }

    if (greenPointImg_ != nullptr) {
        delete greenPointImg_;
        greenPointImg_ = nullptr;
    }

    if (lightSleepTimeLabel_ != nullptr) {
        delete lightSleepTimeLabel_;
        lightSleepTimeLabel_ = nullptr;
    }

    if (lightSleepTimeUnitLabel_ != nullptr) {
        delete lightSleepTimeUnitLabel_;
        lightSleepTimeUnitLabel_ = nullptr;
    }

    if (bluePointImg_ != nullptr) {
        delete bluePointImg_;
        bluePointImg_ = nullptr;
    }

    if (deepSleepTimeLabel_ != nullptr) {
        delete deepSleepTimeLabel_;
        deepSleepTimeLabel_ = nullptr;
    }

    if (deepSleepTimeUnitLabel_ != nullptr) {
        delete deepSleepTimeUnitLabel_;
        deepSleepTimeUnitLabel_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(SLEEP_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SleepDetailsPage::~SleepDetailsPage");
}

void SleepDetailsPage::OnStart(void *data)
{
    group_ = new UIScrollView();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "group_ is nullptr");
        return;
    }
    group_->SetPosition(0, 0);
    group_->Resize(RESOLUTION_WIDTH, RESOLUTION_HEIGHT);
    group_->SetViewId("sleepDetails");

    bgImg_ = new UIImageView();
    if (bgImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "bgImg_ is nullptr");
        return;
    }
    LOADIMG::LoadImageViewImg(bgImg_, SLEEP_IMAGE, IMAGE_SLEEP_BACKGROUND);
    bgImg_->SetPosition(0, 0, RESOLUTION_WIDTH, RESOLUTION_HEIGHT);
    group_->Add(bgImg_);

    sleepTitleLabel_ = new UILabel();
    if (sleepTitleLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "sleepTitleLabel_ is nullptr");
        return;
    }
    sleepTitleLabel_->SetPosition(SLEEPDETAIL_SLEEP_X, SLEEPDETAIL_SLEEP_Y);
    sleepTitleLabel_->Resize(SLEEPDETAIL_SLEEP_WIDTH, SLEEPDETAIL_SLEEP_HEIGHT);
    sleepTitleLabel_->SetText("睡眠");
    sleepTitleLabel_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    sleepTitleLabel_->SetTextColor(Color::White());
    sleepTitleLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, SLEEPDETAIL_SLEEP_FONT_SIZE);
    sleepTitleLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    group_->Add(sleepTitleLabel_);

    InitPage();

    if (GetRecord() == true) {
        InitDetailPage();
    } else {
        InitNoDeatilPage();
    };
    group_->SetOnDragListener(this);
    group_->SetDraggable(true);
    group_->SetTouchable(true);
    AddViewToPageContainer(group_);
}

void SleepDetailsPage::InitSoberingTime()
{
    redPointImg_ = new UIImageView();
    if (redPointImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "redPointImg_ is nullptr");
        return;
    }
    redPointImg_->SetPosition(SLEEPDETAIL_IMAGE1_X, SLEEPDETAIL_IMAGE1_Y);
    LOADIMG::LoadImageViewImg(redPointImg_, SLEEP_IMAGE, IMAGE_SLEEP_RED_CIR);
    group_->Add(redPointImg_);
    soberingTimeLabel_ = new UILabel();
    if (soberingTimeLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "soberingTimeLabel_ is nullptr");
        return;
    }
    soberingTimeLabel_->SetPosition(SLEEPDETAIL_SOBERINGTIME_X, SLEEPDETAIL_SOBERINGTIME_Y);
    soberingTimeLabel_->Resize(SLEEPDETAIL_SOBERINGTIME_WIDTH, SLEEPDETAIL_SOBERINGTIME_HEIGHT);
    soberingTimeLabel_->SetText("清醒");
    soberingTimeLabel_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    soberingTimeLabel_->SetTextColor(Color::White());
    soberingTimeLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SLEEPDETAIL_SOBERINGTIME_FONT_SIZE);
    soberingTimeLabel_->SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    group_->Add(soberingTimeLabel_);
}

void SleepDetailsPage::InitRapidEyeMovement()
{
    orangePointImg_ = new UIImageView();
    if (orangePointImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "orangePointImg_ is nullptr");
        return;
    }
    orangePointImg_->SetPosition(SLEEPDETAIL_IMAGE1_X, SLEEPDETAIL_IMAGE2_Y);
    LOADIMG::LoadImageViewImg(orangePointImg_, SLEEP_IMAGE, IMAGE_SLEEP_ORANGE_CIR);
    group_->Add(orangePointImg_);

    rapidEyeMovementLabel_ = new UILabel();
    if (rapidEyeMovementLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "rapidEyeMovementLabel_ is nullptr");
        return;
    }
    rapidEyeMovementLabel_->SetPosition(SLEEPDETAIL_SOBERINGTIME_X, SLEEPDETAIL_RAPID_Y);
    rapidEyeMovementLabel_->Resize(SLEEPDETAIL_RAPID_WIDTH, SLEEPDETAIL_RAPID_HEIGHT);
    rapidEyeMovementLabel_->SetText("快速动眼");
    rapidEyeMovementLabel_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    rapidEyeMovementLabel_->SetTextColor(Color::White());
    rapidEyeMovementLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SLEEPDETAIL_SOBERINGTIME_FONT_SIZE);
    rapidEyeMovementLabel_->SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    group_->Add(rapidEyeMovementLabel_);
}

void SleepDetailsPage::InitLightSleepTime()
{
    greenPointImg_ = new UIImageView();
    if (greenPointImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "greenPointImg_ is nullptr");
        return;
    }
    greenPointImg_->SetPosition(SLEEPDETAIL_IMAGE1_X, SLEEPDETAIL_IMAGE3_Y);
    LOADIMG::LoadImageViewImg(greenPointImg_, SLEEP_IMAGE, IMAGE_SLEEP_GREEN_CIR);
    group_->Add(greenPointImg_);

    lightSleepTimeLabel_ = new UILabel();
    if (lightSleepTimeLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "lightSleepTimeLabel_ is nullptr");
        return;
    }
    lightSleepTimeLabel_->SetPosition(SLEEPDETAIL_SOBERINGTIME_X, SLEEPDETAIL_LIGHT_Y);
    lightSleepTimeLabel_->Resize(SLEEPDETAIL_SOBERINGTIME_WIDTH, SLEEPDETAIL_SOBERINGTIME_HEIGHT);
    lightSleepTimeLabel_->SetText("浅睡");
    lightSleepTimeLabel_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    lightSleepTimeLabel_->SetTextColor(Color::White());
    lightSleepTimeLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SLEEPDETAIL_SOBERINGTIME_FONT_SIZE);
    lightSleepTimeLabel_->SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    group_->Add(lightSleepTimeLabel_);
}

void SleepDetailsPage::InitDeepSleepTime()
{
    bluePointImg_ = new UIImageView();
    if (bluePointImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "bluePointImg_ is nullptr");
        return;
    }
    bluePointImg_->SetPosition(SLEEPDETAIL_IMAGE1_X, SLEEPDETAIL_IMAGE4_Y);
    LOADIMG::LoadImageViewImg(bluePointImg_, SLEEP_IMAGE, IMAGE_SLEEP_BLUE_CIR);
    group_->Add(bluePointImg_);

    deepSleepTimeLabel_ = new UILabel();
    if (deepSleepTimeLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "deepSleepTimeLabel_ is nullptr");
        return;
    }
    deepSleepTimeLabel_->SetPosition(SLEEPDETAIL_SOBERINGTIME_X, SLEEPDETAIL_DEEP_Y);
    deepSleepTimeLabel_->Resize(SLEEPDETAIL_SOBERINGTIME_WIDTH, SLEEPDETAIL_SOBERINGTIME_HEIGHT);
    deepSleepTimeLabel_->SetText("深睡");
    deepSleepTimeLabel_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    deepSleepTimeLabel_->SetTextColor(Color::White());
    deepSleepTimeLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SLEEPDETAIL_SOBERINGTIME_FONT_SIZE);
    deepSleepTimeLabel_->SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    group_->Add(deepSleepTimeLabel_);
}

void SleepDetailsPage::InitPage()
{
    grayBgImg_ = new UIImageView();
    if (grayBgImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "grayBgImg_ is nullptr");
        return;
    }
    grayBgImg_->SetPosition(SLEEPDETAIL_GRAYBG_X, SLEEPDETAIL_GRAYBG_Y);
    LOADIMG::LoadImageViewImg(grayBgImg_, SLEEP_IMAGE, IMAGE_SLEEP_RECORD_BG);
    group_->Add(grayBgImg_);

    // 加载详情四项
    InitSoberingTime();
    InitRapidEyeMovement();
    InitLightSleepTime();
    InitDeepSleepTime();
}

void SleepDetailsPage::LoadDeatilActualSleepTime()
{
    actualSleepTimeHoursLabel_ = new UILabel();
    if (actualSleepTimeHoursLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "actualSleepTimeHoursLabel_ is nullptr");
        return;
    }
    actualSleepTimeHoursLabel_->SetPosition(SLEEPDETAIL_ACTUALHOUR_X, SLEEPDETAIL_ACTUALHOUR_Y);
    actualSleepTimeHoursLabel_->Resize(SLEEPDETAIL_ACTUALSLEEPTIMINUTES_HEIGHT, SLEEPDETAIL_ACTUALSLEEPTIMINUTES_HEIGHT);
    actualSleepTimeHoursLabel_->SetTextColor(Color::White());
    actualSleepTimeHoursLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, SLEEPDETAIL_ACTUALSLEEPTIMEHOURS_FONT_SIZE);
    group_->Add(actualSleepTimeHoursLabel_);

    actualSleepTimeHoursUnitLabel_ = new UILabel();
    if (actualSleepTimeHoursUnitLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "actualSleepTimeHoursUnitLabel_ is nullptr");
        return;
    }
    actualSleepTimeHoursUnitLabel_->SetText("小时");
    actualSleepTimeHoursUnitLabel_->SetPosition(SLEEPDETAIL_ACTUALHOURTEXT_X, SLEEPDETAIL_ACTUALHOUR_Y);
    actualSleepTimeHoursUnitLabel_->Resize(SLEEPDETAIL_SOBERINGTIME_WIDTH, SLEEPDETAIL_SOBERINGTIME_HEIGHT);
    actualSleepTimeHoursUnitLabel_->SetTextColor(Color::White());
    actualSleepTimeHoursUnitLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SLEEPDETAIL_SOBERINGTIME_FONT_SIZE);
    group_->Add(actualSleepTimeHoursUnitLabel_);

    actualSleepTimeMinutesLabel_ = new UILabel();
    if (actualSleepTimeMinutesLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "actualSleepTimeMinutesLabel_ is nullptr");
        return;
    }

    actualSleepTimeMinutesLabel_->SetPosition(SLEEPDETAIL_ACTUALMINUTE_X, SLEEPDETAIL_ACTUALHOUR_Y);
    actualSleepTimeMinutesLabel_->Resize(SLEEPDETAIL_RAPID_WIDTH, SLEEPDETAIL_ACTUALSLEEPTIMINUTES_HEIGHT);
    actualSleepTimeMinutesLabel_->SetTextColor(Color::White());
    actualSleepTimeMinutesLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, SLEEPDETAIL_ACTUALSLEEPTIMINUTES_FONT_SIZE);
    group_->Add(actualSleepTimeMinutesLabel_);

    actualSleepTimeMinutesUnitLabel_ = new UILabel();
    if (actualSleepTimeMinutesUnitLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "actualSleepTimeHoursLabel_ is nullptr");
        return;
    }

    actualSleepTimeMinutesUnitLabel_->SetText("分钟");
    actualSleepTimeMinutesUnitLabel_->SetPosition(SLEEPDETAIL_ACTUALMINUTETEXT_X, SLEEPDETAIL_ACTUALMINUTE_Y);
    actualSleepTimeMinutesUnitLabel_->Resize(SLEEPDETAIL_SOBERINGTIME_WIDTH, SLEEPDETAIL_SOBERINGTIME_HEIGHT);
    actualSleepTimeMinutesUnitLabel_->SetTextColor(Color::White());
    actualSleepTimeMinutesUnitLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SLEEPDETAIL_SOBERINGTIME_FONT_SIZE);
    group_->Add(actualSleepTimeMinutesUnitLabel_);
}

void SleepDetailsPage::InitActualSleepTime(std::tuple<uint32_t, uint32_t> timeTuple)
{
    LoadDeatilActualSleepTime();
    uint32_t hours = std::get<0>(timeTuple);
    uint32_t minutes = std::get<1>(timeTuple);
    actualSleepTimeHoursLabel_->SetText(std::to_string(hours).c_str());
    actualSleepTimeMinutesLabel_->SetText(std::to_string(minutes).c_str());
}

void SleepDetailsPage::InitDetailPage()
{
    InitActualSleepTime(SleepModel::GetInstance().GetSleepTime());

    soberingTimeUnitLabel_ = new UILabel();
    if (soberingTimeUnitLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "soberingTimeUnitLabel_ is nullptr");
        return;
    }
    soberingTimeUnitLabel_->SetText((SetTimeUnit(SleepModel::GetInstance().GetSoberingTime())).c_str());
    soberingTimeUnitLabel_->SetPosition(SLEEPDETAIL_SOBERINGTEXT_X, SLEEPDETAIL_SOBERINGTIME_Y);
    soberingTimeUnitLabel_->Resize(SLEEPDETAIL_LABEL_WIDTH, SLEEPDETAIL_RAPID_HEIGHT);
    soberingTimeUnitLabel_->SetTextColor(Color::White());
    soberingTimeUnitLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SLEEPDETAIL_SOBERINGTIME_FONT_SIZE);
    soberingTimeUnitLabel_->SetAlign(TEXT_ALIGNMENT_RIGHT, TEXT_ALIGNMENT_CENTER);
    soberingTimeUnitLabel_->SetStyle(STYLE_PADDING_RIGHT, SLEEPDETAIL_PADDING_RIGHT);
    group_->Add(soberingTimeUnitLabel_);

    rapidEyeMovementUnitLabel_ = new UILabel();
    if (rapidEyeMovementUnitLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "rapidEyeMovementUnitLabel_ is nullptr");
        return;
    }
    rapidEyeMovementUnitLabel_->SetText((SetTimeUnit(SleepModel::GetInstance().GetRapidEyeMovementTime())).c_str());
    rapidEyeMovementUnitLabel_->SetPosition(SLEEPDETAIL_SOBERINGTEXT_X, SLEEPDETAIL_RAPID_Y);
    rapidEyeMovementUnitLabel_->Resize(SLEEPDETAIL_LABEL_WIDTH, SLEEPDETAIL_RAPID_HEIGHT);
    rapidEyeMovementUnitLabel_->SetTextColor(Color::White());
    rapidEyeMovementUnitLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SLEEPDETAIL_SOBERINGTIME_FONT_SIZE);
    rapidEyeMovementUnitLabel_->SetAlign(TEXT_ALIGNMENT_RIGHT, TEXT_ALIGNMENT_CENTER);
    rapidEyeMovementUnitLabel_->SetStyle(STYLE_PADDING_RIGHT, SLEEPDETAIL_PADDING_RIGHT);
    group_->Add(rapidEyeMovementUnitLabel_);

    lightSleepTimeUnitLabel_ = new UILabel();
    if (lightSleepTimeUnitLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "lightSleepTimeUnitLabel_ is nullptr");
        return;
    }
    lightSleepTimeUnitLabel_->SetText((SetTimeUnit(SleepModel::GetInstance().GetLightSleepTime())).c_str());
    lightSleepTimeUnitLabel_->SetPosition(SLEEPDETAIL_SOBERINGTEXT_X, SLEEPDETAIL_LIGHT_Y);
    lightSleepTimeUnitLabel_->Resize(SLEEPDETAIL_LABEL_WIDTH, SLEEPDETAIL_RAPID_HEIGHT);
    lightSleepTimeUnitLabel_->SetTextColor(Color::White());
    lightSleepTimeUnitLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SLEEPDETAIL_SOBERINGTIME_FONT_SIZE);
    lightSleepTimeUnitLabel_->SetAlign(TEXT_ALIGNMENT_RIGHT, TEXT_ALIGNMENT_CENTER);
    lightSleepTimeUnitLabel_->SetStyle(STYLE_PADDING_RIGHT, SLEEPDETAIL_PADDING_RIGHT);
    group_->Add(lightSleepTimeUnitLabel_);

    deepSleepTimeUnitLabel_ = new UILabel();
    if (deepSleepTimeUnitLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "deepSleepTimeUnitLabel_ is nullptr");
        return;
    }
    deepSleepTimeUnitLabel_->SetText((SetTimeUnit(SleepModel::GetInstance().GetDeepSleepTime())).c_str());
    deepSleepTimeUnitLabel_->SetPosition(SLEEPDETAIL_SOBERINGTEXT_X, SLEEPDETAIL_DEEP_Y);
    deepSleepTimeUnitLabel_->Resize(SLEEPDETAIL_LABEL_WIDTH, SLEEPDETAIL_RAPID_HEIGHT);
    deepSleepTimeUnitLabel_->SetTextColor(Color::White());
    deepSleepTimeUnitLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SLEEPDETAIL_SOBERINGTIME_FONT_SIZE);
    deepSleepTimeUnitLabel_->SetAlign(TEXT_ALIGNMENT_RIGHT, TEXT_ALIGNMENT_CENTER);
    deepSleepTimeUnitLabel_->SetStyle(STYLE_PADDING_RIGHT, SLEEPDETAIL_PADDING_RIGHT);
    group_->Add(deepSleepTimeUnitLabel_);
}

void SleepDetailsPage::InitNoDeatilPage()
{
    actualSleepNorecordTimeLabel_ = new UILabel();
    if (actualSleepNorecordTimeLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "actualSleepNorecordTimeLabel_ is nullptr");
        return;
    }
    actualSleepNorecordTimeLabel_->SetText("无记录");
    actualSleepNorecordTimeLabel_->SetPosition(SLEEPDETAIL_ACTUALSLEEP_NORECORD_X, SLEEPDETAIL_ACTUALSLEEP_NORECORD_Y);
    actualSleepNorecordTimeLabel_->Resize(SLEEPDETAIL_ACTUAL_NORECODE_X, SLEEPDETAIL_SOBERINGTIME_WIDTH);
    actualSleepNorecordTimeLabel_->SetTextColor(Color::White());
    actualSleepNorecordTimeLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SLEEPDETAIL_ACTUALSLEEP_FONT_SIZE);
    group_->Add(actualSleepNorecordTimeLabel_);

    soberingTimeUnitLabel_ = new UILabel();
    if (soberingTimeUnitLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "soberingTimeUnitLabel_ is nullptr");
        return;
    }
    soberingTimeUnitLabel_->SetText("--");
    soberingTimeUnitLabel_->SetPosition(SLEEPDETAIL_SOBERINGTEXT_X, SLEEPDETAIL_SOBERINGTIME_Y);
    soberingTimeUnitLabel_->Resize(SLEEPDETAIL_LABEL_WIDTH, SLEEPDETAIL_RAPID_HEIGHT);
    soberingTimeUnitLabel_->SetTextColor(Color::White());
    soberingTimeUnitLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SLEEPDETAIL_SOBERINGTIME_FONT_SIZE);
    soberingTimeUnitLabel_->SetAlign(TEXT_ALIGNMENT_RIGHT, TEXT_ALIGNMENT_CENTER);
    group_->Add(soberingTimeUnitLabel_);

    rapidEyeMovementUnitLabel_ = new UILabel();
    if (rapidEyeMovementUnitLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "rapidEyeMovementUnitLabel_ is nullptr");
        return;
    }
    rapidEyeMovementUnitLabel_->SetText("--");
    rapidEyeMovementUnitLabel_->SetPosition(SLEEPDETAIL_SOBERINGTEXT_X, SLEEPDETAIL_RAPID_Y);
    rapidEyeMovementUnitLabel_->Resize(SLEEPDETAIL_LABEL_WIDTH, SLEEPDETAIL_RAPID_HEIGHT);
    rapidEyeMovementUnitLabel_->SetTextColor(Color::White());
    rapidEyeMovementUnitLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SLEEPDETAIL_SOBERINGTIME_FONT_SIZE);
    rapidEyeMovementUnitLabel_->SetAlign(TEXT_ALIGNMENT_RIGHT, TEXT_ALIGNMENT_CENTER);
    group_->Add(rapidEyeMovementUnitLabel_);

    lightSleepTimeUnitLabel_ = new UILabel();
    if (lightSleepTimeUnitLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "lightSleepTimeUnitLabel_ is nullptr");
        return;
    }
    lightSleepTimeUnitLabel_->SetText("--");
    lightSleepTimeUnitLabel_->SetPosition(SLEEPDETAIL_SOBERINGTEXT_X, SLEEPDETAIL_LIGHT_Y);
    lightSleepTimeUnitLabel_->Resize(SLEEPDETAIL_LABEL_WIDTH, SLEEPDETAIL_RAPID_HEIGHT);
    lightSleepTimeUnitLabel_->SetTextColor(Color::White());
    lightSleepTimeUnitLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SLEEPDETAIL_SOBERINGTIME_FONT_SIZE);
    lightSleepTimeUnitLabel_->SetAlign(TEXT_ALIGNMENT_RIGHT, TEXT_ALIGNMENT_CENTER);
    group_->Add(lightSleepTimeUnitLabel_);

    deepSleepTimeUnitLabel_ = new UILabel();
    if (deepSleepTimeUnitLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "deepSleepTimeUnitLabel_ is nullptr");
        return;
    }
    deepSleepTimeUnitLabel_->SetText("--");
    deepSleepTimeUnitLabel_->SetPosition(SLEEPDETAIL_SOBERINGTEXT_X, SLEEPDETAIL_DEEP_Y);
    deepSleepTimeUnitLabel_->Resize(SLEEPDETAIL_LABEL_WIDTH, SLEEPDETAIL_RAPID_HEIGHT);
    deepSleepTimeUnitLabel_->SetTextColor(Color::White());
    deepSleepTimeUnitLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SLEEPDETAIL_SOBERINGTIME_FONT_SIZE);
    deepSleepTimeUnitLabel_->SetAlign(TEXT_ALIGNMENT_RIGHT, TEXT_ALIGNMENT_CENTER);
    group_->Add(deepSleepTimeUnitLabel_);
}

std::string SleepDetailsPage::SetTimeUnit(std::tuple<uint32_t, uint32_t> timeTuple)
{
    uint32_t hours = std::get<0>(timeTuple);
    uint32_t minutes = std::get<1>(timeTuple);

    std::string str = "";
    if ((hours == 0) && (minutes != 0)) {
        str = std::to_string(minutes) + "分钟";
    } else if ((hours != 0) && (minutes == 0)) {
        str = std::to_string(hours) + "小时";
    } else if ((hours != 0) && (minutes != 0)) {
        str = std::to_string(hours) + "小时" + std::to_string(minutes) + "分钟";
    } else if ((hours == 0) && (minutes == 0)) {
        str = "---";
    }
    return str;
}

bool SleepDetailsPage::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if (event.GetDragDirection() == DragEvent::DIRECTION_TOP_TO_BOTTOM) {
        NativeAbility::GetInstance().SwitchPageInSlice(SLEEPING_PAGES::SLEEPING_MAIN_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    } else if (event.GetDragDirection() == DragEvent::DIRECTION_BOTTOM_TO_TOP) {
        if (GetRecord() == false) {
            NativeAbility::GetInstance().SwitchPageInSlice(SLEEPING_PAGES::SLEEPING_TARGET_PAGE,
                TransitionType::TRANSITION_INVALID, false);
        }
    } else if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) &&
               (event.GetDeltaX() > X_DRAG_OFFSET) && (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().ChangeSliceToApplist();
    }
    return true;
}

bool SleepDetailsPage::GetRecord()
{
    if (SleepModel::GetInstance().GetRecord()) {
        return true;
    }
    return false;
}
}
