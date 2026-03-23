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

#ifndef GRAPHIC_LITE_UI_SOFT_KEYBOARD_H
#define GRAPHIC_LITE_UI_SOFT_KEYBOARD_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_list.h"
#include "common/input_method_manager.h"
#include "components/ui_edit_text.h"
#include "input_method/ui_abstract_soft_keyboard.h"
#include "input_method/ui_option_box.h"
#include "input_method/ui_text_preview_box.h"
#include "input_method/ui_pinyin_candidate.h"
#include "gfx_utils/vector.h"

#if ENABLE_SOFT_KEYBOARD
namespace OHOS {
/**
 * @brief Provides the functions related to UIInputMethod.
 */
class UIInputMethod : public InputMethodManager::InputMethodListener,
    public UIView::OnClickListener, public ListScrollListener,
    public UIView::OnDragListener {
    class HanziAdapter : public AbstractAdapter {
    public:
        ~HanziAdapter() override {};
        HanziAdapter() {};
        HanziAdapter(const HanziAdapter&) = delete;
        HanziAdapter& operator=(const HanziAdapter&) = delete;
        UIView* GetView(UIView* inView, int16_t index) override;
        int16_t GetItemWidthWithMargin(int16_t index) override;
        int16_t GetItemHeightWithMargin(int16_t index) override;
        uint16_t GetCount() override;
        bool SetWidthAndHeight(int16_t w, int16_t h);
        void SetData(int8_t hanzi[][PinyinCandidate::IME_HZLENTH], uint8_t len);
        void SetClickListener(UIView::OnClickListener* clickListener)
        {
            clickListener_ = clickListener;
        }

    private:
        UIView::OnClickListener* clickListener_ = nullptr;
        int8_t (*data_)[PinyinCandidate::IME_HZLENTH] = nullptr;
        uint8_t size_ = 0;
        int16_t width_ = 0;
        int16_t height_ = 0;
    };
public:
    static constexpr uint8_t DEFAULT_NUMPERPAGE = 5;
    static constexpr uint8_t MAX_HANZIMATCHED = 255;
    static UIInputMethod* GetInstance();

    /**
     * @brief Invoke this method when edit text view get focused.
     * @param param  the param passed, see InputMethodParam.
     */
    void OnShow(InputMethodManager::InputMethodParam& param) override;

    /**
     * @brief Invoke this method when edit text view get blured.
     */
    void OnHide() override;

    /**
     * @brief Enter a key value in the input method.
     * @param keys key value, see UI_KEYCODE.
     */
    void Input(UI_KEYCODE key);

    void InputPinyin(UI_KEYCODE key);

    void ShowHanziPageList();

    void HideHanziPageList();

    /**
     * @brief Hide soft keyboad option box.
     * @return Returns index of currently selected label.
     */
    uint8_t HideOptionBox();

    /**
     * @brief Show soft keyboad option box.
     * @param keys point key value array, see UI_KEYCODE.
     * @param num key number.
     */
    void ShowOptionBox(UIView *view, UI_KEYCODE *keys, uint8_t num);

    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    UIInputMethod();
    ~UIInputMethod() override;
    UIInputMethod(const UIInputMethod &) = delete;
    UIInputMethod &operator=(const UIInputMethod &) = delete;
    void InitInputMethod();
    void SetupOptionBox();
    void SetupSoftKeyboard();
    void SetupTextPreviewBox();
    void InsertChar(UI_KEYCODE keyCode);
    void InsertPinyin(UI_KEYCODE pinyin);
    void UpdateHanziOption(const std::string &pinyin);
    void ChangSoftKeyboard(UI_KEYCODE keyCode);
    void DealFunctionKey(UI_KEYCODE keyCode);
    void DealNormalKey(UI_KEYCODE keyCode);
    void DealSendKey();
    void DealDelKey();
    void DealPinyinDelKey();

    bool OnDrag(UIView& view, const DragEvent& event) override
    {
        RefreshUi();
        return false;
    }

    void RefreshUi()
    {
        if (inputMethod_ != nullptr) {
            inputMethod_->Invalidate();
        }
    }

    UIViewGroup* inputMethod_ = nullptr;
    UITextPreviewBox* previeBox_ = nullptr;  // soft keyboard edit box
    UIEditText* edit_ = nullptr;         // user creat edit box
    UIOptionBox* optionBox_ = nullptr;  // Preselection box

    UILabel* pinyinPreview_ = nullptr; // pinyin preview box
    void SetupPinyinPreviewBox();
    void HidePinyinPreview();
    void ShowPinyinPreview();

    UIList *hanziList_ = nullptr;           // hanzi candidates
    HanziAdapter *hanziAdapter_ = nullptr;  // hanzi adapter
    int8_t hanziMatched_[MAX_HANZIMATCHED][PinyinCandidate::IME_HZLENTH];
    void ClearHanziCandidate();
    void SetupHanziCandidate();

    PinyinCandidate* pinyinTable_ = nullptr; // pinyin and hanzi find table
    void SetupPinyinTable();
    void ClearPinyinTable();
    using PinyinState = PinyinCandidate::PinyinState;

    UIAbstractSoftKeyboard* curSoftKeyboard_ = nullptr;
    uint8_t curKeyboardIndex_ = 0;
    Graphic::Vector<UIAbstractSoftKeyboard*> customSoftKeyboard_;
    bool isShow_ = false;
    bool isInit_ = false;
};
} // namespace OHOS
#endif // ENABLE_SOFT_KEYBOARD
#endif // GRAPHIC_LITE_UI_SOFT_KEYBOARD_H
