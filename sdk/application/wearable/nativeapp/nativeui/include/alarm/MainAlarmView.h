/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Alarm
 * Author:
 * Create: 2021-09-11
 */

#ifndef MAIN_ALARM_VIEW_H
#define MAIN_ALARM_VIEW_H

#include <string>
#include "View.h"
#include "components/ui_label.h"
#include "components/root_view.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_button.h"
#include "components/ui_label_button.h"
#include "ChangeSliceListener.h"
#include "components/ui_digital_clock.h"
#include "components/ui_toggle_button.h"
#include "alarm/AlarmPresenter.h"
#include "alarm/AlarmSetTimeView.h"
#include "alarm/AlarmSetWeekView.h"
#include "alarm/AlarmJudgeView.h"
#include "alarm/AlarmConfigView.h"
#include "alarm/AlarmNewTimeView.h"
#include "alarm/AlarmMaxClock.h"
#include "alarm/AlarmClockModel.h"

namespace OHOS {
constexpr int16_t D_CLOCK_Y = 210;
constexpr int16_t LABEL_ZERO_Y = 270;
constexpr int16_t ALARM_INTER_ADD = 150;

class AlarmPresenter;
class MainAlarmView : public View<AlarmPresenter> {
public:
    MainAlarmView();
    ~MainAlarmView() override;
    static MainAlarmView *GetInstance(void);
    void OnStart() override;
    void ChangeView(int16_t hideView, int16_t showView);
    void DropView(UIView& view);
    void CreateImage();
    void RefreshAlarm(void);
    bool ChangeMainAlarm(int16_t thisView);
    bool ChangeConfig(int16_t thisView);
    bool ChangeSetTime(int16_t thisView);
    bool ChangeSetWeek(int16_t thisView);
    bool ChangeJudge(int16_t thisView);
    bool ChangeNewTime(int16_t thisView);
    bool ChangeMaxClock(int16_t thisView);
    bool ClickFunc(int16_t interf);
    bool GetRefreshStatus(void);
private:
    void LabelGetWeek(uint8_t week, int16_t alarmNum);
    void RefreshAlarmStatus(uint8_t alarmEnable, int16_t num);
    void NewClockCtl(int16_t alarmNum);
    AlarmConfigView *configView{nullptr};
    AlarmSetTimeView *setTimeView{nullptr};
    AlarmSetWeekView *setWeekView{nullptr};
    AlarmJudgeView *judgeView{nullptr};
    AlarmNewTimeView *newTimeView{nullptr};
    AlarmMaxClock *maxClockView{nullptr};
    UIScrollView *group{nullptr};
    UIToggleButton *statusButton[6]{nullptr}; // 6代表界面中最多存在六个按键
    UIImageView *addButton{nullptr};
    UIDigitalClock *dClock[6]{nullptr}; // 6代表界面中最多存在六个闹钟
    UILabel *dLabel[6]{nullptr}; // 6代表界面中显示闹钟日期的控件个数
    UIScrollView *container[7]{nullptr}; // 7代表容器存有主界面以及跳转的四个副界面，用于跳转后显示或隐藏界面
    uint8_t alarmHour, alarmMin;
    int8_t dClockNum{-1};
    int16_t weekStatus{-1};
    int16_t alarmPositionY{D_CLOCK_Y};
    int16_t labelPositionY{LABEL_ZERO_Y};
    int16_t alarmInterval{ALARM_INTER_ADD};
    bool reflashStatus{false};
};
} // namespace OHOS
#endif // MAIN_ALARM_VIEW_Ha