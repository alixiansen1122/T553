/*
 * Copyright (c) CompanyNameMagicTag 2020-2020. All rights reserved.
 * Description: header file for sea alg0 api
 * Author: audio
 */

#ifndef __SEA_ALG0_API_H__
#define __SEA_ALG0_API_H__

#include "audio_alg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    SEA_ALG_CMD_GET_VERSION = 0x0600,
    SEA_ALG_CMD_SET_PARAM,
    SEA_ALG_CMD_GET_SCR_BUF_SIZE,
    SEA_ALG_CMD_SET_SCR_BUF,
    SEA_ALG_CMD_MAX,
} sea_alg_cmd_id;

audio_alg_component *sea_alg0_report_entry(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SEA_ALG0_API_H__ */
