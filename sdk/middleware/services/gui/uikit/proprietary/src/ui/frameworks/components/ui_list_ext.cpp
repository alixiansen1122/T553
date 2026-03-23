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

#include "components/ui_list_ext.h"
#include "animator/easing_equation.h"
#include "components/ui_menu_item.h"

namespace OHOS {
void UIListExt::ReMeasure()
{
    items_.Clear();
    int16_t midX = GetX() + GetWidth() / 2; // 2: divisor
    int16_t midY = GetY() + GetHeight() / 2; // 2: divisor
    int16_t centerIndex = 0;
    int16_t minCenterOfs = INT16_MAX;

    UIMenuItem* view = dynamic_cast<UIMenuItem*>(GetChildrenHead());
    while (view != nullptr) {
        items_.PushBack(view);
        view->Translate(Vector2<int16_t>(0, 0)); // ignore previous translate to set right transform

        int16_t absOfs = INT16_MAX;
        if (direction_ == VERTICAL) {
            absOfs = abs(midY - (view->GetY() + view->GetHeight() / 2)); // 2: divisor
        } else {
            absOfs = abs(midX - (view->GetX() + view->GetWidth() / 2)); // 2: divisor
        }
        int16_t temp = EasingEquation::SineEaseIn(0, scaleFactor_, absOfs, scaleFactor_);
        float scale = 1.0 - (static_cast<float>(temp) / scaleFactor_);
        view->Scale(Vector2<float>(scale, scale), view->GetScaleCenter());
        if (absOfs < minCenterOfs) {
            centerIndex = items_.Size() - 1;
            minCenterOfs = absOfs;
        }
        view = dynamic_cast<UIMenuItem*>(view->GetNextSibling());
    }
    ModifyItemSpace(centerIndex);
}

void UIListExt::ModifyMidAdjSpace(int16_t centerIndex, int16_t& prevIndex, int16_t& nextIndex)
{
    if (items_.Size() == 0) {
        return;
    }
    Rect centerRect = items_[centerIndex]->GetRect();
    if (direction_ == VERTICAL) {
        int16_t centerY = centerRect.GetTop() + (centerRect.GetHeight() >> 1);
        int16_t midY = GetHeight() / 2;
        if (centerY == midY) {
            return;
        }
        if (centerY > midY) {
            prevIndex = centerIndex - 1;
        } else {
            nextIndex = centerIndex + 1;
        }
        if ((prevIndex >= 0) && (nextIndex < items_.Size())) {
            Rect prevRect = items_[prevIndex]->GetRect();
            Rect nextRect = items_[nextIndex]->GetRect();
            int16_t prevMidY = prevRect.GetTop() + (prevRect.GetHeight() >> 1);
            int16_t nextMidY = nextRect.GetTop() + (nextRect.GetHeight() >> 1);
            int16_t delta = space_ - (nextRect.GetTop() - prevRect.GetBottom());
            int16_t prevOffst = (midY - prevMidY) * delta * 1.0f / (nextMidY - prevMidY);
            int16_t nextOffst = (nextMidY - midY) * delta * 1.0f / (nextMidY - prevMidY);
            items_[prevIndex]->Translate(Vector2<int16_t>(0, -prevOffst));
            items_[nextIndex]->Translate(Vector2<int16_t>(0, nextOffst));
        }
    } else {
        int16_t centerX = centerRect.GetLeft() + (centerRect.GetWidth() >> 1);
        int16_t midX = GetWidth() / 2;
        if (centerX == midX) {
            return;
        }
        if (centerX > midX) {
            prevIndex = centerIndex - 1;
        } else {
            nextIndex = centerIndex + 1;
        }
        if ((prevIndex >= 0) && (nextIndex < items_.Size())) {
            Rect prevRect = items_[prevIndex]->GetRect();
            Rect nextRect = items_[nextIndex]->GetRect();
            int16_t prevMidX = prevRect.GetLeft() + (prevRect.GetWidth() >> 1);
            int16_t nextMidX = nextRect.GetLeft() + (nextRect.GetWidth() >> 1);
            int16_t delta = space_ - (nextRect.GetLeft() - prevRect.GetRight());
            int16_t prevOffst = (midX - prevMidX) * delta * 1.0f / (nextMidX - prevMidX);
            int16_t nextOffst = (nextMidX - midX) * delta * 1.0f / (nextMidX - prevMidX);
            items_[prevIndex]->Translate(Vector2<int16_t>(-prevOffst, 0));
            items_[nextIndex]->Translate(Vector2<int16_t>(nextOffst, 0));
        }
    }
}

void UIListExt::ModifyItemSpace(int16_t centerIndex)
{
    int16_t prevIndex = centerIndex;
    int16_t nextIndex = centerIndex;
    // Step 1. only modify two neighboring items in the middle to ensure smoothly scrolling
    ModifyMidAdjSpace(centerIndex, prevIndex, nextIndex);
    Rect curRect;
    Rect preRect;
    int16_t diff = 0;
    // Step 2. modify other items space
    if (direction_ == VERTICAL) {
        for (int16_t i = prevIndex - 1; i >= 0; i--) {
            curRect = items_[i]->GetRect();
            preRect = items_[i + 1]->GetRect();
            diff = preRect.GetTop() - space_ - curRect.GetBottom();
            items_[i]->Translate(Vector2<int16_t>(0, diff));
        }
        for (int16_t i = nextIndex + 1; i < items_.Size(); i++) {
            curRect = items_[i]->GetRect();
            preRect = items_[i - 1]->GetRect();
            diff = preRect.GetBottom() + space_ - curRect.GetTop();
            items_[i]->Translate(Vector2<int16_t>(0, diff));
        }
    } else {
        for (int16_t i = prevIndex - 1; i >= 0; i--) {
            curRect = items_[i]->GetRect();
            preRect = items_[i + 1]->GetRect();
            diff = preRect.GetLeft() - space_ - curRect.GetRight();
            items_[i]->SetX(items_[i]->GetX() + diff);
        }
        for (int16_t i = nextIndex + 1; i < items_.Size(); i++) {
            curRect = items_[i]->GetRect();
            preRect = items_[i - 1]->GetRect();
            diff = preRect.GetRight() + space_ - curRect.GetLeft();
            items_[i]->SetX(items_[i]->GetX() + diff);
        }
    }
}
} // namespace OHOS
