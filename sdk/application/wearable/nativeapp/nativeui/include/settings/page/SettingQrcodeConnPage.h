/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingQrcodeConnPage
 * Created: 2025-06-05
 */
#ifndef SETTING_QRCODE_CONN_PAGE_H
#define SETTING_QRCODE_CONN_PAGE_H

#include <string>
#include "View.h"
#include "SlicePage.h"
#include "components/root_view.h"
#include "components/ui_view_group.h"
#include "components/ui_scroll_view_nested.h"
#include "components/ui_label.h"
#include "UiConfig.h"
#include "components/ui_view_group.h"
#include "components/ui_label.h"
#include "components/ui_button.h"
#include "components/ui_label_button.h"
#include "components/ui_image_view.h"
#include "components/ui_qrcode.h"
#include "components/ui_label.h"
#include "settings/SettingPresenter.h"

namespace OHOS {

class SettingQrcodeConnPage : public SlicePage<SettingPresenter>,
                              public UIView::OnDragListener {
public:
    SettingQrcodeConnPage();
    ~SettingQrcodeConnPage() override;
    void OnStart(void* data) override;
    void InitView();
private:
    UIScrollViewNested *group_ = nullptr;
    UIQrcode *qrcode_ = nullptr;
    UILabel *content_ = nullptr;
};
}
#endif // SETTING_QRCODE_CONN_PAGE_H
