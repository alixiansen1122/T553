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

#include "ui_test_svg/ui_test_svg.h"
#include "components/root_view.h"

namespace OHOS {
void UITestSVG::SetUp()
{
    if (group_ == nullptr) {
        group_ = new UIViewGroup();
        group_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        group_->SetStyle(STYLE_BACKGROUND_COLOR, 0xFFC8C8C8); // R,G,B are all set to 200.
        group_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    }
    if (canvas_ == nullptr) {
        canvas_ = new UICanvasExt();
        canvas_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        index_ = canvas_->DrawSvg(RES_PATH"/WatchsvgtestPath.bin");
        group_->Add(canvas_);
    }
    if (btn_ == nullptr) {
        btn_ = new UILabelButton();
        btn_->SetPosition(167, 370, 120, 60); // 167:x 370:x 120:width 60:height
        btn_->SetText("Set Matrix");
        btn_->SetOnClickListener(this);
        group_->Add(btn_);
    }
}

void UITestSVG::TearDown()
{
    group_->Remove(canvas_);
    delete canvas_;
    canvas_ = nullptr;

    group_->Remove(btn_);
    delete btn_;
    btn_ = nullptr;

    group_->RemoveAll();
    delete group_;
    group_ = nullptr;
}

const UIView* UITestSVG::GetTestView()
{
    return group_;
}
}
