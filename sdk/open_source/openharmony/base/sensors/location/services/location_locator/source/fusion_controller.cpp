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

#include "fusion_controller.h"
#include "common_utils.h"
#include "constant_definition.h"
#include "location_log.h"
#ifdef FEATURE_NETWORK_SUPPORT
#include "network_ability_proxy.h"
#endif

namespace OHOS {
namespace Location {
const uint32_t FUSION_DEFAULT_FLAG = 0;
const uint32_t FUSION_BASE_FLAG = 1;
const uint32_t REPORT_FUSED_LOCATION_FLAG = FUSION_BASE_FLAG;

#ifdef FEATURE_NETWORK_SUPPORT
const uint32_t QUICK_FIX_FLAG = FUSION_BASE_FLAG << 1;
#endif
const long NANOS_PER_MILLI = 1000000L;
const long MAX_GNSS_LOCATION_COMPARISON_MS = 30 * MILLI_PER_SEC;
const long MAX_INDOOR_LOCATION_COMPARISON_MS = 5 * MILLI_PER_SEC;
const double MAX_INDOOR_LOCATION_SPEED = 3.0;

FusionController* FusionController::GetInstance()
{
    static FusionController data;
    return &data;
}

void FusionController::ActiveFusionStrategies(int type)
{
    if (needReset_) {
        fusedFlag_ = FUSION_DEFAULT_FLAG;
        needReset_ = false;
    }
    switch (type) {
        case SCENE_NAVIGATION:
        case SCENE_TRAJECTORY_TRACKING:
            break;
        case PRIORITY_FAST_FIRST_FIX:
            fusedFlag_ = fusedFlag_ | REPORT_FUSED_LOCATION_FLAG;
            break;
        default:
            break;
    }
}

std::shared_ptr<Location> FusionController::chooseBestLocation(const std::shared_ptr<Location>& location,
    const std::shared_ptr<Location>& lastFuseLocation)
{
    if (location == nullptr) {
        return nullptr;
    }
    if (lastFuseLocation == nullptr) {
        return location;
    }
    if (location->GetLocationSourceType() == LocationSourceType::INDOOR_TYPE) {
        if (CheckIfLastGnssLocationValid(location, lastFuseLocation) &&
            lastFuseLocation->GetSpeed() >= MAX_INDOOR_LOCATION_SPEED) {
            return lastFuseLocation;
        }
        return std::make_shared<Location>(*location);
    } else if (location->GetLocationSourceType() == LocationSourceType::GNSS_TYPE ||
                location->GetLocationSourceType() == LocationSourceType::RTK_TYPE) {
        if (location->GetSpeed() >= MAX_INDOOR_LOCATION_SPEED) {
            return location;
        }
        if (CheckIfLastIndoorLocationValid(location, lastFuseLocation)) {
            return lastFuseLocation;
        }
    } else if (location->GetLocationSourceType() == LocationSourceType::NETWORK_TYPE) {
        if (CheckIfLastIndoorLocationValid(location, lastFuseLocation)) {
            return lastFuseLocation;
        } else if (CheckIfLastGnssLocationValid(location, lastFuseLocation)) {
            return lastFuseLocation;
        }
    }
    return location;
}

bool FusionController::CheckIfLastIndoorLocationValid(const std::shared_ptr<Location>& location,
    const std::shared_ptr<Location>& lastFuseLocation)
{
    if (lastFuseLocation->GetLocationSourceType() == LocationSourceType::INDOOR_TYPE &&
        ((location->GetTimeSinceBoot() / NANOS_PER_MILLI -
        lastFuseLocation->GetTimeSinceBoot() / NANOS_PER_MILLI) < MAX_INDOOR_LOCATION_COMPARISON_MS)) {
        return true;
    } else {
        return false;
    }
}

bool FusionController::CheckIfLastGnssLocationValid(const std::shared_ptr<Location>& location,
    const std::shared_ptr<Location>& lastFuseLocation)
{
    if (lastFuseLocation->GetLocationSourceType() == LocationSourceType::GNSS_TYPE &&
        ((location->GetTimeSinceBoot() / NANOS_PER_MILLI -
        lastFuseLocation->GetTimeSinceBoot() / NANOS_PER_MILLI) < MAX_GNSS_LOCATION_COMPARISON_MS)) {
        return true;
    } else {
        return false;
    }
}

std::shared_ptr<Location> FusionController::GetFuseLocation(const std::shared_ptr<Location>& location,
    const std::shared_ptr<Location>& lastFuseLocation)
{
    LBSLOGD(FUSION_CONTROLLER, " GetFuseLocation enter");
    return chooseBestLocation(location, lastFuseLocation);
}
} // namespace Location
} // namespace OHOS