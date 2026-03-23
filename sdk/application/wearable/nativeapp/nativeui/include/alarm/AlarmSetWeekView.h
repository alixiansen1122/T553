/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Alarm
 * Author:
 * Create: 2021-09-11
 */

#ifndef ALARM_SET_WEEK_VIEW
#define ALARM_SET_WEEK_VIEW

#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "font/ui_font.h"
#include "components/ui_scroll_view.h"
#include "layout/grid_layout.h"
#include "components/root_view.h"
#include "components/ui_time_picker.h"
#include "alarm/AlarmPresenter.h"
#include "alarm/AlarmClockModel.h"

namespace OHOS {
class AlarmSetWeekView {
public:
    AlarmSetWeekView();
    ~AlarmSetWeekView();
    UIScrollView *InitSetWeekView();
    static AlarmSetWeekView *GetInstance(void);
    void RefreshSetWeek1(void);
    void RefreshSetWeek2(uint8_t week);
private:
    UIScrollView *group{nullptr};
    UILabelButton *weekImage{nullptr};
    UILabel *labelRespeat{nullptr};
    UILabelButton *buttonWeek[7]{nullptr}; // 星期一共有7天，因此设置了七个星期的按钮
    void CreateWeekButton1(void);
    void CreateWeekButton2(void);
    void SetLabelButton(void);
    UILabelButton *CreateUILabelButton(const char *buttonText, int16_t x, int16_t y);
    UIView *CreateUIView(int16_t x, int16_t y);
};
} // namespace OHOS
#endif
