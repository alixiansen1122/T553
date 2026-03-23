/*
 * Copyright (c) CompanyNameMagicTag 2021. All rights reserved.
 * Description: voice engine hal.
 */
#ifndef VE_HAL_H
#define VE_HAL_H

#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    ASR_ENGINE
} voice_engine_type;

typedef enum {
    DATA_INJECTION, // voice data is injected
    EXT_PORT,       // voice data is read from external audio port.
    INNER_PORT,     // voice data is read from inner audio port.
} voice_engine_input_mode;

typedef enum {
    VOICE_ENGINE_EVENT_VAD_VALID,
    VOICE_ENGINE_EVENT_VAD_TIMEOUT,
    VOICE_ENGINE_EVENT_VOICE_COMMAND,
    VOICE_ENGINE_EVENT_MAX
} voice_engine_event_type;

typedef struct {
    td_u32 user_id;
    td_uchar *cmd;
    td_u32 cmd_len;
    td_uchar *cmd_args;
    td_u32 cmd_args_len;
} voice_engine_event_param;

typedef struct {
    td_s32 enable_vid;
    td_s32 enable_nlp;
    td_s32 enable_aec;
    td_s32 enable_npu;
} asr_engine_attr;

typedef struct {
    voice_engine_type type;
    union {
        asr_engine_attr asr_attr;
    } u;
} voice_engine_params;

typedef td_void (*voice_engine_cb)(voice_engine_event_type type, td_void *data, td_u32 size);

td_s32 vehal_init(td_void);
td_s32 vehal_deinit(td_void);
td_s32 vehal_set_input(voice_engine_input_mode input_mode, td_void *param, td_u32 length);
td_s32 vehal_create_engine(voice_engine_params *eng_params);
td_s32 vehal_start_engine(td_void);
td_s32 vehal_stop_engine(td_void);
td_s32 vehal_destroy_engine(td_void);
td_s32 vehal_start_vad(td_void);
td_s32 vehal_stop_vad(td_void);
td_s32 vehal_set_callback(voice_engine_cb callback);
td_u32 vehal_inject(td_uchar *data, td_u32 length);
td_u32 vehal_capture(td_uchar *data, td_u32 length);
td_void vehal_dump(td_void);
td_u32 vehal_raw_capture(td_uchar *data, td_u32 length);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif // VE_HAL_H