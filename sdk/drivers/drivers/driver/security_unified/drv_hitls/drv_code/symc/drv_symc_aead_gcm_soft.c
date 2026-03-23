/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides IPC source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-04-18, Create file. \n
 */
#include "drv_symc_aead.h"

#include "crypt_modes_gcm.h"
#include "crypt_errno.h"
#include "crypt_aes.h"

#include "crypto_drv_common.h"

#define MAX_TAG_LEN     16

typedef int32_t (*encType)(void *ctx, const uint8_t *key, uint32_t len);
typedef int32_t (*updateType)(void *ctx, const uint8_t *in, uint8_t *out, uint32_t len);
typedef int32_t (*ctrlType)(void *ctx, uint32_t opt, void *val, uint32_t len);

static const EAL_CipherMethod aes128 = {
    .initCtx = NULL,
    .deinitCtx = NULL,
    .clean = NULL,
    .setEncryptKey = (encType)CRYPT_AES_SetEncryptKey128,
    .setDecryptKey = (encType)CRYPT_AES_SetDecryptKey128,
    .encrypt = (updateType)CRYPT_AES_Encrypt,
    .decrypt = (updateType)CRYPT_AES_Decrypt,
    .ctrl = (ctrlType)NULL,
    .blockSize = 1,
    .ctxSize = sizeof(CRYPT_AES_Key),
    .algId = CRYPT_SYM_AES128
};

#define SYMC_CTX_NUM        2
typedef struct {
    bool is_open;
    MODES_GCM_Ctx gcm_ctx;
    uint8_t tag[16];
    uint32_t tag_len;
} drv_symc_hitls_ctx;

static drv_symc_hitls_ctx g_symc_ctx[SYMC_CTX_NUM];

static int32_t inner_alloc_ctx(uint32_t *handle)
{
    uint32_t i;
    for (i = 0; i < SYMC_CTX_NUM; i++) {
        if (g_symc_ctx[i].is_open == false) {
            *handle = i;
            g_symc_ctx[i].is_open = true;
            return CRYPTO_SUCCESS;
        }
    }
    return CRYPTO_FAILURE;
}

static void inner_free_ctx(uint32_t handle)
{
    drv_symc_hitls_ctx *ctx = NULL;
    if (handle > SYMC_CTX_NUM) {
        return;
    }
    ctx = &g_symc_ctx[handle];
    (void)memset_s(ctx, sizeof(drv_symc_hitls_ctx), 0, sizeof(drv_symc_hitls_ctx));
}

static drv_symc_hitls_ctx *inner_get_ctx(uint32_t handle)
{
    drv_symc_hitls_ctx *ctx = NULL;

    if (handle > SYMC_CTX_NUM) {
        crypto_log_err("invalid handle\n");
        return NULL;
    }

    ctx = &g_symc_ctx[handle];
    if (ctx->is_open == false) {
        crypto_log_err("handle is closed\n");
        return NULL;
    }
    return ctx;
}

int32_t drv_cipher_symc_gcm_create(uint32_t *handle,
    uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len, uint32_t tag_len
)
{
    int32_t ret;
    drv_symc_hitls_ctx *ctx = NULL;
    MODES_GCM_Ctx *gcm_ctx = NULL;
    uint32_t out_tag_len = tag_len;
    crypto_unused(keyslot_handle);

    ret = inner_alloc_ctx(handle);
    crypto_chk_return(ret != CRYPT_SUCCESS, ret, "inner_alloc_ctx failed\n");

    ctx = &g_symc_ctx[*handle];
    gcm_ctx = &ctx->gcm_ctx;

    ret = MODES_GCM_InitCtx(gcm_ctx, &aes128);
    crypto_chk_goto_with_ret(ret, ret != CRYPT_SUCCESS, error_free_ctx, CRYPTO_FAILURE, "MODES_GCM_InitCtx failed\n");

    ret = MODES_GCM_SetKey(gcm_ctx, key, key_len);
    crypto_chk_goto_with_ret(ret, ret != CRYPT_SUCCESS, error_clean, CRYPTO_FAILURE, "MODES_GCM_SetKey failed\n");

    ret = MODES_GCM_Ctrl(gcm_ctx, CRYPT_CTRL_SET_TAGLEN, (void *)&out_tag_len, sizeof(out_tag_len));
    crypto_chk_goto_with_ret(ret, ret != CRYPT_SUCCESS, error_clean, CRYPTO_FAILURE, "MODES_GCM_Ctrl failed\n");

    ret = MODES_GCM_Ctrl(gcm_ctx, CRYPT_CTRL_SET_IV, (void *)iv, iv_len);
    crypto_chk_goto_with_ret(ret, ret != CRYPT_SUCCESS, error_clean, CRYPTO_FAILURE, "MODES_GCM_Ctrl failed\n");

    ret = MODES_GCM_Ctrl(gcm_ctx, CRYPT_CTRL_SET_AAD, (void *)aad, aad_len);
    crypto_chk_goto_with_ret(ret, ret != CRYPT_SUCCESS, error_clean, CRYPTO_FAILURE, "MODES_GCM_Ctrl failed\n");

    ctx->tag_len = tag_len;
    return CRYPTO_SUCCESS;

error_clean:
    MODES_GCM_DeinitCtx(&ctx->gcm_ctx);
error_free_ctx:
    inner_free_ctx(*handle);
    return ret;
}

int32_t drv_cipher_symc_gcm_destroy(uint32_t handle)
{
    drv_symc_hitls_ctx *ctx = NULL;

    if (handle > SYMC_CTX_NUM) {
        crypto_log_err("invalid handle\n");
        return CRYPTO_FAILURE;
    }
    ctx = &g_symc_ctx[handle];

    MODES_GCM_DeinitCtx(&ctx->gcm_ctx);
    inner_free_ctx(handle);

    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_symc_gcm_encrypt_update(uint32_t handle,
    const uint8_t *plain_text, uint8_t *cipher_text, uint32_t data_len
)
{
    int32_t ret;
    drv_symc_hitls_ctx *ctx = NULL;

    ctx = inner_get_ctx(handle);
    crypto_chk_return(ctx == NULL, CRYPTO_FAILURE, "inner_get_ctx failed\n");

    ret = MODES_GCM_Encrypt(&ctx->gcm_ctx, plain_text, cipher_text, data_len);
    crypto_chk_return(ret != CRYPT_SUCCESS, CRYPTO_FAILURE, "MODES_GCM_Encrypt failed\n");

    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_symc_gcm_encrypt_get_tag(uint32_t handle,
    uint8_t *tag, uint32_t tag_len
)
{
    int32_t ret;
    drv_symc_hitls_ctx *ctx = NULL;

    ctx = inner_get_ctx(handle);
    crypto_chk_return(ctx == NULL, CRYPTO_FAILURE, "inner_get_ctx failed\n");

    ret = MODES_GCM_Ctrl(&ctx->gcm_ctx, CRYPT_CTRL_GET_TAG, tag, tag_len);
    crypto_chk_return(ret != CRYPT_SUCCESS, CRYPTO_FAILURE, "MODES_GCM_Ctrl failed\n");

    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_symc_gcm_decrypt_set_tag(uint32_t handle,
    const uint8_t *tag, uint32_t tag_len
)
{
    int32_t ret;
    drv_symc_hitls_ctx *ctx = NULL;

    ctx = inner_get_ctx(handle);
    crypto_chk_return(ctx == NULL, CRYPTO_FAILURE, "inner_get_ctx failed\n");

    ret = memcpy_s(ctx->tag, sizeof(ctx->tag), tag, tag_len);
    crypto_chk_return(ret != EOK, CRYPTO_FAILURE, "memcpy_s failed\n");

    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_symc_gcm_decrypt_update(uint32_t handle,
    const uint8_t *cipher_text, uint8_t *plain_text, uint32_t data_len
)
{
    int32_t ret;
    drv_symc_hitls_ctx *ctx = NULL;

    ctx = inner_get_ctx(handle);
    crypto_chk_return(ctx == NULL, CRYPTO_FAILURE, "inner_get_ctx failed\n");

    ret = MODES_GCM_Decrypt(&ctx->gcm_ctx, cipher_text, plain_text, data_len);
    crypto_chk_return(ret != CRYPT_SUCCESS, CRYPTO_FAILURE, "MODES_GCM_Decrypt failed\n");

    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_symc_gcm_decrypt_verify_tag(uint32_t handle)
{
    int32_t ret;
    uint32_t i;
    uint8_t tag_check = 0;
    uint8_t out_tag[MAX_TAG_LEN];
    drv_symc_hitls_ctx *ctx = NULL;

    ctx = inner_get_ctx(handle);
    crypto_chk_return(ctx == NULL, CRYPTO_FAILURE, "inner_get_ctx failed\n");

    ret = MODES_GCM_Ctrl(&ctx->gcm_ctx, CRYPT_CTRL_GET_TAG, out_tag, ctx->tag_len);
    crypto_chk_return(ret != CRYPT_SUCCESS, CRYPTO_FAILURE, "MODES_GCM_Ctrl failed\n");

    /* tag compare. */
    for (i = 0; i < ctx->tag_len; i++) {
        tag_check |= ctx->tag[i] ^ out_tag[i];
    }
    if (tag_check != 0) {
        crypto_log_err("gcm tag auth failed\n");
        return CRYPTO_FAILURE;
    }
    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_symc_gcm_encrypt(
    uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *plain_text, uint8_t *cipher_text, uint32_t data_len,
    uint8_t *tag, uint32_t tag_len
)
{
    int32_t ret;
    MODES_GCM_Ctx ctx;
    uint32_t out_tag_len = tag_len;

    crypto_unused(keyslot_handle);

    ret = MODES_GCM_InitCtx(&ctx, &aes128);
    crypto_chk_return(ret != CRYPT_SUCCESS, ret, "MODES_GCM_InitCtx failed\n");

    ret = MODES_GCM_SetKey(&ctx, key, key_len);
    crypto_chk_goto_with_ret(ret, ret != CRYPT_SUCCESS, exit_clean, CRYPTO_FAILURE, "MODES_GCM_SetKey failed\n");

    ret = MODES_GCM_Ctrl(&ctx, CRYPT_CTRL_SET_TAGLEN, (void *)&out_tag_len, sizeof(out_tag_len));
    crypto_chk_goto_with_ret(ret, ret != CRYPT_SUCCESS, exit_clean, CRYPTO_FAILURE, "MODES_GCM_Ctrl failed\n");

    ret = MODES_GCM_Ctrl(&ctx, CRYPT_CTRL_SET_IV, (void *)iv, iv_len);
    crypto_chk_goto_with_ret(ret, ret != CRYPT_SUCCESS, exit_clean, CRYPTO_FAILURE, "MODES_GCM_Ctrl failed\n");

    ret = MODES_GCM_Ctrl(&ctx, CRYPT_CTRL_SET_AAD, (void *)aad, aad_len);
    crypto_chk_goto_with_ret(ret, ret != CRYPT_SUCCESS, exit_clean, CRYPTO_FAILURE, "MODES_GCM_Ctrl failed\n");

    ret = MODES_GCM_Encrypt(&ctx, plain_text, cipher_text, data_len);
    crypto_chk_goto_with_ret(ret, ret != CRYPT_SUCCESS, exit_clean, CRYPTO_FAILURE, "MODES_GCM_Encrypt failed\n");

    ret = MODES_GCM_Ctrl(&ctx, CRYPT_CTRL_GET_TAG, tag, out_tag_len);
    crypto_chk_goto_with_ret(ret, ret != CRYPT_SUCCESS, exit_clean, CRYPTO_FAILURE, "MODES_GCM_Ctrl failed\n");

exit_clean:
    MODES_GCM_DeinitCtx(&ctx);
    return ret;
}

int32_t drv_cipher_symc_gcm_decrypt_verify(
    uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *cipher_text, uint8_t *plain_text, uint32_t data_len,
    const uint8_t *tag, uint32_t tag_len
)
{
    int32_t ret;
    MODES_GCM_Ctx ctx;
    uint8_t out_tag[MAX_TAG_LEN];
    uint32_t out_tag_len = tag_len;
    uint8_t tag_check = 0;
    uint32_t i;

    crypto_unused(keyslot_handle);

    crypto_chk_return(tag_len > MAX_TAG_LEN, CRYPTO_FAILURE, "tag_len is too long\n");

    ret = MODES_GCM_InitCtx(&ctx, &aes128);
    crypto_chk_return(ret != CRYPT_SUCCESS, ret, "MODES_GCM_InitCtx failed\n");

    ret = MODES_GCM_SetKey(&ctx, key, key_len);
    crypto_chk_goto_with_ret(ret, ret != CRYPT_SUCCESS, exit_clean, CRYPTO_FAILURE, "MODES_GCM_SetKey failed\n");

    ret = MODES_GCM_Ctrl(&ctx, CRYPT_CTRL_SET_TAGLEN, (void *)&out_tag_len, sizeof(out_tag_len));
    crypto_chk_goto_with_ret(ret, ret != CRYPT_SUCCESS, exit_clean, CRYPTO_FAILURE, "MODES_GCM_Ctrl failed\n");

    ret = MODES_GCM_Ctrl(&ctx, CRYPT_CTRL_SET_IV, (void *)iv, iv_len);
    crypto_chk_goto_with_ret(ret, ret != CRYPT_SUCCESS, exit_clean, CRYPTO_FAILURE, "MODES_GCM_Ctrl failed\n");

    ret = MODES_GCM_Ctrl(&ctx, CRYPT_CTRL_SET_AAD, (void *)aad, aad_len);
    crypto_chk_goto_with_ret(ret, ret != CRYPT_SUCCESS, exit_clean, CRYPTO_FAILURE, "MODES_GCM_Ctrl failed\n");

    ret = MODES_GCM_Decrypt(&ctx, cipher_text, plain_text, data_len);
    crypto_chk_goto_with_ret(ret, ret != CRYPT_SUCCESS, exit_clean, CRYPTO_FAILURE, "MODES_GCM_Decrypt failed\n");

    ret = MODES_GCM_Ctrl(&ctx, CRYPT_CTRL_GET_TAG, out_tag, out_tag_len);
    if (ret != CRYPT_SUCCESS) {
        crypto_log_err("MODES_GCM_Ctrl failed\n");
        ret = CRYPTO_FAILURE;
        (void)memset_s(plain_text, data_len, 0, data_len);
        goto exit_clean;
    }

    /* tag compare. */
    for (i = 0; i < tag_len; i++) {
        tag_check |= tag[i] ^ out_tag[i];
    }
    if (tag_check != 0) {
        crypto_log_err("gcm tag auth failed\n");
        ret = CRYPTO_FAILURE;
        (void)memset_s(plain_text, data_len, 0, data_len);
        goto exit_clean;
    }

    ret = CRYPTO_SUCCESS;
exit_clean:
    MODES_GCM_DeinitCtx(&ctx);
    return ret;
}