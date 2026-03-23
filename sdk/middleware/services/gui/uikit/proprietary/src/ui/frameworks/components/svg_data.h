/*
 * Copyright (c) 2022 CompanyNameMagicTag.
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

#ifndef GRAPHIC_LITE_SVG_DATA_H
#define GRAPHIC_LITE_SVG_DATA_H

#include "securec.h"
#include "common/graphic_hardware_types.h"

namespace OHOS {
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
/**
 * @brief Class to svg data.
 *
 * @since 1.0
 * @version 1.0
 */
class SvgData : public HeapBase {
public:
    bool Init(const char* path);
    void Deinit();

private:
    SvgData()
    {
        memset_s(&svgInfo_, sizeof(svgInfo_), 0, sizeof(svgInfo_));
    }
    virtual ~SvgData();

    bool InitPath(const char* path);
    struct PathHeader {
        uint16_t pathNum;
        uint16_t gradientNum;
        uint16_t stopNum;
        uint16_t clipPathNum;
        uint32_t cmdNum;
        uint32_t dataNum;
        uint32_t animaNum;
    };

    struct PathInfo {
        uint32_t cmdIndex;
        uint32_t dataIndex;
        uint32_t cmdNum;
        uint32_t dataNum;
        uint8_t opacity;
        uint8_t paintStyle;
        uint8_t paintType;
        uint8_t fillRule;
        uint32_t fillColor;
        uint32_t strokeColor;
        uint16_t strokeWidth;
        int16_t gradientIndex;
        int16_t clipPathIndex;
    };

    struct GradientInfo {
        uint8_t type;               // gradient type, linear or radial
        int16_t param1;             // linear:startX; radial:centerX
        int16_t param2;             // linear:startY; radial:centerY
        int16_t param3;             // linear:endX; radial:radius
        int16_t param4;             // linear:endY; radial:invalid
        uint16_t stopIndex;
        uint16_t stopNum;
    };

    struct SvgInfo {
        PathHeader header;
        PathInfo* pathPnt;
        uint8_t* cmdPnt;
        float* dataPnt;
        GradientInfo* gradientPnt;
        struct ColorStop* stopPnt;
        int16_t* clipPathPnt;
    };

    char* path_ = nullptr;
    SvgInfo svgInfo_;
    friend class UICanvasExt;
};

struct SvgPath {
    SvgData* svgObj;
    int16_t startIndex;
    int16_t endIndex;
};
#endif
}
#endif // GRAPHIC_LITE_SVG_DATA_H