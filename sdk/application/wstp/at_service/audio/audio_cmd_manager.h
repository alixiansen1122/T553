/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides at cmd register func \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-07-20， Create file. \n
 */
#ifndef AUDIO_CMD_MANAGER_H
#define AUDIO_CMD_MANAGER_H

#include <stdint.h>
#include <stdio.h>
#include "errcode.h"
#include "securec.h"
#include "at_cmd_api.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */


at_cmd_table_t *get_audio_cmd_table(uint32_t *cmd_count);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif