/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: 平衡二叉树的实现
 * Author: guzhou
 * Create: 2022-02-18
 */

#include <inttypes.h>
#include "sal_mem.h"
#include "bsl_log_internal.h"
#include "bsl_binlog_id.h"
#include "avl.h"

// avl树的最大高度
#define AVL_MAX_HEIGHT 64

static uint32_t GetMaxHeight(uint32_t a, uint32_t b)
{
    if (a >= b) {
        return a;
    } else {
        return b;
    }
}

static uint32_t GetAvlTreeHeight(const BSL_AvlTree *node)
{
    if (node == NULL) {
        return 0;
    } else {
        return node->height;
    }
}

static void UpdateAvlTreeHeight(BSL_AvlTree *node)
{
    if (node != NULL) {
        uint32_t leftHeight = GetAvlTreeHeight(node->leftNode);
        uint32_t rightHeight = GetAvlTreeHeight(node->rightNode);
        if (node->height >= AVL_MAX_HEIGHT) {
            LOG_BINLOG_FIXLEN(BINLOG_ID05001, BSL_LOG_LEVEL_ERR, BSL_LOG_BINLOG_TYPE_RUN,
                "avl tree height exceed max limit", NULL, NULL, NULL, NULL);
            return;
        }
        node->height = GetMaxHeight(leftHeight, rightHeight) + 1u;
    }
}

/**
 * @brief   创建avl节点
 * @param   nodeId [IN]节点ID
 *          data   [IN]节点存储的数据
 * @return  curNode 申请成功返回节点
 *          NULL    申请失败
 */
BSL_AvlTree *BSL_AVL_MakeLeafNode(BSL_ElementData data)
{
    BSL_AvlTree *curNode = (BSL_AvlTree *)SAL_MALLOC(sizeof(BSL_AvlTree));
    if (curNode == NULL) {
        LOG_BINLOG_FIXLEN(BINLOG_ID05002, BSL_LOG_LEVEL_ERR, BSL_LOG_BINLOG_TYPE_RUN,
            "MALLOC for avl tree node failed", NULL, NULL, NULL, NULL);
        return NULL;
    }

    curNode->height = 1;
    curNode->rightNode = NULL;
    curNode->leftNode = NULL;
    curNode->data = data;

    return curNode;
}

/**
 * @brief   AVL左旋转
 * @param   root [IN] 待旋转的根节点
 * @return  rNode 旋转后的根节点
 */
static BSL_AvlTree *AVL_RotateLeft(BSL_AvlTree *root)
{
    /* Rotate Left
                        10                              20
                      5    20    --Rotate Left--->    10  30
                             30                      5      40
                              40

    此场景下输入根节点为10, 输出为20 */
    BSL_AvlTree *rNode = root->rightNode;
    BSL_AvlTree *lNode = rNode->leftNode;
    root->rightNode = lNode;
    rNode->leftNode = root;
    UpdateAvlTreeHeight(root);
    UpdateAvlTreeHeight(rNode);
    return rNode;
}

/**
 * @brief   AVL右旋转
 * @param   root [IN] 待旋转的根节点
 * @return  lNode 旋转后的根节点
 */
static BSL_AvlTree *AVL_RotateRight(BSL_AvlTree *root)
{
    /* Rotate Right
                        40                              30
                       /  \                            /  \
                     30    50   --Rotate Right--->   20    40
                   20  35                          10    35  50
                 10
    此场景下输入根节点为40, 输出为30 */
    BSL_AvlTree *lNode = root->leftNode;
    BSL_AvlTree *rNode = lNode->rightNode;
    root->leftNode = rNode;
    lNode->rightNode = root;
    UpdateAvlTreeHeight(root);
    UpdateAvlTreeHeight(lNode);
    return lNode;
}

/**
 * @brief   AVL右平衡
 * @param   root [IN] 待平衡的根节点
 * @return  root 平衡后的根节点
 */
static BSL_AvlTree *AVL_RebalanceRight(BSL_AvlTree *root)
{
    // 左右子树高度只相差1
    if ((GetAvlTreeHeight(root->leftNode) + 1u) >= GetAvlTreeHeight(root->rightNode)) {
        UpdateAvlTreeHeight(root);
        return root;
    }
    /* 左子树高度大于右子树, 先右旋转再左旋转 */
    BSL_AvlTree *curNode = root->rightNode;
    if (GetAvlTreeHeight(curNode->leftNode) > GetAvlTreeHeight(curNode->rightNode)) {
        root->rightNode = AVL_RotateRight(curNode);
    }
    return AVL_RotateLeft(root);
}

/**
 * @brief   AVL左平衡
 * @param   root [IN] 待平衡的根节点
 * @return  root 平衡后的根节点
 */
static BSL_AvlTree *AVL_RebalanceLeft(BSL_AvlTree *root)
{
    // 左右子树高度只相差1
    if ((GetAvlTreeHeight(root->rightNode) + 1u) >= GetAvlTreeHeight(root->leftNode)) {
        UpdateAvlTreeHeight(root);
        return root;
    }
    /* 右子树高度大于左子树, 先左旋转再右旋转 */
    BSL_AvlTree *curNode = root->leftNode;
    if (GetAvlTreeHeight(curNode->rightNode) > GetAvlTreeHeight(curNode->leftNode)) {
        root->leftNode = AVL_RotateLeft(curNode);
    }
    return AVL_RotateRight(root);
}

static void AVL_FreeData(BSL_ElementData data, BSL_AVL_DATA_FREE_FUNC freeFunc)
{
    if (freeFunc != NULL) {
        freeFunc(data);
    }
}

BSL_AvlTree *BSL_AVL_InsertNode(BSL_AvlTree *root, uint64_t nodeId, BSL_AvlTree *node)
{
    if (root == NULL) {
        node->nodeId = nodeId;
        return node;
    }

    if (root->nodeId > nodeId) {
        // nodeId比根节点nodeId小, 插入左子树
        root->leftNode = BSL_AVL_InsertNode(root->leftNode, nodeId, node);

        return AVL_RebalanceLeft(root);
    } else if (root->nodeId < nodeId) {
        // nodeId比根节点nodeId大, 插入右子树
        root->rightNode = BSL_AVL_InsertNode(root->rightNode, nodeId, node);

        return AVL_RebalanceRight(root);
    }

    /* key相同不能插入 */
    LOG_BINLOG_FIXLEN(BINLOG_ID05003, BSL_LOG_LEVEL_ERR, BSL_LOG_BINLOG_TYPE_RUN,
        "AVL tree insert key nodeId(%"PRIu64") already exist", nodeId, NULL, NULL, NULL);
    return NULL;
}

BSL_AvlTree *BSL_AVL_SearchNode(BSL_AvlTree *root, uint64_t nodeId)
{
    BSL_AvlTree *curNode = root;
    while (curNode != NULL) {
        // 匹配到节点
        if (curNode->nodeId == nodeId) {
            break;
        } else if (curNode->nodeId > nodeId) {
            // nodeId比根节点nodeId小, 搜索左子树
            curNode = curNode->leftNode;
        } else {
            // nodeId比根节点nodeId大, 搜索右子树
            curNode = curNode->rightNode;
        }
    }

    // 搜索不到指定节点, 返回NULL
    return curNode;
}

/**
 * @brief   删除同时拥有左右子节点的指定avl节点
 * @param   rmNodeChild [IN] 待删除avl节点的子节点
 *          removeNode  [IN] 待删除avl节点
 * @return  root 返回删除后的avl树根节点
 */
static BSL_AvlTree *AVL_DeleteNodeWithTwoChilds(BSL_AvlTree *rmNodeChild, BSL_AvlTree *removeNode)
{
    if (rmNodeChild == NULL || removeNode == NULL) {
        return NULL;
    }

    if (rmNodeChild->rightNode == NULL) {
        // 无论rmNodeChild是否有左节点, 都将左节点与祖父节点连接起来
        BSL_AvlTree *curNode = rmNodeChild->leftNode;
        removeNode->nodeId = rmNodeChild->nodeId;
        removeNode->data = rmNodeChild->data;

        SAL_FREE(rmNodeChild);
        return curNode;
    }

    rmNodeChild->rightNode = AVL_DeleteNodeWithTwoChilds(rmNodeChild->rightNode, removeNode);
    return AVL_RebalanceLeft(rmNodeChild);
}

BSL_AvlTree *BSL_AVL_DeleteNode(BSL_AvlTree *root, uint64_t nodeId, BSL_AVL_DATA_FREE_FUNC func)
{
    if (root == NULL) {
        return root;
    }

    if (root->nodeId == nodeId) {
        if (root->leftNode == NULL) {
            if (root->rightNode == NULL) {
                // 左右节点均为NULL
                AVL_FreeData(root->data, func);
                SAL_FREE(root);
                return NULL;
            } else {
                // 只有右节点
                BSL_AvlTree *curNode = root->rightNode;
                AVL_FreeData(root->data, func);
                SAL_FREE(root);
                return (curNode);
            }
        } else if (root->rightNode == NULL) {
            // 只有左节点
            BSL_AvlTree *curNode = root->leftNode;
            AVL_FreeData(root->data, func);
            SAL_FREE(root);
            return (curNode);
        } else {
            // 有左右节点
            AVL_FreeData(root->data, func);
            root->leftNode = AVL_DeleteNodeWithTwoChilds(root->leftNode, root);
            return AVL_RebalanceRight(root);
        }
    }

    if (root->nodeId > nodeId) {
        root->leftNode = BSL_AVL_DeleteNode(root->leftNode, nodeId, func);
        return AVL_RebalanceRight(root);
    } else {
        root->rightNode = BSL_AVL_DeleteNode(root->rightNode, nodeId, func);
        return AVL_RebalanceLeft(root);
    }
}

void BSL_AVL_DeleteTree(BSL_AvlTree *root, BSL_AVL_DATA_FREE_FUNC func)
{
    if (root == NULL) {
        return;
    }

    BSL_AVL_DeleteTree(root->leftNode, func);
    BSL_AVL_DeleteTree(root->rightNode, func);
    AVL_FreeData(root->data, func);
    SAL_FREE(root);
}
