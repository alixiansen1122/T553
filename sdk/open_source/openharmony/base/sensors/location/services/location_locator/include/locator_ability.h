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

#ifndef LOCATOR_ABILITY_H
#define LOCATOR_ABILITY_H

#include <map>
#include "single_instance.h"
#include "location_thread.h"
#include "locator.h"
#include "app_identity.h"
#include "common_utils.h"
#include "constant_definition.h"
#include "permission_status_change_cb.h"
#include "location_request.h"
#include "request_manager.h"
#include "report_manager.h"

namespace OHOS {
namespace Location {
class SubAbility;
class LocatorAbility {
DECLARE_SINGLE_INSTANCE(LocatorAbility);
public:
    bool Initialize();
    bool DeInitialize();
 
    LocationErrCode IsLocationEnabled(bool &isEnabled);

    LocationErrCode EnableAbility(bool isEnabled);
#ifdef FEATURE_GNSS_SUPPORT
    LocationErrCode RegisterGnssStatusCallback(IGnssStatusCallback *callback, pid_t uid);
    LocationErrCode UnregisterGnssStatusCallback(IGnssStatusCallback *callback);
    LocationErrCode RegisterNmeaMessageCallback(INmeaMessageCallback *callback, pid_t uid);
    LocationErrCode UnregisterNmeaMessageCallback(INmeaMessageCallback *callback);
    LocationErrCode SendCommand(const LocationCommand *commands);
#endif
    LocationErrCode StartLocating(const RequestConfig *requestConfig,
        ILocatorCallback *callback, AppIdentity &identity);
    LocationErrCode StopLocating(ILocatorCallback *callback);
    LocationErrCode GetCacheLocation(Location &loc, AppIdentity &identity);

    LocationErrCode IsLocationPrivacyConfirmed(const int type, bool& isConfirmed);
    LocationErrCode SetLocationPrivacyConfirmStatus(const int type, bool isConfirmed);
    LocationErrCode ReportLocation(
        const std::shared_ptr<Location>& location, std::string abilityName, AppIdentity &identity);
    LocationErrCode ReportLocationStatus(ILocatorCallback *callback, int result);
    LocationErrCode ReportErrorStatus(ILocatorCallback *callback, int result);

    LocationErrCode RegisterLocationError(ILocatorCallback *callback, AppIdentity &identity);
    LocationErrCode UnregisterLocationError(ILocatorCallback *callback, AppIdentity &identity);
    void ReportLocationError(std::string uuid, int32_t errCode);

    std::shared_ptr<std::map<std::string, std::list<std::shared_ptr<LocationRequest>>>> GetRequests();
    std::shared_ptr<std::map<ILocatorCallback *, std::list<std::shared_ptr<LocationRequest>>>> GetReceivers();

    void ApplyRequests(int delay);
    int GetActiveRequestNum();
#ifdef PERMISSION_SUPPORT
    void RegisterPermissionCallback(const uint32_t callingTokenId, const std::vector<std::string>& permissionNameList);
    void UnregisterPermissionCallback(const uint32_t callingTokenId);
#endif
    void UpdatePermissionUsedRecord(uint32_t tokenId, std::string permissionName,
        int permUsedType, int succCnt, int failCnt);
    LocationErrCode RemoveInvalidRequests();
    bool IsInvalidRequest(std::shared_ptr<LocationRequest>& request);
#ifdef FEATURE_GNSS_SUPPORT
    LocationErrCode QuerySupportCoordinateSystemType(
        std::vector<LocationCoordinateSystemType>& coordinateSystemTypes);
    LocationErrCode SendNetworkLocation(const std::shared_ptr<Location>& location);
#endif
    void UpdateLastLocationRequestNum();

private:
    LocationErrCode InitAbility();
    LocationErrCode DeInitAbility();
    LocationErrCode StartAbility();
    LocationErrCode StopAbility();
    void InitRequestManagerMap();
 
    void UpdateLoadedAbilityMap();
    bool NeedReportCacheLocation(const std::shared_ptr<LocationRequest>& request, ILocatorCallback *callback);
    void HandleStartLocating(const std::shared_ptr<LocationRequest>& request, ILocatorCallback *callback);
    bool IsCacheVaildScenario(const RequestConfig* requestConfig);
    bool IsSingleRequest(const RequestConfig* requestConfig);
    bool CheckIsReportPermitted(AppIdentity &identity);

    bool isInited_ = false;
    bool isEnabled_ = false;
    MutexId requestsMutex_;
    MutexId receiversMutex_;
#ifdef PERMISSION_SUPPORT
    MutexId permissionMapMutex_;
#endif
    MutexId loadedAbilityMapMutex_;
    std::shared_ptr<std::map<std::string, std::list<std::shared_ptr<LocationRequest>>>> requests_;
    std::shared_ptr<std::map<ILocatorCallback *, std::list<std::shared_ptr<LocationRequest>>>> receivers_;
    std::shared_ptr<std::map<std::string, SubAbility *>> loadedAbilityMap_;
#ifdef PERMISSION_SUPPORT
    std::shared_ptr<std::map<uint32_t, std::shared_ptr<PermissionStatusChangeCb>>> permissionMap_;
#endif
    RequestManager *requestManager_ = nullptr;
    ReportManager *reportManager_ = nullptr;
};

} // namespace Location
} // namespace OHOS
#endif // LOCATOR_ABILITY_H
