/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay get bindstr fail view.
 * Author:
 * Create:
 */

#ifndef ALIPAY_REFRESH_BIND_CODE_H
#define ALIPAY_REFRESH_BIND_CODE_H

#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "alipay/AlipayPresenter.h"
#include "components/ui_qrcode.h"

namespace OHOS {
class AlipayRefreshBind : public UIViewGroup {
public:
    AlipayRefreshBind();
    ~AlipayRefreshBind() override;
    bool InitView();

private:
    bool AlipayInitImage(void);
    bool AlipayInitLabel(void);
    bool AlipayInitButton();
    UILabel *labelBind { nullptr };
    UIImageView *imageBind[ALIPAY_SET_THREE] { nullptr };
    UILabelButton *buttonBind { nullptr };
    UILabelButton *backgroudButton { nullptr };
    UIQrcode *qrcodeGroup { nullptr };
};
}
#endif