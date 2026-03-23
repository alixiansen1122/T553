/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TestCase
 * Author: Hisi Graphic Team
 * Created: 2025-7
 */

#include "uisample/UISampleModel.h"

namespace OHOS {
    UISampleModel::UISampleModel() {}

    UISampleModel::~UISampleModel() {}

    UISampleModel &UISampleModel::GetInstance()
    {
        static UISampleModel instance;
        return instance;
    }

}
