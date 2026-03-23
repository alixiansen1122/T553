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

#ifndef REQUEST_MANAGER_H
#define REQUEST_MANAGER_H

#include <list>
#include <map>
#include <string>
#include "location_thread.h"
#include "i_locator_callback.h"
#include "location_request.h"
#include "work_record.h"

namespace OHOS {
namespace Location {
class RequestManager {
public:
    RequestManager();
    ~RequestManager();
    void HandleStartLocating(std::shared_ptr<LocationRequest> request);
    void HandleStopLocating(ILocatorCallback* callback);
    void HandlePowerSuspendChanged(int32_t pid, int32_t uid, int32_t flag);
    void UpdateRequestRecord(std::shared_ptr<LocationRequest> request, bool shouldInsert);
    void HandleRequest();
    void UpdateUsingPermission(std::shared_ptr<LocationRequest> request, const bool isStart);
    void HandlePermissionChanged(uint32_t tokenId);
    void UpdateLocationErrorCallbackToRequest(ILocatorCallback* callback, uint32_t tokenId, bool state);
    static RequestManager* GetInstance();

private:
    bool RestorRequest(std::shared_ptr<LocationRequest> request);
    void HandleChrEvent(std::list<std::shared_ptr<LocationRequest>> requests);
    void UpdateRequestRecord(std::shared_ptr<LocationRequest> request, std::string abilityName, bool shouldInsert);
    void DeleteRequestRecord(std::shared_ptr<std::list<std::shared_ptr<LocationRequest>>> requests);
    void HandleRequest(std::string abilityName, std::list<std::shared_ptr<LocationRequest>> list);
    void ProxySendLocationRequest(std::string abilityName, WorkRecord& workRecord);
    bool IsUidInProcessing(int32_t uid);
    void UpdateUsingApproximatelyPermission(std::shared_ptr<LocationRequest> request, const bool isStart);
    bool ActiveLocatingStrategies(const std::shared_ptr<LocationRequest>& request);
    bool AddRequestToWorkRecord(std::string abilityName, std::shared_ptr<LocationRequest>& request,
        std::shared_ptr<WorkRecord>& workRecord);
    bool IsRequestAvailable(std::shared_ptr<LocationRequest>& request);
    void UpdateRunningUids(const std::shared_ptr<LocationRequest>& request, std::string abilityName, bool isAdd);
    void ReportDataToResSched(std::string state, const pid_t uid);
    std::map<int32_t, int32_t> runningUidMap_;
    static MutexId requestMutex_;
    MutexId runningUidsMutex_;
    MutexId permissionRecordMutex_;
    std::atomic_bool isDeviceIdleMode_;
    std::atomic_bool isDeviceStillState_;
};
} // namespace Location
} // namespace OHOS
#endif // REQUEST_MANAGER_H
