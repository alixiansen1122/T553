/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: MainClockView
 * Author:
 * Create: 2021-10-18
 */

#ifndef MAIN_CLOCK_VIEW_H
#define MAIN_CLOCK_VIEW_H

#include <string>
#include <ctime>
#include "View.h"
#include "components/ui_label.h"
#include "components/root_view.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label_button.h"
#include "components/ui_sweep_clock.h"
#include "ChangeSliceListener.h"
#include "font/ui_font_bitmap.h"
#include "font/ui_font.h"
#include "graphic_config.h"
#include "main/HealthModel.h"
#include "activity/ActivityModel.h"
#include "UiConfig.h"
#include "components/ui_card_page.h"
#include "animator/animator_manager.h"
#include <ctime>
#include "sys/time.h"
#include "common/periodicupdate_interface.h"

namespace OHOS {
class MainClockView : public UICardPage, public PeriodicUpdateInterface {
public:
    MainClockView();
    ~MainClockView() override;
    static MainClockView *GetInstance(void);
    bool InitView(void);
    void PreLoad(void) override;
    void UnLoad(void) override;
    void RefreshStep(int step);
    void RefreshHeartRate(int value);
    void ClockAnimatorStart(void);
    void ClockAnimatorStop(void);
    void Update(void) override;
    uint32_t GetPeriod(void) override
    {
        return DEFAULT_TASK_PERIOD;
    }

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
    bool viewiInitStatus{false};
};
} // namespace OHOS
#endif // MAIN_CLOCK_VIEW_H
