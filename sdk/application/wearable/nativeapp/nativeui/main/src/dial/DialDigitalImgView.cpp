/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "main/dial/DialDigitalImgView.h"
#include <cmath>
#include "securec.h"

namespace OHOS {
DialDigitalImgView::DialDigitalImgView(void)
{
}

DialDigitalImgView::~DialDigitalImgView(void)
{
    if (imgs_ != nullptr) {
        for (uint8_t i = 0; i < imgCnt_; i++) {
            if (parent_ != nullptr) {
                parent_->Remove(imgs_[i]);
            }
            if (imgs_[i] != nullptr) {
                delete imgs_[i];
            }
        }
        UIFree(imgs_);
        imgs_ = nullptr;
    }
}

bool DialDigitalImgView::SetNumRes(const ImageInfo** src, uint16_t num)
{
    if (num > NUM_IMG_MAX_SIZE) {
        return false;
    }
    if (memcpy_s(numInfo_, sizeof(ImageInfo*) * num, src, sizeof(ImageInfo*) * num) != EOK) {
        return false;
    }
    numCnt_ = num;
    return true;
}

void DialDigitalImgView::SetSignRes(const ImageInfo* src)
{
    signInfo_ = const_cast<ImageInfo*>(src);
}

void DialDigitalImgView::SetDecimalPointRes(const ImageInfo* src)
{
    decimalPointInfo_ = const_cast<ImageInfo*>(src);
}

void DialDigitalImgView::SetIntegerLength(uint8_t length)
{
    integerLength_ = length;
}

void DialDigitalImgView::SetSpace(uint16_t space)
{
    space_ = space;
}

void DialDigitalImgView::SetAlign(ImgAlign align)
{
    align_ = align;
}

void DialDigitalImgView::SetAlignPos(Point pos)
{
    alignPos_ = pos;
}

void DialDigitalImgView::SetParent(UIViewGroup* parent)
{
    parent_ = parent;
}

void DialDigitalImgView::OnTimeUpdate(const struct tm &curTime)
{
    bool isVisible = false;
    if (IsInTimeInterval(curTime)) {
        isVisible = true;
    }
    for (uint8_t i = 0; i < imgCnt_; ++i) {
        imgs_[i]->SetVisible(isVisible);
    }
    SetVisible(isVisible);
}

void DialDigitalImgView::SetDecimalPrecision(uint8_t precision)
{
    decimalPrecision_ = precision;
}

void DialDigitalImgView::SetNumImg(UIImageView* img, const ImageInfo *info)
{
    if (img == nullptr || info == nullptr) {
        return;
    }
    img->SetAutoEnable(true);
    img->SetSrc(info);
    currentIndex_++;
}

static uint8_t GetValueLength(uint16_t value)
{
    uint8_t length = 1;
    if (value > 0) {
        length = log10(value) + 1;
    }
    return length;
}

void DialDigitalImgView::FillIntegerPart(uint16_t intVal)
{
    uint8_t length = GetValueLength(intVal);
    if (length < allocIntegerLen_) {
        for (uint8_t i = 0; i < (allocIntegerLen_ - length); i++) {
            SetNumImg(imgs_[currentIndex_], numInfo_[0]);
        }
    }
    for (uint8_t i = 0; i < length; i++) {
        uint8_t curNum = static_cast<uint16_t>(intVal / (pow(10, length - i - 1))) % 10; // 10: conversion
        SetNumImg(imgs_[currentIndex_], numInfo_[curNum]);
    }
}

void DialDigitalImgView::FillDecimalPart(float data)
{
    uint8_t startIndex = GetValueLength(static_cast<uint16_t>(abs(data))) + 1;
    char dataStr[10] = {'\0'}; // 10: max number of digits.
    sprintf_s(dataStr, 10, "%f", data); // 10: size of dataStr

    for (uint8_t i = 0; i < decimalPrecision_; i++) {
        uint8_t index = startIndex + i;
        if ((index < 10) && (dataStr[index] != '\0')) { // 10: size of dataStr
            uint8_t val = dataStr[index] - '0';
            SetNumImg(imgs_[currentIndex_], numInfo_[val]);
        } else {
            SetNumImg(imgs_[currentIndex_], numInfo_[0]);
        }
    }
}

void DialDigitalImgView::HandlePosition()
{
    uint32_t totalWidth = 0;
    for (uint8_t i = 0; i < currentIndex_; i++) {
        totalWidth += imgs_[i]->GetWidth();
    }
    totalWidth += (currentIndex_ - 1) * space_;

    int16_t curX = alignPos_.x;
    if (align_ == ImgAlign::RIGHT) {
        curX -= totalWidth;
    } else if (align_ == ImgAlign::CENTER) {
        curX -= totalWidth / 2; // 2: half
    }
    UIView* prev = this; // this is always the leading view of those images.
    for (uint8_t i = 0; i < currentIndex_; i++) {
        parent_->Insert(prev, imgs_[i]);
        imgs_[i]->SetPosition(curX, alignPos_.y);
        imgs_[i]->Invalidate();
        curX += imgs_[i]->GetWidth() + space_;
        prev = imgs_[i];
    }
}

bool DialDigitalImgView::CreateImageViews(float data)
{
    uint8_t imgCnt = 0;
    uint16_t integer = static_cast<uint16_t>(abs(data));

    uint8_t actualIntegerLen = GetValueLength(integer);

    if (integerLength_ == 0) {
        imgCnt = actualIntegerLen;
        allocIntegerLen_ = actualIntegerLen;
    } else {
        if (integerLength_ < actualIntegerLen) {
            GRAPHIC_LOGE("The integer part is too long.set len:%d, data len %d", integerLength_, actualIntegerLen);
            return false;
        }
        imgCnt = integerLength_;
        allocIntegerLen_ = integerLength_;
    }
    if (decimalPrecision_ != 0) {
        imgCnt += (decimalPrecision_ + 1);
    }
    if (data < 0.0f) {
        imgCnt++;
    }
    if (imgCnt_ != 0) {
        for (uint8_t i = 0; i < imgCnt_; i++) {
            if (parent_ != nullptr) {
                imgs_[i]->Invalidate();
                parent_->Remove(imgs_[i]);
            }
        }
    }

    if (imgCnt_ < imgCnt) {
        UIImageView** temp = static_cast<UIImageView**>(UIMalloc(sizeof(UIImageView*) * imgCnt));
        if (temp == nullptr) {
            return false;
        }
        if (imgs_ != nullptr) {
            memcpy_s(temp, sizeof(UIImageView*) * imgCnt, imgs_, sizeof(UIImageView*) * imgCnt);
            UIFree(imgs_);
        }
        imgs_ = temp;
        for (uint8_t i = imgCnt_; i < imgCnt; i++) {
            imgs_[i] = new UIImageView();
        }
    }

    imgCnt_ = imgCnt;
    currentIndex_ = 0;
    return true;
}

void DialDigitalImgView::HandleFloatData(float data)
{
    if (!IsVisible()) {
        return;
    }
    if (isValueSet_ && FloatEqual(value_, data)) {
        return;
    }
    if (parent_ == nullptr) {
        return;
    }
    if (!CreateImageViews(data)) {
        GRAPHIC_LOGE("CreateImageViews failed.");
        return;
    }
    if (data < 0.0f && signInfo_ != nullptr) {
        SetNumImg(imgs_[currentIndex_], signInfo_);
    }
    float absVal = abs(data);
    uint16_t intVal = static_cast<uint16_t>(absVal);
    FillIntegerPart(intVal);

    if (decimalPrecision_ != 0 && decimalPointInfo_ != nullptr) {
        SetNumImg(imgs_[currentIndex_], decimalPointInfo_);
        FillDecimalPart(data);
    }
    HandlePosition();
    value_ = data;
    isValueSet_ = true;
}
}
