/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Alarm
 * Author:
 * Create: 2021-09-11
 */

#ifndef ALARM_MAX_CLOCK_H
#define ALARM_MAX_CLOCK_H

#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/root_view.h"
#include "components/ui_time_picker.h"
#include "components/ui_digital_clock.h"
#include "components/ui_scroll_view.h"
#include "components/ui_view_group.h"
#include "layout/grid_layout.h"
#include "font/ui_font.h"
#include "common/screen.h"
#include "alarm/AlarmPresenter.h"
#include "alarm/AlarmClockModel.h"

namespace OHOS {
void CancelScanHandle(void);

class AlarmMaxClock {
public:
    AlarmMaxClock();
    virtual ~AlarmMaxClock();
    UIScrollView *InitAlarmMax();
    static AlarmMaxClock *GetInstance();
    void AlarmMaxDelayExit(void);
private:
    UIScrollView *container{nullptr};
    UILabel *labelText{nullptr};
};
} // namespace OHOS
#endif