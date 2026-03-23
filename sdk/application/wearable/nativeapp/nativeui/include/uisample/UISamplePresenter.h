/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TestCase
 * Author: Hisi Graphic Team
 * Created: 2025-7
 */

#ifndef UISAMPLE_PRESENTER_H
#define UISAMPLE_PRESENTER_H

#include "Presenter.h"
#include "components/ui_view.h"

namespace OHOS {
class UISampleView;
class UISampleModel;
class UISamplePresenter : public Presenter<UISampleView> {
public:
    UISamplePresenter();
    ~UISamplePresenter() override;

    void OnResume() override;
    void OnPause() override;

private:
    UISampleModel *uisampleModel_;
};
}

#endif // UISAMPLE_PRESENTER_H