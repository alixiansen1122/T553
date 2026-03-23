/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: GraphicCondExt
 * Author: Hisi Graphic Team
 * Created: 2025-8
 */

#include "graphic_mutex.h"

namespace OHOS {
class GraphicCondExt : public GraphicCond {
public:
    bool WaitTime(GraphicMutex &mutex, uint32_t time);
};
}  // namespace OHOS
