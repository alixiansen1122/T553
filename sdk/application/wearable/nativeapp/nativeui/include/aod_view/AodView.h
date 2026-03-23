/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: AodView
 * Author:
 * Create: 2024-10-30
 */

#ifndef AODVIEW_H
#define AODVIEW_H
#include "View.h"
#include "main/MainModel.h"
#include "main/DialBinTypesV2.h"
#include "components/ui_view.h"
#include "components/ui_card_page.h"
#include "components/ui_fragment.h"

namespace OHOS {
class AodPresenter;
class AodView : public View<AodPresenter> {
public:
    AodView() {}
    ~AodView();
    AodView(const AodView &) = delete;
    AodView& operator=(const AodView &) = delete;

    void OnStart() override;
    void OnStop() override;
    void Update();

private:
    UIViewGroup *InitAodDial(DialSetting &setting);
    UIViewGroup *InitAodOffDial(DialSetting &setting);
    UIViewGroup *InitDefaultAod();
    bool isLoaded_ = false;

    UIViewGroup *clockDial_{nullptr};
    UIFragment *clockDialFragment_{nullptr};
};
} // OHOS
#endif // AODVIEW_H