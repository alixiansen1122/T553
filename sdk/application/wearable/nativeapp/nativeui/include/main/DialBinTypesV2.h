/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: DialBinTypesV2
 * Author:
 * Create: 2024-10-30
 */

#ifndef GRAPHIC_LITE_DIAL_BIN_TYPE_V2_H
#define GRAPHIC_LITE_DIAL_BIN_TYPE_V2_H
#include "main/DialBinTypes.h"

namespace OHOS {
enum class DisplayState {
    NORMAL,
    AOD,
};
static constexpr uint8_t DIAL_DISPLAY_CAPABILITY_NORMAL_ONLY = 0;
static constexpr uint8_t DIAL_DISPLAY_CAPABILITY_AMBITIENT_ONLY = 1;
static constexpr uint8_t DIAL_DISPLAY_CAPABILITY_AMBITIENT_NORMAL = 2;

struct DialHeader_V2 {
    char dialName[DIAL_NAME_MAX_LEN];
    int16_t width;
    int16_t height;
    char version[DIAL_VERSION_MAX_LEN];
    uint16_t previewImgId;
    char description[DIAL_DESCRIPTION_MAX_LEN];
    uint32_t imageOffset;
    uint8_t freq;
    uint8_t dialViewCount;
    uint8_t displayCapabiliy;
    uint8_t ambientFreq;
};

struct DialStaticImg_V2 {
    Point pos;
    Point center;
    float rotate;
    uint16_t imgId;
    uint8_t displayMode;
};

struct DialSequenceImg_V2 {
    Point pos;
    uint16_t interval;
    bool repeat;
    uint8_t imgNum;
    uint8_t displayMode;
};

struct DialRotateImg_V2 {
    Point pos;
    Point center;
    int16_t rotateStart;
    int16_t rotateEnd;
    uint16_t imgId;
    int16_t bindData;
    bool isPeriod;
    uint8_t displayMode;
    bool isAmbientPeriodic;
};

struct DialOptionImg_V2 {
    Point pos;
    int16_t bindData;
    bool isPeriod;
    uint8_t count;
    uint8_t displayMode;
    bool isAmbientPeriodic;
};

struct DialDigitalImg_V2 {
    uint8_t alignType : 4;
    uint8_t integerLength : 4;
    uint8_t decimalPrecision;
    uint16_t imgIds[DIAL_DIGITALIMG_IMG_MAX_NUM];
    int16_t signImgId;
    int16_t decimalPointImgId;
    uint16_t space;
    Point alignPosition;
    int16_t bindData;
    bool isPeriod;
    uint8_t displayMode;
    bool isAmbientPeriodic;
};

struct DialLabel_V2 {
    uint8_t breakMode;
    uint8_t align;
    uint8_t fontSize;
    bool isPeriod;
    DialRect rect;
    uint32_t color;
    char fontName[64];
    char text[64];
    int16_t bindData;
    uint8_t displayMode;
    bool isAmbientPeriodic;
};

struct DialArcLabel_V2 {
    uint8_t align;
    uint8_t fontSize;
    Point center;
    int16_t radius;
    int16_t rotateStart;
    int16_t rotateEnd;
    uint32_t color;
    char fontName[64];
    char text[64];
    int16_t bindData;
    bool isPeriod;
    uint8_t displayMode;
    bool isAmbientPeriodic;
};

struct DialBoxProgress_V2 {
    uint8_t cap;
    uint8_t direction;
    bool isPeriod;
    int16_t bindData;
    uint32_t bgColor;
    DialProgressType typeData;
    uint8_t displayMode;
    bool isAmbientPeriodic;
};

struct DialArcProgress_V2 {
    uint8_t cap;
    bool isPeriod;
    uint16_t radius;
    uint16_t startAngle;
    uint16_t endAngle;
    uint16_t lineWidth;
    int16_t bindData;
    uint32_t bgColor;
    DialProgressType typeData;
    uint8_t displayMode;
    bool isAmbientPeriodic;
};

struct DialVideo_V2 {
    DialRect rect;
    uint32_t offset;
    Color32 colorKey;
    uint32_t imgId;
    uint8_t displayMode;
};

struct DialClick_V2 {
    uint8_t type;
    DialRect rect;
    union {
        uint32_t sliceId;
        char bundleName[64];
    };
    uint8_t displayMode;
};

struct DialKaleidoscope_V2 {
    uint16_t imgId;
    float crownRotateStep;
    float animatorRotateStep;
    uint8_t displayMode;
};
} // namespace OHOS
#endif // GRAPHIC_LITE_DIAL_BIN_TYPE_V2_H
