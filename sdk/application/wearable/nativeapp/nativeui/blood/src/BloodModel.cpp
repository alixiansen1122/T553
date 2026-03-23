/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BloodModel
 * Create: 2025-03-23
 */

#include <sys/time.h>
#include "ohos_timer.h"
#include "wearable_log.h"
#include "blood/BloodView.h"
#include "blood/BloodModel.h"

namespace OHOS {
BloodModel::BloodModel() {}

BloodModel::~BloodModel() {}

BloodModel &BloodModel::GetInstance()
{
    static BloodModel instance;
    return instance;
}

}
