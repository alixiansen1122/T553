/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: timer src fie.
 * Author: CompanyName
 * Create:
 */

#include "ohos_timer.h"

uint32_t GetOSTick(uint32_t ms)
{
#if !defined(_WIN32)
    return osMs2Tick(ms);
#else
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    double tickPerMs = (double)frequency.QuadPart / 1000.0;
    double tick = (double)ms * tickPerMs;

    return (uint32_t)tick;
#endif
}

