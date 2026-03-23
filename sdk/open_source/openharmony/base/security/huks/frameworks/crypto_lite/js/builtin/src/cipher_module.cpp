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

#include "cipher_module.h"
#include "log.h"
#include "securec.h"
// OHOS BEGIN
#include <string>
#ifdef HUKS_LITEWEARABLE_SUPPORT_ENABLED
#include "app_common.h"
#include "mbedtls/sha256.h"
#include "mbedtls/base64.h"
#include "js_async_work.h"
#endif

const int8_t SHA256_LEN = 32;
const int8_t SHA256_RESULT_LEN = SHA256_LEN * 2;
const int32_t MAX_LEN = 16384;
const int32_t MAX_TXT_LEN = 4096;
const int32_t MAX_KEY_LEN = 64;

struct CipherMsgParams {
    char *msgBody;
    uint8_t type;
    OHOS::ACELite::JSIValue context;
    OHOS::ACELite::JSIValue callback;
    bool flag;
};
#ifdef HUKS_LITEWEARABLE_SUPPORT_ENABLED
struct Sha256Context {
    mbedtls_sha256_context context;
};

Sha256Context *g_sha256Context;
#endif
// OHOS END
namespace OHOS {
namespace ACELite {
void CipherModule::ResetStrBuf(char *strBuf)
{
    if (strBuf == nullptr) {
        return;
    }

    int ret = memset_s(strBuf, strlen(strBuf), 0, strlen(strBuf));
    if (ret != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "memset failed.");
    }
}

static int32_t CheckTxtLen(char *text)
{
    uint32_t textLen = strlen(text);
    if (textLen == 0 || textLen > MAX_TXT_LEN) {
        return ERROR_CODE_GENERAL;
    }
    return ERROR_SUCCESS;
}

static int32_t CheckKeyLen(char *key)
{
    uint32_t keyLen = strlen(key);
    if (keyLen == 0 || keyLen > MAX_KEY_LEN * 2) {
        return ERROR_CODE_GENERAL;
    }
    return ERROR_SUCCESS;
}

JSIValue CipherModule::Rsa(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    JSIValue undefValue = JSI::CreateUndefined();
    if ((args == nullptr) || argsNum == 0 || JSI::ValueIsUndefined(args[0])) {
        return undefValue;
    }

    JSIValue success = JSI::GetNamedProperty(args[0], CB_SUCCESS);
    JSIValue fail = JSI::GetNamedProperty(args[0], CB_FAIL);
    JSIValue complete = JSI::GetNamedProperty(args[0], CB_COMPLETE);
    char *strAction = JSI::GetStringProperty(args[0], "action");
    char *strKey = JSI::GetStringProperty(args[0], "key");
    char *strText = JSI::GetStringProperty(args[0], "text");
    char *strTransformation = JSI::GetStringProperty(args[0], "transformation");
    JSIValue code = JSI::CreateNumber(ERR_CODE);
    JSIValue data = JSI::CreateString("System error");
    JSIValue argvFail[ARGC_TWO] = { data, code };
    JSIValue result = JSI::CreateObject();
    JSIValue argvSuccess[ARGC_ONE] = { result };
    int ret = ERROR_CODE_GENERAL;
    RsaData textIn = { NULL, 0 };
    RsaData textOut = { NULL, 0 };
    RsaKeyData rsaKey = { NULL, NULL, NULL, 0 };
    if ((strKey == nullptr) || (strText == nullptr)) {
        JSI::CallFunction(fail, thisVal, argvFail, ARGC_TWO);
        goto RELEASE;
    }
    rsaKey.key = strKey;
    rsaKey.keyLen = strlen(strKey);
    rsaKey.trans = strTransformation;
    rsaKey.action = strAction;
    textIn.data = strText;
    textIn.length = strlen(strText);

    ret = RsaCrypt(&rsaKey, &textIn, &textOut);
    if ((ret != ERROR_SUCCESS) || (textOut.length == 0)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "RsaCrypt failed.");
        JSI::CallFunction(fail, thisVal, argvFail, ARGC_TWO);
        goto RELEASE;
    }
    textOut.data = static_cast<char *>(ace_malloc(textOut.length));
    if (textOut.data == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ace malloc failed.");
        JSI::CallFunction(fail, thisVal, argvFail, ARGC_TWO);
        goto RELEASE;
    }
    ret = memset_s(textOut.data, textOut.length, 0, textOut.length);
    if (ret) {
        HILOG_ERROR(HILOG_MODULE_ACE, "memset failed.");
    }
    ret = RsaCrypt(&rsaKey, &textIn, &textOut);
    if (ret != ERROR_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "RsaCrypt failed.");
        JSI::CallFunction(fail, thisVal, argvFail, ARGC_TWO);
        goto RELEASE;
    }
    JSI::SetStringProperty(result, "text", (char*)textOut.data);
    JSI::CallFunction(success, thisVal, argvSuccess, ARGC_ONE);

RELEASE:
    JSI::ReleaseString(strAction);
    JSI::ReleaseString(strText);
    ResetStrBuf(strKey);
    JSI::ReleaseString(strKey);
    JSI::ReleaseString(strTransformation);
    JSI::CallFunction(complete, thisVal, nullptr, 0);
    JSI::ReleaseValueList(success, fail, complete, result, code, data, ARGS_END);
    if (textOut.data != nullptr) {
        ace_free(textOut.data);
    }

    return undefValue;
}
// OHOS BEGIN
#ifdef HUKS_LITEWEARABLE_SUPPORT_ENABLED
JSIValue CipherModule::Sha256All(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    JSIValue undefValue = JSI::CreateUndefined();
    if ((args == nullptr) || argsNum == 0 || JSI::ValueIsUndefined(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Sha256 args error.");
        return undefValue;
    }
    JSIValue context = JSI::AcquireValue(thisVal);
    JSIValue success = JSI::GetNamedProperty(args[0], CB_SUCCESS);
    JSIValue fail = JSI::GetNamedProperty(args[0], CB_FAIL);
    char *strText = JSI::GetStringProperty(args[0], "text");
    unsigned char sha256Result[SHA256_LEN];
    unsigned char *strResult = reinterpret_cast<unsigned char *>(strText);
    int32_t ret = -1;
    int size = 2;
    int sizeBuffer = 3;
    if (strlen(strText) == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "args error , text is nullptr");
        goto RELEASE;
    }
    ret = Sha256(strResult, strlen(strText), sha256Result);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Sha256 error.");
        CipherMsgToJS("Sha256All error", context, fail);
        goto RELEASE;
    }
    char result[SHA256_RESULT_LEN + 1];
    for (int i = 0; i < SHA256_LEN; i++) {
        ret = sprintf_s(&result[i * size], sizeBuffer, "%02x", sha256Result[i]);
        if (ret < 0) {
            HILOG_ERROR(HILOG_MODULE_ACE, "Sha256All sprintf_s error.");
            goto RELEASE;
        }
    }
    result[SHA256_RESULT_LEN] = '\0';
    ret = CipherMsgToJS(result, context, success);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Sha256All send message error.");
        CipherMsgToJS("Sha256All send message error error", context, fail);
        goto RELEASE;
    }

RELEASE:
    JSI::ReleaseValueList(success, fail, context, ARGS_END);
    JSI::ReleaseString(strText);
    return undefValue;
}

JSIValue CipherModule::Sha256SubStarts(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    g_sha256Context = CreateSha256Context();
    Sha256Starts(g_sha256Context);
    return JSI::CreateUndefined();
}

JSIValue CipherModule::Sha256SubUpdate(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    JSIValue undefValue = JSI::CreateUndefined();
    if ((args == nullptr) || argsNum == 0 || JSI::ValueIsUndefined(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "args error.");
        return undefValue;
    }
    char *strText = JSI::ValueToString(args[0]);
    if (strlen(strText) == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "args error , text is nullptr");
        JSI::ReleaseString(strText);
        return undefValue;
    }
    unsigned char *strTemp = reinterpret_cast<unsigned char *>(strText);
    Sha256Update(g_sha256Context, strTemp, strlen(strText));
    JSI::ReleaseString(strText);
    return undefValue;
}

JSIValue CipherModule::Sha256SubFinish(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    JSIValue undefValue = JSI::CreateUndefined();
    if ((args == nullptr) || argsNum == 0 || JSI::ValueIsUndefined(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "args error.");
        return undefValue;
    }
    JSIValue context = JSI::AcquireValue(thisVal);
    JSIValue success = JSI::GetNamedProperty(args[0], CB_SUCCESS);
    JSIValue fail = JSI::GetNamedProperty(args[0], CB_FAIL);
    unsigned char sha256Result[SHA256_LEN];
    char result[SHA256_RESULT_LEN + 1];
    Sha256Finish(g_sha256Context, sha256Result);
    int32_t ret = 0;
    int size = 2;
    int sizeBuffer = 3;
    for (int i = 0; i < SHA256_LEN; i++) {
        ret = sprintf_s(&result[i * size], sizeBuffer, "%02x", sha256Result[i]);
        if (ret < 0) {
            HILOG_ERROR(HILOG_MODULE_ACE, "Sha256SubFinish sprintf_s error.");
            return undefValue;
        }
    }
    result[SHA256_RESULT_LEN] = '\0';
    ret = CipherMsgToJS(result, context, success);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Sha256SubFinish send message error.");
        CipherMsgToJS("Sha256SubFinish error", context, fail);
    }
    JSI::ReleaseValueList(success, fail, context, ARGS_END);
    DestroySha256Context(&g_sha256Context);
    return undefValue;
}
#endif
// OHOS END
JSIValue CipherModule::Aes(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    JSIValue undefValue = JSI::CreateUndefined();
    if ((args == nullptr) || argsNum == 0 || JSI::ValueIsUndefined(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "args error.");
        return undefValue;
    }
#ifdef HUKS_LITEWEARABLE_SUPPORT_ENABLED
    JSIValue context = JSI::AcquireValue(thisVal);
#endif
    JSIValue success = JSI::GetNamedProperty(args[0], CB_SUCCESS);
    JSIValue fail = JSI::GetNamedProperty(args[0], CB_FAIL);
    char *strAction = JSI::GetStringProperty(args[0], "action");
    char *strText = JSI::GetStringProperty(args[0], "text");
    char *strKey = JSI::GetStringProperty(args[0], "key");
    char *strTransformation = JSI::GetStringProperty(args[0], "transformation");
    char *strIv = JSI::GetStringProperty(args[0], "iv");
    double ivOffset = JSI::GetNumberProperty(args[0], "ivOffset");
    JSIValue jsIvLen = JSI::GetNamedProperty(args[0], "ivLen");
    JSIValue result = JSI::CreateObject();
    JSIValue code = JSI::CreateNumber(ERR_CODE);
    JSIValue argvSuccess[ARGC_ONE] = { result };
    JSIValue data = JSI::CreateString("System error");
    JSIValue argvFail[ARGC_TWO] = { data, code };
    CipherAesMode mode = GetMode(strTransformation);
    PaddingMode paddingMode = GetPaddingMode(strTransformation);
    AesCryptContext aes = {{NULL, NULL, 0, 0, 0}, mode, {NULL, NULL, 0, 0}, paddingMode};
    AesIvMode iv = {NULL, NULL, 0, 0};
#ifdef HUKS_LITEWEARABLE_SUPPORT_ENABLED
    std::string str;
    int32_t textLen;
#endif
    int ret;
    double ivLen = -1;
    if (strlen(strText) == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "args error , text is nullptr");
        goto RELEASE;
    }
    if (strlen(strText) > MAX_LEN) {
        HILOG_ERROR(HILOG_MODULE_ACE, "args error , text size more than 16KB");
        goto RELEASE;
    }
    if (!strcmp(strAction, "encrypt") && paddingMode == PADDING_NONE && strlen(strText) % AES_BLOCK_SIZE != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "args error , text is not valid");
        goto RELEASE;
    }
    if (JSI::ValueIsNumber(jsIvLen)) {
        ivLen = JSI::ValueToNumber(jsIvLen);
        if (ivLen < 0) {
            HILOG_ERROR(HILOG_MODULE_ACE, "Ivlen:%lf is not natural number.", ivLen);
#ifdef HUKS_LITEWEARABLE_SUPPORT_ENABLED
            CipherMsgToJS("Aes jsIvLen error", context, fail);
#else
            JSI::CallFunction(fail, thisVal, argvFail, ARGC_TWO);
#endif
            goto RELEASE;
        }
    }

    iv.ivBuf = strIv;
    iv.ivLen = ivLen;
    iv.ivOffset = ivOffset;
    iv.transformation = strTransformation;
    ret = InitAesCryptData(strAction, strText, strKey, &iv, &aes);
    if (ret != ERROR_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "InitAesCryptData failed.");
#ifdef HUKS_LITEWEARABLE_SUPPORT_ENABLED
        CipherMsgToJS("InitAesCryptData failed", context, fail);
#else
        JSI::CallFunction(fail, thisVal, argvFail, ARGC_TWO);
#endif
        goto RELEASE;
    }
    ret = CipherAesCrypt(&aes);
    if (ret != ERROR_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "CipherAesCrypt failed.");
#ifdef HUKS_LITEWEARABLE_SUPPORT_ENABLED
        CipherMsgToJS("CipherAesCrypt failed", context, fail);
#else
        JSI::CallFunction(fail, thisVal, argvFail, ARGC_TWO);
#endif
        goto RELEASE;
    }
#ifdef HUKS_LITEWEARABLE_SUPPORT_ENABLED
    str = aes.data.text;
    textLen = aes.data.textLen;
    ret = CipherMsgToJS(str.substr(0, textLen).c_str(), context, success);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Aes send message error.");
        CipherMsgToJS("Aes send message error.", context, fail);
    }
#else
    JSI::SetStringProperty(result, "text", (char*)aes.data.text);
    JSI::CallFunction(success, thisVal, argvSuccess, ARGC_ONE);
#endif

RELEASE:
    JSI::ReleaseString(strAction);
    JSI::ReleaseString(strText);
    ResetStrBuf(strKey);
    ResetStrBuf(strIv);
    JSI::ReleaseString(strKey);
    JSI::ReleaseString(strIv);
    JSI::ReleaseString(strTransformation);
#ifdef HUKS_LITEWEARABLE_SUPPORT_ENABLED
    JSI::ReleaseValue(context);
#endif
    JSI::ReleaseValueList(success, fail, jsIvLen, code, data, result, ARGS_END);
    DeinitAesCryptData(&aes);
    return undefValue;
}
// OHOS BEGIN
#ifdef HUKS_LITEWEARABLE_SUPPORT_ENABLED
void CipherModule::OnDestroy()
{
    if (g_sha256Context != nullptr) {
        DestroySha256Context(&g_sha256Context);
        g_sha256Context = nullptr;
    }
}
#endif
// OHOS END

CipherAesMode CipherModule::GetMode(char *strTransformation)
{
    std::string str(strTransformation);
    int firstIndex = str.find_first_of("/");
    int lastIndex = str.find_last_of("/");
    std::string substr = str.substr(firstIndex + 1, lastIndex - (firstIndex + 1));
    CipherAesMode result = CIPHER_AES_CBC;
    if (substr == "ECB") {
        result = CIPHER_AES_ECB;
    }
    return result;
}

PaddingMode CipherModule::GetPaddingMode(char *strTransformation)
{
    PaddingMode result = PADDING_NONE;
    if (!strcmp(strTransformation, "AES/CBC/PKCS5Padding") || !strcmp(strTransformation, "AES/ECB/PKCS5Padding")) {
        result = PADDING_PKCS5;
    } else if (!strcmp(strTransformation, "AES/CBC/PKCS7Padding") ||
               !strcmp(strTransformation, "AES/ECB/PKCS7Padding")) {
        result = PADDING_PKCS7;
    }
    return result;
}

// OHOS BEGIN
#ifdef HUKS_LITEWEARABLE_SUPPORT_ENABLED
static int32_t CopyStr(char **destStr, const char *srcStr)
{
    if (destStr == nullptr || srcStr == nullptr) {
        return -1;
    }

    uint32_t length = strlen(srcStr) + 1;
    if (length > MAX_LEN) {
        HILOG_ERROR(HILOG_MODULE_ACE, "More than Max Memory");
        return -1;
    }
    *destStr = static_cast<char *>(APPV_MALLOC(length));
    if (*destStr == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to allocate memory for destination string");
        return -1;
    }

    if (memset_s(*destStr, length, 0, length) != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to set memory for destination string");
        APPV_FREE(*destStr);
        return -1;
    }
    if (memcpy_s(*destStr, length, srcStr, length) != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to copy source string to destination string");
        APPV_FREE(*destStr);
        return -1;
    }

    return 0;
}

// HMAC-SHA256实现
JSIValue CipherModule::HmacSha256(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    int32_t ret = 0;
    JSIValue undefValue = JSI::CreateUndefined();
    if ((args == nullptr) || argsNum == 0 || JSI::ValueIsUndefined(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "HmacSha256 args error.");
        return undefValue;
    }

    JSIValue context = JSI::AcquireValue(thisVal);
    JSIValue success = JSI::GetNamedProperty(args[0], "success");
    JSIValue fail = JSI::GetNamedProperty(args[0], "fail");

    char *text = JSI::GetStringProperty(args[0], "text");
    char *key = JSI::GetStringProperty(args[0], "key");
    unsigned char hmacResult[HMAC_SHA256_LEN];
    char result[HMAC_SHA256_RESULT_LEN + 1];
    if (text == nullptr || key == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "HmacSha256: text or key is null");
        CipherMsgToJS("HmacSha256: text or key is null", context, fail);
        goto RELEASE;
    }
    if (CheckTxtLen(text) != ERROR_SUCCESS || CheckKeyLen(key) != ERROR_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "HmacSha256: the length of text or key is out of range.");
        goto RELEASE;
    }

    ret = HmacSha256Compute(
        reinterpret_cast<unsigned char*>(key), strlen(key),
        reinterpret_cast<unsigned char*>(text), strlen(text),
        hmacResult
    );
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "HmacSha256 compute error: %d", ret);
        CipherMsgToJS("HmacSha256 compute error", context, fail);
        goto RELEASE;
    }
    // 转换为十六进制字符串
    for (int i = 0; i < HMAC_SHA256_LEN; i++) {
        ret = sprintf_s(&result[i * 2], 3, "%02x", hmacResult[i]);
        if (ret < 0) {
            HILOG_ERROR(HILOG_MODULE_ACE, "HmacSha256 sprintf_s error.");
            CipherMsgToJS("HmacSha256 format error", context, fail);
            goto RELEASE;
        }
    }
    result[HMAC_SHA256_RESULT_LEN] = '\0';

    ret = CipherMsgToJS(result, context, success);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "HmacSha256 send message error.");
        CipherMsgToJS("HmacSha256 send message error", context, fail);
    }

RELEASE:
    JSI::ReleaseValueList(success, fail, context, ARGS_END);
    if (text != nullptr) {
        JSI::ReleaseString(text);
    }
    if (key != nullptr) {
        JSI::ReleaseString(key);
    }
    return undefValue;
}

JSIValue CipherModule::HmacSha256Bytes(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    int32_t ret = 0;
    JSIValue result[1];
    JSIValue undefValue = JSI::CreateUndefined();
    if ((args == nullptr) || argsNum == 0 || JSI::ValueIsUndefined(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "HmacSha256Bytes args error.");
        return undefValue;
    }

    JSIValue context = JSI::AcquireValue(thisVal);
    JSIValue success = JSI::GetNamedProperty(args[0], "success");
    JSIValue fail = JSI::GetNamedProperty(args[0], "fail");
    uint8_t *bufferPtr = nullptr;
    JSIValue arrayBuffer = JSI::CreateUndefined();

    char *text = JSI::GetStringProperty(args[0], "text");
    char *key = JSI::GetStringProperty(args[0], "key");
    unsigned char hmacResult[HMAC_SHA256_LEN];

    if (text == nullptr || key == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "HmacSha256Bytes: text or key is null");
        CipherMsgToJS("HmacSha256Bytes: text or key is null", context, fail);
        goto RELEASE;
    }
    if (CheckTxtLen(text) != ERROR_SUCCESS || CheckKeyLen(key) != ERROR_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "HmacSha256Bytes: the length of text or key is out of range.");
        goto RELEASE;
    }

    ret = HmacSha256Compute(
        reinterpret_cast<unsigned char*>(key), strlen(key),
        reinterpret_cast<unsigned char*>(text), strlen(text),
        hmacResult
    );
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "HmacSha256Bytes compute error: %d", ret);
        CipherMsgToJS("HmacSha256Bytes compute error", context, fail);
        goto RELEASE;
    }

    // 创建 ArrayBuffer 并直接返回字节数组
    arrayBuffer = JSI::CreateArrayBuffer(HMAC_SHA256_LEN, bufferPtr);
    if (bufferPtr == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "HmacSha256Bytes: CreateArrayBuffer failed");
        CipherMsgToJS("HmacSha256Bytes: CreateArrayBuffer failed", context, fail);
        JSI::ReleaseValue(arrayBuffer);
        goto RELEASE;
    }

    // 拷贝数据到 ArrayBuffer
    if (memcpy_s(bufferPtr, HMAC_SHA256_LEN, hmacResult, HMAC_SHA256_LEN) != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "HmacSha256Bytes: memcpy_s failed");
        CipherMsgToJS("HmacSha256Bytes: memcpy_s failed", context, fail);
        JSI::ReleaseValue(arrayBuffer);
        goto RELEASE;
    }

    // 返回 ArrayBuffer
    result[0] = arrayBuffer;
    JSI::CallFunction(success, context, result, 1);
    JSI::ReleaseValueList(arrayBuffer, ARGS_END);

RELEASE:
    JSI::ReleaseValueList(success, fail, context, ARGS_END);
    if (text != nullptr) {
        JSI::ReleaseString(text);
    }
    if (key != nullptr) {
        JSI::ReleaseString(key);
    }
    return undefValue;
}

// Base64编码实现
JSIValue CipherModule::Base64Encode(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    int32_t ret = 0;
    JSIValue undefValue = JSI::CreateUndefined();
    if ((args == nullptr) || argsNum == 0 || JSI::ValueIsUndefined(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Base64Encode args error.");
        return undefValue;
    }

    JSIValue context = JSI::AcquireValue(thisVal);
    JSIValue success = JSI::GetNamedProperty(args[0], "success");
    JSIValue fail = JSI::GetNamedProperty(args[0], "fail");
    size_t inputLen = 0;
    size_t outputLen = 0;
    char *base64Result = nullptr;

    char *text = JSI::GetStringProperty(args[0], "text");
    if (text == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Base64Encode: text is null");
        CipherMsgToJS("Base64Encode: text is null", context, fail);
        goto RELEASE;
    }
    if (CheckTxtLen(text) != ERROR_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Base64Encode: the length of text is out of range.");
        goto RELEASE;
    }

    inputLen = strlen(text);
    mbedtls_base64_encode(nullptr, 0, &outputLen, 
                         reinterpret_cast<const unsigned char*>(text), inputLen);

    base64Result = static_cast<char*>(APPV_MALLOC(outputLen + 1));
    if (base64Result == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Base64Encode: malloc failed");
        CipherMsgToJS("Base64Encode: memory allocation failed", context, fail);
        goto RELEASE;
    }

    ret = Base64EncodeInternal(
        reinterpret_cast<const unsigned char*>(text), inputLen,
        base64Result, outputLen
    );

    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Base64Encode error: %d", ret);
        CipherMsgToJS("Base64Encode error", context, fail);
        APPV_FREE(base64Result);
        goto RELEASE;
    }

    base64Result[outputLen] = '\0';
    ret = CipherMsgToJS(base64Result, context, success);
    APPV_FREE(base64Result);

    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Base64Encode send message error.");
        CipherMsgToJS("Base64Encode send message error", context, fail);
    }

RELEASE:
    JSI::ReleaseValueList(success, fail, context, ARGS_END);
    if (text != nullptr) {
        JSI::ReleaseString(text);
    }
    return undefValue;
}

// Base64解码实现  
JSIValue CipherModule::Base64Decode(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    int32_t ret = 0;
    JSIValue undefValue = JSI::CreateUndefined();
    if ((args == nullptr) || argsNum == 0 || JSI::ValueIsUndefined(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Base64Decode args error.");
        return undefValue;
    }

    JSIValue context = JSI::AcquireValue(thisVal);
    JSIValue success = JSI::GetNamedProperty(args[0], "success");
    JSIValue fail = JSI::GetNamedProperty(args[0], "fail");
    size_t inputLen = 0;
    size_t outputLen = 0;
    unsigned char *decodedResult = nullptr;
    char *resultStr = nullptr;

    char *base64Text = JSI::GetStringProperty(args[0], "text");
    if (base64Text == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Base64Decode: text is null");
        CipherMsgToJS("Base64Decode: text is null", context, fail);
        goto RELEASE;
    }
    if (CheckTxtLen(base64Text) != ERROR_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Base64Decode: the length of text is out of range.");
        goto RELEASE;
    }

    inputLen = strlen(base64Text);
    mbedtls_base64_decode(nullptr, 0, &outputLen, 
                         reinterpret_cast<const unsigned char*>(base64Text), inputLen);

    decodedResult = static_cast<unsigned char*>(APPV_MALLOC(outputLen + 1));
    if (decodedResult == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Base64Decode: malloc failed");
        CipherMsgToJS("Base64Decode: memory allocation failed", context, fail);
        goto RELEASE;
    }

    ret = Base64DecodeInternal(base64Text, inputLen, decodedResult, outputLen);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Base64Decode error: %d", ret);
        CipherMsgToJS("Base64Decode error", context, fail);
        APPV_FREE(decodedResult);
        goto RELEASE;
    }

    decodedResult[outputLen] = '\0';
    resultStr = reinterpret_cast<char*>(decodedResult);
    ret = CipherMsgToJS(resultStr, context, success);
    APPV_FREE(decodedResult);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Base64Decode send message error.");
        CipherMsgToJS("Base64Decode send message error", context, fail);
    }

RELEASE:
    JSI::ReleaseValueList(success, fail, context, ARGS_END);
    if (base64Text != nullptr) {
        JSI::ReleaseString(base64Text);
    }
    return undefValue;
}

// MD5实现
JSIValue CipherModule::Md5FromString(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    int32_t ret = 0;
    JSIValue undefValue = JSI::CreateUndefined();
    if ((args == nullptr) || argsNum == 0 || JSI::ValueIsUndefined(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Md5 args error.");
        return undefValue;
    }

    JSIValue context = JSI::AcquireValue(thisVal);
    JSIValue success = JSI::GetNamedProperty(args[0], "success");
    JSIValue fail = JSI::GetNamedProperty(args[0], "fail");
    unsigned char md5Result[MD5_LEN];
    char result[MD5_RESULT_LEN + 1];

    char *text = JSI::GetStringProperty(args[0], "text");
    if (text == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Md5: text is null");
        CipherMsgToJS("Md5: text is null", context, fail);
        goto RELEASE;
    }
    if (CheckTxtLen(text) != ERROR_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Md5: the length of text is out of range.");
        goto RELEASE;
    }

    ret = Md5Compute(reinterpret_cast<unsigned char*>(text), strlen(text), md5Result);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Md5 compute error: %d", ret);
        CipherMsgToJS("Md5 compute error", context, fail);
        goto RELEASE;
    }

    // 转换为十六进制字符串
    for (int i = 0; i < MD5_LEN; i++) {
        ret = sprintf_s(&result[i * 2], 3, "%02x", md5Result[i]);
        if (ret < 0) {
            HILOG_ERROR(HILOG_MODULE_ACE, "Md5 sprintf_s error.");
            CipherMsgToJS("Md5 format error", context, fail);
            goto RELEASE;
        }
    }
    result[MD5_RESULT_LEN] = '\0';

    ret = CipherMsgToJS(result, context, success);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Md5 send message error.");
        CipherMsgToJS("Md5 send message error", context, fail);
    }

RELEASE:
    JSI::ReleaseValueList(success, fail, context, ARGS_END);
    if (text != nullptr) {
        JSI::ReleaseString(text);
    }
    return undefValue;
}

JSIValue CipherModule::Md5FromBytes(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    int32_t ret = 0;
    JSIValue undefValue = JSI::CreateUndefined();
    JSIValue context = JSI::CreateUndefined();
    JSIValue success = JSI::CreateUndefined();
    JSIValue fail = JSI::CreateUndefined();
    JSIValue dataBuffer = JSI::CreateUndefined();
    size_t dataLen = 0;
    unsigned char md5Result[MD5_LEN];
    uint8_t* byteData = nullptr;
    char result[MD5_LEN * 2 + 1];
    bool conversionSuccess = true;

    if ((args == nullptr) || argsNum == 0 || JSI::ValueIsUndefined(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Md5FromBytes args error.");
        goto RELEASE;
    }

    context = JSI::AcquireValue(thisVal);
    success = JSI::GetNamedProperty(args[0], "success");
    fail = JSI::GetNamedProperty(args[0], "fail");

    // 获取 ArrayBuffer 参数
    dataBuffer = JSI::GetNamedProperty(args[0], "data");
    if (JSI::ValueIsUndefined(dataBuffer)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Md5FromBytes: data is undefined");
        CipherMsgToJS("Md5FromBytes: data is undefined", context, fail);
        goto RELEASE;
    }

    // 从 ArrayBuffer 获取数据指针和长度
    byteData = JSI::GetArrayBufferInfo(dataBuffer, dataLen);
    if (byteData == nullptr || dataLen == 0 || dataLen > MAX_KEY_LEN) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Md5FromBytes: invalid array buffer");
        CipherMsgToJS("Md5FromBytes: invalid array buffer", context, fail);
        goto RELEASE;
    }

    ret = Md5Compute(byteData, dataLen, md5Result);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Md5FromBytes compute error: %d", ret);
        CipherMsgToJS("Md5FromBytes compute error", context, fail);
        goto RELEASE;
    }

    // 转换为十六进制字符串 - 添加安全检查
    for (int i = 0; i < MD5_LEN; i++) {
        // 检查缓冲区边界
        if (i * 2 + 2 > MD5_LEN * 2) {
            HILOG_ERROR(HILOG_MODULE_ACE, "Md5FromBytes: buffer overflow detected");
            conversionSuccess = false;
            break;
        }

        // 使用 sprintf_s 并检查返回值
        int written = sprintf_s(&result[i * 2], 3, "%02x", md5Result[i]);
        if (written != 2) { // 期望写入2个字符
            HILOG_ERROR(HILOG_MODULE_ACE, "Md5FromBytes: sprintf_s failed at position %d, written=%d", i, written);
            conversionSuccess = false;
            break;
        }
    }

    if (!conversionSuccess) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Md5FromBytes: hex conversion failed");
        CipherMsgToJS("Md5FromBytes: hex conversion failed", context, fail);
        goto RELEASE;
    }

    result[MD5_LEN * 2] = '\0';

    ret = CipherMsgToJS(result, context, success);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Md5FromBytes send message error.");
        CipherMsgToJS("Md5FromBytes send message error", context, fail);
    }

RELEASE:
    JSI::ReleaseValueList(success, fail, context, dataBuffer, ARGS_END);
    return undefValue;
}

// 最大密钥ID数量
#define MAX_KEY_IDS_COUNT 100

// 将密钥内容字节数组转换为十六进制字符串
static char* BytesToHexString(const uint8_t *bytes, uint32_t len)
{
    if (bytes == NULL || len == 0) {
        return NULL;
    }

    char *hexStr = (char*)APPV_MALLOC(len * 2 + 1);
    if (hexStr == NULL) {
        return NULL;
    }

    for (uint32_t i = 0; i < len; i++) {
        int written = snprintf_s(&hexStr[i * 2], 3, 2, "%02x", bytes[i]);
        if (written < 0 || written >= 3) {
            APPV_FREE(hexStr);
            return NULL;
        }
    }
    hexStr[len * 2] = '\0';

    return hexStr;
}

// 将密钥ID数组转换为十六进制字符串
static char* KeyIdsToHexString(uint16_t *keyIds, uint16_t count)
{
    if (keyIds == NULL || count == 0) {
        return NULL;
    }

    // 每个keyId占4个十六进制字符，加上分隔符和结束符
    char *hexStr = (char*)APPV_MALLOC(count * 5); // 5: 4字符 + 1分隔符
    if (hexStr == NULL) {
        return NULL;
    }

    int offset = 0;
    for (uint16_t i = 0; i < count; i++) {
        // 5: 4字符 + 1分隔符
        int written = snprintf_s(hexStr + offset, 5, 4, "%04X", keyIds[i]);
        if (written < 0 || written >= 5) {
            APPV_FREE(hexStr);
            return NULL;
        }
        offset += written;

        // 添加分隔符（最后一个不添加）
        if (i < count - 1) {
            hexStr[offset] = ',';
            offset++;
        }
    }
    hexStr[offset] = '\0';

    return hexStr;
}

// 将十六进制字符串转换为字节数组
static int HexStringToBytes(const char *hexStr, uint8_t *bytes, uint32_t maxLen)
{
    if (hexStr == NULL || bytes == NULL) {
        return -1;
    }

    size_t hexLen = strlen(hexStr);
    if (hexLen % 2 != 0 || hexLen / 2 > maxLen) {
        return -1;
    }

    for (size_t i = 0; i < hexLen; i += 2) {
        char hexByte[3] = {hexStr[i], hexStr[i + 1], '\0'};
        bytes[i / 2] = (uint8_t)strtol(hexByte, NULL, 16);
    }

    return hexLen / 2;
}

/**
 * @brief 获取已使用的密钥ID列表
 */
JSIValue CipherModule::GetUsedKeyIds(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    // 提前声明所有变量
    JSIValue undefValue = JSI::CreateUndefined();
    JSIValue context = JSI::CreateUndefined();
    JSIValue success = JSI::CreateUndefined();
    JSIValue fail = JSI::CreateUndefined();
    JSIValue countValue = JSI::CreateUndefined();
    uint16_t maxCount = MAX_KEY_IDS_COUNT;
    uint16_t *usedIds = NULL;
    uint16_t actualCount = 0;
    char *hexString = NULL;
    
    if ((args == nullptr) || argsNum == 0 || JSI::ValueIsUndefined(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetUsedKeyIdsJSI args error.");
        goto release;
    }

    context = JSI::AcquireValue(thisVal);
    success = JSI::GetNamedProperty(args[0], "success");
    fail = JSI::GetNamedProperty(args[0], "fail");

    // 获取count参数，默认为MAX_KEY_IDS_COUNT
    countValue = JSI::GetNamedProperty(args[0], "count");
    if (!JSI::ValueIsUndefined(countValue)) {
        maxCount = (uint16_t)JSI::ValueToNumber(countValue);
        if (maxCount > MAX_KEY_IDS_COUNT) {
            maxCount = MAX_KEY_IDS_COUNT;
        }
    }

    usedIds = (uint16_t*)APPV_MALLOC(maxCount * sizeof(uint16_t));
    if (usedIds == NULL) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetUsedKeyIds: memory allocation failed");
        CipherMsgToJS("Memory allocation failed", context, fail);
        goto release;
    }

    actualCount = InnerGetUsedKeyIds(usedIds, maxCount);
    
    if (actualCount == 0) {
        HILOG_INFO(HILOG_MODULE_ACE, "No used key IDs found");
        CipherMsgToJS("", context, success);
        goto release;
    }

    // 将密钥ID数组转换为十六进制字符串
    hexString = KeyIdsToHexString(usedIds, actualCount);
    if (hexString != NULL) {
        CipherMsgToJS(hexString, context, success);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetUsedKeyIds: hex string conversion failed");
        CipherMsgToJS("Data conversion failed", context, fail);
    }

release:
    // 释放所有分配的内存
    APPV_FREE(usedIds);
    APPV_FREE(hexString);
    JSI::ReleaseValueList(countValue, success, fail, context, ARGS_END);
    return undefValue;
}

/**
 * @brief 获取可用的密钥ID列表
 */
JSIValue CipherModule::GetAvailableKeyIds(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    // 提前声明所有变量
    JSIValue undefValue = JSI::CreateUndefined();
    JSIValue context = JSI::CreateUndefined();
    JSIValue success = JSI::CreateUndefined();
    JSIValue fail = JSI::CreateUndefined();
    JSIValue countValue = JSI::CreateUndefined();
    uint16_t maxCount = MAX_KEY_IDS_COUNT;
    uint16_t *availableIds = NULL;
    uint16_t actualCount = 0;
    char *hexString = NULL;
    
    if ((args == nullptr) || argsNum == 0 || JSI::ValueIsUndefined(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetAvailableKeyIdsJSI args error.");
        goto release;
    }

    context = JSI::AcquireValue(thisVal);
    success = JSI::GetNamedProperty(args[0], "success");
    fail = JSI::GetNamedProperty(args[0], "fail");

    // 获取count参数，默认为MAX_KEY_IDS_COUNT
    countValue = JSI::GetNamedProperty(args[0], "count");
    if (!JSI::ValueIsUndefined(countValue)) {
        maxCount = (uint16_t)JSI::ValueToNumber(countValue);
        if (maxCount > MAX_KEY_IDS_COUNT) {
            maxCount = MAX_KEY_IDS_COUNT;
        }
    }

    availableIds = (uint16_t*)APPV_MALLOC(maxCount * sizeof(uint16_t));
    if (availableIds == NULL) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetAvailableKeyIds: memory allocation failed");
        CipherMsgToJS("Memory allocation failed", context, fail);
        goto release;
    }

    actualCount = InnerGetAvailableKeyIds(availableIds, maxCount);
    if (actualCount == 0) {
        HILOG_INFO(HILOG_MODULE_ACE, "No available key IDs found");
        CipherMsgToJS("", context, success);
        goto release;
    }

    // 将密钥ID数组转换为十六进制字符串
    hexString = KeyIdsToHexString(availableIds, actualCount);
    if (hexString != NULL) {
        CipherMsgToJS(hexString, context, success);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetAvailableKeyIds: hex string conversion failed");
        CipherMsgToJS("Data conversion failed", context, fail);
    }

release:
    // 释放所有分配的内存
    APPV_FREE(availableIds);
    APPV_FREE(hexString);
    JSI::ReleaseValueList(countValue, success, fail, context, ARGS_END);
    return undefValue;
}

/**
 * @brief 删除密钥
 */
JSIValue CipherModule::DeleteKeyFromNv(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    // 提前声明所有变量
    JSIValue undefValue = JSI::CreateUndefined();
    JSIValue context = JSI::CreateUndefined();
    JSIValue success = JSI::CreateUndefined();
    JSIValue fail = JSI::CreateUndefined();
    JSIValue keyIdValue = JSI::CreateUndefined();
    uint16_t keyId = 0;
    int32_t ret = 0;
    
    if ((args == nullptr) || argsNum == 0 || JSI::ValueIsUndefined(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "DeleteKeyFromNvJSI args error.");
        goto release;
    }

    context = JSI::AcquireValue(thisVal);
    success = JSI::GetNamedProperty(args[0], "success");
    fail = JSI::GetNamedProperty(args[0], "fail");

    // 获取keyId参数
    keyIdValue = JSI::GetNamedProperty(args[0], "keyId");
    if (JSI::ValueIsUndefined(keyIdValue)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "DeleteKeyFromNvJSI: keyId is required");
        CipherMsgToJS("keyId is required", context, fail);
        goto release;
    }

    keyId = (uint16_t)JSI::ValueToNumber(keyIdValue);
    
    ret = InnerDeleteKeyFromNv(keyId);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "DeleteKeyFromNv failed: %d", ret);
        CipherMsgToJS("Key deleted failed", context, fail);
    } else {
        CipherMsgToJS("Key deleted successfully", context, success);
    }

release:
    JSI::ReleaseValueList(keyIdValue, success, fail, context, ARGS_END);
    return undefValue;
}

/**
 * @brief 读取密钥
 */
JSIValue CipherModule::GetKeyFromNv(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum) {
    // 提前声明所有变量
    JSIValue undefValue = JSI::CreateUndefined();
    JSIValue context = JSI::CreateUndefined();
    JSIValue success = JSI::CreateUndefined();
    JSIValue fail = JSI::CreateUndefined();
    JSIValue keyIdValue = JSI::CreateUndefined();
    uint16_t keyId = 0;
    uint8_t *keyData = NULL;
    int32_t ret = 0;
    char *hexKey = NULL;
    uint16_t actualKeyLen = 0; // 新增：实际密钥长度

    if ((args == nullptr) || argsNum == 0 || JSI::ValueIsUndefined(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetKeyFromNvJSI args error.");
        goto release;
    }

    context = JSI::AcquireValue(thisVal);
    success = JSI::GetNamedProperty(args[0], "success");
    fail = JSI::GetNamedProperty(args[0], "fail");

    // 获取keyId参数
    keyIdValue = JSI::GetNamedProperty(args[0], "keyId");
    if (JSI::ValueIsUndefined(keyIdValue)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetKeyFromNvJSI: keyId is required");
        CipherMsgToJS("keyId is required", context, fail);
        goto release;
    }

    keyId = (uint16_t)JSI::ValueToNumber(keyIdValue);

    keyData = (uint8_t*)APPV_MALLOC(UT_NV_MAX_LENGTH);
    if (keyData == NULL) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetKeyFromNv: memory allocation failed");
        CipherMsgToJS("Memory allocation failed", context, fail);
        goto release;
    }

    // 修改：调用支持实际长度的InnerGetKeyFromNv
    ret = InnerGetKeyFromNv(keyId, keyData, &actualKeyLen);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetKeyFromNv failed: %d", ret);
        CipherMsgToJS("Get key failed!", context, fail);
    } else {
        // 修改：使用实际长度转换十六进制字符串
        hexKey = BytesToHexString(keyData, actualKeyLen);
        if (hexKey != NULL) {
            CipherMsgToJS(hexKey, context, success);
        } else {
            CipherMsgToJS("Key data format conversion failed", context, fail);
        }
    }

release:
    // 释放所有分配的内存
    APPV_FREE(keyData);
    APPV_FREE(hexKey);
    JSI::ReleaseValueList(keyIdValue, success, fail, context, ARGS_END);
    return undefValue;
}

/**
 * @brief 保存密钥
 */
/**
 * @brief 保存密钥 - 优化内存使用
 */
JSIValue CipherModule::SaveKeyToNv(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum) {
    // 提前声明所有变量
    int32_t ret = 0;
    uint8_t *keyData = NULL;
    uint16_t keyId = 0;
    bool overwrite = false;
    int keyLen = 0;
    char *keyDataHex = NULL;
    size_t hexLen = 0;
    size_t actualDataLen = 0;

    JSIValue undefValue = JSI::CreateUndefined();
    JSIValue context = JSI::CreateUndefined();
    JSIValue success = JSI::CreateUndefined();
    JSIValue fail = JSI::CreateUndefined();
    JSIValue keyIdValue = JSI::CreateUndefined();
    JSIValue keyDataValue = JSI::CreateUndefined();
    JSIValue overwriteValue = JSI::CreateUndefined();
    
    if ((args == nullptr) || argsNum == 0 || JSI::ValueIsUndefined(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SaveKeyToNvJSI args error.");
        goto release;
    }

    context = JSI::AcquireValue(thisVal);
    success = JSI::GetNamedProperty(args[0], "success");
    fail = JSI::GetNamedProperty(args[0], "fail");

    // 获取必需参数
    keyIdValue = JSI::GetNamedProperty(args[0], "keyId");
    keyDataValue = JSI::GetNamedProperty(args[0], "keyData");
    overwriteValue = JSI::GetNamedProperty(args[0], "overwrite");

    if (JSI::ValueIsUndefined(keyIdValue) || JSI::ValueIsUndefined(keyDataValue)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SaveKeyToNvJSI: keyId and keyData are required");
        CipherMsgToJS("keyId and keyData are required", context, fail);
        goto release;
    }

    keyId = (uint16_t)JSI::ValueToNumber(keyIdValue);
    overwrite = JSI::ValueToBoolean(overwriteValue);
    
    // 获取密钥数据字符串
    keyDataHex = JSI::GetStringProperty(args[0], "keyData");
    if (keyDataHex == NULL) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SaveKeyToNvJSI: keyData is null");
        CipherMsgToJS("keyData is null", context, fail);
        goto release;
    }
    
    // 计算实际需要的缓冲区大小
    hexLen = strlen(keyDataHex);
    if (hexLen % 2 != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SaveKeyToNvJSI: keyData format error (odd length)");
        CipherMsgToJS("keyData format error (must be even length hex string)", context, fail);
        JSI::ReleaseString(keyDataHex);
        goto release;
    }
    
    actualDataLen = hexLen / 2;
    if (actualDataLen > UT_NV_MAX_LENGTH) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SaveKeyToNvJSI: keyData too long");
        CipherMsgToJS("keyData too long", context, fail);
        JSI::ReleaseString(keyDataHex);
        goto release;
    }
    
    // 修改：按实际长度分配内存
    keyData = (uint8_t*)APPV_MALLOC(actualDataLen);
    if (keyData == NULL) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SaveKeyToNv: memory allocation failed");
        CipherMsgToJS("Memory allocation failed", context, fail);
        JSI::ReleaseString(keyDataHex);
        goto release;
    }
    
    // 将十六进制字符串转换为字节数组
    keyLen = HexStringToBytes(keyDataHex, keyData, actualDataLen);
    if (keyLen <= 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SaveKeyToNvJSI: keyData format error");
        CipherMsgToJS("keyData format error (must be hex string)", context, fail);
        JSI::ReleaseString(keyDataHex);
        goto release;
    }
    
    ret = InnerSaveKeyToNv(keyData, keyLen, keyId, overwrite);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SaveKeyToNv failed: %d", ret);
        CipherMsgToJS("Save key failed!", context, fail);
    } else {
        CipherMsgToJS("Key saved successfully", context, success);
    }
    
    JSI::ReleaseString(keyDataHex);

release:
    // 释放所有分配的内存
    APPV_FREE(keyData);
    JSI::ReleaseValueList(keyIdValue, keyDataValue, overwriteValue, success, fail, context, ARGS_END);
    return undefValue;
}
#endif
// OHOS END

void InitCipherModule(JSIValue exports)
{
    JSI::SetModuleAPI(exports, "aes", CipherModule::Aes);
#ifdef HUKS_LITEWEARABLE_SUPPORT_ENABLED
    JSI::SetModuleAPI(exports, "sha", CipherModule::Sha256All);
    JSI::SetModuleAPI(exports, "shaInit", CipherModule::Sha256SubStarts);
    JSI::SetModuleAPI(exports, "shaUpdate", CipherModule::Sha256SubUpdate);
    JSI::SetModuleAPI(exports, "shaFinal", CipherModule::Sha256SubFinish);

    JSI::SetModuleAPI(exports, "hmacSha256", CipherModule::HmacSha256);
    JSI::SetModuleAPI(exports, "hmacSha256Bytes", CipherModule::HmacSha256Bytes);
    JSI::SetModuleAPI(exports, "md5FromBytes", CipherModule::Md5FromBytes);
    JSI::SetModuleAPI(exports, "md5FromString", CipherModule::Md5FromString);
    JSI::SetModuleAPI(exports, "base64Encode", CipherModule::Base64Encode);
    JSI::SetModuleAPI(exports, "base64Decode", CipherModule::Base64Decode);
    JSI::SetModuleAPI(exports, "getUsedKeyIds", CipherModule::GetUsedKeyIds);
    JSI::SetModuleAPI(exports, "getAvailableKeyIds", CipherModule::GetAvailableKeyIds);
    JSI::SetModuleAPI(exports, "deleteKeyFromNv", CipherModule::DeleteKeyFromNv);
    JSI::SetModuleAPI(exports, "getKeyFromNv", CipherModule::GetKeyFromNv);
    JSI::SetModuleAPI(exports, "saveKeyToNv", CipherModule::SaveKeyToNv);

    JSI::SetOnDestroy(exports, CipherModule::OnDestroy);
#endif
}

}  // namespace ACELite
}  // namespace OHOS
// OHOS START
#ifdef HUKS_LITEWEARABLE_SUPPORT_ENABLED
static void ExecuteSendMsgToJS(void *data)
{
    if (data == nullptr) {
        return;
    }
    CipherMsgParams *params = static_cast<CipherMsgParams *>(data);

    char *msgBody = params->msgBody;
    OHOS::ACELite::JSIValue callback = params->callback;
    OHOS::ACELite::JSIValue context = params->context;
    params->msgBody = nullptr;
    delete params;
    params = nullptr;

    OHOS::ACELite::JSIValue dataInfo = OHOS::ACELite::JSI::CreateString(msgBody);
    OHOS::ACELite::JSIValue callbackArgv[OHOS::ACELite::ARGC_ONE] = {dataInfo};
    OHOS::ACELite::JSI::CallFunction(callback, context, callbackArgv, OHOS::ACELite::ARGC_ONE);
    OHOS::ACELite::JSI::ReleaseValue(dataInfo);
    OHOS::ACELite::JSI::ReleaseValue(callback);
    OHOS::ACELite::JSI::ReleaseValue(context);
    OHOS::ACELite::ace_free(msgBody);
    return;
}

int32_t CipherMsgToJS(const char *msgBody, OHOS::ACELite::JSIValue context, OHOS::ACELite::JSIValue callback)
{
    CipherMsgParams *params = new CipherMsgParams();
    if (params == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to allocate memory for params");
        return -1;
    }
    params->context = OHOS::ACELite::JSI::AcquireValue(context);
    params->callback = OHOS::ACELite::JSI::AcquireValue(callback);
    int32_t copyRet = OHOS::ACELite::CopyStr(&params->msgBody, msgBody);
    if (copyRet != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to copy msgBody");
        OHOS::ACELite::JSI::ReleaseValue(params->callback);
        OHOS::ACELite::JSI::ReleaseValue(params->context);
        OHOS::ACELite::ace_free(params->msgBody);
        params->msgBody = nullptr;
        delete params;
        params = nullptr;
        return copyRet;
    }
    bool ret =
        OHOS::ACELite::JsAsyncWork::DispatchAsyncWork(ExecuteSendMsgToJS, static_cast<void *>(params));
    if (!ret) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to dispatch async work");
        OHOS::ACELite::JSI::ReleaseValue(params->callback);
        OHOS::ACELite::JSI::ReleaseValue(params->context);
        OHOS::ACELite::ace_free(params->msgBody);
        params->msgBody = nullptr;
        delete params;
        params = nullptr;
        return -1;
    }
    return 0;
};
#endif
// OHOS END
