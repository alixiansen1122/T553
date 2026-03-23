/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides at cmd register func \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-07-20， Create file. \n
 */
#ifndef AUDIO_AT_PROCESS_H
#define AUDIO_AT_PROCESS_H

#include <stdint.h>
#include <stdio.h>
#include "errcode.h"
#include "soc_osal.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))
#define BSP_MEM_ALLOC(y) osal_kmalloc((y), OSAL_GFP_KERNEL)
#define BSP_MEM_FREE(y) osal_kfree(y)

void audio_at_cmd_process(uint8_t *data, uint32_t data_len);


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif