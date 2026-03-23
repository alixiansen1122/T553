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

#ifndef FONT_GLOBAL_INFO_H
#define FONT_GLOBAL_INFO_H

#include "graphic_config.h"
#if ENABLE_FONT_VECTOR_GLOBAL
namespace OHOS {
struct FontParam;

class LangBinInfo : public HeapBase {
public:
    LangBinInfo();
    ~LangBinInfo();
    char* GetText(uint16_t textId);
    void GetFontParam(uint16_t textId, FontParam& prm);
    bool Init(const char* fileName);

    static constexpr uint32_t FONT_SIZE_MASK = 0xFF000000;
    static constexpr uint32_t FONT_SIZE_BITS_OFFSET = 24;
    static constexpr uint32_t FONT_STR_MASK = 0x000FFFFF;
    static constexpr uint32_t FONTNAME_FLAG_MASK = 0x00800000;
    static constexpr uint32_t FONT_DIRECT_MASK = 0x00600000;
    static constexpr uint32_t FONT_DIRECT_OFFSET = 21;
    static constexpr uint16_t TOTAL_NUM_MASK = 0x7FFF;
    static constexpr uint16_t DEFAULT_FONTNAME_MASK = 0x8000;

private:
    uint8_t* binHead_ = nullptr;
    uint16_t totalStrsNum_ = 0;
    uint32_t* strOffsets_ = nullptr;
    char* strData_ = nullptr;
    bool hasDefaultFontName : true;
};

class StringsInfo : public HeapBase {
public:
    StringsInfo();
    ~StringsInfo();
    StringsInfo(const StringsInfo &) = delete;
    StringsInfo &operator=(const StringsInfo &) = delete;
    LangBinInfo* GetBinInfo();
    bool SetFileName(const char* fileName);
    bool Load();
    void UnLoad();

private:
    char* fileName_ = nullptr;
    LangBinInfo* binInfo_ = nullptr;
};
}
#endif // ENABLE_FONT_VECTOR_GLOBAL
#endif // FONT_GLOBAL_INFO_H