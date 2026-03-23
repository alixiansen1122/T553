/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Alarm
 * Author:
 * Create: 2021-09-11
 */

#ifndef ALARM_SET_TIME_VIEW_H
#define ALARM_SET_TIME_VIEW_H

#include "components/ui_label_button.h"
#include "components/ui_picker.h"
#include "components/ui_scroll_view.h"
#include "components/ui_time_picker.h"
#include "View.h"
#include "alarm/AlarmPresenter.h"
#include "alarm/AlarmClockModel.h"
#include "wearable_log.h"

namespace OHOS {
class AlarmSetTimeView {
public:
    AlarmSetTimeView();
    ~AlarmSetTimeView();
    static AlarmSetTimeView *GetInstance(void);
    UIScrollView *InitSetTimeView();
    void RefreshSetTime(uint32_t time);
private:
    UIScrollView *group{nullptr};
    UILabelButton *timeImage{nullptr};
    UILabel *title{nullptr};
    UITimePicker *picker{nullptr};
    UILabel *GetTitleLabel(const char *titleName);
    void SetLabelButton(void);
};
} // namespace OHOS
#endif // ALARM_SET_TIME_VIEW_H