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
#include "input_method/ui_option_box.h"
#include "input_method/ui_keyboard_button.h"

#if ENABLE_SOFT_KEYBOARD
namespace OHOS {
namespace {
constexpr uint32_t OPT_COLOR = 0xFFE8EAEE;
constexpr uint32_t OPT_HIGHLIGHT_COLOR = 0xFFCCCCCC;
constexpr uint32_t KEY_FONT_SIZE = 20;
}

UIOptionBox::UIOptionBox()
{
    SetStyle(STYLE_BACKGROUND_OPA, 0);
    SetTouchable(false);
    SetDraggable(false);
}

UIOptionBox::~UIOptionBox()
{
    RemoveAll();
    for (int i = 0; i < options_.Size(); i++) {
        if (options_[i] != nullptr) {
            delete options_[i];
        }
    }
    options_.Clear();

    if (listener_ != nullptr) {
        delete listener_;
    }
}

void UIOptionBox::InitUIOptionBox()
{
    static bool isInit = false;
    if (isInit) {
        return;
    }
    for (int i = 0; i < MAX_KEY_VALUE_NUM; i++) {
        UILabel* label = new UILabel();
        this->Add(label);
        label->SetStyle(STYLE_BORDER_RADIUS, 0);
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, KEY_FONT_SIZE);
        label->SetTextColor(Color::Black());
        label->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        label->SetVisible(false);
        options_.PushBack(label);
    }

    if (listener_ == nullptr) {
        listener_ = new OptionDragListener(this);
    }
    isInit = true;
    return;
}

void UIOptionBox::SetTextToLabel(UI_KEYCODE key, UILabel* label)
{
    if (label == nullptr) {
        return;
    }
    std::string tmpText;
    if (key >= UI_KEYCODE::UI_KEY_ENTER) {
        UIKeyboardButton::GetFuncKeyText(key, tmpText);
    } else if (key == UI_KEYCODE::UI_KEY_SPACE) {
        tmpText += "Space";
    } else {
        tmpText.push_back(static_cast<char>(key));
    }
    label->SetText(tmpText.c_str());
}

void UIOptionBox::UpdateContent(UI_KEYCODE *keys, uint8_t num)
{
    uint8_t keyNum = num;
    if (keyNum > MAX_KEY_VALUE_NUM) {
        GRAPHIC_LOGE("Max option number is : %u\n", MAX_KEY_VALUE_NUM);
        return;
    }
    currentOptionNum_ = keyNum;
    uint8_t gap = 2;
    uint16_t buttonGap = (GetWidth() - gap) / keyNum;
    btnWidth_ = buttonGap - gap;
    uint16_t btnHeight = GetHeight() - gap * 2; // 2: half
    for (int i = 0; i < MAX_KEY_VALUE_NUM; i++) {
        if (i < keyNum) {
            options_[i]->SetVisible(true);
            options_[i]->SetPosition(buttonGap * i + gap, gap, btnWidth_, btnHeight);
            options_[i]->SetStyle(STYLE_BACKGROUND_COLOR, OPT_COLOR);
            SetTextToLabel(keys[i], options_[i]);
        } else {
            options_[i]->SetVisible(false);
        }
    }
    lastSelectedIndex_ = selectedIndex_;
    selectedIndex_ = keyNum / 2; // 2: half
    RefreshSelected();
}

void UIOptionBox::RefreshSelected()
{
    if (lastSelectedIndex_ != selectedIndex_) {
        options_[lastSelectedIndex_]->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
        options_[lastSelectedIndex_]->SetStyle(STYLE_BACKGROUND_COLOR, OPT_COLOR);
    }
    options_[selectedIndex_]->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    options_[selectedIndex_]->SetStyle(STYLE_BACKGROUND_COLOR, OPT_HIGHLIGHT_COLOR);
    Invalidate();
}

bool UIOptionBox::OptionDragListener::OnDrag(UIView& view, const DragEvent& event)
{
    if (box_ == nullptr) {
        return false;
    }
    int16_t distance = event.GetDeltaX();
    box_->moveDistance_ += distance;
    int16_t flag = (box_->moveDistance_ > 0) ? 1 : -1;
    box_->moveDistance_ *= flag;
    if (box_->moveDistance_ > static_cast<int16_t>(box_->btnWidth_) / 2) { // 2: half
        if (flag > 0) {
            if (box_->selectedIndex_ < box_->currentOptionNum_ - 1) {
                box_->lastSelectedIndex_ = box_->selectedIndex_;
                box_->selectedIndex_ += 1;
                box_->RefreshSelected();
            }
        } else {
            if (box_->selectedIndex_ > 0) {
                box_->lastSelectedIndex_ = box_->selectedIndex_;
                box_->selectedIndex_ -= 1;
                box_->RefreshSelected();
            }
        }
        box_->moveDistance_ -= static_cast<int16_t>(box_->btnWidth_) / 2; // 2: half
    }
    box_-> moveDistance_ *= flag;
    return true;
}
} // namespace OHOS
#endif // ENABLE_SOFT_KEYBOARD
