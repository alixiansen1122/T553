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

// ohos begin
// Add software sha256 and AES interface
#include "cipher.h"
#include "mbedtls/sha256.h"

#define SHA256_HASH_SIZE 32

struct Sha256Context {
    mbedtls_sha256_context mbed_ctx;
};

Sha256Context* CreateSha256Context(void)
{
    Sha256Context* ctx = malloc(sizeof(Sha256Context));
    mbedtls_sha256_init(&ctx->mbed_ctx);
    return ctx;
}

void DestroySha256Context(Sha256Context** ctx)
{
    if (*ctx) {
        mbedtls_sha256_free(&(*ctx)->mbed_ctx);
        free(*ctx);
        *ctx = NULL;
    }
}

void Sha256Starts(Sha256Context *ctx)
{
    mbedtls_sha256_starts(&ctx->mbed_ctx, 0); // 0 represents SHA-256
}

int32_t Sha256Update(Sha256Context *ctx, const unsigned char *input, size_t ilen)
{
    return mbedtls_sha256_update(&ctx->mbed_ctx, input, ilen);
}

void Sha256Finish(Sha256Context *ctx, unsigned char output[SHA256_HASH_SIZE])
{
    mbedtls_sha256_finish(&ctx->mbed_ctx, output);
}

int32_t Sha256(const unsigned char *input, int ilen, unsigned char output[SHA256_HASH_SIZE])
{
    return mbedtls_sha256(input, (size_t)ilen, output, 0);
}
// ohos end