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

#include "location.h"

namespace OHOS {
namespace Location {
static constexpr double MIN_LATITUDE = -90.0;
static constexpr double MIN_LONGITUDE = -180.0;

Location::Location()
{
    latitude_ = MIN_LATITUDE - 1;
    longitude_ = MIN_LONGITUDE - 1;
    altitude_ = 0.0;
    accuracy_ = 0.0;
    speed_ = 0.0;
    direction_ = 0.0;
    timeStamp_ = 0;
    timeSinceBoot_ = 0;
    additionSize_ = 0;
    altitudeAccuracy_ = 0.0;
    speedAccuracy_ = 0.0;
    directionAccuracy_ = 0.0;
    velocityNorth_ = 0.0;
    velocityEast_ = 0.0;
    velocityDescend_ = 0.0;
    uncertaintyOfTimeSinceBoot_ = 0;
    locationSourceType_ = 0;
    statusNumber_ = 0;
    uuid_ = "";
}

Location::Location(Location& location)
{
    latitude_ = location.GetLatitude();
    longitude_ = location.GetLongitude();
    altitude_ = location.GetAltitude();
    accuracy_ = location.GetAccuracy();
    speed_ = location.GetSpeed();
    direction_ = location.GetDirection();
    timeStamp_ = location.GetTimeStamp();
    timeSinceBoot_ = location.GetTimeSinceBoot();
    additions_ = location.GetAdditions();
    additionsMap_ = location.GetAdditionsMap();
    additionSize_ = location.GetAdditionSize();
    altitudeAccuracy_ = location.GetAltitudeAccuracy();
    speedAccuracy_ = location.GetSpeedAccuracy();
    directionAccuracy_ = location.GetDirectionAccuracy();
    velocityNorth_ = location.GetNorthVelocity();
    velocityEast_ = location.GetEastVelocity();
    velocityDescend_ = location.GetDescendVelocity();
    uncertaintyOfTimeSinceBoot_ = location.GetUncertaintyOfTimeSinceBoot();
    locationSourceType_ = location.GetLocationSourceType();
    statusNumber_ = location.GetStatusNumber();
    uuid_ = location.GetUuid();
}

std::string Location::ToString() const
{
    std::string str =
        ", altitude : " + std::to_string(altitude_) +
        ", accuracy : " + std::to_string(accuracy_) +
        ", speed : " + std::to_string(speed_) +
        ", direction : " + std::to_string(direction_) +
        ", timeStamp : " + std::to_string(timeStamp_) +
        ", timeSinceBoot : " + std::to_string(timeSinceBoot_) +
        ", additionSize : " + std::to_string(additionSize_) +
        ", altitudeAccuracy : " + std::to_string(altitudeAccuracy_) +
        ", speedAccuracy : " + std::to_string(speedAccuracy_) +
        ", directionAccuracy : " + std::to_string(directionAccuracy_) +
        ", velocityEast : " + std::to_string(velocityEast_) +
        ", velocityNorth_ : " + std::to_string(velocityNorth_) +
        ", velocityDescend_ : " + std::to_string(velocityDescend_) +
        ", uncertaintyOfTimeSinceBoot : " + std::to_string(uncertaintyOfTimeSinceBoot_) +
        ", locationSourceType : " + std::to_string(locationSourceType_) +
        ", statusNumber : " + std::to_string(statusNumber_) +
        ", uuid : " + uuid_;
    return str;
}

bool Location::LocationEqual(const Location &location)
{
    if (this->GetLatitude() == location.GetLatitude() &&
        this->GetLongitude() == location.GetLongitude() &&
        this->GetAltitude() == location.GetAltitude() &&
        this->GetAccuracy() == location.GetAccuracy() &&
        this->GetSpeed() == location.GetSpeed() &&
        this->GetDirection() == location.GetDirection() &&
        this->GetTimeStamp() == location.GetTimeStamp() &&
        this->GetTimeSinceBoot() == location.GetTimeSinceBoot() &&
        this->AdditionEqual(location) &&
        this->GetAdditionSize() == location.GetAdditionSize()) {
        return true;
    }
    return false;
}

bool Location::AdditionEqual(const Location &location)
{
    std::vector<std::string> additionA = this->GetAdditions();
    std::vector<std::string> additionB = location.GetAdditions();
    if (additionA.size() != additionB.size()) {
        return false;
    }
    for (size_t i = 0; i < additionA.size(); i++) {
        if (additionA[i].compare(additionB[i]) != 0) {
            return false;
        }
    }
    return true;
}
} // namespace Location
} // namespace OHOS