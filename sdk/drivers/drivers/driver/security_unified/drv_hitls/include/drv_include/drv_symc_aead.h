/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides IPC source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-04-18, Create file. \n
 */
#ifndef DRV_SYMC_AEAD_H
#define DRV_SYMC_AEAD_H

#include "crypto_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* old. */

int32_t drv_cipher_symc_gcm_encrypt(
    uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *plain_text, uint8_t *cipher_text, uint32_t data_len,
    uint8_t *tag, uint32_t tag_len
);

int32_t drv_cipher_symc_gcm_decrypt_verify(
    uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *cipher_text, uint8_t *plain_text, uint32_t data_len,
    const uint8_t *tag, uint32_t tag_len
);

/* new. */
int32_t drv_cipher_symc_gcm_create(uint32_t *handle,
    uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    uint32_t tag_len
);

int32_t drv_cipher_symc_gcm_destroy(uint32_t handle);

int32_t drv_cipher_symc_gcm_encrypt_update(uint32_t handle,
    const uint8_t *plain_text, uint8_t *cipher_text, uint32_t data_len
);

int32_t drv_cipher_symc_gcm_encrypt_get_tag(uint32_t handle,
    uint8_t *tag, uint32_t tag_len
);

int32_t drv_cipher_symc_gcm_decrypt_set_tag(uint32_t handle,
    const uint8_t *tag, uint32_t tag_len
);

int32_t drv_cipher_symc_gcm_decrypt_update(uint32_t handle,
    const uint8_t *cipher_text, uint8_t *plain_text, uint32_t data_len
);

int32_t drv_cipher_symc_gcm_decrypt_verify_tag(uint32_t handle);

void crypto_drv_init(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif