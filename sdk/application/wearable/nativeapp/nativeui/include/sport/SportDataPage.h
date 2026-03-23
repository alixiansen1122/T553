/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportDataPage
 * Create: 2025-06-06
 */

#ifndef SPORT_DATA_PAGE_H
#define SPORT_DATA_PAGE_H

#include "components/ui_label.h"
#include "components/ui_canvas.h"
#include "components/ui_image_view.h"
#include "components/ui_sweep_clock.h"
#include "animator/animator.h"
#include "SlicePage.h"
#include "SportPresenter.h"
#include "SportModel.h"

namespace OHOS {
class SportDataPage : public SlicePage<SportPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    SportDataPage() {}
    ~SportDataPage();
    void OnStart(void* data) override;
    void OnPause() override;
    void OnResume() override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void InitBatteryView();
    void InitTimeLableVIew();
    void ShowUpHeartRate();
    void ShowUpTopView();
    void ShowUpLeftView();
    void ShowUpRightView();
    void ShowUpBottomView();
    void ShowUpSeparateLine();
    void UpdateDataViewByType();
    void CheckTarget();
    void CheckRemind();
    class SportDataAnimatorCallback : public AnimatorCallback {
    public:
        SportDataAnimatorCallback(SportDataPage* page):page_(page) {}
        virtual ~SportDataAnimatorCallback() override {}
        void Callback(UIView* view) override;
    private:
        SportDataPage* page_{nullptr};
    };
    Animator* animator_{nullptr};
    SportDataAnimatorCallback* callback_{nullptr};
    Image *secondHand_{nullptr};
    UISweepClock *sweepClock_{nullptr};
    UIImageView *heartRateScale_{nullptr};
    UIImageView *heartRatePointer_{nullptr};
    UICanvas *separateLine_{nullptr};
    UILabel *topValueLabel_{nullptr};
    UILabel *topUnitLabel_{nullptr};
    UILabel *leftValueLabel_{nullptr};
    UILabel *leftUnitLabel_{nullptr};
    UILabel *rightValueLabel_{nullptr};
    UILabel *rightUnitLabel_{nullptr};
    UILabel *bottomValueLabel_{nullptr};
    UILabel *bottomUnitLabel_{nullptr};
    UILabel *timeLabel_{nullptr};
    UIImageView *batteryIcon_{nullptr};
    UILabel *batteryLabel_{nullptr};
};
}  // namespace OHOS
#endif  // SPORT_DATA_PAGE_H