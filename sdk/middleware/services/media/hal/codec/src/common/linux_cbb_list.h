/*
 * Copyright (c) @CompanyNameMagicTag. 2018-2020. All rights reserved.
 * Description: common define
 * Author: Media Software Group
 * Create: 2018-01-05
 */
#ifndef HPT_LIST_H
#define HPT_LIST_H

#include <stdint.h>

#ifndef HPT_INLINE
#define HPT_INLINE __inline
#endif

typedef uintptr_t HPT_UPTR;

struct cbb_list_head {
    struct cbb_list_head *next, *prev;
};

#ifndef LIST_HEAD_INIT
#define LIST_HEAD_INIT(name) \
    {                        \
        &(name), &(name)     \
    }
#endif

#ifndef LIST_HEAD
#define LIST_HEAD(name) (struct list_head name = LIST_HEAD_INIT(name))
#endif

#ifndef INIT_LIST_HEAD
#define INIT_LIST_HEAD(ptr)  \
    do {                     \
        (ptr)->next = (ptr); \
        (ptr)->prev = (ptr); \
    } while (0)
#endif

static HPT_INLINE void cbb_list_add_inner(struct cbb_list_head *_new, struct cbb_list_head *prev,
    struct cbb_list_head *next)
{
    if (prev == NULL || _new == NULL || next == NULL) {
        return;
    }

    next->prev = _new;
    _new->next = next;
    _new->prev = prev;
    prev->next = _new;
}

static HPT_INLINE void cbb_list_add(struct cbb_list_head *_new, struct cbb_list_head *head)
{
    if (_new == NULL || head == NULL) {
        return;
    }

    cbb_list_add_inner(_new, head, head->next);
}

static HPT_INLINE void cbb_list_add_tail(struct cbb_list_head *_new, struct cbb_list_head *head)
{
    if (_new == NULL || head == NULL) {
        return;
    }

    cbb_list_add_inner(_new, head->prev, head);
}

static HPT_INLINE void cbb_list_del_inner(struct cbb_list_head *prev, struct cbb_list_head *next)
{
    if (prev == NULL || next == NULL) {
        return;
    }

    next->prev = prev;
    prev->next = next;
}

static HPT_INLINE void cbb_list_del(struct cbb_list_head *entry)
{
    if (entry == NULL) {
        return;
    }

    cbb_list_del_inner(entry->prev, entry->next);
}

static HPT_INLINE void __cbb_list_del(struct cbb_list_head *prev __attribute__((unused)),
    struct cbb_list_head *next __attribute__((unused)))
{
    return;
}

static HPT_INLINE void cbb_list_del_init(struct cbb_list_head *entry)
{
    __cbb_list_del(entry->prev, entry->next);
    INIT_LIST_HEAD(entry);
}

static HPT_INLINE int cbb_list_empty(const struct cbb_list_head *head)
{
    return head->next == head;
}

static HPT_INLINE void cbb_list_splice_inner(struct cbb_list_head *list, struct cbb_list_head *head)
{
    struct cbb_list_head *first = list->next;
    struct cbb_list_head *last = list->prev;
    struct cbb_list_head *at = head->next;

    first->prev = head;
    head->next = first;

    last->next = at;
    at->prev = last;
}

static HPT_INLINE void cbb_list_splice(struct cbb_list_head *list, struct cbb_list_head *head)
{
    if (cbb_list_empty(list) == 0) {
        cbb_list_splice_inner(list, head);
    }
}

static HPT_INLINE void cbb_list_splice_init(struct cbb_list_head *list, struct cbb_list_head *head)
{
    if (cbb_list_empty(list) == 0) {
        cbb_list_splice_inner(list, head);
        INIT_LIST_HEAD(list);
    }
}

#define cbb_list_entry(ptr, type, member) ((type *)((uintptr_t)(ptr) - ((uintptr_t)(&((type *)0)->member))))

#define cbb_list_for_each(pos, head) \
    for ((pos) = (head)->next; (((pos) != (head)) && ((pos) != NULL)); (pos) = (pos)->next)

#define cbb_list_for_each_safe(pos, n, head) \
    for ((pos) = (head)->next, (n) = (pos)->next; (pos) != (head); (pos) = (n), (n) = (pos)->next)

#define cbb_get_first_item(attached, type, member) \
    ((type *)((char *)((attached)->next) - (HPT_UPTR)(&((type *)0)->member)))

#define cbb_list_for_each_entry_safe(pos, n, head, member)                                           \
    do {                                                                                             \
        for ((pos) = cbb_list_entry((head)->next, typeof(*pos), (member)),                               \
        (n) = cbb_list_entry((pos)->member.next, typeof(*pos), (member));                              \
            (&pos)->member != (head); (pos) = (n), (n) = cbb_list_entry((n)->member.next, typeof(*n), (member))) \
    } while (0)
#endif
