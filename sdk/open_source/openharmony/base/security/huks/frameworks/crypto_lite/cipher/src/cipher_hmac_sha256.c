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
// Add software hmac-sha256
#include "cipher.h"
#include "cipher_log.h"
#include "mbedtls/md.h"
#include "mbedtls/error.h"
#include <string.h>

// HMAC-SHA256底层计算
int HmacSha256Compute(
    const unsigned char *key, size_t keyLen, const unsigned char *input, size_t inputLen, unsigned char *output)
{
    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    if (md_info == NULL) {
        return -1;
    }

    return mbedtls_md_hmac(md_info, key, keyLen, input, inputLen, output);
}
// ohos end