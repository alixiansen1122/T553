/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: GradientClockView
 * Create: 2025-04
 */

#ifndef GRADIENT_CLOCK_VIEW_H
#define GRADIENT_CLOCK_VIEW_H

#include <ctime>
#include "View.h"
#include "components/ui_canvas_ext.h"
#include "ChangeSliceListener.h"
#include "UiConfig.h"
#include "components/ui_card_page.h"
#include "GradientDateView.h"
#include "common/periodicupdate_interface.h"

namespace OHOS {

struct sectorInfo {
    float startAngle;
    float angleRange;
    uint16_t sectorRadius;
    uint16_t stopCount;
    ColorStop* colorStop;

    sectorInfo(float start, float angle, uint16_t radius, uint16_t count, ColorStop* color = nullptr):
         startAngle(start), angleRange(angle), sectorRadius(radius), stopCount(count), colorStop(color)
    {}
};

class GradientClockView : public UICardPage, public PeriodicUpdateInterface {
public:
    GradientClockView();
    static GradientClockView *GetInstance(void);
    ~GradientClockView() override;
    GradientClockView(const GradientClockView &) = delete;
    GradientClockView &operator=(const GradientClockView &) = delete;
    bool InitView(void);
    void PreLoad(void) override;
    void Update(void) override;
    uint32_t GetPeriod(void) override
    {
        return DEFAULT_TASK_PERIOD;
    }

private:
    void UpdateHands(uint8_t minute, uint8_t second, uint64_t milliSecond);
    void UpdateCanvas(UICanvasExt* canvas, const sectorInfo& sectorInfo);
    void SafeDelete(UIView* ptr);
    UICanvasExt* backgroudCanvas_;
    UICanvasExt* secondHandCanvas_;
    UICanvasExt* minuteHandCanvas_;
    bool viewiInitStatus_{false};
    uint8_t second_ = 0xFF;
    float secondAngle_;
    GradientDateView* gradientDateView_{nullptr};
}; // class GradientClockView
} // namespace OHOS

#endif // GRADIENT_CLOCK_VIEW_H