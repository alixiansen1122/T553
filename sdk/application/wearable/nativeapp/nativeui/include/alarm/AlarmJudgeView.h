/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Alarm
 * Author:
 * Create: 2021-09-11
 */

#ifndef ALARM_JUDGE_VIEW_H
#define ALARM_JUDGE_VIEW_H

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
class AlarmJudgeView {
public:
    AlarmJudgeView();
    virtual ~AlarmJudgeView();
    UIScrollView *InitJudgeView();
    void OnStop();
private:
    void CreateImage(void);
    UIScrollView *container{nullptr};
    UILabel *labelText{nullptr};
    UIImageView *imageViewDelete{nullptr};
    UIImageView *imageViewTrue{nullptr};
};
} // namespace OHOS
#endif