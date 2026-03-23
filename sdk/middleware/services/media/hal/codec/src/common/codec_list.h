/*
 * Copyright (c) @CompanyNameMagicTag. 2016-2020. All rights reserved.
 * Description: Simple doubly linked list implementation
 * Author: Media Software Group
 * Create: 2016-06-06
 */
#ifndef CODEC_LIST_H
#define CODEC_LIST_H

#include "linux_cbb_list.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/** free list node */
#define CODEC_LIST_FREE(p)  do { \
    if (NULL != (p)) { \
        free(p); \
        (p) = NULL; \
    } \
} while (0)

/** define list head struct */
typedef struct cbb_list_head ListHead;

/** define codelist head struct */
typedef ListHead CodecListHead;

/** init list head node */
#define CODEC_LIST_INIT_HEAD_DEFINE(name) LIST_HEAD_INIT(name)

/** list head node define */
#define CODEC_LIST_HEAD_DEFINE(name) LIST_HEAD(name)

/** init list head node pointer */
#define CODEC_LIST_INIT_HEAD_PTR(ptr) INIT_LIST_HEAD(ptr)

/**
 * add a new entry
 * @pstruNew: input, new entry to be added
 * @pstruHead: input, the entry after witch new entry will be add
 */
#define CODEC_LIST_ADD(pstruNew, pstruHead) cbb_list_add(pstruNew, pstruHead)

/**
 * @new: input, pstruNew entry to be added
 * @head: input, pstruHead the entry before witch new entry will be add
 * this is useful for implementing a queue.
 */
#define CODEC_LIST_ADD_TAIL(pstruNew, pstruHead) cbb_list_add_tail(pstruNew, pstruHead)

/*
 * Delete a list entry by making the prev/next entries point to each other.
 * @pstruEntry: input, the entry to delete
 */
#define CODEC_LIST_DEL(pstruEntry) cbb_list_del(pstruEntry)

/**
 * Delete a entry from list and reinitialize it.
 * @pstruEntry: input, the element to delete from the list.
 */
#define CODEC_LIST_DEL_INIT(pstruEntry) cbb_list_del_init(pstruEntry)

/**
 * tests whether a list is empty
 * @pstruHead: input, the list to test.
 */
#define CODEC_LIST_EMPTY(pstruHead) cbb_list_empty(pstruHead)

/**
 * join two lists
 * @pstruList: input, the new list to add.
 * @pstruHead: input, the entry after witch to add the new list.
 */
#define CODEC_LIST_SPLICE(pstruList, pstruHead) cbb_list_splice(pstruList, pstruHead)

/**
 * get the struct address from a list entry
 * @ptr:   input, the pointer of the entry
 * @type: input, the pointer struct type that should return
 * @member: the name of the entry within the struct type.
 */
#define CODEC_LIST_ENTRY(ptr, type, member) cbb_list_entry(ptr, type, member)

/**
 *  iterate over a list
 * @pos:   input, output, the &struct CodecListHead to use as a loop counter.
 * @head:  input, the head of the list.
 */
#define CODEC_LIST_FOR_EACH(pos, head) cbb_list_for_each(pos, head)

/**
 * iterate over a list safe against removal of list entry
 * @pos:   input, output,  the &struct CodecListHead to use as a loop counter.
 * @n:     input, another &struct CodecListHead to use as temporary storage
 * @head:  input, the head of the list.
 */
#define CODEC_LIST_FOR_EACH_SAFE(pos, n, head) cbb_list_for_each_safe(pos, n, head)

/**
 *  iterate over list of given type
 * @pos:    input, output, the type * to use as a loop cursor.
 * @n:  input, another &struct CodecListHead to use as temporary storage
 * @head:  input,  the head for your list.
 * @member: input, the name of the list_struct within the struct.
 */
#define CODEC_LIST_FOR_EACH_ENTRY_SAFE(pos, n, head, member) cbb_list_for_each_entry_safe(pos, n, head, member)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CODEC_LIST_H__ */
