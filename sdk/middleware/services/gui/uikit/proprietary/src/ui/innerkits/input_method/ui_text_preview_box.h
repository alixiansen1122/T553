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

#ifndef GRAPHIC_LITE_UI_EDIT_TEXT_H
#define GRAPHIC_LITE_UI_EDIT_TEXT_H

#include <string>
#include <codecvt>
#include <locale>
#include <vector>
#include "components/ui_edit_text.h"
#include "input_method/ui_keyboard_codetable.h"
#include "graphic_config.h"

#if ENABLE_SOFT_KEYBOARD
namespace OHOS {
struct LineInfo {
    std::vector<uint32_t> charCountPerLine;
    int16_t lineHeight;
};

class CursorCallback : public AnimatorCallback {
public:
    void Callback(UIView* view) override;
    void OnStop(UIView& view) override;
private:
    uint8_t count = 0;
};

/**
 * @brief Provides the functions related to UITextPreviewBox.
 */
class UITextPreviewBox : public UIEditText {
public:
    enum TextPreviewBoxMode {
        TEXT_PREVIEW_BOX_MODE_INSERT,
        TEXT_PREVIEW_BOX_MODE_REPLACE,
        TEXT_PREVIEW_BOX_MODE_READ_ONLY,
        TEXT_PREVIEW_BOX_MODE_UNKNOW,
    };

    UITextPreviewBox() {}
    ~UITextPreviewBox() override;
    UITextPreviewBox(const UITextPreviewBox &) = delete;
    UITextPreviewBox &operator=(const UITextPreviewBox &) = delete;

    /**
     * @brief Obtains the view type.
     *
     * @return Returns <b>UI_TEXT_PREVIEW_BOX</b>, as defined in {@link UIViewType}.
     */
    UIViewType GetViewType() const override
    {
        return UI_TEXT_PREVIEW_BOX;
    }

    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;
    bool OnClickEvent(const ClickEvent& event) override;
    void InsertChar(UI_KEYCODE& keyCode);
    void InsertHanzi(const std::string &u8hanzi);
    void DeleteChar();
    void SetText(std::string text);

    void SetEditMode(TextPreviewBoxMode mode)
    {
        editMode_ = mode;
    }

    TextPreviewBoxMode GetEditMode()
    {
        return editMode_;
    }

    static constexpr uint8_t DEFAULT_EDIT_FONT_SIZE = 32;
protected:
    void InitText() override;
    void InitCursor();
    void StartCursorAnimator();
    void StopCursorAnimator();
    void HandleCursorPos(uint16_t x, uint16_t y);
    void HandleCursorPosByIndex();
    bool CheckSpaceLeft(const char* newString);
    void GetLineInfo(LineInfo& lineInfo);

private:
    std::wstring Utf8ToUtf16(const std::string &text) const
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
        std::wstring wideText = convert.from_bytes(text);
        return wideText;
    }

    std::string Utf16ToUtf8(const std::wstring &wtext) const
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
        return convert.to_bytes(wtext);
    }

    uint32_t cursorIndex_ = 0;
    bool isCursorInited_ = false;
    CursorCallback cursorCallback_;
    Animator *cursorAnimator_ = nullptr;
    UIView *cursorView_ = nullptr;
    LineInfo lineInfo_ = {{0}};
    std::wstring tempText_;
    std::vector<uint16_t> widthOfChars_;

    TextPreviewBoxMode editMode_ = TEXT_PREVIEW_BOX_MODE_INSERT;
};
} // namespace OHOS
#endif // ENABLE_SOFT_KEYBOARD
#endif // GRAPHIC_LITE_UI_EDIT_TEXT_H
