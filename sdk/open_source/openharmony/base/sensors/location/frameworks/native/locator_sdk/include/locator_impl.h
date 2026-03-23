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
#ifndef LOCATOR_IMPL_H
#define LOCATOR_IMPL_H

#include <vector>
#include "locator.h"
#include "location_thread.h"
#include "single_instance.h"
#include "locator_service_interface.h"

namespace OHOS {
namespace Location {

class LocatorImpl {
DECLARE_SINGLE_INSTANCE(LocatorImpl);
public:
    bool Init();
    bool DeInit();

    LocationErrCode IsLocationEnabled(bool &isEnabled);

    LocationErrCode EnableAbility(bool enable);

    LocationErrCode StartLocating(const RequestConfig *requestConfig,
        ILocatorCallback *callback);

    LocationErrCode StopLocating(ILocatorCallback *callback);

    LocationErrCode RegisterGnssStatusCallback(IGnssStatusCallback *callback);

    LocationErrCode UnregisterGnssStatusCallback(IGnssStatusCallback *callback);

    LocationErrCode RegisterNmeaMessageCallback(INmeaMessageCallback *callback);

    LocationErrCode UnregisterNmeaMessageCallback(INmeaMessageCallback *callback);

    LocationErrCode QuerySupportCoordinateSystemType(
        std::vector<LocationCoordinateSystemType>& coordinateSystemTypes);

    LocationErrCode IsLocationPrivacyConfirmed(const int type, bool &isConfirmed);

    LocationErrCode SetLocationPrivacyConfirmStatus(const int type, bool isConfirmed);

    LocationErrCode SendCommand(const LocationCommand *commands);

private:
    MutexId mutex_ = nullptr;
    bool inited_ = false;

    LocatorInterface *locatorProxy_ { nullptr };
};

}  // namespace Location
}  // namespace OHOS
#endif // LOCATOR_IMPL_H
