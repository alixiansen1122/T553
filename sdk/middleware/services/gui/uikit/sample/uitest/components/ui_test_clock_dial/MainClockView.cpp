/*
 * Copyright (c) 2022 CompanyNameMagicTag.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ui_resource_image.h"
#include "components/ui_label.h"
#include "common/image_cache_manager.h"
#include "MainClockView.h"

namespace OHOS {
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

MainClockView::MainClockView()
{
    GRAPHIC_LOGD("MainClockView::MainClockView\n");
    g_pMainClockView = this;
}

MainClockView::~MainClockView()
{
    GRAPHIC_LOGD("MainClockView::~MainClockView\n");
    if (clockAnimator != nullptr) {
        clockAnimator->Stop();
        delete clockAnimator;
        clockAnimator = nullptr;
    }
    mainSweepClock->RemoveAll();
    delete curFace;
    curFace = nullptr;
    delete stepLabel;
    stepLabel = nullptr;
    delete batteryLabel;
    batteryLabel = nullptr;
    delete callBack;
    callBack = nullptr;
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
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(CLOCK_IAMGES);
}

MainClockView *MainClockView::GetInstance(void)
{
    return g_pMainClockView;
}

void MainClockView::InitStepValue()
{
    GRAPHIC_LOGD("MainClockView::InitStepValue\n");

    char stepChar[STEP_CHAR_NUM] = {0};
    if (sprintf_s(stepChar, STEP_CHAR_NUM, "%d", 115200) == -1) { // 115200: default step count
        GRAPHIC_LOGE("MainClockView::InitStepValue error\n");
        return;
    }
    if (stepLabel == nullptr) {
        GRAPHIC_LOGE("MainClockView::InitStepValue nullptr\n");
        return;
    }
    stepLabel->SetText(stepChar);
    stepLabel->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::RELEASED);
    stepLabel->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    stepLabel->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::INACTIVE);
}

bool MainClockView::InitLabelView(void)
{
    stepLabel = new UILabelButton();
    if (stepLabel == nullptr) {
        GRAPHIC_LOGE("stepLabel new fail\n");
        return false;
    }
    stepLabel->SetPosition(STEP_LABEL_X, STEP_LABEL_Y);
    stepLabel->Resize(STEP_LABLE_WIDTH, STEP_LABLE_SIZE);
    stepLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    stepLabel->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    stepLabel->SetTouchable(true);
    InitStepValue();

    batteryLabel = new UILabel();
    if (batteryLabel == nullptr) {
        GRAPHIC_LOGE("batteryLabel new fail\n");
        return false;
    }
    batteryLabel->SetPosition(BATTERYP_LABEL_X, BATTERYP_LABEL_Y, TEXT_WIDTH, TEXT_HEIGHT);
    batteryLabel->SetText("90%");
    batteryLabel->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    batteryLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    batteryLabel->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);

    heartRateLabel = new UILabel();
    if (heartRateLabel == nullptr) {
        GRAPHIC_LOGE("heartRateLabel new fail\n");
        return false;
    }
    heartRateLabel->SetPosition(HEARTRATELABEL_POSITION_X, HEARTRATELABEL_POSITION_Y, TEXT_WIDTH, TEXT_HEIGHT);
    heartRateLabel->SetText("--");
    heartRateLabel->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    heartRateLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    heartRateLabel->SetStyle(STYLE_BACKGROUND_COLOR, OPA_OPAQUE);

    weekLabel = new UILabel();
    if (weekLabel == nullptr) {
        GRAPHIC_LOGE("weekLabel new fail\n");
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
        GRAPHIC_LOGE("bigCircle new fail\n");
        return false;
    }
    if (!LoadClockImage(bigCircle, CLOCK_BIGCIRCLE_IMAGE_PATH)) {
        return false;
    }

    medCircle = new Image();
    if (medCircle == nullptr) {
        GRAPHIC_LOGE("medCircle new fail\n");
        return false;
    }
    if (!LoadClockImage(medCircle, CLOCK_MEDCIRCLE_IMAGE_PATH)) {
        return false;
    }

    smallCircle = new Image();
    if (smallCircle == nullptr) {
        GRAPHIC_LOGE("smallCircle new fail\n");
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
        GRAPHIC_LOGE("secondHand new fail\n");
        return false;
    }
    if (!LoadClockImage(secondHand, CLOCK_SECONDHAND)) {
        return false;
    }

    minuteHand = new Image();
    if (minuteHand == nullptr) {
        GRAPHIC_LOGE("minuteHand new fail\n");
        return false;
    }
    if (!LoadClockImage(minuteHand, CLOCK_MINUTEHAND)) {
        return false;
    }
    hourHand = new Image();
    if (hourHand == nullptr) {
        GRAPHIC_LOGE("hourHand new fail\n");
        return false;
    }
    if (!LoadClockImage(hourHand, CLOCK_HOURHAND)) {
        return false;
    }

    hand24Hour = new Image();
    if (hand24Hour == nullptr) {
        GRAPHIC_LOGE("hand24Hour new fail\n");
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
        GRAPHIC_LOGE("LoadClockImage %x fail\n", resId);
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
        GRAPHIC_LOGE("LoadClockImage %x fail\n", resId);
        return false;
    }
    view->SetSrc(image);
    return true;
}

bool MainClockView::InitSweepClock(void)
{
    curFace = new UIImageView();
    if (curFace == nullptr) {
        GRAPHIC_LOGE("curFace new fail\n");
        return false;
    }
    curFace->SetPosition(0, 0);

    if (!LoadClockImage(curFace, CLOCK_MAIN_FACE_IMAGE)) {
        return false;
    }

    if (!InitLabelView()) {
        return false;
    }
    if (!InitCenterCircleView()) {
        return false;
    }
    heartRate = new UIImageView();
    if (heartRate == nullptr) {
        GRAPHIC_LOGE("heartRate new fail\n");
        return false;
    }
    heartRate->SetPosition(HEARTRATE_POSITION_X, HEARTRATE_POSITION_Y);
    if (!LoadClockImage(heartRate, CLOCK_HEARTRATE_IMAGE_PATH)) {
        return false;
    }

    progress = new UIImageView();
    if (progress == nullptr) {
        GRAPHIC_LOGE("progress new fail\n");
        return false;
    }
    progress->SetPosition(PROGRESS_POSITION_X, PROGRESS_POSITION_Y);
    if (!LoadClockImage(progress, CLOCK_PROGRESS_IMAGE_PATH)) {
        return false;
    }

    InitHandView();
    weekDay = new UIImageView();
    if (weekDay == nullptr) {
        GRAPHIC_LOGE("weekDay new fail\n");
        return false;
    }
    weekDay->SetPosition(WEEKDAY_POSITION_X, WEEKDAY_POSITION_Y);
    if (!LoadClockImage(weekDay, CLOCK_THURSDAY_IMAGE_PATH)) {
        return false;
    }
    return true;
}

bool MainClockView::InitView()
{
    mainSweepClock = new UISweepClock();
    if (mainSweepClock == nullptr) {
        GRAPHIC_LOGE("mainSweepClock new fail\n");
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
    mainSweepClock->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());

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

    callBack = new ClockDialAnimatorCallback(mainSweepClock);
    if (callBack == nullptr) {
        GRAPHIC_LOGE("callBack new fail\n");
        return false;
    }
    clockAnimator = new Animator(callBack, mainSweepClock, 0, true);
    if (clockAnimator == nullptr) {
        GRAPHIC_LOGE("clockAnimator new fail\n");
        return false;
    }
    clockAnimator->Start();
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
    struct tm pTM;
    time_t seconds;
    time(&seconds);
    localtime_r(&seconds, &pTM);
    mainSweepClock->SetTime24Hour(pTM.tm_hour, pTM.tm_min, pTM.tm_sec);
}
}
