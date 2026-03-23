/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_HDI_LOCATION_LOCATION_GNSS_V2_0_GNSSINTERFACEIMPL_H
#define OHOS_HDI_LOCATION_LOCATION_GNSS_V2_0_GNSSINTERFACEIMPL_H

#include "v2_0/ignss_interface.h"

namespace OHOS {
namespace HDI {
namespace Location {
namespace Gnss {
namespace V2_0 {
class GnssInterfaceImpl {
public:
    static GnssInterfaceImpl &GetInstance();

    int32_t SetGnssConfigPara(const GnssConfigPara& para);

    int32_t EnableGnss(IGnssCallback* callbackObj);

    int32_t DisableGnss();

    int32_t StartGnss(GnssStartType type);

    int32_t StopGnss(GnssStartType type);

    int32_t SetGnssReferenceInfo(const GnssRefInfo& refInfo);

    int32_t DeleteAuxiliaryData(unsigned short data);

    int32_t SetPredictGnssData(const std::string& data);

    int32_t GetCachedGnssLocationsSize(int32_t& size);

    int32_t GetCachedGnssLocations();

    int32_t SendNiUserResponse(int32_t gnssNiNotificationId, GnssNiResponseCmd userResponse);

    int32_t SendNetworkInitiatedMsg(const std::string& msg, int32_t length);

    int32_t EnableGnssMeasurement(IGnssMeasurementCallback *callbackObj);

    int32_t DisableGnssMeasurement();

    void ResetGnss();
private:
    GnssInterfaceImpl() = default;
    virtual ~GnssInterfaceImpl() = default;
};

} // V2_0
} // Gnss
} // Location
} // HDI
} // OHOS

#endif // OHOS_HDI_LOCATION_LOCATION_GNSS_V2_0_GNSSINTERFACEIMPL_H
