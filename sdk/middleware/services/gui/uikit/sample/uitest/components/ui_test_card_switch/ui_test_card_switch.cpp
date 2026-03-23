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

#include "ui_test_card_switch/ui_test_card_switch.h"
#include "components/root_view.h"
#include "securec.h"

namespace OHOS {
static const uint16_t LABEL_FONT_SIZE = 200;
static const uint32_t BACK_GROUND_COLOR = 0xfff6f6f6;
static const uint32_t CARD_PAGE_COLOR = 0xffd0d0d0;

void UITestCardSwitch::SetUp()
{
    if (swipe_ == nullptr) {
        swipe_ = new UISwipeView(UISwipeView::HORIZONTAL);
        swipe_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        swipe_->SetThrowDrag(true);
        swipe_->SetStyle(STYLE_BACKGROUND_COLOR, BACK_GROUND_COLOR);
        swipe_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
        swipe_->SetBlankSize(0);
        swipe_->SetReboundSize(200); // 200: rebound size
        swipe_->SetElastic(true);
    }
}

void UITestCardSwitch::TearDown()
{
    if (swipe_ != nullptr) {
        swipe_->RemoveAll();
        delete swipe_;
        swipe_ = nullptr;
    }

    if (label1_ != nullptr) {
        delete label1_;
        label1_ = nullptr;
    }

    if (label2_ != nullptr) {
        delete label2_;
        label2_ = nullptr;
    }

    if (label3_ != nullptr) {
        delete label3_;
        label3_ = nullptr;
    }
}

const UIView* UITestCardSwitch::GetTestView()
{
    Init();
    return swipe_;
}

void UITestCardSwitch::SetUpLabel(UILabel** label, const char* text)
{
    *label = new UILabel();
    if (*label == nullptr) {
        return;
    }

    (*label)->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    (*label)->SetText(text);
    (*label)->SetStyle(STYLE_BACKGROUND_COLOR, CARD_PAGE_COLOR);
    (*label)->SetStyle(STYLE_TEXT_COLOR, Color::Gray().full);
    (*label)->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT_SIZE);
    (*label)->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    (*label)->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    swipe_->Add(*label);
}

void UITestCardSwitch::Init()
{
    SetUpLabel(&label1_, "1");
    SetUpLabel(&label2_, "2");
    SetUpLabel(&label3_, "3");
}
}
