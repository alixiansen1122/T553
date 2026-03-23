/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TestCase
 * Author: Hisi Graphic Team
 * Created: 2025-7
 */

#ifndef UISAMPLE_MODEL_H
#define UISAMPLE_MODEL_H

#include <cstdint>
#include <map>
#include "AppViewIDs.h"
#include "UiConfig.h"
#include "uisample/UISamplePresenter.h"

namespace OHOS {
    class UISampleModel {
    public:
        static UISampleModel &GetInstance(void);

    private:
        UISampleModel();
        UISampleModel(const UISampleModel &);
        UISampleModel &operator=(const UISampleModel &);
        virtual ~UISampleModel();
    };
}

#endif // UISAMPLE_MODEL_H