/*
 * Copyright (c) 2023 CompanyNameMagicTag.
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
#ifndef GRAPHIC_LITE_DIAL_BIN_TYPE_H
#define GRAPHIC_LITE_DIAL_BIN_TYPE_H
#include "color.h"
#include "graphic_types.h"
#include "components/ui_canvas_ext.h"

namespace OHOS {
static constexpr uint8_t DIAL_NAME_MAX_LEN = 16;
static constexpr uint8_t DIAL_VERSION_MAX_LEN = 16;
static constexpr uint8_t DIAL_DESCRIPTION_MAX_LEN = 128;
static constexpr uint8_t DIAL_DIGITALIMG_IMG_MAX_NUM = 10;

struct DialPreviewInfo {
    char dialName[DIAL_NAME_MAX_LEN];
    char description[DIAL_DESCRIPTION_MAX_LEN];
    uint32_t imgId;
    uint32_t offset;
    uint8_t capability;
    uint32_t uuid;
    char protocolVersion[DIAL_VERSION_MAX_LEN];
    char watchVersion[DIAL_VERSION_MAX_LEN];
};

struct DialRect {
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
};

struct DialHeader {
    char dialName[DIAL_NAME_MAX_LEN];
    int16_t width;
    int16_t height;
    char version[DIAL_VERSION_MAX_LEN];
    uint16_t previewImgId;
    char description[DIAL_DESCRIPTION_MAX_LEN];
    uint32_t imageOffset;
    uint8_t freq;
    uint8_t dialViewCount;
};

struct DialStaticImg {
    Point pos;
    Point center;
    float rotate;
    uint16_t imgId;
};

struct DialSequenceImg {
    Point pos;
    uint16_t interval;
    bool repeat;
    uint8_t imgNum;
};

struct DialRotateImg {
    Point pos;
    Point center;
    int16_t rotateStart;
    int16_t rotateEnd;
    uint16_t imgId;
    int16_t bindData;
    bool isPeriod;
};

struct DialOptionImg {
    Point pos;
    int16_t bindData;
    bool isPeriod;
    uint8_t count;
};

struct DialDigitalImg {
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
};

struct DialLabel {
    uint8_t breakMode;
    uint8_t align;
    uint8_t fontSize;
    bool isPeriod;
    DialRect rect;
    uint32_t color;
    char fontName[64];
    char text[64];
    int16_t bindData;
};

struct DialArcLabel {
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
};

struct DialProgressType {
    uint8_t type;       //  image | color | gradient_color
    DialRect rect;
    union {
        uint8_t stopCount;
        uint16_t imgId;
        Color32 color;
    };
};

struct DialBoxProgress {
    uint8_t cap;
    uint8_t direction;
    bool isPeriod;
    int16_t bindData;
    uint32_t bgColor;
    DialProgressType typeData;
};

struct DialArcProgress {
    uint8_t cap;
    bool isPeriod;
    uint16_t radius;
    uint16_t startAngle;
    uint16_t endAngle;
    uint16_t lineWidth;
    int16_t bindData;
    uint32_t bgColor;
    DialProgressType typeData;
};

struct DialVideo {
    DialRect rect;
    uint32_t offset;
    Color32 colorKey;
    uint32_t imgId;
};

struct DialClick {
    uint8_t type;
    DialRect rect;
    union {
        uint32_t sliceId;
        char bundleName[64];
    };
};

struct DialKaleidoscope {
    uint16_t imgId;
    float crownRotateStep;
    float animatorRotateStep;
};
}
#endif // GRAPHIC_LITE_DIAL_BIN_TYPE_H