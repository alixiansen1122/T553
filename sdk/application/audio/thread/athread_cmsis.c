/*
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: create thread
 * Author: audio
 * Create: 2022-03-09
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sample_audio_api.h"
#include "cmsis_os.h"
#include "thread_os.h"

struct athread_inst {
    osThreadId_t h_task;
    td_bool task_exit;
};

td_s32 athread_create(athread_handle *inst, td_void (*fun_thread)(td_void *), td_void *inst_args, athread_attr *attr)
{
    osThreadAttr_t task_attr;
    athread_handle handle;

    handle = (athread_handle)malloc(sizeof(struct athread_inst));
    if (handle == TD_NULL) {
        return EXT_FAILURE;
    }

    handle->task_exit = TD_FALSE;
    *inst = handle;

    (td_void)memset_s(&task_attr, sizeof(task_attr), 0, sizeof(task_attr));
    task_attr.priority = (osPriority_t)attr->priority;
    task_attr.stack_size = attr->stack_size;
    task_attr.name = attr->name;
    handle->h_task = osThreadNew(fun_thread, inst_args, &task_attr);
    if (handle->h_task == TD_NULL) {
        free(handle);
        *inst = TD_NULL;
        sap_printf("Call osThreadNew failed");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_void athread_set_exit(athread_handle inst, td_bool exit)
{
    inst->task_exit = exit;
}

td_void athread_exit(athread_handle inst)
{
    while (!inst->task_exit) {
        sap_msleep(THREAD_SLEEP_10MS);
    }
    free(inst);
}
