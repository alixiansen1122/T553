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

#ifndef LOCATOR_SERVICE_H
#define LOCATOR_SERVICE_H

#include <map>
#include <mutex>
#include "single_instance.h"
#include "service.h"

namespace OHOS {
namespace Location {

class LocatorService : public Service {
DECLARE_SINGLE_INSTANCE(LocatorService);
public:
    bool Initialize();
    Identity *GetIdentity();
    static uint16 GetId(Service *service);

private:
    static const char *GetServiceName(Service *service);
    static BOOL ServiceInitialize(Service *service, Identity identity);
    static TaskConfig GetServiceTaskConfig(Service *service);
    static BOOL ServiceMessageHandle(Service *service, Request *request);

private:
    Identity identity_;
};
} // namespace Location
} // namespace OHOS
#endif // LOCATOR_ABILITY_H
