/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: HeartRateReadyPage
 * Create: 2025-06
 */

#ifndef HEART_RATE_READY_PAGE_H
#define HEART_RATE_READY_PAGE_H

#include <sys/time.h>
#include "components/root_view.h"
#include "components/ui_view_group.h"
#include "components/ui_scroll_view.h"
#include "components/ui_digital_clock.h"
#include "components/ui_label.h"
#include "components/ui_list.h"
#include "components/ui_circle_progress.h"
#include "components/ui_image_animator.h"
#include "components/ui_label_button.h"
#include "layout/grid_layout.h"
#include "animator/animator.h"
#include "ChangeSliceListener.h"
#include "View.h"
#include "ui_test.h"
#include "SlicePageFactory.h"
#include "SlicePage.h"
#include "UiConfig.h"
#include "HeartRateModel.h"
#include "HeartRatePresenter.h"
#include "HeartRateView.h"

namespace OHOS {
class HeartRatePresenter;
class HeartRateReadyPage : public SlicePage<HeartRatePresenter>,
                        public UIView::OnClickListener,
                        public UIView::OnDragListener {
public:
    HeartRateReadyPage();
    ~HeartRateReadyPage() override;
    void OnStart(void *data) override;
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDrag(UIView &view, const DragEvent &event) override;
    void InitAnimator();
    void SetLabelProgressTime(int value);
    void StopAnimator();
    int InitCircleProgress();
    void InitImageAnimator();

private:
    UILabel *labelTips_ = nullptr;
    UIViewGroup *group_ = nullptr;
    UILabelButton *button_ = nullptr;
    UICircleProgress *timeProgress_ = nullptr;
    UILabel *labelProgressTime_ = nullptr;
    struct timeval startTime_;
    class HeartAnimatorCallback : public AnimatorCallback {
    public:
        explicit HeartAnimatorCallback(HeartRateReadyPage *page) : page_(page) {}
        ~HeartAnimatorCallback() override {}
        void Callback(UIView *view) override;

    private:
        HeartRateReadyPage *page_{nullptr};
    };
    Animator *animator_ = nullptr;
    HeartAnimatorCallback *callBack_ = nullptr;
};
}
#endif // HEART_RATE_READY_PAGE_H
