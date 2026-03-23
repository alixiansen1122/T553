
/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file modes_local.h
 * Description: modes 内部处理函数
 * Author: haoruixiang
 * Create: 2022-6-26
 * Modification History
 *  DATE        NAME                         DESCRIPTION
 *  --------------------------------------------------------------------------
 *  2022-06-26  haoruixiang                chacha20 内部处理函数
 */
#ifndef MODES_LOCAL_H
#define MODES_LOCAL_H

#include <stdint.h>
#include <stdbool.h>
#include "crypt_modes_gcm.h"

void GcmTableGen4bit(uint8_t key[GCM_BLOCKSIZE], MODES_GCM_GF128 hTable[16]);

void GcmHashMultiBlock(uint8_t t[GCM_BLOCKSIZE], const MODES_GCM_GF128 hTable[16], const uint8_t *in, uint32_t inLen);

int32_t CryptLenCheckAndRefresh(MODES_GCM_Ctx *ctx, uint32_t len);

uint32_t LastHandle(MODES_GCM_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t len, bool enc);

#endif