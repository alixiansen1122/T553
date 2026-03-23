/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay trans enable view.
 * Author:
 * Create:
 */
#ifndef ALIPAY_TRANS_ENABLE_H
#define ALIPAY_TRANS_ENABLE_H

#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_button.h"
#include "components/ui_qrcode.h"
#include "alipay/AlipayPresenter.h"

namespace OHOS {
class AlipayTransEnable : public UIViewGroup {
public:
    explicit AlipayTransEnable(AlipayPresenter *presenter);
    ~AlipayTransEnable() override;
    bool InitView();

private:
    bool AlipayInitLabel(void);
    bool AlipayInitQrcode();
    AlipayPresenter *presenter_;
    UILabel *labelEnableCode { nullptr };
    UIQrcode *qrcodeGroup { nullptr };
    UILabel *labelTips { nullptr };;
};
}
#endif