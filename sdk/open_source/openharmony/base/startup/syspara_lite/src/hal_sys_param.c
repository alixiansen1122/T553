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

#include "parameter.h"
#include "param_adaptor.h"
#include "hal_sys_param.h"
#include "sysparam_errno.h"
#include "utils_file.h"
#include "ohos_errno.h"
#include <ctype.h>
#include <securec.h>

static const char OHOS_PRODUCT_TYPE[] = {"****"};
static const char OHOS_MANUFACTURE[] = {"****"};
static const char OHOS_BRAND[] = {"****"};
static const char OHOS_MARKET_NAME[] = {"OpenHarmony 5.0"};
static const char OHOS_PRODUCT_SERIES[] = {"****"};
static const char OHOS_PRODUCT_MODEL[] = {"****"};
static const char OHOS_SOFTWARE_MODEL[] = {"****"};
static const char OHOS_HARDWARE_MODEL[] = {"****"};
static const char OHOS_HARDWARE_PROFILE[] = {"aout:true,display:true"};
static const char OHOS_BOOTLOADER_VERSION[] = {"bootloader"};
static const char OHOS_SECURITY_PATCH_TAG[] = {"2024/12/01"};
static const char OHOS_ABI_LIST[] = {"****"};
static const char OHOS_SERIAL[] = {"****"};  // provided by OEM.
const char g_roBuildOs[] = {"OpenHarmony"};
const char g_roBuildVerShow[] = {"OpenHarmony 5.0.1.115"};
const int g_roFirstApiLevel = 1;
const char INCREMENTAL_VERSION[] = {"version"};
const char BUILD_TYPE[] = {"BUILD_TYPE"};
const char BUILD_USER[] = {"BUILD_USER"};
const char BUILD_HOST[] = {"BUILD_HOST"};
const char BUILD_TIME[] = {"BUILD_TIME"};
const char BUILD_ROOTHASH[] = {"BUILD_ROOTHASH"};

#define DATA_PATH          "/user/res/"
#define MAX_KEY_PATH       128

static boolean IsValidChar(const char ch)
{
    if (islower(ch) || isdigit(ch) || (ch == '_') || (ch == '.')) {
        return TRUE;
    }
    return FALSE;
}

static boolean IsValidValue(const char* value, unsigned int len)
{
    if ((value == NULL) || (*value == '\0') || (strlen(value) >= len)) {
        return FALSE;
    }
    return TRUE;
}

static boolean IsValidKey(const char* key)
{
    size_t keyLen = strlen(key);
    if (!IsValidValue(key, MAX_KEY_LEN)) {
        return FALSE;
    }
    if (key[0] == '.' || key[keyLen - 1] == '.') {
        return FALSE;
    }

    for (size_t i = 0; i < keyLen; i++) {
        if (key[i] == '.') {
            if (key[i - 1] == '.') {
                return FALSE;
            }
            continue;
        }
        if (!IsValidChar(key[i])) {
            return FALSE;
        }
    }

    return TRUE;
}

int GetSysParam(const char* key, char* value, unsigned int len)
{
    if (key == NULL || (value == NULL) || (len > MAX_GET_VALUE_LEN)) {
        return EC_INVALID;
    }
    if (!IsValidKey(key)) {
        return EC_FAILURE;
    }

    unsigned int valueLen = 0;
    char* keyPath = (char *)malloc(MAX_KEY_PATH + 1);
    if (keyPath == NULL) {
        return EC_FAILURE;
    }

    if (sprintf_s(keyPath, MAX_KEY_PATH + 1, "%s%s", DATA_PATH, key) < 0) {
        free(keyPath);
        return EC_FAILURE;
    }

    if (UtilsFileStat(keyPath, &valueLen) != EC_SUCCESS) {
        return SYSPARAM_NOT_FOUND;
    }

    if (valueLen >= len) {
        return EC_INVALID;
    }

    int fd = UtilsFileOpen(keyPath, O_RDONLY_FS, 0);
    free(keyPath);
    keyPath = NULL;
    if (fd < 0) {
        return EC_FAILURE;
    }

    int ret = UtilsFileRead(fd, value, valueLen);
    UtilsFileClose(fd);
    fd = -1;
    if (ret < 0) {
        return EC_FAILURE;
    }
    value[valueLen] = '\0';

    return valueLen;
}

int SetSysParam(const char* key, const char* value)
{
    if (!IsValidKey(key) || !IsValidValue(value, MAX_VALUE_LEN)) {
        return EC_INVALID;
    }

    char* keyPath = (char *)malloc(MAX_KEY_PATH + 1);
    if (keyPath == NULL) {
        return EC_FAILURE;
    }
    if (sprintf_s(keyPath, MAX_KEY_PATH + 1, "%s%s", DATA_PATH, key) < 0) {
        free(keyPath);
        return EC_FAILURE;
    }

    int fd = UtilsFileOpen(keyPath, O_RDWR_FS | O_CREAT_FS | O_TRUNC_FS, 0);
    free(keyPath);
    keyPath = NULL;
    if (fd < 0) {
        return EC_FAILURE;
    }

    int ret = UtilsFileWrite(fd, value, strlen(value));
    UtilsFileClose(fd);
    fd = -1;
    return (ret < 0) ? EC_FAILURE : EC_SUCCESS;
}

char* HalGetProductType(void)
{
    char* value = (char*)malloc(strlen(OHOS_PRODUCT_TYPE) + 1);
    if (value == NULL) {
        return NULL;
    }
    if (strcpy_s(value, strlen(OHOS_PRODUCT_TYPE) + 1, OHOS_PRODUCT_TYPE) != 0) {
        free(value);
        return NULL;
    }
    return value;
}

char* HalGetManufacture(void)
{
    char* value = (char*)malloc(strlen(OHOS_MANUFACTURE) + 1);
    if (value == NULL) {
        return NULL;
    }
    if (strcpy_s(value, strlen(OHOS_MANUFACTURE) + 1, OHOS_MANUFACTURE) != 0) {
        free(value);
        return NULL;
    }
    return value;
}

char* HalGetBrand(void)
{
    char* value = (char*)malloc(strlen(OHOS_BRAND) + 1);
    if (value == NULL) {
        return NULL;
    }
    if (strcpy_s(value, strlen(OHOS_BRAND) + 1, OHOS_BRAND) != 0) {
        free(value);
        return NULL;
    }
    return value;
}

char* HalGetMarketName(void)
{
    char* value = (char*)malloc(strlen(OHOS_MARKET_NAME) + 1);
    if (value == NULL) {
        return NULL;
    }
    if (strcpy_s(value, strlen(OHOS_MARKET_NAME) + 1, OHOS_MARKET_NAME) != 0) {
        free(value);
        return NULL;
    }
    return value;
}

char* HalGetProductSeries(void)
{
    char* value = (char*)malloc(strlen(OHOS_PRODUCT_SERIES) + 1);
    if (value == NULL) {
        return NULL;
    }
    if (strcpy_s(value, strlen(OHOS_PRODUCT_SERIES) + 1, OHOS_PRODUCT_SERIES) != 0) {
        free(value);
        return NULL;
    }
    return value;
}

char* HalGetProductModel(void)
{
    char* value = (char*)malloc(strlen(OHOS_PRODUCT_MODEL) + 1);
    if (value == NULL) {
        return NULL;
    }
    if (strcpy_s(value, strlen(OHOS_PRODUCT_MODEL) + 1, OHOS_PRODUCT_MODEL) != 0) {
        free(value);
        return NULL;
    }
    return value;
}

char* HalGetSoftwareModel(void)
{
    char* value = (char*)malloc(strlen(OHOS_SOFTWARE_MODEL) + 1);
    if (value == NULL) {
        return NULL;
    }
    if (strcpy_s(value, strlen(OHOS_SOFTWARE_MODEL) + 1, OHOS_SOFTWARE_MODEL) != 0) {
        free(value);
        return NULL;
    }
    return value;
}

char* HalGetHardwareModel(void)
{
    char* value = (char*)malloc(strlen(OHOS_HARDWARE_MODEL) + 1);
    if (value == NULL) {
        return NULL;
    }
    if (strcpy_s(value, strlen(OHOS_HARDWARE_MODEL) + 1, OHOS_HARDWARE_MODEL) != 0) {
        free(value);
        return NULL;
    }
    return value;
}

char* HalGetHardwareProfile(void)
{
    char* value = (char*)malloc(strlen(OHOS_HARDWARE_PROFILE) + 1);
    if (value == NULL) {
        return NULL;
    }
    if (strcpy_s(value, strlen(OHOS_HARDWARE_PROFILE) + 1, OHOS_HARDWARE_PROFILE) != 0) {
        free(value);
        return NULL;
    }
    return value;
}

char* HalGetSerial(void)
{
    char* value = (char*)malloc(strlen(OHOS_SERIAL) + 1);
    if (value == NULL) {
        return NULL;
    }
    if (strcpy_s(value, strlen(OHOS_SERIAL) + 1, OHOS_SERIAL) != 0) {
        free(value);
        return NULL;
    }
    return value;
}

char* HalGetBootloaderVersion(void)
{
    char* value = (char*)malloc(strlen(OHOS_BOOTLOADER_VERSION) + 1);
    if (value == NULL) {
        return NULL;
    }
    if (strcpy_s(value, strlen(OHOS_BOOTLOADER_VERSION) + 1, OHOS_BOOTLOADER_VERSION) != 0) {
        free(value);
        return NULL;
    }
    return value;
}

char* HalGetSecurityPatchTag(void)
{
    char* value = (char*)malloc(strlen(OHOS_SECURITY_PATCH_TAG) + 1);
    if (value == NULL) {
        return NULL;
    }
    if (strcpy_s(value, strlen(OHOS_SECURITY_PATCH_TAG) + 1, OHOS_SECURITY_PATCH_TAG) != 0) {
        free(value);
        return NULL;
    }
    return value;
}

char* HalGetAbiList(void)
{
    char* value = (char*)malloc(strlen(OHOS_ABI_LIST) + 1);
    if (value == NULL) {
        return NULL;
    }
    if (strcpy_s(value, strlen(OHOS_ABI_LIST) + 1, OHOS_ABI_LIST) != 0) {
        free(value);
        return NULL;
    }
    return value;
}

int HalGetFirstApiVersion(void)
{
    return g_roFirstApiLevel;
}
