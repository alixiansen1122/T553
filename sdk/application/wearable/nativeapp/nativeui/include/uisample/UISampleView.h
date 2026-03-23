/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TestCase
 * Author: Hisi Graphic Team
 * Created: 2025-7
 */

#ifndef UISAMPLE_VIEW_H
#define UISAMPLE_VIEW_H

#include "View.h"

namespace OHOS {
enum SAMPLE_PAGES {
    SAMPLE_MAIN_PAGE = 1,
    SAMPLE_PROPRIETARY_TESTCASE_PAGE = 2,
    SAMPLE_NATIVE_TESTCASE_PAGE = 3,
};

class UISamplePresenter;
class UISampleView : public View<UISamplePresenter> {
};
}
#endif // UISAMPLE_VIEW_H
