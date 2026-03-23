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

#ifndef GRAPHIC_LITE_UI_INPUT_METHOD_H
#define GRAPHIC_LITE_UI_INPUT_METHOD_H

#include "components/ui_label.h"
#include "components/ui_view_group.h"
#include "gfx_utils/vector.h"
#include "input_method/ui_keyboard_button.h"
#include "input_method/ui_keyboard_codetable.h"

#if ENABLE_SOFT_KEYBOARD
namespace OHOS {
class UIAbstractSoftKeyboard : public UIViewGroup {
public:
    void InitSoftKeyboard();

protected:
    UIAbstractSoftKeyboard();
    ~UIAbstractSoftKeyboard() override;
    virtual void CreateKeyButtons() = 0;
    void SetupKeyButton(SoftKey &key);

private:
    static void AllocKeyboardButton();
    static void ClearKeyBtnPool();
    static uint16_t usePoolCount_;
    static Graphic::Vector<UIKeyboardButton*> keyBtnAllocPool_;
    uint16_t curUseKeyBtnIndex_ = 0;
};
} // namespace OHOS
#endif // ENABLE_SOFT_KEYBOARD
#endif // GRAPHIC_LITE_UI_INPUT_METHOD_H
