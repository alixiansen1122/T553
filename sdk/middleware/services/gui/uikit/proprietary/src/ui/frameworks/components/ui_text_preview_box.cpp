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

#include "input_method/ui_text_preview_box.h"
#include "input_method/ui_input_method.h"
#include "font/ui_font.h"

#if ENABLE_SOFT_KEYBOARD
namespace OHOS {
void CursorCallback::Callback(UIView* view)
{
    if (view == nullptr) {
        return;
    }
    ++count;
    if (count == 30) { // 30: 30 cycles per change
        view->SetVisible(!view->IsVisible());
        count = 0;
    }
}

void CursorCallback::OnStop(UIView& view)
{
    count = 0;
    view.SetVisible(false);
}

UITextPreviewBox::~UITextPreviewBox()
{
    StopCursorAnimator();
    if (cursorAnimator_ != nullptr) {
        delete cursorAnimator_;
        cursorAnimator_ = nullptr;
    }
    if (cursorView_ != nullptr) {
        delete cursorView_;
        cursorView_ = nullptr;
    }
    isCursorInited_ = false;
    widthOfChars_.clear();
    tempText_.clear();
}

bool UITextPreviewBox::OnClickEvent(const ClickEvent& event)
{
    HandleCursorPos(event.GetCurrentPos().x, event.GetCurrentPos().y);
    return true;
}

void UITextPreviewBox::InitText()
{
    UIEditText::InitText();
    if (inputText_ != nullptr) {
        inputText_->SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
        inputText_->SetExpandWidth(false);
    }

    InitCursor();
}

void UITextPreviewBox::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    UIView::OnDraw(gfxDstBuffer, invalidatedArea);
    if (inputText_ != nullptr && inputText_->GetText() != nullptr && strlen(inputText_->GetText()) > 0) {
        Rect textRect = GetContentRect();
        Style style = GetStyleConst();
        style.textColor_ = GetTextColor();
        inputText_->ReMeasureTextSize(textRect, style);
        OpacityType opa = GetMixOpaScale();
        inputText_->OnDraw(gfxDstBuffer, invalidatedArea,
            GetOrigRect(), textRect, 0, style, Text::TEXT_ELLIPSIS_END_INV, opa);
    }

    if (GetInputType() != InputType::PASSWORD_TYPE && cursorView_->IsVisible()) {
        cursorView_->OnDraw(gfxDstBuffer, invalidatedArea);
    }
}

bool UITextPreviewBox::CheckSpaceLeft(const char* newString)
{
    if (inputText_ == nullptr) {
        return false;
    }
    if (newString == nullptr) {
        return false;
    }

    Text temp;
    temp.SetFont(DEFAULT_VECTOR_FONT_FILENAME, inputText_->GetFontSize());
    temp.SetText(newString);

    UIFont::GetInstance()->SetCurrentFontId(inputText_->GetFontId(), inputText_->GetFontSize());

    int16_t lineHeight = GetStyleConst().GetStyle(STYLE_LINE_HEIGHT);
    if (lineHeight == 0) {
        lineHeight =
            static_cast<int16_t>(UIFont::GetInstance()->GetHeight()) + GetStyleConst().GetStyle(STYLE_LINE_SPACE);
    }
    uint16_t lineNum = 0;
    uint32_t textLen = strlen(newString);

    uint32_t begin = 0;
    while ((begin < textLen) && (newString[begin] != '\0') && (lineNum < GetMaxLength())) {
        uint32_t count = temp.GetTextLine(begin, textLen, GetContentRect().GetWidth(), lineNum, 0);
        begin += count;
        lineNum++;
    }

    if ((static_cast<uint16_t>(lineNum - 1) * lineHeight + UIFont::GetInstance()->GetHeight()) >
        static_cast<uint16_t>(GetContentRect().GetHeight())) {
        return false;
    }

    return true;
}

void UITextPreviewBox::GetLineInfo(LineInfo& lineInfo)
{
    if (inputText_ == nullptr) {
        return;
    }
    const char* text = inputText_->GetText();
    if (text == nullptr) {
        return;
    }
    std::string u8str(text);
    std::wstring u16str;
    UIFont::GetInstance()->SetCurrentFontId(inputText_->GetFontId(), inputText_->GetFontSize());
    lineInfo.lineHeight = GetStyleConst().GetStyle(STYLE_LINE_HEIGHT);
    if (lineInfo.lineHeight == 0) {
        lineInfo.lineHeight =
            static_cast<int16_t>(UIFont::GetInstance()->GetHeight()) + GetStyleConst().GetStyle(STYLE_LINE_SPACE);
    }
    lineInfo.charCountPerLine.clear();
    uint16_t lineNum = 0;
    uint32_t textLen = strlen(text);
    uint32_t begin = 0;
    while ((begin < textLen) && (text[begin] != '\0') && (lineNum < GetMaxLength())) {
        uint32_t count = inputText_->GetTextLine(begin, textLen, GetContentRect().GetWidth(), lineNum, 0);
        u16str = Utf8ToUtf16(u8str.substr(begin, count));
        begin += count;
        lineInfo.charCountPerLine.push_back(u16str.length());
        lineNum++;
    }
}

void UITextPreviewBox::InitCursor()
{
    if (cursorView_ == nullptr) {
        cursorView_ = new UIView();
    }
    if (cursorAnimator_ == nullptr) {
        cursorAnimator_ = new Animator(&cursorCallback_, cursorView_, 0, true);
    }

    StartCursorAnimator();

    int16_t lineHeight = GetStyleConst().lineHeight_;
    if (lineHeight == 0) {
        uint8_t fontId = UIFont::GetInstance()->GetFontId(DEFAULT_VECTOR_FONT_FILENAME);
        UIFont::GetInstance()->SetCurrentFontId(fontId, DEFAULT_EDIT_FONT_SIZE);
        lineHeight = static_cast<int16_t>(UIFont::GetInstance()->GetHeight());
        if (lineHeight != cursorView_->GetHeight()) {
            cursorView_->SetHeight(lineHeight);
        }
    }

    if (!isCursorInited_) {
        isCursorInited_ = true;
        cursorView_->SetPosition(GetRelativeRect().GetX(), GetRelativeRect().GetY(), 1, lineHeight);
    }

    Invalidate();
}

void UITextPreviewBox::InsertChar(UI_KEYCODE& keyCode)
{
    uint16_t len = TypedText::GetUTF8CharacterSize(GetText());
    if (len >= GetMaxLength()) {
        return;
    }
    std::wstring temp = tempText_;
    std::string str;
    temp.insert(cursorIndex_, 1, static_cast<wchar_t>(keyCode));
    str = Utf16ToUtf8(temp);
    if (!CheckSpaceLeft(str.c_str())) {
        GRAPHIC_LOGE("Character length exceeded. Fail to input the letter.");
        return;
    }

    tempText_.insert(cursorIndex_, 1, static_cast<wchar_t>(keyCode));
    uint16_t width = UIFont::GetInstance()->GetWidth(static_cast<uint32_t>(keyCode), 0);
    widthOfChars_.insert(widthOfChars_.begin() + cursorIndex_, width);

    str = Utf16ToUtf8(tempText_);
    UIEditText::SetText(str.c_str());
    GetLineInfo(lineInfo_);

    cursorIndex_++;
    HandleCursorPosByIndex();
}

void UITextPreviewBox::InsertHanzi(const std::string &u8hanzi)
{
    uint16_t len = TypedText::GetUTF8CharacterSize(GetText());
    if (len >= GetMaxLength()) {
        return;
    }
    std::wstring temp = tempText_;
    std::string str;
    std::wstring u16hanzi = Utf8ToUtf16(u8hanzi);
    temp.insert(cursorIndex_, 1, u16hanzi[0]);
    str = Utf16ToUtf8(temp);
    if (!CheckSpaceLeft(str.c_str())) {
        GRAPHIC_LOGE("Character length exceeded. Fail to input the letter.");
        return;
    }

    tempText_.insert(cursorIndex_, 1, u16hanzi[0]);
    uint16_t width = UIFont::GetInstance()->GetWidth(u16hanzi[0], 0);
    widthOfChars_.insert(widthOfChars_.begin() + cursorIndex_, width);

    str = Utf16ToUtf8(tempText_);
    UIEditText::SetText(str.c_str());
    GetLineInfo(lineInfo_);

    cursorIndex_++;
    HandleCursorPosByIndex();
}

void UITextPreviewBox::DeleteChar()
{
    if (tempText_.empty() || cursorIndex_ == 0) {
        return;
    }
    tempText_.erase(cursorIndex_ - 1, 1);
    widthOfChars_.erase(widthOfChars_.begin() + cursorIndex_ - 1);

    std::string temp = Utf16ToUtf8(tempText_);
    UIEditText::SetText(temp.c_str());
    GetLineInfo(lineInfo_);

    cursorIndex_--;
    HandleCursorPosByIndex();
}

void UITextPreviewBox::StartCursorAnimator()
{
    if (cursorAnimator_->GetState() != Animator::START) {
        cursorAnimator_->Start();
    }
}

void UITextPreviewBox::StopCursorAnimator()
{
    if (cursorAnimator_->GetState() != Animator::STOP) {
        cursorAnimator_->Stop();
    }
}

void UITextPreviewBox::HandleCursorPosByIndex()
{
    if (cursorView_ == nullptr) {
        return;
    }

    uint32_t begin = 0;
    uint16_t lineCount = lineInfo_.charCountPerLine.size();
    uint16_t lineNum = 0;
    for (uint16_t i = 0; i < lineCount; i++) {
        lineNum = i;
        if (cursorIndex_ <= (begin + lineInfo_.charCountPerLine[i])) {
            break;
        }
        begin += lineInfo_.charCountPerLine[i];
    }

    uint16_t xPos = 0;
    for (uint32_t j = begin; j < cursorIndex_; j++) {
        xPos += widthOfChars_[j];
    }

    cursorView_->SetPosition(xPos + GetRelativeRect().GetX(),
        GetRelativeRect().GetY() + lineNum * lineInfo_.lineHeight);
    Invalidate();
}

void UITextPreviewBox::HandleCursorPos(uint16_t x, uint16_t y)
{
    if (cursorView_ == nullptr) {
        return;
    }

    uint32_t begin = 0;
    uint16_t lineCount = lineInfo_.charCountPerLine.size();
    if (lineCount == 0) {
        return;
    }
    uint16_t lineNum = 0;
    uint16_t currentLineBottomY = static_cast<uint16_t>(GetContentRect().GetY());
    bool isBelowLastRow = false;
    for (uint16_t i = 0; i < lineCount; i++) {
        currentLineBottomY += static_cast<uint16_t>(lineInfo_.lineHeight);
        lineNum = i;
        if (y < currentLineBottomY) {
            isBelowLastRow = true;
            break;
        }
        begin += lineInfo_.charCountPerLine[i];
    }

    if (!isBelowLastRow) {
        begin -= lineInfo_.charCountPerLine[lineNum];
    }

    uint16_t xPos = static_cast<uint16_t>(GetContentRect().GetX());
    uint32_t index = begin;
    for (uint32_t j = begin; j < (begin + lineInfo_.charCountPerLine[lineNum]); j++) {
        if (x <= (xPos + widthOfChars_[j] / 2)) { // 2: half
            break;
        }
        xPos += widthOfChars_[j];
        index = j + 1;
    }

    cursorIndex_ = index;
    cursorView_->SetPosition(xPos, GetRelativeRect().GetY() + lineNum * lineInfo_.lineHeight);

    Invalidate();
}

void UITextPreviewBox::SetText(std::string text)
{
    widthOfChars_.clear();
    tempText_.clear();

    tempText_ = Utf8ToUtf16(text);
    UIEditText::SetText(text.c_str());
    UIFont::GetInstance()->SetCurrentFontId(inputText_->GetFontId(), inputText_->GetFontSize());

    for (uint16_t i = 0; i < tempText_.length(); i++) {
        uint16_t width = UIFont::GetInstance()->GetWidth(tempText_[i], 0);
        widthOfChars_.push_back(width);
    }

    GetLineInfo(lineInfo_);

    cursorIndex_ = widthOfChars_.size();
    HandleCursorPosByIndex();
}
} // namespace OHOS
#endif // ENABLE_SOFT_KEYBOARD
