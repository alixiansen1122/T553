/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CIPHER_H
#define CIPHER_H

#ifdef __cplusplus
#include <cstdint>
#include <cstdio>
#include <cstdbool>
#else
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#endif
#include "nv.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define KEY_LEN              16 // AES128 key length 16, AES256 key length 32
#define AES_BLOCK_SIZE       16
#define ERROR_CODE_GENERAL   (-1)
#define ERROR_SUCCESS         0

#define UT_NV_MAX_LENGTH 4060
#define KEY_ID_START NV_ID_ACE_CIPHER
#define KEY_ID_END 0x2900
#define KEY_ID_COUNT (KEY_ID_END - KEY_ID_START)

typedef struct Sha256Context Sha256Context;

typedef enum {
    CIPHER_AES_ECB,
    CIPHER_AES_CBC
} CipherAesMode;

// 定义填充模式枚举
typedef enum {
    PADDING_PKCS5 = 0,
    PADDING_PKCS7 = 1,
    PADDING_NONE = 2
} PaddingMode;

typedef struct {
    char *text;
    char *key;
    int32_t action;
    int32_t textLen;
    int32_t keyLen;
} CryptData;

typedef struct {
    char *transformation;
    char *ivBuf;
    int32_t ivOffset;
    int32_t ivLen;
} AesIvMode;

typedef struct {
    CryptData data;
    CipherAesMode mode;
    AesIvMode iv;
    PaddingMode paddingMode;
} AesCryptContext;

typedef struct {
    char *data;
    size_t length;
} RsaData;

typedef struct {
    char *trans;
    char *action;
    char *key;
    size_t keyLen;
} RsaKeyData;

// ohos begin
// Add software sha256 and AES interface
Sha256Context* CreateSha256Context(void);
void DestroySha256Context(Sha256Context** ctx);
int32_t Sha256(const unsigned char *input, int ilen, unsigned char output[32]);
void Sha256Starts(Sha256Context *ctx);
int32_t Sha256Update(Sha256Context *ctx, const unsigned char *input, size_t ilen);
void Sha256Finish(Sha256Context *ctx, unsigned char output[32]);

int Base64EncodeInternal(const unsigned char *input, size_t inputLen, char *output, size_t outputLen);
int Base64DecodeInternal(const char *input, size_t inputLen, unsigned char *output, size_t outputLen);
int HmacSha256Compute(const unsigned char *key,
    size_t keyLen, const unsigned char *input, size_t inputLen, unsigned char *output);
int Md5Compute(const unsigned char *input, size_t inputLen, unsigned char *output);
uint16_t InnerGetUsedKeyIds(uint16_t *used_ids, uint16_t max_count);
uint16_t InnerGetAvailableKeyIds(uint16_t *available_ids, uint16_t max_count);
int32_t InnerDeleteKeyFromNv(uint16_t keyId);
int32_t InnerGetKeyFromNv(uint16_t keyId, uint8_t *data, uint16_t *actualKeyLen);
int32_t InnerSaveKeyToNv(unsigned char *keyBuff, uint32_t keyBuffLen, uint16_t keyId, bool overwrite);


// ohos end
int32_t InitAesCryptData(const char *action, const char *text, const char *key,
    const AesIvMode *iv, AesCryptContext *aesCryptCxt);
int32_t CipherAesCrypt(AesCryptContext *aesCryptCxt);
void DeinitAesCryptData(AesCryptContext *aesCryptCxt);
int32_t RsaCrypt(RsaKeyData *key, RsaData *inData, RsaData *outData);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif // CIPHER_H