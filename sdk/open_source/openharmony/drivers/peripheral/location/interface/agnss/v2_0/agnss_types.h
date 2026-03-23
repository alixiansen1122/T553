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

#ifndef OHOS_HDI_LOCATION_AGNSS_V2_0_AGNSSTYPES_H
#define OHOS_HDI_LOCATION_AGNSS_V2_0_AGNSSTYPES_H

#include <cstdbool>
#include <cstdint>
#include <string>
#include <vector>

#define ERR_OK 0

namespace OHOS {
namespace HDI {
namespace Location {
namespace Agnss {
namespace V2_0 {

using namespace OHOS;

/**
 * @brief Enumerates return value types.
 */
typedef enum {
    AGNSS_SUCCESS  = 0, /**< The operation is successful. */
    AGNSS_FAILURE = -1, /**< Failed to invoke the OS underlying function. */
    AGNSS_ERR_NOT_SUPPORT = -2, /**< Not supported. */
    AGNSS_ERR_INVALID_PARAM = -3, /**< Invalid parameter. */
    AGNSS_ERR_INVALID_OBJECT = -4, /**< Invalid object. */
    AGNSS_ERR_MALLOC_FAIL    = -6, /**< Memory allocation fails. */
    AGNSS_ERR_TIMEOUT        = -7, /**< Timeout occurs. */
    AGNSS_ERR_THREAD_CREATE_FAIL = -10, /**< Failed to create a thread. */
    AGNSS_ERR_QUEUE_FULL  = -15, /**< The queue is full. */
    AGNSS_ERR_DEVICE_BUSY = -16, /**< The device is busy. */
    AGNSS_ERR_IO          = -17, /**< I/O error. */
    AGNSS_ERR_BAD_FD      = -18, /**< Incorrect file descriptor. */
    AGNSS_ERR_NOPERM      = -19, /**< No permission. */
    AGNSS_ERR_OUT_OF_RANGE = -20, /**< Failed to get all result */
} AGNSS_ERR_STATUS;

enum AGnssRefInfoType : int32_t {
    ANSS_REF_INFO_TYPE_CELLID = 1,
    ANSS_REF_INFO_TYPE_MAC = 2,
};

enum AGnssUserPlaneProtocol : int32_t {
    AGNSS_TYPE_SUPL = 1,
    AGNSS_TYPE_C2K = 2,
    AGNSS_TYPE_SUPL_IMS = 3,
    AGNSS_TYPE_SUPL_EIMS = 4,
};

enum DataLinkSetUpType : int32_t {
    ESTABLISH_DATA_CONNECTION = 1,
    RELEASE_DATA_CONNECTION = 2,
};

enum DataConnectionState : int32_t {
    DATA_CONNECTION_DISCONNECTED = 1,
    DATA_CONNECTION_CONNECTED = 2,
};

enum CellIdType : int32_t {
    CELLID_TYPE_GSM = 1,
    CELLID_TYPE_UMTS = 2,
    CELLID_TYPE_LTE = 3,
    CELLID_TYPE_NR = 4,
};

enum SubscriberSetIdType : int32_t {
    AGNSS_SETID_TYPE_NULL = 0,
    AGNSS_SETID_TYPE_IMSI = 1,
    AGNSS_SETID_TYPE_MSISDN = 2,
};

enum ApnIpCategory : int32_t {
    APN_CATEGORY_INVALID = 0,
    APN_CATEGORY_IPV4 = 1,
    APN_CATEGORY_IPV6 = 2,
    APN_CATEGORY_IPV4V6 = 3,
};

struct AGnssRefCellId {
    OHOS::HDI::Location::Agnss::V2_0::CellIdType type;
    uint16_t mcc;
    uint16_t mnc;
    uint16_t lac;
    uint32_t cid;
    uint16_t tac;
    uint16_t pcid;
    uint32_t nci;
} __attribute__ ((aligned(8)));

struct AGnssServerInfo {
    OHOS::HDI::Location::Agnss::V2_0::AGnssUserPlaneProtocol type;
    std::string server;
    int32_t port;
};

struct SubscriberSetId {
    OHOS::HDI::Location::Agnss::V2_0::SubscriberSetIdType type;
    std::string id;
};

struct AGnssRefMac {
    std::vector<uint8_t> mac;
};

struct AGnssRefInfo {
    OHOS::HDI::Location::Agnss::V2_0::AGnssRefInfoType type;
    OHOS::HDI::Location::Agnss::V2_0::AGnssRefCellId cellId;
    OHOS::HDI::Location::Agnss::V2_0::AGnssRefMac mac;
};

struct AGnssDataLinkRequest {
    OHOS::HDI::Location::Agnss::V2_0::AGnssUserPlaneProtocol agnssType;
    OHOS::HDI::Location::Agnss::V2_0::DataLinkSetUpType setUpType;
    std::vector<uint8_t> serverIpAddr;
    std::vector<uint8_t> serverIpV6Addr;
};

struct NetworkState {
    int32_t netId;
    OHOS::HDI::Location::Agnss::V2_0::ApnIpCategory apnIpCategory;
    std::string apn;
    OHOS::HDI::Location::Agnss::V2_0::DataConnectionState state;
};

} // V2_0
} // Agnss
} // Location
} // HDI
} // OHOS

#endif // OHOS_HDI_LOCATION_AGNSS_V2_0_AGNSSTYPES_H