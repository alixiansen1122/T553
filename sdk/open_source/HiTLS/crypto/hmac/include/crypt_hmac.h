/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file crypt_hmac.h
 * Description: hmac原语实现头文件
 * Author: liushaoming
 * Create: 2022-04-18
 */
#ifndef CRYPT_HMAC_H
#define CRYPT_HMAC_H

#include <stdint.h>
#include "crypt_local_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cpluscplus */

#define HMAC_MAXBLOCKSIZE 128
#define HMAC_MAXOUTSIZE   64

typedef struct HMAC_Ctx {
    const EAL_MdMethod *method;
    void *mdCtx;            /* md ctx */
    void *oCtx;             /* opad ctx */
    void *iCtx;             /* ipad ctx */
} CRYPT_HMAC_Ctx;

int32_t CRYPT_HMAC_InitCtx(CRYPT_HMAC_Ctx *ctx, const EAL_MdMethod *m);
void    CRYPT_HMAC_DeinitCtx(CRYPT_HMAC_Ctx *ctx);
int32_t CRYPT_HMAC_Init(CRYPT_HMAC_Ctx *ctx, const uint8_t *key, uint32_t len);
int32_t CRYPT_HMAC_Update(CRYPT_HMAC_Ctx *ctx, const uint8_t *in, uint32_t len);
int32_t CRYPT_HMAC_Final(CRYPT_HMAC_Ctx *ctx, uint8_t *out, uint32_t *len);
void    CRYPT_HMAC_Reinit(CRYPT_HMAC_Ctx *ctx);
void    CRYPT_HMAC_Deinit(CRYPT_HMAC_Ctx *ctx);
uint32_t  CRYPT_HMAC_GetMacLen(const CRYPT_HMAC_Ctx *ctx);

#ifdef __cplusplus
}
#endif /* __cpluscplus */

#endif
