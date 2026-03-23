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

#include "ui_test_icosahedron.h"
#include "common/screen.h"
#include "common/image_cache_manager.h"
#include "components/ui_image_view.h"

namespace OHOS {
static constexpr uint8_t PLANES_NUM = 20;
void EntryAnimatorCallback::Callback(UIView *view)
{
    UIIcosahedronView* footView = dynamic_cast<UIIcosahedronView*>(view);
    int16_t el = EasingEquation::LinearEaseNone(startPos_, endPos_, animator_->GetRunTime(), animator_->GetTime());
    footView->SetSideLength(el);
    footView->RefreshIcosahedron();
    const Vector3<float> rotateStart(Screen::GetInstance().GetWidth() / 2.0, // 2.0: divisor
        Screen::GetInstance().GetHeight() / 2.0, 0); // 2.0: divisor
    const Vector3<float> rotateEnd(0, 0, 0);
    // 270 : stop angle
    int16_t angle = EasingEquation::LinearEaseNone(0, 270, animator_->GetRunTime(), animator_->GetTime());
    footView->RotatePlanes(angle, rotateStart, rotateEnd);
}

bool UITestIcosahedron::OnClick(UIView& view, const ClickEvent& event)
{
    if (view.GetViewType() == UI_IMAGE_VIEW) {
        UIImageView* img = static_cast<UIImageView*>(&view);
        img->SetSrc(imgInverseInfo_);
    }
    return true;
}

void UITestIcosahedron::SetUp()
{
    if (imgInfo_ == nullptr) {
        imgInfo_ = ImageCacheManager::GetInstance().LoadSingleRes(RES_PATH"regular_hexagon.bin");
        if (imgInfo_ == nullptr) {
            return;
        }
    }
    if (imgInverseInfo_ == nullptr) {
        imgInverseInfo_ = ImageCacheManager::GetInstance().LoadSingleRes(RES_PATH"regular_hexagon_inverse.bin");
        if (imgInverseInfo_ == nullptr) {
            return;
        }
    }

    if (container_ == nullptr) {
        container_ = new UIIcosahedronView();
        container_->SetThrowDrag(true);
        container_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        container_->SetDefaultImage(imgInverseInfo_);
        container_->SetLuminanceFactor(2); // 2: Twice as bright
        for (uint8_t i = 0; i < PLANES_NUM - 5; i++) { // 5: Reduce the quantity and test the default image.
            UIImageView* view = new UIImageView();
            view->SetTouchable(true);
            view->SetOnClickListener(this);
            view->SetSrc(imgInfo_);
            container_->Add(view);
        }
        if (callback_ == nullptr) {
            callback_ = new EntryAnimatorCallback(container_, 30, 200); // 30: Initial Size 200: Final display size
        }
        enterAnimator_ = callback_->GetAnimator();
    }
}

void UITestIcosahedron::TearDown()
{
    if (container_ != nullptr) {
        DeleteChildren(container_);
        container_ = nullptr;
    }
    if (imgInfo_ != nullptr) {
        ImageCacheManager::GetInstance().UnloadSingleRes(RES_PATH"regular_hexagon.bin");
        imgInfo_ = nullptr;
    }
    if (imgInverseInfo_ != nullptr) {
        ImageCacheManager::GetInstance().UnloadSingleRes(RES_PATH"regular_hexagon_inverse.bin");
        imgInverseInfo_ = nullptr;
    }
    if (callback_ != nullptr) {
        delete callback_;
        callback_ = nullptr;
    }
}

const UIView* UITestIcosahedron::GetTestView()
{
    if (enterAnimator_ != nullptr) {
        enterAnimator_->Start();
    }
    return container_;
}
}