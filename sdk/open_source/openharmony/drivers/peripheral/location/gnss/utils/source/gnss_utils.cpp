/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: pgnss encode
 * Author:
 * Create:2024-11-11
 */

#include "gnss_utils.h"
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/time.h>
#ifdef CMSIS_SUPPORT
#include <cmsis_os2.h>
#else
#include <pthread.h>
#endif
#include "securec.h"
#include "location_utils.h"
#include "location_log.h"
#include "v2_0/gnss_types.h"

using namespace OHOS::HDI::Location;
using namespace OHOS::HDI::Location::Gnss::V2_0;

#define ONE_CENTURY_YEARS 100
#define FOUR_CENTURY_YEARS 400
#define LEAP_YEAR_LENGTH 4
#define UNIX_TIME_START_YEAR 1970  // start year of unix time
#define MAX_MONTH (12)
#define TIME_UNIT 1000
#define SECONDS_PER_MIN (60)
#define SECONDS_PER_HOUR (60 * SECONDS_PER_MIN)
#define SECONDS_PER_DAY (24 * SECONDS_PER_HOUR)
#define SECONDS_PER_NORMAL_YEAR (365 * SECONDS_PER_DAY)

/* days per month on normal and leap year */
static int32_t g_normMonthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static int32_t g_leapMonthDays[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/* 获取begin和end年份之间的闰年数 */
static int32_t GetLeapYearNum(uint16_t begin, uint16_t end)
{
    if (end <= begin || end < 1) {
        return GNSS_FAILURE;
    }
    int32_t numPer400Year = (end - 1) / FOUR_CENTURY_YEARS - begin / FOUR_CENTURY_YEARS;
    int32_t numPer100Year = (end - 1) / ONE_CENTURY_YEARS - begin / ONE_CENTURY_YEARS;
    int32_t numPer4Year = (end - 1) / LEAP_YEAR_LENGTH - begin / LEAP_YEAR_LENGTH;
    return (numPer400Year + numPer4Year - numPer100Year);
}

/* 闰年判断 */
static int32_t CheckIfLeapYear(uint16_t year)
{
    if ((year % ONE_CENTURY_YEARS) == 0) {
        if ((year % FOUR_CENTURY_YEARS) == 0) {
            return 1;
        }
    } else {
        if ((year % LEAP_YEAR_LENGTH) == 0) {
            return 1;
        }
    }
    return 0;
}

/* 将日历时(年月日时分秒) 转换为 unix时间(1970年1月1日起的时间秒数) */
int32_t LocationGnssUtilsUtc2UnixTime(const GnssUtcTime *utcTime, int64_t *unixTime)
{
    if (utcTime == NULL || unixTime == NULL) {
        LBSLOGE(HDI_GNSS, "error: [Utc2UnixTime] null pointer!");
        return GNSS_FAILURE;
    }

    if (utcTime->month > MAX_MONTH || utcTime->valid == 0) {
        LBSLOGE(HDI_GNSS, "error: [Utc2UnixTime] utcTime invalid!");
        return GNSS_FAILURE;
    }

    // 判断是否为闰年
    int32_t *monthDays = (CheckIfLeapYear(utcTime->year) == 1) ? g_leapMonthDays : g_normMonthDays;

    // 计算从1970.01.01开始的unix 秒数
    int64_t timestamp = 0;
    timestamp += (int64_t)(utcTime->year - UNIX_TIME_START_YEAR) * SECONDS_PER_NORMAL_YEAR;
    timestamp += (int64_t)GetLeapYearNum(UNIX_TIME_START_YEAR, utcTime->year) * SECONDS_PER_DAY;

    for (int32_t i = 1; i < utcTime->month; i++) {
        timestamp += (int64_t)monthDays[i - 1] * SECONDS_PER_DAY;
    }

    timestamp += (int64_t)(utcTime->day - 1) * SECONDS_PER_DAY;
    timestamp += (int64_t)utcTime->hour * SECONDS_PER_HOUR;
    timestamp += (int64_t)utcTime->minute * SECONDS_PER_MIN;
    int64_t timestampSec = (int64_t)(utcTime->second * TIME_UNIT + utcTime->ms);

    *unixTime = timestamp + timestampSec;
    return GNSS_SUCCESS;
}

int64_t LocationGnssUtilsGetTimestampMS(void)
{
    constexpr int64_t US_PER_MS = 1000;
    constexpr int64_t MS_PER_S = 1000;
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    int64_t ms = static_cast<int64_t>(tv.tv_usec) / US_PER_MS;
    return static_cast<int64_t>(tv.tv_sec) * MS_PER_S + ms;
}

int32_t FillGnssLocationTimeInfo(const GnssInfo *gnssInfo, GnssLocation *location)
{
    if (gnssInfo->field & GNSS_NMEA_FIELD_TYPE_UTCTIME) {
        location->fieldValidity |= GNSS_LOCATION_TIME_VALID;
        GnssUtcTime utcTime = {gnssInfo->utc.year, gnssInfo->utc.month, gnssInfo->utc.day,      gnssInfo->utc.hour,
                               gnssInfo->utc.min,  gnssInfo->utc.sec,   gnssInfo->utc.centisec, 1};
        int64_t unixTime = 0;
        LocationGnssUtilsUtc2UnixTime(&utcTime, &unixTime);
        location->timeForFix = unixTime;
        location->size++;
    }
    struct timespec timestamp = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &timestamp);
    const uint64_t SEC_TO_NANO = 1000 * 1000 * 1000;
    uint64_t timeSinceBoot = timestamp.tv_sec * SEC_TO_NANO + timestamp.tv_nsec;

    location->timeSinceBoot = timeSinceBoot;
    location->timeUncertainty = 0;
    LBSLOGD(HDI_GNSS, "timeForFix:%u timeSinceBoot:%llu", location->timeForFix, location->timeSinceBoot);
    return GNSS_SUCCESS;
}

int32_t LocationGnssUtilsFillGnssLocation(const GnssInfo *gnssInfo, GnssLocation *location)
{
    if (gnssInfo->signal == 0) {
        LBSLOGW(HDI_GNSS, "Location invalid");
        return GNSS_FAILURE;
    }
    location->fieldValidity = GNSS_NMEA_FIELD_TYPE_SMASK;
    if (gnssInfo->field & GNSS_NMEA_FIELD_TYPE_SIGNAL) {
        location->signal = (int32_t)gnssInfo->signal;
        location->fieldValidity |= GNSS_LOCATION_SIGNAL_VALID;
        location->size++;
    }
    if (gnssInfo->field & GNSS_NMEA_FIELD_TYPE_LAT) {
        location->latitude = gnssInfo->lat;
        location->fieldValidity |= GNSS_LOCATION_LAT_VALID;
        location->size++;
    }
    if (gnssInfo->field & GNSS_NMEA_FIELD_TYPE_LON) {
        location->longitude = gnssInfo->lon;
        location->fieldValidity |= GNSS_LOCATION_LONG_VALID;
        location->size++;
    }
    if (gnssInfo->field & GNSS_NMEA_FIELD_TYPE_ANTALT) {
        location->altitude = gnssInfo->altitude;
        location->fieldValidity |= GNSS_LOCATION_ALTITUDE_VALID;
        location->size++;
    }
    if (gnssInfo->field & GNSS_NMEA_FIELD_TYPE_SPEED) {
        location->speed = gnssInfo->speed;
        location->fieldValidity |= GNSS_LOCATION_SPEED_VALID;
        location->size++;
    }
    if (gnssInfo->field & GNSS_NMEA_FIELD_TYPE_TRACK) {
        location->bearing = gnssInfo->trackAng;
        location->fieldValidity |= GNSS_LOCATION_BEARING_VALID;
        location->size++;
    }
    if (gnssInfo->field & GNSS_NMEA_FIELD_TYPE_HORIZONTAL_ACCURACY) {
        location->horizontalAccuracy = gnssInfo->horizontalAccuracy;
        location->fieldValidity |= GNSS_LOCATION_HORIZONTAL_ACCURACY_VALID;
        location->size++;
    }
    if (gnssInfo->field & GNSS_NMEA_FIELD_TYPE_VERTICAL_ACCURACY) {
        location->verticalAccuracy = gnssInfo->verticalAccuracy;
        location->fieldValidity |= GNSS_LOCATION_VERTICAL_ACCURACY_VALID;
        location->size++;
    }
    if (gnssInfo->field & GNSS_NMEA_FIELD_TYPE_SPEED_ACCURACY) {
        location->speedAccuracy = gnssInfo->speedAccuracy;
        location->fieldValidity |= GNSS_LOCATION_SPEED_ACCURACY_VALID;
        location->size++;
    }
    if (gnssInfo->field & GNSS_NMEA_FIELD_TYPE_NORTH_VELOCITY) {
        location->velocityNorth = gnssInfo->velocityNorth;
        location->fieldValidity |= GNSS_LOCATION_NORTH_VELOCITY_VALID;
        location->size++;
    }
    if (gnssInfo->field & GNSS_NMEA_FIELD_TYPE_EAST_VELOCITY) {
        location->velocityEast = gnssInfo->velocityEast;
        location->fieldValidity |= GNSS_LOCATION_EAST_VELOCITY_VALID;
        location->size++;
    }
    if (gnssInfo->field & GNSS_NMEA_FIELD_TYPE_DESCEND_VELOCITY) {
        location->velocityDescend = gnssInfo->velocityDescend;
        location->fieldValidity |= GNSS_LOCATION_DESCEND_VELOCITY_VALID;
        location->size++;
    }

    FillGnssLocationTimeInfo(gnssInfo, location);

    LBSLOGD(HDI_GNSS,
            "location size :%lu field:0x%x lat%.3f lon:%.3f alti:%.3f speed:%.3f bearing:%.3f "
            "hAcc:%.3f vAcc:%.3f sAcc:%.3f bearingAcc:%.3f timeFix:%lld tSB:%llu velocityNorth:%.3f"
            "velocityEast:%.3f velocityDescend:%.3f",
            location->size, location->fieldValidity, location->latitude, location->longitude, location->altitude,
            location->speed, location->bearing, location->horizontalAccuracy, location->verticalAccuracy,
            location->speedAccuracy, location->bearingAccuracy, location->timeForFix, location->timeSinceBoot, location->velocityNorth,
            location->velocityEast, location->velocityDescend);
    return GNSS_SUCCESS;
}

static int32_t FillGnssSatelliteStatusFromStatusInfo(const GnssSatelliteStatusInfo *satInfo,
                                                     OHOS::HDI::Location::GnssSatelliteStatus *status,
                                                     ConstellationCategory category)
{
    if (satInfo->satViewedCnt == 0 || satInfo->satViewedCnt > GNSS_SAT_NUM_MAX ||
        status->satellitesNum >= SATELLITE_NUM_MAXIMUM) {
        LBSLOGI(HDI_GNSS, "satViewedCnt invalid");
        return GNSS_FAILURE;
    }
    for (uint32_t satIdx = 0; satIdx < satInfo->satViewedCnt; satIdx++) {
        status->satellitesList[status->satellitesNum].constellationCategory = category;
        status->satellitesList[status->satellitesNum].satelliteId = satInfo->satelliteStatus[satIdx].svid;
        status->satellitesList[status->satellitesNum].elevation = satInfo->satelliteStatus[satIdx].ele;
        status->satellitesList[status->satellitesNum].azimuth = satInfo->satelliteStatus[satIdx].az;
        status->satellitesList[status->satellitesNum].carrierFrequency = 0.0;
        status->satellitesList[status->satellitesNum].cn0 = satInfo->satelliteStatus[satIdx].cnr;
        status->satellitesList[status->satellitesNum].satelliteAdditionalInfo = 0;
        LBSLOGD(HDI_GNSS, "category:%d satInfo sat index:%u svid:%u ele:%u az:%u cnr:%u", category, satIdx,
                satInfo->satelliteStatus[satIdx].svid, satInfo->satelliteStatus[satIdx].ele,
                satInfo->satelliteStatus[satIdx].az, satInfo->satelliteStatus[satIdx].cnr);
        status->satellitesNum++;
    }
    return GNSS_SUCCESS;
}

int32_t LocationGnssUtilsFillAllSatelliteStatus(const GnssInfo *gnssInfo,
                                                OHOS::HDI::Location::GnssSatelliteStatus *status)
{
    if (!(gnssInfo->field & GNSS_NMEA_FIELD_TYPE_SATINVIEW)) {
        LBSLOGI(HDI_GNSS, "no satViewed");
        return GNSS_FAILURE;
    }
    const GnssSatelliteStatusInfo *satInfo = &gnssInfo->satelist.bds;
    FillGnssSatelliteStatusFromStatusInfo(satInfo, status, CONSTELLATION_CATEGORY_BEIDOU);

    satInfo = &gnssInfo->satelist.gps;
    FillGnssSatelliteStatusFromStatusInfo(satInfo, status, CONSTELLATION_CATEGORY_GPS);

    satInfo = &gnssInfo->satelist.glonass;
    FillGnssSatelliteStatusFromStatusInfo(satInfo, status, CONSTELLATION_CATEGORY_GLONASS);

    satInfo = &gnssInfo->satelist.galileo;
    FillGnssSatelliteStatusFromStatusInfo(satInfo, status, CONSTELLATION_CATEGORY_GALILEO);

    satInfo = &gnssInfo->satelist.qzss;
    FillGnssSatelliteStatusFromStatusInfo(satInfo, status, CONSTELLATION_CATEGORY_QZSS);

    return GNSS_SUCCESS;
}