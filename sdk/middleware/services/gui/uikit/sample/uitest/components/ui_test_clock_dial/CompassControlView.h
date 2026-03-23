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

#ifndef COMPASS_CONTROL_VIEW_H
#define COMPASS_CONTROL_VIEW_H

#include "components/root_view.h"
#include "UiConfig.h"
#include "AppGroupView.h"
#include "CompassView.h"

namespace OHOS {
class CompassControlView : public AppGroupView {
public:
    CompassControlView() {}
    ~CompassControlView() override;
    bool InitView() override;
    CompassControlView(const CompassControlView &) = delete;
    CompassControlView &operator=(const CompassControlView &) = delete;

private:
    class ScrollCompassCallback : public AnimatorCallback {
    public:
        ~ScrollCompassCallback() override {}
        void Callback(UIView* view) override;

    private:
        int16_t lastRotateAngle { 0 };
    };
    void Clear(void);

    ScrollCompassCallback callback_;
    Animator* animator_ = nullptr;
    CompassView *compassView { nullptr };
};
}
#endif
