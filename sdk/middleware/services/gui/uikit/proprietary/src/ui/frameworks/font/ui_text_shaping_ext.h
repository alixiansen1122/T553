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

#ifndef GRAPHIC_LITE_TEXT_SHAPING_EXT_H
#define GRAPHIC_LITE_TEXT_SHAPING_EXT_H

#include "graphic_config.h"
#if ENABLE_SHAPING
#include "draw/draw_utils.h"
#include "font/ui_font_header.h"
#include "font/ui_line_break.h"
#include "gfx_utils/file.h"
#include "gfx_utils/geometry2d.h"
#include "gfx_utils/graphic_log.h"
#include "graphic_config.h"
#include "font/ui_shaping_cache.h"
#include "font/ui_text_shaping.h"

namespace OHOS {
constexpr uint8_t SHIFT_TTF_ID = 24;
constexpr uint8_t SHIFT_X_OFFSET = 18;
constexpr uint8_t SHIFT_Y_OFFSET = 12;

constexpr uint32_t TEXT_MASK_DIRECT_RTL = 0x80000000; // the 31bit means direction RTL
constexpr uint32_t TEXT_MASK_BREAK = 0x40000000; // the 30bit means BREAK.
constexpr uint32_t TEXT_MASK_TTF_ID = 0x1F000000;   // the 24~29 bit means the ttfId.
constexpr uint32_t TEXT_MASK_X_DIRECT = 0x00800000; // the 23bit means X_DIRECT. 1 means negative offset.
constexpr uint32_t TEXT_MASK_X_OFFSET = 0x007C0000; // the 18~22 bit means X_OFFSET
constexpr uint32_t TEXT_MASK_Y_DIRECT = 0x00020000; // the 17 bit means Y_DIRECT. 1 means negative offset.
constexpr uint32_t TEXT_MASK_Y_OFFSET = 0x0001F000; // the 12~16 bit means Y_OFFSET

class UITextShapingExt {
public:
    UITextShapingExt(const UITextShapingExt &) = delete;
    UITextShapingExt &operator=(const UITextShapingExt &) = delete;
    UITextShapingExt();
    ~UITextShapingExt();
    bool SetPsramMemory(uintptr_t psramAddr, uint32_t psramLen);
    void DrawTextOneLine(BufferInfo& gfxDstBuffer, const LabelLineInfo& labelLine);
    uint16_t Shape(uint8_t ttfId, UITextLanguageDirect direct,
        const uint16_t* str, uint32_t len, uint32_t*& codePoints);
    Point GetTextSize(const uint32_t* codePoints, uint16_t codePointsNum,
        int16_t letterSpace, int16_t lineSpace, int16_t maxWidth, uint8_t shapingId);
    uint32_t GetNextLineAndWidth(const uint32_t* codePoints, uint32_t codePointsNum,
        int16_t space, uint8_t shapingId, int16_t& maxWidth);

    static UITextShapingExt* GetInstance()
    {
        static UITextShapingExt instance;
        return &instance;
    }

    inline uint8_t GetTTFId(uint32_t code)
    {
        return ((code & TEXT_MASK_TTF_ID) >> SHIFT_TTF_ID);
    }

    inline UITextLanguageDirect GetDirect(uint32_t code)
    {
        if ((code & TEXT_MASK_DIRECT_RTL) >> SHIFT_DIRECT_RTL) {
            return TEXT_DIRECT_RTL;
        } else {
            return TEXT_DIRECT_LTR;
        }
    }

    inline bool IsBreak(uint32_t code)
    {
        if ((code & TEXT_MASK_BREAK) != 0) {
            return true;
        } else {
            return false;
        }
    }

    bool SetShapingScript(ShapingScript script)
    {
        if (script == SHAPING_SCRIPT_INVALID) {
            return false;
        }
        script_ = script;
        return true;
    }

    inline int8_t GetXOffset(uint32_t code, uint8_t fontSize)
    {
        if (GetTTFId(code) == INVALID_SHAPING_TTF_ID) {
            return 0;
        }
        int8_t xOffset = (((code >> SHIFT_X_OFFSET) & 0x1F) * fontSize) >> FONT_SIZE_SHIFT;
        if (code & TEXT_MASK_X_DIRECT) {
            return -xOffset;
        } else {
            return xOffset;
        }
    }

    inline int8_t GetYOffset(uint32_t code, uint8_t fontSize)
    {
    if (GetTTFId(code) == INVALID_SHAPING_TTF_ID) {
            return 0;
        }
        int8_t yOffset = (((code >> SHIFT_Y_OFFSET) & 0x1F) * fontSize) >> FONT_SIZE_SHIFT;
        if (code & TEXT_MASK_Y_OFFSET) {
            return -yOffset;
        } else {
            return yOffset;
        }
    }

    inline uint32_t ClearMask(uint32_t code)
    {
        uint32_t ret = code & (~TEXT_MASK_DIRECT_RTL) & (~TEXT_MASK_BREAK);
        if (GetTTFId(code) != INVALID_SHAPING_TTF_ID) {
            ret = ret & (~TEXT_SHAPE_MASK);
        }
        return ret;
    }

private:
    uintptr_t ttfDataAddr_;
    TtfHeader ttfHeader_;
    ShapingScript script_;
    UIShapingCache* shapingCache_;

    static constexpr uint8_t INVALID_SHAPING_TTF_ID = 0;
    static constexpr uint8_t SHIFT_DIRECT_RTL = 31;
    static constexpr uint8_t FONT_SIZE_SHIFT = 5;
    static constexpr uint32_t TEXT_SHAPE_MASK = TEXT_MASK_X_DIRECT | TEXT_MASK_X_OFFSET |
        TEXT_MASK_Y_DIRECT | TEXT_MASK_Y_OFFSET;

    bool ReadTTFDataToPsram(uint8_t ttfId);
    void AdjustDirect(UITextLanguageDirect direct, uint32_t* codes, uint32_t len);
    uint32_t HBShape(uint8_t ttfId, uint8_t direct,
        const uint16_t* text, uint32_t len, uint32_t*& codePoints);
    uint32_t BidiShape(uint8_t ttfId, UITextLanguageDirect direct, const uint16_t* str, uint32_t len,
        uint32_t*& codePoints);
}; // class UITextShapingExt
} // namespace OHOS
#endif // ENABLE_SHAPING
#endif // GRAPHIC_LITE_TEXT_SHAPING_EXT_H