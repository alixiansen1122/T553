/*
 * @file list_impl.h的实现文件
 * Copyright (c) @CompanyNameMagicTag. 2020-2020. All rights reserved.
 * Description: 双向链表 list_impl 对外头文件。
 * Create: 2020-06-01
 */

#include "hal_list.h"

#ifdef __cplusplus
extern "C" {
#endif

void HalListInit(List *list, const DupFreeFuncPair *dataFunc)
{
    if (list == NULL) {
        return;
    }
    list->count = 0;
    list->head.next = &list->head;
    list->head.prev = &list->head;
    if (dataFunc != NULL) {
        list->dataFunc.freeFunc = dataFunc->freeFunc;
        list->dataFunc.dupFunc = dataFunc->dupFunc;
    } else {
        list->dataFunc.freeFunc = NULL;
        list->dataFunc.dupFunc = NULL;
    }
}

bool HalListEmpty(const List *list)
{
    if (list == NULL) {
        return false;
    }
    return list->head.next == &list->head;
}

size_t HalListSize(const List *list)
{
    if (list == NULL) {
        return 0;
    }
    return list->count;
}

static inline uint32_t DupUserData(const DupFunc dupFunc, ListNode *entry, const uintptr_t userData)
{
    if (dupFunc == NULL) {
        entry->userdata = userData;
    } else {
        entry->userdata = (uintptr_t)(dupFunc(userData));
    }

    return LIST_OK;
}

static inline void RawListInsert(List *list, RawListNode *curNode, RawListNode *newNode)
{
    newNode->prev = curNode->prev;
    newNode->next = curNode;
    newNode->prev->next = newNode;
    newNode->next->prev = newNode;
    list->count++;
}

uint32_t HalListPushFront(List *list, uintptr_t userData)
{
    if (list == NULL) {
        return LIST_ERROR;
    }
    ListNode *entry = (ListNode*)malloc(sizeof(ListNode));
    if (entry == NULL) {
        return LIST_ERROR;
    }

    if (DupUserData(list->dataFunc.dupFunc, entry, userData) == LIST_ERROR) {
        free(entry);
        return LIST_ERROR;
    }

    RawListInsert(list, list->head.next, &entry->rawNode);

    return LIST_OK;
}

uint32_t HalListPushBack(List *list, uintptr_t userData)
{
    if (list == NULL) {
        return LIST_ERROR;
    }
    ListNode *entry = (ListNode*)malloc(sizeof(ListNode));
    if (entry == NULL) {
        return LIST_ERROR;
    }
    if (DupUserData(list->dataFunc.dupFunc, entry, userData) == LIST_ERROR) {
        free(entry);
        return LIST_ERROR;
    }

    RawListInsert(list, &list->head, &entry->rawNode);

    return LIST_OK;
}

uintptr_t HalListFront(List *list)
{
    if (list == NULL || HalListEmpty(list)) {
        return 0; // 无法区分是值为零，还是链表为空
    }
    ListNode *node = (ListNode*)list->head.next;

    return node->userdata;
}

uintptr_t HalListBack(List *list)
{
    if (list == NULL || HalListEmpty(list)) {
        return 0; // 无法区分是值为零，还是链表为空
    }
    ListNode *node = (ListNode*)list->head.prev;

    return node->userdata;
}

uintptr_t HalListIterData(ListIterator it)
{
    if (it == NULL) {
        return 0;
    }
    return ((ListNode*)it)->userdata;
}

ListIterator HalListIterErase(List *list, ListIterator it)
{
    if (it == NULL) {
        return NULL;
    }
    uintptr_t data = HalListIterData(it);
    if (list->dataFunc.freeFunc != NULL) {
        list->dataFunc.freeFunc(data);
    }

    ListIterator tmp = it->next;
    it->prev->next = it->next;
    it->next->prev = it->prev;
    list->count--;
    free(it);

    return tmp;
}

void HalListPopFront(List *list)
{
    if (list == NULL || HalListEmpty(list)) {
        return;
    }

    (void)HalListIterErase(list, list->head.next);
}

void HalListPopBack(List *list)
{
    if (list == NULL || HalListEmpty(list)) {
        return;
    }

    (void)HalListIterErase(list, list->head.prev);
}

ListIterator HalListIterBegin(List *list)
{
    if (list == NULL) {
        return NULL;
    }
    if (HalListEmpty(list)) {
        return &list->head;
    }
    return list->head.next;
}

ListIterator HalListIterEnd(List *list)
{
    if (list == NULL) {
        return NULL;
    }
    return &list->head;
}

ListIterator HalListIterNext(const List *list, ListIterator it)
{
    if (list == NULL || it == NULL) {
        return NULL;
    }
    if (it == &list->head) {
        return it;
    }
    return it->next;
}

uint32_t HalListInsert(List *list, ListIterator it, uintptr_t userData)
{
    if (list == NULL) {
        return LIST_ERROR;
    }
    ListNode *entry = (ListNode*)malloc(sizeof(ListNode));
    if (entry == NULL) {
        return LIST_ERROR;
    }

    if (DupUserData(list->dataFunc.dupFunc, entry, userData) == LIST_ERROR) {
        free(entry);
        return LIST_ERROR;
    }

    RawListInsert(list, it, &entry->rawNode);

    return LIST_OK;
}

void HalListDeinit(List *list)
{
    if (list == NULL || HalListEmpty(list)) {
        return;
    }

    for (ListIterator it = HalListIterBegin(list); it != HalListIterEnd(list); it = HalListIterErase(list, it)) {}

    list->dataFunc.dupFunc = NULL;
    list->dataFunc.freeFunc = NULL;
}

#ifdef __cplusplus
}
#endif
