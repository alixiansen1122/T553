/*
 * Copyright (c) CompanyNameMagicTag 2020-2020. All rights reserved.
 * Description: kws api
 * Author: audio
 */

#include <stdlib.h>
#include <string.h>
#include "securec.h"
#include "plugin_errno.h"
#include "audio_sea.h"
#include "audio_type.h"
#include "sea_alg0_api.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define SEA_ALG_FRAME_LEN  20
#define SEA_ALG_SCR_BUF_SIZE (0x8000)
#define SEA_ALG_INST_SIZE 0x100
#define SEA_ALG_DATA_SIZE ((16000 << 1) / (1000 / SEA_ALG_FRAME_LEN)) /* 16KHz/16bit/1ch/20ms */

#define sea_alg_check_null_return(ptr) \
    do { \
        if ((td_void *)(ptr) == TD_NULL) { \
            return PLUGIN_FAILURE; \
        } \
    } while (0)

typedef struct {
    td_void *handle;
    sea_pcm_format pcm_attr;
    td_u8 *inst_base;
    td_u8 *scr_buf;
    td_u32 scr_size;
    td_u8 *pcm_buf;
    td_u32 pcm_size;
} sea_alg_inst;

static td_s32 sea_alg_api_init(td_void *handle)
{
    sea_alg_inst *alg_inst = (sea_alg_inst *)handle;
    sea_alg_check_null_return(handle);

    alg_inst->handle = TD_NULL;
    alg_inst->inst_base = (td_u8 *)(handle + sizeof(sea_alg_inst));
    alg_inst->scr_buf = TD_NULL;
    alg_inst->scr_size = SEA_ALG_SCR_BUF_SIZE;
    alg_inst->pcm_size = SEA_ALG_DATA_SIZE;
    alg_inst->pcm_buf = (td_u8 *)(handle + sizeof(sea_alg_inst) + SEA_ALG_INST_SIZE);
    return PLUGIN_SUCCESS;
}

static td_s32 sea_alg_set_scr_buf(td_void *handle, td_u8 *scr_buf)
{
    sea_alg_inst *alg_inst = (sea_alg_inst *)handle;
    sea_alg_check_null_return(handle);
    sea_alg_check_null_return(scr_buf);

    alg_inst->scr_buf = scr_buf;
    return PLUGIN_SUCCESS;
}

static td_s32 sea_alg_set_param(td_void *handle, const td_void *param)
{
    sea_alg_inst *alg_inst = (sea_alg_inst *)handle;
    sea_alg_check_null_return(handle);
    sea_alg_check_null_return(param);

    audio_unused(alg_inst);
    return PLUGIN_SUCCESS;
}

static td_s32 sea_alg_set_config(td_void *handle, td_u32 cmd, const td_void *cfg)
{
    switch (cmd) {
        case ALG_COMMON_CMD_API_INIT:
            return sea_alg_api_init(handle);

        case SEA_ALG_CMD_SET_SCR_BUF:
            return sea_alg_set_scr_buf(handle, (td_u8 *)cfg);

        case SEA_ALG_CMD_SET_PARAM:
            return sea_alg_set_param(handle, cfg);

        default:
            break;
    }

    return PLUGIN_SUCCESS;
}

static td_s32 sea_alg_get_api_size(td_u32 *size)
{
    sea_alg_check_null_return(size);
    *size = sizeof(sea_alg_inst) + SEA_ALG_INST_SIZE + SEA_ALG_DATA_SIZE;
    return PLUGIN_SUCCESS;
}

static td_s32 sea_alg_get_scr_buf_size(td_u32 *size)
{
    sea_alg_check_null_return(size);
    *size = SEA_ALG_SCR_BUF_SIZE;
    return PLUGIN_SUCCESS;
}

static td_s32 sea_alg_get_config(td_void *handle, td_u32 cmd, td_void *cfg)
{
    switch (cmd) {
        case ALG_COMMON_CMD_GET_API_SIZE:
            return sea_alg_get_api_size((td_u32 *)cfg);

        case SEA_ALG_CMD_GET_SCR_BUF_SIZE:
            return sea_alg_get_scr_buf_size((td_u32 *)cfg);

        case SEA_ALG_CMD_GET_VERSION:
            return PLUGIN_SUCCESS;

        default:
            break;
    }

    audio_unused(handle);
    return PLUGIN_SUCCESS;
}

static td_s32 sea_alg_destroy(td_void *handle)
{
    sea_alg_inst *alg_inst = (sea_alg_inst *)handle;
    sea_alg_check_null_return(handle);

    audio_unused(alg_inst);
    return PLUGIN_SUCCESS;
}

static td_s32 sea_alg_create(td_void *handle, const td_void *attr)
{
    sea_alg_inst *alg_inst = (sea_alg_inst *)handle;
    sea_alg_check_null_return(handle);

    if ((td_void *)alg_inst->inst_base != (handle + sizeof(sea_alg_inst))) {
        return PLG_ERR_NOT_INIT;
    }

    alg_inst->handle = (td_void *)alg_inst->inst_base;
    audio_unused(attr);
    return PLUGIN_SUCCESS;
}

static td_s32 sea_alg_process(td_void *handle, td_void *input, td_void *output)
{
    sea_alg_inst *alg_inst = (sea_alg_inst *)handle;
    sea_pcm_frame *input_frame = (sea_pcm_frame *)input;
    sea_pcm_frame *output_frame = (sea_pcm_frame *)output;

    sea_alg_check_null_return(handle);
    sea_alg_check_null_return(input);
    sea_alg_check_null_return(output);
    sea_alg_check_null_return(alg_inst->handle);

    (td_void)memcpy_s(alg_inst->pcm_buf, alg_inst->pcm_size, (td_u8 *)input_frame->data, alg_inst->pcm_size);
    (td_void)memcpy_s(output_frame, sizeof(*output_frame), input_frame, sizeof(*input_frame));
    output_frame->data = (td_s32 *)alg_inst->pcm_buf;
    return PLUGIN_SUCCESS;
}

static td_s32 sea_alg_reset(td_void *handle)
{
    sea_alg_inst *alg_inst = (sea_alg_inst *)handle;
    sea_alg_check_null_return(handle);
    sea_alg_check_null_return(alg_inst->handle);

    return PLUGIN_SUCCESS;
}

static audio_alg_component g_sea_alg0_comp = {
    .name = "sea alg0",
    .version = 0,

    .create = sea_alg_create,
    .destroy = sea_alg_destroy,

    .set_config = sea_alg_set_config,
    .get_config = sea_alg_get_config,

    .reset = sea_alg_reset,

    .process = sea_alg_process,
};

audio_alg_component *sea_alg0_report_entry(td_void)
{
    return &g_sea_alg0_comp;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
