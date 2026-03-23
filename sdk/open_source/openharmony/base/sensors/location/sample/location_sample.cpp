/*
 * Copyright (c) CompanyNameMagicTag. 2022-2022. All rights reserved.
 * Description: video play sample wrapper
 * Author: Media Software Group
 * Create: 2022-10-17
 */

#include "location_sample.h"

#include <climits>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include "securec.h"
#include "wearable_log.h"
#include "locator.h"

#ifdef __cplusplus
extern "C" {
#endif

using namespace OHOS::Location;

#define LOCATION_CMD_LEN_MAX 128
#define ARRAY_SIZE(array) (sizeof((array)) / sizeof((array)[0]))

static int32_t LocationEnableLocating(const char *param);
static int32_t LocationDisableLocating(const char *param);
static int32_t LocationStartLocating(const char *param);
static int32_t LocationStopLocating(const char *param);
static int32_t LocationQuerySupportCoordinateSystemType(const char *param);
static int32_t LocationUtTest(const char *param);

typedef struct {
    char cmd[LOCATION_CMD_LEN_MAX];
    int32_t (*func)(const char *param);
} FuncsMap;

static FuncsMap g_locatorFuncs[] = {
    {"enable", LocationEnableLocating},
    {"start", LocationStartLocating},
    {"stop", LocationStopLocating},
    {"disable", LocationDisableLocating},
    {"query", LocationQuerySupportCoordinateSystemType},
    {"ut", LocationUtTest},
};

class GnssStatusCallback : public IGnssStatusCallback {
public:
    GnssStatusCallback() {}
    ~GnssStatusCallback() {}

    void OnStatusChange(const std::shared_ptr<SatelliteStatus> &statusInfo) override
    {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "OnStatusChange, num:%d\n", statusInfo->GetSatellitesNumber());
        for (int32_t satIdx = 0; satIdx < statusInfo->GetSatellitesNumber(); satIdx++) {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "sat index:%d svid:%d ele:%.3f az:%.3f  cnr:%.3f \n", satIdx,
                          statusInfo->GetSatelliteIds()[satIdx], statusInfo->GetAltitudes()[satIdx],
                          statusInfo->GetAzimuths()[satIdx], statusInfo->GetCarrierToNoiseDensitys()[satIdx]);
        }
    }

private:
};

class NmeaMessageCallback : public INmeaMessageCallback {
public:
    NmeaMessageCallback() {}
    ~NmeaMessageCallback() {}

    void OnMessageChange(int64_t timestamp, const std::string msg) override
    {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "OnMessageChange timestamp :%lld msg:%s\n", timestamp, msg.c_str());
    }

private:
};

class LocatorCallback : public ILocatorCallback {
public:
    LocatorCallback() {}

    ~LocatorCallback() {}

    void OnLocationReport(const std::shared_ptr<Location> &location) override
    {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "OnLocationReport Lat:%.3f Long:%.3f Alt:%.3f Speed:%.3f dir:%.3f"
                      " AltitudeAccuracy:%.3f SpeedAccuracy:%.3f DirectionAccuracy:%.3f Nvelocity:%.3f Evelocity:%.3f DVelocity:%.3f statusNumber:%d\n",
                      location->GetLatitude(), location->GetLongitude(), location->GetAltitude(),
                      location->GetSpeed(), location->GetDirection(), location->GetAltitudeAccuracy(), location->GetSpeedAccuracy(), location->GetDirectionAccuracy(),
                      location->GetNorthVelocity(), location->GetEastVelocity(), location->GetDescendVelocity(), location->GetStatusNumber());
    }

    void OnErrorReport(const int errorCode) override
    {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "error code:%d\n", errorCode);
    }

    void OnLocatingStatusChange(const int status) override
    {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "OnLocatingStatusChange status:%d\n", status);
    }

private:
};

typedef struct {
    Locator *locator;
    bool isStarted;
    LocatorCallback locatorCallback;
    GnssStatusCallback gnssStatusCallback;
    NmeaMessageCallback nmeaMessageCallback;
} LocationSampleContext;

static LocationSampleContext g_locationContext = {
    .locator = nullptr,
    .isStarted = false,
};

static int32_t LocationEnableLocating(const char *param)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "LocationStartLocating\n");
    if (g_locationContext.locator == nullptr) {
        g_locationContext.locator = &Locator::GetInstance();
    }
    if (g_locationContext.locator == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "locator is nullptr\n");
        return ERRCODE_FAILURE;
    }
    bool isEnabled = false;
    LocationErrCode errCode = g_locationContext.locator->IsLocationEnabled(isEnabled);
    if (errCode != ERRCODE_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "locator IsLocationEnabled failed:%d\n", errCode);
        return errCode;
    }
    if (isEnabled) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "locator IsLocationEnabled\n");
        return ERRCODE_SUCCESS;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "EnableAbility\n");
    errCode = g_locationContext.locator->EnableAbility(true);
    if (errCode != ERRCODE_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "locator EnableAbility failed:%d\n", errCode);
        return errCode;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "Enable Locating end\n");
    return ERRCODE_SUCCESS;
}

static int32_t LocationStartLocating(const char *param)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "LocationStartLocating\n");
    if (g_locationContext.locator == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "locator is nullptr\n");
        return ERRCODE_FAILURE;
    }
    bool isEnabled = false;
    LocationErrCode errCode = g_locationContext.locator->IsLocationEnabled(isEnabled);
    if (errCode != ERRCODE_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "locator IsLocationEnabled failed:%d\n", errCode);
        return errCode;
    }
    if (!isEnabled) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "locator Is not Enabled\n");
        return ERRCODE_FAILURE;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "RegisterGnssStatusCallback\n");
    errCode = g_locationContext.locator->RegisterGnssStatusCallback(&g_locationContext.gnssStatusCallback);
    if (errCode != ERRCODE_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "locator RegisterGnssStatusCallback failed:%d\n", errCode);
        return errCode;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "RegisterNmeaMessageCallback\n");
    errCode = g_locationContext.locator->RegisterNmeaMessageCallback(&g_locationContext.nmeaMessageCallback);
    if (errCode != ERRCODE_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "locator RegisterNmeaMessageCallback failed:%d\n", errCode);
        return errCode;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "StartLocating\n");
    RequestConfig requestConfig;
    requestConfig.SetPriority(PRIORITY_ACCURACY);
    errCode = g_locationContext.locator->StartLocating(&requestConfig, &g_locationContext.locatorCallback);
    if (errCode != ERRCODE_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "locator StartLocating failed:%d\n", errCode);
        return errCode;
    }
    g_locationContext.isStarted = true;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "StartLocating end\n");
    return ERRCODE_SUCCESS;
}

static int32_t LocationStopLocating(const char *param)
{
    if (!g_locationContext.isStarted) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "locator is not started\n");
        return ERRCODE_FAILURE;
    }
    LocationErrCode errCode =
        g_locationContext.locator->UnregisterNmeaMessageCallback(&g_locationContext.nmeaMessageCallback);
    if (errCode != ERRCODE_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "locator UnregisterNmeaMessageCallback failed:%d\n", errCode);
    }
    errCode = g_locationContext.locator->UnregisterGnssStatusCallback(&g_locationContext.gnssStatusCallback);
    if (errCode != ERRCODE_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "locator RegisterGnssStatusCallback failed:%d\n", errCode);
    }
    errCode = g_locationContext.locator->StopLocating(&g_locationContext.locatorCallback);
    if (errCode != ERRCODE_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "locator StopLocating failed:%d\n", errCode);
    }
    g_locationContext.isStarted = false;
    return ERRCODE_SUCCESS;
}

static int32_t LocationDisableLocating(const char *param)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "Disable Locating\n");
    if (g_locationContext.locator == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "locator is nullptr\n");
        return ERRCODE_FAILURE;
    }
    if (g_locationContext.isStarted) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_APP, "locator is not stop\n");
        LocationStopLocating(nullptr);
    }
    bool isEnabled = false;
    LocationErrCode errCode = g_locationContext.locator->IsLocationEnabled(isEnabled);
    if (errCode != ERRCODE_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "locator IsLocationEnabled failed:%d\n", errCode);
        return errCode;
    }
    if (!isEnabled) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "locator Is not Enabled\n");
        return ERRCODE_FAILURE;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "EnableAbility false\n");
    errCode = g_locationContext.locator->EnableAbility(false);
    if (errCode != ERRCODE_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "locator EnableAbility failed:%d\n", errCode);
        return errCode;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "Disable end\n");
    return ERRCODE_SUCCESS;
}

static int32_t LocationQuerySupportCoordinateSystemType(const char *param)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "LocationUtTest enter\n");
    if (g_locationContext.locator == nullptr) {
        g_locationContext.locator = &Locator::GetInstance();
    }
    if (g_locationContext.locator == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "locator is nullptr\n");
        return ERRCODE_FAILURE;
    }
    std::vector<LocationCoordinateSystemType> coordinateSystemTypes;
    LocationErrCode errCode = g_locationContext.locator->QuerySupportCoordinateSystemType(coordinateSystemTypes);
    if (errCode != ERRCODE_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "QuerySupportCoordinateSystemType failed:%d\n", errCode);
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "query size:%lu\n", coordinateSystemTypes.size());
    for (auto type : coordinateSystemTypes) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "query type:%d\n", type);
    }
    return ERRCODE_SUCCESS;
}

static int32_t LocationUtTest(const char *param)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "LocationUtTest enter\n");
    if (g_locationContext.locator == nullptr) {
        g_locationContext.locator = &Locator::GetInstance();
    }
    if (g_locationContext.locator == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "locator is nullptr\n");
        return ERRCODE_FAILURE;
    }
    bool isConfirmed = false;
    LocationErrCode errCode = g_locationContext.locator->IsLocationPrivacyConfirmed(0, isConfirmed);
    if (errCode != ERRCODE_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "IsLocationPrivacyConfirmed failed:%d\n", errCode);
    }
    errCode = g_locationContext.locator->SetLocationPrivacyConfirmStatus(0, isConfirmed);
    if (errCode != ERRCODE_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SetLocationPrivacyConfirmStatus failed:%d\n", errCode);
    }
    LocationCommand commands;
    errCode = g_locationContext.locator->SendCommand(&commands);
    if (errCode != ERRCODE_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SendCommand failed:%d\n", errCode);
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "LocationUtTest end\n");
    return ERRCODE_SUCCESS;
}

int32_t LocationSample(const char *argv)
{
    if (argv == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "argv is invalid\n");
        return -1;
    }
    WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "enter LocationSample :argv:%s\n", argv);
    int32_t ret = -1;
    for (uint32_t i = 0; i < ARRAY_SIZE(g_locatorFuncs); i++) {
        if (strcmp(argv, g_locatorFuncs[i].cmd) == 0) {
            ret = g_locatorFuncs[i].func(nullptr);
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "%s execute %s\n", g_locatorFuncs[i].cmd,
                          (ret != 0) ? "failed" : "success");
        }
    }
    WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "LocationSample exit\n");
    return 0;
}

#ifdef __cplusplus
};
#endif
