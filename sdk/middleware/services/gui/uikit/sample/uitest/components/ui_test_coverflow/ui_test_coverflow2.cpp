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

#include "ui_test_coverflow2.h"
#include "common/screen.h"
#include "font/ui_font.h"
#include "gfx_utils/graphic_math.h"
#include "gfx_utils/rect.h"
#include "common/image_cache_manager.h"
#include "components/ui_image_view.h"
#include "components/ui_label_ext.h"
#include "components/ui_transform_group.h"

namespace OHOS {
namespace {
static constexpr uint8_t COVERFLOW_MAX_PAGE_NUM = 6;
const char* g_faceInfo[COVERFLOW_MAX_PAGE_NUM] = {
    RES_PATH"/rainbow.bin",
    RES_PATH"/kaleidoscope.bin",
    RES_PATH"/planet.bin",
    RES_PATH"/rainbow.bin",
    RES_PATH"/kaleidoscope.bin",
    RES_PATH"/planet.bin"
};

static const char* ADD_IMG_PATH = RES_PATH"/add.bin";
static const char* SUB_IMG_PATH = RES_PATH"/sub.bin";
static constexpr uint8_t PAGE_MARGIN = 20;
static constexpr uint16_t PAGE_WIDTH = 250;
static constexpr uint16_t PAGE_HEIGHT = 250;
static constexpr uint8_t DIVISOR_TWO = 2;
static constexpr int16_t LABEL_FONT_SIZE = 30;
static constexpr uint16_t IMAGE_WIDTH = 200;
static constexpr uint16_t IMAGE_HEIGHT = 200;
} // namespace

void UITestCoverflow2::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetThrowDrag(true);
        container_->SetHorizontalScrollState(false);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    }
}

void UITestCoverflow2::TearDown()
{
    if (coverflow_ != nullptr) {
        UITransformGroup* child = dynamic_cast<UITransformGroup*>(coverflow_->GetChildrenHead());
        while (child != nullptr) {
            UIView* node = child;
            child = dynamic_cast<UITransformGroup*>(child->GetNextSibling());
            coverflow_->Remove(node);
            delete node;
        }
        delete coverflow_;
        coverflow_ = nullptr;
    }
    lastIndex_ = 0;
    addImage_ = nullptr;
    if (label_ != nullptr) {
        delete label_;
        label_ = nullptr;
    }

    if (container_ != nullptr) {
        delete container_;
        container_ = nullptr;
    }
}

const UIView* UITestCoverflow2::GetTestView()
{
    UITestCoverflow2001();
    return container_;
}

void UITestCoverflow2::UITestCoverflow2001()
{
    if (container_ == nullptr) {
        return;
    }
    if (coverflow_ == nullptr) {
        coverflow_ = new UICoverFlowView2();
    }
    coverflow_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    coverflow_->SetThrowDrag(true);
    coverflow_->SetOnCoverflowListener(this);
    coverflow_->SetPagePadding(PAGE_MARGIN);
    container_->Add(coverflow_);
    for (uint8_t i = 0; i < COVERFLOW_MAX_PAGE_NUM - 1; i++) {
        UITransformGroup* page = new UITransformGroup();
        page->SetPosition(0, 100, PAGE_WIDTH, PAGE_HEIGHT); // 100: y position
        UIImageView* image = new UIImageView();
        image->Resize(IMAGE_WIDTH, IMAGE_HEIGHT);
        page->Add(image);
        ImageInfo* src = ImageCacheManager::GetInstance().LoadSingleRes(std::string(g_faceInfo[i]));
        image->SetSrc(src);
        image->LayoutCenterOfParent();

        UIImageView* imageSub = new UIImageView();
        imageSub->SetPosition(200, 0, 50, 50); // 200: x position 50: width and height
        page->Add(imageSub);
        src = ImageCacheManager::GetInstance().LoadSingleRes(std::string(SUB_IMG_PATH));
        imageSub->SetSrc(src);
        imageSub->SetTouchable(true);
        imageSub->SetOnClickListener(this);
        coverflow_->Add(page);
    }
    CreateAddButton();
    CreateTitleLabel();
}

void UITestCoverflow2::CreateAddButton()
{
    UITransformGroup* page = new UITransformGroup();
    page->SetPosition(0, 100, PAGE_WIDTH, PAGE_HEIGHT); // 100: y

    if (addImage_ == nullptr) {
        addImage_ = new UIImageView();
    }

    addImage_->Resize(IMAGE_WIDTH, IMAGE_HEIGHT);
    addImage_->SetTouchable(true);
    addImage_->SetAutoEnable(false);
    addImage_->SetResizeMode(UIImageView::ImageResizeMode::FILL);
    addImage_->SetOnClickListener(this);
    page->Add(addImage_);
    addImage_->LayoutCenterOfParent();

    ImageInfo* src = ImageCacheManager::GetInstance().LoadSingleRes(std::string(ADD_IMG_PATH));
    addImage_->SetSrc(src);
    coverflow_->Add(page);
}

void UITestCoverflow2::CreateTitleLabel()
{
    if (label_ == nullptr) {
        label_ = new UILabel();
    }
    label_->SetPosition(127, 50, 200, 50); // 127 50 200 50: label position
    label_->SetTextColor(Color::White());
    label_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    label_->SetText("[ITEM_NAME]");
    label_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT_SIZE);
    container_->Add(label_);
}

bool UITestCoverflow2::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == addImage_) {
        UITransformGroup* page = new UITransformGroup();
        page->SetPosition(0, 100, PAGE_WIDTH, PAGE_HEIGHT); // 100: y

        UIImageView* image = new UIImageView();
        image->Resize(IMAGE_WIDTH, IMAGE_HEIGHT);
        page->Add(image);
        ImageInfo* src = ImageCacheManager::GetInstance().LoadSingleRes(RES_PATH"/planet.bin");
        image->SetSrc(src);
        image->LayoutCenterOfParent();

        UIImageView* imageSub = new UIImageView();
        imageSub->SetPosition(200, 0, 50, 50); // 200: x position 50: width and height
        page->Add(imageSub);
        src = ImageCacheManager::GetInstance().LoadSingleRes(std::string(SUB_IMG_PATH));
        imageSub->SetSrc(src);
        imageSub->SetTouchable(true);
        imageSub->SetOnClickListener(this);
        coverflow_->Insert(nullptr, page);
    } else {
        UITransformGroup* page = dynamic_cast<UITransformGroup*>(view.GetParent());
        UIView* child = page->GetChildrenHead();
        while (child != nullptr) {
            UIView* node = child;
            child = child->GetNextSibling();
            page->Remove(node);
            delete node;
        }
        coverflow_->Remove(page);
        delete page;
    }
    return true;
}

bool UITestCoverflow2::OnScroll(uint16_t distance)
{
    int16_t contMidX = static_cast<int16_t>(Screen::GetInstance().GetWidth() / DIVISOR_TWO);
    int16_t minOffset = contMidX;
    uint8_t newCenterIndex = lastIndex_;
    for (uint8_t i = 0; i < coverflow_->GetChildrenNumber(); i++) {
        Rect rect =  coverflow_->GetViewByIndex(i)->GetOrigRect();
        int16_t x = rect.GetLeft();
        int16_t x1 = rect.GetRight();
        int16_t xMid = static_cast<int16_t>(x + (x1 - x + 1) / DIVISOR_TWO);
        int16_t distance = MATH_ABS(xMid - contMidX);
        if (distance < minOffset) {
            newCenterIndex = i;
            minOffset = distance;
        }
    }
    if (newCenterIndex != lastIndex_ && newCenterIndex < COVERFLOW_MAX_PAGE_NUM) {
        lastIndex_ = newCenterIndex;
    }

    Rect curViewRect = coverflow_->GetViewByIndex(lastIndex_)->GetOrigRect();
    int16_t centerMid = curViewRect.GetLeft() + curViewRect.GetWidth() / DIVISOR_TWO;
    int16_t offset = MATH_ABS(contMidX - centerMid);
    if (offset > curViewRect.GetWidth() / DIVISOR_TWO) {
        label_->SetStyle(STYLE_TEXT_OPA, 0);
    } else {
        uint8_t alpha =  (uint8_t)((1.0f - ((float)offset / (curViewRect.GetWidth() / DIVISOR_TWO))) * OPA_OPAQUE);
        label_->SetStyle(STYLE_TEXT_OPA, alpha);
    }
    return true;
}
} // namespace OHOS
