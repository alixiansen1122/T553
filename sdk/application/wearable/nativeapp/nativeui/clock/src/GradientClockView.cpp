/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: GradientClockView
 * Create: 2025-04
 */

#include "clock/GradientClockView.h"
#include "wearable_log.h"
#include "clock/ClockResourceImage.h"
#include "common/screen.h"
#include "sys/time.h"
#include "UIWatchDialFactory.h"
#include "main/DialBinTypesV2.h"

namespace OHOS {

const DialInfo dialInfoConfig = {
        GRADIENT_CLOCK, "渐变表盘", PNG_SET_GRADIENT_CLOCK, DEFAULT_TASK_PERIOD, DIAL_DISPLAY_CAPABILITY_NORMAL_ONLY
    };

REGIST_WATCH_DIAL(DIAL_DISPLAY_SUPPORT_NORMAL, GRADIENT_CLOCK, GradientClockView, &dialInfoConfig);
static GradientClockView *g_pGradientClockView = nullptr;

static constexpr float SEC_TO_MILLI = 1000.0f;
static constexpr int16_t USEC_TO_MILLI = 1000;
static constexpr int16_t MIN_TO_HOUR = 60;
static constexpr float SEC_TO_MIN = 60.0f;
static constexpr float SECOND_HAND_RANGE = 160.0f;
static constexpr float MINUTE_HAND_RANGE = 120.0f;
static constexpr float SECOND_START_ANGLE = 200.0f;
static constexpr float MINUTE_START_ANGLE = 240.0f;
static constexpr int16_t ANGLE_DETAL = 3;
static constexpr int16_t USEC_TO_SEC = 60;
static constexpr float ANGEL_MINUTE_STEPS = 6.0f;
static constexpr int16_t CLOCK_RADIUS = HORIZONTAL_RESOLUTION / 2;
static constexpr int16_t CLOCK_RADIUS_RECTANGLE = (VERTICAL_RESOLUTION > HORIZONTAL_RESOLUTION) ?\
                                                    VERTICAL_RESOLUTION / 2 + 75 : HORIZONTAL_RESOLUTION / 2;
static constexpr int16_t MINUTE_RADIUS = CLOCK_RADIUS - static_cast<int16_t>(0.103524 * HORIZONTAL_RESOLUTION);
static constexpr int16_t MINUTE_RADIUS_RECTANGLE = CLOCK_RADIUS_RECTANGLE;
static constexpr int16_t MARGIN_BG = 10;
static constexpr int16_t CENTER_X = HORIZONTAL_RESOLUTION / 2;
static constexpr int16_t CENTER_Y = VERTICAL_RESOLUTION/ 2;
static constexpr int16_t MINUTE_COUNT = 2;
static ColorStop minutestops[MINUTE_COUNT] = {{0, 0x00FFFAEF}, {0.333, 0xFFFFD383}}; // Alaph 00~FF
static constexpr int16_t SECOND_COUNT = 2;
static ColorStop secondstops[SECOND_COUNT] = {{0, 0x0AF8F5FD}, {0.444, 0xFF7948EA}}; // Alaph 0A~FF
static constexpr int16_t BACKGROUD_COUNT = 3;
ColorStop backgroudstops[BACKGROUD_COUNT] = {{0, 0xFF994EDE}, {0.50974, 0xFFFC62BA}, {1, 0xFFFEC45E}};

void GradientClockView::SafeDelete(UIView* ptr)
{
    if (ptr != nullptr) {
        Remove(ptr);
        delete ptr;
        ptr = nullptr;
    }
}

GradientClockView *GradientClockView::GetInstance(void)
{
    return g_pGradientClockView;
}

GradientClockView::GradientClockView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "GradientClockView::GradientClockView");
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    secondAngle_ = SECOND_START_ANGLE;
    PreLoad();
    g_pGradientClockView = this;
}

GradientClockView::~GradientClockView()
{
    if (!viewiInitStatus_) {
        return;
    }
    SafeDelete(backgroudCanvas_);
    SafeDelete(secondHandCanvas_);
    SafeDelete(minuteHandCanvas_);
    if (gradientDateView_) {
        gradientDateView_->RemoveAll();
        Remove(gradientDateView_);
        delete gradientDateView_;
        gradientDateView_ = nullptr;
    }
    g_pGradientClockView = nullptr;
}

bool GradientClockView::InitView(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "GradientClockView::InitView");

    backgroudCanvas_ = new UICanvasExt();
    if (backgroudCanvas_ != nullptr) {
        PaintExt backPaint;
        backPaint.SetStyle(Paint::PaintStyle::FILL_STYLE);
        backPaint.SetLinearGradient({-MARGIN_BG, VERTICAL_RESOLUTION, HORIZONTAL_RESOLUTION + MARGIN_BG,
                                    VERTICAL_RESOLUTION}, backgroudstops, BACKGROUD_COUNT);
        backPaint.SetAntialiased(true);
        backgroudCanvas_->SetStyle(STYLE_BACKGROUND_OPA, 0);
        backgroudCanvas_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
        uint16_t radius = (Screen::GetInstance().GetScreenShape() == ScreenShape::RECTANGLE) ?\
                            CLOCK_RADIUS_RECTANGLE : CLOCK_RADIUS;
        backgroudCanvas_->DrawCircle({CENTER_X, CENTER_Y}, radius, backPaint);
        Add(backgroudCanvas_);
    }

    secondHandCanvas_ = new UICanvasExt();
    if (secondHandCanvas_ != nullptr) {
        Add(secondHandCanvas_);
    }

    minuteHandCanvas_ = new UICanvasExt();
    if (minuteHandCanvas_ != nullptr) {
        Add(minuteHandCanvas_);
    }

    gradientDateView_ = new GradientDateView();
    if (gradientDateView_ != nullptr) {
        Add(gradientDateView_);
        gradientDateView_->InitView();
    }

    SetCoverable(true);
    Update();
    viewiInitStatus_ = true;
    return true;
}

void GradientClockView::PreLoad(void)
{
    if (!viewiInitStatus_) {
        InitView();
        viewiInitStatus_ = true;
    }
}

void GradientClockView::Update(void)
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    uint64_t second = tv.tv_sec;
    uint64_t uSecond = tv.tv_usec;
    uint64_t minute = second / MIN_TO_HOUR;
    uint8_t realMinute = static_cast<uint8_t>(minute % MIN_TO_HOUR);
    uint64_t realMilliSecond = static_cast<uint64_t>(uSecond / USEC_TO_MILLI);
    uint8_t realSecond = second % USEC_TO_SEC;

    if (gradientDateView_ != nullptr) {
        gradientDateView_->UpdateTime();
    }

    // update all canvas
    UpdateHands(realMinute, realSecond, realMilliSecond);
}

void GradientClockView::UpdateHands(uint8_t minute, uint8_t second, uint64_t milliSecond)
{
    if (second !=second_) {
        secondAngle_ = second % USEC_TO_SEC * ANGEL_MINUTE_STEPS;
        second_ = second;
    }

    // update second hand
    float secondAngle =  SECOND_START_ANGLE + secondAngle_ + milliSecond / SEC_TO_MILLI * ANGEL_MINUTE_STEPS;
    uint16_t colckRadius = (Screen::GetInstance().GetScreenShape() == ScreenShape::RECTANGLE) ?\
                            CLOCK_RADIUS_RECTANGLE :CLOCK_RADIUS;
    sectorInfo sectorStru(secondAngle, SECOND_HAND_RANGE, colckRadius, SECOND_COUNT, secondstops);
    UpdateCanvas(secondHandCanvas_, sectorStru);

    // update minute hand
    float minuteAngle = MINUTE_START_ANGLE + minute % USEC_TO_SEC * ANGEL_MINUTE_STEPS +
                        second / SEC_TO_MIN * ANGEL_MINUTE_STEPS;
    uint16_t minuteRadius = (Screen::GetInstance().GetScreenShape() == ScreenShape::RECTANGLE) ?\
                                MINUTE_RADIUS_RECTANGLE : MINUTE_RADIUS;
    sectorInfo minuteStru(minuteAngle, MINUTE_HAND_RANGE, minuteRadius, MINUTE_COUNT, minutestops);
    UpdateCanvas(minuteHandCanvas_, minuteStru);
}

void GradientClockView::UpdateCanvas(UICanvasExt *canvas, const sectorInfo& sectorInfo)
{
    if (canvas == nullptr) {
        GRAPHIC_LOGE("callback view is not UICanvasExt");
        return;
    }

    canvas->Clear();
    float sectorEnd = sectorInfo.startAngle + sectorInfo.angleRange;
    PaintExt secPaint;
    secPaint.SetStyle(Paint::PaintStyle::FILL_STYLE);
    secPaint.SetStrokeWidth(1);
    SweepGradient sweep = {CENTER_X, CENTER_Y, sectorInfo.startAngle};
    secPaint.SetSweepGradient(sweep, sectorInfo.colorStop, sectorInfo.stopCount);
    secPaint.SetOpacity(OPA_OPAQUE);
    secPaint.SetAntialiased(true);
    canvas->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    canvas->DrawSector({CENTER_X, CENTER_Y}, sectorInfo.sectorRadius,  sectorInfo.startAngle + ANGLE_DETAL,
        sectorEnd, secPaint);
    canvas->SetStyle(STYLE_BACKGROUND_OPA, 0);
}

}