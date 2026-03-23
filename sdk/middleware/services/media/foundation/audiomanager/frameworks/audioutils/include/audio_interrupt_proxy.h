/*
 * Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
 * Description: audio interrupt define
 * Author: Media Software Group
 * Create: 2021-03-02
 */

#ifndef AUDIO_INTERRUPT_PROXY_H
#define AUDIO_INTERRUPT_PROXY_H

#include <stdint.h>
#include "audio_base_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef void (*InterruptListenerCb)(uintptr_t owner, uintptr_t userData, int32_t type, int32_t hint);

/**
 * @brief Defines InterruptListener callbacks for the AudioService.
 */
typedef struct {
    /** for callback dispatcher */
    uintptr_t owner;
    /** for User's callback implementation */
    uintptr_t userData;
    AudioStreamType streamType;
    AudioSession sessionID;
    InterruptListenerCb interruptListenerCb;
} AudioInterruptProxy;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif // AUDIO_INTERRUPT_PROXY_H