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
// Add software md5
#include "cipher.h"
#include "mbedtls/md5.h"

// MD5底层计算
int Md5Compute(const unsigned char *input, size_t inputLen, unsigned char *output)
{
    mbedtls_md5_context ctx;
    mbedtls_md5_init(&ctx);

    int ret = mbedtls_md5_starts(&ctx);
    if (ret != 0) {
        goto cleanup;
    }

    ret = mbedtls_md5_update(&ctx, input, inputLen);
    if (ret != 0) {
        goto cleanup;
    }

    ret = mbedtls_md5_finish(&ctx, output);

cleanup:
    mbedtls_md5_free(&ctx);
    return ret;
}
// ohos end