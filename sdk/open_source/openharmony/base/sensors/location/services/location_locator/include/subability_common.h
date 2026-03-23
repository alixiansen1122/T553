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

#ifndef SUBABILITY_COMMON_H
#define SUBABILITY_COMMON_H

#include <vector>
#include "constant_definition.h"
#include "i_locator_callback.h"
#include "location.h"
#include "location_log.h"
#include "work_record.h"

namespace OHOS {
namespace Location {

class SubAbility  {
public:
    SubAbility();
    virtual ~SubAbility();
    virtual bool Initialize() = 0;
    virtual bool DeInitialize() = 0;
    virtual LocationErrCode StartAbility() = 0;
    virtual LocationErrCode StopAbility() = 0;
    virtual LocationErrCode EnableAbility() = 0;
    virtual LocationErrCode DisableAbility() = 0;
    virtual LocationErrCode SendLocationRequest(WorkRecord &workrecord) = 0;
    void SetAbility(std::string name);
    void LocationRequest(WorkRecord &workrecord);
    void HandleRefrashRequirements();
    int GetRequestNum();

    void ReportLocationInfo(const std::string& systemAbility, const std::shared_ptr<Location>& location);
    void StopAllLocationRequests();
    void RestartAllLocationRequests();
private:
    void HandleLocalRequest(WorkRecord &record);
    void HandleRemoveRecord(WorkRecord &newRecord);
    void HandleAddRecord(WorkRecord &newRecord);
    virtual void RequestRecord(WorkRecord &workRecord, bool isAdded) = 0;

    int interval_ = 0;
    std::string name_ = "";
    std::shared_ptr<WorkRecord> lastRecord_;
    std::shared_ptr<WorkRecord> newRecord_;
};
} // namespace Location
} // namespace OHOS
#endif // SUBABILITY_COMMON_H
