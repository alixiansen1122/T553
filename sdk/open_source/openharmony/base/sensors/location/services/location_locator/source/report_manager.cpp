/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "report_manager.h"

#include <cmath>
#ifdef PERMISSION_SUPPORT
#include "privacy_kit.h"
#endif
#include "common_utils.h"
#include "constant_definition.h"
#include "fusion_controller.h"
#include "i_locator_callback.h"
#include "location_log.h"
#include "locator_ability.h"
#include "permission_manager.h"

namespace OHOS {
namespace Location {
const long NANOS_PER_MILLI = 1000000L;
const int MAX_SA_SCHEDULING_JITTER_MS = 200;
static constexpr double MAXIMUM_FUZZY_LOCATION_DISTANCE = 4000.0; // Unit m
static constexpr double MINIMUM_FUZZY_LOCATION_DISTANCE = 3000.0; // Unit m
static constexpr int GNSS_FIX_CACHED_TIME = 60;
static constexpr int NLP_FIX_CACHED_TIME = 45;
const double DEFAULT_APPROXIMATELY_ACCURACY = 5000.0;

ReportManager* ReportManager::GetInstance()
{
    static ReportManager data;
    return &data;
}

ReportManager::ReportManager()
{
    clock_gettime(CLOCK_REALTIME, &lastUpdateTime_);
    offsetRandom_ = CommonUtils::DoubleRandom(0, 1);
    MutexAttr attr = { false };
    lastLocationMutex_ = MutexCreate(&attr);
}

ReportManager::~ReportManager() 
{
    MutexDestroy(&lastLocationMutex_);
}

bool ReportManager::OnReportLocation(const std::shared_ptr<Location>& location, std::string abilityName)
{
    LBSLOGI(REPORT_MANAGER, "OnReportLocation GetLatitude : %f", location->GetLatitude());
    auto fusionController = FusionController::GetInstance();
    if (fusionController == nullptr) {
        return false;
    }
    UpdateCacheLocation(location, abilityName);

    auto requestMap = LocatorAbility::GetInstance().GetRequests();
    if (requestMap == nullptr) {
        return false;
    }
    auto requestListIter = requestMap->find(abilityName);
    if (requestListIter == requestMap->end()) {
        return false;
    }
    auto requestList = requestListIter->second;
    std::list<std::shared_ptr<LocationRequest>> deadRequests;
    for (auto iter = requestList.begin(); iter != requestList.end(); iter++) {
        auto request = *iter;
        ProcessRequestForReport(request, deadRequests, location, abilityName);
    }
    for (auto iter = deadRequests.begin(); !deadRequests.empty() && iter != deadRequests.end(); ++iter) {
        auto request = *iter;
        if (request == nullptr) {
            continue;
        }
        auto requestManger = RequestManager::GetInstance();
        if (requestManger != nullptr) {
            LBSLOGI(REPORT_MANAGER, "UpdateRequestRecord");
            requestManger->UpdateRequestRecord(request, false);
        }
    }
    LocatorAbility::GetInstance().ApplyRequests(1);
    deadRequests.clear();
    return true;
}

void ReportManager::UpdateLocationByRequest(const uint32_t tokenId, const uint64_t tokenIdEx,
    std::shared_ptr<Location>& location)
{
    if (location == nullptr) {
        return;
    }
}

bool ReportManager::ProcessRequestForReport(std::shared_ptr<LocationRequest>& request,
    std::list<std::shared_ptr<LocationRequest>> & deadRequests,
    const std::shared_ptr<Location>& location, std::string abilityName)
{
    if (location == nullptr ||
        request == nullptr || request->GetRequestConfig() == nullptr || !request->GetIsRequesting()) {
        return false;
    }
    std::shared_ptr<Location> fuseLocation;
    std::shared_ptr<Location> finalLocation;
    if (IsRequestFuse(request)) {
        auto fusionController = FusionController::GetInstance();
        if (fusionController == nullptr) {
            return false;
        }
        std::shared_ptr<Location> bestLocation = std::make_shared<Location>(*request->GetBestLocation());
        fuseLocation = fusionController->GetFuseLocation(location, bestLocation);
        request->SetBestLocation(*fuseLocation);
    }
    finalLocation = GetPermittedLocation(request, IsRequestFuse(request) ? fuseLocation : location);
    if (!ResultCheck(finalLocation, request)) {
        // add location permission using record
        int permUsedType = request->GetPermUsedType();
        LocatorAbility::GetInstance().UpdatePermissionUsedRecord(request->GetTokenId(),
            ACCESS_APPROXIMATELY_LOCATION, permUsedType, 0, 1);
        return false;
    }
    UpdateLocationByRequest(request->GetTokenId(), request->GetTokenIdEx(), finalLocation);

    request->SetLastLocation(*finalLocation);
    auto locatorCallback = request->GetLocatorCallBack();
    if (locatorCallback != nullptr) {
        LBSLOGI(REPORT_MANAGER, "report location to %d, TimeSinceBoot : %s",
            request->GetTokenId(), std::to_string(finalLocation->GetTimeSinceBoot()).c_str());
        locatorCallback->OnLocationReport(finalLocation);
        // add location permission using record
        int permUsedType = request->GetPermUsedType();
        LocatorAbility::GetInstance().UpdatePermissionUsedRecord(request->GetTokenId(),
            ACCESS_APPROXIMATELY_LOCATION, permUsedType, 1, 0);
    }
    int fixTime = request->GetRequestConfig()->GetFixNumber();
    if (fixTime > 0) {
        deadRequests.push_back(request);
        return false;
    }
    return true;
}

std::shared_ptr<Location> ReportManager::GetPermittedLocation(const std::shared_ptr<LocationRequest>& request,
    const std::shared_ptr<Location>& location)
{
    if (location == nullptr) {
        return nullptr;
    }
    std::string bundleName = "";
    auto tokenId = request->GetTokenId();
    auto firstTokenId = request->GetFirstTokenId();
    auto tokenIdEx = request->GetTokenIdEx();
    auto uid =  request->GetUid();
    if (!PermissionManager::CheckSystemPermission(tokenId, tokenIdEx) &&
        !CommonUtils::CheckAppForUser(uid)) {
        return nullptr;
    }
    std::shared_ptr<Location> finalLocation = location;
    // for api8 and previous version, only ACCESS_LOCATION permission granted also report original location info.
    if (PermissionManager::CheckLocationPermission(tokenId, firstTokenId)) {
        return finalLocation;
    }
    if (PermissionManager::CheckApproximatelyPermission(tokenId, firstTokenId)) {
        LBSLOGI(REPORT_MANAGER, "%d has ApproximatelyLocation permission", tokenId);
        finalLocation = ApproximatelyLocation(location);
        return finalLocation;
    }
    LBSLOGE(REPORT_MANAGER, "%d has no location permission failed", tokenId);
    auto locationErrorCallback = request->GetLocationErrorCallBack();
    if (locationErrorCallback != nullptr) {
        locationErrorCallback->OnErrorReport(LOCATING_FAILED_LOCATION_PERMISSION_DENIED);
    }
    return nullptr;
}

bool ReportManager::ReportRemoteCallback(ILocatorCallback* locatorCallback, int type, int result)
{
    switch (type) {
        case ILocatorCallback::RECEIVE_LOCATION_STATUS_EVENT: {
            locatorCallback->OnLocatingStatusChange(result);
            break;
        }
        case ILocatorCallback::RECEIVE_ERROR_INFO_EVENT: {
            locatorCallback->OnErrorReport(result);
            break;
        }
        default:
            return false;
    }
    return true;
}

bool ReportManager::ResultCheck(const std::shared_ptr<Location>& location,
    const std::shared_ptr<LocationRequest>& request)
{
    if (request == nullptr) {
        return false;
    }
    if (location == nullptr) {
        return false;
    }
    int permissionLevel = PermissionManager::GetPermissionLevel(request->GetTokenId(), request->GetFirstTokenId());
    if (request->GetLastLocation() == nullptr || request->GetRequestConfig() == nullptr) {
        return true;
    }
    float maxAcc = request->GetRequestConfig()->GetMaxAccuracy();
    LBSLOGD(REPORT_MANAGER, "acc ResultCheck :  %f - %f", maxAcc, location->GetAccuracy());
    if ((permissionLevel == PERMISSION_ACCURATE) &&
        (maxAcc > 0) && (location->GetAccuracy() > maxAcc)) {
        LBSLOGE(REPORT_MANAGER, "accuracy check fail, do not report location");
        return false;
    }
    if (CommonUtils::DoubleEqual(request->GetLastLocation()->GetLatitude(), MIN_LATITUDE - 1)) {
        LBSLOGD(REPORT_MANAGER, "no valid cache location, no need to check");
        return true;
    }
    int minTime = request->GetRequestConfig()->GetTimeInterval();
    long deltaMs = (location->GetTimeSinceBoot() - request->GetLastLocation()->GetTimeSinceBoot()) / NANOS_PER_MILLI;
    if (deltaMs < (minTime * MILLI_PER_SEC - MAX_SA_SCHEDULING_JITTER_MS)) {
        LBSLOGE(REPORT_MANAGER,
            "%d timeInterval check fail, do not report location, current deltaMs = %ld",
            request->GetTokenId(), deltaMs);
        return false;
    }

    double distanceInterval = request->GetRequestConfig()->GetDistanceInterval();
    double deltaDis = CommonUtils::CalDistance(location->GetLatitude(), location->GetLongitude(),
        request->GetLastLocation()->GetLatitude(), request->GetLastLocation()->GetLongitude());
    if (deltaDis - distanceInterval < 0) {
        LBSLOGE(REPORT_MANAGER, "%d distanceInterval check fail, do not report location",
            request->GetTokenId());
        return false;
    }
    return true;
}

void ReportManager::UpdateCacheLocation(const std::shared_ptr<Location>& location, std::string abilityName)
{
    if (abilityName == GNSS_ABILITY) {
        cacheGnssLocation_ = *location;
        UpdateLastLocation(location);
    } else if (abilityName == NETWORK_ABILITY &&
        location->GetLocationSourceType() != LocationSourceType::INDOOR_TYPE) {
        cacheNlpLocation_ = *location;
        UpdateLastLocation(location);
    } else {
        UpdateLastLocation(location);
    }
}

void ReportManager::UpdateLastLocation(const std::shared_ptr<Location>& location)
{
    int currentUserId = 0;
    if (CommonUtils::GetCurrentUserId(currentUserId)) {
        AutoLock lock(lastLocationMutex_);
        lastLocationsMap_.insert(std::make_pair(currentUserId, std::make_shared<Location>(*location)));
    }
}

std::shared_ptr<Location> ReportManager::GetLastLocation()
{
    int currentUserId = 0;
    if (CommonUtils::GetCurrentUserId(currentUserId)) {
        AutoLock lock(lastLocationMutex_);
        auto iter = lastLocationsMap_.find(currentUserId);
        if (iter == lastLocationsMap_.end()) {
            return nullptr;
        }
        std::shared_ptr<Location> lastLocation = iter->second;
        if (CommonUtils::DoubleEqual(lastLocation->GetLatitude(), MIN_LATITUDE - 1)) {
            return nullptr;
        }
        return lastLocation;
    }
    return nullptr;
}

std::shared_ptr<Location> ReportManager::GetCacheLocation(const std::shared_ptr<LocationRequest>& request)
{
    int64_t curTime = CommonUtils::GetCurrentTimeStamp();
    std::shared_ptr<Location> cacheLocation = nullptr;
    if (!CommonUtils::DoubleEqual(cacheGnssLocation_.GetLatitude(), MIN_LATITUDE - 1) &&
        (curTime - cacheGnssLocation_.GetTimeStamp() / MILLI_PER_SEC) <= GNSS_FIX_CACHED_TIME) {
        cacheLocation = std::make_shared<Location>(cacheGnssLocation_);
    } else if (!CommonUtils::DoubleEqual(cacheNlpLocation_.GetLatitude(), MIN_LATITUDE - 1) &&
        (curTime - cacheNlpLocation_.GetTimeStamp() / MILLI_PER_SEC) <= NLP_FIX_CACHED_TIME) {
        cacheLocation = std::make_shared<Location>(cacheNlpLocation_);
    }
    std::shared_ptr<Location> finalLocation = GetPermittedLocation(request, cacheLocation);
    if (!ResultCheck(finalLocation, request)) {
        return nullptr;
    }
    UpdateLocationByRequest(request->GetTokenId(), request->GetTokenIdEx(), finalLocation);
    return finalLocation;
}

void ReportManager::UpdateRandom()
{
    int num = LocatorAbility::GetInstance().GetActiveRequestNum();
    if (num > 0) {
        LBSLOGD(REPORT_MANAGER, "Exists %d active request, cannot refresh offset", num);
        return;
    }
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    if (abs(now.tv_sec - lastUpdateTime_.tv_sec) > LONG_TIME_INTERVAL) {
        offsetRandom_ = CommonUtils::DoubleRandom(0, 1);
    }
}

std::shared_ptr<Location> ReportManager::ApproximatelyLocation(const std::shared_ptr<Location>& location)
{
    std::shared_ptr<Location> coarseLocation = std::make_shared<Location>(*location);
    double startLat = coarseLocation->GetLatitude();
    double startLon = coarseLocation->GetLongitude();
    double brg = offsetRandom_ * DIS_FROMLL_PARAMETER * M_PI; // 2PI
    double dist = offsetRandom_ * (MAXIMUM_FUZZY_LOCATION_DISTANCE -
        MINIMUM_FUZZY_LOCATION_DISTANCE) + MINIMUM_FUZZY_LOCATION_DISTANCE;
    double perlat = (DIS_FROMLL_PARAMETER * M_PI * EARTH_RADIUS) / DEGREE_DOUBLE_PI; // the radian value of per degree

    double lat = startLat + (dist * sin(brg)) / perlat;
    double lon;
    if (cos(brg) < 0) {
        lon = startLon - (dist * DEGREE_DOUBLE_PI) / (DIS_FROMLL_PARAMETER * M_PI * EARTH_RADIUS);
    } else {
        lon = startLon + (dist * DEGREE_DOUBLE_PI) / (DIS_FROMLL_PARAMETER * M_PI * EARTH_RADIUS);
    }
    if (lat < -MAX_LATITUDE) {
        lat = -MAX_LATITUDE;
    } else if (lat > MAX_LATITUDE) {
        lat = MAX_LATITUDE;
    } else {
        lat = std::round(lat * std::pow(10, 8)) / std::pow(10, 8); // 8 decimal
    }
    if (lon < -MAX_LONGITUDE) {
        lon = -MAX_LONGITUDE;
    } else if (lon > MAX_LONGITUDE) {
        lon = MAX_LONGITUDE;
    } else {
        lon = std::round(lon * std::pow(10, 8)) / std::pow(10, 8); // 8 decimal
    }
    coarseLocation->SetLatitude(lat);
    coarseLocation->SetLongitude(lon);
    coarseLocation->SetAccuracy(DEFAULT_APPROXIMATELY_ACCURACY); // approximately location acc
    std::vector<std::string> emptyAdds;
    coarseLocation->SetAdditions(emptyAdds, false);
    coarseLocation->SetAdditionSize(0);
    return coarseLocation;
}

bool ReportManager::IsRequestFuse(const std::shared_ptr<LocationRequest>& request)
{
    if (request == nullptr || request->GetRequestConfig() == nullptr) {
        return false;
    }
    if ((request->GetRequestConfig()->GetScenario() == SCENE_UNSET &&
        request->GetRequestConfig()->GetPriority() == PRIORITY_LOW_POWER) ||
        request->GetRequestConfig()->GetScenario() == SCENE_NO_POWER ||
        request->GetRequestConfig()->GetScenario() == SCENE_DAILY_LIFE_SERVICE) {
        return false;
    }
    return true;
}

} // namespace OHOS
} // namespace Location
