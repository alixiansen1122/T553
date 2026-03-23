/**
 * @copyright   Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @brief       链表操作
 */

#ifndef BSL_LIST_H
#define BSL_LIST_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 该结构被用来保存双向链表中节点的前向指针和后向指针。
 * 这个链表不包含实质的数据区，一般用于组织(串接)数据节点
 */
typedef struct ListHeadNode {
    struct ListHeadNode *next, *prev;
} ListHead;

/**
 * @brief   初始化链表(链表重用时的初始化)
 *
 * @param   head [IN] 链表头结点的地址(The address of the head of a list )
 */
#define LIST_INIT(head) (head)->next = (head)->prev = (head)

/**
 * @brief   将节点item插入到节点where之后
            变化前: where->A->B  变化后: where->item->A->B
 *
 * @param   where [IN] item插入后的前一个节点地址。(The address where the item will be inserted after)
 * @param   item  [IN] 待插入节点地址(The address of the item)
 */
#define LIST_ADD_AFTER(where, item) do {  \
    (item)->next       = (where)->next; \
    (item)->prev       = (where);       \
    (where)->next      = (item);        \
    (item)->next->prev = (item);        \
} while (0)

/**
 * @brief   将节点item插入到节点where之前
 *          变化前: A->where->B  变化后: A->item->where->B
 *
 * @param   where [IN] item插入后的后一个节点地址。(The address where the item will be inserted before)
 * @param   item  [IN] 待插入节点地址(The address of the item)
 */
#define LIST_ADD_BEFORE(where, item) LIST_ADD_AFTER((where)->prev, (item))

/**
 * @brief   删除节点item
 *
 * @param   item [IN] 待删除的节点(The address of the item to be removed)
 */
#define LIST_REMOVE(item) do { \
    (item)->prev->next = (item)->next; \
    (item)->next->prev = (item)->prev; \
} while (0)

/**
 * @brief   检查链表是否为空(Judge whether a list is empty)
 *
 * @param   head [IN] 需要检查的链表(The address of the list to be judged)
 */
#define LIST_IS_EMPTY(head) ((head)->next == (head))

/**
 * @brief   安全遍历一个链表(Travel through a list safety)
 *
 * @param   head [IN] 需要遍历的链表(The head of a list)
 * @param   temp [IN] 指向当前节点以便安全删除当前节点(pointer used to save current item so you can free item safety)
 * @param   item [IN] 遍历链表所用的缓存节点(A temporary list item for travelling the list)
 */
#define LIST_FOR_EACH_ITEM_SAFE(item, temp, head) \
    for ((item) = (head)->next, (temp) = (item)->next; (item) != (head); (item) = (temp), (temp) = (item)->next)

/**
 * @brief   通过链表某个节点(小结点)找到该节点所在结构(大节点)的起始地址
 *
 * @param   item   [IN] 特定节点变量(The address of a list item)
 * @param   type   [IN] 包含链表节点的大节点类型(The type of a struct which includes the list item)
 * @param   member [IN] 结构体内的list节点成员名称(The member variable of the struct whose type is list item)
 *
 * 说明:
 * 每个结构变量形成一个一个的大节点(包含数据和list小结点), 大节点是通过list 这个链表(小节点)串起来的。
 *  ---------      ---------      ---------    --               ----
 * |  pre    |<---|  pre    |<---|  pre    |     |==>小结点         |
 * |  next   |--->|  next   |--->|  next   |     |                  |
 *  ---------      ---------      ---------    --                   | ===> 大节点
 * |  data1  |    |  data1  |    |  data1  |                        |
 * |  data2  |    |  data2  |    |  data2  |                        |
 *  ---------      ---------      ---------                      ----
 * 不直接用list作为大节点的原因是 list(ListHead 类型)只有头尾指针，不包含数据区。
 * 这样 list链表可以适用挂接任意个数据的场合，具有通用性。
 */
#define LIST_ENTRY(item, type, member) \
    ((type *)((uintptr_t)(char *)(item) - (uintptr_t)(&((type *)0)->member)))

#ifdef __cplusplus
}
#endif
#endif // BSL_LIST_H