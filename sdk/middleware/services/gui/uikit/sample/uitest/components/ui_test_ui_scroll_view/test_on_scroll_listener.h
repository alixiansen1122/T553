/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: NATIVE TEST CASE
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */
#ifndef TEST_ON_SCROLL_LISTENER_H
#define TEST_ON_SCROLL_LISTENER_H

#include "ui_test_ui_scroll_view.h"
namespace OHOS {

class TestOnScrollListener : public UIScrollView::OnScrollListener {
    void OnScrollStart() override;

    void OnScrollEnd() override;
};
}

#endif // TEST_ON_SCROLL_LISTENER_H