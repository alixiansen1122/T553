/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: GraphicCondExt
 * Author: Hisi Graphic Team
 * Created: 2025-8
 */

#include "common/graphic_cond_ext.h"

namespace OHOS {
bool GraphicCondExt::WaitTime(GraphicMutex &mutex, uint32_t time)
{
#ifdef _WIN32
        mutex.Unlock();
        WaitForSingleObject(cond_, time);
        ResetEvent(cond_);
        mutex.Lock();
#elif defined(__FREERTOS__)
        mutex.Unlock();
        osEventFlagsClear(cond_, 0x0001U);
        uint32_t ret = osEventFlagsWait(cond_, 0x0001U, osFlagsWaitAny, time);
        mutex.Lock();
        if ((ret == osFlagsErrorParameter) || (ret == osFlagsErrorResource) ||
            (ret == osFlagsErrorTimeout) || (ret == osFlagsErrorUnknown)) {
            return false;
        }
        return true;
#elif defined __linux__ || defined __LITEOS__
        pthread_mutex_t& lock = mutex.GetMutex();
        struct timespec t;
        t.tv_nsec = (time % 1000) * 1e6; // 1000: 1s = 1000ms
        t.tv_sec = time / 1000; // 1000: 1s = 1000ms
        return (pthread_cond_timedwait(&cond_, &lock, &t) == 0);
#endif
}
} // namespace OHOS
