/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: DialModelTime
 * Author:
 * Create: 2023-12
 */

#ifndef DIAL_MODEL_TIME_H
#define DIAL_MODEL_TIME_H

#include "main/dial/DialBaseModel.h"
#include "gfx_utils/list.h"

namespace OHOS {
class TimeChangeListener;
class DialModelTime : public DialBaseModel {
public:
    ~DialModelTime() override
    {
        listeners_.Clear();
    }

    static DialModelTime* GetInstance()
    {
        static DialModelTime instance;
        return &instance;
    }

    bool GetDialFloatData(DialDataType& type, float& out) override;
    void UpdateTime();
    void Register(TimeChangeListener* listener);
    void Unregister(TimeChangeListener* listener);

private:
    DialModelTime() = default;
    struct tm *tblock_;
    uint16_t angleH_ = 0;
    uint16_t angleM_ = 0;
    uint16_t angleS_ = 0;
    float angleMS_ = 0;
    List<TimeChangeListener*> listeners_;
};

class TimeChangeListener {
public:
    struct TimeInterval {
        uint8_t start;
        uint8_t end;
    };
    virtual void OnTimeUpdate(const struct tm &curTime) = 0;
    void SetTimeInervals(TimeInterval *intervals, uint8_t num);
    bool IsInTimeInterval(const struct tm &curTime);
    TimeChangeListener(const TimeChangeListener&) = delete;
    TimeChangeListener& operator=(const TimeChangeListener&) = delete;
    TimeChangeListener();
    virtual ~TimeChangeListener();
private:
    TimeInterval *intervals_ = nullptr;
    uint8_t num_ = 0;
};
} // namespace OHOS
#endif
