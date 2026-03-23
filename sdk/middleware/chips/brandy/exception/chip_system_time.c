/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: get time
 * Author:
 * Create:
 */
#ifndef BUILD_APPLICATION_SSB

#include "chip_system_time.h"

#define SECONDS_PER_MINUTE 60 /* second per minute */
#define MINUTES_PER_HOUR 60   /* second per minute */
#define MSEC_PER_SEC 1000
#define USEC_PER_MSEC 1000
#define TZ_ADDRESS 0x5702C014
#define MAX_TIMEZONE_HOURS 24
#define TIME_MAX 64
#define ONE_DAY_MIN 1440
#define BEIJING_TIMEZONE 480
#define MIN_INTERVAL 15
#define CUSTOM_PER_HOUR (60 / MIN_INTERVAL)

void print_time(calendar_t *date)
{
    printf("CALLENDAR:year = %d, month = %d, day = %d, hour = %d, minute = %d, second = %d \r\n",
           date->year,
           date->mon,
           date->day,
           date->hour,
           date->min,
           date->sec);
}

void print_time_2(struct tm *date)
{
    printf("TM:year = %d, month = %d, day = %d, hour = %d, minute = %d, second = %d \r\n",
           date->tm_year,
           date->tm_mon,
           date->tm_mday,
           date->tm_hour,
           date->tm_min,
           date->tm_sec);
}

static uint16_t get_timezone(void)
{
    return readw(TZ_ADDRESS) & 0x00FF;
}

static void set_timezone(uint16_t timezone)
{
    uint16_t tz_zone = readw(TZ_ADDRESS) & 0xFF00;
    tz_zone = tz_zone | timezone;
    writew(TZ_ADDRESS, tz_zone);
}

int get_timezone_min(void)
{
    uint16_t sdk_zone = get_timezone();
    if (sdk_zone > (CUSTOM_PER_HOUR * MAX_TIMEZONE_HOURS)) {
        return BEIJING_TIMEZONE;
    }
    int zone = 0;
    // 东边 1-12H, 西边 13-24H
    if (sdk_zone > (12 * CUSTOM_PER_HOUR)) { // 西边 13-24H
        zone = (sdk_zone - (12 * CUSTOM_PER_HOUR)) * MIN_INTERVAL * (-1);
    } else { // 东边 1-12H
        zone = sdk_zone * MIN_INTERVAL;
    }
    return zone;
}

void save_time(void)
{
    uint64_t time_sec = get_local_timestamp();
    time_sec = time_sec * MSEC_PER_SEC;
    uapi_calendar_set_timestamp(time_sec);
}

uint32_t get_current_timestamp(void) // 0时区timestamp
{
    uint32_t timestamp = 0;
    struct timeval current_time = {0};
    if (gettimeofday(&current_time, NULL)) {
        return 0;
    }
    struct tm tm;
    localtime_r(&current_time.tv_sec, &tm);
    timestamp = (uint32_t)current_time.tv_sec;
    return timestamp;
}

uint64_t get_current_timestamp_us(void) // 0时区timestamp_us
{
    uint64_t timestamp = 0;
    struct timeval current_time = {0};
    if (gettimeofday(&current_time, NULL)) {
        return 0;
    }
    struct tm tm;
    localtime_r(&current_time.tv_sec, &tm);
    timestamp = (uint32_t)current_time.tv_sec*1000+current_time.tv_usec;
    return timestamp;
}

uint32_t get_local_timestamp(void) // 本地时区timestamp
{
    uint32_t timestamp = get_current_timestamp() + get_timezone_min() * SECONDS_PER_MINUTE;
    return timestamp;
}

void get_local_time(struct tm *tm)
{
    uint32_t timestamp = get_current_timestamp();
    localtime_r(&timestamp, tm);
    tm->tm_year = tm->tm_year + 1900, // 1900年
    tm->tm_mon = tm->tm_mon + 1;
}

void get_current_time(struct tm *tm)
{
    uint32_t timestamp = get_current_timestamp() - (get_timezone_min() * 60);
    localtime_r(&timestamp, tm);
    tm->tm_year = tm->tm_year + 1900, // 1900年
    tm->tm_mon = tm->tm_mon + 1;
}

void restore_time(void)
{
    calendar_t date;
    uapi_calendar_get_datetime(&date);
    print_time(&date);
    uint64_t sec;
    uapi_calendar_get_timestamp(&sec);
    printf("callendar timestamp = %lld \r\n", sec);
    sec = sec / MSEC_PER_SEC;
    // 东边 1-12H, 西边 13-24H, 需在这里转换成 -1到-12
    uint16_t timezone = get_timezone();
    int timezone_trans = timezone > (12 * CUSTOM_PER_HOUR) ? \
        ((timezone - (12 * CUSTOM_PER_HOUR)) * (-1)) : timezone;
    timezone_trans *= MIN_INTERVAL;
    sync_time(sec, timezone_trans);
}

uint32_t sync_time(uint32_t timestamp, int timezone)
{
    printf("sync time timestamp %d, timezone %d \r\n", timestamp, timezone);
    uint32_t seconds;
    if (timezone > ONE_DAY_MIN) {
        return ERRCODE_FAIL;
    }
    if (timestamp == 0) {
        return ERRCODE_FAIL;
    } else {
        seconds = timestamp - (timezone * SECONDS_PER_MINUTE);
    }

    // 东边1-12H, 西边13-24H
    uint32_t timezone_trans = (timezone / MIN_INTERVAL) < 0 ? \
        abs(timezone / MIN_INTERVAL) + (12 * CUSTOM_PER_HOUR) : (timezone / MIN_INTERVAL);
    set_timezone(timezone_trans);
    struct timeval tv = {0};
    struct timezone tzone = {0};
    tv.tv_sec = seconds;
    tv.tv_usec = 0;
    tzone.tz_dsttime = 0;
    tzone.tz_minuteswest = timezone;
    return settimeofday(&tv, &tzone);
}

void test_bandy_time(void)
{
    struct tm tm;
    uint32_t time = 1716044461; // 2024-5-18 15:1:1
    printf("--------SYNC TIME----------\r\n");
    get_current_timestamp();
    get_current_time(&tm);
    sync_time(time, 480);
    get_current_timestamp();
    get_current_time(&tm);
    printf("--------SAVE TIME----------\r\n");
    get_current_timestamp();
    get_current_time(&tm);
    save_time();
    get_current_timestamp();
    get_current_time(&tm);
    printf("--------RESTORE TIME----------\r\n");
    get_current_timestamp();
    get_current_time(&tm);
    restore_time();
    get_current_timestamp();
    get_current_time(&tm);
}
#endif
