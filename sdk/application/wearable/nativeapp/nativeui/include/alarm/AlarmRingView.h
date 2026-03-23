/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Alarm
 * Author:
 * Create: 2021-09-11
 */

#ifndef ALARM_RING_VIEW_H
#define ALARM_RING_VIEW_H

#include "components/ui_label.h"
#include "components/root_view.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_button.h"
#include "components/ui_label_button.h"
#include "ChangeSliceListener.h"
#include "components/ui_digital_clock.h"
#include "UiConfig.h"
#include "alarm/AlarmClockModel.h"
#include "ChangeSliceListener.h"

namespace OHOS {
class AlarmRingView {
public:
    AlarmRingView();
    virtual ~AlarmRingView();
    static AlarmRingView *GetInstance(void);
    UIViewGroup *InitRingView(void);
    void AlarmRingDelayShowtimer();
    void CancelTimerHandle(void);
private:
    UIViewGroup *group{nullptr};
    UIImageView *imageRingClock{nullptr};
    UILabel *labelRing{nullptr};
};
} // namespace OHOS
#endif