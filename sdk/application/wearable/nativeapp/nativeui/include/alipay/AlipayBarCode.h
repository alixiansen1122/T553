/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay barcode view.
 * Author:
 * Create:
 */

#ifndef ALIPAY_BARCODE_H
#define ALIPAY_BARCODE_H

#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_barcode.h"
#include "alipay/AlipayPresenter.h"

namespace OHOS {
class AlipayBarCode : public UIViewGroup {
public:
    explicit AlipayBarCode(AlipayPresenter *presenter);
    ~AlipayBarCode() override;
    static AlipayBarCode *GetInstance();
    bool InitView();
    void AlipayRefreshBarcode(const char *refreshStr);

private:
    bool AlipayInitImage(void);
    bool AlipayInitLabel(void);
    bool AlipayInitButton();
    bool AlipayInitBarcode();
    void AlipayBarcodeReformat(uint8_t *str, uint8_t *strBarcode);
    AlipayPresenter *presenter_;
    UILabel *labelBarCode { nullptr };
    UILabel *labelPay { nullptr };
    UILabelButton *buttonBarCode { nullptr };
    UIBarcode *alipayBarcode { nullptr };
    UIImageView *alipayImageGrey { nullptr };
    UIImageView *alipayImageWhite { nullptr };
};
}
#endif