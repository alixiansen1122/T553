/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */
#ifndef LV_HASH_MAP_H
#define LV_HASH_MAP_H

#include <stdbool.h>
#include "lv_conf.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct KeyValue {
    struct KeyValue* next;
    char* key;
    void* value;
} LvKeyValue;

typedef struct {
    LvKeyValue** table;
} LvHashTable;

typedef uint32_t(*HashGetResultFunc)(void* info, void* param);

LvHashTable* LvHashInit(uint32_t maxSize);
void LvHashDeinit(LvHashTable* ht);

bool LvHashInsert(LvHashTable* ht, const char* key, void* value);
void LvHashDelete(LvHashTable* ht, const char* key);
void* LvHashSearch(LvHashTable* ht, const char* key);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // LV_HASH_MAP_H
