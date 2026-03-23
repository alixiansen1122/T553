/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: TimerModel
 * Create: 2025-06-09
 */

#ifndef TIMER_MODEL_H
#define TIMER_MODEL_H

#include <sys/time.h>
#include <cstdint>
#include <map>
#include <mutex>
#include "AppViewIDs.h"
#include "UiConfig.h"
#include "TimerPresenter.h"
#include "graphic_timer.h"
#include "ohos_timer.h"

namespace OHOS {
class TimerModel {
public:
    static TimerModel &GetInstance(void);

    void SetTotalTime(uint64_t time);  // 设置计时器的总时间
    uint64_t GetTotalTime();           // 获取计时器的总时间
    bool GetIsShowFragment()
    {
        return isShowFragment_;
    }
    void SetIsShowFragment(bool value)
    {
        isShowFragment_ = value;
    }
    bool GetIsCountDown()
    {
        return isCountDown_;
    }
    void SetIsCountDown(bool value)
    {
        isCountDown_ = value;
    }
    bool GetIsPause()
    {
        return isPause_;
    }
    void SetIsPause(bool value)
    {
        isPause_ = value;
    }
    uint64_t GetStartTime()
    {
        return startTime_;
    };
    void SetStartTime(uint64_t value)
    {
        startTime_ = value;
    };
    uint64_t GetEclipseTime();
    void SetEclipseTime(bool flag, uint64_t value);

    void InitTimer();
    void DisableTimer();
    bool GetExistTimer()
    {
        return haveTimer_;
    };
    void SetExistTimer(bool value)
    {
        haveTimer_ = value;
    };

private:
    TimerModel();
    virtual ~TimerModel();
    TimerModel(const TimerModel &) = delete;
    TimerModel &operator=(const TimerModel &) = delete;
    uint64_t startTime_;
    uint64_t eclipseTime_;
    uint64_t totalTime_ = 0;
    GraphicTimer *startCounting_ = nullptr;  // 定时器
    bool haveTimer_ = false;
    bool isCountDown_ = false;
    bool isPause_ = false;
    bool isShowFragment_ = false;
};
}

#endif // TIMER_MODEL_H