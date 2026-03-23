/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "gnss_interface_impl.h"

#include "location_thread.h"
#include <unordered_map>

#include "location_vendor_interface.h"
#include "location_vendor_lib.h"
#include "string_utils.h"
#include "location_utils.h"
#include "location_log.h"

namespace OHOS {
namespace HDI {
namespace Location {
namespace Gnss {
namespace V2_0 {
namespace {
MutexId g_mutex;
IGnssCallback *g_gnssCallback = nullptr;
#ifndef EMULATOR_ENABLED
IGnssMeasurementCallback *g_gnssMeasurementCallback = nullptr;
#endif
GnssConfigParameter g_configPara;

}  // namespace

static void NiNotifyCallback(OHOS::HDI::Location::GnssNiNotificationRequest *notification)
{
    if (notification == nullptr) {
        LBSLOGE(HDI_GNSS, "%s:niNotificationRequest is nullptr.", __func__);
        return;
    }
    LBSLOGI(HDI_GNSS, "%s:NiNotifyCb.", __func__);
    AutoLock lock(g_mutex);
    GnssNiNotificationRequest niNotification;
    niNotification.gnssNiNotificationId = notification->gnssNiNotificationId;
    niNotification.gnssNiRequestCategory = GnssNiRequestCategory(notification->gnssNiRequestCategory);
    niNotification.notificationCategory = notification->notificationCategory;
    niNotification.requestTimeout = notification->requestTimeout;
    niNotification.defaultResponseCmd = notification->defaultResponseCmd;
    niNotification.supplicantInfo = notification->supplicantInfo;
    niNotification.notificationText = notification->notificationText;
    niNotification.supplicantInfoEncoding = GnssNiRequestEncodingFormat(notification->supplicantInfoEncoding);
    niNotification.notificationTextEncoding = GnssNiRequestEncodingFormat(notification->notificationTextEncoding);

    LBSLOGI(HDI_GNSS, "reportNiNotification: notificationId %u, niType %d, notifyType %d, timeout "
             "%d, defaultRespone %{private}d",
        niNotification.gnssNiNotificationId,
        niNotification.gnssNiRequestCategory,
        niNotification.notificationCategory,
        niNotification.requestTimeout,
        niNotification.defaultResponseCmd);

    if (g_gnssCallback != nullptr) {
        g_gnssCallback->ReportGnssNiNotification(niNotification);
    }
    LBSLOGI(HDI_GNSS, "%s:NiNotifyCallback.", __func__);
}

static void LocationUpdate(GnssLocation *location)
{
    if (location == nullptr) {
        LBSLOGE(HDI_GNSS, "%s:location is nullptr.", __func__);
        return;
    }
    LBSLOGD(HDI_GNSS, "LocationUpdate.latitude:%f", location->latitude);
    AutoLock lock(g_mutex);
    LocationInfo locationNew;
    locationNew.fieldValidity = location->fieldValidity;
    locationNew.latitude = location->latitude;
    locationNew.longitude = location->longitude;
    locationNew.altitude = location->altitude;
    locationNew.horizontalAccuracy = location->horizontalAccuracy;
    locationNew.speed = location->speed;
    locationNew.velocityNorth = location->velocityNorth;
    locationNew.velocityEast = location->velocityEast;
    locationNew.velocityDescend = location->velocityDescend;
    locationNew.bearing = location->bearing;
    locationNew.verticalAccuracy = location->verticalAccuracy;
    locationNew.speedAccuracy = location->speedAccuracy;
    locationNew.bearingAccuracy = location->bearingAccuracy;
    locationNew.timeForFix = location->timeForFix;
    locationNew.timeSinceBoot = location->timeSinceBoot;
    locationNew.timeUncertainty = location->timeUncertainty;
    locationNew.signal = location->signal;
    if (g_gnssCallback != nullptr) {
        g_gnssCallback->ReportLocation(locationNew);
    }
}

#ifndef EMULATOR_ENABLED
static void SetGnssClock(OHOS::HDI::Location::Gnss::V2_0::GnssMeasurementInfo *gnssMeasurementInfoNew,
    OHOS::HDI::Location::GnssMeasurementInfo *gnssMeasurementInfo)
{
    gnssMeasurementInfoNew->gnssClock.fieldValidFlags = gnssMeasurementInfo->gnssClock.fieldValidFlags;
    gnssMeasurementInfoNew->gnssClock.leapSecond = gnssMeasurementInfo->gnssClock.leapSecond;
    gnssMeasurementInfoNew->gnssClock.receiverClockTime = gnssMeasurementInfo->gnssClock.receiverClockTime;
    gnssMeasurementInfoNew->gnssClock.timeUncertainty = gnssMeasurementInfo->gnssClock.timeUncertainty;
    gnssMeasurementInfoNew->gnssClock.rcvClockFullBias = gnssMeasurementInfo->gnssClock.rcvClockFullBias;
    gnssMeasurementInfoNew->gnssClock.rcvClockSubBias = gnssMeasurementInfo->gnssClock.rcvClockSubBias;
    gnssMeasurementInfoNew->gnssClock.biasUncertainty = gnssMeasurementInfo->gnssClock.biasUncertainty;
    gnssMeasurementInfoNew->gnssClock.clockDrift = gnssMeasurementInfo->gnssClock.clockDrift;
    gnssMeasurementInfoNew->gnssClock.clockDriftUncertainty = gnssMeasurementInfo->gnssClock.clockDriftUncertainty;
    gnssMeasurementInfoNew->gnssClock.clockInterruptCnt = gnssMeasurementInfo->gnssClock.clockInterruptCnt;
    gnssMeasurementInfoNew->gnssClock.clockJumpThreshold = gnssMeasurementInfo->gnssClock.clockJumpThreshold;
    gnssMeasurementInfoNew->gnssClock.clockHWFreBiasIndicator = gnssMeasurementInfo->gnssClock.clockHWFreBiasIndicator;
    gnssMeasurementInfoNew->gnssClock.clockHWFreDriftIndicator =
        gnssMeasurementInfo->gnssClock.clockHWFreDriftIndicator;
}

static void GnssMeasurementUpdate(OHOS::HDI::Location::GnssMeasurementInfo *gnssMeasurementInfo)
{
    if (gnssMeasurementInfo == nullptr) {
        LBSLOGE(HDI_GNSS, "%s:gnssMeasurementInfo is nullptr.", __func__);
        return;
    }
    AutoLock lock(g_mutex);
    OHOS::HDI::Location::Gnss::V2_0::GnssMeasurementInfo gnssMeasurementInfoNew;
    SetGnssClock(&gnssMeasurementInfoNew, gnssMeasurementInfo);
    gnssMeasurementInfoNew.elapsedRealtime = static_cast<long>(gnssMeasurementInfo->elapsedRealtime);
    gnssMeasurementInfoNew.uncertainty = static_cast<long>(gnssMeasurementInfo->uncertainty);
    gnssMeasurementInfoNew.measurementCount = static_cast<int>(gnssMeasurementInfo->measurementCount);

    std::vector<OHOS::HDI::Location::Gnss::V2_0::GnssMeasurement> gnssMeasurements;
    for (OHOS::HDI::Location::GnssMeasurement infoItem : gnssMeasurementInfo->measurements) {
        OHOS::HDI::Location::Gnss::V2_0::GnssMeasurement gnssMeasurement;
        gnssMeasurement.fieldValidflags = infoItem.fieldValidflags;
        gnssMeasurement.satelliteId = infoItem.satelliteId;
        gnssMeasurement.constellationCategory = infoItem.constellationCategory;
        gnssMeasurement.timeOffset = infoItem.timeOffset;
        gnssMeasurement.syncState = infoItem.syncState;
        gnssMeasurement.receivedSatelliteTime = infoItem.receivedSatelliteTime;
        gnssMeasurement.receivedSatelliteTimeUncertainty = infoItem.receivedSatelliteTimeUncertainty;
        gnssMeasurement.cn0 = infoItem.cn0;
        gnssMeasurement.pseudorangeRate = infoItem.pseudorangeRate;
        gnssMeasurement.pseudorangeRateUncertainty = infoItem.pseudorangeRateUncertainty;
        gnssMeasurement.accumulatedDeltaRangeFlag = infoItem.accumulatedDeltaRangeFlag;
        gnssMeasurement.accumulatedDeltaRange = infoItem.accumulatedDeltaRange;
        gnssMeasurement.accumulatedDeltaRangeUncertainty = infoItem.accumulatedDeltaRangeUncertainty;
        gnssMeasurement.carrierFrequency = infoItem.carrierFrequency;
        gnssMeasurement.carrierCyclesCount = infoItem.carrierCyclesCount;
        gnssMeasurement.carrierPhase = infoItem.carrierPhase;
        gnssMeasurement.carrierPhaseUncertainty = infoItem.carrierPhaseUncertainty;
        gnssMeasurement.multipathFlag = infoItem.multipathFlag;
        gnssMeasurement.agcGain = infoItem.agcGain;
        gnssMeasurement.codeCategory = infoItem.codeCategory;
        gnssMeasurement.ionoCorrect = infoItem.ionoCorrect;
        gnssMeasurement.tropCorrect = infoItem.tropCorrect;
        gnssMeasurement.satelliteClockBias = infoItem.satelliteClockBias;
        gnssMeasurement.satelliteClockDriftBias = infoItem.satelliteClockDriftBias;
        gnssMeasurements.push_back(gnssMeasurement);
    }
    gnssMeasurementInfoNew.measurements = gnssMeasurements;
    if (g_gnssMeasurementCallback != nullptr) {
        g_gnssMeasurementCallback->ReportGnssMeasurementInfo(gnssMeasurementInfoNew);
    }
}
#endif

static void GnssWorkingStatusUpdate(uint16_t *status)
{
    if (status == nullptr) {
        LBSLOGE(HDI_GNSS, "%s:param is nullptr.", __func__);
        return;
    }
    AutoLock lock(g_mutex);
    GnssWorkingStatus gnssStatus = static_cast<GnssWorkingStatus>(*status);
    if (g_gnssCallback != nullptr) {
        g_gnssCallback->ReportGnssWorkingStatus(gnssStatus);
    }
}

static void SvStatusCallback(GnssSatelliteStatus *svInfo)
{
    if (svInfo == nullptr) {
        LBSLOGE(HDI_GNSS, "%s:sv_info is null.", __func__);
        return;
    }
    LBSLOGD(HDI_GNSS, " ");
    AutoLock lock(g_mutex);
    SatelliteStatusInfo svStatus;
    svStatus.satellitesNumber = svInfo->satellitesNum;
    for (unsigned int i = 0; i < svInfo->satellitesNum; i++) {
        svStatus.satelliteIds.push_back(svInfo->satellitesList[i].satelliteId);
        svStatus.constellation.push_back(
            static_cast<ConstellationCategory>(svInfo->satellitesList[i].constellationCategory));
        svStatus.elevation.push_back(svInfo->satellitesList[i].elevation);
        svStatus.azimuths.push_back(svInfo->satellitesList[i].azimuth);
        svStatus.carrierFrequencies.push_back(svInfo->satellitesList[i].carrierFrequency);
        svStatus.carrierToNoiseDensitys.push_back(svInfo->satellitesList[i].cn0);
        svStatus.additionalInfo.push_back(svInfo->satellitesList[i].satelliteAdditionalInfo);
    }
    if (g_gnssCallback != nullptr) {
        g_gnssCallback->ReportSatelliteStatusInfo(svStatus);
    }
}

static void NmeaCallback(int64_t timestamp, const char *nmea, int length)
{
    if (nmea == nullptr) {
        LBSLOGE(HDI_GNSS, "%s:nmea is nullptr.", __func__);
        return;
    }
    AutoLock lock(g_mutex);
    if (g_gnssCallback != nullptr) {
        g_gnssCallback->ReportNmea(timestamp, nmea, length);
    }
}

static void GetGnssBasicCallbackMethods(GnssBasicCallbackIfaces *device)
{
    if (device == nullptr) {
        return;
    }
    device->size = sizeof(GnssCallbackStruct);
    device->locationUpdate = LocationUpdate;
    device->gnssWorkingStatusUpdate = GnssWorkingStatusUpdate;
    device->satelliteStatusUpdate = SvStatusCallback;
    device->nmeaUpdate = NmeaCallback;
    device->capabilitiesUpdate = nullptr;
    device->requestRefInfo = nullptr;
    device->requestExtendedEphemeris = nullptr;
}

static void GetGnssCacheCallbackMethods(GnssCacheCallbackIfaces *device)
{
    if (device == nullptr) {
        return;
    }
    device->size = 0;
    device->cachedLocationUpdate = nullptr;
}

static void GetGnssCallbackMethods(GnssCallbackStruct *device)
{
    if (device == nullptr) {
        return;
    }
    device->size = sizeof(GnssCallbackStruct);
    static GnssBasicCallbackIfaces basicCallback;
    GetGnssBasicCallbackMethods(&basicCallback);
    device->gnssCallback = basicCallback;
    static GnssCacheCallbackIfaces cacheCallback;
    GetGnssCacheCallbackMethods(&cacheCallback);
    device->gnssCacheCallback = cacheCallback;
}

#ifndef EMULATOR_ENABLED
static void GetGnssMeasurementCallbackMethods(GnssMeasurementCallbackIfaces *device)
{
    if (device == nullptr) {
        return;
    }
    device->size = sizeof(GnssMeasurementCallbackIfaces);
    device->gnssMeasurementUpdate = GnssMeasurementUpdate;
}
#endif

GnssInterfaceImpl &GnssInterfaceImpl::GetInstance()
{
    static GnssInterfaceImpl instance;
    InitStaticMutexLock(&g_mutex);
    return instance;
}

int32_t GnssInterfaceImpl::SetGnssConfigPara(const GnssConfigPara &para)
{
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    auto gnssInterface = LocationVendorInterface::GetInstance()->GetGnssVendorInterface();
    if (gnssInterface == nullptr) {
        LBSLOGE(HDI_GNSS, "%s:GetGnssVendorInterface return nullptr.", __func__);
        return GNSS_ERR_INVALID_PARAM;
    }
    g_configPara.startCategory = static_cast<uint32_t>(GnssStartCategory::GNSS_START_CATEGORY_NORMAL);
    g_configPara.u.gnssBasicConfig.gnssMode = para.gnssBasic.gnssMode;
    g_configPara.u.gnssBasicConfig.size = sizeof(GnssBasicConfigPara);
    int ret = gnssInterface->setGnssConfigPara(&g_configPara);
    LBSLOGI(HDI_GNSS, "%s, ret=%d", __func__, ret);
    return ret;
}

int32_t GnssInterfaceImpl::EnableGnss(IGnssCallback *callbackObj)
{
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    if (callbackObj == nullptr) {
        LBSLOGE(HDI_GNSS, "%s:invalid callbackObj", __func__);
        return GNSS_ERR_INVALID_PARAM;
    }
    AutoLock lock(g_mutex);

    static GnssCallbackStruct gnssCallback;
    GetGnssCallbackMethods(&gnssCallback);
    auto gnssInterface = LocationVendorInterface::GetInstance()->GetGnssVendorInterface();
    if (gnssInterface == nullptr) {
        LBSLOGE(HDI_GNSS, "%s:GetGnssVendorInterface return nullptr.", __func__);
        return GNSS_ERR_INVALID_PARAM;
    }
    int ret = gnssInterface->enableGnss(&gnssCallback);
    if (ret != GNSS_SUCCESS) {
        LBSLOGE(HDI_GNSS, "enableGnss failed.");
        return GNSS_FAILURE;
    }
    static GnssNetInitiatedCallbacks niCallback;
    niCallback.reportNiNotification = NiNotifyCallback;
    int moduleType = static_cast<int>(GnssModuleIfaceCategory::GNSS_NET_INITIATED_MODULE_INTERFACE);
    auto niInterface = static_cast< GnssNetInitiatedInterface *>(
        LocationVendorInterface::GetInstance()->GetModuleInterface(moduleType));
    if (niInterface != nullptr) {
        niInterface->setCallback(&niCallback);
    } else {
        LBSLOGE(HDI_GNSS, "%s:can not get gnssNiInterface.", __func__);
    }

    g_gnssCallback = callbackObj;
    return ret;
}

int32_t GnssInterfaceImpl::DisableGnss()
{
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    AutoLock lock(g_mutex);
    auto gnssInterface = LocationVendorInterface::GetInstance()->GetGnssVendorInterface();
    if (gnssInterface == nullptr) {
        LBSLOGE(HDI_GNSS, "%s:GetGnssVendorInterface return nullptr.", __func__);
        return GNSS_ERR_INVALID_PARAM;
    }
    int ret = gnssInterface->disableGnss();
    g_gnssCallback = nullptr;
    return ret;
}

int32_t GnssInterfaceImpl::StartGnss(GnssStartType type)
{
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    int startType = int(type);
    auto gnssInterface = LocationVendorInterface::GetInstance()->GetGnssVendorInterface();
    if (gnssInterface == nullptr) {
        LBSLOGE(HDI_GNSS, "%s:GetGnssVendorInterface return nullptr.", __func__);
        return GNSS_ERR_INVALID_PARAM;
    }
    return gnssInterface->startGnss(startType);
}

int32_t GnssInterfaceImpl::StopGnss(GnssStartType type)
{
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    int startType = static_cast<int>(type);
    auto gnssInterface = LocationVendorInterface::GetInstance()->GetGnssVendorInterface();
    if (gnssInterface == nullptr) {
        LBSLOGE(HDI_GNSS, "%s:GetGnssVendorInterface return nullptr.", __func__);
        return GNSS_ERR_INVALID_PARAM;
    }
    return gnssInterface->stopGnss(startType);
}

int32_t GnssInterfaceImpl::SetGnssReferenceInfo(const GnssRefInfo &refInfo)
{
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    auto gnssInterface = LocationVendorInterface::GetInstance()->GetGnssVendorInterface();
    if (gnssInterface == nullptr) {
        LBSLOGE(HDI_GNSS, "%s:GetGnssVendorInterface return nullptr.", __func__);
        return GNSS_ERR_INVALID_PARAM;
    }
    GnssReferenceInfo referenceInfo;
    referenceInfo.category = refInfo.type;
    switch (refInfo.type) {
        case GNSS_REF_INFO_TIME:
            referenceInfo.u.time.size = sizeof(GnssRefTime);
            referenceInfo.u.time.time = refInfo.time.time;
            referenceInfo.u.time.elapsedRealtime = refInfo.time.elapsedRealtime;
            referenceInfo.u.time.uncertaintyOfTime = refInfo.time.uncertaintyOfTime;
            referenceInfo.size = sizeof(GnssReferenceInfo);
            return gnssInterface->injectsGnssReferenceInfo(referenceInfo.category, &referenceInfo);
        case GNSS_REF_INFO_LOCATION:
            referenceInfo.u.gnssLocation.size = sizeof(GnssLocation);
            referenceInfo.u.gnssLocation.fieldValidity = refInfo.gnssLocation.fieldValidity;
            referenceInfo.u.gnssLocation.latitude = refInfo.gnssLocation.latitude;
            referenceInfo.u.gnssLocation.longitude = refInfo.gnssLocation.longitude;
            referenceInfo.u.gnssLocation.altitude = refInfo.gnssLocation.altitude;
            referenceInfo.u.gnssLocation.speed = refInfo.gnssLocation.speed;
            referenceInfo.u.gnssLocation.bearing = refInfo.gnssLocation.bearing;
            referenceInfo.u.gnssLocation.horizontalAccuracy = refInfo.gnssLocation.horizontalAccuracy;
            referenceInfo.u.gnssLocation.verticalAccuracy = refInfo.gnssLocation.verticalAccuracy;
            referenceInfo.u.gnssLocation.speedAccuracy = refInfo.gnssLocation.speedAccuracy;
            referenceInfo.u.gnssLocation.bearingAccuracy = refInfo.gnssLocation.bearingAccuracy;
            referenceInfo.u.gnssLocation.timeForFix = refInfo.gnssLocation.timeForFix;
            referenceInfo.u.gnssLocation.timeSinceBoot = refInfo.gnssLocation.timeSinceBoot;
            referenceInfo.u.gnssLocation.timeUncertainty = refInfo.gnssLocation.timeUncertainty;
            referenceInfo.size = sizeof(GnssReferenceInfo);
            return gnssInterface->injectsGnssReferenceInfo(referenceInfo.category, &referenceInfo);
        default:
            LBSLOGI(HDI_GNSS, "%s: do not support now", __func__);
            return GNSS_ERR_INVALID_PARAM;
    }
}

int32_t GnssInterfaceImpl::DeleteAuxiliaryData(unsigned short data)
{
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    uint16_t flags = data;
    LBSLOGI(HDI_GNSS, "%s, flag=%d", __func__, flags);
    auto gnssInterface = LocationVendorInterface::GetInstance()->GetGnssVendorInterface();
    if (gnssInterface == nullptr) {
        LBSLOGE(HDI_GNSS, "%s:GetGnssVendorInterface return nullptr.", __func__);
        return GNSS_ERR_INVALID_PARAM;
    }
    gnssInterface->removeAuxiliaryData(flags);
    return GNSS_SUCCESS;
}

int32_t GnssInterfaceImpl::SetPredictGnssData(const std::string &data)
{
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    return GNSS_SUCCESS;
}

int32_t GnssInterfaceImpl::GetCachedGnssLocationsSize(int32_t &size)
{
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    return GNSS_SUCCESS;
}

int32_t GnssInterfaceImpl::GetCachedGnssLocations()
{
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    return GNSS_SUCCESS;
}

int32_t GnssInterfaceImpl::SendNiUserResponse(int32_t gnssNiNotificationId, GnssNiResponseCmd userResponse)
{
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    int moduleType = static_cast<int>(GnssModuleIfaceCategory::GNSS_NET_INITIATED_MODULE_INTERFACE);
    auto niInterface = static_cast<const GnssNetInitiatedInterface *>(
        LocationVendorInterface::GetInstance()->GetModuleInterface(moduleType));
    if (niInterface == nullptr) {
        LBSLOGE(HDI_GNSS, "%s:can not get gnssNiInterface.", __func__);
        return GNSS_ERR_INVALID_PARAM;
    }
    niInterface->sendUserResponse(gnssNiNotificationId, static_cast<int32_t>(userResponse));
    return GNSS_SUCCESS;
}

int32_t GnssInterfaceImpl::SendNetworkInitiatedMsg(const std::string &msg, int length)
{
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    if (msg.empty()) {
        LBSLOGE(HDI_GNSS, "%s msg is empty", __func__);
        return GNSS_FAILURE;
    }
    int moduleType = static_cast<int>(GnssModuleIfaceCategory::GNSS_NET_INITIATED_MODULE_INTERFACE);
    auto niInterface = static_cast<const GnssNetInitiatedInterface *>(
        LocationVendorInterface::GetInstance()->GetModuleInterface(moduleType));
    if (niInterface == nullptr) {
        LBSLOGE(HDI_GNSS, "%s:can not get gnssNiInterface.", __func__);
        return GNSS_ERR_INVALID_PARAM;
    }
    std::vector<uint8_t> data = StringUtils::HexToByteVector(msg);
    LBSLOGI(HDI_GNSS, "%s. msg : %s, length %d, msg size %d, data size %d",
        __func__,
        msg.c_str(),
        length,
        int(msg.size()),
        int(data.size()));

    niInterface->sendNetworkInitiatedMsg(data.data(), data.size());
    return GNSS_SUCCESS;
}

int32_t GnssInterfaceImpl::EnableGnssMeasurement(IGnssMeasurementCallback *callbackObj)
{
#ifndef EMULATOR_ENABLED
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    if (callbackObj == nullptr) {
        LBSLOGE(HDI_GNSS, "%s:invalid callbackObj", __func__);
        return GNSS_ERR_INVALID_PARAM;
    }
    AutoLock lock(g_mutex);
    static GnssMeasurementCallbackIfaces gnssMeasurementCallback;
    GetGnssMeasurementCallbackMethods(&gnssMeasurementCallback);
    int moduleType = static_cast<int>(GnssModuleIfaceCategory::GNSS_MEASUREMENT_MODULE_INTERFACE);
    auto gnssMeasurementInterface = static_cast<const GnssMeasurementInterface *>(
        LocationVendorInterface::GetInstance()->GetModuleInterface(moduleType));
    if (gnssMeasurementInterface == nullptr) {
        LBSLOGE(HDI_GNSS, "%s:GetModuleInterface return nullptr.", __func__);
        return GNSS_ERR_INVALID_PARAM;
    }
    bool result = gnssMeasurementInterface->enable(&gnssMeasurementCallback);
    if (!result) {
        LBSLOGE(HDI_GNSS, "enableGnssMeasurement failed.");
        return GNSS_FAILURE;
    }
    g_gnssMeasurementCallback = callbackObj;
    return GNSS_SUCCESS;
#else
    return GNSS_SUCCESS;
#endif
}

int32_t GnssInterfaceImpl::DisableGnssMeasurement()
{
#ifndef EMULATOR_ENABLED
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    AutoLock lock(g_mutex);
    int moduleType = static_cast<int>(GnssModuleIfaceCategory::GNSS_MEASUREMENT_MODULE_INTERFACE);
    auto gnssMeasurementInterface = static_cast<const GnssMeasurementInterface *>(
        LocationVendorInterface::GetInstance()->GetModuleInterface(moduleType));
    if (gnssMeasurementInterface == nullptr) {
        LBSLOGE(HDI_GNSS, "%s:GnssMeasurementInterface return nullptr.", __func__);
        return GNSS_ERR_INVALID_PARAM;
    }
    gnssMeasurementInterface->disable();
    g_gnssMeasurementCallback = nullptr;
#endif
    return GNSS_SUCCESS;
}

void GnssInterfaceImpl::ResetGnss()
{
    LBSLOGI(HDI_GNSS, "%s called.", __func__);
    StopGnss(GNSS_START_TYPE_NORMAL);
    DisableGnssMeasurement();
    DisableGnss();
}
}  // namespace V2_0
}  // namespace Gnss
}  // namespace Location
}  // namespace HDI
}  // namespace OHOS
