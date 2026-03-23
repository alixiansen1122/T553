/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef LOTT_PATH_H
#define LOTT_PATH_H

#include "lottie/lott_shape.h"
#include "lottie/lott_property.h"

namespace OHOS {

class LottPath : public LottShape {
public:
    LottPath();
    virtual ~LottPath();

    bool Update(float frameNo) override;
    bool GetPathData(Path& path) override;

    LottPathDataProp pathData_;
};
}
#endif // LOTT_PATH_H