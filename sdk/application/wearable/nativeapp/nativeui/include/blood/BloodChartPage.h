/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BloodChartPage
 * Create: 2025-06
 */

#ifndef BLOOD_CHART_PAGE_H
#define BLOOD_CHART_PAGE_H

#include <sys/time.h>
#include "components/ui_scroll_view.h"
#include "components/ui_image_view.h"
#include "components/ui_swipe_view.h"
#include "components/ui_label.h"
#include "components/ui_chart.h"
#include "components/ui_label_button.h"
#include "components/ui_image_animator.h"
#include "SlicePage.h"
#include "UiConfig.h"
#include "BloodView.h"
#include "BloodPresenter.h"

namespace OHOS {
static constexpr uint8_t HEART_AVE_NUM = 360;
static constexpr uint8_t BLOOD_ANIMATOR_IMAGE_NUM = 30;
static constexpr uint8_t BLOOD_ANIMATOR_IMAGE_UPDATE = 40;
static constexpr uint8_t INDEX_1 = 1;
static constexpr uint8_t INDEX_2 = 2;
static constexpr uint8_t INDEX_3 = 3;
static constexpr uint8_t DATA_SERIAL_COUNT = 4;
class BloodPresenter;
class BloodChartPage : public SlicePage<BloodPresenter>,
                            public UIView::OnClickListener,
                            public UIView::OnDragListener {
public:
    BloodChartPage();
    ~BloodChartPage() override;
    void OnStart(void *data) override;
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDrag(UIView &view, const DragEvent &event) override;
    void InitImageAnimator();
    
private:
    UIViewGroup *group_ = nullptr;
    UILabel *labelTitle_ = nullptr;
    UILabel *labelTestTime_ = nullptr;
    UILabel *labelXAxisPointOne_ = nullptr;
    UILabel *labelXAxisPointTwo_ = nullptr;
    UILabel *labelXAxisPointThree_ = nullptr;
    UILabelButton *labelButtonRetest_ = nullptr;
    UIChartDataSerial *dataSerial_{nullptr};
    UIChartPillar *chartPillar_ = nullptr;
    UIImageView *mainImg_ = nullptr;
    UIImageView *chartImg_ = nullptr;
    UIImageAnimatorView *imageAnimator_ = nullptr;
    ImageAnimatorInfo imageAnimatorImageInfo_[BLOOD_ANIMATOR_IMAGE_NUM];
};
}
#endif // BLOOD_CHART_PAGE_H