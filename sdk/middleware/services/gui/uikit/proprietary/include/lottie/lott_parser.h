/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef LOTT_PARSER_H
#define LOTT_PARSER_H
#include "gfx_utils/heap_base.h"
#include "lott_comp.h"
#include "lott_flatbuffer.h"
#include "lott_shape_group.h"

namespace OHOS {

class LottParser : public HeapBase {
public:
    LottParser() : buffer_(nullptr), offset_(0), fp_(nullptr), comp_(nullptr), hasError_(false) {}
    ~LottParser() {}

    LottComp* Parse(std::string file);

private:
    struct LottHeader {
        uint32_t startFlag;
        uint32_t version;
        uint32_t bufferSize;
    };

    struct shapeGroup {
        uint32_t fillIndex = 0;
        uint32_t strokeindex = 0;
        uint32_t transIndex = 0;
        uint32_t trimIndex = 0;
        Graphic::Vector<uint32_t> shapes;
    };

    void* buffer_;
    uint32_t offset_;
    std::string file_;
    FILE* fp_;
    LottComp* comp_;
    bool hasError_;
    const ::flatbuffers::Vector< ::flatbuffers::Offset<LottieFlatBuffer::Asset>>* assets_;

    void LoadFile();
    void ParseLayers(const flatbuffers::Vector<::flatbuffers::Offset<void>>* layers,
        const flatbuffers::Vector<uint8_t>* layers_type, Graphic::Vector<LottLayer*>& parent);
    void ParseBaseLayer(const void* layer, LottLayer* lottLayer);
    LottLayer* ParsePreComLayer(const void* layer);
    LottLayer* ParseImageLayer(const void* layer);
    LottLayer* ParseShapeLayer(const void* layer);
    LottLayer* ParseNullLayer(const void* layer);
    LottLayer* ParseSolidLayer(const void* layer);
    LottLayer* ParseTextLayer(const void* layer);
    LottTransform* ParseTransform(const LottieFlatBuffer::Transform* trans);

    Graphic::Vector<shapeGroup>* BuildGroups(
        const flatbuffers::Vector<::flatbuffers::Offset<void>>* shapes,
        const flatbuffers::Vector<uint8_t>* shapes_type);

    LottShapeGroup* BuildGroup(shapeGroup* group, const flatbuffers::Vector<::flatbuffers::Offset<void>>* shapes,
        const flatbuffers::Vector<uint8_t>* shapes_type);
    LottShape* ParseShape(const void* shape, uint8_t type);
    LottShape* ParseGroup(const void* shape);
    LottShape* ParseEllipse(const void* shape);
    LottShape* ParseRect(const void* shape);
    LottShape* ParsePath(const void* shape);
    LottFill* ParseFill(const void* shape, uint8_t type);
    LottStroke* ParseStroke(const void* shape, uint8_t type);
    LottTransform* ParseTransShape(const void* shape);
    LottTrim* ParseTrimPath(const void* shape);

    void ParseSplitProp(const void* split, LottTransform::LottSplitPosition& lottSplit);
    void ParsePositionProp(const void* value,  LottieFlatBuffer::PositionProp type, LottPositionProp& lottPosition);
    void ParseFloatProp(const void* value, LottieFlatBuffer::FloatProp type, LottFloatProp& lottFloat);
    void ParseIntegerProp(const void* value, LottieFlatBuffer::IntegerProp type, LottIntegerProp& lottInt);
    void ParseBezierProp(const void* value, LottieFlatBuffer::BezierDataProp type, LottPathDataProp& lottBez);
    void ParseVectorProp(const void* value, LottieFlatBuffer::VectorProp type, LottVectorProp& lottVec);
    void ParseColorProp(const void* value, LottieFlatBuffer::ColorProp type, LottColorProp& lottColor);
    void ParseGradientProp(const void* value, LottieFlatBuffer::GradientProp type, LottColorStopsProp& lottGradient);
    void ParseBezierData(const LottieFlatBuffer::BezierData* bezData, Path& path);
    void ParseColorStops(const LottieFlatBuffer::Gradient* gradient, Graphic::Vector<ColorStop>& colorStops);

    void BuildHierarchy(Graphic::Vector<LottLayer*>& parent);
};
}
#endif // LOTT_PARSER_H

