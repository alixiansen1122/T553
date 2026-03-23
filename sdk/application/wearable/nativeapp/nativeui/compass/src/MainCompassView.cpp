/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: MainCompassView.cpp
 * Author:
 * Create: 2021-12-23
 */

#include "compass/MainCompassView.h"
#include "compass/MainCompassPresenter.h"
#include "compass/CompassModel.h"
#include "wearable_log.h"

namespace OHOS {
static constexpr int16_t TWO_HOUR_SECOND = 7200;

void MainCompassView::OnStart()
{
    compassFragment_ = new CompassFragment();
    if (compassFragment_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainCompassView:: OnStart compassFragment_ null");
        return;
    }
    compassFragment_->CreateView();
    compassFragment_->GetFragmentView()->SetTouchable(true);
    compassFragment_->GetFragmentView()->SetDraggable(true);
    compassFragment_->GetFragmentView()->SetOnDragListener(presenter_);
    AddViewToRootContainer(compassFragment_->GetFragmentView());
    CompassModel::GetInstance()->OpenCompassSensor();
}

void MainCompassView::OnStop()
{
    if (compassFragment_ != nullptr) {
        compassFragment_->DestroyView();
        delete compassFragment_;
        compassFragment_ = nullptr;
    }
    CompassModel::GetInstance()->CloseCompassSensor();
    return;
}
}
