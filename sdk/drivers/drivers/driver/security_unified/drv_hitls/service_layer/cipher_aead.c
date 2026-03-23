/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides IPC source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-04-18, Create file. \n
 */
#include "cipher_aead.h"
#include "drv_symc_aead.h"

#include "sl_common.h"

errcode_t uapi_drv_cipher_symc_gcm_encrypt(
    uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *plain_text, uint8_t *cipher_text, uint32_t data_len,
    uint8_t *tag, uint32_t tag_len)
{
    int32_t ret = drv_cipher_symc_gcm_encrypt(
        key, key_len, keyslot_handle,
        iv, iv_len,
        aad, aad_len,
        plain_text, cipher_text, data_len,
        tag, tag_len);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_gcm_decrypt_verify(
    uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *cipher_text, uint8_t *plain_text, uint32_t data_len,
    const uint8_t *tag, uint32_t tag_len)
{
    int32_t ret = drv_cipher_symc_gcm_decrypt_verify(
        key, key_len, keyslot_handle,
        iv, iv_len,
        aad, aad_len,
        cipher_text, plain_text, data_len,
        tag, tag_len);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_gcm_create(uint32_t *handle,
    uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    uint32_t tag_len)
{
    int32_t ret = drv_cipher_symc_gcm_create(
        handle, key, key_len, keyslot_handle,
        iv, iv_len,
        aad, aad_len,
        tag_len);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_gcm_destroy(uint32_t handle)
{
    int32_t ret = drv_cipher_symc_gcm_destroy(handle);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_gcm_encrypt_update(uint32_t handle,
    const uint8_t *plain_text, uint8_t *cipher_text, uint32_t data_len)
{
    int32_t ret = drv_cipher_symc_gcm_encrypt_update(handle, plain_text, cipher_text, data_len);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_gcm_encrypt_get_tag(uint32_t handle,
    uint8_t *tag, uint32_t tag_len)
{
    int32_t ret = drv_cipher_symc_gcm_encrypt_get_tag(handle, tag, tag_len);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_gcm_decrypt_set_tag(uint32_t handle,
    const uint8_t *tag, uint32_t tag_len)
{
    int32_t ret = drv_cipher_symc_gcm_decrypt_set_tag(handle, tag, tag_len);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_gcm_decrypt_update(uint32_t handle,
    const uint8_t *cipher_text, uint8_t *plain_text, uint32_t data_len)
{
    int32_t ret = drv_cipher_symc_gcm_decrypt_update(handle, cipher_text, plain_text, data_len);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_gcm_decrypt_verify_tag(uint32_t handle)
{
    int32_t ret = drv_cipher_symc_gcm_decrypt_verify_tag(handle);
    return crypto_sl_common_get_errcode(ret);
}