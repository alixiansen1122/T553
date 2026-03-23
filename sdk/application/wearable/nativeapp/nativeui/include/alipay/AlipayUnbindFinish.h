/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay removie bind finish view.
 * Author:
 * Create:
 */

#ifndef ALIPAY_UNBIND_FINISH_CODE_H
#define ALIPAY_UNBIND_FINISH_CODE_H

#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "alipay/AlipayPresenter.h"
#include "components/ui_qrcode.h"

namespace OHOS {
class AlipayUnbindFinish : public UIViewGroup {
public:
    explicit AlipayUnbindFinish(AlipayPresenter *presenter);
    ~AlipayUnbindFinish() override;
    bool InitView();

private:
    bool AlipayInitImage(void);
    bool AlipayInitLabel(void);
    bool AlipayInitButton();
    AlipayPresenter *presenter_;
    UILabel *titleUnbind { nullptr };
    UILabel *textUnbind { nullptr };
    UILabelButton *buttonUnbind { nullptr };
};
}
#endif