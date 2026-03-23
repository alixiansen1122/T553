/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: audio error define
* Author: Media Software Group
* Create: 2021-04-21
*/

/**
 * @addtogroup MultiMedia_AudioManager
 * @{
 *
 * @brief Defines the native audio error codes.
 *
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file audio_errors.h
 *
 * @brief Defines the native audio error codes.
 *
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef AUDIO_ERRORS_H
#define AUDIO_ERRORS_H

#include <stdint.h>

enum {
    AUDIO_SUCCESS            = 0,
    AUDIO_ERROR              = -1,
    AUDIO_INVALID_PARAMS     = -2,
    AUDIO_INVALID_OPERATION  = -3,
    AUDIO_PERMISSION_DENIED  = -4,
    AUDIO_NO_INIT            = -5,
    AUDIO_NO_MEM             = -6,
    AUDIO_RETRY_READ         = -7,
    AUDIO_NOT_SUPPORT_GET_FRAME = -8,
    AUDIO_DEAD_OBJECT        = -9,
    AUDIO_BAD_STATE          = -10,
    AUDIO_NOT_FOUND_ACTIVE_STREAM          = -11,
    AUDIO_NOT_SUPPORT = -12,
};

#endif // AUDIO_ERRORS_H
