/**
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: appmanger_module module.
 * Author: CompanyName
 * Create: 2025-09-04
 */
#ifndef CALENDAR_TIMER_H
#define CALENDAR_TIMER_H

#include <stdint.h>
#include <ctime>
#include <stdbool.h>
#include "sys/time.h"

#define MAX_PERIOD_MS 86400000  // Maximum period is 24 hours
#define MS_PER_SECOND 1000
#define NS_PER_MS 1000000

static constexpr int16_t SEC_TO_MILLI = 1000;
static constexpr int16_t USEC_TO_MILLI = 1000;
static constexpr int16_t MIN_TO_HOUR = 60;
static constexpr int16_t HOURLY_SYSTEM = 24;

namespace OHOS {
namespace ACELite {

// Timer callback function type
typedef void (*CalendarTimerCb)(void *arg);

// Timer structure definition
struct CalendarTimer {
    timer_t timerId;     // System timer ID
    CalendarTimerCb cb;  // Callback function
    void *arg;           // Argument for callback function
    int32_t periodMs;    // Period time (in milliseconds)
    bool isPeriodic;     // Whether it's a periodic timer
};

// Timer handle structure (opaque type, users don't need to know the internal implementation)
typedef struct CalendarTimer CalendarTimer;

CalendarTimer *CalendarTimerInit(int32_t periodMs, CalendarTimerCb cb, void *arg, bool isPeriodic);

/**
 * Create a timer
 * @param periodMs Period time in milliseconds. For one-shot timers, this is the first trigger time
 * @param cb Timer callback function
 * @param arg Argument passed to the callback function
 * @param isPeriodic Whether it's a periodic timer
 * @return Returns timer handle on success, nullptr on failure
 */
CalendarTimer *CalendarTimerCreate(int32_t periodMs, CalendarTimerCb cb, void *arg, bool isPeriodic);

/**
 * Set the timer's time
 * @param timer Timer handle
 * @param periodMs Period time in milliseconds
 * @param isPeriodic Whether it's a periodic timer
 * @return Returns 0 on success, -1 on failure
 */
int CalendarTimerSetTime(CalendarTimer *timer, int32_t periodMs, bool isPeriodic);

/**
 * Start the timer
 * @param timer Timer handle
 * @return Returns 0 on success, -1 on failure
 */
int CalendarTimerStart(CalendarTimer *timer);

/**
 * Stop the timer
 * @param timer Timer handle
 * @return Returns 0 on success, -1 on failure
 */
int CalendarTimerStop(CalendarTimer *timer);

/**
 * Delete the timer
 * @param timer Timer handle
 * @return Returns 0 on success, -1 on failure
 */
int CalendarTimerDelete(CalendarTimer *timer);

}  // namespace ACELite
}  // namespace OHOS

#endif  // CALENDAR_TIMER_H
