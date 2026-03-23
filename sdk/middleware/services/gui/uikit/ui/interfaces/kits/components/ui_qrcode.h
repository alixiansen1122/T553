/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
 * @file ui_qrcode.h
 *
 * @brief Declares the attributes and functions of the <b>UIQrcode</b> class.
 *
 * @since 3.0
 * @version 5.0
 */

#ifndef GRAPHIC_LITE_UI_QRCODE_H
#define GRAPHIC_LITE_UI_QRCODE_H

#include "QrCode.hpp"
#include "components/ui_image_view.h"

using namespace std;
namespace qrcodegen {
class QrCode;
}
using qrcodegen::QrCode;
namespace OHOS {
/**
 * @brief Provides functions related to quick response (QR) codes.
 *
 * @since 3.0
 * @version 5.0
 */
class UIQrcode : public UIImageView {
public:
    /**
     * @brief A default constructor used to create a <b>UIQrcode</b> instance.
     *
     * @since 3.0
     * @version 5.0
     */
    UIQrcode();

    /**
     * @brief A destructor used to delete the <b>UIQrcode</b> instance.
     *
     * @since 3.0
     * @version 5.0
     */
    virtual ~UIQrcode();

    /**
     * @brief Sets the QR code information.
     *
     * @param val Indicates the pointer to the content used to generate the QR code.
     * @param backgroundColor Indicates the background color of the QR code. It is white by default.
     * @param qrColor Indicates the QR code color. It is black by default.
     * @since 3.0
     * @version 5.0
     */
    void SetQrcodeInfo(const char* val, ColorType backgroundColor = Color::White(), ColorType qrColor = Color::Black());

    /**
     * @brief Sets the QR code information.
     *
     * @param val Indicates the pointer to the binary used to generate the QR code.
     * @param len Indicates the binary length.
     * @param backgroundColor Indicates the background color of the QR code. It is white by default.
     * @param qrColor Indicates the QR code color. It is black by default.
     * @since 3.0
     * @version 5.0
     */
    void SetQrcodeInfo(const uint8_t* val, uint16_t len, ColorType backgroundColor = Color::White(), ColorType qrColor = Color::Black());

    /**
     * @brief Obtains the view type.
     *
     * @return Returns the view type. For details, see {@link UIViewType}.
     * @since 1.0
     * @version 1.0
     */
    UIViewType GetViewType() const override
    {
        return UI_QRCODE;
    }

    /**
     * @brief Sets the QR code width.
     *
     * @param width Indicates the width to set.
     * @since 1.0
     * @version 1.0
     */
    void SetWidth(int16_t width) override;

    /**
     * @brief Sets the QR code height.
     *
     * @param height Indicates the height to set.
     * @since 1.0
     * @version 1.0
     */
    void SetHeight(int16_t height) override;

    /**
     * @brief Sets the QR code ecc level.
     *
     * @param level Indicates the ecc value to set.
     * @since 1.0
     * @version 1.0
     */
    void SetECCLevel(QrCode::Ecc level)
    {
        eccLevel_ = level;
    }

private:
    void ReMeasure() override;
    void RefreshQrcode();
    void SetImageInfo(QrCode& qrcode);
    void FillQrCodeColor(QrCode& qrcode);
    void SetQrcodeVal(const char* val, uint32_t length);
    void SetQrcodeVal(const uint8_t* val, uint32_t length);
    void GetDestData(uint8_t* destData, uint16_t offset, int32_t step);
    void FillImgBackground(ImageInfo& info);

    ImageInfo imageInfo_;
    int16_t width_;
    int16_t stride_;
    bool needDraw_;
    ColorType qrColor_;
    ColorType bgColor_;
    char* qrcodeVal_;
    vector<uint8_t> dataBin_;
    QrCode::Ecc eccLevel_;
};
} // namespace OHOS
#endif // GRAPHIC_LITE_UI_QRCODE_H
/**
 * @}
 */
