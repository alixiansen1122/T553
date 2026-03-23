/**
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: appmanger_module module.
 * Author: CompanyName
 * Create: 2025-09-04
 */
#include <securec.h>
#include <cstdlib>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <cerrno>
#include <cstdio>
#include "ace_mem_base.h"
#include "memory_heap.h"
#include "ace_log.h"
#include "calendar_timer.h"

namespace OHOS {
namespace ACELite {

// Timer callback function
static void TimerCallback(union sigval v)
{
    struct CalendarTimer *timer = (struct CalendarTimer *)v.sival_ptr;
    if (timer != nullptr && timer->cb != nullptr) {
        timer->cb(timer->arg);
    }
}

CalendarTimer *CalendarTimerInit(int32_t periodMs, CalendarTimerCb cb, void *arg, bool isPeriodic)
{
    CalendarTimer *timer = CalendarTimerCreate(periodMs, cb, arg, isPeriodic);
    if (timer) {
        int ret = CalendarTimerSetTime(timer, periodMs, true);
        if (ret != 0) {
            CalendarTimerDelete(timer);
            return nullptr;
        }
    } else {
        return nullptr;
    }
    int ret = CalendarTimerStart(timer);
    if (ret != 0) {
        CalendarTimerDelete(timer);
        return nullptr;
    }
    return timer;
}

CalendarTimer *CalendarTimerCreate(int32_t periodMs, CalendarTimerCb cb, void *arg, bool isPeriodic)
{
    // Parameter validation
    if (periodMs <= 0 || periodMs > MAX_PERIOD_MS || cb == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Timer create failed, invalid parameters. period=%d", periodMs);
        return nullptr;
    }

    // Allocate timer structure
    struct CalendarTimer *timer = (struct CalendarTimer *)malloc(sizeof(struct CalendarTimer));
    if (timer == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Timer create failed, out of memory.");
        return nullptr;
    }

    // Initialize timer structure
    memset_s(timer, sizeof(struct CalendarTimer), 0, sizeof(struct CalendarTimer));
    timer->cb = cb;
    timer->arg = arg;
    timer->periodMs = periodMs;
    timer->isPeriodic = isPeriodic;

    // Create system timer
    struct sigevent sev;
    memset_s(&sev, sizeof(sev), 0, sizeof(sev));
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = TimerCallback;
    sev.sigev_value.sival_ptr = timer;

    if (timer_create(CLOCK_REALTIME, &sev, &timer->timerId) == -1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Timer create failed. err=%s", strerror(errno));
        free(timer);
        return nullptr;
    }

    return timer;
}

int CalendarTimerSetTime(CalendarTimer *timer, int32_t periodMs, bool isPeriodic)
{
    if (timer == nullptr || periodMs <= 0 || periodMs > MAX_PERIOD_MS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Timer set time failed, invalid parameters.");
        return -1;
    }

    timer->periodMs = periodMs;
    timer->isPeriodic = isPeriodic;
    return 0;
}

int CalendarTimerStart(CalendarTimer *timer)
{
    if (timer == nullptr || timer->periodMs <= 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Timer start failed, invalid timer.\n");
        return -1;
    }

    struct itimerspec its;
    its.it_value.tv_sec = timer->periodMs / MS_PER_SECOND;
    its.it_value.tv_nsec = (timer->periodMs % MS_PER_SECOND) * NS_PER_MS;

    if (timer->isPeriodic) {
        its.it_interval.tv_sec = its.it_value.tv_sec;
        its.it_interval.tv_nsec = its.it_value.tv_nsec;
    } else {
        its.it_interval.tv_sec = 0;
        its.it_interval.tv_nsec = 0;
    }

    if (timer_settime(timer->timerId, 0, &its, nullptr) == -1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Timer start failed. err=%s\n", strerror(errno));
        return -1;
    }

    return 0;
}

int CalendarTimerStop(CalendarTimer *timer)
{
    if (timer == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Timer stop failed, invalid timer.\n");
        return -1;
    }

    struct itimerspec its;
    memset_s(&its, sizeof(its), 0, sizeof(its));

    if (timer_settime(timer->timerId, 0, &its, nullptr) == -1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Timer stop failed. err=%s\n", strerror(errno));
        return -1;
    }

    return 0;
}

int CalendarTimerDelete(CalendarTimer *timer)
{
    if (timer == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Timer delete failed, invalid timer.\n");
        return -1;
    }

    // Stop the timer first
    CalendarTimerStop(timer);

    // Delete system timer
    if (timer_delete(timer->timerId) == -1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Timer delete failed. err=%s\n", strerror(errno));
        free(timer);
        return -1;
    }

    // Free timer structure
    free(timer);
    return 0;
}
}  // namespace ACELite
}  // namespace OHOS
