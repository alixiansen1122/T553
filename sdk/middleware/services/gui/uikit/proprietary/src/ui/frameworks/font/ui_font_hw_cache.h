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

#ifndef UI_FONT_HW_CACHE_H
#define UI_FONT_HW_CACHE_H

#include <unordered_map>
#include <vector>
#include "graphic_config.h"
#include "font/ui_font_allocator.h"

namespace OHOS {
class UIFontHWCache {
public:
    struct ListHead {
        ListHead* prev;
        ListHead* next;
    };
    struct CacheData {
        ListHead lruHead;
        uint8_t fontId;
        uint32_t unicode;
        uint8_t data[0];
    };

    UIFontHWCache();

    ~UIFontHWCache();

    uint8_t* GetSpace(uint8_t fontId, uint32_t unicode, uint32_t size);

    void PutSpace(uint8_t* addr);

    uint8_t* GetCacheData(uint8_t fontId, uint32_t unicode);

    bool TryToFreeFontCache();

private:
    struct FontInfo {
        uint8_t fontId;
        std::unordered_map<uint32_t, void*> caches;
    };

    void ListInit(ListHead& head)
    {
        head.prev = &head;
        head.next = &head;
    }
    void ListAdd(ListHead& node, ListHead& head) const
    {
        head.next->prev = &node;
        node.next = head.next;
        node.prev = &head;
        head.next = &node;
    }
    void ListDel(ListHead& node) const
    {
        node.next->prev = node.prev;
        node.prev->next = node.next;
    }
    FontInfo* SearchFontInfo(uint8_t fontId);

    std::vector<FontInfo*> fontInfos_;
    ListHead lruList_;
};
} // namespace OHOS
#endif /* UI_FONT_CACHE_H */