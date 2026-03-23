/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: HeartRateMainPage
 * Create: 2025-06
 */

#ifndef HEART_RATE_MAIN_PAGE_H
#define HEART_RATE_MAIN_PAGE_H

#include <sys/time.h>
#include "components/root_view.h"
#include "components/ui_view_group.h"
#include "components/ui_scroll_view.h"
#include "components/ui_digital_clock.h"
#include "components/ui_label.h"
#include "components/ui_image_animator.h"
#include "components/ui_lite_surface_view.h"
#include "layout/grid_layout.h"
#include "ChangeSliceListener.h"
#include "UiConfig.h"
#include "ui_test.h"
#include "View.h"
#include "SlicePageFactory.h"
#include "SlicePage.h"
#include "video_play_wrapper.h"

namespace OHOS {
class HeartRatePresenter;
class HeartRateMainPage : public SlicePage<HeartRatePresenter>,
                            public UIView::OnClickListener,
                            public UIView::OnDragListener {
public:
    HeartRateMainPage();
    ~HeartRateMainPage() override;
    void OnStart(void *data) override;
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDrag(UIView &view, const DragEvent &event) override;
    void OnResume() override;
    void OnPause() override;
    bool IsToday(const struct timeval &tv);
    void SetTestTime();
    void InitImageAnimator();

private:
    UIViewGroup *group_ = nullptr;
    UILabelButton *button_ = nullptr;
    UILabel *labelTitle_ = nullptr;
    UILabel *labelPreTest_ = nullptr;
    UILiteSurfaceView *surfaceView_ =  nullptr;
    MediaVideoPlay *videoPlay_ = nullptr;
    ColorType colorKey_;
};
}
#endif // HEART_RATE_MAIN_PAGE_H