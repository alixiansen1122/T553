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

#include "ui_test_coverflow.h"
#include "common/screen.h"
#include "font/ui_font.h"
#include "gfx_utils/graphic_math.h"
#include "gfx_utils/rect.h"
#include "common/image_cache_manager.h"

namespace OHOS {
namespace {
struct FaceInfo {
    const char* name;
    const char* path;
} ;

static constexpr uint8_t COVERFLOW_MAX_PAGE_NUM = 6;
static FaceInfo g_faceInfo[COVERFLOW_MAX_PAGE_NUM] = {
    {"1 rainbow", RES_PATH"/rainbow.bin"},
    {"2 kaleidoscope", RES_PATH"/kaleidoscope.bin"},
    {"3 planet", RES_PATH"/planet.bin"},
    {"4 rainbow", RES_PATH"/rainbow.bin"},
    {"5 kaleidoscope", RES_PATH"/kaleidoscope.bin"},
    {"6 planet", RES_PATH"/planet.bin"}
};

static constexpr uint8_t COVERFLOW_PRELOAD_PAGE_NUM = 3;
static constexpr int16_t LABEL_FONT_SIZE = 30;
static constexpr uint8_t DEFAULT_STEP = 10;
static constexpr uint16_t BUTTON_WIDTH = 60;
static constexpr uint16_t BUTTON_HEIGHT = 40;
static constexpr uint16_t BUTTON_GROUP_WIDTH = 300;
static constexpr uint16_t BUTTON_GROUP_HEIGHT = 80;
static constexpr uint8_t DIVISOR_TWO = 2;
} // namespace

void UITestCoverflow::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetThrowDrag(true);
        container_->SetHorizontalScrollState(false);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    }
}

void UITestCoverflow::TearDown()
{
    container_->Remove(coverflow_);
    DeleteChildren(container_);
    if (container_ != nullptr) {
        container_ = nullptr;
    }
    delete coverflow_;
    coverflow_ = nullptr;

    label_ = nullptr;
    lastIndex_ = 0;
    showBtn_ = nullptr;
    isShowBtnGrp_ = false;
    btnGrp_ = nullptr;
    loadNum_ = 0;

    addBtn_ = nullptr;
    removeBtn_ = nullptr;
    imgWidthAddBtn_ = nullptr;
    imgWidthSubBtn_ = nullptr;
    imgPaddingAddBtn_ = nullptr;
    imgPaddingSubBtn_ = nullptr;
    maxAngleAddBtn_ = nullptr;
    maxAngleSubBtn_ = nullptr;
    loopBtn_ = nullptr;

    imgWidth_ = 200;        // 200: default value
    imgPadding_ = 50;       // 50: default value
    mirrorOpa_ = 30;        // 30: default value
    rotateAngle_ = 70.0f;   // 70: default value
}

const UIView* UITestCoverflow::GetTestView()
{
    UITestCoverflowView001();
    return container_;
}


void UITestCoverflow::CreateLabelButton(UILabelButton** btn, const char* tile, int16_t x, int16_t y)
{
    UILabelButton* tempBtn = nullptr;
    if (tempBtn == nullptr) {
        tempBtn = new UILabelButton();
    }
    tempBtn->SetPosition(x, y, BUTTON_WIDTH, BUTTON_HEIGHT);
    tempBtn->SetText(tile);
    tempBtn->SetOnClickListener(this);
    btnGrp_->Add(tempBtn);
    *btn = tempBtn;
}

void UITestCoverflow::InitButtonGroup()
{
    if (showBtn_ == nullptr) {
        showBtn_ = new UIButton();
        showBtn_->SetPosition(202, 10, BUTTON_WIDTH, BUTTON_HEIGHT);  // 202: x position 10: y position
        showBtn_->SetStyleForState(STYLE_BACKGROUND_OPA, 0, UIButton::ButtonState::RELEASED);
        showBtn_->SetStyleForState(STYLE_BACKGROUND_OPA, 0, UIButton::ButtonState::PRESSED);
        showBtn_->SetOnClickListener(this);
        container_->Add(showBtn_);
    }
    if (btnGrp_ == nullptr) {
        btnGrp_ = new UIViewGroup();
        btnGrp_->SetPosition(77, 40, BUTTON_GROUP_WIDTH, BUTTON_GROUP_HEIGHT); // 77: x position 40: y position
        btnGrp_->SetStyle(STYLE_BACKGROUND_OPA, 0);
        btnGrp_->SetVisible(false);
        container_->Add(btnGrp_);
    }
    CreateLabelButton(&imgWidthAddBtn_, "wid+", 0, 0);
    CreateLabelButton(&imgWidthSubBtn_, "wid-", 60, 0);    // 60: x position
    CreateLabelButton(&maxAngleAddBtn_, "ang+", 120, 0);    // 120: x position
    CreateLabelButton(&maxAngleSubBtn_, "ang-", 180, 0);    // 180: x position
    CreateLabelButton(&addBtn_, "add", 0, 40);              // 40: y position
    CreateLabelButton(&removeBtn_, "sub", 60, 40);          // 60: x position 40: y position
    CreateLabelButton(&loopBtn_, "loop", 120, 40);          // 120: x position 40: y position
    CreateLabelButton(&imgPaddingAddBtn_, "pad+", 180, 40); // 180: x position 40: y position
    CreateLabelButton(&imgPaddingSubBtn_, "pad-", 240, 40); // 240: x position 40: y position
}

void UITestCoverflow::UITestCoverflowView001()
{
    if (container_ != nullptr) {
        if (coverflow_ == nullptr) {
            coverflow_ = new UICoverflowView();
        }
        coverflow_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        coverflow_->SetIntercept(true);
        coverflow_->SetThrowDrag(true);
        for (uint8_t i = 0; i < COVERFLOW_PRELOAD_PAGE_NUM; i++) {
            ImageInfo* src = ImageCacheManager::GetInstance().LoadSingleRes(std::string(g_faceInfo[i].path));
            coverflow_->Add(g_faceInfo[i].name, src);
            loadNum_++;
        }

        if (label_ == nullptr) {
            label_ = new UILabel();
        }
        label_->SetPosition(127, 50, 200, 50); // 127 50 200 50: label position
        label_->SetTextColor(Color::White());
        label_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        lastIndex_ = coverflow_->GetCurrentPage();
        label_->SetText(g_faceInfo[lastIndex_].name);
        label_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT_SIZE);

        coverflow_->SetOnCoverflowListener(this);
        container_->Add(coverflow_);
        container_->Add(label_);
        InitButtonGroup();
    }
}

bool UITestCoverflow::OnScroll()
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
    if (newCenterIndex != lastIndex_) {
        lastIndex_ = newCenterIndex;
        label_->SetText(coverflow_->GetViewByIndex(lastIndex_)->GetViewId());
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

bool UITestCoverflow::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == showBtn_) {
        btnGrp_->SetVisible(!btnGrp_->IsVisible());
    } else if (&view == loopBtn_) {
        coverflow_->SetLoopState(true);
        loopBtn_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
    } else if (&view == imgWidthAddBtn_) {
        imgWidth_ += DEFAULT_STEP;
        coverflow_->SetImgSize(imgWidth_, imgWidth_);
    } else if (&view == imgWidthSubBtn_) {
        imgWidth_ -= DEFAULT_STEP;
        coverflow_->SetImgSize(imgWidth_, imgWidth_);
    } else if (&view == imgPaddingAddBtn_) {
        imgPadding_ += DEFAULT_STEP;
        coverflow_->SetImgPadding(imgPadding_);
    } else if (&view == imgPaddingSubBtn_) {
        imgPadding_ -= DEFAULT_STEP;
        coverflow_->SetImgPadding(imgPadding_);
    } else if (&view == addBtn_) {
        if (loadNum_ >= COVERFLOW_MAX_PAGE_NUM) {
            return true;
        }
        ImageInfo* src = ImageCacheManager::GetInstance().LoadSingleRes(std::string(g_faceInfo[loadNum_].path));
        coverflow_->Add(g_faceInfo[loadNum_++].name, src);
    } else if (&view == removeBtn_) {
        if (loadNum_ > 0) {
            coverflow_->Remove(g_faceInfo[--loadNum_].name);
        }
    } else if (&view == maxAngleAddBtn_) {
        rotateAngle_ += 2.0;  // 2.0 variables per click
        coverflow_->SetRotateAngle(rotateAngle_);
    } else if (&view == maxAngleSubBtn_) {
        rotateAngle_ -= 2.0; // 2.0 variables per click
        coverflow_->SetRotateAngle(rotateAngle_);
    }
    coverflow_->Invalidate();
    return true;
}
} // namespace OHOS
