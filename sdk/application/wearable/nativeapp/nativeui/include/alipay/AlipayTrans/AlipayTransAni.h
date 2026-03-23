/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay bind animate view.
 * Author:
 * Create:
 */

#ifndef ALIPAY_TRANS_ANI_H
#define ALIPAY_TRANS_ANI_H

#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_image_animator.h"
#include "alipay/AlipayPresenter.h"

#define ALIPAY_TRANS_SEARCH_IMAGE_NUM 8

namespace OHOS {
class AlipayTransAni : public UIViewGroup {
public:
    explicit AlipayTransAni(AlipayPresenter *presenter);
    ~AlipayTransAni() override;
    bool InitView();

private:
    bool AlipayInitLabel(void);
    bool AlipayInitImageAni(void);
    AlipayPresenter *presenter_;
    UILabel *alipayLabel { nullptr };
    UIImageAnimatorView *alipayImageAni { nullptr };
    ImageAnimatorInfo searchImageInfo[ALIPAY_TRANS_SEARCH_IMAGE_NUM];
};
}
#endif