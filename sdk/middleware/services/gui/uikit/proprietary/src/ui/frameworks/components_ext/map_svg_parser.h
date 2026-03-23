/*
 * Copyright (c) 2024 CompanyNameMagicTag.
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
#ifndef GRAPHIC_LITE_MAP_SVG_PARSER_H
#define GRAPHIC_LITE_MAP_SVG_PARSER_H
#include <cstdint>
#include <string>
#include "gfx_utils/color.h"

namespace OHOS {
struct MapRect {
    int16_t x;
    int16_t y;
    int16_t width;
    int16_t height;
};

struct PathStyle {
    uint8_t strokeCap : 4;
    uint8_t strokeJoin : 4;
    Color24 strokeColor;
    Color24 fillColor;
    float strokeWidth;
};

struct MapPathBase {
    uint8_t type;
    uint8_t pathStyleIndex;
    uint16_t cmdNum;
    uint16_t dataNum;
};

struct PathDetail {
    MapPathBase base;
    uint8_t* cmd;
    float* data;
};

struct ImageBase {
    uint8_t id;
    MapRect rect;
};

struct ImageDetail {
    ImageBase base;
    ImageInfo* info;
};

struct MapTextBase {
    uint8_t fontSize;
    Color24 fontColor;
    Color24 strokeColor;
    float strokeWidth;
    MapRect rect;
    uint16_t charLen;
};

struct TextDetail {
    MapTextBase base;
    char* text;
    Point* offset;
};

struct POIDetail {
    int8_t imgIndex;
    int8_t textIndex[2];
    uint8_t tag;
    uint16_t rank;
    uint16_t id;
};

struct RoadDetail {
    int8_t textIndex;
};

class MapSVGParser {
public:
    MapSVGParser(void);
    virtual ~MapSVGParser(void);

    bool SetSVG(const std::string& file);
    bool SetSVG(uint8_t* data, uint32_t length);
    void ClearSVG();

    Color24 GetBackGroundColor(void);
    uint32_t GetImageOffset(void);
    void GetSize(int16_t& width, int16_t& height);

    uint8_t GetPaths(PathDetail*& pathList);
    uint8_t GetPOIs(POIDetail*& poiList);
    uint8_t GetRoads(RoadDetail*& roadList);

    PathStyle* GetPathStyleByIndex(uint8_t index);
    TextDetail* GetTextByIndex(uint8_t index);
    ImageDetail* GetImageByIndex(uint8_t index);

private:
    struct MapHeader {
        Color24 bgColor;
        uint8_t nodeNum;
        uint8_t pathStyleNum;
        int16_t width;
        int16_t height;
        uint32_t imgDataOffset;
    };

    struct MapNode {
        uint8_t type;
        uint8_t count;
        uint32_t offset;
    };

    static constexpr uint8_t PARSER_TYPE_NUM = 5;
    using MapTypeReadByData = uint32_t (MapSVGParser::*)(uint8_t *data, uint32_t length,
        uint32_t offset, uint8_t count);
    using MapTypeReadByPath = uint32_t (MapSVGParser::*)(FILE* fp, uint8_t count);
    using MapTypeRelease = void (MapSVGParser::*)(uint8_t count);

    uint32_t ReadMapNode(uint8_t* data, uint32_t length, uint32_t offset);
    uint32_t ReadPathStyle(uint8_t* data, uint32_t length, uint32_t offset);
    uint32_t ReadMapPath(uint8_t* data, uint32_t length, uint32_t offset, uint8_t count);
    uint32_t ReadMapText(uint8_t* data, uint32_t length, uint32_t offset, uint8_t count);
    uint32_t ReadMapImg(uint8_t* data, uint32_t length, uint32_t offset, uint8_t count);
    uint32_t ReadMapRoad(uint8_t* data, uint32_t length, uint32_t offset, uint8_t count);
    uint32_t ReadMapPOI(uint8_t* data, uint32_t length, uint32_t offset, uint8_t count);

    uint32_t ReadMapNode(FILE* fp);
    uint32_t ReadPathStyle(FILE* fp);
    uint32_t ReadMapPath(FILE* fp, uint8_t count);
    uint32_t ReadMapText(FILE* fp, uint8_t count);
    uint32_t ReadMapImg(FILE* fp, uint8_t count);
    uint32_t ReadMapRoad(FILE* fp, uint8_t count);
    uint32_t ReadMapPOI(FILE* fp, uint8_t count);

    void ReleaseMapPath(uint8_t count);
    void ReleaseMapText(uint8_t count);
    void ReleaseMapImg(uint8_t count);
    void ReleaseMapRoad(uint8_t count);
    void ReleaseMapPOI(uint8_t count);

    void ReleaseResource(void);
    bool AllocPath(PathDetail& path, uint32_t cmdNum, uint32_t dataNum);

    MapTypeReadByData readByDataFunc[PARSER_TYPE_NUM] = {
        &MapSVGParser::ReadMapPath, &MapSVGParser::ReadMapText, &MapSVGParser::ReadMapImg,
        &MapSVGParser::ReadMapRoad, &MapSVGParser::ReadMapPOI };

    MapTypeReadByPath readByPathFunc[PARSER_TYPE_NUM] = {
        &MapSVGParser::ReadMapPath, &MapSVGParser::ReadMapText, &MapSVGParser::ReadMapImg,
        &MapSVGParser::ReadMapRoad, &MapSVGParser::ReadMapPOI };

    MapTypeRelease releaseFunc[PARSER_TYPE_NUM] = {
        &MapSVGParser::ReleaseMapPath, &MapSVGParser::ReleaseMapText, &MapSVGParser::ReleaseMapImg,
        &MapSVGParser::ReleaseMapRoad, &MapSVGParser::ReleaseMapPOI };

    MapHeader mapHeader_ = {{0}};
    MapNode* nodeArray_ = nullptr;
    PathStyle* pathStyleArray_ = nullptr;
    PathDetail* pathArray_ = nullptr;
    uint8_t pathCount_ = 0;
    TextDetail* textArray_ = nullptr;
    uint8_t textCount_ = 0;
    ImageDetail* imageArray_ = nullptr;
    uint8_t imageCount_ = 0;
    RoadDetail* roadArray_ = nullptr;
    uint8_t roadCount_ = 0;
    POIDetail* poiArray_ = nullptr;
    uint8_t poiCount_ = 0;
    uint8_t* data_ = nullptr;
    std::string file_;
};
}
#endif // GRAPHIC_LITE_MAP_SVG_PARSER_H
