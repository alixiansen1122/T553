/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "app_provision.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "securec.h"
#include "verify_util_config.h"

#ifdef LITE_WEARABLE
#include "cJSON.h"
#include "parameter.h"
#include "verify_util_log.h"
#else
#ifdef __Z_LITE__
#include "cJSON.h"
#else
#include "cjson/cJSON.h"
#endif
#endif
#include "app_common.h"

void ProfInit(ProfileProf *pf)
{
    int ret = memset_s(pf, sizeof(ProfileProf), 0, sizeof(ProfileProf));
    if (ret != V_OK) {
        HILOG_ERROR(LOG_MODULE_AAFWK, "memset failed");
        return;
    }
    return;
}

static char *GetStringTag(const cJSON *root, const char *tag)
{
    cJSON *jsonObj = cJSON_GetObjectItem(root, tag);
    if (jsonObj == NULL) {
        //LOG_PRINT_STR("failed to get %s", tag);
        return NULL;
    }
    int objLen = strlen(jsonObj->valuestring);
    if (objLen < 0) {
        //LOG_PRINT_STR("len error");
        return NULL;
    }
    char *value = APPV_MALLOC(objLen + 1);
    if (value == NULL) {
        HILOG_ERROR(LOG_MODULE_AAFWK, "malloc error: %d", objLen + 1);
        return NULL;
    }
    int ret = strcpy_s(value, objLen + 1, jsonObj->valuestring);
    if (ret != V_OK) {
        APPV_FREE(value);
        HILOG_ERROR(LOG_MODULE_AAFWK, "strcpy error: %d", ret);
        return NULL;
    }
    return value;
}

static void FreeStringAttay(char **array, int num)
{
    if (array == NULL) {
        return;
    }
    for (int i = 0; i < num; i++) {
        if (array[i] != NULL) {
            APPV_FREE(array[i]);
        }
    }
    APPV_FREE(array);
    return;
}

static char **GetStringArrayTag(const cJSON *root, const char *tag, int *numReturn)
{
    cJSON *jsonObj = cJSON_GetObjectItem(root, tag);
    if (jsonObj == NULL) {
        //LOG_PRINT_STR("failed to get %s", tag);
        return NULL;
    }
    int num = cJSON_GetArraySize(jsonObj);
    if (num == 0) {
        HILOG_ERROR(LOG_MODULE_AAFWK, "array num 0");
        *numReturn = 0;
        return NULL;
    }
    char **value = APPV_MALLOC(sizeof(char *) * num);
    P_NULL_RETURN_NULL_WTTH_LOG(value);
    (void)memset_s(value, sizeof(char *) * num, 0, sizeof(char *) * num);

    for (int i = 0; i < num; i++) {
        cJSON *item = cJSON_GetArrayItem(jsonObj, i);
        P_NULL_GOTO_WTTH_LOG(item);

        int len = strlen(item->valuestring);
        value[i] = APPV_MALLOC(len + 1);
        P_NULL_GOTO_WTTH_LOG(value[i]);

        int ret = strcpy_s(value[i], len + 1, item->valuestring);
        if (ret != V_OK) {
            HILOG_ERROR(LOG_MODULE_AAFWK, "str cpy error : %d", ret);
            FreeStringAttay(value, num);
            return NULL;
        }
    }
    *numReturn = num;
    return value;
EXIT:
    HILOG_ERROR(LOG_MODULE_AAFWK, "get string array error");
    FreeStringAttay(value, num);
    return NULL;
}

static int GetProfValidity(const cJSON *root, ProfValidity *profVal)
{
    cJSON *jsonObj = cJSON_GetObjectItem(root, "validity");
    if (jsonObj == NULL) {
        HILOG_ERROR(LOG_MODULE_AAFWK, "failed to get validity");
        return V_ERR;
    }

    cJSON *notBefore = cJSON_GetObjectItem(jsonObj, "not-before");
    if (notBefore == NULL) {
        HILOG_ERROR(LOG_MODULE_AAFWK, "failed to get not-before");
        return V_ERR;
    }
    profVal->notBefore = notBefore->valueint;

    cJSON *notAfter = cJSON_GetObjectItem(jsonObj, "not-after");
    if (notAfter == NULL) {
        HILOG_ERROR(LOG_MODULE_AAFWK, "failed to get not-after");
        return V_ERR;
    }
    profVal->notAfter = notAfter->valueint;
    return V_OK;
}

static int GetProfBundleInfo(const cJSON *root, ProfBundleInfo *profVal)
{
    cJSON *jsonObj = cJSON_GetObjectItem(root, "bundle-info");
    if (jsonObj == NULL) {
        HILOG_ERROR(LOG_MODULE_AAFWK, "failed to get bundle-info");
        return V_ERR;
    }
    /* if failed, free by caller */
    profVal->developerId = GetStringTag(jsonObj, "developer-id");
    P_NULL_RETURN_WTTH_LOG(profVal->developerId);

    profVal->devCert = (unsigned char *)GetStringTag(jsonObj, "development-certificate");
    if (profVal->devCert == NULL) {
        HILOG_ERROR(LOG_MODULE_AAFWK, "get development-certificat failed");
        profVal->devCert = APPV_MALLOC(sizeof(char));
        P_NULL_RETURN_WTTH_LOG(profVal->devCert);
        profVal->devCert[0] = '\0';
    }

    profVal->releaseCert = (unsigned char *)GetStringTag(jsonObj, "distribution-certificate");
    if (profVal->releaseCert == NULL) {
        HILOG_INFO(LOG_MODULE_AAFWK, "get distribution-certificat failed");
        profVal->releaseCert = APPV_MALLOC(sizeof(char));
        P_NULL_RETURN_WTTH_LOG(profVal->releaseCert);
        profVal->releaseCert[0] = '\0';
    }

    profVal->bundleName = GetStringTag(jsonObj, "bundle-name");
    P_NULL_RETURN_WTTH_LOG(profVal->bundleName);

    profVal->appFeature = GetStringTag(jsonObj, "app-feature");
    P_NULL_RETURN_WTTH_LOG(profVal->appFeature);

    return V_OK;
}

static int GetProfPermission(const cJSON *root, ProfPermission *profVal)
{
    cJSON *jsonObj = cJSON_GetObjectItem(root, "permissions");
    if (jsonObj == NULL) {
        HILOG_ERROR(LOG_MODULE_AAFWK, "failed to get permissions");
        return V_ERR;
    }
    profVal->permission = GetStringArrayTag(jsonObj, "feature-permissions", &profVal->permissionNum);
    profVal->restricPermission = GetStringArrayTag(jsonObj, "restricted-permissions", &profVal->restricNum);
    return V_OK;
}

#define MAX_UDID_NUM 10
static int GetProfDebugInfo(const cJSON *root, ProfDebugInfo *profVal)
{
    cJSON *jsonObj = cJSON_GetObjectItem(root, "debug-info");
    if (jsonObj == NULL) {
        HILOG_INFO(LOG_MODULE_AAFWK, "failed to get debug-info");
        return V_OK;
    }
    profVal->devIdType = GetStringTag(jsonObj, "device-id-type");
    if (profVal->devIdType == NULL) {
        HILOG_INFO(LOG_MODULE_AAFWK, "failed to get device-id-type");
        return V_OK;
    }
    profVal->deviceId = GetStringArrayTag(jsonObj, "device-ids", &profVal->devidNum);
    if (profVal->devidNum > MAX_UDID_NUM) {
        HILOG_ERROR(LOG_MODULE_AAFWK, "udid num exceed");
        return V_ERR_INVALID_DEVID;
    }
    return V_OK;
}

static int GetProfIssuerInfo(const cJSON *root, ProfileProf *pf)
{
    int len;
    pf->issuer = GetStringTag(root, "issuer");
    if (pf->issuer == NULL) {
        len = strlen("Huawei App Store");
        pf->issuer = APPV_MALLOC(len + 1);
        if (pf->issuer == NULL) {
            return V_ERR;
        }
        int ret = strcpy_s(pf->issuer, len + 1, "Huawei App Store");
        if (ret != V_OK) {
            HILOG_ERROR(LOG_MODULE_AAFWK, "str cpy error: %d", ret);
        }
        return ret;
    }
    return V_OK;
}

static void FreeProfBundle(ProfBundleInfo *pfval)
{
    FREE_IF_NOT_NULL(pfval->appFeature);
    FREE_IF_NOT_NULL(pfval->bundleName);
    FREE_IF_NOT_NULL(pfval->devCert);
    FREE_IF_NOT_NULL(pfval->developerId);
    FREE_IF_NOT_NULL(pfval->releaseCert);
    return;
}

static void FreeProfPerssion(ProfPermission *pfval)
{
    FreeStringAttay(pfval->permission, pfval->permissionNum);
    pfval->permissionNum = 0;
    pfval->permission = NULL;

    FreeStringAttay(pfval->restricPermission, pfval->restricNum);
    pfval->restricNum = 0;
    pfval->restricPermission = NULL;
    return;
}

static void FreeProfDebuginfo(ProfDebugInfo *pfval)
{
    FREE_IF_NOT_NULL(pfval->devIdType);

    FreeStringAttay(pfval->deviceId, pfval->devidNum);
    pfval->devidNum = 0;
    pfval->deviceId = NULL;

    return;
}

void ProfFreeData(ProfileProf *pf)
{
    if (pf == NULL) {
        return;
    }
    FREE_IF_NOT_NULL(pf->versionName);
    FREE_IF_NOT_NULL(pf->uuid);
    FREE_IF_NOT_NULL(pf->type);
    FREE_IF_NOT_NULL(pf->appDistType);
    FreeProfBundle(&pf->bundleInfo);
    FreeProfPerssion(&pf->permission);
    FreeProfDebuginfo(&pf->debugInfo);
    FREE_IF_NOT_NULL(pf->issuer);
    FREE_IF_NOT_NULL(pf->appid);
    return;
}

/* parse profile */
int ParseProfile(const char *buf, int len, ProfileProf *pf)
{
    P_NULL_RETURN_WTTH_LOG(pf);
    P_NULL_RETURN_WTTH_LOG(buf);
    ProfInit(pf);
    int ret;
    cJSON *root = cJSON_Parse(buf);
    P_NULL_RETURN_WTTH_LOG(root);

    cJSON *jsonObj = cJSON_GetObjectItem(root, "version-code");
    P_NULL_GOTO_WTTH_LOG(jsonObj);
    pf->versionCode = jsonObj->valueint;

    ret = GetProfDebugInfo(root, &pf->debugInfo);
    P_ERR_GOTO_WTTH_LOG(ret);

    pf->versionName = GetStringTag(root, "version-name");
    P_NULL_GOTO_WTTH_LOG(pf->versionName);

    pf->uuid = GetStringTag(root, "uuid");
    P_NULL_GOTO_WTTH_LOG(pf->uuid);

    pf->type = GetStringTag(root, "type");
    P_NULL_GOTO_WTTH_LOG(pf->type);

    pf->appDistType = GetStringTag(root, "app-distribution-type");
    if (pf->appDistType == NULL) {
        pf->appDistType = APPV_MALLOC(sizeof(char));
        P_NULL_GOTO_WTTH_LOG(pf->appDistType);
        pf->appDistType[0] = '\0';
    }

    ret = GetProfValidity(root, &pf->validity);
    P_ERR_GOTO_WTTH_LOG(ret);

    ret = GetProfBundleInfo(root, &pf->bundleInfo);
    P_ERR_GOTO_WTTH_LOG(ret);

    ret = GetProfPermission(root, &pf->permission);
    P_ERR_GOTO_WTTH_LOG(ret);

    ret = GetProfIssuerInfo(root, pf);
    P_ERR_GOTO_WTTH_LOG(ret);

    HILOG_INFO(LOG_MODULE_AAFWK, "parse profile json sucess");
    cJSON_Delete(root);
    return V_OK;

EXIT:
    HILOG_ERROR(LOG_MODULE_AAFWK, "parse profile error");
    cJSON_Delete(root);
    ProfFreeData(pf);
    return V_ERR;
}

static int VerifyAppTypeAndDistribution(const ProfileProf *pf)
{
    if ((strcmp(pf->type, DEBUG_TYPE) != 0) && (strcmp(pf->type, RELEASE_TYPE) != 0)) {
        //LOG_PRINT_STR("invalid app type: %s", pf->type);
        return V_ERR;
    }
    if (strcmp(pf->type, RELEASE_TYPE) == 0) {
        if ((strcmp(pf->appDistType, "app_gallery") != 0) && (strcmp(pf->appDistType, "enterprise") != 0) &&
            (strcmp(pf->appDistType, "os_integration") != 0)) {
            //LOG_PRINT_STR("invalid app dis type: %s", pf->appDistType);
            return V_ERR;
        }
    }
    return V_OK;
}

static int VerifyAppBundleInfo(const ProfileProf *pf)
{
    if (!strcmp(pf->type, "debug")) {
        if (strlen((char *)pf->bundleInfo.devCert) == 0) {
            HILOG_ERROR(LOG_MODULE_AAFWK, "debug app, dev cert null");
            return V_ERR;
        }
    } else if (!strcmp(pf->type, "release")) {
        if (strlen((char *)pf->bundleInfo.releaseCert) == 0) {
            HILOG_ERROR(LOG_MODULE_AAFWK, "debug app, dev cert null");
            return V_ERR;
        }
    } else {
        HILOG_ERROR(LOG_MODULE_AAFWK, "invalid app type");
        //LOG_PRINT_STR("invalid app type: %s", pf->type);
        return V_ERR;
    }
    return V_OK;
}

#ifdef LITE_WEARABLE
#define UDID_ORI_LEN 32
#define UDID_FINAL_LEN 64
#define HEX_LEN 2
static int VerifyUdid(const ProfileProf *pf)
{
    // OHOS_SRV_DevInfoQuery 接口不存在, 通过添加的GetDevUdid获取设备udid
    char udid[UDID_FINAL_LEN + 1] = {0};
    memset_s(udid, sizeof(udid), 0, sizeof(udid));
    int ret = GetDevUdid(udid, sizeof(udid));
    if (ret != V_OK) {
        HILOG_ERROR(LOG_MODULE_AAFWK, "get udid error");
        return V_ERR;
    }
    for (int i = 0; i < pf->debugInfo.devidNum; i++) {
        if (!strcmp(pf->debugInfo.deviceId[i], udid)) {
            return V_OK;
        }
    }
    HILOG_ERROR(LOG_MODULE_AAFWK, "udid invalid");
    return V_ERR;
}
#endif
static int VerifyDebugInfo(const ProfileProf *pf)
{
    if (strcmp(pf->type, "debug")) {
        HILOG_INFO(LOG_MODULE_AAFWK, "not debug app, return ok");
        return V_OK;
    }
    //LOG_PRINT_STR("devid type: %s", pf->debugInfo.devIdType);
#ifdef LITE_WEARABLE
    int ret;
    if (!strcmp(pf->debugInfo.devIdType, "sn")) {
        HILOG_ERROR(LOG_MODULE_AAFWK, "sn invalid");
        ret = V_ERR;
    } else if (!strcmp(pf->debugInfo.devIdType, "udid")) {
        ret = VerifyUdid(pf);
    } else {
        HILOG_ERROR(LOG_MODULE_AAFWK, "devid type invalid");
        ret = V_ERR;
    }
    return ret;
#else
    return V_OK;
#endif
}

int VerifyProfileContent(const ProfileProf *pf)
{
    P_NULL_RETURN_WTTH_LOG(pf);
    int ret = VerifyAppTypeAndDistribution(pf);
    if (ret != V_OK) {
        //LOG_PRINT_STR("invalid profile distribution type : %s", pf->appDistType);
        return V_ERR_INVALID_DISP_TYPE;
    }
    ret = VerifyAppBundleInfo(pf);
    if (ret != V_OK) {
        HILOG_ERROR(LOG_MODULE_AAFWK, "invalid profile app bundle info");
        return V_ERR_INVALID_APP_BUNDLE;
    }
    /* verify debug device id */
    ret = VerifyDebugInfo(pf);
    if (ret != V_OK) {
        HILOG_ERROR(LOG_MODULE_AAFWK, "validate date error");
        return V_ERR_INVALID_DEVID;
    }
    return V_OK;
}
