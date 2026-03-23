/*
 * Copyright (c) @CompanyNameMagicTag. 2017-2019. All rights reserved.
 * Description: plugins manager common
 * Author: Media Software Group
 * Create: 2017-12-22
 */

#include "plugins_manager_common.h"

#include <sys/time.h>
#ifdef SUPPORT_DL
#include <dirent.h>
#endif
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "securec.h"
#include "media_hal_common.h"
#include "demuxer_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define MODULE_NAME "PluginManagerCommon"
#define BASE_PATH "/usr/lib/"
#define DNAME_TYPE_FILE 8
#define PATH_LEN 1024

static void AddNode(ListNode *nodeA, ListNode *nodeB)
{
    if (nodeA == NULL || nodeB == NULL) {
        return;
    }
    ListNode *nodeNext = nodeA->next;
    nodeB->prev = nodeA;
    nodeB->next = nodeNext;
    nodeA->next = nodeB;
    if (nodeNext != NULL) {
        nodeNext->prev = nodeB;
    }
}

static bool CheckPluginExist(const PluginEntry *list, const PluginEntry *node)
{
    bool exist = false;
    const PluginEntry *nodeTmp = list;
    CHK_NULL_RETURN_NO_LOG(node, false);
    while (nodeTmp != NULL) {
        if (nodeTmp->plugin == node->plugin) {
            exist = true;
            break;
        }
        nodeTmp = (const PluginEntry *)nodeTmp->list.next;
    }
    return exist;
}

PluginEntry *PluginFillNode(const PluginDesc *entry, const void *module)
{
    PluginEntry *node = (PluginEntry *)malloc(sizeof(PluginEntry));
    CHK_NULL_RETURN(node, NULL, "malloc failed");
    node->list.prev = NULL;
    node->list.next = NULL;
    node->plugin = entry;
    node->module = (void *)module;
    return node;
}

static int32_t AddPluginNode(PluginEntry **list, PluginEntry *node)
{
    if (CheckPluginExist(*list, node)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "CheckPluginExist failed :%s!", node->plugin->libName);
        return -1;
    }
    if ((*list) == NULL) {
        node->list.prev = node->list.next = NULL;
        (*list) = node;
        return 0;
    }
    PluginEntry *entryTmp = *list;
    while (entryTmp != NULL) {
        if (entryTmp->plugin != NULL && node->plugin != NULL && entryTmp->plugin->priority < node->plugin->priority) {
            entryTmp = (PluginEntry *)entryTmp->list.prev;
            break;
        }
        if (entryTmp->list.next == NULL) {
            break;
        }
        entryTmp = (PluginEntry *)entryTmp->list.next;
    }
    if (entryTmp == NULL) {
        entryTmp = (*list);
        (*list) = node;
        node->list.prev = NULL;
        node->list.next = (ListNode *)entryTmp;
        entryTmp->list.prev = (ListNode *)node;
        return 0;
    }
    /* 当前节点添加 */
    AddNode((ListNode *)entryTmp, (ListNode *)node);
    return 0;
}

#ifdef SUPPORT_DL
static int32_t CheckAndAddNode(const struct dirent *ptr, PluginEntry **list, GetPluginEntry getEntry)
{
    const char *name = ptr->d_name;
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
        return -1;
    }
    if (ptr->d_type != DNAME_TYPE_FILE) {
        return -1;
    }
    char path[PATH_LEN];
    if (strcpy_s(path, PATH_LEN, BASE_PATH) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "strcpy_s failed");
        return -1;
    }
    if (strcat_s(path, PATH_LEN, name) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "strcat_s failed");
        return -1;
    }
    PluginEntry *node = getEntry(path, 0);
    if (node == NULL) {
        return -1;
    }
    if (AddPluginNode(list, node) != 0) {
        free(node);
        node = NULL;
        return -1;
    }
    return 0;
}

static void DynamicLoadAllNodes(PluginEntry **list, int32_t *nodeNum, GetPluginEntry getEntry)
{
    DIR *dir = NULL;
    struct dirent *ptr = NULL;
    if ((dir = opendir(BASE_PATH)) == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "Open lib dir error: %s", BASE_PATH);
        return;
    }
    while ((ptr = readdir(dir)) != NULL) {
        if (CheckAndAddNode(ptr, list, getEntry) == 0) {
            (*nodeNum)++;
        }
    }
    closedir(dir);
}
#endif

static void StaticLoadAllNodes(PluginEntry **list, int32_t *nodeNum, GetPluginEntry getEntry)
{
    int32_t i = 0;
    PluginEntry *node = getEntry(NULL, i);
    while (node != NULL) {
        if (AddPluginNode(list, node) != 0) {
            free(node);
            node = NULL;
            return;
        }
        i++;
        (*nodeNum)++;
        node = getEntry(NULL, i);
    }
}

void PluginScanAllNodes(PluginEntry **list, int32_t *nodeNum, bool dynamicLoad, GetPluginEntry getEntry)
{
    if (list == NULL || nodeNum == NULL || getEntry == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "input param error");
        return;
    }
    if (dynamicLoad) {
#ifdef SUPPORT_DL
        return DynamicLoadAllNodes(list, nodeNum, getEntry);
#else
        return;
#endif
    } else {
        return StaticLoadAllNodes(list, nodeNum, getEntry);
    }
}

void PluginRemoveAllNodes(PluginEntry *list, PluginDeleteNode pluginDeleteNode)
{
    PluginEntry *node = NULL;
    PluginEntry *entryTmp = list;
    while (entryTmp != NULL) {
        node = entryTmp;
        entryTmp = (PluginEntry *)node->list.next;
        node->list.prev = NULL;
        node->list.next = NULL;
        if (pluginDeleteNode != NULL) {
            pluginDeleteNode(node);
        }
        node = NULL;
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
