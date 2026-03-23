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

#include <string>
#include <new>
#include "global.h"
#include "parameter.h"
// #include "sysversion.h"
#include "ace_log.h"
#include "js_app_context.h"
#include "js_async_work.h"
#include "jsi.h"
static constexpr int MAX_BUFFER = 65;
namespace OHOS {
namespace ACELite {
    
JSIValue JSGetDeviceType(void)
{
    const char *value = GetDeviceType();
    if (value == nullptr) {
        value = "";
    }

    JSIValue deviceType = JSI::CreateString(value);
    return deviceType;
}

JSIValue JSGetManufacture(void)
{
    const char *value = GetManufacture();
    if (value == nullptr) {
        value = "";
    }

    JSIValue manfactureName = JSI::CreateString(value);
    return manfactureName;
}

JSIValue JSGetBrand(void)
{
    const char *value = GetBrand();
    if (value == nullptr) {
        value = "";
    }

    JSIValue productBrand = JSI::CreateString(value);
    return productBrand;
}

JSIValue JSGetMarketName(void)
{
    const char *value = GetMarketName();
    if (value == nullptr) {
        value = "";
    }

    JSIValue marketName = JSI::CreateString(value);
    return marketName;
}

JSIValue JSGetProductSeries(void)
{
    const char *value = GetProductSeries();
    if (value == nullptr) {
        value = "";
    }

    JSIValue productSeries = JSI::CreateString(value);
    return productSeries;
}

JSIValue JSGetProductModel(void)
{
    const char *value = GetProductModel();
    if (value == nullptr) {
        value = "";
    }

    JSIValue productModel = JSI::CreateString(value);
    return productModel;
}

JSIValue JSGetSoftwareModel(void)
{
    const char *value = GetSoftwareModel();
    if (value == nullptr) {
        value = "";
    }

    JSIValue softwareModel = JSI::CreateString(value);
    return softwareModel;
}

JSIValue JSGetHardwareModel(void)
{
    const char *value = GetHardwareModel();
    if (value == nullptr) {
        value = "";
    }

    JSIValue hardwareModel = JSI::CreateString(value);
    return hardwareModel;
}

JSIValue JSGetHardwareProfile(void)
{
    const char *value = GetHardwareProfile();
    if (value == nullptr) {
        value = "";
    }

    JSIValue hardwareProfile = JSI::CreateString(value);
    return hardwareProfile;
}

JSIValue JSGetSerial(void)
{
    const char *value = GetSerial();
    if (value == nullptr) {
        value = "";
    }

    JSIValue serialNumber = JSI::CreateString(value);
    return serialNumber;
}

JSIValue JSGetBootloaderVersion(void)
{
    const char *value = GetBootloaderVersion();
    if (value == nullptr) {
        value = "";
    }

    JSIValue bootloaderVersion = JSI::CreateString(value);
    return bootloaderVersion;
}

JSIValue JSGetAbiList(void)
{
    const char *value = GetAbiList();
    if (value == nullptr) {
        value = "";
    }

    JSIValue abiList = JSI::CreateString(value);
    return abiList;
}

JSIValue JSGetSecurityPatchTag(void)
{
    const char *value = GetSecurityPatchTag();
    if (value == nullptr) {
        value = "";
    }

    JSIValue securityPatchTag = JSI::CreateString(value);
    return securityPatchTag;
}

JSIValue JSGetDisplayVersion(void)
{
    const char *value = GetDisplayVersion();
    if (value == nullptr) {
        value = "";
    }

    JSIValue productVersion = JSI::CreateString(value);
    return productVersion;
}

JSIValue JSGetIncrementalVersion(void)
{
    const char *value = GetIncrementalVersion();
    if (value == nullptr) {
        value = "";
    }

    JSIValue incrementalVersion = JSI::CreateString(value);
    return incrementalVersion;
}

JSIValue JSGetOsReleaseType(void)
{
    const char *value = GetOsReleaseType();
    if (value == nullptr) {
        value = "";
    }

    JSIValue osReleaseType = JSI::CreateString(value);
    return osReleaseType;
}

JSIValue JSGetOSFullName(void)
{
    const char *value = GetOSFullName();
    if (value == nullptr) {
        value = "";
    }
    JSIValue osVersion = JSI::CreateString(value);
    return osVersion;
}

JSIValue JSGetMajorVersion(void)
{
    int value = GetMajorVersion();
    JSIValue majorVersion = JSI::CreateUndefined();
    if (value < 0) {
        return majorVersion;
    }

    majorVersion = JSI::CreateString(std::to_string(value).c_str());
    return majorVersion;
}

JSIValue JSGetSeniorVersion(void)
{
    int value = GetSeniorVersion();
    JSIValue seniorVersion = JSI::CreateUndefined();
    if (value < 0) {
        return seniorVersion;
    }

    seniorVersion = JSI::CreateString(std::to_string(value).c_str());
    return seniorVersion;
}

JSIValue JSGetFeatureVersion(void)
{
    int value = GetFeatureVersion();
    JSIValue featureVersion = JSI::CreateUndefined();
    if (value < 0) {
        return featureVersion;
    }

    featureVersion = JSI::CreateString(std::to_string(value).c_str());
    return featureVersion;
}

JSIValue JSGetBuildVersion(void)
{
    int value = GetBuildVersion();
    JSIValue buildVersion = JSI::CreateUndefined();
    if (value < 0) {
        return buildVersion;
    }

    buildVersion = JSI::CreateString(std::to_string(value).c_str());
    return buildVersion;
}

JSIValue JSGetSdkApiVersion(void)
{
    int value = GetSdkApiVersion();
    JSIValue sdkApiVersion = JSI::CreateUndefined();
    if (value < 0) {
        return sdkApiVersion;
    }

    sdkApiVersion = JSI::CreateString(std::to_string(value).c_str());
    return sdkApiVersion;
}

JSIValue JSGetFirstApiVersion(void)
{
    int value = GetFirstApiVersion();
    JSIValue firstApiVersion = JSI::CreateUndefined();
    if (value < 0) {
        return firstApiVersion;
    }

    firstApiVersion = JSI::CreateString(std::to_string(value).c_str());
    return firstApiVersion;
}

JSIValue JSGetVersionId(void)
{
    const char *value = GetVersionId();
    if (value == nullptr) {
        value = "";
    }

    JSIValue versionId = JSI::CreateString(value);
    return versionId;
}

JSIValue JSGetBuildType(void)
{
    const char *value = GetBuildType();
    if (value == nullptr) {
        value = "";
    }

    JSIValue buildType = JSI::CreateString(value);
    return buildType;
}

JSIValue JSGetBuildUser(void)
{
    const char *value = GetBuildUser();
    if (value == nullptr) {
        value = "";
    }

    JSIValue buildUser = JSI::CreateString(value);
    return buildUser;
}

JSIValue JSGetBuildHost(void)
{
    const char *value = GetBuildHost();
    if (value == nullptr) {
        value = "";
    }

    JSIValue buildHost = JSI::CreateString(value);
    return buildHost;
}

JSIValue JSGetBuildTime(void)
{
    const char *value = GetBuildTime();
    if (value == nullptr) {
        value = "";
    }

    JSIValue buildTime = JSI::CreateString(value);
    return buildTime;
}

JSIValue JSGetBuildRootHash(void)
{
    const char *value = GetBuildRootHash();
    if (value == nullptr) {
        value = "";
    }

    JSIValue buildRootHash = JSI::CreateString(value);
    return buildRootHash;
}

JSIValue JSGetDevUdid(void)
{
    char udid[MAX_BUFFER] = { 0 };
    int ret = GetDevUdid(udid, sizeof(udid));
    if (ret < 0) {
        return JSI::CreateUndefined();
    }
    return JSI::CreateString(udid);
}

JSIValue JSGetDistributionOSName(void)
{
    const char *value = GetDistributionOSName();
    if (value == nullptr) {
        value = "";
    }
    JSIValue distributionOSName = JSI::CreateString(value);
    return distributionOSName;
}

JSIValue JSGetDistributionOSVersion(void)
{
    const char *value = GetDistributionOSVersion();
    if (value == nullptr) {
        value = "";
    }
    JSIValue distributionOSVersion = JSI::CreateString(value);
    return distributionOSVersion;
}

JSIValue JSGetDistributionOSApiVersion(void)
{
    int value = GetDistributionOSApiVersion();
    JSIValue distributionOSApiVersion = JSI::CreateUndefined();
    if (value < 0) {
        return distributionOSApiVersion;
    }

    distributionOSApiVersion = JSI::CreateString(std::to_string(value).c_str());
    return distributionOSApiVersion;
}

JSIValue JSGetDistributionOSReleaseType(void)
{
    const char *value = GetDistributionOSReleaseType();
    if (value == nullptr) {
        value = "";
    }
    JSIValue distributionOSReleaseType = JSI::CreateString(value);
    return distributionOSReleaseType;
}

void InitDeviceInfoModule(JSIValue exports)
{
    JSI::SetNamedProperty(exports, "deviceType", JSGetDeviceType());
    JSI::SetNamedProperty(exports, "manufacture", JSGetManufacture());
    JSI::SetNamedProperty(exports, "brand", JSGetBrand());
    JSI::SetNamedProperty(exports, "marketName", JSGetMarketName());
    JSI::SetNamedProperty(exports, "productSeries", JSGetProductSeries());
    JSI::SetNamedProperty(exports, "productModel", JSGetProductModel());
    JSI::SetNamedProperty(exports, "softwareModel", JSGetSoftwareModel());
    JSI::SetNamedProperty(exports, "hardwareModel", JSGetHardwareModel());
    JSI::SetNamedProperty(exports, "hardwareProfile", JSGetHardwareProfile());
    JSI::SetNamedProperty(exports, "serial", JSGetSerial());
    JSI::SetNamedProperty(exports, "bootloaderVersion", JSGetBootloaderVersion());
    JSI::SetNamedProperty(exports, "abiList", JSGetAbiList());
    JSI::SetNamedProperty(exports, "securityPatchTag", JSGetSecurityPatchTag());
    JSI::SetNamedProperty(exports, "displayVersion", JSGetDisplayVersion());
    JSI::SetNamedProperty(exports, "incrementalVersion", JSGetIncrementalVersion());
    JSI::SetNamedProperty(exports, "osReleaseType", JSGetOsReleaseType());
    JSI::SetNamedProperty(exports, "osFullName", JSGetOSFullName());
    JSI::SetNamedProperty(exports, "majorVersion", JSGetMajorVersion());
    JSI::SetNamedProperty(exports, "seniorVersion", JSGetSeniorVersion());
    JSI::SetNamedProperty(exports, "featureVersion", JSGetFeatureVersion());
    JSI::SetNamedProperty(exports, "buildVersion", JSGetBuildVersion());
    JSI::SetNamedProperty(exports, "sdkApiVersion", JSGetSdkApiVersion());
    JSI::SetNamedProperty(exports, "firstApiVersion", JSGetFirstApiVersion());
    JSI::SetNamedProperty(exports, "versionId", JSGetVersionId());
    JSI::SetNamedProperty(exports, "buildType", JSGetBuildType());
    JSI::SetNamedProperty(exports, "buildUser", JSGetBuildUser());
    JSI::SetNamedProperty(exports, "buildHost", JSGetBuildHost());
    JSI::SetNamedProperty(exports, "buildTime", JSGetBuildTime());
    JSI::SetNamedProperty(exports, "buildRootHash", JSGetBuildRootHash());
    JSI::SetNamedProperty(exports, "udid", JSGetDevUdid());
    JSI::SetNamedProperty(exports, "distributionOSName", JSGetDistributionOSName());
    JSI::SetNamedProperty(exports, "distributionOSVersion", JSGetDistributionOSVersion());
    JSI::SetNamedProperty(exports, "distributionOSApiVersion", JSGetDistributionOSApiVersion());
    JSI::SetNamedProperty(exports, "distributionOSReleaseType", JSGetDistributionOSReleaseType());
}
} // ACELite
} // OHOS
