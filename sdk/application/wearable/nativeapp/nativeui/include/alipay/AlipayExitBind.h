/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay binding process exit binding view.
 * Author:
 * Create:
 */

#ifndef ALIPAY_EXIT_BIND_H
#define ALIPAY_EXIT_BIND_H

#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "alipay/AlipayPresenter.h"

namespace OHOS {
class AlipayExitBind : public UIViewGroup {
public:
    explicit AlipayExitBind(AlipayPresenter *presenter);
    ~AlipayExitBind() override;
    bool InitView();

private:
    bool AlipayInitLabel();
    bool AlipayInitButton();
    bool InitSimilarButton(int16 num, int16 alipayX, int16 alipayY, const char *buttonStr, const char *viewId);
    AlipayPresenter *presenter_;
    UILabel *alipayLabel[ALIPAY_SET_TWO] { nullptr };
    UILabelButton *alipayButton[ALIPAY_SET_TWO] { nullptr };
};
}
#endif