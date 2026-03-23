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

#include "location_request.h"
#include "common_utils.h"
#include "constant_definition.h"

namespace OHOS {
namespace Location {
LocationRequest::LocationRequest()
{
    pid_ = -1;
    uid_ = -1;
    tokenId_ = 0;
    tokenIdEx_ = 0;
    firstTokenId_ = 0;
    packageName_ = "";
    isRequesting_ = false;
    permUsedType_ = 0;
    requestConfig_ = new (std::nothrow) RequestConfig();
    isUsingLocationPerm_ = false;
    isUsingBackgroundPerm_ = false;
    isUsingApproximatelyPerm_ = false;
    nlpRequestType_ = 0;
}

LocationRequest::LocationRequest(const RequestConfig *requestConfig, ILocatorCallback *callback,
    AppIdentity &identity)
{
    pid_ = -1;
    uid_ = -1;
    tokenId_ = 0;
    firstTokenId_ = 0;
    packageName_ = "";
    isRequesting_ = false;
    permUsedType_ = 0;
    requestConfig_ = new (std::nothrow) RequestConfig();
    isUsingLocationPerm_ = false;
    isUsingBackgroundPerm_ = false;
    isUsingApproximatelyPerm_ = false;
    nlpRequestType_ = 0;
    SetUid(identity.GetUid());
    SetPid(identity.GetPid());
    SetTokenId(identity.GetTokenId());
    SetTokenIdEx(identity.GetTokenIdEx());
    SetFirstTokenId(identity.GetFirstTokenId());
    SetPackageName(identity.GetBundleName());
    SetRequestConfig(*requestConfig);
    requestConfig_->SetTimeStamp(CommonUtils::GetCurrentTime());
    SetLocatorCallBack(callback);
    SetUuid(CommonUtils::GenerateUuid());
}


LocationRequest::~LocationRequest() {
    if (requestConfig_) {
        delete requestConfig_;
        requestConfig_ = nullptr;
    }
}

void LocationRequest::SetRequestConfig(const RequestConfig& requestConfig)
{
    if (requestConfig_ == nullptr) {
        return;
    }
    requestConfig_->Set(requestConfig);
}

void LocationRequest::SetLocatorCallBack(ILocatorCallback *callback)
{
    callBack_ = callback;
}

RequestConfig *LocationRequest::GetRequestConfig()
{
    return requestConfig_;
}

ILocatorCallback *LocationRequest::GetLocatorCallBack()
{
    return callBack_;
}

void LocationRequest::SetUid(pid_t uid)
{
    uid_ = uid;
}

pid_t LocationRequest::GetUid()
{
    return uid_;
}

void LocationRequest::SetPid(pid_t pid)
{
    pid_ = pid;
}

pid_t LocationRequest::GetPid()
{
    return pid_;
}

void LocationRequest::SetTokenId(uint32_t tokenId)
{
    tokenId_ = tokenId;
}

uint32_t LocationRequest::GetTokenId()
{
    return tokenId_;
}

int LocationRequest::GetPermUsedType()
{
    return permUsedType_;
}

void LocationRequest::SetPermUsedType(int type)
{
    permUsedType_ = type;
}

void LocationRequest::SetTokenIdEx(uint64_t tokenIdEx)
{
    tokenIdEx_ = tokenIdEx;
}

uint64_t LocationRequest::GetTokenIdEx()
{
    return tokenIdEx_;
}

void LocationRequest::SetFirstTokenId(uint32_t firstTokenId)
{
    firstTokenId_ = firstTokenId;
}

uint32_t LocationRequest::GetFirstTokenId()
{
    return firstTokenId_;
}

void LocationRequest::SetPackageName(std::string packageName)
{
    packageName_ = packageName;
}

std::string LocationRequest::GetPackageName()
{
    return packageName_;
}

bool LocationRequest::GetIsRequesting()
{
    return isRequesting_;
}

void LocationRequest::SetRequesting(bool state)
{
    isRequesting_ = state;
}

Location *LocationRequest::GetLastLocation()
{
    return &lastLocation_;
}

std::string LocationRequest::GetUuid()
{
    return uuid_;
}

void LocationRequest::SetUuid(std::string uuid)
{
    uuid_ = uuid;
}

void LocationRequest::SetLastLocation(const Location &location)
{
    lastLocation_.SetLatitude(location.GetLatitude());
    lastLocation_.SetLongitude(location.GetLongitude());
    lastLocation_.SetAltitude(location.GetAltitude());
    lastLocation_.SetAccuracy(location.GetAccuracy());
    lastLocation_.SetSpeed(location.GetSpeed());
    lastLocation_.SetDirection(location.GetDirection());
    lastLocation_.SetTimeStamp(location.GetTimeStamp());
    lastLocation_.SetTimeSinceBoot(location.GetTimeSinceBoot());
    lastLocation_.SetLocationSourceType(location.GetLocationSourceType());
}

Location *LocationRequest::GetBestLocation()
{
    return &bestLocation_;
}

void LocationRequest::SetBestLocation(const Location &location)
{
    bestLocation_.SetLatitude(location.GetLatitude());
    bestLocation_.SetLongitude(location.GetLongitude());
    bestLocation_.SetAltitude(location.GetAltitude());
    bestLocation_.SetAccuracy(location.GetAccuracy());
    bestLocation_.SetSpeed(location.GetSpeed());
    bestLocation_.SetDirection(location.GetDirection());
    bestLocation_.SetTimeStamp(location.GetTimeStamp());
    bestLocation_.SetTimeSinceBoot(location.GetTimeSinceBoot());
    bestLocation_.SetLocationSourceType(location.GetLocationSourceType());
}

void LocationRequest::GetProxyName(std::shared_ptr<std::list<std::string>> proxys)
{
    if (requestConfig_ == nullptr || proxys == nullptr) {
        return;
    }
#ifdef EMULATOR_ENABLED
    proxys->push_back(GNSS_ABILITY);
#else
    switch (requestConfig_->GetScenario()) {
        case SCENE_NAVIGATION:
        case SCENE_TRAJECTORY_TRACKING:
        case SCENE_CAR_HAILING: {
            proxys->push_back(GNSS_ABILITY);
            proxys->push_back(NETWORK_ABILITY);
            break;
        }
        case SCENE_DAILY_LIFE_SERVICE: {
            proxys->push_back(NETWORK_ABILITY);
            break;
        }
        case SCENE_NO_POWER: {
            proxys->push_back(PASSIVE_ABILITY);
            break;
        }
        case SCENE_UNSET: {
            GetProxyNameByPriority(proxys);
            break;
        }
        default:
            break;
    }
#endif
}

void LocationRequest::GetProxyNameByPriority(std::shared_ptr<std::list<std::string>> proxys)
{
    if (requestConfig_ == nullptr || proxys == nullptr) {
        return;
    }
#ifdef EMULATOR_ENABLED
    proxys->push_back(GNSS_ABILITY);
#else
    switch (requestConfig_->GetPriority()) {
        case PRIORITY_LOW_POWER:
            proxys->push_back(NETWORK_ABILITY);
            break;
        case PRIORITY_ACCURACY:
        case PRIORITY_FAST_FIRST_FIX:
            proxys->push_back(GNSS_ABILITY);
            proxys->push_back(NETWORK_ABILITY);
            break;
        default:
            break;
    }
#endif
}

bool LocationRequest::GetLocationPermState()
{
    return isUsingLocationPerm_;
}

bool LocationRequest::GetBackgroundPermState()
{
    return isUsingBackgroundPerm_;
}

bool LocationRequest::GetApproximatelyPermState()
{
    return isUsingApproximatelyPerm_;
}

void LocationRequest::SetLocationPermState(bool state)
{
    isUsingLocationPerm_ = state;
}

void LocationRequest::SetBackgroundPermState(bool state)
{
    isUsingBackgroundPerm_ = state;
}

void LocationRequest::SetApproximatelyPermState(bool state)
{
    isUsingApproximatelyPerm_ = state;
}

void LocationRequest::SetNlpRequestType(int nlpRequestType)
{
    nlpRequestType_ = nlpRequestType;
}

int LocationRequest::GetNlpRequestType()
{
    return nlpRequestType_;
}

void LocationRequest::SetNlpRequestType()
{
    if (requestConfig_->GetScenario() == SCENE_NAVIGATION ||
        requestConfig_->GetScenario() == SCENE_TRAJECTORY_TRACKING ||
        requestConfig_->GetScenario() == SCENE_CAR_HAILING ||
        requestConfig_->GetPriority() == PRIORITY_ACCURACY ||
        requestConfig_->GetPriority() == PRIORITY_FAST_FIRST_FIX) {
        nlpRequestType_ = NlpRequestType::PRIORITY_TYPE_INDOOR;
    } else {
        nlpRequestType_ = NlpRequestType::PRIORITY_TYPE_BALANCED_POWER_ACCURACY;
    }
}

std::string LocationRequest::ToString() const
{
    if (requestConfig_ == nullptr) {
        return "";
    }
    std::string str = "[request config: " + requestConfig_->ToString() +
        "] from pid:" + std::to_string(pid_) +
        ", uid:" + std::to_string(uid_) +
        ", tokenId:" + std::to_string(tokenId_) +
        ", tokenIdEx:" + std::to_string(tokenIdEx_) +
        ", firstTokenId:" + std::to_string(firstTokenId_) +
        ", uuid:" + uuid_ + ", packageName:" + packageName_;
    return str;
}

void LocationRequest::SetLocationErrorCallBack(ILocatorCallback *callback)
{
    locationErrorcallBack_ = callback;
}

ILocatorCallback* LocationRequest::GetLocationErrorCallBack()
{
    return locationErrorcallBack_;
}

} // namespace Location
} // namespace OHOS