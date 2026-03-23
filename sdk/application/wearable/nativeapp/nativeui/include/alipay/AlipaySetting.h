/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay setting view.
 * Author:
 * Create:
 */

#ifndef ALIPAY_SETTING_H
#define ALIPAY_SETTING_H

#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "alipay/AlipayPresenter.h"
#include "components/ui_qrcode.h"

namespace OHOS {
class AlipaySetting : public UIViewGroup {
public:
    explicit AlipaySetting(AlipayPresenter *presenter);
    ~AlipaySetting() override;
    bool InitView();

private:
    bool AlipayInitButton();
    bool AlipayInitLabel(void);
    AlipayPresenter *presenter_;
    UILabel *labelTitle { nullptr };
    UILabel *labelName { nullptr };
    UILabel *labelId { nullptr };
    UILabelButton *buttonSetting { nullptr };
};
}
#endif