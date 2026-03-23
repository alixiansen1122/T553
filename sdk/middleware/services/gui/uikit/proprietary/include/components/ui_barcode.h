/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: UIBarcode
 * Author:
 * Create: 2023-12
 */

/**
 * @addtogroup UI_Components
 * @{
 *
 * @brief Defines UI components such as buttons, texts, images, lists, and progress bars.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file ui_barcode.h
 *
 * @brief Declares the attributes and functions of the <b>UIBarcode</b> class.
 *
 * @since 3.0
 * @version 5.0
 */

#ifndef GRAPHIC_UI_BARCODE_H
#define GRAPHIC_UI_BARCODE_H

#include "components/ui_image_view.h"
namespace OHOS {
/**
 * @brief Provides functions related to quick response (Bar) codes.
 *
 * @since 3.0
 * @version 5.0
 */
class UIBarcode : public UIImageView {
public:
    /**
     * @brief A default constructor used to create a <b>UIBarcode</b> instance.
     *
     * @since 3.0
     * @version 5.0
     */
    UIBarcode();

    /**
     * @brief A destructor used to delete the <b>UIBarcode</b> instance.
     *
     * @since 3.0
     * @version 5.0
     */
    ~UIBarcode() override;

    /**
     * @brief Sets the bar code information.
     *
     * @param srcVal Indicates the pointer to the content used to generate the bar code.
     * @since 3.0
     * @version 5.0
     */
    void SetBarcodeInfo(const char* srcVal);

    /**
     * @brief Obtains the view type.
     *
     * @return Returns the view type. For details, see {@link UIViewType}.
     * @since 1.0
     * @version 1.0
     */
    UIViewType GetViewType() const override
    {
        return UI_BARCODE;
    }

    /**
     * @brief Sets the bar code width.
     *
     * @param width Indicates the width to set.
     * @since 1.0
     * @version 1.0
     */
    void SetWidth(int16_t width) override;

    /**
     * @brief Sets the bar code height.
     *
     * @param height Indicates the height to set.
     * @since 1.0
     * @version 1.0
     */
    void SetHeight(int16_t height) override;

    /**
     * @brief Enable barcode stretch, to fill the rect.
     *
     * @param enable Enable barcode stretch or not.
     * @since 1.0
     * @version 1.0
     */
    void EnableStretch(bool enable);

private:
    void ReMeasure() override;
    void RefreshBarcode();
    bool FillImageInfo();
    void FillBarCodeColor(ImageInfo& imageInfo, int32_t actualLength, uint8_t *destData, uint32_t offset);
    void SetBarcodeVal(const char* barcodeVal, uint32_t length);

    ImageInfo imageInfo_;
    int16_t stride_ = 0;
    bool needDraw_;
    ColorType barColor_;
    char* barcodeVal_;
};
}  // namespace OHOS
#endif  // GRAPHIC_UI_BARCODE_H
/**
 * @}
 */
