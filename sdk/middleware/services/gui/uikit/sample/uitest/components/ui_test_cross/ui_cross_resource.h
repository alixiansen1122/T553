/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: UiCrossView
 * Author:
 * Create: 2023-09
 */

#ifndef UI_CROSS_RESOURCE_H
#define UI_CROSS_RESOURCE_H
#include <cstdint>
namespace OHOS {
    constexpr const char *CROSS_IMG_PATH = RES_PATH"/cross.bin";
    const int DIR_MASK = 0xfff00000;
    const int PNG_MASK = 0x000fffff;
    const int CROSS_CAT = 0x100001;
    const int CROSS_DOVE = 0x100002;
    const int CROSS_DROP = 0x100003;
    const int CROSS_ROSE = 0x100004;
} // OHOS
#endif // UI_CROSS_RESOURCE_H