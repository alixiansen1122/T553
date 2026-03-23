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

#ifndef UI_SHAPING_CACHE_H
#define UI_SHAPING_CACHE_H

#include "font/ui_font_allocator.h"
#include "graphic_config.h"

#if ENABLE_SHAPING
namespace OHOS {
class UIShapingCache {
public:
    static constexpr uint8_t SHAPING_CACHE_MAX_SIZE = 128;

    struct ListHead {
        ListHead* prev;
        ListHead* next;
    };

    struct ShapingCacheEntry {
        ListHead hashHead;
        ListHead lruHead;
        uint32_t hashKey;
        uint8_t ttfId;
        uint16_t* key;
        uint32_t keyLen;
        uint32_t* value;
        uint32_t valueLen;
    };

    UIShapingCache(uint8_t* ram, uint32_t len);
    ~UIShapingCache();

    uint32_t Find(uint8_t ttfId, const uint16_t* key, uint32_t keyLen, uint32_t*& value);
    bool Cache(uint8_t ttfId, const uint16_t* key, uint32_t keyLen, const uint32_t* value, uint32_t valueLen);

private:
    bool CompareEntry(const ShapingCacheEntry* entry, uint32_t hashKey,
        uint8_t ttfId, const uint16_t* key, uint32_t keyLen) const;
    uint32_t GetHashKey(const uint16_t* key, uint16_t keyLen) const;

    void UpdateLru(ShapingCacheEntry* entry)
    {
        ListDel(entry->lruHead);
        ListInit(entry->lruHead);
        ListAdd(entry->lruHead, lruList_);
    }

    void ListInit(ListHead& head) const
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

    UIFontAllocator allocator_;
    ListHead* hashTable_ = nullptr;
    ListHead lruList_ = {};
};
} // namespace OHOS
#endif
#endif /* UI_SHAPING_CACHE_H */