/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: time
 * Author:
 * Create:
 */
#ifndef CHIP_SYSTEM_TIME_H
#define CHIP_SYSTEM_TIME_H
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "calendar.h"
#include "chip_io.h"
#include "errcode.h"
#include "time.h"
#include "sys/time.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef BUILD_APPLICATION_SSB
/**
 * @brief  Print the time represented by the given calendar date
 * @return void.
 */
void print_time(calendar_t *date);

/**
 * @brief  Save the current system time to non-volatile memory
 * @return void.
 */
void save_time(void);

/**
 * @brief  Get the current system time as a Unix timestamp
 * @return void.
 */
uint32_t get_current_timestamp(void);
uint64_t get_current_timestamp_us(void);
/**
 * @brief  Get the current system time as a timeval struct and a tm
 * @return void.
 */
void get_current_time(struct tm *tm);

void get_local_time(struct tm *tm);

/**
 * @brief  Restore the system time from the saved non-volatile
 * @return void.
 */
void restore_time(void);

/**
 * @brief  get timezone minute
 * @return timezone minute.
 */
int get_timezone_min(void);

/**
 * @brief  Synchronize the system time with the given timestamp and timezone
 * @return void.
 */
uint32_t sync_time(uint32_t timestamp, int timezone);

/**
 * @brief  use for test
 * @return void.
 */
void test_bandy_time(void);

/**
 * @brief  Get the current system time as a local timestamp with timezone offset
 * @return void.
 */
uint32_t get_local_timestamp(void);
#endif

#ifdef __cplusplus
}
#endif
#endif
