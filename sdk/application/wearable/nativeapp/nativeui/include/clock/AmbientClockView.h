/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: AmbientClockView
 * Author:
 * Create: 2024-10-30
 */

#ifndef AMBIENT_VIEW_GROUP_H
#define AMBIENT_VIEW_GROUP_H

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
#include "components/ui_fragment.h"
#include "animator/animator_manager.h"
#include "sys/time.h"
#include "common/periodicupdate_interface.h"

namespace OHOS {
class AmbientClockView : public UIFragment, public PeriodicUpdateInterface {
public:
    AmbientClockView();
    ~AmbientClockView() override;
    AmbientClockView(const AmbientClockView &) = delete;
    AmbientClockView &operator=(const AmbientClockView &) = delete;
    void Update(void) override;
    uint32_t GetPeriod(void) override
    {
        return 1000; // 1000: ms
    }

private:
    void OnCreateView(void* data) override;
    void OnDestroyView() override;
    bool InitCenterCircleView(void);
    bool InitHandView(void);
    void InitDateTime(void);
    bool InitSweepClock(void);
    bool LoadClockImage(Image *view, uint32_t resId);
    UISweepClock *mainSweepClock_{nullptr};
    Image *secondHand_{nullptr};
    Image *minuteHand_{nullptr};
    Image *hourHand_{nullptr};
    Image *hand24Hour_{nullptr};
    Image *bigCircle_{nullptr};
    Image *medCircle_{nullptr};
    Image *smallCircle_{nullptr};
}; // class AmbientClockView
} // namespace OHOS
#endif // AMBIENT_VIEW_GROUP_H

