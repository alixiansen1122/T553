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

#ifndef OHOS_HDI_LOCATION_GNSS_V2_0_IGNSSMEASUREMENTCALLBACK_H
#define OHOS_HDI_LOCATION_GNSS_V2_0_IGNSSMEASUREMENTCALLBACK_H

#include <stdint.h>
#include "v2_0/gnss_types.h"

namespace OHOS {
namespace HDI {
namespace Location {
namespace Gnss {
namespace V2_0 {
using namespace OHOS;
using namespace OHOS::HDI;

class IGnssMeasurementCallback {
public:

    virtual ~IGnssMeasurementCallback() = default;

    virtual int32_t ReportGnssMeasurementInfo(const OHOS::HDI::Location::Gnss::V2_0::GnssMeasurementInfo& data) = 0;

    virtual int32_t GetVersion(uint32_t& majorVer, uint32_t& minorVer)
    {
        majorVer = 2;
        minorVer = 0;
        return GNSS_SUCCESS;
    }
};
} // V2_0
} // Gnss
} // Location
} // HDI
} // OHOS

#endif // OHOS_HDI_LOCATION_GNSS_V2_0_IGNSSMEASUREMENTCALLBACK_H