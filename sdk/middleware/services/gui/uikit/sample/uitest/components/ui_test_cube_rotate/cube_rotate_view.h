/*
 * Copyright (c) 2023 CompanyNameMagicTag.
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

#ifndef CUBE_ROTATE_H
#define CUBE_ROTATE_H

#include "components/ui_label.h"
#include "components/ui_image_view.h"
#include "components/ui_scroll_view.h"
#include "components/ui_label_button.h"
#include "animator/animator_manager.h"
#include "gfx_utils/vector.h"
#include "common/screen.h"

namespace OHOS {
struct CubeInfo {
    Vector3<float> n; // normal of the plane
    Matrix4<float> matrix;
    UIImageView* img;
};

class CubeAnimatorCallback : public AnimatorCallback {
public:
    explicit CubeAnimatorCallback(UIView* view) {}
    ~CubeAnimatorCallback() override {}
    void Callback(UIView* view) override;
    int angle_ { 0 };
};

class CubeRotateView : public UIScrollView, public UIView::OnClickListener {
public:
    CubeRotateView();
    ~CubeRotateView() override;
    CubeRotateView(const CubeRotateView &) = delete;
    CubeRotateView &operator=(const CubeRotateView &) = delete;
    bool InitView();
    Animator* animator { nullptr };
    bool OnClick(UIView& view, const ClickEvent& event) override;
    bool OnDragStartEvent(const DragEvent& event) override;
    bool OnDragEvent(const DragEvent& event) override;
    bool OnDragEndEvent(const DragEvent& event) override;
    void RotatePlanes(int angle, const Vector3<float>& rotateStart, const Vector3<float>& rotateEnd);

private:
    Graphic::Vector<CubeInfo> cubePlane_;
    CubeAnimatorCallback* callBack_ { nullptr };
    Matrix4<float> perspectiveMatrix_;
    Matrix4<float> matrix_;
    Matrix4<float> lastPhaseMatrix_;
    Point startPoint_{0, 0};
    UILabelButton* controlButton_ { nullptr };
    ImageInfo* img_ { nullptr };
    void InitPlanes();
    void ClearAll();
};
} // namespace OHOS
#endif // CUBE_ROTATE_H
