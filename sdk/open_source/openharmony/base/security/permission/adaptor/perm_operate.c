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

#include "perm_operate.h"

#include <stdlib.h>
#include <string.h>
#include "mem_util.h"

#define RET_OK 0
#define RET_NOK (-1)

int PermissionExists(TList *list, int taskID, const char *permission)
{
    TNode **cur = &list->head;
    while (*cur) {
        if ((*cur)->taskID != taskID) {
            cur = &(*cur)->next;
            continue;
        }
        for (int i = 0; i < (*cur)->permNum; i++) {
            if (strcmp(((PermissionSaved *)(*cur)->permList + i)->name, permission) == 0) {
                return (int)((PermissionSaved *)(*cur)->permList + i)->granted;
            }
        }
        return RET_NOK;
    }
    return RET_NOK;
}

int ModifyPermission(TList *list, int taskID, const char *permission, enum IsGranted granted)
{
    TNode **cur = &list->head;
    while (*cur) {
        if ((*cur)->taskID != taskID) {
            cur = &(*cur)->next;
            continue;
        }
        for (int i = 0; i < (*cur)->permNum; i++) {
            if (strcmp(((PermissionSaved *)(*cur)->permList + i)->name, permission) == 0) {
                ((PermissionSaved *)(*cur)->permList + i)->granted = granted;
                return RET_OK;
            }
        }
        return RET_NOK;
    }
    return RET_NOK;
}

void AddTask(TList *list, TNode *node)
{
    if (list->head == NULL) {
        list->head = node;
    } else {
        node->next = list->head;
        list->head = node;
    }
}

void DeleteTask(TList *list, int taskID)
{
    TNode **cur = &list->head;
    while (*cur) {
        TNode *entry = *cur;
        if (entry->taskID == taskID) {
            *cur = entry->next;
            if (entry->permList != NULL) {
                PSRAM_Free(entry->permList);
                entry->permList = NULL;
            }
            PSRAM_Free(entry);
            entry = NULL;
        } else {
            cur = &entry->next;
        }
    }
}

TNode *GetTask(TList *list, int taskID)
{
    TNode **cur = &list->head;
    while (*cur) {
        TNode *entry = *cur;
        if (entry->taskID == taskID) {
            return entry;
        } else {
            cur = &entry->next;
        }
    }
    return NULL;
}