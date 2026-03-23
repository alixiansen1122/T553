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

#ifndef MAIN_CLOCK_VIEW_H
#define MAIN_CLOCK_VIEW_H

#include <string>
#include <ctime>
#include "components/ui_label.h"
#include "components/root_view.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label_button.h"
#include "components/ui_sweep_clock.h"
#include "font/ui_font_bitmap.h"
#include "font/ui_font.h"
#include "graphic_config.h"
#include "UiConfig.h"
#include "AppGroupView.h"
#include "animator/animator_manager.h"
#include "sys/time.h"
#include "securec.h"

namespace OHOS {
class ClockDialAnimatorCallback : public AnimatorCallback {
public:
    explicit ClockDialAnimatorCallback(UISweepClock *clock) : sweepClock(clock) {}
    ~ClockDialAnimatorCallback() override {}

    void Callback(UIView *view) override
    {
        UNUSED(view);
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        uint64_t second = static_cast<uint64_t>(tv.tv_sec);
        uint64_t uSecond = static_cast<uint64_t>(tv.tv_usec);
        uint64_t minute = second / MIN_TO_HOUR;
        uint64_t hour = minute / MIN_TO_HOUR;
        uint8_t realHour = static_cast<uint8_t>(hour % HOURLY_SYSTEM);
        uint8_t realMinute = static_cast<uint8_t>(minute % MIN_TO_HOUR);
        uint16_t realMilliSecond = static_cast<uint16_t>((second % MIN_TO_HOUR) *
            SEC_TO_MILLI + uSecond / USEC_TO_MILLI);
        sweepClock->SetSweepTime12Hour(realHour, realMinute, realMilliSecond, 1);
    }

private:
    static constexpr int16_t SEC_TO_MILLI = 1000;
    static constexpr int16_t USEC_TO_MILLI = 1000;
    static constexpr int16_t MIN_TO_HOUR = 60;
    static constexpr int16_t HOURLY_SYSTEM = 24;
    static constexpr int16_t INTERVAL = 30;

    UISweepClock *sweepClock{nullptr};
};

class MainClockView : public AppGroupView {
public:
    MainClockView();
    ~MainClockView() override;
    MainClockView(const MainClockView &) = delete;
    MainClockView &operator=(const MainClockView &) = delete;
    static MainClockView *GetInstance(void);
    bool InitView() override;
    void RefreshStep(int step);
    void RefreshHeartRate(int value);
private:
    void InitStepValue();
    bool InitLabelView(void);
    bool InitCenterCircleView(void);
    bool InitHandView(void);
    void InitDateTime(void);
    bool InitSweepClock(void);
    bool LoadClockImage(Image *view, uint32_t resId);
    bool LoadClockImage(UIImageView *view, uint32_t resId);
    UIImageView *curFace{nullptr};
    UILabelButton *stepLabel{nullptr};
    UILabel *batteryLabel{nullptr};
    UILabel *weekLabel{nullptr};
    UILabel *heartRateLabel{nullptr};
    ClockDialAnimatorCallback *callBack{nullptr};
    Animator *clockAnimator{nullptr};
    UISweepClock *mainSweepClock{nullptr};
    Image *secondHand{nullptr};
    Image *minuteHand{nullptr};
    Image *hourHand{nullptr};
    UIImageView *heartRate{nullptr};
    Image *hand24Hour{nullptr};
    UIImageView *weekDay{nullptr};
    UIImageView *progress{nullptr};
    Image *bigCircle{nullptr};
    Image *medCircle{nullptr};
    Image *smallCircle{nullptr};
};
} // namespace OHOS
#endif // MAIN_CLOCK_VIEW_H
