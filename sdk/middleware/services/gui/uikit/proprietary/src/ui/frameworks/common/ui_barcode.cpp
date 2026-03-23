/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: UIBarcode
 * Author:
 * Create: 2023-12
 */

#include "securec.h"
#include "barcode128.h"
#include "gfx_utils/graphic_log.h"
#include "hals/display_dev.h"
#include "components/ui_barcode.h"

namespace OHOS {
UIBarcode::UIBarcode() : needDraw_(false), barColor_(Color::Black()), barcodeVal_(nullptr)
{
    style_ = new Style();
    style_->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);

    imageInfo_ = {{0}};
    SetAutoEnable(false);
    SetResizeMode(CENTER);
}

UIBarcode::~UIBarcode()
{
    if (barcodeVal_ != nullptr) {
        UIFree(barcodeVal_);
        barcodeVal_ = nullptr;
    }

    if (imageInfo_.data != nullptr) {
        ImageCacheFree(imageInfo_);
        imageInfo_.data = nullptr;
    }

    if (style_ != nullptr) {
        delete style_;
        style_ = nullptr;
    }
}

void UIBarcode::SetBarcodeInfo(const char* srcVal)
{
    if (srcVal == nullptr) {
        GRAPHIC_LOGE("UIBarcode::SetQrcodeInfo val is null!");
        return;
    }
    uint32_t length = static_cast<uint32_t>(strlen(srcVal));
    if ((length > QRCODE_VAL_MAX) || (length == 0)) {
        GRAPHIC_LOGE("UIBarcode::SetQrcodeInfo val length is equal 0 or greater than QRCODE_VAL_MAX!");
        return;
    }
    SetBarcodeVal(srcVal, length);
    RefreshBarcode();
}

void UIBarcode::RefreshBarcode()
{
    Invalidate();
    if (!needDraw_) {
        needDraw_ = true;
    }
}

void UIBarcode::EnableStretch(bool enable)
{
    if (enable) {
        SetResizeMode(FILL);
    } else {
        SetResizeMode(CENTER);
    }
}
void UIBarcode::SetWidth(int16_t width)
{
    if (GetWidth() != width) {
        UIView::SetWidth(width);
        RefreshBarcode();
    }
}

void UIBarcode::SetHeight(int16_t height)
{
    if (GetHeight() != height) {
        UIView::SetHeight(height);
        RefreshBarcode();
    }
}

void UIBarcode::ReMeasure()
{
    if (!needDraw_) {
        return;
    }
    needDraw_ = false;
    if (barcodeVal_ == nullptr) {
        GRAPHIC_LOGE("UIBarcode::ReMeasure barcodeVal_ is null!");
        return;
    }
    if (FillImageInfo()) {
        SetSrc(&imageInfo_);
    }
}

void UIBarcode::SetBarcodeVal(const char* barcodeVal, uint32_t length)
{
    if (barcodeVal_ != nullptr) {
        UIFree(barcodeVal_);
        barcodeVal_ = nullptr;
    }

    uint32_t len = static_cast<uint32_t>(length + 1);
    barcodeVal_ = static_cast<char*>(UIMalloc(len));
    if (barcodeVal_ != nullptr) {
        if (memcpy_s(barcodeVal_, len, barcodeVal, len) != EOK) {
            UIFree(reinterpret_cast<void*>(barcodeVal_));
            barcodeVal_ = nullptr;
        }
    }
}

bool UIBarcode::FillImageInfo()
{
    if ((barcodeVal_ == nullptr) || (strlen(barcodeVal_) == 0)) {
        GRAPHIC_LOGE("barcodeVal_ is nullptr");
        return false;
    }
    int16_t width = GetWidth();
    int16_t height = GetHeight();

    int32_t barcodeLength = Code128EstimateLen(barcodeVal_);
    char* destData = static_cast<char*>(UIMalloc(barcodeLength));
    if (destData == nullptr) {
        GRAPHIC_LOGE("destData is nullptr");
        return false;
    }

    int32_t actualLength = Code128Encode(barcodeVal_, destData, barcodeLength);
    if (actualLength == 0 || actualLength > width) {
        GRAPHIC_LOGE("actualLength is 0 or actualLength > width");
        return false;
    }
    uint32_t step = static_cast<uint32_t>(width / actualLength);
    imageInfo_.header.width = static_cast<uint32_t>(actualLength) * step;
    imageInfo_.header.height = height;
    imageInfo_.header.colorMode = RGB565;
    stride_ = DisplayDev::GetInstance()->CalcStride(imageInfo_.header.width, PIXEL_FMT_RGB_565);
    imageInfo_.dataSize = stride_ * height;
    if (imageInfo_.data != nullptr) {
        ImageCacheFree(imageInfo_);
        imageInfo_.data = nullptr;
    }
    imageInfo_.data = reinterpret_cast<uint8_t*>(ImageCacheMalloc(imageInfo_));
    imageInfo_.color = barColor_.full;
    if (imageInfo_.data == nullptr) {
        GRAPHIC_LOGE("UIBarcode::FillImageInfo imageInfo_.data is null!\n");
        return false;
    }
    (void)memset_s(const_cast<uint8_t*>(imageInfo_.data), imageInfo_.dataSize, 0xff, imageInfo_.dataSize);
    FillBarCodeColor(imageInfo_, actualLength, reinterpret_cast<uint8_t*>(destData), step);

    return true;
}

void UIBarcode::FillBarCodeColor(ImageInfo& imageInfo, int32_t actualLength, uint8_t *destData, uint32_t offset)
{
    uint8_t* data = const_cast<uint8_t *>(imageInfo.data);

    for (uint32_t i = 0; i < static_cast<uint32_t>(actualLength); i++) {
        if ((*(destData + i)) == 0) {
            continue;
        }

        int16_t codeOffset = static_cast<int16_t>(i * offset * 2); // 2: rgb565 has 2 byte per pixel
        for (uint32_t k = 0; k < offset; k++) {
            *(data + codeOffset + k * 2) = 0; // 2: rgb565 has 2 byte per pixel
            *(data + codeOffset + k * 2 + 1) = 0; // 2: rgb565 has 2 byte per pixel
        }
    }

    for (uint32_t j = 1; j < imageInfo_.header.height; j++) {
        if (memcpy_s(data + stride_ * j, stride_, data, stride_) != EOK) {
            GRAPHIC_LOGE("memcpy_s failed.\n");
            return;
        }
    }
    return;
}
}  // namespace OHOS
