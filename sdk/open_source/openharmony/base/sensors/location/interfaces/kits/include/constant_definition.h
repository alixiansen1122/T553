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

#ifndef CONSTANT_DEFINITION_H
#define CONSTANT_DEFINITION_H

#include <string>

namespace OHOS {
namespace Location {

enum LocationErrCode {
    ERRCODE_SUCCESS = 0,                      /* SUCCESS. */
    ERRCODE_FAILURE= -1,                      /* FAILURE. */
    ERRCODE_INVALID_PARAMS = 401,              /* Parameter error. */
    ERRCODE_NOT_SUPPORTED = 801,              /* Capability not supported. */
    ERRCODE_SERVICE_UNAVAILABLE = 3301000,    /* Location service is unavailable. */
    ERRCODE_LOCATING_FAIL = 3301200,          /* Failed to obtain the geographical location. */
    ERRCODE_NO_RESPONSE = 3301700,            /* No response to the request */
};

enum LocationRequestScenario {
    SCENE_UNSET = 0x0300,
    SCENE_NAVIGATION = 0x0301,
    SCENE_TRAJECTORY_TRACKING = 0x0302,
    SCENE_CAR_HAILING = 0x0303,
    SCENE_DAILY_LIFE_SERVICE = 0x0304,
    SCENE_NO_POWER = 0x0305
};

enum LocationRequestPriority {
    PRIORITY_UNSET = 0x0200,
    PRIORITY_ACCURACY = 0x0201,
    PRIORITY_LOW_POWER = 0x0202,
    PRIORITY_FAST_FIRST_FIX = 0x0203
};

enum LocationSatelliteConstellation {
    SV_CONSTELLATION_CATEGORY_UNKNOWN = 0,
    SV_CONSTELLATION_CATEGORY_GPS,
    SV_CONSTELLATION_CATEGORY_GLONASS,
    SV_CONSTELLATION_CATEGORY_QZSS,
    SV_CONSTELLATION_CATEGORY_BEIDOU,
    SV_CONSTELLATION_CATEGORY_GALILEO,
};

enum LocationSatelliteAdditionalInfo {
    SV_ADDITIONAL_INFO_NULL = 0,
    SV_ADDITIONAL_INFO_EPHEMERIS_DATA_EXIST = 1,
    SV_ADDITIONAL_INFO_ALMANAC_DATA_EXIST = 2,
    SV_ADDITIONAL_INFO_USED_IN_FIX = 4,
    SV_ADDITIONAL_INFO_CARRIER_FREQUENCY_EXIST = 8,
};

enum LocationErr {
    LOCATING_FAILED_DEFAULT = -1,
    LOCATING_FAILED_LOCATION_PERMISSION_DENIED = -2,
    LOCATING_FAILED_BACKGROUND_PERMISSION_DENIED = -3,
    LOCATING_FAILED_LOCATION_SWITCH_OFF = -4,
};

enum LocationSourceType {
    GNSS_TYPE = 1,
    NETWORK_TYPE = 2,
    INDOOR_TYPE = 3,
    RTK_TYPE = 4,
};

enum LocationLocatingStatus {
    LOCATING_STARTED = 0x0002,
    LOCATING_STOPED = 0x0003,
};

enum LocationCoordinateSystemType {
    WGS84 = 1,
    GCJ02,
};

typedef struct {
    int scenario;
    std::string command;
} LocationCommand;

} // namespace Location
} // namespace OHOS
#endif // CONSTANT_DEFINITION_H
