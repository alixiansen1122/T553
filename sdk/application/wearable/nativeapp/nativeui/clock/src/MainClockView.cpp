/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: MainClockView
 * Author:
 * Create: 2021-08-14
 */

#include "clock/MainClockView.h"
#include "main/MainPresenterSample.h"
#include "components/ui_label.h"
#include "wearable_log.h"
#include "ui_resource_image.h"
#include "common/image_cache_manager.h"
#include "UIWatchDialFactory.h"
#include "main/DialBinTypesV2.h"
#include <ctime>

namespace OHOS {

const DialInfo dialInfoConfig = {
        DIAL_CLOCK, "扫秒表盘", PNG_SET_DIAL_CLOCK, DEFAULT_TASK_PERIOD, DIAL_DISPLAY_CAPABILITY_NORMAL_ONLY
    };

REGIST_WATCH_DIAL(DIAL_DISPLAY_SUPPORT_NORMAL, DIAL_CLOCK, MainClockView, &dialInfoConfig);

static MainClockView *g_pMainClockView = nullptr;

static constexpr uint16_t FONT_DEFAULT_SIZE = 25;
static constexpr uint16_t FONT_TEXT_SIZE = 24;
static constexpr uint16_t STEP_LABEL_X = 100;
static constexpr uint16_t STEP_LABEL_Y = 230;
static constexpr uint16_t BATTERYP_LABEL_X = 315;
static constexpr uint16_t BATTERYP_LABEL_Y = 230;
static constexpr uint16_t TEXT_WIDTH = 60;
static constexpr uint16_t TEXT_HEIGHT = 35;
static constexpr uint16_t POINT_POSITION_X = 202;
static constexpr uint16_t POINT_POSITION_Y = 16;
static constexpr uint16_t SECOND_POINT_POSITION_Y = 16;
static constexpr uint16_t POINT_CENTER_X = 25;
static constexpr uint16_t POINT_CENTER_Y = 211;
static constexpr uint16_t STEP_CHAR_NUM = 255;
static constexpr int16_t STEP_LABLE_WIDTH = 75;
static constexpr int16_t STEP_LABLE_SIZE = 35;
static constexpr uint16_t WEEKLABEL_POSITION_X = 247;
static constexpr uint16_t WEEKLABEL_POSITION_Y = 122;
static constexpr uint16_t HEARTRATELABEL_POSITION_X = 198;
static constexpr uint16_t HEARTRATELABEL_POSITION_Y = 330;
static constexpr uint16_t HEARTRATE_POSITION_X = 156;
static constexpr uint16_t HEARTRATE_POSITION_Y = 260;
static constexpr uint16_t PROGRESS_POSITION_X = 53;
static constexpr uint16_t PROGRESS_POSITION_Y = 54;
static constexpr uint16_t BIGCIRCLE_POSITION_X = 202;
static constexpr uint16_t BIGCIRCLE_POSITION_Y = 202;
static constexpr uint16_t MEDCIRCLE_POSITION_X = 202;
static constexpr uint16_t MEDCIRCLE_POSITION_Y = 202;
static constexpr uint16_t SMALLCIRCLE_POSITION_X = 202;
static constexpr uint16_t SMALLCIRCLE_POSITION_Y = 202;
static constexpr uint16_t WEEKDAY_POSITION_X = 193;
static constexpr uint16_t WEEKDAY_POSITION_Y = 127;
static constexpr int16_t SEC_TO_MILLI = 1000;
static constexpr int16_t USEC_TO_MILLI = 1000;
static constexpr int16_t MIN_TO_HOUR = 60;
static constexpr int16_t HOURLY_SYSTEM = 24;
static constexpr int16_t INTERVAL = 30;

MainClockView::MainClockView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainClockView::MainClockView");
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    g_pMainClockView = this;
}

MainClockView::~MainClockView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainClockView::~MainClockView");
    if (viewiInitStatus) {
        mainSweepClock->RemoveAll();
        delete curFace;
        curFace = nullptr;
        delete stepLabel;
        stepLabel = nullptr;
        delete batteryLabel;
        batteryLabel = nullptr;
        delete secondHand;
        secondHand = nullptr;
        delete minuteHand;
        minuteHand = nullptr;
        delete hourHand;
        hourHand = nullptr;
        delete weekLabel;
        weekLabel = nullptr;
        delete heartRateLabel;
        heartRateLabel = nullptr;
        delete heartRate;
        heartRate = nullptr;
        delete hand24Hour;
        hand24Hour = nullptr;
        delete weekDay;
        weekDay = nullptr;
        delete progress;
        progress = nullptr;
        delete bigCircle;
        bigCircle = nullptr;
        delete medCircle;
        medCircle = nullptr;
        delete smallCircle;
        smallCircle = nullptr;
        Remove(mainSweepClock);
        delete mainSweepClock;
        mainSweepClock = nullptr;
        g_pMainClockView = nullptr;
    }
}

MainClockView *MainClockView::GetInstance(void)
{
    return g_pMainClockView;
}

void MainClockView::InitStepValue()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainClockView::InitStepValue");
    uint32_t stepValue;
    int weekToday = ActivityModel::GetInstance()->GetWeekToday();
    ActivityModel::GetInstance()->GetStepData((StepDataDistribute)weekToday, &stepValue);
    char stepChar[STEP_CHAR_NUM] = {0};
    if (sprintf_s(stepChar, STEP_CHAR_NUM, "%d", stepValue) == -1) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainClockView::InitStepValue error");
        return;
    }
    if (stepLabel == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainClockView::InitStepValue nullptr");
        return;
    }
    stepLabel->SetText(stepChar);
    stepLabel->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::RELEASED);
    stepLabel->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    stepLabel->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::INACTIVE);
}

bool MainClockView::InitLabelView(void)
{
    UIView::OnClickListener *clickListener = (UIView::OnClickListener *)OHOS::MainPresenterSample::GetInstance();
    stepLabel = new UILabelButton();
    if (stepLabel == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "stepLabel new fail");
        return false;
    }
    stepLabel->SetPosition(STEP_LABEL_X, STEP_LABEL_Y);
    stepLabel->Resize(STEP_LABLE_WIDTH, STEP_LABLE_SIZE);
    stepLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    stepLabel->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    stepLabel->SetViewId(MAIN_CLICK_ACTIVITY);
    stepLabel->SetTouchable(true);
    stepLabel->SetOnClickListener(clickListener);
    InitStepValue();

    batteryLabel = new UILabel();
    if (batteryLabel == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "batteryLabel new fail");
        return false;
    }
    batteryLabel->SetPosition(BATTERYP_LABEL_X, BATTERYP_LABEL_Y, TEXT_WIDTH, TEXT_HEIGHT);
    batteryLabel->SetText("90%");
    batteryLabel->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    batteryLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    batteryLabel->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);

    heartRateLabel = new UILabel();
    if (heartRateLabel == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "heartRateLabel new fail");
        return false;
    }
    heartRateLabel->SetPosition(HEARTRATELABEL_POSITION_X, HEARTRATELABEL_POSITION_Y, TEXT_WIDTH, TEXT_HEIGHT);
    heartRateLabel->SetText("--");
    heartRateLabel->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    heartRateLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    heartRateLabel->SetStyle(STYLE_BACKGROUND_COLOR, OPA_OPAQUE);

    weekLabel = new UILabel();
    if (weekLabel == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "weekLabel new fail");
        return false;
    }
    weekLabel->SetPosition(WEEKLABEL_POSITION_X, WEEKLABEL_POSITION_Y, TEXT_WIDTH, TEXT_HEIGHT);
    weekLabel->SetText("2");
    weekLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_TEXT_SIZE);
    weekLabel->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
    return true;
}

bool MainClockView::InitCenterCircleView(void)
{
    bigCircle = new Image();
    if (bigCircle == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "bigCircle new fail");
        return false;
    }
    if (!LoadClockImage(bigCircle, CLOCK_BIGCIRCLE_IMAGE_PATH)) {
        return false;
    }

    medCircle = new Image();
    if (medCircle == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "medCircle new fail");
        return false;
    }
    if (!LoadClockImage(medCircle, CLOCK_MEDCIRCLE_IMAGE_PATH)) {
        return false;
    }

    smallCircle = new Image();
    if (smallCircle == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "smallCircle new fail");
        return false;
    }
    if (!LoadClockImage(smallCircle, CLOCK_CIRCLE_IMAGE_PATH)) {
        return false;
    }
    return true;
}

bool MainClockView::InitHandView(void)
{
    secondHand = new Image();
    if (secondHand == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "secondHand new fail");
        return false;
    }
    if (!LoadClockImage(secondHand, CLOCK_SECONDHAND)) {
        return false;
    }

    minuteHand = new Image();
    if (minuteHand == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "minuteHand new fail");
        return false;
    }
    if (!LoadClockImage(minuteHand, CLOCK_MINUTEHAND)) {
        return false;
    }
    hourHand = new Image();
    if (hourHand == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "hourHand new fail");
        return false;
    }
    if (!LoadClockImage(hourHand, CLOCK_HOURHAND)) {
        return false;
    }

    hand24Hour = new Image();
    if (hand24Hour == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "hand24Hour new fail");
        return false;
    }
    if (!LoadClockImage(hand24Hour, CLOCK_HAND24HOUR_IMAGE_PATH)) {
        return false;
    }
    return true;
}

bool MainClockView::LoadClockImage(Image *view, uint32_t resId)
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

bool MainClockView::LoadClockImage(UIImageView *view, uint32_t resId)
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

bool MainClockView::InitSweepClock(void)
{
    curFace = new UIImageView();
    if (curFace == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "curFace new fail");
        return false;
    }
    curFace->SetPosition(0, 0);

    if (!LoadClockImage(curFace, CLOCK_MAIN_FACE_IMAGE)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "curFace LoadClockImage CLOCK_MAIN_FACE_IMAGE fail");
        return false;
    }

    if (!InitLabelView()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitLabelView() fail");
        return false;
    }
    if (!InitCenterCircleView()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitCenterCircleView() fail");
        return false;
    }
    heartRate = new UIImageView();
    if (heartRate == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "heartRate new fail");
        return false;
    }
    heartRate->SetPosition(HEARTRATE_POSITION_X, HEARTRATE_POSITION_Y);
    if (!LoadClockImage(heartRate, CLOCK_HEARTRATE_IMAGE_PATH)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "heartRate LoadClockImage CLOCK_HEARTRATE_IMAGE_PATH fail");
        return false;
    }

    progress = new UIImageView();
    if (progress == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "progress new fail");
        return false;
    }
    progress->SetPosition(PROGRESS_POSITION_X, PROGRESS_POSITION_Y);
    if (!LoadClockImage(progress, CLOCK_PROGRESS_IMAGE_PATH)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "progress LoadClockImage CLOCK_PROGRESS_IMAGE_PATH fail");
        return false;
    }

    InitHandView();
    weekDay = new UIImageView();
    if (weekDay == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "weekDay new fail");
        return false;
    }
    weekDay->SetPosition(WEEKDAY_POSITION_X, WEEKDAY_POSITION_Y);
    if (!LoadClockImage(weekDay, CLOCK_THURSDAY_IMAGE_PATH)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "weekDay LoadClockImage CLOCK_THURSDAY_IMAGE_PATH fail");
        return false;
    }
    return true;
}

void MainClockView::PreLoad(void)
{
    if (!viewiInitStatus) {
        InitView();
        viewiInitStatus = true;
    }
}

void MainClockView::UnLoad(void)
{
    // Main clock view is commonly used page, so we do not release it's resources inner slice.
}

bool MainClockView::InitView(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainClockView::InitView");

    mainSweepClock = new UISweepClock();
    if (mainSweepClock == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "mainSweepClock new fail");
        return false;
    }
    if (!InitSweepClock()) {
        return false;
    }

    mainSweepClock->Add(curFace);
    mainSweepClock->Add(stepLabel);
    mainSweepClock->Add(heartRate);
    mainSweepClock->Add(weekLabel);
    mainSweepClock->Add(heartRateLabel);
    mainSweepClock->Add(weekDay);
    mainSweepClock->Add(progress);
    mainSweepClock->Add(batteryLabel);
    Add(mainSweepClock);

    mainSweepClock->SetDragParentInstead(true);
    mainSweepClock->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);

    // {214, 3}: position x, y {14, 223}: center x, y
    mainSweepClock->SetHandImage(UISweepClock::HandType::SECOND_HAND, *secondHand,
        { POINT_POSITION_X, SECOND_POINT_POSITION_Y }, { POINT_CENTER_X, POINT_CENTER_Y });
    mainSweepClock->SetHandImage(UISweepClock::HandType::MINUTE_HAND, *minuteHand,
        { POINT_POSITION_X, POINT_POSITION_Y }, { POINT_CENTER_X, POINT_CENTER_Y });
    mainSweepClock->SetHandImage(UISweepClock::HandType::HOUR_HAND, *hourHand, { POINT_POSITION_X, POINT_POSITION_Y },
        { POINT_CENTER_X, POINT_CENTER_Y });
    mainSweepClock->SetHandCircleImage(UISweepClock::HandType::SECOND_HAND, *smallCircle,
        { SMALLCIRCLE_POSITION_X, SMALLCIRCLE_POSITION_Y });
    mainSweepClock->SetHandCircleImage(UISweepClock::HandType::MINUTE_HAND, *medCircle,
        { MEDCIRCLE_POSITION_X, MEDCIRCLE_POSITION_Y });
    mainSweepClock->SetHandCircleImage(UISweepClock::HandType::HOUR_HAND, *bigCircle,
        { BIGCIRCLE_POSITION_X, BIGCIRCLE_POSITION_Y });

    mainSweepClock->SetInitTime24Hour(0, 0, 0);
    mainSweepClock->SetWorkMode(UISweepClock::WorkMode::NORMAL);
    InitDateTime();
    SetCoverable(true);
    return true;
}

void MainClockView::RefreshStep(int step)
{
    char stepChar[STEP_CHAR_NUM] = {0};

    (void)sprintf_s(stepChar, STEP_CHAR_NUM, "%d", step);
    stepLabel->SetText(stepChar);
}

void MainClockView::RefreshHeartRate(int value)
{
    if (value == 0) {
        heartRateLabel->SetText("--");
        return;
    }
    char heartRateChar[STEP_CHAR_NUM] = {0};

    (void)sprintf_s(heartRateChar, STEP_CHAR_NUM, "%d", value);
    heartRateLabel->SetText(heartRateChar);
}

void MainClockView::InitDateTime(void)
{
    Update();
}

void MainClockView::Update(void)
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
    mainSweepClock->SetSweepTime12Hour(realHour, realMinute, realMilliSecond, 1);
}
}
