/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SleepTargetTime
 * Created: 2025-06-05
 */

#ifndef SLEEP_TARGET_TIME_H
#define SLEEP_TARGET_TIME_H

#include "View.h"
#include "wearable_log.h"
#include "UiConfig.h"
#include "components/ui_label.h"
#include "components/ui_button.h"
#include "components/ui_label_button.h"
#include "components/ui_image_animator.h"
#include "components/ui_circle_progress.h"
#include "components/ui_view_group.h"
#include "components/ui_picker.h"
#include "components/ui_scroll_view.h"
#include "main/LoadImg.h"
#include "sleep/SleepPresenter.h"
#include "sleep/SleepView.h"
namespace OHOS {
static constexpr int16_t PICKER_COUNT = 2;

class SleepTargetTime : public SlicePage<SleepPresenter>,
                        public UIPicker::SelectedListener,
                        public UIView::OnClickListener,
                        public UIView::OnDragListener {
public:
    SleepTargetTime();
    ~SleepTargetTime() override;

    void OnStart(void* data) override;
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDrag(UIView &view, const DragEvent &event) override;
    void OnPickerStoped(UIPicker& picker) override;
    void InitStartButton();
    void CreatePicker();
    void CreateLabel();
    void SetPickerTime();
private:
    UIScrollView *group_{nullptr};
    UILabel* targetTitleLabel_{nullptr};
    UILabel* timerUnitLabel_{nullptr};
    UILabelButton *startButton_{nullptr};
    UIPicker* timerPicker_[PICKER_COUNT]{nullptr};
};
}
#endif // TARGET_SLEEP_TIME_H