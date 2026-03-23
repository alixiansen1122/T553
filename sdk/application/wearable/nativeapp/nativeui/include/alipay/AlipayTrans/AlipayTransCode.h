/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay transcode view.
 * Author:
 * Create:
 */
#ifndef ALIPAY_TRANS_CODE_H
#define ALIPAY_TRANS_CODE_H

#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_button.h"
#include "components/ui_qrcode.h"
#include "alipay/AlipayPresenter.h"

namespace OHOS {
class AlipayTransCode : public UIViewGroup {
public:
    explicit AlipayTransCode(AlipayPresenter *presenter);
    ~AlipayTransCode() override;
    bool InitView();

private:
    bool AlipayInitImage(void);
    bool AlipayInitLabel(void);
    bool AlipayInitButton();
    bool AlipayInitQrcode();
    AlipayPresenter *presenter_;
    UILabel *labelPayCode { nullptr };
    UIQrcode *qrcodeGroup { nullptr };
    UILabelButton *buttonPayCode { nullptr };;
};
}
#endif