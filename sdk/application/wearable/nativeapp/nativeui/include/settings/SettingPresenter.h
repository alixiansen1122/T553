/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingPresenter
 * Create: 2025-06-01
 */
#ifndef SETTING_PRESENTER_H
#define SETTING_PRESENTER_H

#include "Presenter.h"

namespace OHOS {
class SettingView;
class SettingPresenter : public Presenter<SettingView> {
public:
    virtual ~SettingPresenter();
};
}
#endif // SETTING_PRESENTER_H