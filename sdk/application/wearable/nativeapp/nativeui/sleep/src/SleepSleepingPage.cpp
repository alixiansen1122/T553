/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SleepSleepingPage
 * Created: 2025-06-05
 */

#include <string>
#include "components/ui_list_nested.h"
#include "components/ui_scroll_view_nested.h"
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "common/image_cache_manager.h"
#include "diag_service.h"
#include "msg_center_device.h"
#include "diag_common.h"
#include "sleep/SleepTargetTime.h"
#include "sleep/SleepPresenter.h"
#include "sleep/SleepView.h"
#include "sleep/SleepSleepingPage.h"

namespace OHOS {

REGIST_SLICE_PAGE(VIEW_SLEEPING, SLEEPING_PAGES::SLEEPING_SLEEP_PAGE, SleepSleepingPage, false);

static constexpr int16_t SLEEPING_SLEEP_X = 97;
static constexpr int16_t SLEEPING_SLEEP_Y = 31;
static constexpr int16_t SLEEPING_SLEEP_WIDTH = 260;
static constexpr int16_t SLEEPING_SLEEP_HEIGHT = 53;
static constexpr int16_t SLEEPING_SLEEP_FONT_SIZE = 38;
static constexpr int16_t SLEEPING_NORECORD_X = 135;
static constexpr int16_t SLEEPING_NORECORD_Y = 220;
static constexpr int16_t SLEEPING_NORECORD_WIDTH = 200;
static constexpr int16_t SLEEPING_NORECORD_HEIGHT = 48;
static constexpr int16_t SLEEPING_NORECORD_FONT_SIZE = 32;
static constexpr int16_t SLEEPING_TIPS_FONT_SIZE = 24;
static constexpr int16_t SLEEPING_FONT_SIZE_LARGE = 40;
static constexpr int16_t SLEEPING_LABEL_WIDTH = 48;
static constexpr int16_t SLEEPING_LABEL_HEIGHT = 29;
static constexpr int16_t SLEEPING_TIME_LABEL_WIDTH = 95;
static constexpr int16_t SLEEPING_TIME_LABEL_HEIGHT = 50;
static constexpr int16_t SLEEPING_ACTUALMINUTE_X = 230;
static constexpr int16_t SLEEPING_ACTUALMINUTE_Y = 314;
static constexpr int16_t ACTUALMINUTES_X = 180;
static constexpr int16_t ACTUALMINUTES_Y = 298;
static constexpr int16_t SLEEPING_HEIGHT = 50;
static constexpr int16_t SLEEPING_CIR_HIGHT = 444;
static constexpr int16_t SLEEPING_CIR_X = 228;
static constexpr int16_t SLEEPING_CIR_Y = 228;
static constexpr int16_t SLEEPING_CIR_RADIUS = 215;
static constexpr int16_t SLEEPING_CIR_ANGLE = 360;
static constexpr int16_t SLEEPING_CIR_LINE_WIDTH = 10;
static constexpr int16_t SLEEPING_TIMETEXT_HIGHT = 95;
static constexpr int16_t SLEEPING_MOON_POSX = 178;
static constexpr int16_t SLEEPING_TIME_ELAPSED = 5;
static constexpr int16_t SLEEPING_BG_X = 120;
static constexpr int16_t SLEEPING_BG_Y = 295;
static constexpr int16_t SLEEPING_TIME_MINUTE_X = 157;
static constexpr int16_t SLEEPING_TIME_HOUR_X = 128;
static constexpr int16_t SLEEPING_TIME_HOUR_TEXT_X = 178;
static constexpr int16_t SLEEPING_TIME_MINUTE1_X = 238;
static constexpr int16_t SLEEPING_TIME_MINUTE1_TEXT_X = 288;

SleepSleepingPage::SleepSleepingPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SleepSleepingPage::SleepSleepingPage");
}

SleepSleepingPage::~SleepSleepingPage()
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

    if (moonImg_ != nullptr) {
        delete moonImg_;
        moonImg_ = nullptr;
    }

    if (sleepingLabel_ != nullptr) {
        delete sleepingLabel_;
        sleepingLabel_ = nullptr;
    }

    if (sleepTimeBgImg_ != nullptr) {
        delete sleepTimeBgImg_;
        sleepTimeBgImg_ = nullptr;
    }

    if (targetSleepTimeHoursLabel_ != nullptr) {
        delete targetSleepTimeHoursLabel_;
        targetSleepTimeHoursLabel_ = nullptr;
    }

    if (targetSleepTimeHoursUnitLabel_ != nullptr) {
        delete targetSleepTimeHoursUnitLabel_;
        targetSleepTimeHoursUnitLabel_ = nullptr;
    }

    if (targetSleepTimeMinutesLabel_ != nullptr) {
        delete targetSleepTimeMinutesLabel_;
        targetSleepTimeMinutesLabel_ = nullptr;
    }

    if (targetSleepTimeMinutesUnitLabel_ != nullptr) {
        delete targetSleepTimeMinutesUnitLabel_;
        targetSleepTimeMinutesUnitLabel_ = nullptr;
    }

    if (circleProgress_ != nullptr) {
        delete circleProgress_;
        circleProgress_ = nullptr;
    }

    if (callBack_ != nullptr) {
        delete callBack_;
        callBack_ = nullptr;
    }

    if (animator_ != nullptr) {
        delete animator_;
        animator_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(SLEEP_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SleepSleepingPage::~SleepSleepingPage");
}

void SleepSleepingPage::OnStart(void *data)
{
    group_ = new UIScrollView();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "group_ is nullptr");
        return;
    }
    group_->SetPosition(0, 0);
    group_->Resize(RESOLUTION_WIDTH, RESOLUTION_HEIGHT);

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
    sleepTitleLabel_->SetPosition(SLEEPING_SLEEP_X, SLEEPING_SLEEP_Y);
    sleepTitleLabel_->Resize(SLEEPING_SLEEP_WIDTH, SLEEPING_SLEEP_HEIGHT);
    sleepTitleLabel_->SetText("睡眠");
    sleepTitleLabel_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    sleepTitleLabel_->SetTextColor(Color::White());
    sleepTitleLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, SLEEPING_SLEEP_FONT_SIZE);
    sleepTitleLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    group_->Add(sleepTitleLabel_);

    moonImg_ = new UIImageView();
    if (moonImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "moonImg_ is nullptr");
        return;
    }
    moonImg_->SetPosition(SLEEPING_MOON_POSX, SLEEPING_TIMETEXT_HIGHT);
    LOADIMG::LoadImageViewImg(moonImg_, SLEEP_IMAGE, IMAGE_SLEEP_SLEEPING_MOON);
    group_->Add(moonImg_);

    sleepingLabel_ = new UILabel();
    if (sleepingLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "sleepingLabel_ is nullptr");
        return;
    }
    sleepingLabel_->SetPosition(SLEEPING_NORECORD_X, SLEEPING_NORECORD_Y);
    sleepingLabel_->Resize(SLEEPING_NORECORD_WIDTH, SLEEPING_NORECORD_HEIGHT);
    sleepingLabel_->SetText("正在睡眠");
    sleepingLabel_->SetTextColor(Color::White());
    sleepingLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SLEEPING_NORECORD_FONT_SIZE);
    sleepingLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    group_->Add(sleepingLabel_);

    InitCircleProgress();
    InitSleepTimeLabel();
    LoadTargetTimeForHourAndMin(SleepModel::GetInstance().GetTargetTime());
    SleepModel::GetInstance().LoadData();

    callBack_ = new SleepAnimatorCallback(this);
    if (callBack_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "callBack_ is nullptr");
        return;
    }
    animator_ = new Animator(callBack_, nullptr, 0, true);
    if (animator_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "animator_ is nullptr");
        return;
    }

    group_->SetOnDragListener(this);
    group_->SetDraggable(true);
    group_->SetTouchable(true);

    animator_->Start();
    gettimeofday(&startTime_, nullptr);
    AddViewToPageContainer(group_);
}

void SleepSleepingPage::OnPause()
{
    animator_->Stop();
}

void SleepSleepingPage::SleepAnimatorCallback::Callback(UIView *view)
{
    (void)view;
    struct timeval current_time;
    gettimeofday(&current_time, nullptr);
    uint32_t elapsed_seconds = current_time.tv_sec - page_->startTime_.tv_sec;
    if (elapsed_seconds == SLEEPING_TIME_ELAPSED) {
        page_->StopAnimator();
    }
}

void SleepSleepingPage::StopAnimator()
{
    animator_->Stop();
    NativeAbility::GetInstance().SwitchPageInSlice(SLEEPING_PAGES::SLEEPING_MAIN_PAGE,
        TransitionType::TRANSITION_INVALID, false);
}

void SleepSleepingPage::InitSleepTimeLabel()
{
    sleepTimeBgImg_ = new UIImageView();
    if (sleepTimeBgImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "sleepTimeBgImg_ is nullptr");
        return;
    }
    sleepTimeBgImg_->SetPosition(SLEEPING_BG_X, SLEEPING_BG_Y);
    LOADIMG::LoadImageViewImg(sleepTimeBgImg_, SLEEP_IMAGE, IMAGE_SLEEP_SLEEPING_TIME);
    group_->Add(sleepTimeBgImg_);

    targetSleepTimeHoursLabel_ = new UILabel();
    if (targetSleepTimeHoursLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "targetSleepTimeHoursLabel_ is nullptr");
        return;
    }
    targetSleepTimeHoursLabel_->SetTextColor(Color::White());
    targetSleepTimeHoursLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, SLEEPING_FONT_SIZE_LARGE);
    targetSleepTimeHoursLabel_->SetVisible(false);

    targetSleepTimeHoursUnitLabel_ = new UILabel();
    if (targetSleepTimeHoursUnitLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "targetSleepTimeHoursUnitLabel_ is nullptr");
        return;
    }
    targetSleepTimeHoursUnitLabel_->SetText("小时");
    targetSleepTimeHoursUnitLabel_->Resize(SLEEPING_LABEL_WIDTH, SLEEPING_LABEL_HEIGHT);
    targetSleepTimeHoursUnitLabel_->SetTextColor(Color::White());
    targetSleepTimeHoursUnitLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SLEEPING_TIPS_FONT_SIZE);
    targetSleepTimeHoursUnitLabel_->SetVisible(false);

    targetSleepTimeMinutesLabel_ = new UILabel();
    if (targetSleepTimeMinutesLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "targetSleepTimeMinutesLabel_ is nullptr");
        return;
    }
    targetSleepTimeMinutesLabel_->Resize(SLEEPING_TIME_LABEL_WIDTH, SLEEPING_TIME_LABEL_HEIGHT);
    targetSleepTimeMinutesLabel_->SetTextColor(Color::White());
    targetSleepTimeMinutesLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, SLEEPING_FONT_SIZE_LARGE);
    targetSleepTimeMinutesLabel_->SetVisible(false);

    targetSleepTimeMinutesUnitLabel_ = new UILabel();
    if (targetSleepTimeMinutesUnitLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "targetSleepTimeMinutesUnitLabel_ is nullptr");
        return;
    }
    targetSleepTimeMinutesUnitLabel_->SetText("分钟");
    targetSleepTimeMinutesUnitLabel_->Resize(SLEEPING_LABEL_WIDTH, SLEEPING_LABEL_HEIGHT);
    targetSleepTimeMinutesUnitLabel_->SetTextColor(Color::White());
    targetSleepTimeMinutesUnitLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SLEEPING_TIPS_FONT_SIZE);
    targetSleepTimeMinutesUnitLabel_->SetVisible(false);
}

void SleepSleepingPage::LoadTargetTimeForHourAndMin(std::tuple<uint32_t, uint32_t> timeTuple)
{
    uint32_t hours = std::get<0>(timeTuple);
    uint32_t minutes = std::get<1>(timeTuple);

    if ((hours == 0) && (minutes != 0)) {
        targetSleepTimeMinutesLabel_->SetPosition(SLEEPING_TIME_MINUTE_X, ACTUALMINUTES_Y);
        targetSleepTimeMinutesLabel_->SetVisible(true);
        targetSleepTimeMinutesLabel_->SetText(std::to_string(minutes).c_str());
        targetSleepTimeMinutesLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        group_->Add(targetSleepTimeMinutesLabel_);

        targetSleepTimeMinutesUnitLabel_->SetPosition(SLEEPING_ACTUALMINUTE_X, SLEEPING_ACTUALMINUTE_Y);
        targetSleepTimeMinutesUnitLabel_->SetVisible(true);
        group_->Add(targetSleepTimeMinutesUnitLabel_);
    } else if ((hours != 0) && (minutes == 0)) {
        targetSleepTimeHoursLabel_->SetPosition(ACTUALMINUTES_X, ACTUALMINUTES_Y);
        targetSleepTimeHoursLabel_->Resize(SLEEPING_HEIGHT, SLEEPING_HEIGHT);
        targetSleepTimeHoursLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        targetSleepTimeHoursLabel_->SetVisible(true);
        targetSleepTimeHoursLabel_->SetText(std::to_string(hours).c_str());
        group_->Add(targetSleepTimeHoursLabel_);

        targetSleepTimeHoursUnitLabel_->SetPosition(SLEEPING_ACTUALMINUTE_X, SLEEPING_ACTUALMINUTE_Y);
        targetSleepTimeHoursUnitLabel_->SetVisible(true);
        group_->Add(targetSleepTimeHoursUnitLabel_);
    } else if ((hours != 0) && (minutes != 0)) {
        targetSleepTimeHoursLabel_->SetPosition(SLEEPING_TIME_HOUR_X, ACTUALMINUTES_Y);
        targetSleepTimeHoursLabel_->Resize(SLEEPING_HEIGHT, SLEEPING_HEIGHT);
        targetSleepTimeHoursLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        targetSleepTimeHoursLabel_->SetVisible(true);
        targetSleepTimeHoursLabel_->SetText(std::to_string(hours).c_str());
        group_->Add(targetSleepTimeHoursLabel_);

        targetSleepTimeHoursUnitLabel_->SetPosition(SLEEPING_TIME_HOUR_TEXT_X, SLEEPING_ACTUALMINUTE_Y);
        targetSleepTimeHoursUnitLabel_->Resize(SLEEPING_HEIGHT, SLEEPING_LABEL_HEIGHT);
        targetSleepTimeHoursUnitLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        targetSleepTimeHoursUnitLabel_->SetVisible(true);
        group_->Add(targetSleepTimeHoursUnitLabel_);

        targetSleepTimeMinutesLabel_->SetPosition(SLEEPING_TIME_MINUTE1_X, ACTUALMINUTES_Y);
        targetSleepTimeMinutesLabel_->Resize(SLEEPING_HEIGHT, SLEEPING_HEIGHT);
        targetSleepTimeMinutesLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        targetSleepTimeMinutesLabel_->SetVisible(true);
        targetSleepTimeMinutesLabel_->SetText(std::to_string(minutes).c_str());
        group_->Add(targetSleepTimeMinutesLabel_);

        targetSleepTimeMinutesUnitLabel_->SetPosition(SLEEPING_TIME_MINUTE1_TEXT_X, SLEEPING_ACTUALMINUTE_Y);
        targetSleepTimeMinutesUnitLabel_->Resize(SLEEPING_HEIGHT, SLEEPING_LABEL_HEIGHT);
        targetSleepTimeMinutesUnitLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        targetSleepTimeMinutesUnitLabel_->SetVisible(true);
        group_->Add(targetSleepTimeMinutesUnitLabel_);
    }
}

void SleepSleepingPage::InitCircleProgress()
{
    circleProgress_ = new UICircleProgress();
    if (circleProgress_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "circleProgress_ is nullptr");
        return;
    }
    circleProgress_->SetPosition(0, 0);
    circleProgress_->Resize(SLEEPING_CIR_HIGHT, SLEEPING_CIR_HIGHT);
    circleProgress_->SetCenterPosition(SLEEPING_CIR_X, SLEEPING_CIR_Y);
    circleProgress_->SetRadius(SLEEPING_CIR_RADIUS);
    circleProgress_->SetBackgroundStyle(STYLE_LINE_COLOR, 0xFF6f29a0);
    circleProgress_->SetForegroundStyle(STYLE_LINE_COLOR, 0xFFB566FF);
    circleProgress_->SetBackgroundStyle(STYLE_LINE_WIDTH, SLEEPING_CIR_LINE_WIDTH);
    circleProgress_->SetForegroundStyle(STYLE_LINE_WIDTH, SLEEPING_CIR_LINE_WIDTH);
    circleProgress_->SetStartAngle(0);
    circleProgress_->SetEndAngle(SLEEPING_CIR_ANGLE);
    circleProgress_->SetRange(SLEEPING_TIME_ELAPSED, 0);
    circleProgress_->SetValue(SLEEPING_TIME_ELAPSED);
    group_->Add(circleProgress_);
    circleProgress_->Invalidate();
}
}
