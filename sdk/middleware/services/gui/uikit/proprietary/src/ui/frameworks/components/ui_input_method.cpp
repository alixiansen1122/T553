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

#include <securec.h>
#include "common/screen.h"
#include "components/root_view.h"
#include "input_method/ui_soft_keyboard.h"
#include "input_method/ui_input_method.h"

#if ENABLE_SOFT_KEYBOARD
namespace OHOS {
enum class SYS_IMT_TYPE {
    SYS_IME_EN,
    SYS_IME_CAP,
    SYS_IME_NUMBER,
    SYS_IME_SYMBOL,
    SYS_IME_PINYIN,
    SYS_IME_ALL,
};
UIInputMethod::UIInputMethod()
{
    customSoftKeyboard_.PushBack(UIEnglishSoftKeyboard::GetInstance());
    customSoftKeyboard_.PushBack(UICapitalSoftKeyboard::GetInstance());
    customSoftKeyboard_.PushBack(UINumberSoftKeyboard::GetInstance());
    customSoftKeyboard_.PushBack(UISymbolSoftKeyboard::GetInstance());
    customSoftKeyboard_.PushBack(UIPinyinSoftKeyboard::GetInstance());
    curSoftKeyboard_ = customSoftKeyboard_[static_cast<int>(SYS_IMT_TYPE::SYS_IME_EN)];
}

UIInputMethod::~UIInputMethod()
{
    if (isShow_) {
        OnHide();
    }

    if (inputMethod_ != nullptr) {
        inputMethod_->RemoveAll();
        delete inputMethod_;
        inputMethod_ = nullptr;
    }

    if (previeBox_ != nullptr) {
        delete previeBox_;
        previeBox_ = nullptr;
    }

    if (pinyinPreview_ != nullptr) {
        delete pinyinPreview_;
        pinyinPreview_ = nullptr;
    }

    ClearPinyinTable();

    ClearHanziCandidate();

    curSoftKeyboard_ = nullptr;
    isInit_ = false;
}

UIInputMethod* UIInputMethod::GetInstance()
{
    static UIInputMethod instance;
    return &instance;
}

void UIInputMethod::OnShow(InputMethodManager::InputMethodParam& param)
{
    if (isShow_) {
        GRAPHIC_LOGD("UI system soft keyboard has been shown\n");
        return;
    }
    SetupPinyinTable();
    InitInputMethod();

    // update edit text
    edit_ = static_cast<UIEditText *>(param.view);
    if (previeBox_ != nullptr) {
        previeBox_->SetInputType(param.inputType);
        previeBox_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, UITextPreviewBox::DEFAULT_EDIT_FONT_SIZE);
        previeBox_->SetText(param.text.c_str());
        previeBox_->Focus();
        if (edit_ != nullptr) {
            previeBox_->SetStyle(STYLE_LINE_HEIGHT, edit_->GetStyle(STYLE_LINE_HEIGHT));
            previeBox_->SetMaxLength(edit_->GetMaxLength());
        }
    }

    InputMethodManager::GetInstance().DeleteBackward(param.text.length());

    // add keyboard view to root view
    RootView::GetInstance()->Add(inputMethod_);
    inputMethod_->Invalidate();
    isShow_ = true;

    // keyboard show callback
    InputMethodManager::GetInstance().OnKeyboardShow();
}

void UIInputMethod::OnHide()
{
    if (!isShow_) {
        GRAPHIC_LOGD("UI system soft keyboard not shown\n");
        return;
    }

    if (previeBox_ != nullptr) {
        previeBox_->Blur();
    }

    // remove keyboard view from root view
    RootView::GetInstance()->Remove(inputMethod_);
    RootView::GetInstance()->Invalidate();
    edit_ = nullptr;
    isShow_ = false;

    // keyboard show callback
    InputMethodManager::GetInstance().OnKeyboardHide();
}

void UIInputMethod::Input(UI_KEYCODE key)
{
    if (key >= UI_KEYCODE::UI_KEY_SOFT_KEYBOARD) {
        ChangSoftKeyboard(key);
    } else if (key >= UI_KEYCODE::UI_KEY_ENTER) {
        DealFunctionKey(key);
    } else {
        DealNormalKey(key);
    }
    RefreshUi();
}

void UIInputMethod::InputPinyin(UI_KEYCODE key)
{
    // process pinyin logic here
    if (key >= UI_KEYCODE::UI_KEY_SOFT_KEYBOARD) {
        if (curKeyboardIndex_ == static_cast<uint8_t>(SYS_IMT_TYPE::SYS_IME_PINYIN)) {
            HidePinyinPreview();
            HideHanziPageList();
        }
        ChangSoftKeyboard(key);
        return;
    }
    if (curSoftKeyboard_ == nullptr) {
        return;
    }

    switch (key) {
        case UI_KEYCODE::UI_KEY_ENTER:
            HidePinyinPreview();
            HideHanziPageList();
            DealSendKey();
            break;
        case UI_KEYCODE::UI_KEY_DEL:
            if (pinyinPreview_ && pinyinPreview_->IsVisible()) {
                DealPinyinDelKey();
            } else {
                DealDelKey();
            }
            break;
        default:
            if (key >= UI_KEYCODE::UI_KEY_a && key <= UI_KEYCODE::UI_KEY_z) {
                InsertPinyin(key);
            } else {
                DealNormalKey(key);
            }
            break;
    }
    RefreshUi();
}

void UIInputMethod::ShowHanziPageList()
{
    if (hanziList_ == nullptr || pinyinPreview_ == nullptr) {
        return;
    }
    if (pinyinPreview_->GetText() != nullptr && strlen(pinyinPreview_->GetText()) != 0) {
        hanziList_->SetVisible(true);
    }
    RefreshUi();
}

void UIInputMethod::HideHanziPageList()
{
    if (hanziList_ != nullptr) {
        hanziList_->SetVisible(false);
    }
}

uint8_t UIInputMethod::HideOptionBox()
{
    if ((optionBox_ == nullptr) || !optionBox_->IsVisible()) {
        return 0;
    }
    optionBox_->SetVisible(false);
    inputMethod_->Invalidate();
    return optionBox_->GetSelectIndex();
}

void UIInputMethod::ShowOptionBox(UIView *view, UI_KEYCODE *keys, uint8_t num)
{
    if ((optionBox_ == nullptr) || (optionBox_->IsVisible())) {
        return;
    }
    if (view != nullptr && view->GetOnDragListener() == nullptr) {
        view->SetOnDragListener(optionBox_->GetDragListener());
    }
    optionBox_->UpdateContent(keys, num);
    optionBox_->SetVisible(true);
    inputMethod_->Invalidate();
    return;
}

bool UIInputMethod::OnClick(UIView& view, const ClickEvent& event)
{
    if (previeBox_ == nullptr) {
        return false;
    }
    UILabelButton *btn = dynamic_cast<UILabelButton*>(&view);
    if (btn == nullptr) {
        return false;
    }

    std::string u8hanzi;
    const char *hanzi = btn->GetText();
    if (hanzi != nullptr) {
        u8hanzi = hanzi;
    }
    previeBox_->InsertHanzi(u8hanzi);
    HideHanziPageList();
    HidePinyinPreview();
    RefreshUi();
    return true;
}

void UIInputMethod::DealSendKey()
{
    std::string inputText = previeBox_->GetText();
    InputMethodManager::GetInstance().InsertText(inputText);
    ClearPinyinTable();
    InputMethodManager::GetInstance().HideInputMethod();
}

void UIInputMethod::DealDelKey()
{
    if (previeBox_ != nullptr) {
        previeBox_->DeleteChar();
    }
}

void UIInputMethod::DealPinyinDelKey()
{
    if (pinyinPreview_ == nullptr) {
        return;
    }
    const char *str = pinyinPreview_->GetText();
    std::string pinyin;
    if (str != nullptr && strlen(str) > 0) {
        pinyin = str;
    }
    if (pinyin.length() > 0) {
        pinyin = pinyin.substr(0, pinyin.length() - 1);
        pinyinPreview_->SetText(pinyin.c_str());
    }
    const char *text = pinyinPreview_->GetText();
    if (text == nullptr || strlen(text) == 0) {
        pinyinPreview_->SetVisible(false);
        HideHanziPageList();
        return;
    }

    // find pinyin hanzi, and display!
    UpdateHanziOption(text);
}

void UIInputMethod::HidePinyinPreview()
{
    if (pinyinPreview_ != nullptr) {
        pinyinPreview_->SetText("");
        pinyinPreview_->SetVisible(false);
    }
}

void UIInputMethod::ShowPinyinPreview()
{
    if (pinyinPreview_ != nullptr) {
        pinyinPreview_->SetVisible(true);
    }
}

void UIInputMethod::ClearHanziCandidate()
{
    if (hanziAdapter_ != nullptr) {
        delete hanziAdapter_;
        hanziAdapter_ = nullptr;
    }
    if (hanziList_ != nullptr) {
        delete hanziList_;
        hanziList_ = nullptr;
    }
    memset_s(hanziMatched_, sizeof(hanziMatched_), 0,
        sizeof(hanziMatched_));
}

void UIInputMethod::SetupHanziCandidate()
{
    if (inputMethod_ == nullptr) {
        return;
    }
    ClearHanziCandidate();
    int16_t x = 28;
    int16_t y = 200;
    int16_t w = 398;
    int16_t h = 50;

    hanziAdapter_ = new HanziAdapter;
    // set height, set width, set data
    hanziAdapter_->SetWidthAndHeight(w, h);
    hanziAdapter_->SetClickListener(this);

    hanziList_ = new UIList(UIList::HORIZONTAL);
    // set position, font, adapter, listener, and add to inputmethod
    hanziList_->SetPosition(x, y, w, h);
    hanziList_->SetStyle(STYLE_BACKGROUND_COLOR, 0xFFE8EAEE);
    hanziList_->SetIntercept(false);
    hanziList_->SetAdapter(hanziAdapter_);
    hanziList_->SetScrollStateListener(this);
    hanziList_->SetOnDragListener(this);
    hanziList_->SetVisible(false);
    inputMethod_->Add(hanziList_);
}

void UIInputMethod::DealFunctionKey(UI_KEYCODE keyCode)
{
    if (curSoftKeyboard_ != nullptr) {
        switch (keyCode) {
            case UI_KEYCODE::UI_KEY_ENTER:
                DealSendKey();
                break;
            case UI_KEYCODE::UI_KEY_DEL:
                DealDelKey();
                break;
            default:
                break;
        }
    }
}

void UIInputMethod::DealNormalKey(UI_KEYCODE keyCode)
{
    if (previeBox_ != nullptr) {
        UITextPreviewBox::TextPreviewBoxMode mode = previeBox_->GetEditMode();
        switch (mode) {
            case UITextPreviewBox::TEXT_PREVIEW_BOX_MODE_INSERT:
                InsertChar(keyCode);
                break;
            case UITextPreviewBox::TEXT_PREVIEW_BOX_MODE_REPLACE:
                DealDelKey();
                InsertChar(keyCode);
                break;
            default:
                break;
        }
    }
}

void UIInputMethod::ChangSoftKeyboard(UI_KEYCODE keyCode)
{
    switch (keyCode) {
        case UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_EN:
            curKeyboardIndex_ = static_cast<uint8_t>(SYS_IMT_TYPE::SYS_IME_EN);
            break;
        case UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_CAP:
            curKeyboardIndex_ = static_cast<uint8_t>(SYS_IMT_TYPE::SYS_IME_CAP);
            break;
        case UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_NUMBER:
            curKeyboardIndex_ = static_cast<uint8_t>(SYS_IMT_TYPE::SYS_IME_NUMBER);
            break;
        case UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_SYMBOL:
            curKeyboardIndex_ = static_cast<uint8_t>(SYS_IMT_TYPE::SYS_IME_SYMBOL);
            break;
        case UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_PINYIN:
            curKeyboardIndex_ = static_cast<uint8_t>(SYS_IMT_TYPE::SYS_IME_PINYIN);
            break;
        default:
            curKeyboardIndex_++;
            break;
    }
    curKeyboardIndex_ = (curKeyboardIndex_ > (customSoftKeyboard_.Size() - 1))
                            ? static_cast<uint8_t>(SYS_IMT_TYPE::SYS_IME_EN)
                            : curKeyboardIndex_;
    inputMethod_->Remove(curSoftKeyboard_);

    curSoftKeyboard_ = customSoftKeyboard_[curKeyboardIndex_];
    if (curSoftKeyboard_ == nullptr) {
        GRAPHIC_LOGD("current select soft keyboard is nullptr\n");
        return;
    }
    curSoftKeyboard_->InitSoftKeyboard();
    inputMethod_->Add(curSoftKeyboard_);
    inputMethod_->Invalidate();
}

void UIInputMethod::InsertChar(UI_KEYCODE keyCode)
{
    if (previeBox_ != nullptr) {
        previeBox_->InsertChar(keyCode);
    }
    return;
}

void UIInputMethod::InsertPinyin(UI_KEYCODE pinyin)
{
    if (pinyinPreview_ == nullptr) {
        return;
    }
    char ch = static_cast<char>(pinyin);
    std::string originPinyin;
    const char *str = pinyinPreview_->GetText();
    if (str != nullptr && strlen(str) > 0) {
        originPinyin = str;
    }
    originPinyin = originPinyin + ch;
    pinyinPreview_->SetText(originPinyin.c_str());
    pinyinPreview_->SetVisible(true);
    const char *text = pinyinPreview_->GetText();
    if (text == nullptr) {
        return;
    }

    // find pinyin hanzi, and display!
    UpdateHanziOption(text);
}

void UIInputMethod::UpdateHanziOption(const std::string &pinyin)
{
    if (pinyin.empty() || pinyinTable_ == nullptr) {
        return;
    }

    PinyinState res = pinyinTable_->FindHanzi(pinyin);
    if (res != PinyinState::SUCCESS) {
        GRAPHIC_LOGE("%s", pinyinTable_->DebugInfo(res).c_str());
        return;
    }

    uint8_t matchnum = pinyinTable_->GetMatchHanziNum();
    if (matchnum == 0) {
        return;
    }

    memset_s(hanziMatched_, sizeof(hanziMatched_),
        0, sizeof(hanziMatched_));
    res = pinyinTable_->GetAllHanzi(hanziMatched_,
        sizeof(hanziMatched_) / sizeof(hanziMatched_[0]));
    if (res != PinyinState::SUCCESS) {
        GRAPHIC_LOGE("%s", pinyinTable_->DebugInfo(res).c_str());
        return;
    }
    hanziAdapter_->SetData(hanziMatched_, matchnum);
    hanziList_->ScrollTo(0);
    hanziList_->SetVisible(true);
    hanziList_->RefreshList();
}

void UIInputMethod::InitInputMethod()
{
    if (isInit_) {
        return;
    }

    if (inputMethod_ == nullptr) {
        inputMethod_ = new UIViewGroup();
        inputMethod_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        inputMethod_->SetStyle(STYLE_BACKGROUND_COLOR, 0xFFF0F4FF);
        inputMethod_->SetStyle(STYLE_BACKGROUND_OPA, 0xFF);
        inputMethod_->SetStyle(STYLE_BORDER_RADIUS, 10); // 10: border radius
        inputMethod_->SetTouchable(true);
        inputMethod_->SetDraggable(true);
    }

    SetupOptionBox();
    SetupSoftKeyboard();
    SetupTextPreviewBox();
    SetupHanziCandidate();
    SetupPinyinPreviewBox();
    isInit_ = true;
}

void UIInputMethod::SetupOptionBox()
{
    if (optionBox_ == nullptr) {
        optionBox_ = new UIOptionBox();
        optionBox_->InitUIOptionBox();
        optionBox_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
        optionBox_->SetStyle(STYLE_BACKGROUND_OPA, 150); // 150: opa
        optionBox_->SetPosition(28, 200, 398, 50); // 28: x, 200: y, 398: width, 50: height
        optionBox_->SetVisible(false);
        inputMethod_->Add(optionBox_);
    }
}

void UIInputMethod::SetupSoftKeyboard()
{
    if (curSoftKeyboard_ == nullptr) {
        return;
    }

    curSoftKeyboard_->InitSoftKeyboard();
    inputMethod_->Add(curSoftKeyboard_);
}

void UIInputMethod::SetupTextPreviewBox()
{
    if (previeBox_ == nullptr) {
        previeBox_ = new UITextPreviewBox();
        previeBox_->SetPosition(100, 40, 254, 150); // 100: x, 40: y, 254: width, 150: height
        previeBox_->SetStyle(STYLE_BACKGROUND_COLOR, 0xFFFFFFFF);
        previeBox_->SetStyle(STYLE_BORDER_RADIUS, 10); // 10: border radius
        previeBox_->SetText("");
        previeBox_->SetTouchable(true);
        previeBox_->SetTextColor(Color::Black());
        previeBox_->SetViewId("input_edit_text_view_id");
        inputMethod_->Add(previeBox_);
    }
}

void UIInputMethod::SetupPinyinPreviewBox()
{
    if (pinyinPreview_ != nullptr) {
        return;
    }
    pinyinPreview_ = new UILabel;
    int16_t x = 28;
    int16_t y = 160;
    int16_t w = 77;
    int16_t h = 30;
    pinyinPreview_->SetPosition(x, y, w, h);
    pinyinPreview_->SetStyle(STYLE_BACKGROUND_COLOR, 0xFFE8EAEE);
    pinyinPreview_->SetStyle(STYLE_BORDER_RADIUS, 10); // 10: border radius
    pinyinPreview_->SetText("");
    pinyinPreview_->SetTouchable(false);
    pinyinPreview_->SetTextColor(Color::Black());
    pinyinPreview_->SetViewId("input_pinyin_text_view_id");
    pinyinPreview_->SetVisible(false);

    inputMethod_->Add(pinyinPreview_);
}

void UIInputMethod::SetupPinyinTable()
{
    if (pinyinTable_ != nullptr) {
        return;
    }
    pinyinTable_ = new PinyinCandidate();
    if (pinyinTable_ == nullptr) {
        return;
    }

#ifndef WIN32
    PinyinState state = pinyinTable_->Init("/user/res/pymb.bin");
#else
    PinyinState state = pinyinTable_->Init(RES_PATH"/pymb.bin");
#endif
    if (state != PinyinState::SUCCESS) {
        ClearPinyinTable();
    }
}

void UIInputMethod::ClearPinyinTable()
{
    if (pinyinTable_ != nullptr) {
        delete pinyinTable_;
        pinyinTable_ = nullptr;
    }
}

UIView* UIInputMethod::HanziAdapter::GetView(UIView* inView, int16_t index)
{
    if (data_ == nullptr || size_ == 0) {
        return nullptr;
    }
    if (index < 0 || index >= static_cast<int16_t>(size_)) {
        return nullptr;
    }

    UILabelButton* item = dynamic_cast<UILabelButton*>(inView);
    if (item == nullptr) {
        int16_t gap = 2;
        item  = new UILabelButton;
        item->SetPosition(0, 0);
        item->Resize(GetItemWidthWithMargin(index) - gap, GetItemHeightWithMargin(index));
    }
    item->SetOnClickListener(clickListener_);
    item->SetText(reinterpret_cast<char*>(data_[index]));
    item->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 40); // 40: means font size
    item->SetViewIndex(index);
    item->SetAlign(TEXT_ALIGNMENT_CENTER);
    item->SetStyleForState(STYLE_BORDER_RADIUS, 0, UIButton::RELEASED);
    item->SetStyleForState(STYLE_BORDER_RADIUS, 0, UIButton::PRESSED);
    item->SetStyleForState(STYLE_BORDER_RADIUS, 0, UIButton::INACTIVE);
    item->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::RELEASED);
    item->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    item->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::INACTIVE);
    item->SetTextColor(Color::Black());
    return item;
}

uint16_t UIInputMethod::HanziAdapter::GetCount()
{
    return size_;
}

int16_t UIInputMethod::HanziAdapter::GetItemWidthWithMargin(int16_t index)
{
    return width_ * 1.0 / UIInputMethod::DEFAULT_NUMPERPAGE;
}

int16_t UIInputMethod::HanziAdapter::GetItemHeightWithMargin(int16_t index)
{
    return height_;
}

bool UIInputMethod::HanziAdapter::SetWidthAndHeight(int16_t w, int16_t h)
{
    if (w > 0 && h > 0) {
        width_ = w;
        height_ = h;
        return true;
    }
    return false;
}

void UIInputMethod::HanziAdapter::SetData(int8_t hanzi[][PinyinCandidate::IME_HZLENTH], uint8_t len)
{
    data_ = hanzi;
    size_ = len;
}
} // namespace OHOS
#endif // ENABLE_SOFT_KEYBOARD
