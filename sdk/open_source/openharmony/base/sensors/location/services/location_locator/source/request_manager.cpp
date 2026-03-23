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
#include "request_manager.h"
#ifdef PERMISSION_SUPPORT
#include "privacy_kit.h"
#endif
#include "common_utils.h"
#include "constant_definition.h"

#include "fusion_controller.h"
#include "location_log.h"
#include "locator_ability.h"
#include "gnss_ability.h"
#include "locator_event_manager.h"

#include "request_config.h"
#include "permission_manager.h"

namespace OHOS {
namespace Location {

constexpr size_t LBS_REQUEST_MAX_SIZE = 20;

MutexId RequestManager::requestMutex_;

RequestManager *RequestManager::GetInstance()
{
    static RequestManager data;
    return &data;
}

RequestManager::RequestManager()
{
    MutexAttr attr = {false};
    requestMutex_ = MutexCreate(&attr);
    runningUidsMutex_ = MutexCreate(&attr);
    permissionRecordMutex_ = MutexCreate(&attr);
    isDeviceIdleMode_.store(false);
    isDeviceStillState_.store(false);
    auto locatorDftManager = LocatorDftManager::GetInstance();
    if (locatorDftManager != nullptr) {
        locatorDftManager->Init();
    }
}

RequestManager::~RequestManager()
{
    MutexDestroy(&requestMutex_);
    MutexDestroy(&runningUidsMutex_);
    MutexDestroy(&permissionRecordMutex_);
}

void RequestManager::UpdateUsingPermission(std::shared_ptr<LocationRequest> request, const bool isStart)
{
    AutoLock lock(permissionRecordMutex_);
    if (request == nullptr) {
        LBSLOGE(REQUEST_MANAGER, "request is null");
        return;
    }
    UpdateUsingApproximatelyPermission(request, isStart);
}

void RequestManager::UpdateUsingApproximatelyPermission(std::shared_ptr<LocationRequest> request, const bool isStart)
{
#ifdef PERMISSION_SUPPORT
    uint32_t callingTokenId = request->GetTokenId();
    if (isStart && !request->GetApproximatelyPermState()) {
        PrivacyKit::StartUsingPermission(callingTokenId, ACCESS_APPROXIMATELY_LOCATION);
        LocatorAbility::GetInstance().UpdatePermissionUsedRecord(request->GetTokenId(), ACCESS_APPROXIMATELY_LOCATION,
                                                                 request->GetPermUsedType(), 1, 0);
        request->SetApproximatelyPermState(true);
    } else if (!isStart && request->GetApproximatelyPermState()) {
        PrivacyKit::StopUsingPermission(callingTokenId, ACCESS_APPROXIMATELY_LOCATION);
        request->SetApproximatelyPermState(false);
    }
#endif
}

void RequestManager::HandleStartLocating(std::shared_ptr<LocationRequest> request)
{
    auto locatorDftManager = LocatorDftManager::GetInstance();
    if (locatorDftManager == nullptr) {
        return;
    }
    // restore request to all request list
    bool isNewRequest = RestorRequest(request);
    // update request map
    if (isNewRequest) {
#ifdef PERMISSION_SUPPORT
        LocatorAbility::GetInstance().RegisterPermissionCallback(
            request->GetTokenId(), {ACCESS_APPROXIMATELY_LOCATION, ACCESS_LOCATION, ACCESS_BACKGROUND_LOCATION});
#endif
        UpdateRequestRecord(request, true);
        locatorDftManager->LocationSessionStart(request);
    }
    // process location request
    HandleRequest();
}

bool RequestManager::RestorRequest(std::shared_ptr<LocationRequest> newRequest)
{
    AutoLock lock(requestMutex_);
    auto receivers = LocatorAbility::GetInstance().GetReceivers();
    if (receivers == nullptr) {
        LBSLOGE(REQUEST_MANAGER, "receivers is empty");
        return false;
    }
    if (newRequest == nullptr) {
        LBSLOGE(REQUEST_MANAGER, "newRequest is empty");
        return false;
    }
    newRequest->SetRequesting(true);
    ILocatorCallback *newCallback = newRequest->GetLocatorCallBack();

    // if callback and request config type is same, take new request configuration over the old one in request list
    // otherwise, add restore the new request in the list.
    auto iterator = receivers->find(newCallback);
    if (iterator == receivers->end()) {
        std::list<std::shared_ptr<LocationRequest>> requestList;
        requestList.push_back(newRequest);
        receivers->insert(make_pair(newCallback, requestList));
        LBSLOGD(REQUEST_MANAGER, "add new receiver with new callback");
        return true;
    }

    RequestConfig *newConfig = newRequest->GetRequestConfig();
    std::list<std::shared_ptr<LocationRequest>> requestWithSameCallback = iterator->second;
    for (auto iter = requestWithSameCallback.begin(); iter != requestWithSameCallback.end(); ++iter) {
        auto request = *iter;
        if (request == nullptr) {
            continue;
        }
        auto requestConfig = request->GetRequestConfig();
        if (requestConfig == nullptr || newConfig == nullptr) {
            continue;
        }
        if (newConfig->IsSame(*requestConfig)) {
            request->SetRequestConfig(*newConfig);
            LBSLOGI(REQUEST_MANAGER, "find same type request, update request configuration");
            return false;
        }
    }
    requestWithSameCallback.push_back(newRequest);
    LBSLOGD(REQUEST_MANAGER, "add new receiver with old callback");
    return true;
}

void RequestManager::UpdateRequestRecord(std::shared_ptr<LocationRequest> request, bool shouldInsert)
{
    std::shared_ptr<std::list<std::string>> proxys = std::make_shared<std::list<std::string>>();
    request->GetProxyName(proxys);
    if (proxys->empty()) {
        LBSLOGE(REQUEST_MANAGER, "can not get proxy name according to request configuration");
        return;
    }

    for (std::list<std::string>::iterator iter = proxys->begin(); iter != proxys->end(); ++iter) {
        std::string abilityName = *iter;
        UpdateRequestRecord(request, abilityName, shouldInsert);
    }
}

void RequestManager::UpdateRequestRecord(std::shared_ptr<LocationRequest> request, std::string abilityName,
                                         bool shouldInsert)
{
    AutoLock lock(requestMutex_);
    auto requests = LocatorAbility::GetInstance().GetRequests();
    if (requests == nullptr) {
        LBSLOGE(REQUEST_MANAGER, "requests map is empty");
        return;
    }
    auto mapIter = requests->find(abilityName);
    if (mapIter == requests->end()) {
        LBSLOGE(REQUEST_MANAGER, "can not find %s ability request list.", abilityName.c_str());
        return;
    }

    auto list = &(mapIter->second);
    LBSLOGD(REQUEST_MANAGER, "%s ability current request size %s", abilityName.c_str(),
            std::to_string(list->size()).c_str());
    if (shouldInsert) {
        list->push_back(request);
        HandleChrEvent(*list);
        UpdateRunningUids(request, abilityName, true);
    } else {
        for (auto iter = list->begin(); iter != list->end();) {
            auto findRequest = *iter;
            if (request == findRequest) {
                iter = list->erase(iter);
                UpdateRunningUids(findRequest, abilityName, false);
                LBSLOGD(REQUEST_MANAGER, "find request");
            } else {
                ++iter;
            }
        }
    }
    LBSLOGD(REQUEST_MANAGER, "%s ability request size %s", abilityName.c_str(), std::to_string(list->size()).c_str());
}

void RequestManager::HandleChrEvent(std::list<std::shared_ptr<LocationRequest>> requests)
{
    if (requests.size() > LBS_REQUEST_MAX_SIZE) {
        std::vector<std::string> names;
        std::vector<std::string> values;
        int index = 0;
        for (auto it = requests.begin(); it != requests.end(); ++it, ++index) {
            auto request = *it;
            if (request == nullptr) {
                continue;
            }
            names.push_back(std::to_string(index));
            std::string packageName = request->GetPackageName();
            values.push_back(packageName);
        }
    }
}

void RequestManager::HandleStopLocating(ILocatorCallback *callback)
{
    if (callback == nullptr) {
        LBSLOGE(REQUEST_MANAGER, "stop locating but callback is null");
        return;
    }
    AutoLock lock(requestMutex_);
    auto receivers = LocatorAbility::GetInstance().GetReceivers();
    if (receivers == nullptr) {
        LBSLOGE(REQUEST_MANAGER, "receivers map is empty");
        return;
    }
    // get dead request list
    LBSLOGD(REQUEST_MANAGER, "stop callback");
    auto iterator = receivers->find(callback);
    if (iterator == receivers->end()) {
        LBSLOGD(REQUEST_MANAGER, "this callback has no record in receiver map");
        return;
    }

    auto requests = iterator->second;
    auto deadRequests = std::make_shared<std::list<std::shared_ptr<LocationRequest>>>();
    for (auto iter = requests.begin(); iter != requests.end(); ++iter) {
        auto request = *iter;
#ifdef PERMISSION_SUPPORT
        LocatorAbility::GetInstance().UnregisterPermissionCallback(request->GetTokenId());
#endif
        deadRequests->push_back(request);
        LBSLOGI(REQUEST_MANAGER, "remove request:%s", request->ToString().c_str());
    }
    LBSLOGD(REQUEST_MANAGER, "get %s dead request", std::to_string(deadRequests->size()).c_str());
    // update request map
    if (deadRequests->size() == 0) {
        return;
    }
    iterator->second.clear();
    receivers->erase(iterator);
    DeleteRequestRecord(deadRequests);
    deadRequests->clear();
    // process location request
    HandleRequest();
}

void RequestManager::DeleteRequestRecord(std::shared_ptr<std::list<std::shared_ptr<LocationRequest>>> requests)
{
    for (auto iter = requests->begin(); iter != requests->end(); ++iter) {
        auto request = *iter;
        UpdateRequestRecord(request, false);
        UpdateUsingPermission(request, false);
    }
}

void RequestManager::HandleRequest()
{
    AutoLock lock(requestMutex_);
    auto requests = LocatorAbility::GetInstance().GetRequests();
    if (requests == nullptr) {
        LBSLOGE(REQUEST_MANAGER, "requests map is empty");
        return;
    }
    std::map<std::string, std::list<std::shared_ptr<LocationRequest>>>::iterator iter;
    for (iter = requests->begin(); iter != requests->end(); ++iter) {
        std::string abilityName = iter->first;
        std::list<std::shared_ptr<LocationRequest>> requestList = iter->second;
        HandleRequest(abilityName, requestList);
    }
}

void RequestManager::HandleRequest(std::string abilityName, std::list<std::shared_ptr<LocationRequest>> list)
{
    // generate work record, and calculate interval
    std::shared_ptr<WorkRecord> workRecord = std::make_shared<WorkRecord>();
    for (auto iter = list.begin(); iter != list.end(); iter++) {
        auto request = *iter;
        if (!AddRequestToWorkRecord(abilityName, request, workRecord)) {
            LBSLOGD(REQUEST_MANAGER, "remove pid:%d uid:%d %s", request->GetPid(), request->GetUid(),
                    request->GetPackageName().c_str());
            UpdateUsingPermission(request, false);
            continue;
        }
        UpdateUsingPermission(request, true);
        if (!ActiveLocatingStrategies(request)) {
            continue;
        }
        LBSLOGD(REQUEST_MANAGER, "add pid:%d uid:%d %s", request->GetPid(), request->GetUid(),
                request->GetPackageName().c_str());
    }
    LBSLOGD(REQUEST_MANAGER, "detect %s ability requests(size:%s) work record:%s", abilityName.c_str(),
            std::to_string(list.size()).c_str(), workRecord->ToString().c_str());

    ProxySendLocationRequest(abilityName, *workRecord);
}

bool RequestManager::ActiveLocatingStrategies(const std::shared_ptr<LocationRequest> &request)
{
    if (request == nullptr) {
        return false;
    }
    auto requestConfig = request->GetRequestConfig();
    if (requestConfig == nullptr) {
        return false;
    }
    int requestType = requestConfig->GetScenario();
    if (requestType == SCENE_UNSET) {
        requestType = requestConfig->GetPriority();
    }
    auto fusionController = FusionController::GetInstance();
    if (fusionController != nullptr) {
        fusionController->ActiveFusionStrategies(requestType);
    }
    return true;
}

/**
 * determine whether the request is valid.
 */
bool RequestManager::IsRequestAvailable(std::shared_ptr<LocationRequest> &request)
{
    if (!request->GetIsRequesting()) {
        return false;
    }
    // for once_request app, if it has timed out, do not add to workRecord
    int64_t curTime = CommonUtils::GetCurrentTime();
    if (request->GetRequestConfig()->GetFixNumber() == 1 &&
        fabs(curTime - request->GetRequestConfig()->GetTimeStamp()) >
        (request->GetRequestConfig()->GetTimeOut() / MILLI_PER_SEC)) {
        LBSLOGE(LOCATOR, "%d has timed out.", request->GetPid());
        return false;
    }
    return true;
}

bool RequestManager::AddRequestToWorkRecord(std::string abilityName, std::shared_ptr<LocationRequest> &request,
                                            std::shared_ptr<WorkRecord> &workRecord)
{
    if (request == nullptr) {
        return false;
    }
    if (!IsRequestAvailable(request)) {
        return false;
    }
    auto locationErrorCallback = request->GetLocationErrorCallBack();
    bool isEnabled = false;
    if (LocatorAbility::GetInstance().IsLocationEnabled(isEnabled) == ERRCODE_SUCCESS) {
        if (!isEnabled) {
            if (locationErrorCallback != nullptr) {
                locationErrorCallback->OnErrorReport(LOCATING_FAILED_LOCATION_SWITCH_OFF);
            }
            LBSLOGE(LOCATOR, "the location switch is off");
            return false;
        }
    }

    uint32_t tokenId = request->GetTokenId();
    uint32_t firstTokenId = request->GetFirstTokenId();
#ifdef PERMISSION_SUPPORT
    // if location access permission granted, add request info to work record
    if (!PermissionManager::CheckLocationPermission(tokenId, firstTokenId) &&
        !PermissionManager::CheckApproximatelyPermission(tokenId, firstTokenId)) {
        if (locationErrorCallback != nullptr) {
            locationErrorCallback->OnErrorReport(LOCATING_FAILED_LOCATION_PERMISSION_DENIED);
        }
        LBSLOGI(LOCATOR, "CheckLocationPermission return false, tokenId=%d", tokenId);
        return false;
    }
    std::string bundleName = "";
    pid_t uid = request->GetUid();
    if (!CommonUtils::GetBundleNameByUid(uid, bundleName)) {
        LBSLOGD(REPORT_MANAGER, "Fail to Get bundle name: uid = %d.", uid);
    }
    auto reportManager = ReportManager::GetInstance();
    if (reportManager != nullptr) {
        if (reportManager->IsAppBackground(bundleName, tokenId, request->GetTokenIdEx(), uid) &&
            !PermissionManager::CheckBackgroundPermission(tokenId, firstTokenId)) {
            if (locationErrorCallback != nullptr) {
                locationErrorCallback->OnErrorReport(LOCATING_FAILED_BACKGROUND_PERMISSION_DENIED);
            }
            LBSLOGE(REPORT_MANAGER, "CheckBackgroundPermission return false, tokenId=%d", tokenId);
            return false;
        }
    }
#endif
    auto requestConfig = request->GetRequestConfig();
    if (requestConfig == nullptr) {
        return false;
    }
#ifdef PERMISSION_SUPPORT
    if (!PermissionManager::CheckSystemPermission(tokenId, request->GetTokenIdEx()) &&
        !CommonUtils::CheckAppForUser(uid)) {
        LBSLOGD(REPORT_MANAGER, "AddRequestToWorkRecord uid: %d ,CheckAppIsCurrentUser fail", uid);
        return false;
    }
#endif
    // add request info to work record
    if (workRecord != nullptr) {
        request->SetNlpRequestType();
        workRecord->Add(request);
    }
    return true;
}

void RequestManager::ProxySendLocationRequest(std::string abilityName, WorkRecord &workRecord)
{
    LBSLOGI(LOCATOR, "%s workRecord uid_ size %d", abilityName.c_str(), workRecord.Size());
    if (abilityName == GNSS_ABILITY) {
#ifdef FEATURE_GNSS_SUPPORT
        GnssAbility::GetInstance().SendLocationRequest(workRecord);
#endif
    } else if (abilityName == NETWORK_ABILITY) {
#ifdef FEATURE_NETWORK_SUPPORT
        std::unique_ptr<NetworkAbilityProxy> networkProxy = std::make_unique<NetworkAbilityProxy>(remoteObject);
        networkProxy->SendLocationRequest(workRecord);
#endif
    } else if (abilityName == PASSIVE_ABILITY) {
#ifdef FEATURE_PASSIVE_SUPPORT
        std::unique_ptr<PassiveAbilityProxy> passiveProxy = std::make_unique<PassiveAbilityProxy>(remoteObject);
        passiveProxy->SendLocationRequest(workRecord);
#endif
    }
}

void RequestManager::HandlePowerSuspendChanged(int32_t pid, int32_t uid, int32_t state)
{
#ifdef APP_STATE_SUPPORT
    if (!IsUidInProcessing(uid)) {
        LBSLOGD(REQUEST_MANAGER, "Current uid : %d is not locating.", uid);
        return;
    }
    auto requests = LocatorAbility::GetInstance().GetRequests();
    if (requests == nullptr || requests->empty()) {
        LBSLOGE(REQUEST_MANAGER, "requests map is empty");
        return;
    }
    bool isActive = (state == static_cast<int>(AppExecFwk::ApplicationState::APP_STATE_FOREGROUND));
    for (auto mapIter = requests->begin(); mapIter != requests->end(); mapIter++) {
        auto list = mapIter->second;
        for (auto request : list) {
            std::string uid1 = std::to_string(request->GetUid());
            std::string uid2 = std::to_string(uid);
            std::string pid1 = std::to_string(request->GetPid());
            std::string pid2 = std::to_string(pid);
            if ((uid1.compare(uid2) != 0) || (pid1.compare(pid2) != 0)) {
                continue;
            }
            auto locatorBackgroundProxy = LocatorBackgroundProxy::GetInstance();
            if (locatorBackgroundProxy != nullptr) {
                locatorBackgroundProxy->OnSuspend(request, isActive);
            }
        }
    }
    if (LocatorAbility::GetInstance() != nullptr) {
        LocatorAbility::GetInstance()->ApplyRequests(1);
    }
#endif
}

void RequestManager::HandlePermissionChanged(uint32_t tokenId)
{
#ifdef PERMISSION_SUPPORT
    auto requests = LocatorAbility::GetInstance().GetRequests();
    if (requests == nullptr || requests->empty()) {
        LBSLOGE(REQUEST_MANAGER, "HandlePermissionChanged requests map is empty");
        return;
    }
    for (auto mapIter = requests->begin(); mapIter != requests->end(); mapIter++) {
        auto list = mapIter->second;
        for (auto request : list) {
            if (request == nullptr || tokenId != request->GetTokenId()) {
                continue;
            }
            auto backgroundProxy = LocatorBackgroundProxy::GetInstance();
            if (backgroundProxy != nullptr) {
                backgroundProxy->UpdateListOnRequestChange(request);
            }
        }
    }
#endif
}

bool RequestManager::IsUidInProcessing(int32_t uid)
{
    AutoLock lock(runningUidsMutex_);
    auto iter = runningUidMap_.find(uid);
    if (iter == runningUidMap_.end()) {
        return false;
    }
    return true;
}

void RequestManager::UpdateRunningUids(const std::shared_ptr<LocationRequest> &request, std::string abilityName,
                                       bool isAdd)
{
    AutoLock lock(runningUidsMutex_);
    auto uid = request->GetUid();
    auto pid = request->GetPid();
    int32_t uidCount = 0;
    auto iter = runningUidMap_.find(uid);
    if (iter != runningUidMap_.end()) {
        uidCount = iter->second;
        runningUidMap_.erase(uid);
    }
    if (isAdd) {
        auto requestConfig = request->GetRequestConfig();
        LBSLOGI(
            REQUEST_MANAGER,
            "PackageName:%s abilityName:%s requestAddress:%s scenario:%d priority:%d timeInterval:%d maxAccuracy:%d",
            request->GetPackageName().c_str(), abilityName.c_str(), request->GetUuid().c_str(),
            requestConfig->GetScenario(), requestConfig->GetPriority(), requestConfig->GetTimeInterval(),
            requestConfig->GetMaxAccuracy());
        uidCount += 1;
        if (uidCount == 1) {
            LBSLOGI(REQUEST_MANAGER, "AppLocatingState start pid:%d Uuid:%s", request->GetPid(),
                    request->GetUuid().c_str());
            ReportDataToResSched("start", uid);
        }
    } else {
        LBSLOGI(REQUEST_MANAGER, "PackageName:%s abilityName:%s requestAddress:%s", request->GetPackageName().c_str(),
                abilityName.c_str(), request->GetUuid().c_str());
        uidCount -= 1;
        if (uidCount == 0) {
            LBSLOGI(REQUEST_MANAGER, "AppLocatingState stop pid:%d Uuid:%s", request->GetPid(),
                    request->GetUuid().c_str());
            ReportDataToResSched("stop", uid);
        }
    }
    if (uidCount > 0) {
        runningUidMap_.insert(std::make_pair(uid, uidCount));
    }
}

void RequestManager::ReportDataToResSched(std::string state, const pid_t uid)
{
#ifdef RES_SCHED_SUPPROT
    std::unordered_map<std::string, std::string> payload;
    payload["uid"] = std::to_string(uid);
    payload["state"] = state;
    uint32_t type = ResourceSchedule::ResType::RES_TYPE_LOCATION_STATUS_CHANGE;
    int64_t value = ResourceSchedule::ResType::LocationStatus::APP_LOCATION_STATUE_CHANGE;
    ResourceSchedule::ResSchedClient::GetInstance().ReportData(type, value, payload);
#endif
}

void RequestManager::UpdateLocationErrorCallbackToRequest(ILocatorCallback *callback, uint32_t tokenId, bool state)
{
    auto requests = LocatorAbility::GetInstance().GetRequests();
    if (requests == nullptr || requests->empty()) {
        LBSLOGE(REQUEST_MANAGER, "requests map is empty");
        return;
    }
    for (auto mapIter = requests->begin(); mapIter != requests->end(); mapIter++) {
        auto list = mapIter->second;
        for (auto request : list) {
            if (request == nullptr || tokenId != request->GetTokenId()) {
                continue;
            }
            if (state) {
                request->SetLocationErrorCallBack(callback);
            } else {
                request->SetLocationErrorCallBack(nullptr);
            }
        }
    }
}

}  // namespace Location
}  // namespace OHOS
