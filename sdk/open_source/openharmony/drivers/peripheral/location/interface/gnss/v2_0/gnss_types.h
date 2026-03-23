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

#ifndef OHOS_HDI_LOCATION_GNSS_V2_0_GNSSTYPES_H
#define OHOS_HDI_LOCATION_GNSS_V2_0_GNSSTYPES_H

#include <cstdbool>
#include <cstdint>
#include <string>
#include <vector>

namespace OHOS {
namespace HDI {
namespace Location {
namespace Gnss {
namespace V2_0 {

/**
 * @brief Enumerates return value types.
 */
typedef enum {
    GNSS_SUCCESS  = 0, /**< The operation is successful. */
    GNSS_FAILURE = -1, /**< Failed to invoke the OS underlying function. */
    GNSS_ERR_NOT_SUPPORT = -2, /**< Not supported. */
    GNSS_ERR_INVALID_PARAM = -3, /**< Invalid parameter. */
    GNSS_ERR_INVALID_OBJECT = -4, /**< Invalid object. */
    GNSS_ERR_MALLOC_FAIL    = -6, /**< Memory allocation fails. */
    GNSS_ERR_TIMEOUT        = -7, /**< Timeout occurs. */
    GNSS_ERR_THREAD_CREATE_FAIL = -10, /**< Failed to create a thread. */
    GNSS_ERR_QUEUE_FULL  = -15, /**< The queue is full. */
    GNSS_ERR_DEVICE_BUSY = -16, /**< The device is busy. */
    GNSS_ERR_IO          = -17, /**< I/O error. */
    GNSS_ERR_BAD_FD      = -18, /**< Incorrect file descriptor. */
    GNSS_ERR_NOPERM      = -19, /**< No permission. */
    GNSS_ERR_OUT_OF_RANGE = -20, /**< Failed to get all result */
} GNSS_ERR_STATUS;

enum GnssLocationValidity : int32_t {
    GNSS_LOCATION_LAT_VALID = 1,
    GNSS_LOCATION_LONG_VALID = 2,
    GNSS_LOCATION_ALTITUDE_VALID = 4,
    GNSS_LOCATION_SPEED_VALID = 8,
    GNSS_LOCATION_BEARING_VALID = 16,
    GNSS_LOCATION_HORIZONTAL_ACCURACY_VALID = 32,
    GNSS_LOCATION_VERTICAL_ACCURACY_VALID = 64,
    GNSS_LOCATION_SPEED_ACCURACY_VALID = 128,
    GNSS_LOCATION_BEARING_ACCURACY_VALID = 256,
    GNSS_LOCATION_TIME_VALID = 512,
    GNSS_LOCATION_TIME_SINCE_BOOT_VALID = 1024,
    GNSS_LOCATION_TIME_UNCERTAINTY_VALID = 2048,
    GNSS_LOCATION_NORTH_VELOCITY_VALID  = 4096,
    GNSS_LOCATION_EAST_VELOCITY_VALID   = 8192,
    GNSS_LOCATION_DESCEND_VELOCITY_VALID  = 16384,
    GNSS_LOCATION_SIGNAL_VALID = 32768,
};

enum GnssWorkingMode : int32_t {
    GNSS_WORKING_MODE_STANDALONE = 1,
    GNSS_WORKING_MODE_MS_BASED = 2,
    GNSS_WORKING_MODE_MS_ASSISTED = 3,
};

enum GnssStartType : int32_t {
    GNSS_START_TYPE_NORMAL = 1,
    GNSS_START_TYPE_GNSS_CACHE = 2,
};

enum GnssRefInfoType : int32_t {
    GNSS_REF_INFO_TIME = 1,
    GNSS_REF_INFO_LOCATION = 2,
    GNSS_REF_INFO_BEST_LOCATION = 3,
    GNSS_REF_INFO_GNSS_LOCATION_EMERGENCY = 4,
    GNSS_REF_INFO_BEST_LOCATION_EMERGENCY = 5,
};

enum GnssAuxiliaryDataType : int32_t {
    GNSS_AUXILIARY_DATA_EPHEMERIS = 1,
    GNSS_AUXILIARY_DATA_ALMANAC = 2,
    GNSS_AUXILIARY_DATA_POSITION = 4,
    GNSS_AUXILIARY_DATA_TIME = 8,
    GNSS_AUXILIARY_DATA_IONO = 16,
    GNSS_AUXILIARY_DATA_UTC = 32,
    GNSS_AUXILIARY_DATA_HEALTH = 64,
    GNSS_AUXILIARY_DATA_SVDIR = 128,
    GNSS_AUXILIARY_DATA_SVSTEER = 256,
    GNSS_AUXILIARY_DATA_SADATA = 512,
    GNSS_AUXILIARY_DATA_RTI = 1024,
    GNSS_AUXILIARY_DATA_CELLDB_INFO = 2048,
    GNSS_AUXILIARY_DATA_ALL = 65535,
};

enum GnssWorkingStatus : int32_t {
    GNSS_WORKING_STATUS_NONE = 0,
    GNSS_WORKING_STATUS_SESSION_BEGIN = 1,
    GNSS_WORKING_STATUS_SESSION_END = 2,
    GNSS_WORKING_STATUS_ENGINE_ON = 3,
    GNSS_WORKING_STATUS_ENGINE_OFF = 4,
};

enum GnssCapabilities : int32_t {
    GNSS_CAP_SUPPORT_MSB = 1,
    GNSS_CAP_SUPPORT_MSA = 2,
    GNSS_CAP_SUPPORT_GEOFENCING = 4,
    GNSS_CAP_SUPPORT_MEASUREMENTS = 8,
    GNSS_CAP_SUPPORT_NAV_MESSAGES = 16,
    GNSS_CAP_SUPPORT_GNSS_CACHE = 32,
};

enum ConstellationCategory : int32_t {
    CONSTELLATION_CATEGORY_UNKNOWN = 0,
    CONSTELLATION_CATEGORY_GPS = 1,
    CONSTELLATION_CATEGORY_SBAS = 2,
    CONSTELLATION_CATEGORY_GLONASS = 3,
    CONSTELLATION_CATEGORY_QZSS = 4,
    CONSTELLATION_CATEGORY_BEIDOU = 5,
    CONSTELLATION_CATEGORY_GALILEO = 6,
    CONSTELLATION_CATEGORY_IRNSS = 7,
    CONSTELLATION_CATEGORY_MAXIMUM = 8,
};

enum SatelliteAdditionalInfo : int32_t {
    SATELLITES_ADDITIONAL_INFO_NULL = 0,
    SATELLITES_ADDITIONAL_INFO_EPHEMERIS_DATA_EXIST = 1,
    SATELLITES_ADDITIONAL_INFO_ALMANAC_DATA_EXIST = 2,
    SATELLITES_ADDITIONAL_INFO_USED_IN_FIX = 4,
    SATELLITES_ADDITIONAL_INFO_CARRIER_FREQUENCY_EXIST = 8,
};

enum GnssNiRequestCategory : int32_t {
    GNSS_NI_REQUEST_CATEGORY_EMERGENCY_SUPL = 1,
    GNSS_NI_REQUEST_CATEGORY_VOICE = 2,
    GNSS_NI_REQUEST_CATEGORY_UMTS_CONTROL_PLANE = 3,
    GNSS_NI_REQUEST_CATEGORY_UMTS_SUPL = 4,
};

enum GnssNiResponseCmd : int32_t {
    GNSS_NI_RESPONSE_CMD_ACCEPT = 1,
    GNSS_NI_RESPONSE_CMD_NO_RESPONSE = 2,
    GNSS_NI_RESPONSE_CMD_REJECT = 3,
};

enum GnssNiNotificationCategory : int32_t {
    GNSS_NI_NOTIFICATION_REQUIRE_NOTIFY = 1,
    GNSS_NI_NOTIFICATION_REQUIRE_VERIFY = 2,
    GNSS_NI_NOTIFICATION_REQUIRE_PRIVACY_OVERRIDE = 4,
};

enum GnssNiRequestEncodingFormat : int32_t {
    GNSS_NI_ENCODING_FORMAT_NULL = 1,
    GNSS_NI_ENCODING_FORMAT_SUPL_GSM_DEFAULT = 2,
    GNSS_NI_ENCODING_FORMAT_SUPL_UCS2 = 3,
    GNSS_NI_ENCODING_FORMAT_SUPL_UTF8 = 4,
};

struct SatelliteStatusInfo {
    uint32_t satellitesNumber;
    std::vector<int16_t> satelliteIds;
    std::vector<OHOS::HDI::Location::Gnss::V2_0::ConstellationCategory> constellation;
    std::vector<float> carrierToNoiseDensitys;
    std::vector<float> elevation;
    std::vector<float> azimuths;
    std::vector<float> carrierFrequencies;
    std::vector<uint32_t> additionalInfo;
};

struct GnssBasicConfig {
    uint32_t minInterval;
    OHOS::HDI::Location::Gnss::V2_0::GnssWorkingMode gnssMode;
} __attribute__ ((aligned(8)));

struct GnssCachingConfig {
    uint32_t interval;
    bool fifoFullNotify;
} __attribute__ ((aligned(8)));

struct GnssConfigPara {
    OHOS::HDI::Location::Gnss::V2_0::GnssBasicConfig gnssBasic;
    OHOS::HDI::Location::Gnss::V2_0::GnssCachingConfig gnssCaching;
} __attribute__ ((aligned(8)));

struct GnssRefTime {
    int64_t time;
    int64_t elapsedRealtime;
    int64_t uncertaintyOfTime;
} __attribute__ ((aligned(8)));

struct LocationInfo {
    uint32_t fieldValidity;
    double latitude;
    double longitude;
    double altitude;
    float speed;
    float bearing;
    float horizontalAccuracy;
    float verticalAccuracy;
    float speedAccuracy;
    float bearingAccuracy;
    float velocityNorth;
    float velocityEast;
    float velocityDescend;
    int64_t timeForFix;
    int64_t timeSinceBoot;
    int64_t timeUncertainty;
    int32_t signal;
} __attribute__ ((aligned(8)));

struct GnssRefInfo {
    OHOS::HDI::Location::Gnss::V2_0::GnssRefInfoType type;
    OHOS::HDI::Location::Gnss::V2_0::GnssRefTime time;
    OHOS::HDI::Location::Gnss::V2_0::LocationInfo gnssLocation;
    OHOS::HDI::Location::Gnss::V2_0::LocationInfo bestLocation;
} __attribute__ ((aligned(8)));

struct GnssNiNotificationRequest {
    int16_t gnssNiNotificationId;
    OHOS::HDI::Location::Gnss::V2_0::GnssNiRequestCategory gnssNiRequestCategory;
    int32_t notificationCategory;
    int32_t requestTimeout;
    int32_t defaultResponseCmd;
    std::string supplicantInfo;
    std::string notificationText;
    OHOS::HDI::Location::Gnss::V2_0::GnssNiRequestEncodingFormat supplicantInfoEncoding;
    OHOS::HDI::Location::Gnss::V2_0::GnssNiRequestEncodingFormat notificationTextEncoding;
};

struct GnssMeasurement {
    uint32_t fieldValidflags;
    int16_t satelliteId;
    int16_t constellationCategory;
    double timeOffset;
    uint32_t syncState;
    int64_t receivedSatelliteTime;
    int64_t receivedSatelliteTimeUncertainty;
    double cn0;
    double pseudorangeRate;
    double pseudorangeRateUncertainty;
    uint32_t accumulatedDeltaRangeFlag;
    double accumulatedDeltaRange;
    double accumulatedDeltaRangeUncertainty;
    float carrierFrequency;
    int64_t carrierCyclesCount;
    double carrierPhase;
    double carrierPhaseUncertainty;
    uint32_t multipathFlag;
    double agcGain;
    uint32_t codeCategory;
    double ionoCorrect;
    double tropCorrect;
    double satelliteClockBias;
    double satelliteClockDriftBias;
} __attribute__ ((aligned(8)));

struct GnssClockInfo {
    uint16_t fieldValidFlags;
    int16_t leapSecond;
    int64_t receiverClockTime;
    double timeUncertainty;
    int64_t rcvClockFullBias;
    double rcvClockSubBias;
    double biasUncertainty;
    double clockDrift;
    double clockDriftUncertainty;
    uint32_t clockInterruptCnt;
    double clockJumpThreshold;
    uint32_t clockHWFreBiasIndicator;
    uint32_t clockHWFreDriftIndicator;
} __attribute__ ((aligned(8)));

struct GnssMeasurementInfo {
    OHOS::HDI::Location::Gnss::V2_0::GnssClockInfo gnssClock;
    int64_t elapsedRealtime;
    int64_t uncertainty;
    int32_t measurementCount;
    std::vector<OHOS::HDI::Location::Gnss::V2_0::GnssMeasurement> measurements;
};

} // V2_0
} // Gnss
} // Location
} // HDI
} // OHOS

#endif // OHOS_HDI_LOCATION_GNSS_V2_0_GNSSTYPES_H