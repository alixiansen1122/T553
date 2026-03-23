/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay trans tips view.
 * Author:
 * Create:
 */

#ifndef ALIPAY_TRANS_TIPS_H
#define ALIPAY_TRANS_TIPS_H

#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "alipay/AlipayPresenter.h"

namespace OHOS {
class AlipayTransTips : public UIViewGroup {
public:
    explicit AlipayTransTips(AlipayPresenter *presenter);
    ~AlipayTransTips() override;
    bool InitTipsView(uint32_t index);
    void AlipayTransRefreshTips(void);
private:
    bool AlipayInitLabel(const char *resStr);
    bool AlipayInitImage(const char *imageSrc);
    bool AlipayInitButton(uint32_t index);
    AlipayPresenter *presenter_;
    UILabel *alipayLabel { nullptr };
    UIImageView *alipayImage { nullptr };
    UILabelButton *alipayButton { nullptr };
};
    
}
#endif