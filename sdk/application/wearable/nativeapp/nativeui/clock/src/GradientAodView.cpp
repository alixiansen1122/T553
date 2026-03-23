/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: GradientAodView
 * Create: 2025-04
 */

#include "clock/GradientAodView.h"
#include "wearable_log.h"
#include "UIWatchDialFactory.h"

namespace OHOS {

REGIST_WATCH_DIAL(DIAL_DISPLAY_SUPPORT_AOD, GRADIENT_CLOCK, GradientAodView, nullptr);

static GradientAodView* g_pGradientAodView = nullptr;

GradientAodView::GradientAodView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "GradientAodView::GradientAodView");
    g_pGradientAodView = this;
}

GradientAodView::~GradientAodView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "GradientAodView::~GradientAodView");
}

GradientAodView *GradientAodView::GetInstance(void)
{
    return g_pGradientAodView;
}

void GradientAodView::Update(void)
{
    if (gradientDateView_ != nullptr) {
        gradientDateView_->UpdateTime();
    }
}

void GradientAodView::OnCreateView(void* data)
{
    fragmentView_.SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "GradientAodView::OnCreateView");
    gradientDateView_ = new GradientDateView();
    if (!gradientDateView_) {
        GRAPHIC_LOGE("gradientDateView_ malloc memory Failed\n");
        return;
    }
    fragmentView_.Add(gradientDateView_);
    gradientDateView_->InitView();
    Update();
}

void GradientAodView::OnDestroyView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "GradientAodView::OnDestroyView");
    if (gradientDateView_) {
        gradientDateView_->RemoveAll();
        fragmentView_.Remove(gradientDateView_);
        delete gradientDateView_;
        gradientDateView_ = nullptr;
    }
}
}