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

#include "ui_test_map.h"
#include "common/screen.h"
#include "components/root_view.h"

namespace OHOS {
static float g_scale = 1.0;
static const float SCALE_LIMIT = 0.5;
static const float SCALE_STEP = 0.1;
static const Vector2<float> SCALE_CENTER = {228, 178}; // 228, 178: scale center

void UITestMap::SetUp()
{
    container_ = new UIScrollView();
    container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    container_->SetScrollBlankSize(200); // 200: blank size

    map_ = new UIMapView();
    map_->SetPosition(100, 50, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight()); // 100, 50: x, y
    map_->SetSVG(RES_PATH"map.bin");
    container_->Add(map_);

    scale_ = new UILabelButton();
    scale_->SetPosition(57, 340, 100, 50); // 57: x, 340: y, 100: width, 50: height
    scale_->SetText("scale");
    scale_->SetOnClickListener(this);
    container_->Add(scale_);

    cache_ = new UILabelButton();
    cache_->SetPosition(177, 340, 100, 50); // 177: x, 340: y, 100: width, 50: height
    cache_->SetText("cache");
    cache_->SetOnClickListener(this);
    container_->Add(cache_);

    reset_ = new UILabelButton();
    reset_->SetPosition(297, 340, 100, 50); // 297: x, 340: y, 100: width, 50: height
    reset_->SetText("reset");
    reset_->SetOnClickListener(this);
    container_->Add(reset_);
}

void UITestMap::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    scale_ = nullptr;
    cache_ = nullptr;
    reset_ = nullptr;
    map_ = nullptr;
}

const UIView* UITestMap::GetTestView()
{
    return container_;
}

bool UITestMap::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == scale_) {
        g_scale -= SCALE_STEP;
        if (g_scale < SCALE_LIMIT) {
            g_scale = SCALE_LIMIT;
        }
        map_->EnableCache(false);
        map_->ScaleMap(g_scale, SCALE_CENTER);
    } else if (&view == cache_) {
        if (!cached_) {
            cache_->SetText("clear cache");
            map_->EnableCache(true);
            cached_ = true;
        } else {
            cache_->SetText("cache");
            map_->EnableCache(false);
            cached_ = false;
        }
    } else if (&view == reset_) {
        g_scale = 1.0;
        map_->EnableCache(false);
        map_->ScaleMap(g_scale, SCALE_CENTER);
    }
    RootView::GetInstance()->Invalidate();
    return true;
}
}
