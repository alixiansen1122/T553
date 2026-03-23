/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportPreparePage
 * Create: 2025-06-06
 */

#ifndef SPORT_PREPARE_PAGE_H
#define SPORT_PREPARE_PAGE_H

#include "components/ui_label.h"
#include "components/ui_image_view.h"
#include "components/ui_label_button.h"
#include "components/ui_view_group.h"
#include "animator/animator.h"
#include "SlicePage.h"
#include "SportPresenter.h"
#include "SportModel.h"

namespace OHOS {
const char* SPORT_PREPARE_GO_BUTTON = "sportPrepareGoButton";

class SportPreparePage : public SlicePage<SportPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    SportPreparePage() {}
    ~SportPreparePage();
    void OnStart(void* data) override;
    void OnPause() override;
    void OnResume() override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void InitGpsSignalImage();
    void InitBackgroundView();
    void InitBatteryView();
    void InitHeartbeatView();
    void InitTimeLableVIew();
    void InitCenterView();
    void InitGPSView();
    void InitHeartRateView();
    void InitGoButtonView();
    class SportPrepareAnimatorCallback : public AnimatorCallback {
    public:
        SportPrepareAnimatorCallback(SportPreparePage* page):page_(page) {}
        virtual ~SportPrepareAnimatorCallback() override {}
        void Callback(UIView* view) override;
    private:
        SportPreparePage* page_{nullptr};
    };
    SportPrepareAnimatorCallback* callback_{nullptr};
    Animator* animator_{nullptr};
    UIViewGroup *group_{nullptr};
    UIImageView *background_{nullptr};
    UILabel *timeLabel_{nullptr};
    UIImageView *batteryIcon_{nullptr};
    UILabel *batteryLabel_{nullptr};
    UILabel *locatingLabel_{nullptr};
    UIImageView *gpsImageView_{nullptr};
    UIImageView *gpsSignalView_{nullptr};
    UIImageView *heartRateImageView_{nullptr};
    UILabel *heartRateLabel_{nullptr};
    UILabelButton *goButton_{nullptr};
    UIImageView *iconImageView_{nullptr};
    time_t startTime_{0};
    bool isOverTime_{false};
};
}  // namespace OHOS
#endif  // SPORT_PREPARE_PAGE_H