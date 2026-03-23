/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: callback dispatcher interface
* Author: Media Software Group
* Create: 2021-02-28
*/

#ifndef AUDIO_CALL_BACK_DISPATCHER_H
#define AUDIO_CALL_BACK_DISPATCHER_H

#include <stddef.h>
#include <stdint.h>
#ifdef SUPPORT_SYS_PRCTL
#include <sys/prctl.h>
#endif
#include <stdbool.h>
#include <unistd.h>
#include "media_thread_adapt.h"

typedef void *DispatcherHandle;

typedef struct {
    uintptr_t userData;
    int32_t type;
    int32_t hint;
    void *cookie;
} Message;

int32_t AudioCallbackDispatcherCreate(DispatcherHandle *handle);

int32_t AudioCallbackDispatcherInitialize(DispatcherHandle handle);

// Posts |msg| to the listener's queue. If |realTime| is true, the listener thread is notified
// that a new message is available on the queue. Otherwise, the message stays on the queue, but
// the listener is not notified of it. It will process this message when a subsequent message
// is posted with |realTime| set to true.
int32_t AudioCallbackDispatcherPost(DispatcherHandle handle, Message msg, bool realTime);

int32_t AudioCallbackDispatcherDestroy(DispatcherHandle handle);

#endif  // AUDIO_CALL_BACK_DISPATCHER_H
