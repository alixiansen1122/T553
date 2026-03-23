/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay bind view.
 * Author:
 * Create:
 */

#ifndef ALIPAY_BIND_H
#define ALIPAY_BIND_H

#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "alipay/AlipayPresenter.h"
#include "components/ui_qrcode.h"

namespace OHOS {
class AlipayBind : public UIViewGroup {
public:
    explicit AlipayBind(AlipayPresenter *presenter);
    ~AlipayBind() override;
    bool InitView();

private:
    bool AlipayInitImage(void);
    bool AlipayInitLabel(void);
    AlipayPresenter *presenter_;
    UILabel *labelBind { nullptr };
    UIQrcode *qrcodeGroup { nullptr };
};
}
#endif