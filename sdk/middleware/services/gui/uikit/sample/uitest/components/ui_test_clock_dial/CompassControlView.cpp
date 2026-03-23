/*
 * Copyright (c) 2022 CompanyNameMagicTag.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "CompassControlView.h"
#include <cstdlib>

namespace OHOS {
static constexpr int16_t CLOCKWISE_MAX_DEGREE = 360;
void CompassControlView::ScrollCompassCallback::Callback(UIView* view)
{
    CompassView* compass = reinterpret_cast<CompassView*>(view);

    compass->CompassStartRotate(lastRotateAngle);
    compass->SetAngleText(lastRotateAngle);
    lastRotateAngle++;
    if (lastRotateAngle >= CLOCKWISE_MAX_DEGREE) {
        lastRotateAngle = 0;
    }
}

CompassControlView::~CompassControlView()
{
    GRAPHIC_LOGD("CompassControlView::~CompassControlView()\n");
    if (animator_ != nullptr) {
        animator_->Stop();
        delete animator_;
        animator_ = nullptr;
    }
    RemoveAll();
    Clear();
}

void CompassControlView::Clear(void)
{
    if (compassView != nullptr) {
        delete compassView;
        compassView = nullptr;
    }
}

bool CompassControlView::InitView()
{
    compassView = new CompassView();
    if (compassView == nullptr) {
        GRAPHIC_LOGE("CompassView::new compassView fail\n");
        return false;
    }
    if (!compassView->InitPage()) {
        Clear();
        return false;
    }

    if (animator_ == nullptr) {
        animator_ = new Animator(&callback_, compassView, DEFAULT_TASK_PERIOD, true);
    }
    animator_->Start();

    Add(compassView);
    initViewStatus = true;
    return true;
}
}
