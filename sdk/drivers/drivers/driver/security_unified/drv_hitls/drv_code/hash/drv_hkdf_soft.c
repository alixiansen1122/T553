/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides IPC source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-04-18, Create file. \n
 */
#include "drv_hkdf.h"

#include "crypt_hmac.h"
#include "crypt_hkdf.h"
#include "crypt_errno.h"
#include "crypt_sha2.h"

#include "crypto_drv_common.h"

static const EAL_MacMethod g_mac_hmac_method = {
    .initCtx = (MacInitCtx)CRYPT_HMAC_InitCtx,
    .init = (MacInit)CRYPT_HMAC_Init,
    .update = (MacUpdate)CRYPT_HMAC_Update,
    .final = (MacFinal)CRYPT_HMAC_Final,
    .deinit = (MacDeinit)CRYPT_HMAC_Deinit,
    .deinitCtx = (MacDeinitCtx)CRYPT_HMAC_DeinitCtx,
    .reinit = (MacReinit)CRYPT_HMAC_Reinit,
    .getLen = (MacGetMacLen)CRYPT_HMAC_GetMacLen,
    .ctxSize = sizeof(CRYPT_HMAC_Ctx)
};

static const EAL_MdMethod g_md_sha256_meth = {
    .blockSize = 64,    // 64: block size for sha256
    .mdSize = 32,       // 32: result size for sha256
    .ctxSize = sizeof(CRYPT_SHA2_256_Ctx),
    .init = (MdInit)CRYPT_SHA2_256_Init,
    .update = (MdUpdate)CRYPT_SHA2_256_Update,
    .final = (MdFinal)CRYPT_SHA2_256_Final,
    .deinit = (MdDeinit)CRYPT_SHA2_256_Deinit,
    .copyCtx = (MdCopyCtx)CRYPT_SHA2_256_CopyCtx,
};

int32_t drv_cipher_hkdf(crypto_hkdf_t *hkdf_param, uint8_t *okm, uint32_t okm_length)
{
    int32_t ret;

    ret = CRYPT_HKDF(&g_mac_hmac_method, &g_md_sha256_meth, hkdf_param->ikm, hkdf_param->ikm_length,
        hkdf_param->salt, hkdf_param->salt_length, hkdf_param->info, hkdf_param->info_length,
        okm, okm_length);
    crypto_chk_return(ret != CRYPT_SUCCESS, ret, "CRYPT_HKDF failed\n");

    return CRYPTO_SUCCESS;
}