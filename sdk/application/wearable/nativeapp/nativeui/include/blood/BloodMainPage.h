/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BloodMainPage
 * Create: 2025-06
 */

#ifndef BLOOD_MAIN_PAGE_H
#define BLOOD_MAIN_PAGE_H

#include "View.h"
#include "components/ui_view_group.h"
#include "components/ui_scroll_view.h"
#include "components/ui_digital_clock.h"
#include "components/ui_lite_surface_view.h"
#include "ChangeSliceListener.h"
#include "UiConfig.h"
#include "layout/grid_layout.h"
#include "ui_test.h"
#include "SlicePageFactory.h"
#include "SlicePage.h"
#include "video_play_wrapper.h"
#include "BloodModel.h"
#include "BloodPresenter.h"
#include "BloodView.h"

namespace OHOS {
class BloodPresenter;
class BloodModel;
class BloodMainPage : public SlicePage<BloodPresenter>,
                    public UIView::OnClickListener,
                    public UIView::OnDragListener {
public:
    BloodMainPage();
    ~BloodMainPage() override;
    static BloodMainPage *GetInstance();
    void OnStart(void *data) override;
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDrag(UIView &view, const DragEvent &event) override;
    void OnResume() override;
    void OnPause() override;
    void InitImageAnimator();
    bool IsToday(const struct timeval &tv);
    void SetTestTime();
private:
    UIViewGroup *group_ = nullptr;
    UILabelButton *buttonStart_ = nullptr;
    UILabel *labelTitle_ = nullptr;
    UILabel *labelPreTest_ = nullptr;
    UILiteSurfaceView *surfaceView_ =  nullptr;
    MediaVideoPlay *videoPlay_ = nullptr;
    ColorType colorKey_;
};
}
#endif // BLOOD_MAIN_PAGE_H
