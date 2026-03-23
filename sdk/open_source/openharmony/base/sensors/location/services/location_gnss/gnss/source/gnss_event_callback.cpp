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

#ifdef FEATURE_GNSS_SUPPORT
#include "gnss_event_callback.h"
#include <sys/time.h>
#include "common_utils.h"
#include "gnss_ability.h"
#include "location_log.h"
#include "agnss_ni_manager.h"

#ifdef TIME_SERVICE_ENABLE
#include "ntp_time_check.h"
#include "time_service_client.h"
#endif
namespace OHOS {
namespace Location {
const int WEAK_GPS_SIGNAL_SCENARIO_COUNT = 3;
const int MAX_SV_COUNT = 64;
const int GPS_DUMMY_SV_COUNT = 5;
const int AZIMUTH_DEGREES = 60;
const int ELEVATION_DEGREES = 90;
bool g_hasLocation = false;
bool g_svIncrease = false;
std::shared_ptr<SatelliteStatus> g_svInfo = nullptr;

static void SetGpsTime(int64_t gpsTime)
{
#ifdef TIME_SERVICE_ENABLE
    auto ntpTimeCheck = NtpTimeCheck::GetInstance();
    if (ntpTimeCheck != nullptr) {
        auto elapsedRealTime = MiscServices::TimeServiceClient::GetInstance()->GetBootTimeMs();
        ntpTimeCheck->SetGpsTime(gpsTime, elapsedRealTime);
    }
#endif
}

int32_t GnssEventCallback::ReportLocation(const LocationInfo& location)
{
    // std::string identity = IPCSkeleton::ResetCallingIdentity();
    std::string identity = "";
    LBSLOGI(LOCATOR_GNSS, "location latitude:%f !", location.latitude);
    std::shared_ptr<Location> locationNew = std::make_shared<Location>();
    locationNew->SetLatitude(location.latitude);
    locationNew->SetLongitude(location.longitude);
    locationNew->SetAltitude(location.altitude);
    locationNew->SetAccuracy(location.horizontalAccuracy);
    locationNew->SetSpeed(location.speed);
    locationNew->SetDirection(location.bearing);
    locationNew->SetAltitudeAccuracy(location.verticalAccuracy);
    locationNew->SetSpeedAccuracy(location.speedAccuracy);
    locationNew->SetDirectionAccuracy(location.bearingAccuracy);
    locationNew->SetNorthVelocity(location.velocityNorth);
    locationNew->SetEastVelocity(location.velocityEast);
    locationNew->SetDescendVelocity(location.velocityDescend);
    locationNew->SetTimeStamp(location.timeForFix);
    locationNew->SetTimeSinceBoot(location.timeSinceBoot);
    locationNew->SetUncertaintyOfTimeSinceBoot(location.timeUncertainty);
    locationNew->SetStatusNumber(location.signal);
    locationNew->SetLocationSourceType(LocationSourceType::GNSS_TYPE);

    // add dummy sv if needed
    SendDummySvInfo();
    struct timeval now;
    gettimeofday(&now, NULL);

    GnssAbility::GetInstance().ReportLocationInfo(GNSS_ABILITY, locationNew);
#ifdef FEATURE_PASSIVE_SUPPORT
    GnssAbility::GetInstance().ReportLocationInfo(PASSIVE_ABILITY, locationNew);
#endif
    SetGpsTime(locationNew->GetTimeStamp());
    return ERRCODE_SUCCESS;
}

int32_t GnssEventCallback::ReportGnssWorkingStatus(GnssWorkingStatus status)
{
    GnssAbility::GetInstance().ReportGnssSessionStatus(static_cast<int>(status));
    return ERRCODE_SUCCESS;
}

int32_t GnssEventCallback::ReportNmea(int64_t timestamp, const std::string& nmea, int32_t length)
{
    std::string nmeaStr = nmea;
    GnssAbility::GetInstance().ReportNmea(timestamp, nmeaStr);
    return ERRCODE_SUCCESS;
}

int32_t GnssEventCallback::ReportGnssCapabilities(unsigned int capabilities)
{
    return ERRCODE_SUCCESS;
}

int32_t GnssEventCallback::ReportSatelliteStatusInfo(const SatelliteStatusInfo& info)
{
    std::shared_ptr<SatelliteStatus> svStatus = std::make_shared<SatelliteStatus>();
    if (info.satellitesNumber < 0) {
        LBSLOGD(LOCATOR_GNSS, "SvStatusCallback, satellites_num < 0!");
        return ERRCODE_INVALID_PARAMS;
    }
    std::vector<std::string> names;
    std::vector<std::string> satelliteStatusInfos;
    names.push_back("SatelliteStatusInfo");
    satelliteStatusInfos.push_back(std::to_string(info.satellitesNumber));

    svStatus->SetSatellitesNumber(info.satellitesNumber);
    for (unsigned int i = 0; i < info.satellitesNumber; i++) {
        svStatus->SetAltitude(info.elevation[i]);
        svStatus->SetAzimuth(info.azimuths[i]);
        svStatus->SetCarrierFrequencie(info.carrierFrequencies[i]);
        svStatus->SetCarrierToNoiseDensity(info.carrierToNoiseDensitys[i]);
        svStatus->SetSatelliteId(info.satelliteIds[i]);
        svStatus->SetConstellationType(info.constellation[i]);
        svStatus->SetSatelliteAdditionalInfo(info.additionalInfo[i]);
        std::string stainfo = "satelliteId : " + std::to_string(info.satelliteIds[i]) +
            ", carrierToNoiseDensity : " + std::to_string(info.carrierToNoiseDensitys[i]) +
            ", elevation : " + std::to_string(info.elevation[i]) +
            ", azimuth : " + std::to_string(info.azimuths[i]) +
            ", carrierFrequencie : " + std::to_string(info.carrierFrequencies[i]);
        names.push_back(std::to_string(i));
        satelliteStatusInfos.push_back(stainfo);
        LBSLOGD(LOCATOR_GNSS, "SvStatusCallback, stainfo:%s!", stainfo.c_str());
    }
    // save sv info
    g_svInfo = nullptr;
    g_svInfo = std::make_shared<SatelliteStatus>(*svStatus);
    GnssAbility::GetInstance().ReportSv(svStatus);
    return ERRCODE_SUCCESS;
}

void GnssEventCallback::SendDummySvInfo()
{
    if (g_svInfo == nullptr) {
        LBSLOGE(LOCATOR_GNSS, "sv is nullptr.");
        return;
    }
    // indicates location is coming
    g_hasLocation = true;
    int usedSvCount = 0;
    int svListSize = g_svInfo->GetSatellitesNumber();
    // calculate the num of used GPS satellites
    for (int svSize = 0; svSize < svListSize; svSize++) {
        if (IsSvTypeGps(g_svInfo, svSize) && IsSvUsed(g_svInfo, svSize)) {
            usedSvCount++;
        }
    }
    LBSLOGD(LOCATOR_GNSS, " the USED GPS SV Count is %d", usedSvCount);
    // weak gps signal scenario
    if (usedSvCount <= WEAK_GPS_SIGNAL_SCENARIO_COUNT) {
        // indicates the need for dummy satellites
        g_svIncrease = true;
        LBSLOGD(LOCATOR_GNSS, "start increase dummy sv");

        if (MAX_SV_COUNT - svListSize >= GPS_DUMMY_SV_COUNT) {
            AddDummySv(g_svInfo, 4, 6); // sv1: svid = 4, cN0Dbhz = 6
            AddDummySv(g_svInfo, 7, 15); // sv2: svid = 7, cN0Dbhz = 15
            AddDummySv(g_svInfo, 1, 2); // sv3: svid = 1, cN0Dbhz = 2
            AddDummySv(g_svInfo, 11, 10); // sv4: svid = 11, cN0Dbhz = 10
            AddDummySv(g_svInfo, 17, 5); // sv5: svid = 17, cN0Dbhz = 5
            g_svInfo->
                SetSatellitesNumber(g_svInfo->GetSatellitesNumber() + GPS_DUMMY_SV_COUNT);
            ReportDummySv(g_svInfo);
        } else {
            LBSLOGD(LOCATOR_GNSS, "sv number > 58, no need send dummy satellites");
        }
        LBSLOGD(LOCATOR_GNSS, " increase sv finished");
    } else {
        // indicates no need for dummy satellites
        g_svIncrease = false;
    }
}

void GnssEventCallback::ReportDummySv(const std::shared_ptr<SatelliteStatus> &sv)
{
    if (sv == nullptr) {
        LBSLOGE(LOCATOR_GNSS, "gnss sv is nullptr.");
        return;
    }
    GnssAbility::GetInstance().ReportSv(sv);
}

bool GnssEventCallback::IsNeedSvIncrease()
{
    if (g_hasLocation && g_svIncrease) {
        return true;
    }
    return false;
}

bool GnssEventCallback::IsSvTypeGps(const std::shared_ptr<SatelliteStatus> &sv, int index)
{
    if (sv == nullptr) {
        return false;
    }
    return sv->GetConstellationTypes()[index] == HDI::Location::Gnss::V2_0::CONSTELLATION_CATEGORY_GPS;
}

bool GnssEventCallback::IsSvUsed(const std::shared_ptr<SatelliteStatus> &sv, int index)
{
    if (sv == nullptr) {
        return false;
    }
    return static_cast<uint32_t>(sv->GetSatelliteAdditionalInfoList()[index]) &
        static_cast<uint8_t>(HDI::Location::Gnss::V2_0::SATELLITES_ADDITIONAL_INFO_USED_IN_FIX);
}

void GnssEventCallback::AddDummySv(std::shared_ptr<SatelliteStatus> &sv, int svid, int cN0Dbhz)
{
    if (sv == nullptr) {
        return;
    }
    sv->SetSatelliteId(svid);
    sv->SetConstellationType(HDI::Location::Gnss::V2_0::CONSTELLATION_CATEGORY_GPS);
    sv->SetCarrierToNoiseDensity(cN0Dbhz);
    sv->SetAltitude(ELEVATION_DEGREES); // elevationDegrees
    sv->SetAzimuth(AZIMUTH_DEGREES); // azimuthDegrees
    sv->SetCarrierFrequencie(0); // carrierFrequencyHz
}

int32_t GnssEventCallback::RequestGnssReferenceInfo(GnssRefInfoType type)
{
    LBSLOGI(LOCATOR_GNSS, "RequestGnssReferenceInfo: request type %d", static_cast<int>(type));
    switch (type) {
        case GnssRefInfoType::GNSS_REF_INFO_TIME:
            GnssAbility::GetInstance().InjectTime();
            break;
        case GnssRefInfoType::GNSS_REF_INFO_LOCATION:
            GnssAbility::GetInstance().InjectLocation();
            break;
        default:
            LBSLOGI(LOCATOR_GNSS, "RequestGnssReferenceInfo: request type not support now");
            break;
    }
    return ERRCODE_SUCCESS;
}

int32_t GnssEventCallback::RequestPredictGnssData()
{
    return ERRCODE_SUCCESS;
}

int32_t GnssEventCallback::ReportCachedLocation(const std::vector<LocationInfo>& gnssLocations)
{
    return ERRCODE_SUCCESS;
}

int32_t GnssEventCallback::ReportGnssNiNotification(const GnssNiNotificationRequest& notification)
{
#ifdef FEATURE_AGNSS_NI_SUPPORT
    auto agnssNiManager = AGnssNiManager::GetInstance();
    if (agnssNiManager == nullptr) {
        LBSLOGE(LOCATOR_GNSS, "ReportGnssNiNotification: agnssNiManager is nullptr.");
        return ERRCODE_SUCCESS;
    }
    agnssNiManager->HandleNiNotification(notification);
#endif
    return ERRCODE_SUCCESS;
}
}  // namespace Location
}  // namespace OHOS
#endif
