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

/**
 * @addtogroup UI_Common
 * @{
 *
 * @brief Defines common UI capabilities, such as image and text processing.
 *
 */

/**
 * @file text_ext.h
 *
 * @brief Declares the <b>TextExt</b> class of the graphics module, which extends text ability.
 *
 */

#ifndef GRAPHIC_LITE_TEXT_EXT_H
#define GRAPHIC_LITE_TEXT_EXT_H
#include "font/font_global_manager.h"
#include "graphic_config.h"
#include "common/text.h"

#if ENABLE_FONT_VECTOR_GLOBAL || ENABLE_SHAPING
namespace OHOS {
/**
 *
 * @brief Declares the <b>TextExt</b> class of the graphics module, which extends text ability.
 *
 */
class TextExt : public Text {
public:
    /**
    * @brief Construct a new Text Ext object
    *
    */
    TextExt() {}

    /**
     * @brief Destroy the Text Ext object
     *
     */
    ~TextExt() override;

    void SetText(const char* text) override;

#if ENABLE_FONT_VECTOR_GLOBAL
    /**
     * @brief Set the text string by text id.
     *
     * @param textId Index of text array.
     */
    void SetTextId(uint16_t textId);

    /**
     * @brief Reset text string by text id.
     *
     * @param textId Index of text array.
     */
    void ForceResetText(uint16_t textId = FontGlobalManager::INVALID_TEXT_ID);

#endif
#if ENABLE_SHAPING
    /**
     * @brief Remeasure text rect.
     *
     * @param textRect Rect that need to calculate from text string.
     * @param style Text view style.
     */
    void ReMeasureTextSize(const Rect& textRect, const Style& style) override;
#endif

protected:
#if ENABLE_SHAPING
    uint16_t GetLetterIndexByPosition(const Rect& textRect, const Style& style, const Point& pos) override;
    uint32_t GetTextStrLen() override;
    uint32_t GetTextLine(uint32_t begin, uint32_t textLen, int16_t width,
        uint16_t lineNum, uint8_t letterSpace) override;
    void Draw(BufferInfo& gfxDstBuffer, const Rect& mask, const Rect& coords, const Style& style,
        int16_t offsetX, uint16_t ellipsisIndex, OpacityType opaScale) override;
    uint16_t GetLine(int16_t width, uint8_t letterSpace, uint16_t ellipsisIndex, uint32_t& maxLineBytes) override;
#endif

private:
    void SetTextInnner(const char* text);
    uint16_t textId_ = FontGlobalManager::INVALID_TEXT_ID;
#if ENABLE_SHAPING
    bool isNeedShaping_ = 0;
    uint8_t shapingFontId_ = 0;
    uint32_t* codePoints_ = nullptr;
    uint16_t codePointsLen_ = 0;
#endif
};
} // namespace OHOS
#endif // ENABLE_FONT_VECTOR_GLOBAL || ENABLE_SHAPING
#endif // GRAPHIC_LITE_TEXT_EXT_H
/**
 * @}
 */
