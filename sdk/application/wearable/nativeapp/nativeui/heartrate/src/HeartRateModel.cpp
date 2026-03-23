/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: HeartRateModel
 * Create: 2025-06
 */

#include <sys/time.h>
#include "ohos_timer.h"
#include "wearable_log.h"
#include "heartrate/HeartRateView.h"
#include "heartrate/HeartRateModel.h"

namespace OHOS {
HeartRateModel::HeartRateModel() {}

HeartRateModel::~HeartRateModel() {}

HeartRateModel &HeartRateModel::GetInstance()
{
    static HeartRateModel instance;
    return instance;
}
}