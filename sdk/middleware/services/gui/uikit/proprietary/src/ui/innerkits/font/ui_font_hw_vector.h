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

#ifndef UI_FONT_HW_VECTOR_H
#define UI_FONT_HW_VECTOR_H
#include <memory>
#include "font/ui_font_vector.h"
#include "graphic_config.h"
#include "freetype/freetype.h"
#include "font/ui_font_hw_cache.h"
#include "gfx_utils/vector.h"

namespace OHOS {
class UIFontHWVector : public UIFontVector {
public:
    UIFontHWVector() {}
    ~UIFontHWVector() override;
    UIFontHWVector(const UIFontHWVector&) = delete;
    UIFontHWVector& operator=(const UIFontHWVector&) noexcept = delete;
    int8_t SetCurrentFontId(uint8_t fontId, uint8_t size = 0) override;
    uint16_t GetHeight() override;
    int16_t GetWidth(uint32_t unicode, uint8_t fontId) override;
    bool GetGlyphPathData(uint32_t unicode, uint8_t fontId, GlyphPathData& data) override;
    uint8_t* GetBitmap(uint32_t unicode, GlyphNode& glyphNode, uint8_t fontId) override;
    int8_t GetCurrentFontHeader(FontHeader& fontHeader) override;
    int8_t GetGlyphNode(uint32_t unicode, GlyphNode& glyphNode) override;
    void RecordCommand(uint8_t cmd)
    {
        cmds_.PushBack(cmd);
    }
    void RecordPointData(float data)
    {
        pointData_.PushBack(data);
    }

    bool TryToFreeFontCache() override;

private:
    float size_ = 0;
    UIFontHWCache* pathCache_ = nullptr;
    struct GlyphPathCacheData {
        int16_t left;
        int16_t top;
        uint16_t width;
        uint16_t height;
        uint16_t advance;
        uint16_t cmdsNum;
        uint16_t pointsNum;
        uint16_t reserve; // used for 16 byte align
        uint8_t data[0];  // data of cmds and points
    };
    Graphic::Vector<uint8_t> cmds_;
    Graphic::Vector<float> pointData_;
    GlyphPathCacheData* LoadAndCacheGlyph(uint8_t fontId, uint32_t unicode);
};
} // namespace OHOS
#endif

