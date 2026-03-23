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

#include "components/ui_qrcode.h"
#include "gfx_utils/graphic_log.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "securec.h"

namespace OHOS {
UIQrcode::UIQrcode()
    : width_(0), needDraw_(false), qrColor_(Color::Black()), bgColor_(Color::White()), qrcodeVal_(nullptr),
    eccLevel_(QrCode::Ecc::QUARTILE)
{
    style_ = new Style();
    imageInfo_ = {{0}};
}

UIQrcode::~UIQrcode()
{
    if (qrcodeVal_ != nullptr) {
        UIFree(qrcodeVal_);
        qrcodeVal_ = nullptr;
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

void UIQrcode::SetQrcodeInfo(const char* val, ColorType backgroundColor, ColorType qrColor)
{
    if (val == nullptr) {
        GRAPHIC_LOGE("UIQrcode::SetQrcodeInfo val is null!\n");
        return;
    }
    uint32_t length = static_cast<uint32_t>(strlen(val));
    if ((length > QRCODE_VAL_MAX) || (length == 0)) {
        GRAPHIC_LOGE("UIQrcode::SetQrcodeInfo val length is equal 0 or greater than QRCODE_VAL_MAX!\n");
        return;
    }
    style_->SetStyle(STYLE_BACKGROUND_COLOR, backgroundColor.full);
    qrColor_ = qrColor;
    bgColor_ = backgroundColor;
    SetQrcodeVal(val, length);
    RefreshQrcode();
}

void UIQrcode::SetQrcodeInfo(const uint8_t* val, uint16_t len, ColorType backgroundColor, ColorType qrColor)
{
    if (val == nullptr) {
        GRAPHIC_LOGE("UIQrcode::SetQrcodeInfo val is null!\n");
        return;
    }
    if ((len > QRCODE_VAL_MAX) || (len == 0)) {
        GRAPHIC_LOGE("UIQrcode::SetQrcodeInfo len equals 0 or greater than QRCODE_VAL_MAX!\n");
        return;
    }
    style_->SetStyle(STYLE_BACKGROUND_COLOR, backgroundColor.full);
    qrColor_ = qrColor;
    bgColor_ = backgroundColor;
    SetQrcodeVal(val, len);
    RefreshQrcode();
}

void UIQrcode::RefreshQrcode()
{
    Invalidate();
    if (!needDraw_) {
        needDraw_ = true;
    }
}

void UIQrcode::SetWidth(int16_t width)
{
    if (GetWidth() != width) {
        UIView::SetWidth(width);
        RefreshQrcode();
    }
}

void UIQrcode::SetHeight(int16_t height)
{
    if (GetHeight() != height) {
        UIView::SetHeight(height);
        RefreshQrcode();
    }
}

void UIQrcode::ReMeasure()
{
    if (!needDraw_) {
        return;
    }
    needDraw_ = false;
    if (qrcodeVal_ != nullptr) {
        QrCode qr = QrCode::encodeText(qrcodeVal_, eccLevel_);
        SetImageInfo(qr);
    } else if (!dataBin_.empty()) {
        QrCode qr = QrCode::encodeBinary(dataBin_, eccLevel_);
        SetImageInfo(qr);
    } else {
        return;
    }
    SetSrc(&imageInfo_);
}

void UIQrcode::SetQrcodeVal(const char* qrcodeVal, uint32_t length)
{
    if (qrcodeVal_ != nullptr) {
        UIFree(qrcodeVal_);
        qrcodeVal_ = nullptr;
    }
    dataBin_.clear();

    uint32_t len = static_cast<uint32_t>(length + 1);
    qrcodeVal_ = static_cast<char*>(UIMalloc(len));
    if (qrcodeVal_ != nullptr) {
        if (memcpy_s(qrcodeVal_, len, qrcodeVal, len) != EOK) {
            UIFree(reinterpret_cast<void*>(qrcodeVal_));
            qrcodeVal_ = nullptr;
        }
    }
}

void UIQrcode::SetQrcodeVal(const uint8_t* val, uint32_t length)
{
    if (qrcodeVal_ != nullptr) {
        UIFree(qrcodeVal_);
        qrcodeVal_ = nullptr;
    }
    dataBin_.clear();
    for (uint32_t i = 0; i < length; i++) {
        dataBin_.push_back(val[i]);
    }
}

void UIQrcode::FillImgBackground(ImageInfo& info)
{
    BufferInfo gfxDstBuffer;
    gfxDstBuffer.virAddr = static_cast<void*>(const_cast<uint8_t*>(info.data));
    gfxDstBuffer.phyAddr = gfxDstBuffer.virAddr;
    gfxDstBuffer.width = width_;
    gfxDstBuffer.height = width_;
    gfxDstBuffer.mode = RGB565;
    gfxDstBuffer.compressMode = 0; // no compress
    gfxDstBuffer.stride = stride_;
    LiteMGfxEngine::GetInstance()->Fill(gfxDstBuffer, Rect(0, 0 , width_ - 1, width_ - 1), bgColor_, OPA_OPAQUE);
    LiteMGfxEngine::GetInstance()->SyncHwDraw();
}

void UIQrcode::SetImageInfo(qrcodegen::QrCode& qrcode)
{
    int16_t width = GetWidth();
    int16_t height = GetHeight();
    width_ = (width >= height) ? height : width;
    if (width_ < qrcode.getSize()) {
        GRAPHIC_LOGE("UIQrcode::SetImageInfo width is less than the minimum qrcode width!\n");
        return;
    }
    width_ = (width_ / qrcode.getSize()) * qrcode.getSize();
    imageInfo_.header.width = width_;
    imageInfo_.header.height = width_;
    imageInfo_.header.colorMode = RGB565;
    stride_ = ALIGN_BYTE(width_, BYTE_ALIGNMENT) * 2; // 2: 2 byte per pixel for RGB565
    imageInfo_.dataSize = stride_ * imageInfo_.header.height;
    if (imageInfo_.data != nullptr) {
        ImageCacheFree(imageInfo_);
        imageInfo_.data = nullptr;
    }
    imageInfo_.data = reinterpret_cast<uint8_t*>(ImageCacheMalloc(imageInfo_));
    if (imageInfo_.data == nullptr) {
        GRAPHIC_LOGE("UIQrcode::SetImageInfo imageInfo_.data is null!\n");
        return;
    }
    FillImgBackground(imageInfo_);
    FillQrCodeColor(qrcode);
}

void UIQrcode::FillQrCodeColor(qrcodegen::QrCode& qrcode)
{
    int32_t qrWidth = qrcode.getSize();
    if (qrWidth <= 0) {
        GRAPHIC_LOGE("UIQrcode::FillQrCodeColor generated qrcode size is less or equal 0!\n");
        return;
    }
    uint16_t step = width_ / qrWidth;

    uint32_t scaledOneLinePix = stride_ * step;
    uint8_t* destData = const_cast<uint8_t*>(imageInfo_.data);
    for (int32_t y = 0; y < qrWidth; ++y) {
        uint16_t xOffset = 0;
        for (int32_t x = 0; x < qrWidth; ++x) {
            if (qrcode.getModule(x, y)) {
                GetDestData(destData, xOffset, step);
            }
            xOffset += step * 2; // 2: 2 byte per pixel for RGB565
        }
        destData += scaledOneLinePix;
    }
}

void UIQrcode::GetDestData(uint8_t* destData, uint16_t offset, int32_t step)
{
    for (int32_t x = 0; x < step; ++x) {
        uint32_t xOffset = offset;
        for (int32_t y = 0; y < step; ++y) {
            Color16* temp = reinterpret_cast<Color16*>(destData + xOffset);
            temp->full = Color::ColorTo16(qrColor_);
            xOffset += 2; // 2: 2 byte per pixel for RGB565
        }
        destData += stride_;
    }
}
} // namespace OHOS
