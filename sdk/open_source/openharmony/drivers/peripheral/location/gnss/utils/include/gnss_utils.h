/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: pgnss encode
 * Create:2024-11-11
 */
#ifndef GNSS_UTILS_H
#define GNSS_UTILS_H
#include <stdint.h>
#include "gnss_nmea_parse.h"
#include "location_vendor_lib.h"

#pragma pack(1)

typedef struct {
    uint16_t year;   // YYYY
    uint8_t month;   // MM, 1~12
    uint8_t day;     // DD, 1~31
    uint8_t hour;    // hh, 0~23
    uint8_t minute;  // mm, 0~59
    uint8_t second;  // ss, 0~59
    uint16_t ms;     // ms, 0~999
    uint8_t valid;   // 0: invalid, 1: valid
} GnssUtcTime;

#pragma pack()

int32_t LocationGnssUtilsUtc2UnixTime(const GnssUtcTime *utcTime, int64_t *unixTime);

int64_t LocationGnssUtilsGetTimestampMS(void);

int32_t LocationGnssUtilsFillGnssLocation(const GnssInfo *gnssInfo, OHOS::HDI::Location::GnssLocation *location);

int32_t LocationGnssUtilsFillAllSatelliteStatus(const GnssInfo *gnssInfo,
                                                OHOS::HDI::Location::GnssSatelliteStatus *status);

#endif