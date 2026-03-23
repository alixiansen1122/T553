/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_CIPHER_MODULE_H
#define OHOS_ACELITE_CIPHER_MODULE_H

#include "jsi.h"
#include "cipher.h"

namespace OHOS {
namespace ACELite {
class CipherModule final : public MemoryHeap {
public:
    CipherModule() {}
    ~CipherModule() {}
    static JSIValue Rsa(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue Aes(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
#ifdef HUKS_LITEWEARABLE_SUPPORT_ENABLED
    static JSIValue Sha256All(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue Sha256SubStarts(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue Sha256SubUpdate(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue Sha256SubFinish(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);

    static JSIValue HmacSha256(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue HmacSha256Bytes(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue Md5FromString(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue Md5FromBytes(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue Base64Encode(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue Base64Decode(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue GetAvailableKeyIds(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue GetUsedKeyIds(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue DeleteKeyFromNv(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue GetKeyFromNv(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue SaveKeyToNv(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);

    static void OnDestroy();
#endif
private:
    static const int HMAC_SHA256_LEN = 32; // SHA1不安全，否则长度为20
    static const int HMAC_SHA256_RESULT_LEN = 64;
    static const int MD5_LEN = 16;
    static const int MD5_RESULT_LEN = 32;

    static void ResetStrBuf(char *strBuf);
    static CipherAesMode GetMode(char *strTransformation);
    static PaddingMode GetPaddingMode(char *strTransformation);
    static constexpr uint8_t ERR_CODE = 200;
};

void InitCipherModule(JSIValue exports);
} // namespace ACELite
} // namespace OHOS
#ifdef HUKS_LITEWEARABLE_SUPPORT_ENABLED
int32_t CipherMsgToJS(const char *msgBody, OHOS::ACELite::JSIValue context, OHOS::ACELite::JSIValue callback);
#endif
#endif // OHOS_ACELITE_CIPHER_MODULE_H
