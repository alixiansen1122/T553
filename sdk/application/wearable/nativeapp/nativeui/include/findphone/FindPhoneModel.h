/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: FindPhoneModel
 * Create: 2025-03-23
 */

#ifndef FINDPHONE_MODEL_H
#define FINDPHONE_MODEL_H

#include <sys/time.h>
#include <list>
#include <string>
#include "graphic_timer.h"

namespace OHOS {
class FindPhoneModel {
public:
    static FindPhoneModel &GetInstance(void);

private:
    FindPhoneModel();
    FindPhoneModel(const FindPhoneModel &);
    FindPhoneModel &operator=(const FindPhoneModel &);
    virtual ~FindPhoneModel();
};
}

#endif // FINDPHONE_MODEL_H