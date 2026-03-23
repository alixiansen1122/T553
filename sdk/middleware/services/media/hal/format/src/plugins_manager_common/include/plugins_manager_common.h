/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: plugin manager common interfaces
* Author: Media Software Group
* Create: 2021-04-21
*/


#ifndef PLUGINS_MANAGER_COMMON_H
#define PLUGINS_MANAGER_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include "plugin_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct tafListNode {
    struct tafListNode *prev;
    struct tafListNode *next;
} ListNode;

typedef struct {
    ListNode list;
    const PluginDesc *plugin;
    void *module;
} PluginEntry;

typedef PluginEntry *(*GetPluginEntry)(const char *name, int32_t index);
typedef void (*PluginDeleteNode)(PluginEntry *node);

PluginEntry *PluginFillNode(const PluginDesc *entry, const void *module);
void PluginScanAllNodes(PluginEntry **list, int32_t *nodeNum, bool dynamicLoad, GetPluginEntry getEntry);
void PluginRemoveAllNodes(PluginEntry *list, PluginDeleteNode pluginDeleteNode);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  // PLUGINS_MANAGER_COMMON_H
