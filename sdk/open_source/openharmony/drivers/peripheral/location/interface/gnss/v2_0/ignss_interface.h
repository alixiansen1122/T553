/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_HDI_LOCATION_GNSS_V2_0_IGNSSINTERFACE_H
#define OHOS_HDI_LOCATION_GNSS_V2_0_IGNSSINTERFACE_H

#include <stdint.h>
#include <string>
#include "v2_0/gnss_types.h"
#include "v2_0/ignss_callback.h"
#include "v2_0/ignss_measurement_callback.h"

namespace OHOS {
namespace HDI {
namespace Location {
namespace Gnss {
namespace V2_0 {
using namespace OHOS;
using namespace OHOS::HDI;

class IGnssInterface {
public:

    static IGnssInterface &GetInstance();

    int32_t SetGnssConfigPara(const OHOS::HDI::Location::Gnss::V2_0::GnssConfigPara& para);

    int32_t EnableGnss(OHOS::HDI::Location::Gnss::V2_0::IGnssCallback *callbackObj);

    int32_t DisableGnss();

    int32_t StartGnss(OHOS::HDI::Location::Gnss::V2_0::GnssStartType type);

    int32_t StopGnss(OHOS::HDI::Location::Gnss::V2_0::GnssStartType type);

    int32_t SetGnssReferenceInfo(const OHOS::HDI::Location::Gnss::V2_0::GnssRefInfo& refInfo);

    int32_t DeleteAuxiliaryData(uint16_t data);

    int32_t SetPredictGnssData(const std::string& data);

    int32_t GetCachedGnssLocationsSize(int32_t& size);

    int32_t GetCachedGnssLocations();

    int32_t SendNiUserResponse(int32_t gnssNiNotificationId,
         OHOS::HDI::Location::Gnss::V2_0::GnssNiResponseCmd userResponse);

    int32_t SendNetworkInitiatedMsg(const std::string& msg, int32_t length);

    int32_t EnableGnssMeasurement(OHOS::HDI::Location::Gnss::V2_0::IGnssMeasurementCallback* callbackObj);

    int32_t DisableGnssMeasurement();

    virtual int32_t GetVersion(uint32_t& majorVer, uint32_t& minorVer)
    {
        majorVer = 2;
        minorVer = 0;
        return GNSS_SUCCESS;
    }
private:
    IGnssInterface() = default;
    virtual ~IGnssInterface() = default;
};
} // V2_0
} // Gnss
} // Location
} // HDI
} // OHOS

#endif // OHOS_HDI_LOCATION_GNSS_V2_0_IGNSSINTERFACE_H