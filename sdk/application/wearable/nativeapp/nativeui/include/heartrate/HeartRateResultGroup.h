/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: HeartRateResultGroup
 * Create: 2025-06
 */

#ifndef HEARTRATE_RESULT_GROUP_H
#define HEARTRATE_RESULT_GROUP_H

#include <sys/time.h>
#include "components/ui_scroll_view.h"
#include "components/ui_image_view.h"
#include "components/ui_swipe_view.h"
#include "components/ui_label.h"
#include "components/ui_chart.h"
#include "components/ui_label_button.h"
#include "components/ui_image_animator.h"
#include "HeartRatePresenter.h"
#include "SlicePage.h"
#include "UiConfig.h"
#include "HeartRateView.h"

namespace OHOS {
static constexpr uint16_t HEARTRATE_ANIMATOR_IMAGE_NUM = 30;
static constexpr uint16_t HEARTRATE_ANIMATOR_IMAGE_UPDATE = 70;
class HeartRatePresenter;
class HeartRateResultGroup : public UIViewGroup {
public:
    HeartRateResultGroup();
    ~HeartRateResultGroup() override;
    void InitPage();
    void InitImageAnimator();
    UILabel *labelHeartRateValue_ = nullptr;
private:
    UIImageAnimatorView *imageAnimator_ = nullptr;
    UILabel *labelTitle_ = nullptr;
    UIChartPolyline *chartPolyline_ = nullptr;
    UILabel *labelLastMeasureTime_ = nullptr;
    UILabel *labelUnitText_ = nullptr;
    UIChartDataSerial *dataSerial_{nullptr};
    UIImageView *imageRedArrow_ = nullptr;
    UILabel *labelMaxValue_ = nullptr;
    UIImageView *imageBlackArrow_ = nullptr;
    UILabel *labelMinValue_ = nullptr;
    // X轴坐标三个点位
    UILabel *labelXAxisPointOne_ = nullptr;
    UILabel *labelXAxisPointTwo_ = nullptr;
    UILabel *labelXAxisPointThree_ = nullptr;
    // Y轴坐标四个点位
    UILabel *labelYAxisPointOne_ = nullptr;
    UILabel *labelYAxisPointTwo_ = nullptr;
    UILabel *labelYAxisPointThree_ = nullptr;
    UILabel *labelYAxisPointFour_ = nullptr;
    UIImageView *mainImg_ = nullptr;
    ImageAnimatorInfo imageAnimatorImageInfo_[HEARTRATE_ANIMATOR_IMAGE_NUM];
};
}
#endif // HEARTRATE_RESULT_GROUP_H
