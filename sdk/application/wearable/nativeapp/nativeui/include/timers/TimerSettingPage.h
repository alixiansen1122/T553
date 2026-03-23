/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: TimerSettingPage
 * Create: 2025-06-09
 */

#ifndef TIMER_SETTING_PAGE_H
#define TIMER_SETTING_PAGE_H

#include "View.h"
#include "components/ui_button.h"
#include "components/ui_label.h"
#include "components/ui_time_picker.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "UiConfig.h"
#include "TimerPresenter.h"
#include "TimerModel.h"
#include "SlicePage.h"
#include "ChangeSliceListener.h"
#include "main/LoadImg.h"
#include "ui_resource_timer.h"

namespace OHOS {
static constexpr uint8_t PICKER_COUNT = 3;
    class TimerSettingPage : public UIPicker::SelectedListener,
                             public SlicePage<TimerPresenter>,
                             public UIView::OnClickListener,
                             public UIView::OnDragListener {
    public:
        TimerSettingPage();
        ~TimerSettingPage() override;

        void OnStart(void *data) override;
        bool OnClick(UIView &view, const ClickEvent &event) override;
        bool OnDrag(UIView &view, const DragEvent &event) override;

        void CreateLabel(void);
        void CreateStartButton(void);
        void CreatePicker();
        UIScrollView *InitTimerSetView();
        void OnPickerStoped(UIPicker& picker) override;

    private:
        UIScrollView *group_{nullptr};
        UILabel *labelTitle_{nullptr};
        UIImageView *startImg_{nullptr};
        UILabel *labelUnit_{nullptr};
        UIPicker *timerPicker_[PICKER_COUNT]{nullptr};
    };
}
#endif // TIMER_SETTING_PAGE_H