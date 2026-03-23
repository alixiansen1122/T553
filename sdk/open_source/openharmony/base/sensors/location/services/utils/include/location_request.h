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

#ifndef REQUEST_H
#define REQUEST_H

#include <list>
#include <string>

#include "i_locator_callback.h"
#include "request_config.h"
#include "app_identity.h"

namespace OHOS {
namespace Location {

enum NlpRequestType {
    PRIORITY_TYPE_BALANCED_POWER_ACCURACY = 102,  // NLP
    PRIORITY_TYPE_INDOOR = 300,                   // indoor
};

class LocationRequest {
public:
    LocationRequest();
    LocationRequest(const RequestConfig *requestConfig,
        ILocatorCallback *callback, AppIdentity &identity);
    ~LocationRequest();
    pid_t GetUid();
    pid_t GetPid();
    std::string GetPackageName();
    RequestConfig *GetRequestConfig();
    ILocatorCallback *GetLocatorCallBack();
    std::string GetUuid();
    void SetUid(pid_t uid);
    void SetPid(pid_t pid);
    void SetPackageName(std::string packageName);
    void SetRequestConfig(const RequestConfig& requestConfig);
    void SetLocatorCallBack(ILocatorCallback *callback);
    void SetUuid(std::string uuid);
    std::string ToString() const;
    void GetProxyName(std::shared_ptr<std::list<std::string>> proxys);
    bool GetIsRequesting();
    void SetRequesting(bool state);
    Location *GetLastLocation();
    void SetLastLocation(const Location& location);
    Location *GetBestLocation();
    void SetBestLocation(const Location& location);
    uint32_t GetTokenId();
    uint32_t GetFirstTokenId();
    uint64_t GetTokenIdEx();
    int GetPermUsedType();
    void SetPermUsedType(int type);
    void SetTokenId(uint32_t tokenId);
    void SetFirstTokenId(uint32_t firstTokenId);
    void SetTokenIdEx(uint64_t tokenIdEx);
    bool GetLocationPermState();
    bool GetBackgroundPermState();
    bool GetApproximatelyPermState();
    void SetLocationPermState(bool state);
    void SetBackgroundPermState(bool state);
    void SetApproximatelyPermState(bool state);
    void SetNlpRequestType(int nlpRequestType);
    int GetNlpRequestType();
    void SetNlpRequestType();
    void SetLocationErrorCallBack(ILocatorCallback *callback);
    ILocatorCallback *GetLocationErrorCallBack();

private:
    void GetProxyNameByPriority(std::shared_ptr<std::list<std::string>> proxys);

    pid_t uid_;
    pid_t pid_;
    uint32_t tokenId_;
    uint64_t tokenIdEx_;
    uint32_t firstTokenId_;
    int32_t nlpRequestType_;
    Location lastLocation_;
    Location bestLocation_;
    std::string packageName_;
    std::string uuid_;
    RequestConfig *requestConfig_ = nullptr;
    ILocatorCallback *callBack_ = nullptr;
    ILocatorCallback *locationErrorcallBack_ = nullptr;
    bool isRequesting_;
    bool isUsingLocationPerm_;
    bool isUsingBackgroundPerm_;
    bool isUsingApproximatelyPerm_;
    int32_t permUsedType_;
};
} // namespace Location
} // namespace OHOS
#endif // REQUEST_H
