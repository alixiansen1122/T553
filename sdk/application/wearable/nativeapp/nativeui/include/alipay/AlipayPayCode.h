/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay pay view.
 * Author:
 * Create:
 */
#ifndef ALIPAY_PAYCODE_H
#define ALIPAY_PAYCODE_H

#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_button.h"
#include "components/ui_qrcode.h"
#include "alipay/AlipayPresenter.h"

namespace OHOS {
class AlipayPayCode : public UIViewGroup {
public:
    explicit AlipayPayCode(AlipayPresenter *presenter);
    ~AlipayPayCode() override;
    static AlipayPayCode *GetInstance();
    bool InitView();
    void AlipayRefreshQrcode(const char *refreshStr);

private:
    AlipayPresenter *presenter_;
    bool AlipayInitImage(void);
    bool AlipayInitLabel(void);
    bool AlipayInitButton();
    bool AlipayInitQrcode();
    UILabel *labelPayCode { nullptr };
    UIQrcode *qrcodeGroup { nullptr };
    UILabelButton *buttonPayCode { nullptr };
    UIImageView *alipayImageGrey { nullptr };
    UIImageView *alipayImageWhite { nullptr };
};
}
#endif