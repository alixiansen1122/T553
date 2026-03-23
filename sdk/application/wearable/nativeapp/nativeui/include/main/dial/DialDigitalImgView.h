/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef DIAL_DIGITAL_IMG_VIEW_H
#define DIAL_DIGITAL_IMG_VIEW_H

#include <cstdint>
#include "main/dial/DialView.h"
#include "main/dial/DialModelTime.h"
#include "components/ui_image_view.h"
#include "gfx_utils/graphic_math.h"
#include "components/ui_view_group.h"

namespace OHOS {

enum class ImgAlign {
    LEFT,
    CENTER,
    RIGHT,
};

class DialDigitalImgView : public UIImageView, public DialView, public TimeChangeListener {
public:
    DialDigitalImgView();
    ~DialDigitalImgView() override;

    void HandleFloatData(float data) override;

    bool SetNumRes(const ImageInfo** src, uint16_t num);
    void SetSignRes(const ImageInfo* src);
    void SetDecimalPointRes(const ImageInfo* src);
    void SetIntegerLength(uint8_t length);
    void SetSpace(uint16_t space);
    void SetAlign(ImgAlign align);
    void SetAlignPos(Point pos);
    void SetDecimalPrecision(uint8_t precision);
    void SetParent(UIViewGroup* parent);
    void OnTimeUpdate(const struct tm &curTime) override;

private:
    void SetNumImg(UIImageView* img, const ImageInfo *info);
    void FillIntegerPart(uint16_t intVal);
    void FillDecimalPart(float data);
    void HandlePosition();
    bool CreateImageViews(float data);

    static const uint8_t NUM_IMG_MAX_SIZE = 10;
    ImageInfo* numInfo_[NUM_IMG_MAX_SIZE];
    uint16_t numCnt_ = 0;
    ImageInfo* signInfo_ = nullptr;
    ImageInfo* decimalPointInfo_ = nullptr;
    uint16_t space_ = 0;
    ImgAlign align_ = ImgAlign::LEFT;
    Point alignPos_ = {0, 0};
    uint8_t integerLength_ = 0;
    uint8_t allocIntegerLen_ = 0;
    uint8_t decimalPrecision_ = 0;
    uint8_t currentIndex_ = 0;

    UIViewGroup* parent_ = nullptr;
    UIImageView** imgs_ = nullptr;
    uint8_t imgCnt_ = 0;
    bool isValueSet_ = false;
    float value_ = 0.0f;
};
}
#endif
