/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef LOTT_SHAPE_H
#define LOTT_SHAPE_H

#include "common/graphic_hardware_types.h"
#include "hals/gralloc_engines.h"
#include "lottie/lott_trim_path.h"
#include "lottie/lott_trim.h"

namespace OHOS {
enum class LottShapeType {
    INVALID,
    RECT,
    ELLIPSE,
    PATH,
    SHAPE_GROUP,
};

class LottShape : public HeapBase {
public:
    LottShape();
    virtual ~LottShape();

    virtual void OnStop();
    virtual bool Update(float frameNo);
    virtual bool GetPathData(Path& path);
    LottShapeType GetType();
    bool ProcessTrim(const LottTrimValue& trimValue);
    LottTrimPath* trimPath_ = nullptr;
    Path trimmedPath_ = {0};
    bool isPathUpdated_ = true;
    void ResetUpdateState();

protected:
    friend class LottShapeGroup;
    void ClearPathInner();
    bool isFirstFrame_ = true;
    Path path_ = {0};
    LottShapeType type_ = LottShapeType::INVALID;
    bool isUpdated_ = false;
};
}
#endif // LOTT_SHAPE_H