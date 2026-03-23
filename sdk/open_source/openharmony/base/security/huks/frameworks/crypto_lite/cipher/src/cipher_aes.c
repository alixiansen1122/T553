/*
 * Copyright (c) 2025-2030 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <mbedtls/aes.h>
#include <mbedtls/base64.h>
#include "cipher.h"
#include "securec.h"
#include "cipher_log.h"

#define AES_BYTE_SIZE  128
#define MAX_BUFFER_SIZE (1024 * 16)
#define KEY_LEN 16  // 128-bit key

// 通用填充函数
static int32_t AddPadding(char *data, int32_t dataLen, PaddingMode paddingMode)
{
    if (data == NULL || dataLen < 0) {
        return ERROR_CODE_GENERAL;
    }
    
    if (paddingMode == PADDING_NONE) {
        return dataLen;
    }

    int pad_len = AES_BLOCK_SIZE - (dataLen % AES_BLOCK_SIZE);
    if (pad_len == AES_BLOCK_SIZE) {
        pad_len = (paddingMode == PADDING_NONE) ? 0 : AES_BLOCK_SIZE;
    }
    
    if (pad_len > 0) {
        for (int i = 0; i < pad_len; i++) {
            data[dataLen + i] = (char)pad_len;
        }
    }
    return dataLen + pad_len;
}

// 通用解填充函数
static int32_t RemovePadding(char *data, int32_t dataLen, PaddingMode paddingMode)
{
    if (data == NULL || dataLen <= 0) {
        return ERROR_CODE_GENERAL;
    }
    
    if (paddingMode == PADDING_NONE) {
        return dataLen;
    }

    if (dataLen % AES_BLOCK_SIZE != 0) {
        return ERROR_CODE_GENERAL;
    }

    uint8_t pad_len = data[dataLen - 1];
    if (pad_len < 1 || pad_len > AES_BLOCK_SIZE) {
        return ERROR_CODE_GENERAL;
    }
    
    for (int i = 1; i <= pad_len; i++) {
        if (data[dataLen - i] != pad_len) {
            return ERROR_CODE_GENERAL;
        }
    }
    return dataLen - pad_len;
}

// 解析填充模式和加密模式
static PaddingMode ParsePaddingModeAndType(const char *transformation, CipherAesMode *mode)
{
    PaddingMode padding = PADDING_PKCS5; // 默认PKCS5
    *mode = CIPHER_AES_CBC;             // 默认CBC模式
    
    if (transformation == NULL) {
        return padding;
    }
    
    // 检测填充模式
    if (strstr(transformation, "PKCS5Padding") != NULL) {
        padding = PADDING_PKCS5;
    } else if (strstr(transformation, "PKCS7Padding") != NULL) {
        padding = PADDING_PKCS7;
    } else if (strstr(transformation, "NoPadding") != NULL) {
        padding = PADDING_NONE;
    }
    
    // 检测加密模式
    if (strstr(transformation, "ECB") != NULL) {
        *mode = CIPHER_AES_ECB;
    } else if (strstr(transformation, "CBC") != NULL) {
        *mode = CIPHER_AES_CBC;
    }
    
    return padding;
}

static char *MallocDecodeData(const char *text, size_t *olen)
{
    if (text == NULL) {
        return NULL;
    }
    
    size_t inputLen = strlen(text);
    size_t decodeLen = 0;
    int32_t ret = mbedtls_base64_decode(NULL, 0, &decodeLen, (const unsigned char *)text, inputLen);
    if (ret != MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL) {
        return NULL;
    }

    if (decodeLen == 0) {
        return NULL;
    }
    
    // 分配精确长度，不添加额外终止符
    char *decData = (char *)malloc(decodeLen);
    if (decData == NULL) {
        CIPHER_LOG_E("malloc failed, length:%zu.\n", decodeLen);
        return NULL;
    }
    
    if (mbedtls_base64_decode((unsigned char *)decData, decodeLen, olen,
        (const unsigned char *)text, inputLen) != 0) {
        free(decData);
        CIPHER_LOG_E("decode data failed, text:%s.\n", text);
        return NULL;
    }
    
    return decData;
}

static char *MallocEncodeData(const unsigned char *text, size_t inputLen, size_t *outLen)
{
    if (text == NULL || inputLen == 0) {
        return NULL;
    }
    
    size_t encodeLen = 0;
    int32_t ret = mbedtls_base64_encode(NULL, 0, &encodeLen, text, inputLen);
    if (ret != MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL) {
        return NULL;
    }
    
    if (encodeLen == 0) {
        return NULL;
    }
    
    char *encData = (char *)malloc(encodeLen + 1);
    if (encData == NULL) {
        CIPHER_LOG_E("malloc data failed, expect len:%zu.\n", encodeLen);
        return NULL;
    }
    
    (void)memset_s(encData, encodeLen + 1, 0, encodeLen + 1);
    if (mbedtls_base64_encode((unsigned char *)encData, encodeLen, outLen, text, inputLen) != 0) {
        CIPHER_LOG_E("encode data failed.\n");
        free(encData);
        return NULL;
    }
    
    return encData;
}

// 使用二进制数据处理IV
static int32_t SetIv(const unsigned char *ivBuf, size_t ivBufSize, int32_t ivOffset, int32_t ivLen, AesCryptContext *ctx)
{
    if (ctx == NULL) {
        CIPHER_LOG_E("SetIv: NULL context\n");
        return ERROR_CODE_GENERAL;
    }
    
    // ECB模式不需要IV
    if (ctx->mode == CIPHER_AES_ECB) {
        ctx->iv.ivBuf = NULL;
        ctx->iv.ivLen = 0;
        ctx->iv.ivOffset = 0;
        return ERROR_SUCCESS;
    }
    
    if (ivBuf == NULL) {
        CIPHER_LOG_E("SetIv: NULL ivBuf\n");
        return ERROR_CODE_GENERAL;
    }
    
    if (ivOffset < 0 || ivLen <= 0) {
        CIPHER_LOG_E("ivLen or ivOffset negative err.\n");
        return ERROR_CODE_GENERAL;
    }
    
    if (ivBufSize < (size_t)(ivOffset + ivLen)) {
        CIPHER_LOG_E("ivBufSize too small: %zu < %d\n",
                    ivBufSize, ivOffset + ivLen);
        return ERROR_CODE_GENERAL;
    }
    
    ctx->iv.ivBuf = malloc(ivLen);
    if (ctx->iv.ivBuf == NULL) {
        CIPHER_LOG_E("malloc failed for IV.\n");
        return ERROR_CODE_GENERAL;
    }
    
    int32_t ret = memcpy_s(ctx->iv.ivBuf, ivLen, ivBuf + ivOffset, ivLen);
    if (ret != EOK) {
        CIPHER_LOG_E("memcpy failed, ret:%d.\n", ret);
        free(ctx->iv.ivBuf);
        ctx->iv.ivBuf = NULL;
        return ERROR_CODE_GENERAL;
    }

    ctx->iv.ivLen = ivLen;
    return ERROR_SUCCESS;
}

static void GetPaddingLen(char *data, int32_t *paddingLen)
{
    uint8_t pad_len = data[*paddingLen - 1];
    for (int i = 0; i < *paddingLen; i++) {
        uint8_t temp = data[i];
        if (temp > 0 && temp <= AES_BLOCK_SIZE && temp == pad_len) {
            *paddingLen = i;
            break;
        }
    }
}

static int32_t InitAesCryptContext(const char *key, const AesIvMode *iv, AesCryptContext *ctx)
{
    int32_t ret;
    if (iv == NULL || ctx == NULL || key == NULL) {
        CIPHER_LOG_E("InitAesCryptContext: NULL parameter\n");
        return ERROR_CODE_GENERAL;
    }
    
    // 解析填充模式和加密模式
    ctx->paddingMode = ParsePaddingModeAndType(iv->transformation, &ctx->mode);
    
    ctx->iv.ivOffset = iv->ivOffset;
    ctx->iv.ivLen = (ctx->mode == CIPHER_AES_CBC) ? AES_BLOCK_SIZE : 0;

    // ECB模式不需要IV长度检查
    if (ctx->mode == CIPHER_AES_CBC) {
        if (iv->ivLen != AES_BLOCK_SIZE) {
            CIPHER_LOG_E("ivLen:%d error, need be %d Bytes.\n",
                         iv->ivLen, AES_BLOCK_SIZE);
            return ERROR_CODE_GENERAL;
        }
        
        // 处理Base64编码的IV
        if (iv->ivBuf != NULL) {
            size_t decodedIvLen = 0;
            char* decodedIvBuf = MallocDecodeData(iv->ivBuf, &decodedIvLen);
            if (decodedIvBuf == NULL) {
                CIPHER_LOG_E("base64 decode failed for IV.\n");
                return ERROR_CODE_GENERAL;
            }
            
            // 使用二进制数据设置IV
            ret = SetIv((const unsigned char *)decodedIvBuf, decodedIvLen,
                        ctx->iv.ivOffset, ctx->iv.ivLen, ctx);
            free(decodedIvBuf);
            
            if (ret != ERROR_SUCCESS) {
                CIPHER_LOG_E("SetIv failed after decoding, ret:%d.\n", ret);
                return ERROR_CODE_GENERAL;
            }
        } else {
            // 直接使用二进制密钥数据
            if (ctx->data.key == NULL) {
                CIPHER_LOG_E("Key is NULL when using as IV\n");
                return ERROR_CODE_GENERAL;
            }
            
            ret = SetIv((const unsigned char *)ctx->data.key, ctx->data.keyLen,
                        ctx->iv.ivOffset, ctx->iv.ivLen, ctx);
            if (ret != ERROR_SUCCESS) {
                CIPHER_LOG_E("SetIv from key failed, ret:%d.\n", ret);
                return ERROR_CODE_GENERAL;
            }
        }
    } else {
        // ECB模式不需要IV
        ctx->iv.ivBuf = NULL;
        ctx->iv.ivLen = 0;
    }
    return ERROR_SUCCESS;
}

static int32_t InitAesCryptDataText(const char *action, const char *key, const char *text, CryptData *data, PaddingMode paddingMode)
{
    if (action == NULL || text == NULL || data == NULL) {
        CIPHER_LOG_E("InitAesCryptDataText: NULL parameter\n");
        return ERROR_CODE_GENERAL;
    }
    
    if (strcmp(action, "encrypt") == 0) {
        data->action = MBEDTLS_AES_ENCRYPT;
        int32_t originalLen = strlen(text);
        
        // 计算填充后长度
        int32_t paddedLen;
        if (paddingMode == PADDING_NONE) {
            paddedLen = originalLen;
            // 无填充模式需要长度是块大小的倍数
            if (paddedLen % AES_BLOCK_SIZE != 0) {
                CIPHER_LOG_E("Text length %d not multiple of block size for NoPadding\n", paddedLen);
                return ERROR_CODE_GENERAL;
            }
        } else {
            int pad_len = AES_BLOCK_SIZE - (originalLen % AES_BLOCK_SIZE);
            pad_len = (pad_len == 0) ? AES_BLOCK_SIZE : pad_len;
            paddedLen = originalLen + pad_len;
        }

        if (paddedLen <= 0) {
            CIPHER_LOG_E("Invalid padded length: %d\n", paddedLen);
            return ERROR_CODE_GENERAL;
        }
        
        data->text = malloc(paddedLen + 1);
        if (data->text == NULL) {
            CIPHER_LOG_E("malloc failed for text\n");
            return ERROR_CODE_GENERAL;
        }
        
        (void)memset_s(data->text, paddedLen + 1, 0, paddedLen + 1);
        
        if (memcpy_s(data->text, paddedLen + 1, text, originalLen) != EOK) {
            CIPHER_LOG_E("memcpy_s failed for text\n");
            free(data->text);
            data->text = NULL;
            return ERROR_CODE_GENERAL;
        }

        // 添加填充
        if (paddingMode != PADDING_NONE) {
            data->textLen = AddPadding(data->text, originalLen, paddingMode);
            if (data->textLen != paddedLen) {
                CIPHER_LOG_E("Padding failed: expected %d, got %d\n", paddedLen, data->textLen);
                free(data->text);
                data->text = NULL;
                return ERROR_CODE_GENERAL;
            }
        } else {
            data->textLen = paddedLen;
        }
    } else if (strcmp(action, "decrypt") == 0) {
        data->action = MBEDTLS_AES_DECRYPT;
        size_t textLen = 0;
        data->text = MallocDecodeData(text, &textLen);
        if (data->text == NULL) {
            CIPHER_LOG_E("Base64 decode failed for ciphertext\n");
            return ERROR_CODE_GENERAL;
        }
        data->textLen = (uint32_t)textLen;
        
        // 无填充模式不需要对齐块大小
        if (paddingMode != PADDING_NONE) {
            // 确保数据长度是块大小的整数倍
            if (data->textLen % AES_BLOCK_SIZE != 0) {
                CIPHER_LOG_E("Ciphertext length %d not multiple of block size\n", data->textLen);
                free(data->text);
                data->text = NULL;
                return ERROR_CODE_GENERAL;
            }
        }
    } else {
        CIPHER_LOG_E("Invalid action: %s\n", action);
        return ERROR_CODE_GENERAL;
    }
    return ERROR_SUCCESS;
}

static int32_t InitAesData(const char *action, const char *key, const char *text, CryptData *data, PaddingMode paddingMode)
{
    if (action == NULL || text == NULL || data == NULL || key == NULL) {
        CIPHER_LOG_E("InitAesData: NULL parameter\n");
        return ERROR_CODE_GENERAL;
    }
    
    // 先初始化文本数据
    int32_t ret = InitAesCryptDataText(action, key, text, data, paddingMode);
    if (ret != ERROR_SUCCESS) {
        CIPHER_LOG_E("InitAesCryptDataText failed\n");
        return ERROR_CODE_GENERAL;
    }
    
    // 解码Base64编码的密钥
    size_t keyLen = 0;
    data->key = MallocDecodeData(key, &keyLen);
    if (data->key == NULL) {
        CIPHER_LOG_E("Base64 decode failed for key\n");
        goto ERROR;
    }
    
    // 验证密钥长度（使用实际解码长度）
    if (keyLen != KEY_LEN) {
        CIPHER_LOG_E("key length:%zu error, need be %d Bytes.\n", keyLen, KEY_LEN);
        (void)memset_s(data->key, keyLen, 0, keyLen);
        free(data->key);
        data->key = NULL;
        goto ERROR;
    }
    
    // 使用实际解码长度
    data->keyLen = keyLen;
    return ERROR_SUCCESS;

ERROR:
    if (data->text != NULL) {
        free(data->text);
        data->text = NULL;
    }
    return ERROR_CODE_GENERAL;
}

void DeinitAesCryptData(AesCryptContext *aesCryptCxt)
{
    if (aesCryptCxt == NULL) {
        return;
    }

    if (aesCryptCxt->iv.ivBuf != NULL) {
        free(aesCryptCxt->iv.ivBuf);
        aesCryptCxt->iv.ivBuf = NULL;
    }

    if (aesCryptCxt->data.key != NULL) {
        (void)memset_s(aesCryptCxt->data.key, aesCryptCxt->data.keyLen, 0, aesCryptCxt->data.keyLen);
        free(aesCryptCxt->data.key);
        aesCryptCxt->data.key = NULL;
    }

    if (aesCryptCxt->data.text != NULL) {
        free(aesCryptCxt->data.text);
        aesCryptCxt->data.text = NULL;
    }
}

static int32_t DoAesCbcEncrypt(mbedtls_aes_context *aesCtx, AesCryptContext *ctx)
{
    int32_t ret;
    if (ctx->data.action == MBEDTLS_AES_ENCRYPT) {
        ret = mbedtls_aes_setkey_enc(aesCtx, (const unsigned char *)ctx->data.key, AES_BYTE_SIZE);
    } else {
        ret = mbedtls_aes_setkey_dec(aesCtx, (const unsigned char *)ctx->data.key, AES_BYTE_SIZE);
    }
    if (ret != 0) {
        CIPHER_LOG_E("aes setkey error, ret:%d.\n", ret);
        return ERROR_CODE_GENERAL;
    }
    
    ret = mbedtls_aes_crypt_cbc(aesCtx, ctx->data.action, ctx->data.textLen,
        (unsigned char *)ctx->iv.ivBuf, (const unsigned char *)ctx->data.text, (unsigned char *)ctx->data.text);
    if (ret != 0) {
        CIPHER_LOG_E("aes crypt cbc error, ret:%d.\n", ret);
        return ERROR_CODE_GENERAL;
    }
    
    if (ctx->data.action == MBEDTLS_AES_ENCRYPT) {
        size_t textLen = ctx->data.textLen;
        char *out = MallocEncodeData((const unsigned char *)ctx->data.text, textLen, &textLen);
        if (out == NULL) {
            CIPHER_LOG_E("base64 encode failed.\n");
            return ERROR_CODE_GENERAL;
        }
        ctx->data.textLen = (uint32_t)textLen;
        free(ctx->data.text);
        ctx->data.text = out;
    } else {
        // 使用上下文中的填充模式进行解填充
        int32_t unpaddedLen = RemovePadding(ctx->data.text, ctx->data.textLen, ctx->paddingMode);
        if (unpaddedLen < 0) {
            CIPHER_LOG_E("remove padding failed.\n");
            return ERROR_CODE_GENERAL;
        }
        if (ctx->paddingMode == PADDING_NONE) {
            GetPaddingLen(ctx->data.text, &unpaddedLen);
        }
        ctx->data.textLen = unpaddedLen;
        
        // 添加字符串终止符
        ctx->data.text[unpaddedLen] = '\0';
    }

    return ERROR_SUCCESS;
}

// ECB模式加解密函数
static int32_t DoAesEcbEncrypt(mbedtls_aes_context *aesCtx, AesCryptContext *ctx)
{
    int32_t ret;
    if (ctx->data.action == MBEDTLS_AES_ENCRYPT) {
        ret = mbedtls_aes_setkey_enc(aesCtx, (const unsigned char *)ctx->data.key, AES_BYTE_SIZE);
    } else {
        ret = mbedtls_aes_setkey_dec(aesCtx, (const unsigned char *)ctx->data.key, AES_BYTE_SIZE);
    }
    if (ret != 0) {
        CIPHER_LOG_E("aes setkey error, ret:%d.\n", ret);
        return ERROR_CODE_GENERAL;
    }
    
    // ECB模式需要分块处理
    int32_t blockCount = ctx->data.textLen / AES_BLOCK_SIZE;
    for (int i = 0; i < blockCount; i++) {
        uint8_t input[AES_BLOCK_SIZE];
        uint8_t output[AES_BLOCK_SIZE];
        memcpy(input, ctx->data.text + i * AES_BLOCK_SIZE, AES_BLOCK_SIZE);
        
        ret = mbedtls_aes_crypt_ecb(aesCtx, ctx->data.action, input, output);
        if (ret != 0) {
            CIPHER_LOG_E("aes crypt ecb error at block %d, ret:%d.\n", i, ret);
            return ERROR_CODE_GENERAL;
        }
        
        memcpy(ctx->data.text + i * AES_BLOCK_SIZE, output, AES_BLOCK_SIZE);
    }
    
    if (ctx->data.action == MBEDTLS_AES_ENCRYPT) {
        size_t textLen = ctx->data.textLen;
        char *out = MallocEncodeData((const unsigned char *)ctx->data.text, textLen, &textLen);
        if (out == NULL) {
            CIPHER_LOG_E("base64 encode failed.\n");
            return ERROR_CODE_GENERAL;
        }
        ctx->data.textLen = (uint32_t)textLen;
        free(ctx->data.text);
        ctx->data.text = out;
    } else {
        // 使用上下文中的填充模式进行解填充
        int32_t unpaddedLen = RemovePadding(ctx->data.text, ctx->data.textLen, ctx->paddingMode);
        if (unpaddedLen < 0) {
            CIPHER_LOG_E("remove padding failed.\n");
            return ERROR_CODE_GENERAL;
        }
        if (ctx->paddingMode == PADDING_NONE) {
            GetPaddingLen(ctx->data.text, &unpaddedLen);
        }
        ctx->data.textLen = unpaddedLen;
        
        // 添加字符串终止符
        ctx->data.text[unpaddedLen] = '\0';
    }

    return ERROR_SUCCESS;
}

int32_t InitAesCryptData(const char *action, const char *text, const char *key, const AesIvMode *iv,
    AesCryptContext *aesCryptCxt)
{
    if (action == NULL || text == NULL || key == NULL || iv == NULL || aesCryptCxt == NULL) {
        CIPHER_LOG_E("InitAesCryptData: NULL parameter\n");
        return ERROR_CODE_GENERAL;
    }
    
    // 初始化填充模式和加密模式
    aesCryptCxt->paddingMode = ParsePaddingModeAndType(iv->transformation, &aesCryptCxt->mode);
    
    // 初始化数据
    int32_t ret = InitAesData(action, key, text, &(aesCryptCxt->data), aesCryptCxt->paddingMode);
    if (ret != ERROR_SUCCESS) {
        CIPHER_LOG_E("InitAesData failed, ret:%d.\n", ret);
        DeinitAesCryptData(aesCryptCxt);
        return ERROR_CODE_GENERAL;
    }
    
    // 初始化上下文（包括IV）
    ret = InitAesCryptContext(key, iv, aesCryptCxt);
    if (ret != ERROR_SUCCESS) {
        CIPHER_LOG_E("InitAesCryptContext failed, ret:%d.\n", ret);
        DeinitAesCryptData(aesCryptCxt);
        return ERROR_CODE_GENERAL;
    }
    return ERROR_SUCCESS;
}

int32_t CipherAesCrypt(AesCryptContext* aesCryptCxt)
{
    if (aesCryptCxt == NULL) {
        CIPHER_LOG_E("AesCrypt: NULL context\n");
        return ERROR_CODE_GENERAL;
    }
    
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    int32_t ret = ERROR_CODE_GENERAL;
    
    if (aesCryptCxt->mode == CIPHER_AES_CBC) {
        ret = DoAesCbcEncrypt(&aes, aesCryptCxt);
        if (ret != ERROR_SUCCESS) {
            CIPHER_LOG_E("Aes cbc crypt failed, ret:%d.\n", ret);
        }
    } else if (aesCryptCxt->mode == CIPHER_AES_ECB) {
        ret = DoAesEcbEncrypt(&aes, aesCryptCxt);
        if (ret != ERROR_SUCCESS) {
            CIPHER_LOG_E("Aes ecb crypt failed, ret:%d.\n", ret);
        }
    } else {
        CIPHER_LOG_E("unsupported crypt mode: %d\n", aesCryptCxt->mode);
    }
    
    mbedtls_aes_free(&aes);
    return ret;
}
