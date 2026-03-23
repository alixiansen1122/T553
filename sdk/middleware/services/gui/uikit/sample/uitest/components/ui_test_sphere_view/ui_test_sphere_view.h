/*
 * Copyright (c) 2024 CompanyNameMagicTag.
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
#ifndef UI_TEST_SPHERE_VIEW_H
#define UI_TEST_SPHERE_VIEW_H

#include "components/ui_sphere_view.h"
#include "components/ui_label_button.h"
#include "ui_test.h"

namespace OHOS {
class UITestSphereView : public UITest, public UIView::OnClickListener {
public:
    UITestSphereView();
    ~UITestSphereView() override {}
    void SetUp() override;
    void TearDown() override;;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    class StateListener : public UISphereView::ChildStateListener {
    public:
        bool StateChange(UIView& view, float rowAngle, float colAngle) override
        {
            if (FloatMore(rowAngle, 90.0f) && FloatLess(rowAngle, 270.0f)) { // 90 270: angle range
                view.SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE >> 1);
            } else {
                view.SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
            }
            return true;
        }
    };

    void SetUpLabelButton();
    UIViewGroup* container_ = nullptr;
    UISphereView* sphereView_ = nullptr;

    UIViewGroup* controlGroup_ = nullptr;
    UILabelButton* controlBt_ = nullptr;
    UILabelButton* sizeIncreaseBt_ = nullptr;
    UILabelButton* sizeDiminishedBt_ = nullptr;
    UILabelButton* addItemBt_ = nullptr;
    UILabelButton* removeItemBt_ = nullptr;
    UILabelButton* updateCameraDistance = nullptr;
    StateListener* listener_ = nullptr;
};
}
#endif