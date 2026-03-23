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

#ifndef GRAPHIC_LITE_UI_OPTION_BOX_H
#define GRAPHIC_LITE_UI_OPTION_BOX_H
#include "components/ui_label.h"
#include "components/ui_view_group.h"
#include "gfx_utils/vector.h"
#include "graphic_config.h"
#include "input_method/ui_keyboard_codetable.h"

#if ENABLE_SOFT_KEYBOARD
namespace OHOS {
class UIOptionBox : public UIViewGroup {
public:
    UIOptionBox();
    ~UIOptionBox() override;
    UIOptionBox(const UIOptionBox &) = delete;
    UIOptionBox &operator=(const UIOptionBox &) = delete;
    void InitUIOptionBox();
    void UpdateContent(UI_KEYCODE *keys, uint8_t num);
    uint8_t GetSelectIndex()
    {
        return selectedIndex_;
    }

    UIViewType GetViewType() const override
    {
        return UI_OPTION_BOX;
    }

    class OptionDragListener : public UIView::OnDragListener {
    public:
        explicit OptionDragListener(UIOptionBox* optBox) : box_(optBox) {}
        ~OptionDragListener() override {}

        bool OnDrag(UIView& view, const DragEvent& event) override;
    private:
        UIOptionBox* box_ = nullptr;
    };

    OptionDragListener* GetDragListener()
    {
        return listener_;
    }
private:
    friend class OptionDragListener;
    void RefreshSelected();
    void SetTextToLabel(UI_KEYCODE key, UILabel* label);

    OptionDragListener* listener_ = nullptr;
    Graphic::Vector<UILabel*> options_;
    uint8_t currentOptionNum_ = 4;
    uint16_t btnWidth_ = 0;
    int16_t moveDistance_ = 0;
    uint8_t selectedIndex_ = 0;
    uint8_t lastSelectedIndex_ = 0;
};
} // namespace OHOS
#endif // ENABLE_SOFT_KEYBOARD
#endif // GRAPHIC_LITE_UI_OPTION_BOX_H