/*
 * Copyright (c) CompanyNameMagicTag 2024. All rights reserved.
 * Description: Transition
 * Author:
 * Create: 2024-03
 */

#ifndef TRANSITION_TYPES_H
#define TRANSITION_TYPES_H
#include "stdint.h"
#ifdef JS_ENABLE
#include "want.h"
#endif
namespace OHOS {
enum class TransitionType {
    TRANSITION_INVALID = 0,
    TRANSITION_ZOOM,
    TRANSITION_ZOOM_OUT,
    TRANSITION_HEXAGONS,
    TRANSITION_BACK_TO_HEXAGONS,
    TRANSITION_WATERFALL,
    TRANSITION_BACK_TO_WATERFALL,
    TRANSITION_ENTER_WATERFALL,
    TRANSITION_FADEINOUT,
    TRANSITION_TRANSPARENT_GRADIENT,
    TRANSITION_GAUSSIAN_BLUR,
    TRANSITION_CROSS_SHAPED,
    TRANSITION_BACK_TO_CROSS_SHAPED,
    TRANSITION_3DFOLD,
    TRANSITION_MAX,
};

enum class TransitionCaller {
    CHANGE_SLICE,
#ifdef JS_ENABLE
    START_ABILITY,
    TERMINATE_ABILITY,
    FORCE_STOP_BUNDLE,
    FORCE_STOP,
#endif
    BACK_TRANSITION,
    INVALID
};

typedef struct {
    uint32_t sliceAndPageId;
    void *data;
    uint16_t dataLength;
} SliceInfo;

typedef union {
    SliceInfo sliceInfo; // The number of high 16 bits is priority, and the number of low 16 bits is slice id
#ifdef JS_ENABLE
    Want* want;
    char* bundleName;
    uint64_t token;
#endif
} TransitionTarget;
}
#endif