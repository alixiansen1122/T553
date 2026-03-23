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

#include "locator_service.h"
#include "locator_service_interface.h"
#include "location_log.h"
#include "common_utils.h"

#include "ohos_init.h"
#include "samgr_lite.h"

namespace OHOS {
namespace Location {
const int STACK_SIZE = 0x1C00;
const int QUEUE_SIZE = 20;
IMPLEMENT_SINGLE_INSTANCE(LocatorService);

static void Init()
{
    SamgrLite *sm = SAMGR_GetInstance();
    CHECK_NULLPTR_RETURN(sm, "LocatorService", "get samgr error");
    bool inited = LocatorService::GetInstance().Initialize();
    CHECK_FAILED_RETURN("LocatorService", inited, true, "LocatorService init error");
    BOOL result = sm->RegisterService(&LocatorService::GetInstance());
    LBSLOGI(LOCATOR_STANDARD, "Locator service starts %s", result ? "successfully" : "unsuccessfully");
}

SYSEX_SERVICE_INIT(Init);

bool LocatorService::Initialize()
{
    this->Service::GetName = LocatorService::GetServiceName;
    this->Service::Initialize = LocatorService::ServiceInitialize;
    this->Service::MessageHandle = LocatorService::ServiceMessageHandle;
    this->Service::GetTaskConfig = LocatorService::GetServiceTaskConfig;
    return true;
}

const char *LocatorService::GetServiceName(Service *service)
{
    (void)service;
    return LOCATOR_SERVICE;
}

BOOL LocatorService::ServiceInitialize(Service *service, Identity identity)
{
    if (service == nullptr) {
        return FALSE;
    }
    LocatorService *locatorService = static_cast<LocatorService *>(service);
    locatorService->identity_ = identity;
    return TRUE;
}

Identity *LocatorService::GetIdentity()
{
    return &identity_;
}

BOOL LocatorService::ServiceMessageHandle(Service *service, Request *request)
{
    LBSLOGI(LOCATOR_STANDARD, "LocatorService::ServiceMessageHandle");
    if (service == nullptr || request == nullptr) {
        return FALSE;
    }
    return TRUE;
}

TaskConfig LocatorService::GetServiceTaskConfig(Service *service)
{
    TaskConfig config = {LEVEL_HIGH, PRI_NORMAL, STACK_SIZE, QUEUE_SIZE, SINGLE_TASK};
    return config;
}
} // namespace Location
} // namespace OHOS
