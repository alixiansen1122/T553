/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef LOTT_NULL_LAYER_H
#define LOTT_NULL_LAYER_H

#include "lottie/lott_layer.h"

namespace OHOS {

class LottNullLayer : public LottLayer {
public:
    LottNullLayer()
    {
        type_ = LottLayerType::NULL_LAYER;
    }
};
}
#endif // LOTT_NULL_LAYER_H