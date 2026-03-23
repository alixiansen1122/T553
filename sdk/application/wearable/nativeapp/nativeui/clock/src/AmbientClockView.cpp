/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: AmbientClockView
 * Author:
 * Create: 2024-10-30
 */

#include "clock/AmbientClockView.h"
#include "main/MainPresenterSample.h"
#include "components/ui_label.h"
#include "wearable_log.h"
#include "ui_resource_image.h"
#include "common/image_cache_manager.h"
#include "UIWatchDialFactory.h"
#ifdef BUILD_IN_LINUX
#include "localtime.h"
#endif

namespace OHOS {

REGIST_WATCH_DIAL(DIAL_DISPLAY_SUPPORT_AOD, DIAL_CLOCK, AmbientClockView, nullptr);

static constexpr uint16_t POINT_POSITION_X = 202;
static constexpr uint16_t POINT_POSITION_Y = 16;
static constexpr uint16_t SECOND_POINT_POSITION_Y = 16;
static constexpr uint16_t POINT_CENTER_X = 25;
static constexpr uint16_t POINT_CENTER_Y = 211;
static constexpr uint16_t BIGCIRCLE_POSITION_X = 202;
static constexpr uint16_t BIGCIRCLE_POSITION_Y = 202;
static constexpr uint16_t MEDCIRCLE_POSITION_X = 202;
static constexpr uint16_t MEDCIRCLE_POSITION_Y = 202;
static constexpr uint16_t SMALLCIRCLE_POSITION_X = 202;
static constexpr uint16_t SMALLCIRCLE_POSITION_Y = 202;
static constexpr int16_t SEC_TO_MILLI = 1000;
static constexpr int16_t USEC_TO_MILLI = 1000;
static constexpr int16_t MIN_TO_HOUR = 60;
static constexpr int16_t HOURLY_SYSTEM = 24;

AmbientClockView::AmbientClockView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AmbientClockView::AmbientClockView");
}

AmbientClockView::~AmbientClockView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AmbientClockView::~AmbientClockView");
}

bool AmbientClockView::InitCenterCircleView(void)
{
    bigCircle_ = new Image();
    if (bigCircle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "bigCircle new fail");
        return false;
    }
    if (!LoadClockImage(bigCircle_, CLOCK_BIGCIRCLE_IMAGE_PATH)) {
        return false;
    }

    medCircle_ = new Image();
    if (medCircle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "medCircle new fail");
        return false;
    }
    if (!LoadClockImage(medCircle_, CLOCK_MEDCIRCLE_IMAGE_PATH)) {
        return false;
    }

    smallCircle_ = new Image();
    if (smallCircle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "smallCircle new fail");
        return false;
    }
    if (!LoadClockImage(smallCircle_, CLOCK_CIRCLE_IMAGE_PATH)) {
        return false;
    }
    return true;
}

bool AmbientClockView::InitHandView(void)
{
    secondHand_ = new Image();
    if (secondHand_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "secondHand new fail");
        return false;
    }
    if (!LoadClockImage(secondHand_, CLOCK_SECONDHAND)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "secondHand load fail");
        return false;
    }

    minuteHand_ = new Image();
    if (minuteHand_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "minuteHand new fail");
        return false;
    }
    if (!LoadClockImage(minuteHand_, CLOCK_MINUTEHAND)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "minuteHand load fail");
        return false;
    }
    hourHand_ = new Image();
    if (hourHand_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "hourHand new fail");
        return false;
    }
    if (!LoadClockImage(hourHand_, CLOCK_HOURHAND)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "hourHand load fail");
        return false;
    }

    hand24Hour_ = new Image();
    if (hand24Hour_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "hand24Hour new fail");
        return false;
    }
    if (!LoadClockImage(hand24Hour_, CLOCK_HAND24HOUR_IMAGE_PATH)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "hand24Hour load fail");
        return false;
    }
    return true;
}

bool AmbientClockView::LoadClockImage(Image *view, uint32_t resId)
{
    std::string file = CLOCK_IAMGES;
    ImageInfo* image = ImageCacheManager::GetInstance().LoadOneInMultiRes(resId, file);
    if (image == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "LoadClockImage %x fail", resId);
        return false;
    }
    view->SetSrc(image);
    return true;
}

bool AmbientClockView::InitSweepClock(void)
{
    if (!InitCenterCircleView()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitCenterCircleView() fail");
        return false;
    }

    InitHandView();
    return true;
}

void AmbientClockView::InitDateTime(void)
{
    struct tm pTM;
    time_t seconds;
    time(&seconds);
    localtime_r(&seconds, &pTM);
    mainSweepClock_->SetTime24Hour(pTM.tm_hour, pTM.tm_min, pTM.tm_sec);
}

void AmbientClockView::Update(void)
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    uint64_t second = tv.tv_sec;
    uint64_t uSecond = tv.tv_usec;
    uint64_t minute = second / MIN_TO_HOUR;
    uint64_t hour = minute / MIN_TO_HOUR + timezone / (MIN_TO_HOUR * MIN_TO_HOUR);
    uint8_t realHour = static_cast<uint8_t>(hour % HOURLY_SYSTEM);
    uint8_t realMinute = static_cast<uint8_t>(minute % MIN_TO_HOUR);
    uint16_t realMilliSecond = static_cast<uint16_t>((second % MIN_TO_HOUR) *
        SEC_TO_MILLI + uSecond / USEC_TO_MILLI);
    mainSweepClock_->SetSweepTime12Hour(realHour, realMinute, realMilliSecond, 1);
}

void AmbientClockView::OnCreateView(void* data)
{
    fragmentView_.SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AmbientClockView::InitView");

    mainSweepClock_ = new UISweepClock();
    if (mainSweepClock_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "mainSweepClock new fail");
        return;
    }
    if (!InitSweepClock()) {
        return;
    }

    fragmentView_.Add(mainSweepClock_);

    mainSweepClock_->SetDragParentInstead(true);
    mainSweepClock_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);

    // {214, 3}: position x, y {14, 223}: center x, y
    mainSweepClock_->SetHandImage(UISweepClock::HandType::SECOND_HAND, *secondHand_,
        { POINT_POSITION_X, SECOND_POINT_POSITION_Y }, { POINT_CENTER_X, POINT_CENTER_Y });
    mainSweepClock_->SetHandImage(UISweepClock::HandType::MINUTE_HAND, *minuteHand_,
        { POINT_POSITION_X, POINT_POSITION_Y }, { POINT_CENTER_X, POINT_CENTER_Y });
    mainSweepClock_->SetHandImage(UISweepClock::HandType::HOUR_HAND, *hourHand_, { POINT_POSITION_X, POINT_POSITION_Y },
        { POINT_CENTER_X, POINT_CENTER_Y });
    mainSweepClock_->SetHandCircleImage(UISweepClock::HandType::SECOND_HAND, *smallCircle_,
        { SMALLCIRCLE_POSITION_X, SMALLCIRCLE_POSITION_Y });
    mainSweepClock_->SetHandCircleImage(UISweepClock::HandType::MINUTE_HAND, *medCircle_,
        { MEDCIRCLE_POSITION_X, MEDCIRCLE_POSITION_Y });
    mainSweepClock_->SetHandCircleImage(UISweepClock::HandType::HOUR_HAND, *bigCircle_,
        { BIGCIRCLE_POSITION_X, BIGCIRCLE_POSITION_Y });

    mainSweepClock_->SetInitTime24Hour(0, 0, 0);
    mainSweepClock_->SetWorkMode(UISweepClock::WorkMode::NORMAL);
    InitDateTime();
}

void AmbientClockView::OnDestroyView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AmbientClockView::OnDestroyView");
    if (mainSweepClock_ != nullptr) {
        mainSweepClock_->RemoveAll();
        delete secondHand_;
        secondHand_ = nullptr;
        delete minuteHand_;
        minuteHand_ = nullptr;
        delete hourHand_;
        hourHand_ = nullptr;
        delete hand24Hour_;
        hand24Hour_ = nullptr;
        delete bigCircle_;
        bigCircle_ = nullptr;
        delete medCircle_;
        medCircle_ = nullptr;
        delete smallCircle_;
        smallCircle_ = nullptr;
        fragmentView_.Remove(mainSweepClock_);
        delete mainSweepClock_;
        mainSweepClock_ = nullptr;
    }
}
}

