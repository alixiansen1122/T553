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
// Add software base64
#include "cipher.h"
#include "mbedtls/base64.h"

// Base64编码底层实现
int Base64EncodeInternal(const unsigned char *input, size_t inputLen, char *output, size_t outputLen)
{
    size_t actualOutputLen = 0;
    return mbedtls_base64_encode((unsigned char *)(output), outputLen, &actualOutputLen, input, inputLen);
}

// Base64解码底层实现
int Base64DecodeInternal(const char *input, size_t inputLen, unsigned char *output, size_t outputLen)
{
    size_t actualOutputLen = 0;
    return mbedtls_base64_decode(
        output, outputLen, &actualOutputLen, (const unsigned char *)(input), inputLen);
}
// ohos end