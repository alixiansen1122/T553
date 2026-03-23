/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */
#ifndef LV_LRU_LIST_H
#define LV_LRU_LIST_H

#include <stdbool.h>
#include "lv_conf.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct LruNode {
    struct LruNode* prev;
    struct LruNode* next;
} LvLruNode;

/**
 * @brief  Init a LvLruNode. Set prev and next to itself. Should init the node before using it.
 * @param  [in]  node    Pointer to a LvLruNode.
 * @return Returns true if success; returns false otherwise.
 */
bool LvInitLruNode(LvLruNode* node);

/**
 * @brief  Append a LvLruNode to another.
 * @param  [in]  node    Pointer to the current node.
 * @param  [in]  preNode    Pointer to the previous Node.
 * @return Returns true if success; returns false otherwise.
 */
bool LvAddLruNode(LvLruNode* node, LvLruNode* preNode);

/**
 * @brief  Delete a LvLruNode from its current list.
 * @param  [in]  node    Pointer to the current node.
 * @return Returns true if success; returns false otherwise.
 */
bool LvDelLruNode(LvLruNode* node);

/**
 * @brief  Update the list by LRU rule. The given node will be moved to the listHead's next.
 * @param  [in]  listHead    Pointer to the list head node.
 * @param  [in]  node    Pointer to the current node.
 * @return Returns true if success; returns false otherwise.
 */
bool LvUpdateLruNode(LvLruNode* listHead, LvLruNode* node);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_LRU_LIST_H */