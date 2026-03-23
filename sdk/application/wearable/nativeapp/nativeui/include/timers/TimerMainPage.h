/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: TimerMainPage
 * Create: 2025-06-09
 */

#ifndef TIMER_MAIN_PAGE_H
#define TIMER_MAIN_PAGE_H

#include <sys/time.h>
#include "View.h"
#include "components/root_view.h"
#include "components/ui_view_group.h"
#include "components/ui_scroll_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "UiConfig.h"
#include "ChangeSliceListener.h"
#include "components/ui_image_view.h"
#include "TimerPresenter.h"
#include "TimerModel.h"
#include "SlicePage.h"
#include "common/image_cache_manager.h"
#include "main/LoadImg.h"
#include "ui_resource_timer.h"
#include "timers/TimerCountDown.h"

namespace OHOS {
static constexpr uint8_t SELECT_IMAGE_COUNT = 4;

    class TimerMainPage : public SlicePage<TimerPresenter>,
                          public UIView::OnClickListener,
                          public UIView::OnDragListener {
    public:
        TimerMainPage();
        ~TimerMainPage() override;
        void OnResume() override;
        void OnPause() override;
        void OnStart(void *data) override;
        bool OnClick(UIView &view, const ClickEvent &event) override;
        bool OnDrag(UIView &view, const DragEvent &event) override;
        static TimerMainPage *GetInstance(void);
        void ShowMainPage();
        void LabelTitle();
        void CreateButton();
        void InitView();

    private:
        UIScrollView *group_{nullptr};
        UIImageView *selectMin_[SELECT_IMAGE_COUNT]{nullptr};
        UIImageView *addButton_{nullptr};
        UILabel *labelTitle_{nullptr};
        UIImageView *bgImg_{nullptr};
        TimerCountDown *countDownFragment_{nullptr};
    };
}
#endif // TIMER_MAIN_PAGE_H