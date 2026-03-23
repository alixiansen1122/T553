/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay get trans list view.
 * Author:
 * Create:
 */

#ifndef ALIPAY_GET_TRANS_LIST_H
#define ALIPAY_GET_TRANS_LIST_H

#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "alipay/AlipayPresenter.h"

namespace OHOS {
class AlipayGetTransList : public UIViewGroup {
public:
    explicit AlipayGetTransList(AlipayPresenter *presenter);
    ~AlipayGetTransList() override;
    bool InitGetView(uint32_t index);
private:
    AlipayPresenter *presenter_;
    bool AlipayInitLabel(uint32_t index);
    bool AlipayInitButton(uint32_t index);
    bool InitSimilarButton(int16 num, int16 alipayX, int16 alipayY, const char *buttonStr, const char *viewId);
    UILabel *alipayLabel { nullptr };
    UILabelButton *alipayButton[ALIPAY_SET_TWO] { nullptr };
};
}
#endif