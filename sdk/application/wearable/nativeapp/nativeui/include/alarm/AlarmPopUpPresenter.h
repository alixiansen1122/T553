/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Alarm
 * Author:
 * Create: 2021-09-11
 */

#ifndef ALARM_POP_UP_PRESENTER_H
#define ALARM_POP_UP_PRESENTER_H

#include <string>
#include "View.h"
#include "Presenter.h"
#include "ChangeSliceListener.h"
#include "UiConfig.h"
#include "alarm/AlarmPopUpView.h"
#include "components/ui_label.h"
#include "common/screen.h"
#include "components/ui_label.h"
#include "components/root_view.h"
#include "components/ui_view_group.h"
#include "components/ui_checkbox.h"
#include "components/ui_image_view.h"
#include "gfx_utils/graphic_types.h"
#include "components/ui_button.h"
#include "components/ui_label_button.h"
#include "components/ui_digital_clock.h"
#include "gfx_utils/graphic_log.h"
#include "alarm/AlarmCloseRingView.h"
#include "alarm/AlarmRingView.h"
#include "alarm/AlarmPopUpView.h"
#include "alarm/AlarmClockModel.h"
#include "wearable_log.h"
#include "common/task.h"

namespace OHOS {
#define SET_TIME "settime"
#define SET_WEEK "setweek"
#define ALARM_DELETE "alarmdelete"
#define ALARM_CLOCK "imageclock"
#define ALARM_NEXT "alarmnext"
#define ALARM_OK "alarmok"

typedef enum {
    CHANGE_POP_UP_INTERFACE,
    CHANGE_INTERFACE_CLOSE_RING,
    CHANGE_INTERFACE_DELAY_RING,
} InterfacesPopUpId;

class AlarmPopUpView;
class AlarmPopUpPresenter : public Presenter<AlarmPopUpView>, public UIView::OnClickListener,
    public UIView::OnDragListener, public Task {
public:
    AlarmPopUpPresenter();
    ~AlarmPopUpPresenter() override;
    static AlarmPopUpPresenter *GetInstance();
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;
    void Callback() override;
    void CancelAlarmRing(void);
    void DelayAlarmRing(void);
    void OnPause() override;
    void OnResume() override;
private:
    bool isStartCalcu{true};
    uint16_t calcuNotifyNums{0};
};
}
#endif