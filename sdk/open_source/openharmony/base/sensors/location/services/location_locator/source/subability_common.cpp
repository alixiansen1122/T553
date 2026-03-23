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

#include "subability_common.h"
#include "locator_ability.h"
#include "common_utils.h"

#include "work_record_statistic.h"
#include "app_identity.h"

namespace OHOS {
namespace Location {
SubAbility::SubAbility()
{
    newRecord_ = std::make_shared<WorkRecord>();
    lastRecord_ = std::make_shared<WorkRecord>();
}

SubAbility::~SubAbility()
{
    newRecord_ = nullptr;
    lastRecord_ = nullptr;
}

void SubAbility::SetAbility(std::string name)
{
    name_ = name;
}

void SubAbility::StopAllLocationRequests()
{
    // When the switch is turned off, all current requests are stopped
    std::shared_ptr<WorkRecord> emptyRecord = std::make_shared<WorkRecord>();
    HandleLocalRequest(*emptyRecord);
    lastRecord_->Clear();
}

void SubAbility::RestartAllLocationRequests()
{
    // When the switch is turned on, all SA requests will be refreshed
    lastRecord_->Clear();
    HandleRefrashRequirements();
}

void SubAbility::LocationRequest(WorkRecord &workRecord)
{
    interval_ = workRecord.GetTimeInterval(0);
    newRecord_->Clear();
    newRecord_->Set(workRecord);
    HandleRefrashRequirements();
}

void SubAbility::HandleRefrashRequirements()
{
    LBSLOGD(LOCATOR, "refrash requirements");

    // send local request
    HandleLocalRequest(*newRecord_);
    lastRecord_->Clear();
    lastRecord_->Set(*newRecord_);
    auto workRecordStatistic = WorkRecordStatistic::GetInstance();
    if (!workRecordStatistic->Update(name_, GetRequestNum())) {
        LBSLOGE(LOCATOR, "workRecordStatistic::Update failed");
    }
}

int SubAbility::GetRequestNum()
{
    if (newRecord_ == nullptr) {
        return 0;
    }
    return newRecord_->Size();
}

void SubAbility::HandleLocalRequest(WorkRecord &record)
{
    HandleRemoveRecord(record);
    HandleAddRecord(record);
}

void SubAbility::HandleRemoveRecord(WorkRecord &newRecord)
{
    for (int i = 0; i < lastRecord_->Size(); i++) {
        int uid = lastRecord_->GetUid(i);
        bool isFind = newRecord.Find(uid, lastRecord_->GetName(i), lastRecord_->GetUuid(i));
        LBSLOGD(LOCATOR, "remove record isFind:%d, uid:%d, lastRecord:%s, newRecord:%s",
            isFind, uid, lastRecord_->ToString().c_str(), newRecord.ToString().c_str());
        if (!isFind) {
            std::shared_ptr<WorkRecord> workRecord = std::make_shared<WorkRecord>();
            workRecord->Add(*lastRecord_, i);
            workRecord->SetDeviceId(newRecord.GetDeviceId());
            RequestRecord(*workRecord, false);
        }
    }
}

void SubAbility::HandleAddRecord(WorkRecord &newRecord)
{
    for (int i = 0; i < newRecord.Size(); i++) {
        int uid = newRecord.GetUid(i);
        bool isFind = lastRecord_->Find(uid, newRecord.GetName(i), lastRecord_->GetUuid(i));
        LBSLOGD(LOCATOR, "add record isFind:%d, uid:%d, lastRecord:%s, newRecord:%s",
            isFind, uid, lastRecord_->ToString().c_str(), newRecord.ToString().c_str());
        if (!isFind) {
            std::shared_ptr<WorkRecord> workRecord = std::make_shared<WorkRecord>();
            workRecord->Add(newRecord, i);
            workRecord->SetDeviceId(newRecord.GetDeviceId());
            RequestRecord(*workRecord, true);
        }
    }
}
void SubAbility::ReportLocationInfo(
    const std::string& systemAbility, const std::shared_ptr<Location>& location)
{
    AppIdentity identity;
    LocatorAbility::GetInstance().ReportLocation(location, systemAbility, identity);
}
} // namespace Location
} // namespace OHOS
