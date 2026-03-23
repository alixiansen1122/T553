/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay bind result view.
 * Author:
 * Create:
 */

#ifndef ALIPAY_BIND_RES_H
#define ALIPAY_BIND_RES_H

#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "alipay/AlipayPresenter.h"

namespace OHOS {
class AlipayBindRes : public UIViewGroup {
public:
    explicit AlipayBindRes(AlipayPresenter *presenter);
    ~AlipayBindRes() override;
    bool InitView();
    void AlipayRefreshBindRes(void);

private:
    AlipayPresenter *presenter_;
    bool AlipayInitLabel(const char *resStr);
    bool AlipayInitImage(const char *imageSrc);
    UILabel *alipayLabel { nullptr };
    UIImageView *alipayImage { nullptr };
};
}
#endif