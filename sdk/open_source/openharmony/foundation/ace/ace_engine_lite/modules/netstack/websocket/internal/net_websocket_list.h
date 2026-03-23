/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef __NET_WS_LIST_H__
#define __NET_WS_LIST_H__

#include <stdio.h>

typedef union WsNodeValueUnion {
    void *object;
    char *string;
} WsNodeValue;

typedef struct WsNodeStruct {
    WsNodeValue value;
    struct WsNodeStruct *next;
} WsNode, WsList;

WsList *WsListCreate(void);

void WsListDelete(WsList *list);

void WsListDeleteClean(WsList **list);

int WsListAppend(WsList *list, WsNodeValue value);

#endif
