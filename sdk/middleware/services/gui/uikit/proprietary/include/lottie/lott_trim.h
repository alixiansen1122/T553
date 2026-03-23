/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-7
 */

#ifndef LOTT_TRIM_H
#define LOTT_TRIM_H

#include "lottie/lott_property.h"
#include "lottie/lott_trim_path.h"
#include "display_vgu.h"
#include "gfx_utils/graphic_math.h"
#include "gfx_utils/heap_base.h"

namespace OHOS {

enum class LottTrimType {
    Simultaneous,
    Individual,
};

struct LottTrimValue {
    bool enable;
    float start;
    float end;
    float offset;
    LottTrimType type;
};

class LottTrim : public HeapBase {
public:
    LottTrim();
    virtual ~LottTrim();

    LottFloatProp start_;
    LottFloatProp end_;
    LottFloatProp offset_;
    LottTrimValue trimValue_;
    void Update(float frameNo);
};
}
#endif // LOTT_TRIM_H