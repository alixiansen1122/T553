/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: FindPhoneModel
 * Create: 2025-03-23
 */

#include "ohos_timer.h"
#include "wearable_log.h"
#include "findphone/FindPhoneView.h"
#include "findphone/FindPhoneModel.h"

namespace OHOS {
FindPhoneModel::FindPhoneModel() {}

FindPhoneModel::~FindPhoneModel() {}

FindPhoneModel &FindPhoneModel::GetInstance()
{
    static FindPhoneModel instance;
    return instance;
}
}
