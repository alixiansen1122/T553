/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: policy manager internal interfaces
* Author: Media Software Group
* Create: 2021-04-07
*/


/**
 * @addtogroup Audio
 * @{
 *
 * @brief Defines custom types needed for audio policy manager-related APIs and provides functions, for example,
 * to load drivers, access a driver adapter, and control audio streams.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file policy_manager_internal.h
 *
 * @brief Declares APIs for operations related to the audio policy manager.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef POLICY_MANAGER_INTERNAL_H
#define POLICY_MANAGER_INTERNAL_H

#include <sys/time.h>
#include "audio_base_type.h"
#include "policy_manager.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct {
    AudioSession maxSessionID;
    /* 记录当前最新的作为焦点的 AudioInterrupt */
    AudioInterruptProxy focusInterrupt;
    AudioPolicyCallback callback;
    AudioStreamStrategy streamStrategy;
} PolicyMgrContext;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* POLICY_MANAGER_INTERNAL_H */
/** @} */
