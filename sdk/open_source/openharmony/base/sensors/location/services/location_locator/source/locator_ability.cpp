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

#include <cmath>
#include "locator_ability.h"
#include "location_log.h"
#include "constant_definition.h"
#include "gnss_ability.h"
#include "location_config_manager.h"
#include "permission_status_change_cb.h"
#include "work_record_statistic.h"
#include "permission_manager.h"

namespace OHOS {
namespace Location {
IMPLEMENT_SINGLE_INSTANCE(LocatorAbility);

const float_t PRECISION = 0.000001;
const uint32_t REQUEST_DEFAULT_TIMEOUT_SECOUND = 5 * 60;

bool LocatorAbility::Initialize()
{
    if (isInited_) {
        return true;
    }
    MutexAttr attr = {false};
    requestsMutex_ = MutexCreate(&attr);
    receiversMutex_ = MutexCreate(&attr);
    loadedAbilityMapMutex_ = MutexCreate(&attr);
    requests_ = std::make_shared<std::map<std::string, std::list<std::shared_ptr<LocationRequest>>>>();
    receivers_ = std::make_shared<std::map<ILocatorCallback *, std::list<std::shared_ptr<LocationRequest>>>>();
    loadedAbilityMap_ = std::make_shared<std::map<std::string, SubAbility *>>();
    UpdateLoadedAbilityMap();
    InitAbility();
    InitRequestManagerMap();
#ifdef PERMISSION_SUPPORT
    permissionMap_ = std::make_shared<std::map<uint32_t, std::shared_ptr<PermissionStatusChangeCb>>>();
#endif
    reportManager_ = ReportManager::GetInstance();
    requestManager_ = RequestManager::GetInstance();
    isInited_ = true;
    LBSLOGI(LOCATOR, "LocatorAbility Initialize.");
    return true;
}

bool LocatorAbility::DeInitialize()
{
    if (!isInited_) {
        return false;
    }
    DeInitAbility();
    MutexDestroy(&requestsMutex_);
    requestsMutex_ = nullptr;
    MutexDestroy(&receiversMutex_);
    receiversMutex_ = nullptr;
    MutexDestroy(&loadedAbilityMapMutex_);
    loadedAbilityMapMutex_ = nullptr;
    requests_ = nullptr;
    receivers_ = nullptr;
    loadedAbilityMap_ = nullptr;
#ifdef PERMISSION_SUPPORT
    permissionMap_ = nullptr;
#endif
    reportManager_ = nullptr;
    requestManager_ = nullptr;
    isInited_ = false;
    return true;
}

LocationErrCode LocatorAbility::InitAbility()
{
    LBSLOGI(LOCATOR, "initAbility start");
    AutoLock lock(loadedAbilityMapMutex_);
    for (auto iter = loadedAbilityMap_->begin(); iter != loadedAbilityMap_->end(); iter++) {
        SubAbility *abilityObject = iter->second;
        int error = abilityObject->Initialize();
        if (error != ERRCODE_SUCCESS) {
            LBSLOGE(LOCATOR, "Initialize %s ability, result %d", (iter->first).c_str(), error);
            return ERRCODE_SERVICE_UNAVAILABLE;
        }
    }
    return ERRCODE_SUCCESS;
}

LocationErrCode LocatorAbility::DeInitAbility()
{
    LBSLOGI(LOCATOR, "DeInitAbility start");
    AutoLock lock(loadedAbilityMapMutex_);
    for (auto iter = loadedAbilityMap_->begin(); iter != loadedAbilityMap_->end(); iter++) {
        SubAbility *abilityObject = iter->second;
        int error = abilityObject->DeInitialize();
        if (error != ERRCODE_SUCCESS) {
            LBSLOGE(LOCATOR, "DeInitialize %s ability, result %d", (iter->first).c_str(), error);
            return ERRCODE_SERVICE_UNAVAILABLE;
        }
    }
    return ERRCODE_SUCCESS;
}

LocationErrCode LocatorAbility::StartAbility()
{
    LBSLOGI(LOCATOR, "StartAbility");
    AutoLock lock(loadedAbilityMapMutex_);
    for (auto iter = loadedAbilityMap_->begin(); iter != loadedAbilityMap_->end(); iter++) {
        SubAbility *abilityObject = iter->second;
        int error = abilityObject->StartAbility();
        if (error != ERRCODE_SUCCESS) {
            LBSLOGE(LOCATOR, "StartAbility %s ability, result %d", (iter->first).c_str(), error);
            return ERRCODE_SERVICE_UNAVAILABLE;
        }
    }
    return ERRCODE_SUCCESS;
}

LocationErrCode LocatorAbility::StopAbility()
{
    LBSLOGI(LOCATOR, "StopAbility");
    AutoLock lock(loadedAbilityMapMutex_);
    for (auto iter = loadedAbilityMap_->begin(); iter != loadedAbilityMap_->end(); iter++) {
        SubAbility *abilityObject = iter->second;
        int error = abilityObject->StopAbility();
        if (error != ERRCODE_SUCCESS) {
            LBSLOGE(LOCATOR, "StopAbility %s ability, result %d", (iter->first).c_str(), error);
            return ERRCODE_SERVICE_UNAVAILABLE;
        }
    }
    return ERRCODE_SUCCESS;
}

void LocatorAbility::InitRequestManagerMap()
{
    AutoLock lock(requestsMutex_);
    if (requests_ != nullptr) {
#ifdef FEATURE_GNSS_SUPPORT
        std::list<std::shared_ptr<LocationRequest>> gnssList;
        requests_->insert(make_pair(GNSS_ABILITY, gnssList));
#endif
#ifdef FEATURE_NETWORK_SUPPORT
        std::list<std::shared_ptr<LocationRequest>> networkList;
        requests_->insert(make_pair(NETWORK_ABILITY, networkList));
#endif
#ifdef FEATURE_PASSIVE_SUPPORT
        std::list<std::shared_ptr<LocationRequest>> passiveList;
        requests_->insert(make_pair(PASSIVE_ABILITY, passiveList));
#endif
    }
}

std::shared_ptr<std::map<std::string, std::list<std::shared_ptr<LocationRequest>>>> LocatorAbility::GetRequests()
{
    AutoLock lock(requestsMutex_);
    return requests_;
}

int LocatorAbility::GetActiveRequestNum()
{
    AutoLock lock(requestsMutex_);
    int num = 0;
#ifdef FEATURE_GNSS_SUPPORT
    auto gpsListIter = requests_->find(GNSS_ABILITY);
    if (gpsListIter != requests_->end()) {
        auto list = &(gpsListIter->second);
        num += static_cast<int>(list->size());
    }
#endif
#ifdef FEATURE_NETWORK_SUPPORT
    auto networkListIter = requests_->find(NETWORK_ABILITY);
    if (networkListIter != requests_->end()) {
        auto list = &(networkListIter->second);
        num += static_cast<int>(list->size());
    }
#endif
    LBSLOGI(LOCATOR, "ActiveRequestNum :%d", num);
    return num;
}

std::shared_ptr<std::map<ILocatorCallback *, std::list<std::shared_ptr<LocationRequest>>>>
LocatorAbility::GetReceivers()
{
    AutoLock lock(receiversMutex_);
    return receivers_;
}

void LocatorAbility::ApplyRequests(int delay)
{
    if (requestManager_ != nullptr) {
        requestManager_->HandleRequest();
    }
}

void LocatorAbility::UpdateLoadedAbilityMap()
{
    AutoLock lock(loadedAbilityMapMutex_);
    loadedAbilityMap_->clear();
#ifdef FEATURE_GNSS_SUPPORT
    GnssAbility *objectGnss = &GnssAbility::GetInstance();
    loadedAbilityMap_->insert(make_pair(GNSS_ABILITY, objectGnss));
#endif
}

LocationErrCode LocatorAbility::IsLocationEnabled(bool &isEnabled)
{
    MutexId loadedAbilityMapMutex_;
    isEnabled = isEnabled_;
    return ERRCODE_SUCCESS;
}

LocationErrCode LocatorAbility::EnableAbility(bool isEnabled)
{
    LBSLOGI(LOCATOR, "EnableAbility %d", isEnabled);
    if ((isEnabled_ && isEnabled) || (!isEnabled_ && !isEnabled)) {
        LBSLOGD(LOCATOR, "no need to Enable location ability, enable:%d", isEnabled);
        return ERRCODE_SUCCESS;
    }
    AutoLock lock(loadedAbilityMapMutex_);
    for (auto iter = loadedAbilityMap_->begin(); iter != loadedAbilityMap_->end(); iter++) {
        SubAbility *abilityObject = iter->second;
        int error = isEnabled ? abilityObject->EnableAbility() : abilityObject->DisableAbility();
        if (error != ERRCODE_SUCCESS) {
            LBSLOGE(LOCATOR, "Enable %s ability, result %d", (iter->first).c_str(), error);
            return ERRCODE_SERVICE_UNAVAILABLE;
        }
    }
    isEnabled_ = isEnabled;
    ApplyRequests(0);
    return ERRCODE_SUCCESS;
}

LocationErrCode LocatorAbility::IsLocationPrivacyConfirmed(const int type, bool &isConfirmed)
{
    return LocationConfigManager::GetInstance()->GetPrivacyTypeState(type, isConfirmed);
}

LocationErrCode LocatorAbility::SetLocationPrivacyConfirmStatus(const int type, bool isConfirmed)
{
    return LocationConfigManager::GetInstance()->SetPrivacyTypeState(type, isConfirmed);
}

#ifdef FEATURE_GNSS_SUPPORT
LocationErrCode LocatorAbility::RegisterGnssStatusCallback(IGnssStatusCallback *callback, pid_t uid)
{
    LBSLOGD(LOCATOR, "uid is: %d", uid);
    if (!isEnabled_) {
        LBSLOGE(LOCATOR, "Locator Ability is not Enable");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    return GnssAbility::GetInstance().RegisterGnssStatusCallback(callback, uid);
}
#endif

#ifdef FEATURE_GNSS_SUPPORT
LocationErrCode LocatorAbility::UnregisterGnssStatusCallback(IGnssStatusCallback *callback)
{
    if (!isEnabled_) {
        LBSLOGE(LOCATOR, "Locator Ability is not Enable");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    return GnssAbility::GetInstance().UnregisterGnssStatusCallback(callback);
}
#endif

#ifdef FEATURE_GNSS_SUPPORT
LocationErrCode LocatorAbility::RegisterNmeaMessageCallback(INmeaMessageCallback *callback, pid_t uid)
{
    if (!isEnabled_) {
        LBSLOGE(LOCATOR, "Locator Ability is not Enable");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    return GnssAbility::GetInstance().RegisterNmeaMessageCallback(callback, uid);
}
#endif

#ifdef FEATURE_GNSS_SUPPORT
LocationErrCode LocatorAbility::UnregisterNmeaMessageCallback(INmeaMessageCallback *callback)
{
    if (!isEnabled_) {
        LBSLOGE(LOCATOR, "Locator Ability is not Enable");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    return GnssAbility::GetInstance().UnregisterNmeaMessageCallback(callback);
}
#endif

#ifdef FEATURE_GNSS_SUPPORT
LocationErrCode LocatorAbility::SendCommand(const LocationCommand *commands)
{
    if (!isEnabled_) {
        LBSLOGE(LOCATOR, "Locator Ability is not Enable");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    return GnssAbility::GetInstance().SendCommand(commands);
}
#endif

LocationErrCode LocatorAbility::StartLocating(const RequestConfig *requestConfig, ILocatorCallback *callback,
                                              AppIdentity &identity)
{
#if !defined(FEATURE_GNSS_SUPPORT) && !defined(FEATURE_NETWORK_SUPPORT) && !defined(FEATURE_PASSIVE_SUPPORT)
    LBSLOGE(LOCATOR, "service unavailable");
    return ERRCODE_NOT_SUPPORTED;
#endif
    if (!isEnabled_) {
        LBSLOGE(LOCATOR, "Locator Ability is not Enable");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    // update offset before add request
    if (reportManager_ == nullptr || requestManager_ == nullptr) {
        StopAbility();
        LBSLOGE(LOCATOR, "Locator Ability reportManager_ or requestManager_ invalid");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    LocationErrCode errCode = StartAbility();
    if (errCode != ERRCODE_SUCCESS) {
        LBSLOGE(LOCATOR, "Locator Ability start failed");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    reportManager_->UpdateRandom();
    std::shared_ptr<LocationRequest> request = std::make_shared<LocationRequest>(requestConfig, callback, identity);

    if (NeedReportCacheLocation(request, callback)) {
        LBSLOGI(LOCATOR, "report cache location to %s", identity.GetBundleName().c_str());
    } else {
        HandleStartLocating(request, callback);
    }
    return ERRCODE_SUCCESS;
}

bool LocatorAbility::IsCacheVaildScenario(const RequestConfig *requestConfig)
{
    if (requestConfig->GetFixNumber() == 1 && requestConfig->GetPriority() != PRIORITY_ACCURACY &&
        ((requestConfig->GetScenario() == SCENE_DAILY_LIFE_SERVICE) ||
         ((requestConfig->GetScenario() == SCENE_UNSET) && (requestConfig->GetPriority() == PRIORITY_FAST_FIRST_FIX)) ||
         ((requestConfig->GetScenario() == SCENE_UNSET) && (requestConfig->GetPriority() == PRIORITY_LOW_POWER)))) {
        return true;
    }
    return false;
}

bool LocatorAbility::IsSingleRequest(const RequestConfig *requestConfig)
{
    if (requestConfig->GetFixNumber() == 1) {
        return true;
    }
    return false;
}

void LocatorAbility::UpdateLastLocationRequestNum()
{
    auto workRecordStatistic = WorkRecordStatistic::GetInstance();
    if (!workRecordStatistic->Update("CacheLocation", -1)) {
        LBSLOGE(LOCATOR, "workRecordStatistic::Update failed");
    }
}

void LocatorAbility::UpdatePermissionUsedRecord(uint32_t tokenId, std::string permissionName, int permUsedType,
                                                int succCnt, int failCnt)
{
#ifdef PERMISSION_SUPPORT
    Security::AccessToken::AddPermParamInfo info;
    info.tokenId = tokenId;
    info.permissionName = permissionName;
    info.successCount = succCnt;
    info.failCount = failCnt;
    info.type = static_cast<OHOS::Security::AccessToken::PermissionUsedType>(permUsedType);
    Security::AccessToken::PrivacyKit::AddPermissionUsedRecord(info);
#endif
}

bool LocatorAbility::NeedReportCacheLocation(const std::shared_ptr<LocationRequest> &request,
                                             ILocatorCallback *callback)
{
    if (reportManager_ == nullptr || request == nullptr) {
        return false;
    }
    // report cache location in single location request
    if (IsSingleRequest(request->GetRequestConfig()) && IsCacheVaildScenario(request->GetRequestConfig())) {
        auto cacheLocation = reportManager_->GetCacheLocation(request);
        if (cacheLocation != nullptr && callback != nullptr) {
            auto workRecordStatistic = WorkRecordStatistic::GetInstance();
            if (!workRecordStatistic->Update("CacheLocation", 1)) {
                LBSLOGE(LOCATOR, "workRecordStatistic::Update failed");
            }
#ifdef PERMISSION_SUPPORT
            PrivacyKit::StartUsingPermission(request->GetTokenId(), ACCESS_APPROXIMATELY_LOCATION);
            callback->OnLocationReport(cacheLocation);
            // add location permission using record
            UpdatePermissionUsedRecord(request->GetTokenId(), ACCESS_APPROXIMATELY_LOCATION, request->GetPermUsedType(),
                                       1, 0);
            PrivacyKit::StopUsingPermission(request->GetTokenId(), ACCESS_APPROXIMATELY_LOCATION);
#endif
            UpdateLastLocationRequestNum();
            return true;
        }
    } else if (!IsSingleRequest(request->GetRequestConfig()) && IsCacheVaildScenario(request->GetRequestConfig())) {
        auto cacheLocation = reportManager_->GetCacheLocation(request);
        if (cacheLocation != nullptr && callback != nullptr) {
            auto workRecordStatistic = WorkRecordStatistic::GetInstance();
            if (!workRecordStatistic->Update("CacheLocation", 1)) {
                LBSLOGE(LOCATOR, "workRecordStatistic::Update failed");
            }
            callback->OnLocationReport(cacheLocation);
            // add location permission using record
            UpdatePermissionUsedRecord(request->GetTokenId(), ACCESS_APPROXIMATELY_LOCATION, request->GetPermUsedType(),
                                       1, 0);
        }
    }
    return false;
}

void LocatorAbility::HandleStartLocating(const std::shared_ptr<LocationRequest> &request, ILocatorCallback *callback)
{
    if (requestManager_ != nullptr) {
        requestManager_->HandleStartLocating(request);
    }
    if (callback != nullptr) {
        ReportLocationStatus(callback, LOCATING_STARTED);
    }
}

LocationErrCode LocatorAbility::StopLocating(ILocatorCallback *callback)
{
#if !defined(FEATURE_GNSS_SUPPORT) && !defined(FEATURE_NETWORK_SUPPORT) && !defined(FEATURE_PASSIVE_SUPPORT)
    LBSLOGE(LOCATOR, "service unavailable");
    return ERRCODE_NOT_SUPPORTED;
#endif
    if (requestManager_ == nullptr) {
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    LocationErrCode errCode = StopAbility();
    if (errCode != ERRCODE_SUCCESS) {
        LBSLOGE(LOCATOR, "Locator Ability stop failed");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    requestManager_->HandleStopLocating(callback);
    if (callback != nullptr) {
        ReportLocationStatus(callback, LOCATING_STOPED);
    }
    return ERRCODE_SUCCESS;
}

bool LocatorAbility::CheckIsReportPermitted(AppIdentity &identity)
{
    AutoLock lock(requestsMutex_);
    if (requests_ == nullptr || requests_->empty()) {
        LBSLOGE(LOCATOR, "requests map is empty");
        return false;
    }

    bool isPermitted = true;
    for (auto mapIter = requests_->begin(); mapIter != requests_->end(); mapIter++) {
        auto list = mapIter->second;
        for (auto request : list) {
            if (request == nullptr || request->GetTokenId() != identity.GetTokenId()) {
                continue;
            }
            auto locationErrorCallback = request->GetLocationErrorCallBack();
            if (locationErrorCallback != nullptr) {
                if (!isEnabled_) {
                    LBSLOGE(LOCATOR, "location switch is off");
                    isPermitted = false;
                    locationErrorCallback->OnErrorReport(LOCATING_FAILED_LOCATION_SWITCH_OFF);
                    continue;
                }
#ifdef PERMISSION_SUPPORT
                std::string bundleName = "";
                auto tokenId = request->GetTokenId();
                auto firstTokenId = request->GetFirstTokenId();
                auto tokenIdEx = request->GetTokenIdEx();
                auto uid = request->GetUid();
                if (!CommonUtils::GetBundleNameByUid(uid, bundleName)) {
                    LBSLOGE(LOCATOR, "Fail to Get bundle name: uid = %d.", uid);
                }
                if (reportManager_->IsAppBackground(bundleName, tokenId, tokenIdEx, uid) &&
                    !PermissionManager::CheckBackgroundPermission(tokenId, firstTokenId)) {
                    isPermitted = false;
                    // app background, no background permission, not ContinuousTasks
                    locationErrorCallback->OnErrorReport(LOCATING_FAILED_BACKGROUND_PERMISSION_DENIED);
                    continue;
                }
                if (!PermissionManager::CheckLocationPermission(tokenId, firstTokenId) &&
                    !PermissionManager::CheckApproximatelyPermission(tokenId, firstTokenId)) {
                    LBSLOGE(LOCATOR, "%d has no location permission failed", tokenId);
                    isPermitted = false;
                    locationErrorCallback->OnErrorReport(LOCATING_FAILED_LOCATION_PERMISSION_DENIED);
                    continue;
                }
#endif
            }
        }
    }
    return isPermitted;
}

LocationErrCode LocatorAbility::ReportLocation(const std::shared_ptr<Location> &location, std::string abilityName,
                                               AppIdentity &identity)
{
    if (requests_ == nullptr) {
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    if (!CheckIsReportPermitted(identity)) {
        LBSLOGE(LOCATOR, "report is not allowed");
        return ERRCODE_NOT_SUPPORTED;
    }
    if (reportManager_ != nullptr) {
        std::string abilityName = GNSS_ABILITY;
        int64_t time = location->GetTimeStamp();
        int64_t timeSinceBoot = location->GetTimeSinceBoot();
        double acc = location->GetAccuracy();
        LBSLOGI(LOCATOR, "receive location: [%s time=%s timeSinceBoot=%s acc=%f]", abilityName.c_str(),
                std::to_string(time).c_str(), std::to_string(timeSinceBoot).c_str(), acc);
        reportManager_->OnReportLocation(location, abilityName);
    }
    LBSLOGI(LOCATOR, "report Location is finish");
    return ERRCODE_SERVICE_UNAVAILABLE;
}

LocationErrCode LocatorAbility::ReportLocationStatus(ILocatorCallback *callback, int result)
{
    if (!isEnabled_) {
        LBSLOGE(LOCATOR, "location switch is off");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    if (reportManager_ != nullptr &&
        reportManager_->ReportRemoteCallback(callback, ILocatorCallback::RECEIVE_LOCATION_STATUS_EVENT, result)) {
        return ERRCODE_SUCCESS;
    }
    return ERRCODE_SERVICE_UNAVAILABLE;
}

LocationErrCode LocatorAbility::ReportErrorStatus(ILocatorCallback *callback, int result)
{
    if (!isEnabled_) {
        LBSLOGE(LOCATOR, "location switch is off");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    if (reportManager_ != nullptr &&
        reportManager_->ReportRemoteCallback(callback, ILocatorCallback::RECEIVE_ERROR_INFO_EVENT, result)) {
        return ERRCODE_SUCCESS;
    }
    return ERRCODE_SERVICE_UNAVAILABLE;
}

#ifdef PERMISSION_SUPPORT
void LocatorAbility::RegisterPermissionCallback(const uint32_t callingTokenId,
                                                const std::vector<std::string> &permissionNameList)
{
    AutoLock lock(permissionMapMutex_);
    if (permissionMap_ == nullptr) {
        LBSLOGE(LOCATOR, "permissionMap is null.");
        return;
    }
    PermStateChangeScope scopeInfo;
    scopeInfo.permList = permissionNameList;
    scopeInfo.tokenIDs = {callingTokenId};
    auto callbackPtr = std::make_shared<PermissionStatusChangeCb>(scopeInfo);
    permissionMap_->erase(callingTokenId);
    permissionMap_->insert(std::make_pair(callingTokenId, callbackPtr));
    LBSLOGD(LOCATOR, "after tokenId:%d register, permission callback size:%s", callingTokenId,
            std::to_string(permissionMap_->size()).c_str());
    int32_t res = AccessTokenKit::RegisterPermStateChangeCallback(callbackPtr);
    if (res != SUCCESS) {
        LBSLOGE(LOCATOR, "RegisterPermStateChangeCallback failed.");
    }
}

void LocatorAbility::UnregisterPermissionCallback(const uint32_t callingTokenId)
{
    AutoLock lock(permissionMapMutex_);
    if (permissionMap_ == nullptr) {
        LBSLOGE(LOCATOR, "permissionMap is null.");
        return;
    }
    auto iter = permissionMap_->find(callingTokenId);
    if (iter != permissionMap_->end()) {
        auto callbackPtr = iter->second;
        int32_t res = AccessTokenKit::UnRegisterPermStateChangeCallback(callbackPtr);
        if (res != SUCCESS) {
            LBSLOGE(LOCATOR, "UnRegisterPermStateChangeCallback failed.");
        }
    }
    permissionMap_->erase(callingTokenId);
    LBSLOGD(LOCATOR, "after tokenId:%d unregister, permission callback size:%s", callingTokenId,
            std::to_string(permissionMap_->size()).c_str());
}
#endif

#ifdef FEATURE_GNSS_SUPPORT
LocationErrCode LocatorAbility::QuerySupportCoordinateSystemType(
    std::vector<LocationCoordinateSystemType> &coordinateSystemTypes)
{
    return GnssAbility::GetInstance().QuerySupportCoordinateSystemType(coordinateSystemTypes);
}

LocationErrCode LocatorAbility::SendNetworkLocation(const std::shared_ptr<Location> &location)
{
    LBSLOGI(LOCATOR, "send network location");
    int64_t time = location->GetTimeStamp();
    int64_t timeSinceBoot = location->GetTimeSinceBoot();
    double acc = location->GetAccuracy();
    LBSLOGI(LOCATOR, "receive network location: [ time=%s timeSinceBoot=%s acc=%f]", std::to_string(time).c_str(),
            std::to_string(timeSinceBoot).c_str(), acc);
    return GnssAbility::GetInstance().SendNetworkLocation(location);
}
#endif

LocationErrCode LocatorAbility::RegisterLocationError(ILocatorCallback *callback, AppIdentity &identity)
{
    if (requestManager_ != nullptr) {
        requestManager_->UpdateLocationErrorCallbackToRequest(callback, identity.GetTokenId(), true);
    }
    return ERRCODE_SUCCESS;
}

LocationErrCode LocatorAbility::UnregisterLocationError(ILocatorCallback *callback, AppIdentity &identity)
{
    if (requestManager_ != nullptr) {
        requestManager_->UpdateLocationErrorCallbackToRequest(callback, identity.GetTokenId(), false);
    }
    return ERRCODE_SUCCESS;
}

void LocatorAbility::ReportLocationError(std::string uuid, int32_t errCode)
{
    if (requests_->empty()) {
        LBSLOGE(REQUEST_MANAGER, "requests map is empty");
        return;
    }
    for (auto mapIter = requests_->begin(); mapIter != requests_->end(); mapIter++) {
        auto list = mapIter->second;
        for (auto request : list) {
            if (uuid != "" && uuid != request->GetUuid()) {
                continue;
            } else if (uuid != "") {
                auto locationCallbackHost = request->GetLocatorCallBack();
                locationCallbackHost->OnErrorReport(errCode);
            }
            auto locationErrorCallbackHost = request->GetLocationErrorCallBack();
            if (locationErrorCallbackHost != nullptr) {
                LBSLOGE(LOCATOR, "errCode : %d ,uuid : %s", errCode, uuid.c_str());
                locationErrorCallbackHost->OnErrorReport(errCode);
            }
        }
    }
}

LocationErrCode LocatorAbility::RemoveInvalidRequests()
{
    std::list<std::shared_ptr<LocationRequest>> invalidRequestList;
    int32_t requestNum = 0;
    int32_t invalidRequestNum = 0;
    {
        AutoLock lock(requestsMutex_);
#ifdef FEATURE_GNSS_SUPPORT
        auto gpsListIter = requests_->find(GNSS_ABILITY);
        if (gpsListIter != requests_->end()) {
            auto list = &(gpsListIter->second);
            requestNum += static_cast<int>(list->size());
            for (auto &item : *list) {
                if (IsInvalidRequest(item)) {
                    invalidRequestList.push_back(item);
                    invalidRequestNum++;
                }
            }
        }
#endif
#ifdef FEATURE_NETWORK_SUPPORT
        auto networkListIter = requests_->find(NETWORK_ABILITY);
        if (networkListIter != requests_->end()) {
            auto list = &(networkListIter->second);
            requestNum += static_cast<int>(list->size());
            for (auto &item : *list) {
                if (IsInvalidRequest(item)) {
                    invalidRequestList.push_back(item);
                    invalidRequestNum++;
                }
            }
        }
#endif
    }
    LBSLOGI(LOCATOR, "request num : %d, invalid request num: %d", requestNum, invalidRequestNum);
    for (auto &item : invalidRequestList) {
        ILocatorCallback *callback = item->GetLocatorCallBack();
        StopLocating(callback);
    }
    return ERRCODE_SUCCESS;
}

bool LocatorAbility::IsInvalidRequest(std::shared_ptr<LocationRequest> &request)
{
    LBSLOGI(LOCATOR, "request : %s %s", request->GetPackageName().c_str(),
            request->GetRequestConfig()->ToString().c_str());
    int64_t timeDiff = fabs(CommonUtils::GetCurrentTime() - request->GetRequestConfig()->GetTimeStamp());
    if (request->GetRequestConfig()->GetFixNumber() == 1 &&
        timeDiff > (request->GetRequestConfig()->GetTimeOut() / MILLI_PER_SEC)) {
        LBSLOGI(LOCATOR, "once request is timeout");
        return true;
    }

    if (timeDiff > REQUEST_DEFAULT_TIMEOUT_SECOUND) {
        LBSLOGI(LOCATOR, "request process is not running");
        return true;
    }
    return false;
}

}  // namespace Location
}  // namespace OHOS
