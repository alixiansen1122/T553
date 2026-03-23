/*
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: header file for create thread
 * Author: audio
 */

#ifndef __THREAD_OS_H__
#define __THREAD_OS_H__

#include "td_type.h"

typedef struct athread_inst* athread_handle;
typedef td_void (*athread_fun)(td_void *arg);

#define ATHREAD_PRIORITY_NORMAL 24

typedef struct {
    td_char *name;        /* name of the thread */
    td_u32 stack_size;    /* size of stack */
    td_u32 priority;      /* thread priority */
} athread_attr;

td_s32 athread_create(athread_handle *inst, td_void (*fun_thread)(td_void *), td_void *inst_args, athread_attr *attr);

td_void athread_exit(athread_handle inst);

td_void athread_set_exit(athread_handle inst, td_bool exit);

#endif
