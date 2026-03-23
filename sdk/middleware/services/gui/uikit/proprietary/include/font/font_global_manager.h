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

#ifndef FONT_GLOBAL_MANAGER_H
#define FONT_GLOBAL_MANAGER_H
#include <cstdint>
#include "common/text.h"
#include "components/ui_view.h"
#include "font/font_global_header.h"
#include "graphic_config.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/heap_base.h"
#if ENABLE_FONT_VECTOR_GLOBAL
namespace OHOS {
struct FontParam {
    uint8_t fontSize;
    uint8_t direct;
    char* fontName;
};

class FontGlobalManager : public HeapBase {
public:
    class LangChangeListener {
    public:
        virtual void OnLangChange(uint8_t langId) = 0;
        virtual ~LangChangeListener() {}
    };
    char* GetText(uint16_t textId);
    bool RegisterLanguageInfo(const char** resFile, uint8_t totalLangId, uint8_t defaultLangId);
    void UnRegisterLanguageInfo();
    void SetCurrentLangId(uint8_t langId);
    uint8_t GetCurrentLangId() const
    {
        return currentLangId_;
    }
    FontParam GetFontParam(uint16_t textId);
    uint8_t GetTotalLangId();
    static FontGlobalManager* GetInstance();
    static constexpr uint16_t INVALID_TEXT_ID = 0xFFFF;
    static constexpr uint8_t INVALID_LANG_ID = 0xFF;

private:
    void RefreshViews(UIView* rootView);
    FontGlobalManager() {};
    ~FontGlobalManager();
    FontGlobalManager(const FontGlobalManager &) = delete;
    FontGlobalManager &operator=(const FontGlobalManager &) = delete;
    LangBinInfo* GetLangBinInfo();
    bool ReallocStringInfos(uint8_t totalLangId);
    StringsInfo* stringsInfos_ = nullptr;
    uint8_t currentLangId_ = INVALID_LANG_ID; // 0xFF: the max lang id
    uint8_t totalLangId_ = 0;
};
} // namespace OHOS
#endif // ENABLE_FONT_VECTOR_GLOBAL

#endif // _FONT_GLOBAL_MANAGER_H_
