/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Alarm
 * Author:
 * Create: 2021-09-11
 */

#ifndef ALARM_NEW_TIME_VIEW_H
#define ALARM_NEW_TIME_VIEW_H

#include "components/ui_label_button.h"
#include "components/ui_picker.h"
#include "components/ui_scroll_view.h"
#include "components/ui_time_picker.h"
#include "View.h"
#include "alarm/AlarmPresenter.h"
#include "alarm/AlarmClockModel.h"

namespace OHOS {
class AlarmNewTimeView : public UITimePicker::SelectedListener {
public:
    AlarmNewTimeView();
    ~AlarmNewTimeView();
    static AlarmNewTimeView *GetInstance(void);
    UIScrollView *InitNewTimeView();
    void RefreshNewTime(void);
private:
    void CreateLabelTime(void);
    UIScrollView *group{nullptr};
    UILabel *title{nullptr};
    UITimePicker *picker{nullptr};
    UILabelButton *labelButton{nullptr};
    UILabel *GetTitleLabel(const char *titleName);
};
} // namespace OHOS
#endif // ALARM_SET_TIME_VIEW_H