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
#include <securec.h>
#include "hal_sys_param.h"
#include "ohos_errno.h"
#include "param_adaptor.h"
#include "mbedtls/sha256.h"
#include "mbedtls/version.h"
#if (MBEDTLS_VERSION_NUMBER >= 0x03000000)
#include "mbedtls/compat-2.x.h"
#endif

#define FILE_RO "ro."
#define VERSION_ID_LEN 256
#define PROPERTY_MAX_LENGTH 2048
#define INCREMENTAL_VERSION "version"
#define BUILD_TYPE "BUILD_TYPE"
#define BUILD_USER "BUILD_USER"
#define BUILD_HOST "BUILD_HOST"
#define BUILD_TIME "BUILD_TIME"
#define BUILD_ROOTHASH "BUILD_ROOTHASH"

#define HASH_LENGTH 32
#define DEV_BUF_LENGTH 3
#define DEV_BUF_MAX_LENGTH 1024
#define DEV_UUID_LENGTH 65
#define OS_FULL_NAME_LEN 128
#define BT_MAC_BUFF_SIZE 6
#define BT_MAC_BUFF_LEN 17

static char g_roBuildOs[] = {"OpenHarmony-5.0.1.115"};
static char g_roBuildVerShow[] = {"1.0.0"};
static char g_roSdkApiLevel[] = {"13"};
static char g_roFirstApiLevel[] = {"1"};
static const char OHOS_OS_NAME[] = { "OpenHarmony" };
static const int OHOS_SDK_API_VERSION = 13;
static const char OHOS_SECURITY_PATCH_TAG[] = {"2024/12/01"};
static const char OHOS_RELEASE_TYPE[] = { "Release" };
static const char OHOS_DEFAULT_VALUE[] = { "Default" };

static const int MAJOR_VERSION = 5;
static const int SENIOR_VERSION = 0;
static const int FEATURE_VERSION = 0;
static const int BUILD_VERSION = 0;

static const char EMPTY_STR[] = { "" };

static boolean IsValidValue(const char* value, unsigned int len)
{
    if ((value == NULL) || !strlen(value) || (strlen(value) + 1 > len)) {
        return FALSE;
    }
    return TRUE;
}

int GetParameter(const char* key, const char* def, char* value, unsigned int len)
{
    if ((key == NULL) || (value == NULL)) {
        return EC_INVALID;
    }
    if (!CheckPermission()) {
        return EC_FAILURE;
    }
    int ret = GetSysParam(key, value, len);
    if (ret == EC_INVALID) {
        return EC_INVALID;
    }
    if ((ret < 0) && (def != NULL)) {
        if (!IsValidValue(def, len)) {
            return EC_INVALID;
        }
        if (strcpy_s(value, len, def) != 0) {
            return EC_FAILURE;
        }
        ret = strlen(def);
    }
    return ret;
}

int SetParameter(const char* key, const char* value)
{
    if ((key == NULL) || (value == NULL)) {
        return EC_INVALID;
    }
    if (!CheckPermission()) {
        return EC_FAILURE;
    }
    if (strncmp(key, FILE_RO, strlen(FILE_RO)) == 0) {
        return EC_INVALID;
    }

    return SetSysParam(key, value);
}

char* GetDeviceType(void)
{
    return HalGetProductType();
}

char* GetProductType(void)
{
    return HalGetProductType();
}

char* GetManufacture(void)
{
    return HalGetManufacture();
}

char* GetBrand(void)
{
    return HalGetBrand();
}

char* GetMarketName(void)
{
    return HalGetMarketName();
}

char* GetProductSeries(void)
{
    return HalGetProductSeries();
}

char* GetProductModel(void)
{
    return HalGetProductModel();
}

char* GetSoftwareModel(void)
{
    return HalGetSoftwareModel();
}

char* GetHardwareModel(void)
{
    return HalGetHardwareModel();
}

char* GetHardwareProfile(void)
{
    return HalGetHardwareProfile();
}

char* GetSerial(void)
{
    return HalGetSerial();
}

char* GetBootloaderVersion(void)
{
    return HalGetBootloaderVersion();
}

char* GetSecurityPatchTag(void)
{
    return HalGetSecurityPatchTag();
}

char* GetAbiList(void)
{
    return HalGetAbiList();
}

static char* GetSysProperty(const char* propertyInfo, const size_t propertySize)
{
    if ((propertySize == 0) || (propertySize > PROPERTY_MAX_LENGTH)) {
        return NULL;
    }
    char* prop = (char*)malloc(propertySize);
    if (prop == NULL) {
        return NULL;
    }
    if (strcpy_s(prop, propertySize, propertyInfo) != 0) {
        free(prop);
        prop = NULL;
        return NULL;
    }
    return prop;
}

char* GetOsName(void)
{
    return GetSysProperty(g_roBuildOs, strlen(g_roBuildOs) + 1);
}

char* GetDisplayVersion(void)
{
    return GetSysProperty(g_roBuildVerShow, strlen(g_roBuildVerShow) + 1);
}

char* GetSdkApiLevel(void)
{
    return GetSysProperty(g_roSdkApiLevel, strlen(g_roSdkApiLevel) + 1);
}

char* GetFirstApiLevel(void)
{
    return GetSysProperty(g_roFirstApiLevel, strlen(g_roFirstApiLevel) + 1);
}

char* GetIncrementalVersion(void)
{
    return GetSysProperty(INCREMENTAL_VERSION, strlen(INCREMENTAL_VERSION) + 1);
}

char* GetVersionId(void)
{
    char* value = (char*)malloc(VERSION_ID_LEN);
    if (value == NULL) {
        return NULL;
    }
    if (memset_s(value, VERSION_ID_LEN, 0, VERSION_ID_LEN) != 0) {
        free(value);
        value = NULL;
        return NULL;
    }
    char* productType = GetProductType();
    char* manufacture = GetManufacture();
    char* brand = GetBrand();
    char* productSerial = GetProductSeries();
    char* productModel = GetProductModel();
    char* softwareModel = GetSoftwareModel();
    if (productType == NULL || manufacture == NULL || brand == NULL ||
        productSerial == NULL || productModel == NULL || softwareModel == NULL) {
        free(productType);
        free(manufacture);
        free(brand);
        free(productSerial);
        free(productModel);
        free(softwareModel);
        free(value);
        value = NULL;
        return NULL;
    }
    int len = sprintf_s(value, VERSION_ID_LEN, "%s/%s/%s/%s/%s/%s/%s/%s/%s/%s",
        productType, manufacture, brand, productSerial, g_roBuildOs, productModel,
        softwareModel, g_roSdkApiLevel, INCREMENTAL_VERSION, BUILD_TYPE);
    free(productType);
    free(manufacture);
    free(brand);
    free(productSerial);
    free(productModel);
    free(softwareModel);
    if (len < 0) {
        free(value);
        value = NULL;
        return NULL;
    }
    return value;
}

char* GetBuildType(void)
{
    return GetSysProperty(BUILD_TYPE, strlen(BUILD_TYPE) + 1);
}

char* GetBuildUser(void)
{
    return GetSysProperty(BUILD_USER, strlen(BUILD_USER) + 1);
}

char* GetBuildHost(void)
{
    return GetSysProperty(BUILD_HOST, strlen(BUILD_HOST) + 1);
}

char* GetBuildTime(void)
{
    return GetSysProperty(BUILD_TIME, strlen(BUILD_TIME) + 1);
}

char* GetBuildRootHash(void)
{
    return GetSysProperty(BUILD_ROOTHASH, strlen(BUILD_ROOTHASH) + 1);
}

static int GetSha256Value(const unsigned char *input, char *udid, int udidSize)
{
    if (input == NULL) {
        return EC_FAILURE;
    }
    char buf[DEV_BUF_LENGTH] = {0};
    unsigned char hash[HASH_LENGTH] = {0};

    mbedtls_sha256_context context;
    mbedtls_sha256_init(&context);
    mbedtls_sha256_starts_ret(&context, 0);
    mbedtls_sha256_update_ret(&context, input, strlen((const char *)input));
    mbedtls_sha256_finish_ret(&context, hash);

    for (size_t i = 0; i < HASH_LENGTH; i++) {
        char value = hash[i];
        memset_s(buf, DEV_BUF_LENGTH, 0, DEV_BUF_LENGTH);
        sprintf_s(buf, sizeof(buf), "%02X", value);
        if (strcat_s(udid, udidSize, buf) != 0) {
            return EC_FAILURE;
        }
    }
    return EC_SUCCESS;
}

int GetMacAddress(char *macAddress, int size)
{
    if (size < BT_MAC_BUFF_LEN + 1) {
        return EC_FAILURE;
    }
    memset(macAddress, 0, size);
    uint8_t mac[BT_MAC_BUFF_SIZE] = {0};
    uint32_t ret = bluetooth_get_local_addr(mac, BT_MAC_BUFF_SIZE);
    if(ret != 0) {
        return EC_FAILURE;
    }
    snprintf(macAddress, size + 1, "%02x:%02x:%02x:%02x:%02x:%02x",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]); // ¹Ì¶¨6¸ö×Ö½ÚÆ´½Ó
    return ret;
}

int GetDevUdid(char *udid, int size)
{
    if (size < DEV_UUID_LENGTH) {
        return EC_FAILURE;
    }

    const char *manufacture = GetManufacture();
    const char *model = GetHardwareModel();
    const char *sn = GetSerial();
    if (manufacture == NULL || model == NULL || sn == NULL) {
        return EC_FAILURE;
    }
    int tmpSize = 0;
    if (strcmp(sn, "****") == 0) {
        tmpSize = strlen(manufacture) + strlen(model) + BT_MAC_BUFF_LEN + 1;
    } else {
        tmpSize = strlen(manufacture) + strlen(model) + strlen(sn) + 1; 
    }

    if (tmpSize <= 0 || tmpSize > DEV_BUF_MAX_LENGTH) {
        return EC_FAILURE;
    }
    char *tmp = malloc(tmpSize);
    if (tmp == NULL) {
        return EC_FAILURE;
    }

    memset_s(tmp, tmpSize, 0, tmpSize);
    if ((strcat_s(tmp, tmpSize, manufacture) != 0) ||
        (strcat_s(tmp, tmpSize, model) != 0)) {
        free(tmp);
        return EC_FAILURE;
    }
    if (strcmp(sn, "****") == 0) {
        uint8_t mac[BT_MAC_BUFF_SIZE] = {0};
        uint32_t ret = bluetooth_get_local_addr(mac, BT_MAC_BUFF_SIZE);
        char mac_str[BT_MAC_BUFF_LEN + 1] = {0}; // 17字节 + 终止符
        snprintf(mac_str, sizeof(mac_str), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]); // 固定6个字节拼接
        if ((strcat_s(tmp, tmpSize, mac_str) != 0)) {
            free(tmp);
            return EC_FAILURE;
        }
    } else {
        if ((strcat_s(tmp, tmpSize, sn) != 0)) {
            free(tmp);
            return EC_FAILURE;
        }
    }
    int ret = GetSha256Value(tmp, udid, size);
    free(tmp);
    return ret;
}

int GetSdkApiVersion(void)
{
    return OHOS_SDK_API_VERSION;
}

int GetFirstApiVersion(void)
{
    return HalGetFirstApiVersion();
}

const char *GetOsReleaseType(void)
{
    return OHOS_RELEASE_TYPE;
}
/* *
 * Major(M) version number.
 */
static int g_majorVersion = 5;

/* *
 * Senior(S) version number.
 */
static int g_seniorVersion = 0;

/* *
 * Feature(F) version number.
 */
static int g_featureVersion = 1;

/* *
 * Build(B) version number.
 */
static int g_buildVersion = 115;
static const char *BuildOSFullName(void)
{
    const char release[] = "Release";
    char value[OS_FULL_NAME_LEN] = {0};
    const char *releaseType = GetOsReleaseType();
    int length;
    if (strncmp(releaseType, release, sizeof(release) - 1) == 0) {
        length = sprintf_s(value, OS_FULL_NAME_LEN, "%s-%d.%d.%d.%d",
            OHOS_OS_NAME, g_majorVersion, g_seniorVersion, g_featureVersion, g_buildVersion);
    } else {
        length = sprintf_s(value, OS_FULL_NAME_LEN, "%s-%d.%d.%d.%d(%s)",
            OHOS_OS_NAME, g_majorVersion, g_seniorVersion, g_featureVersion, g_buildVersion, releaseType);
    }
    if (length < 0) {
        return EMPTY_STR;
    }
    const char *osFullName = strdup(value);
    return osFullName;
}

const char *GetOSFullName(void)
{
    static const char *osFullName = NULL;
    if (osFullName != NULL) {
        return osFullName;
    }
    osFullName = BuildOSFullName();
    if (osFullName == NULL) {
        return EMPTY_STR;
    }
    return osFullName;
}

int GetMajorVersion(void)
{
    return MAJOR_VERSION;
}

int GetSeniorVersion(void)
{
    return SENIOR_VERSION;
}

int GetFeatureVersion(void)
{
    return FEATURE_VERSION;
}

int GetBuildVersion(void)
{
    return BUILD_VERSION;
}

const char *GetDistributionOSName(void)
{
    return OHOS_DEFAULT_VALUE;
}

const char *GetDistributionOSVersion(void)
{
    return OHOS_DEFAULT_VALUE;
}

int GetDistributionOSApiVersion(void)
{
    return 0;
}

const char *GetDistributionOSReleaseType(void)
{
    return OHOS_DEFAULT_VALUE;
}