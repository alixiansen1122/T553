/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "lv_lru_list.h"
#include <stddef.h>

bool LvInitLruNode(LvLruNode* node)
{
    if (node == NULL) {
        return false;
    }
    node->prev = node;
    node->next = node;
    return true;
}

bool LvAddLruNode(LvLruNode* node, LvLruNode* preNode)
{
    if ((node == NULL) || (preNode == NULL) || (preNode->next == NULL)) {
        return false;
    }
    preNode->next->prev = node;
    node->next = preNode->next;
    node->prev = preNode;
    preNode->next = node;
    return true;
}

bool LvDelLruNode(LvLruNode* node)
{
    if ((node == NULL) || (node->next == NULL) || (node->prev == NULL)) {
        return false;
    }
    node->next->prev = node->prev;
    node->prev->next = node->next;
    return true;
}

bool LvUpdateLruNode(LvLruNode* listHead, LvLruNode* node)
{
    if (LvDelLruNode(node) && LvInitLruNode(node) && LvAddLruNode(node, listHead)) {
        return true;
    }
    return false;
}