/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay help qrcode view.
 * Author:
 * Create:
 */

#ifndef ALIPAY_HELP_H
#define ALIPAY_HELP_H

#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "alipay/AlipayPresenter.h"
#include "components/ui_qrcode.h"

namespace OHOS {
class AlipayHelp : public UIViewGroup {
public:
    explicit AlipayHelp(AlipayPresenter *presenter);
    ~AlipayHelp() override;
    bool InitView();

private:
    bool AlipayInitImage(void);
    bool AlipayInitLabel(void);
    AlipayPresenter *presenter_;
    UILabel *labelHelp { nullptr };
    UIQrcode *qrcodeGroup { nullptr };
};
}
#endif