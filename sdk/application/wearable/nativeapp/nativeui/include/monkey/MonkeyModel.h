/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: MonkeyModel
 * Create: 2025-04-24
 */
#ifndef MONKEY_MODEL_H
#define MONKEY_MODEL_H

#include <iostream>
#include <cstdint>
#include <map>
#include <vector>
#include <string>
#include <sys/time.h>
#include "AppViewIDs.h"
#include "UiConfig.h"
#include "animator.h"

namespace OHOS {
typedef struct {
    uint32_t sliceId;
    uint32_t pageId;
} SlicePageId;

class MonkeyModel {
public:
    static MonkeyModel &GetInstance(void);
    void SetStart(bool start);

private:
    MonkeyModel();
    MonkeyModel(const MonkeyModel &);
    MonkeyModel &operator=(const MonkeyModel &);
    virtual ~MonkeyModel();
    class TimeAnimatorCallback : public AnimatorCallback {
    public:
        TimeAnimatorCallback(uint32_t periol) : periol_(periol) {}
        virtual ~TimeAnimatorCallback() override {}
        void Callback(UIView* view) override;
    private:
        uint32_t periol_;
    };
    TimeAnimatorCallback* timeCallback_ = nullptr;
    Animator* timeAnimator_ = nullptr;
};
}

#endif  // MONKEY_MODEL_H