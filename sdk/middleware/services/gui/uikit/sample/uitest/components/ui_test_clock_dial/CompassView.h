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

#ifndef COMPASS_VIEW_H
#define COMPASS_VIEW_H

#include "components/root_view.h"
#include "components/ui_label.h"
#include "components/ui_image_view.h"
#include "UiConfig.h"
#include "AppGroupView.h"

namespace OHOS {
constexpr float VIEW_CENTERS = 227;
struct CompassItem {
    uint16_t beginAngle;
    uint16_t endAngle;
    const char *dir;
};

class CompassView : public AppGroupView {
public:
    CompassView() {}
    ~CompassView() override;
    CompassView(const CompassView &) = delete;
    CompassView &operator=(const CompassView &) = delete;

    void DirectionSetText(const char *text, const char *angle);
    void CompassStartRotate(int16_t numb);
    bool InitPage(void);
    bool InitView() override;
    bool GetInitViewState(void);
    bool InitCompassImage(void);
    void SetAngleText(uint16_t angle);

private:
    bool LoadCompassImage(UIImageView *view, uint32_t resId);
    void Clear(void);
    UILabel *directionLabel { nullptr };
    UILabel *angledLabel { nullptr };
    UIImageView *backGround { nullptr };
    UIImageView *rotateShaft { nullptr };
    UIImageView *triangulation { nullptr };
    const Vector2<float> VIEW_CENTER = { VIEW_CENTERS, VIEW_CENTERS };
};
}
#endif
