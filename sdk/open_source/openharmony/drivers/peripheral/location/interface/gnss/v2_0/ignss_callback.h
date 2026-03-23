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

#ifndef OHOS_HDI_LOCATION_GNSS_V2_0_IGNSSCALLBACK_H
#define OHOS_HDI_LOCATION_GNSS_V2_0_IGNSSCALLBACK_H

#include <stdint.h>
#include <string>
#include <vector>
#include "v2_0/gnss_types.h"

#ifndef HDI_BUFF_MAX_SIZE
#define HDI_BUFF_MAX_SIZE (1024 * 200)
#endif

namespace OHOS {
namespace HDI {
namespace Location {
namespace Gnss {
namespace V2_0 {
using namespace OHOS;
using namespace OHOS::HDI;

enum {
    CMD_GNSS_CALLBACK_GET_VERSION = 0,
    CMD_GNSS_CALLBACK_REPORT_LOCATION = 1,
    CMD_GNSS_CALLBACK_REPORT_GNSS_WORKING_STATUS = 2,
    CMD_GNSS_CALLBACK_REPORT_NMEA = 3,
    CMD_GNSS_CALLBACK_REPORT_GNSS_CAPABILITIES = 4,
    CMD_GNSS_CALLBACK_REPORT_SATELLITE_STATUS_INFO = 5,
    CMD_GNSS_CALLBACK_REQUEST_GNSS_REFERENCE_INFO = 6,
    CMD_GNSS_CALLBACK_REQUEST_PREDICT_GNSS_DATA = 7,
    CMD_GNSS_CALLBACK_REPORT_CACHED_LOCATION = 8,
    CMD_GNSS_CALLBACK_REPORT_GNSS_NI_NOTIFICATION = 9,
};

class IGnssCallback {
public:

    virtual ~IGnssCallback() = default;

    virtual int32_t ReportLocation(const OHOS::HDI::Location::Gnss::V2_0::LocationInfo& location) = 0;

    virtual int32_t ReportGnssWorkingStatus(OHOS::HDI::Location::Gnss::V2_0::GnssWorkingStatus status) = 0;

    virtual int32_t ReportNmea(int64_t timestamp, const std::string& nmea, int32_t length) = 0;

    virtual int32_t ReportGnssCapabilities(uint32_t capabilities) = 0;

    virtual int32_t ReportSatelliteStatusInfo(const OHOS::HDI::Location::Gnss::V2_0::SatelliteStatusInfo& info) = 0;

    virtual int32_t RequestGnssReferenceInfo(OHOS::HDI::Location::Gnss::V2_0::GnssRefInfoType type) = 0;

    virtual int32_t RequestPredictGnssData() = 0;

    virtual int32_t ReportCachedLocation(const std::vector<OHOS::HDI::Location::Gnss::V2_0::LocationInfo>& gnssLocations) = 0;

    virtual int32_t ReportGnssNiNotification(const OHOS::HDI::Location::Gnss::V2_0::GnssNiNotificationRequest& notification) = 0;

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

#endif // OHOS_HDI_LOCATION_GNSS_V2_0_IGNSSCALLBACK_H