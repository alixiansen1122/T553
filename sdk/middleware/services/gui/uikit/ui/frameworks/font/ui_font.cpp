/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "font/ui_font.h"
#include "common/text.h"
#include "font/ui_font_cache.h"
#include "graphic_config.h"
#if ENABLE_VECTOR_FONT
#if CAPABILITY_HWDRAW_LETTER_VECTOR
#include "font/ui_font_hw_vector.h"
#else
#include "font/ui_font_vector.h"
#endif
#else
#if defined(CAPABILITY_SWDRAW_LETTER_BITMAP) && CAPABILITY_SWDRAW_LETTER_BITMAP
#include "font/ui_font_bitmap_custom.h"
#else
#include "font/ui_font_bitmap.h"
#endif
#endif
#if ENABLE_MULTI_FONT
#include "font/ui_multi_font_manager.h"
#endif

namespace OHOS {
bool UIFont::setFontAllocFlag_ = false;
UIFont::UIFont() : instance_(nullptr), defaultInstance_(nullptr){};

UIFont::~UIFont(){};

UIFont* UIFont::GetInstance()
{
    static UIFont instance;

#if ENABLE_VECTOR_FONT
    if (instance.instance_ == nullptr) {
#if CAPABILITY_HWDRAW_LETTER_VECTOR
        instance.defaultInstance_ = new UIFontHWVector();
#else
        instance.defaultInstance_ = new UIFontVector();
#endif
        instance.instance_ = instance.defaultInstance_;
        setFontAllocFlag_ = true;
    }
#else
    if (instance.instance_ == nullptr) {
#if defined(CAPABILITY_SWDRAW_LETTER_BITMAP) && CAPABILITY_SWDRAW_LETTER_BITMAP
        instance.defaultInstance_ = new UIFontBitmapCustom();
#else
        instance.defaultInstance_ = new UIFontBitmap();
#endif
        instance.instance_ = instance.defaultInstance_;
        setFontAllocFlag_ = true;
    }
#endif
    return &instance;
}

void UIFont::SetFont(BaseFont* font)
{
    if (font != nullptr) {
        if (defaultInstance_ != nullptr && setFontAllocFlag_) {
            delete defaultInstance_;
            defaultInstance_ = nullptr;
            setFontAllocFlag_ = false;
        }
        defaultInstance_ = font;
        instance_ = font;
    }
}

bool UIFont::TryToFreeFontCache()
{
    if (instance_ == nullptr) {
        return false;
    }
    return instance_->TryToFreeFontCache();
}

bool UIFont::GetGlyphPathData(uint32_t unicode, uint8_t shapingFont, GlyphPathData& data)
{
    bool ret = false;
    uint8_t currentFontId = GetCurrentFontId();
#if ENABLE_MULTI_FONT
    // shaping font is in search list, search shaping font first
    if (shapingFont > 1) {
        SetCurrentFontId(shapingFont);
        ret = instance_->GetGlyphPathData(unicode, GetCurrentFontId(), data);
        SetCurrentFontId(currentFontId);
        if (ret) {
            return ret;
        }
    }
#endif
    ret = instance_->GetGlyphPathData(unicode, currentFontId, data);
    if (ret) {
        return ret;
    }
#if ENABLE_MULTI_FONT
    uint8_t* searchLists = nullptr;
    int8_t listSize = UIMultiFontManager::GetInstance()->GetSearchFontList(currentFontId, &searchLists);
    int8_t currentIndex = 0;
    if ((searchLists == nullptr) || (listSize == 0)) {
        return false;
    }

    do {
        SetCurrentFontId(searchLists[currentIndex]);
        ret = instance_->GetGlyphPathData(unicode, GetCurrentFontId(), data);
        if (ret) {
            SetCurrentFontId(currentFontId);
            return ret;
        }
        // switch to next search List
        currentIndex++;
    } while ((currentIndex < listSize) && (searchLists != nullptr));
    SetCurrentFontId(currentFontId);
#endif
    return false;
}

uint8_t* UIFont::GetBitmap(uint32_t unicode, GlyphNode& glyphNode, uint8_t shapingFont)
{
    uint8_t* bitmap = nullptr;
    uint8_t currentFontId = GetCurrentFontId();
#if ENABLE_MULTI_FONT
    // shaping font is in search list, search shaping font first
    if (shapingFont > 1) {
        bitmap = instance_->GetBitmap(unicode, glyphNode, shapingFont);
        SetCurrentFontId(currentFontId);
        if (bitmap != nullptr) {
            return bitmap;
        }
    }
#endif
    bitmap = instance_->GetBitmap(unicode, glyphNode, currentFontId);
    if (bitmap != nullptr) {
        return bitmap;
    }
#if ENABLE_MULTI_FONT
    uint8_t* searchLists = nullptr;
    int8_t listSize = UIMultiFontManager::GetInstance()->GetSearchFontList(currentFontId, &searchLists);
    int8_t currentIndex = 0;
    if ((searchLists == nullptr) || (listSize == 0)) {
        return nullptr;
    }
    do {
        SetCurrentFontId(searchLists[currentIndex], 0);
        bitmap = instance_->GetBitmap(unicode, glyphNode, GetCurrentFontId());
        if (bitmap != nullptr) {
            SetCurrentFontId(currentFontId, 0);
            return bitmap;
        }
        // switch to next search List
        currentIndex++;
    } while ((currentIndex < listSize) && (searchLists != nullptr));
    SetCurrentFontId(currentFontId);
#endif
    return nullptr;
}

bool UIFont::GetEmoji(uint32_t unicode, ImageInfo &unicodeInfo)
{
#if ENABLE_VECTOR_FONT
    UIFontVector* font = dynamic_cast<UIFontVector*>(instance_);
    if (font == nullptr) {
        return false;
    }
    return font->GetEmoji(unicode, unicodeInfo);
#else
    return false;
#endif
}

uint16_t UIFont::GetWidth(uint32_t unicode, uint8_t shapingId)
{
    int16_t result;
    uint8_t currentFontId = GetCurrentFontId();
#if ENABLE_MULTI_FONT
    if (shapingId > 1) {
        result = instance_->GetWidth(unicode, shapingId);
        SetCurrentFontId(currentFontId);
        if (result >= 0) {
            return result;
        }
    }
#endif
    result = instance_->GetWidth(unicode, currentFontId);
    if (result >= 0) {
        return result;
    }

#if ENABLE_MULTI_FONT
    uint8_t* searchLists = nullptr;
    int8_t listSize = UIMultiFontManager::GetInstance()->GetSearchFontList(currentFontId, &searchLists);
    if ((searchLists == nullptr) || (listSize == 0)) {
        return 0;
    }
    int8_t currentIndex = 0;
    do {
        SetCurrentFontId(searchLists[currentIndex], 0);
        result = instance_->GetWidth(unicode, GetCurrentFontId());
        if (result >= 0) {
            SetCurrentFontId(currentFontId, 0);
            return result;
        }
        currentIndex++;
    } while ((currentIndex < listSize) && (searchLists != nullptr));
    SetCurrentFontId(currentFontId);
#endif
    return 0;
}
} // namespace OHOS
