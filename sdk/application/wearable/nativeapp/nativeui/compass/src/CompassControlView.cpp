/*
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: CompassControlView.cpp
 * Author:
 * Create: 2022-04-02
 */
#include "compass/CompassControlView.h"
#include "wearable_log.h"

namespace OHOS {
CompassControlView::CompassControlView()
{
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
}

CompassControlView::~CompassControlView()
{
    UnLoad();
}

void CompassControlView::PreLoad(void)
{
    if (compassFragment_ == nullptr) {
        compassFragment_ = new CompassFragment();
        if (compassFragment_ == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CompassView::new compassFragment_ fail");
            return;
        }
    }

    compassFragment_->CreateView();
    Add(compassFragment_->GetFragmentView());
}

void CompassControlView::UnLoad(void)
{
    if (compassFragment_ != nullptr) {
        compassFragment_->DestroyView();
        Remove(compassFragment_->GetFragmentView());
        delete compassFragment_;
        compassFragment_ = nullptr;
    }
}

void CompassControlView::OnPause(void)
{
    if (compassFragment_ != nullptr) {
        compassFragment_->PauseView();
    }
}

void CompassControlView::OnActive(void)
{
    if (compassFragment_ != nullptr) {
        compassFragment_->ResumeView();
    }
}

void CompassControlView::ScrollBegin(bool isActive)
{
    if (isActive && (compassFragment_ != nullptr)) {
        compassFragment_->PauseView();
    }
}

void CompassControlView::ScrollEnd(bool isActive)
{
    if (isActive && (compassFragment_ != nullptr)) {
        compassFragment_->ResumeView();
    }
}

void CompassControlView::Update(void)
{
    if (compassFragment_ != nullptr) {
        compassFragment_->Update();
    }
}
}
