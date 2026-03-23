/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides IPC source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-04-18, Create file. \n
 */
#include "cipher.h"
#include "drv_hkdf.h"

#include "sl_common.h"

errcode_t uapi_drv_cipher_hkdf(uapi_drv_cipher_hkdf_t *hkdf_param, uint8_t *okm, uint32_t okm_length)
{
    int32_t ret = drv_cipher_hkdf((crypto_hkdf_t *)(uintptr_t)hkdf_param, okm, okm_length);
    return crypto_sl_common_get_errcode(ret);
}