/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay trans action res view.
 * Author:
 * Create:
 */

#ifndef ALIPAY_TRANS_RES_H
#define ALIPAY_TRANS_RES_H

#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "alipay/AlipayPresenter.h"

namespace OHOS {
class AlipayTransRes : public UIViewGroup {
public:
    explicit AlipayTransRes(AlipayPresenter *presenter);
    ~AlipayTransRes() override;
    bool InitResView(uint32_t index);
    void AlipayTransRefreshRes(void);
private:
    bool AlipayInitLabel(const char *resStr);
    bool AlipayInitImage(const char *imageSrc);
    bool AlipayInitButton(uint32_t index);
    AlipayPresenter *presenter_;
    UILabel *alipayLabel { nullptr };
    UIImageView *alipayImage { nullptr };
    UILabelButton *alipayButton { nullptr };
    UILabelButton *alipayButtonSec { nullptr };
};
    
}
#endif