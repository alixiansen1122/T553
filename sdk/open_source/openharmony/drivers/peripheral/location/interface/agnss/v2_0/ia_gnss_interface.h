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

#ifndef OHOS_HDI_LOCATION_AGNSS_V2_0_IAGNSSINTERFACE_H
#define OHOS_HDI_LOCATION_AGNSS_V2_0_IAGNSSINTERFACE_H

#include <stdint.h>
#include "v2_0/agnss_types.h"
#include "v2_0/ia_gnss_callback.h"

namespace OHOS {
namespace HDI {
namespace Location {
namespace Agnss {
namespace V2_0 {
using namespace OHOS;
using namespace OHOS::HDI;

class IAGnssInterface {
public:

    static IAGnssInterface &GetInstance();

    int32_t SetAgnssCallback(OHOS::HDI::Location::Agnss::V2_0::IAGnssCallback* callbackObj);

    int32_t SetAgnssServer(const OHOS::HDI::Location::Agnss::V2_0::AGnssServerInfo& server);

    int32_t SetAgnssRefInfo(const OHOS::HDI::Location::Agnss::V2_0::AGnssRefInfo& refInfo);

    int32_t SetSubscriberSetId(const OHOS::HDI::Location::Agnss::V2_0::SubscriberSetId& id);

    int32_t SendNetworkState(const OHOS::HDI::Location::Agnss::V2_0::NetworkState& state);

    virtual int32_t GetVersion(uint32_t& majorVer, uint32_t& minorVer)
    {
        majorVer = 2;
        minorVer = 0;
        return AGNSS_SUCCESS;
    }
private:
    IAGnssInterface() = default;
    virtual ~IAGnssInterface() = default;
};
} // V2_0
} // Agnss
} // Location
} // HDI
} // OHOS

#endif // OHOS_HDI_LOCATION_AGNSS_V2_0_IAGNSSINTERFACE_H