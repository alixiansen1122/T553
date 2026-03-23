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

#include "ui_test_sphere_view.h"
#include "components/ui_transform_group.h"
#include "components/ui_image_view.h"
#include "common/image_cache_manager.h"
#include "common/screen.h"

namespace OHOS {
static constexpr uint8_t STEP_SIZE = 10;
UITestSphereView::UITestSphereView()
{
}

const UIView* UITestSphereView::GetTestView()
{
    return container_;
}

void UITestSphereView::SetUp()
{
    container_ = new UIViewGroup();
    container_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    container_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    container_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);

    sphereView_ = new UISphereView(8); // 8: Setting the Number of Displayed Lines.
    sphereView_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    sphereView_->SetThrowDrag(true);
    if (listener_ == nullptr) {
        listener_ = new UITestSphereView::StateListener();
        sphereView_->SetChildStateListener(listener_);
    }
    sphereView_->RequestFocus();
    container_->Add(sphereView_);
    for (uint8_t i = 0; i < 24; i++) { // 24: item number.
        UITransformGroup* group = new UITransformGroup();
        group->SetPosition(0, 0, 60, 60); // 60: view size
        group->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

        UIImageView* image = new UIImageView();
        image->SetPosition(0, 0, 60, 60); // 60: view size
        image->SetAutoEnable(false);
        image->SetResizeMode(UIImageView::ImageResizeMode::FILL);
        ImageInfo* info = ImageCacheManager::GetInstance().LoadSingleRes(RES_PATH"/kaleidoscope.bin");
        image->SetSrc(info);
        group->Add(image);
        sphereView_->Add(group);
    }

    SetUpLabelButton();
}

void UITestSphereView::TearDown()
{
    sphereView_->ClearFocus();
    DeleteChildren(container_);
    container_ = nullptr;
    sphereView_ = nullptr;
    controlGroup_ = nullptr;
    controlBt_ = nullptr;
    sizeIncreaseBt_ = nullptr;
    sizeDiminishedBt_ = nullptr;
    addItemBt_ = nullptr;
    updateCameraDistance = nullptr;
    removeItemBt_ = nullptr;
    if (listener_ != nullptr) {
        delete listener_;
        listener_ = nullptr;
    }
}

void UITestSphereView::SetUpLabelButton()
{
    controlGroup_ = new UIViewGroup();
    controlGroup_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    controlGroup_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    controlGroup_->SetVisible(false);
    container_->Add(controlGroup_);

    controlBt_ = new UILabelButton();
    controlBt_->SetPosition(200, 400, 60, 30); // 200, 400, 60, 30: label button size
    controlBt_->SetOnClickListener(this);
    controlBt_->SetText("control");
    container_->Add(controlBt_);

    sizeIncreaseBt_ = new UILabelButton();
    sizeIncreaseBt_->SetPosition(50, 100, 60, 30); // 50, 100, 60, 30: label button size
    sizeIncreaseBt_->SetOnClickListener(this);
    sizeIncreaseBt_->SetText("size +");
    controlGroup_->Add(sizeIncreaseBt_);

    sizeDiminishedBt_ = new UILabelButton();
    sizeDiminishedBt_->SetPosition(50, 200, 60, 30); // 50, 200, 60, 30: label button size
    sizeDiminishedBt_->SetOnClickListener(this);
    sizeDiminishedBt_->SetText("size -");
    controlGroup_->Add(sizeDiminishedBt_);

    addItemBt_ = new UILabelButton();
    addItemBt_->SetPosition(140, 100, 60, 30); // 140, 100, 60, 30: label button size
    addItemBt_->SetOnClickListener(this);
    addItemBt_->SetText("item +");
    controlGroup_->Add(addItemBt_);

    removeItemBt_ = new UILabelButton();
    removeItemBt_->SetPosition(140, 200, 60, 30); // 140, 200, 60, 30: label button size
    removeItemBt_->SetOnClickListener(this);
    removeItemBt_->SetText("item -");
    controlGroup_->Add(removeItemBt_);

    updateCameraDistance = new UILabelButton();
    updateCameraDistance->SetPosition(230, 100, 60, 30); // 230, 100, 60, 30: label button size
    updateCameraDistance->SetOnClickListener(this);
    updateCameraDistance->SetText("update distance");
    controlGroup_->Add(updateCameraDistance);
}

bool UITestSphereView::OnClick(UIView& view, const ClickEvent& event)
{
    int16_t x = sphereView_->GetX();
    int16_t y = sphereView_->GetY();
    uint16_t width = sphereView_->GetWidth();
    uint16_t height = sphereView_->GetHeight();
    if (&view == controlBt_) {
        bool visible = controlGroup_->IsVisible();
        controlGroup_->SetVisible(!visible);
    } else if (&view == sizeIncreaseBt_) {
        sphereView_->SetPosition(x + (STEP_SIZE >> 1), y + (STEP_SIZE >> 1), width - STEP_SIZE, height - STEP_SIZE);
    } else if (&view == sizeDiminishedBt_) {
        sphereView_->SetPosition(x - (STEP_SIZE >> 1), y - (STEP_SIZE >> 1), width + STEP_SIZE, height + STEP_SIZE);
    } else if (&view == addItemBt_) {
        UITransformGroup* group = new UITransformGroup();
        group->SetPosition(0, 0, 60, 60); // 60: view size
        group->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

        UIImageView* image = new UIImageView();
        image->SetPosition(0, 0, 60, 60); // 60: view size
        image->SetAutoEnable(false);
        image->SetResizeMode(UIImageView::ImageResizeMode::FILL);
        ImageInfo* info = ImageCacheManager::GetInstance().LoadSingleRes(RES_PATH"/kaleidoscope.bin");
        image->SetSrc(info);
        group->Add(image);
        sphereView_->Add(group);
    } else if (&view == removeItemBt_) {
        UIView* child = sphereView_->GetChildrenTail();
        if (child != nullptr) {
            sphereView_->Remove(child);
        }
    } else if (&view == updateCameraDistance) {
        sphereView_->SetSphereCameraDistance(-1000); // -1000 cramer distance
    }
    return true;
}
}