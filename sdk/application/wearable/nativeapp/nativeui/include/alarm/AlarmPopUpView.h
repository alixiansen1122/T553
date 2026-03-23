/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Alarm
 * Author:
 * Create: 2021-09-11
 */

#ifndef ALARM_POP_UP_VIEW_H
#define ALARM_POP_UP_VIEW_H

#include "components/ui_label.h"
#include "components/root_view.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_button.h"
#include "components/ui_label_button.h"
#include "ChangeSliceListener.h"
#include "components/ui_digital_clock.h"
#include "alarm/AlarmSetTimeView.h"
#include "alarm/AlarmCloseRingView.h"
#include "alarm/AlarmRingView.h"
#include "alarm/AlarmClockModel.h"
#include "UiConfig.h"

namespace OHOS {
class AlarmPopUpPresenter;
class AlarmPopUpView : public View<AlarmPopUpPresenter> {
public:
    AlarmPopUpView();
    ~AlarmPopUpView() override;
    static AlarmPopUpView *GetInstance(void);
    void OnStart() override;
    void ChangeView(int16_t hideView, int16_t showView);
    void AlarmCreateLabel(void);
    void AlarmCreateImage(void);
    UIView *GetPopUpView(int16_t viewIndex);
private:
    UIViewGroup *container[3]{nullptr}; // 3代表容器内存有弹窗界面以及两个跳转界面，用于跳转后显示或隐藏界面
    AlarmCloseRingView *closeRingView{nullptr};
    AlarmRingView *ringView{nullptr};
    UIViewGroup *group{nullptr};
    UILabel *labelPopup{nullptr};
    UIDigitalClock *dClock{nullptr};
    UIImageView *imageDelete{nullptr};
    UIImageView *imageClock{nullptr};
};
} // namespace OHOS
#endif