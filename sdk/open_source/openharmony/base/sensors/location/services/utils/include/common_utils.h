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
#ifndef LOCATION_COMMON_UTILS_H
#define LOCATION_COMMON_UTILS_H

#include <atomic>
#include <condition_variable>
#include <unistd.h>
#include <string>

#include "constant_definition.h"
#include "location_log.h"
#include "location.h"

namespace OHOS {
namespace Location {
const std::string GNSS_ABILITY = "gps";
const std::string NETWORK_ABILITY = "network";
const std::string PASSIVE_ABILITY = "passive";
const std::string FUSED_ABILITY = "fused";
const std::string DEFAULT_ABILITY = "default";

const std::string LOCATION_DIR = "/data/service/el1/public/location/";
const std::string SWITCH_CONFIG_NAME = "location_switch";
const std::string PRIVACY_CONFIG_NAME = "location_privacy";

constexpr const char* SUPL_MODE_NAME = "const.location.supl_mode";
constexpr const char* EDM_POLICY_NAME = "persist.edm.location_policy";
constexpr const char* AGNSS_SERVER_ADDR = "const.location.agnss_server_addr";
constexpr const char* AGNSS_SERVER_PORT = "const.location.agnss_server_port";

const int MAX_BUFF_SIZE = 100;

const int MILLI_PER_SEC = 1000;

const double PI = 3.1415926;
const double DEGREE_PI = 180.0;
const double DIS_FROMLL_PARAMETER = 2;
const double EARTH_RADIUS = 6378137.0; // earth semimajor axis (WGS84) (m)

static constexpr double MIN_LATITUDE = -90.0;
static constexpr double MAX_LATITUDE = 90.0;
static constexpr double MIN_LONGITUDE = -180.0;
static constexpr double MAX_LONGITUDE = 180.0;
static constexpr double DEGREE_DOUBLE_PI = 360.0;
static constexpr long LONG_TIME_INTERVAL = 24 * 60 * 60;
static constexpr int32_t LOCATION_LOADSA_TIMEOUT_MS = 5000;

static constexpr int LOCATOR_UID = 1021;
static constexpr int MAXIMUM_INTERATION = 100;
static constexpr int MAXIMUM_CACHE_LOCATIONS = 1000;
static constexpr int MAXIMUM_LOCATING_REQUIRED_DATAS = 200;
static constexpr int MAC_LEN = 6;
static constexpr int DEFAULT_CODE = 0;
static constexpr int MAX_TRANSITION_SIZE = 3;
static constexpr int MAX_NOTIFICATION_REQUEST_LIST_SIZE = 3;

enum {
    DISABLED = 0,
    ENABLED = 1,
    DEFAULT_STATE = 2
};

enum {
    SUCCESS = 0,
    COMMON_ERROR = 98,
    PARAM_IS_EMPTY = 99,
    NOT_SUPPORTED = 100,
    INPUT_PARAMS_ERROR = 101,
    REVERSE_GEOCODE_ERROR,
    GEOCODE_ERROR,
    LOCATOR_ERROR,
    LOCATION_SWITCH_ERROR,
    LAST_KNOWN_LOCATION_ERROR,
    LOCATION_REQUEST_TIMEOUT_ERROR,
    QUERY_COUNTRY_CODE_ERROR,
};

enum {
    PERMISSION_REVOKED_OPER = 0,
    PERMISSION_GRANTED_OPER = 1
};

#define CHECK_NULLPTR_RETURN_PTR(point, name, fmt)            \
    do {                                                      \
        if (point == nullptr) {                               \
            LBSLOGE(name, fmt);                                \
            return nullptr;                                   \
        }                                                     \
    } while (0)

#define CHECK_NULLPTR_RETURN(point, name, fmt)            \
    do {                                                  \
        if (point == nullptr) {                           \
            LBSLOGE(name, fmt);                            \
            return;                                       \
        }                                                 \
    } while (0)

#define CHECK_NULLPTR_RETURN_CODE(point, name, fmt, code) \
    do {                                                  \
        if (point == nullptr) {                           \
            LBSLOGE(name, fmt);                            \
            return code;                                  \
        }                                                 \
    } while (0)

#define CHECK_FAILED_RETURN(name, value, target, outStr) \
    do { \
        if ((value) != (target)) { \
            LBSLOGE(name, "%s %d != %d", ((outStr) != nullptr) ? (outStr) : " ", (value), (target)); \
            return; \
        } \
    } while (0)


class CommonUtils {
public:

    static bool GetCurrentUserId(int &userId);
    static bool DoubleEqual(double a, double b);
    static double CalDistance(const double lat1, const double lon1, const double lat2, const double lon2);
    static double DoubleRandom(double min, double max);
    static int IntRandom(int min, int max);
    static int64_t GetCurrentTime();
    static int64_t GetCurrentTimeStamp();
    static std::vector<std::string> Split(std::string str, std::string pattern);
    static errno_t GetMacArray(const std::string& strMac, uint8_t mac[MAC_LEN]);
    static unsigned char ConvertStringToDigit(std::string str);
    static std::string GenerateUuid();
    static bool CheckAppForUser(int32_t uid);
    static int64_t GetSinceBootTime();
};

} // namespace Location
} // namespace OHOS
#endif // COMMON_UTILS_H
