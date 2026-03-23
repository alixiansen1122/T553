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

#ifndef REPORT_MANAGER_H
#define REPORT_MANAGER_H

#include <string>
#include <time.h>
#include <map>
#include "location_thread.h"
#include "i_locator_callback.h"
#include "location.h"
#include "location_request.h"

namespace OHOS {
namespace Location {
class ReportManager {
public:
    ReportManager();
    ~ReportManager();
    bool ReportRemoteCallback(ILocatorCallback *locatorCallback, int type, int result);
    bool OnReportLocation(const std::shared_ptr<Location> &location, std::string abilityName);
    bool ResultCheck(const std::shared_ptr<Location> &location, const std::shared_ptr<LocationRequest> &request);
    void UpdateCacheLocation(const std::shared_ptr<Location> &location, std::string abilityName);
    std::shared_ptr<Location> GetLastLocation();
    std::shared_ptr<Location> GetCacheLocation(const std::shared_ptr<LocationRequest> &request);
    std::shared_ptr<Location> GetPermittedLocation(const std::shared_ptr<LocationRequest> &request,
                                                   const std::shared_ptr<Location> &location);
    void UpdateRandom();
    bool IsRequestFuse(const std::shared_ptr<LocationRequest> &request);
    void UpdateLocationByRequest(const uint32_t tokenId, const uint64_t tokenIdEx, std::shared_ptr<Location> &location);

    static ReportManager *GetInstance();

private:
    struct timespec lastUpdateTime_;
    double offsetRandom_;
    std::map<int, std::shared_ptr<Location>> lastLocationsMap_;
    Location cacheGnssLocation_;
    Location cacheNlpLocation_;
    MutexId lastLocationMutex_;
    std::shared_ptr<Location> ApproximatelyLocation(const std::shared_ptr<Location> &location);
    bool ProcessRequestForReport(std::shared_ptr<LocationRequest> &request,
                                 std::list<std::shared_ptr<LocationRequest>> &deadRequests,
                                 const std::shared_ptr<Location> &location, std::string abilityName);

    void UpdateLastLocation(const std::shared_ptr<Location> &location);
};
}  // namespace OHOS
}  // namespace Location
#endif  // REPORT_MANAGER_H
