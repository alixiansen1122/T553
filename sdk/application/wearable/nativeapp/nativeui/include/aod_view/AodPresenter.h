/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: AodPresenter
 * Author:
 * Create: 2024-10-30
 */

#ifndef AODPRESENTER_H
#define AODPRESENTER_H
#include "Presenter.h"
#include "aod_view/AodView.h"
#include "components/ui_view.h"
#include "key/KeyInputListener.h"
#include "common/task.h"

namespace OHOS {
class AodView;
class AodModel;
class AodPresenter : public Presenter<AodView>, public Task {
public:
    AodPresenter() {}
    ~AodPresenter() override {}
    AodPresenter(const AodPresenter &) = delete;
    AodPresenter &operator=(const AodPresenter &) = delete;
    void OnResume() override;
    void OnPause() override;
    void Callback() override;
}; // AodPresenter
} // OHOS

#endif // AODPRESENTER_H