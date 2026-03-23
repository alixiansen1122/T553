/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file avl.h
 * Description: 平衡树功能相关接口
 * Author: guzhou
 * Create: 2022-02-18
 */

#ifndef AVL_H
#define AVL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *BSL_ElementData;

typedef void (*BSL_AVL_DATA_FREE_FUNC)(BSL_ElementData data);

/* AVL tree node structure */
typedef struct AvlTree {
    uint32_t height;
    uint64_t nodeId;
    struct AvlTree *rightNode;
    struct AvlTree *leftNode;
    BSL_ElementData data;
} BSL_AvlTree;

/**
 * @ingroup BSL
 * @brief 创建一个树节点
 *
 * @par 描述：
 * 创建一个树节点，并设置节点数据
 *
 * @attention 无
 * @param data [IN] 树节点的数据指针
 * @retval NULL 内存分配失败
 */
BSL_AvlTree *BSL_AVL_MakeLeafNode(BSL_ElementData data);

/**
 * @ingroup BSL
 * @brief 查找节点
 *
 * @par 描述：
 * 通过nodeId查找avl树的节点
 *
 * @attention 无
 * @param root 树的根节点指针
 * @param nodeId [IN] 作为key，树的节点id
 * @retval NULL 没有查到相应的节点
 * @retval 非NULL 查到的相应节点的指针
 */
BSL_AvlTree *BSL_AVL_SearchNode(BSL_AvlTree *root, uint64_t nodeId);

/**
 * @ingroup BSL
 * @brief 在树中创建一个节点
 *
 * @par 描述：
 * 在树中创建一个节点
 *
 * @attention 如果nodeId已经存在，则无法插入
 * @param root [IN] 树的根节点指针
 * @param nodeId [IN] 作为被创建节点的key
 * @param node [IN] 树节点
 * @retval 非NULL 树或子树的根节点
 */
BSL_AvlTree *BSL_AVL_InsertNode(BSL_AvlTree *root, uint64_t nodeId, BSL_AvlTree *node);

/**
 * @ingroup BSL
 * @brief 删除特定的树节点
 *
 * @par 描述：
 * 删除nodeId相应的树节点
 *
 * @attention 无
 * @param root [IN] 树的根节点指针
 * @param nodeId [IN] 被删节点的key
 * @param func [IN] 被删节点数据释放的函数指针
 * @retval NULL 树中的节点都已删除
 * @retval 非NULL 树或子树的根结点指针
 */
BSL_AvlTree *BSL_AVL_DeleteNode(BSL_AvlTree *root, uint64_t nodeId, BSL_AVL_DATA_FREE_FUNC func);

/**
 * @ingroup BSL
 * @brief 删除树的所有节点
 *
 * @par 描述：
 * 删除树的所有节点
 *
 * @attention 无
 * @param root [IN] 树的根节点指针
 * @param func [IN] 被删节点数据释放的函数指针
 */
void BSL_AVL_DeleteTree(BSL_AvlTree *root, BSL_AVL_DATA_FREE_FUNC func);

#ifdef __cplusplus
}
#endif

#endif