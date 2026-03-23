/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef LOTT_FLATBUFFER_H
#define LOTT_FLATBUFFER_H

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 24 && FLATBUFFERS_VERSION_MINOR == 3 && FLATBUFFERS_VERSION_REVISION == 25,
              "Non-compatible flatbuffers version included");

namespace LottieFlatBuffer {

struct Point;

struct Color32;

struct Color24;

struct ColorStop;

struct Gradient;
struct GradientBuilder;

struct FloatFrame;
struct FloatFrameBuilder;

struct FloatFrameArray;
struct FloatFrameArrayBuilder;

struct FloatScalar;
struct FloatScalarBuilder;

struct IntegerFrame;
struct IntegerFrameBuilder;

struct IntegerFrameArray;
struct IntegerFrameArrayBuilder;

struct IntegerScalar;
struct IntegerScalarBuilder;

struct BezierData;
struct BezierDataBuilder;

struct VectorFrame;
struct VectorFrameBuilder;

struct VectorFrameArray;
struct VectorFrameArrayBuilder;

struct ColorFrame;
struct ColorFrameBuilder;

struct ColorFrameArray;
struct ColorFrameArrayBuilder;

struct Color24Scalar;
struct Color24ScalarBuilder;

struct GradientFrame;
struct GradientFrameBuilder;

struct GradientFrameArray;
struct GradientFrameArrayBuilder;

struct BezierDataFrame;
struct BezierDataFrameBuilder;

struct BezierDataFrameArray;
struct BezierDataFrameArrayBuilder;

struct PositionFrame;
struct PositionFrameBuilder;

struct PositionFrameArray;
struct PositionFrameArrayBuilder;

struct SplitProp;
struct SplitPropBuilder;

struct Transform;
struct TransformBuilder;

struct EllipseShape;
struct EllipseShapeBuilder;

struct RectShape;
struct RectShapeBuilder;

struct PathShape;
struct PathShapeBuilder;

struct FillStyle;
struct FillStyleBuilder;

struct StrokeStyle;
struct StrokeStyleBuilder;

struct GradientFillStyle;
struct GradientFillStyleBuilder;

struct GradientStrokeStyle;
struct GradientStrokeStyleBuilder;

struct TrimPath;
struct TrimPathBuilder;

struct TransformShape;
struct TransformShapeBuilder;

struct GroupShape;
struct GroupShapeBuilder;

struct BaseLayer;
struct BaseLayerBuilder;

struct ShapeLayer;
struct ShapeLayerBuilder;

struct PrecompositionLayer;
struct PrecompositionLayerBuilder;

struct NullLayer;
struct NullLayerBuilder;

struct Rect;

struct TextDocument;
struct TextDocumentBuilder;

struct TextDocumentFrame;
struct TextDocumentFrameBuilder;

struct TextData;
struct TextDataBuilder;

struct TextLayer;
struct TextLayerBuilder;

struct ImageLayer;
struct ImageLayerBuilder;

struct SolidLayer;
struct SolidLayerBuilder;

struct Asset;
struct AssetBuilder;

struct Animations;
struct AnimationsBuilder;

enum FloatProp : uint8_t {
    FloatProp_NONE = 0,
    FloatProp_frames = 1,
    FloatProp_value = 2,
    FloatProp_MIN = FloatProp_NONE,
    FloatProp_MAX = FloatProp_value
};

inline const FloatProp (&EnumValuesFloatProp())[3]
{
    static const FloatProp values[] = {FloatProp_NONE, FloatProp_frames, FloatProp_value};
    return values;
}

inline const char *const *EnumNamesFloatProp()
{
    static const char *const names[4] = {"NONE", "frames", "value", nullptr};
    return names;
}

inline const char *EnumNameFloatProp(FloatProp e)
{
    if (::flatbuffers::IsOutRange(e, FloatProp_NONE, FloatProp_value))
        return "";
    const size_t index = static_cast<size_t>(e);
    return EnumNamesFloatProp()[index];
}

template <typename T>
struct FloatPropTraits {
    static const FloatProp enum_value = FloatProp_NONE;
};

template <>
struct FloatPropTraits<LottieFlatBuffer::FloatFrameArray> {
    static const FloatProp enum_value = FloatProp_frames;
};

template <>
struct FloatPropTraits<LottieFlatBuffer::FloatScalar> {
    static const FloatProp enum_value = FloatProp_value;
};

bool VerifyFloatProp(::flatbuffers::Verifier &verifier, const void *obj, FloatProp type);
bool VerifyFloatPropVector(::flatbuffers::Verifier &verifier,
                           const ::flatbuffers::Vector<::flatbuffers::Offset<void>> *values,
                           const ::flatbuffers::Vector<uint8_t> *types);

enum IntegerProp : uint8_t {
    IntegerProp_NONE = 0,
    IntegerProp_frames = 1,
    IntegerProp_value = 2,
    IntegerProp_MIN = IntegerProp_NONE,
    IntegerProp_MAX = IntegerProp_value
};

inline const IntegerProp (&EnumValuesIntegerProp())[3]
{
    static const IntegerProp values[] = {IntegerProp_NONE, IntegerProp_frames, IntegerProp_value};
    return values;
}

inline const char *const *EnumNamesIntegerProp()
{
    static const char *const names[4] = {"NONE", "frames", "value", nullptr};
    return names;
}

inline const char *EnumNameIntegerProp(IntegerProp e)
{
    if (::flatbuffers::IsOutRange(e, IntegerProp_NONE, IntegerProp_value))
        return "";
    const size_t index = static_cast<size_t>(e);
    return EnumNamesIntegerProp()[index];
}

template <typename T>
struct IntegerPropTraits {
    static const IntegerProp enum_value = IntegerProp_NONE;
};

template <>
struct IntegerPropTraits<LottieFlatBuffer::IntegerFrameArray> {
    static const IntegerProp enum_value = IntegerProp_frames;
};

template <>
struct IntegerPropTraits<LottieFlatBuffer::IntegerScalar> {
    static const IntegerProp enum_value = IntegerProp_value;
};

bool VerifyIntegerProp(::flatbuffers::Verifier &verifier, const void *obj, IntegerProp type);
bool VerifyIntegerPropVector(::flatbuffers::Verifier &verifier,
                             const ::flatbuffers::Vector<::flatbuffers::Offset<void>> *values,
                             const ::flatbuffers::Vector<uint8_t> *types);

enum VectorProp : uint8_t {
    VectorProp_NONE = 0,
    VectorProp_frames = 1,
    VectorProp_value = 2,
    VectorProp_MIN = VectorProp_NONE,
    VectorProp_MAX = VectorProp_value
};

inline const VectorProp (&EnumValuesVectorProp())[3]
{
    static const VectorProp values[] = {VectorProp_NONE, VectorProp_frames, VectorProp_value};
    return values;
}

inline const char *const *EnumNamesVectorProp()
{
    static const char *const names[4] = {"NONE", "frames", "value", nullptr};
    return names;
}

inline const char *EnumNameVectorProp(VectorProp e)
{
    if (::flatbuffers::IsOutRange(e, VectorProp_NONE, VectorProp_value))
        return "";
    const size_t index = static_cast<size_t>(e);
    return EnumNamesVectorProp()[index];
}

template <typename T>
struct VectorPropTraits {
    static const VectorProp enum_value = VectorProp_NONE;
};

template <>
struct VectorPropTraits<LottieFlatBuffer::VectorFrameArray> {
    static const VectorProp enum_value = VectorProp_frames;
};

template <>
struct VectorPropTraits<LottieFlatBuffer::Point> {
    static const VectorProp enum_value = VectorProp_value;
};

bool VerifyVectorProp(::flatbuffers::Verifier &verifier, const void *obj, VectorProp type);
bool VerifyVectorPropVector(::flatbuffers::Verifier &verifier,
                            const ::flatbuffers::Vector<::flatbuffers::Offset<void>> *values,
                            const ::flatbuffers::Vector<uint8_t> *types);

enum ColorProp : uint8_t {
    ColorProp_NONE = 0,
    ColorProp_frames = 1,
    ColorProp_color = 2,
    ColorProp_MIN = ColorProp_NONE,
    ColorProp_MAX = ColorProp_color
};

inline const ColorProp (&EnumValuesColorProp())[3]
{
    static const ColorProp values[] = {ColorProp_NONE, ColorProp_frames, ColorProp_color};
    return values;
}

inline const char *const *EnumNamesColorProp()
{
    static const char *const names[4] = {"NONE", "frames", "color", nullptr};
    return names;
}

inline const char *EnumNameColorProp(ColorProp e)
{
    if (::flatbuffers::IsOutRange(e, ColorProp_NONE, ColorProp_color))
        return "";
    const size_t index = static_cast<size_t>(e);
    return EnumNamesColorProp()[index];
}

template <typename T>
struct ColorPropTraits {
    static const ColorProp enum_value = ColorProp_NONE;
};

template <>
struct ColorPropTraits<LottieFlatBuffer::ColorFrameArray> {
    static const ColorProp enum_value = ColorProp_frames;
};

template <>
struct ColorPropTraits<LottieFlatBuffer::Color24Scalar> {
    static const ColorProp enum_value = ColorProp_color;
};

bool VerifyColorProp(::flatbuffers::Verifier &verifier, const void *obj, ColorProp type);
bool VerifyColorPropVector(::flatbuffers::Verifier &verifier,
                           const ::flatbuffers::Vector<::flatbuffers::Offset<void>> *values,
                           const ::flatbuffers::Vector<uint8_t> *types);

enum GradientProp : uint8_t {
    GradientProp_NONE = 0,
    GradientProp_frames = 1,
    GradientProp_value = 2,
    GradientProp_MIN = GradientProp_NONE,
    GradientProp_MAX = GradientProp_value
};

inline const GradientProp (&EnumValuesGradientProp())[3]
{
    static const GradientProp values[] = {GradientProp_NONE, GradientProp_frames, GradientProp_value};
    return values;
}

inline const char *const *EnumNamesGradientProp()
{
    static const char *const names[4] = {"NONE", "frames", "value", nullptr};
    return names;
}

inline const char *EnumNameGradientProp(GradientProp e)
{
    if (::flatbuffers::IsOutRange(e, GradientProp_NONE, GradientProp_value))
        return "";
    const size_t index = static_cast<size_t>(e);
    return EnumNamesGradientProp()[index];
}

template <typename T>
struct GradientPropTraits {
    static const GradientProp enum_value = GradientProp_NONE;
};

template <>
struct GradientPropTraits<LottieFlatBuffer::GradientFrameArray> {
    static const GradientProp enum_value = GradientProp_frames;
};

template <>
struct GradientPropTraits<LottieFlatBuffer::Gradient> {
    static const GradientProp enum_value = GradientProp_value;
};

bool VerifyGradientProp(::flatbuffers::Verifier &verifier, const void *obj, GradientProp type);
bool VerifyGradientPropVector(::flatbuffers::Verifier &verifier,
                              const ::flatbuffers::Vector<::flatbuffers::Offset<void>> *values,
                              const ::flatbuffers::Vector<uint8_t> *types);

enum BezierDataProp : uint8_t {
    BezierDataProp_NONE = 0,
    BezierDataProp_frames = 1,
    BezierDataProp_value = 2,
    BezierDataProp_MIN = BezierDataProp_NONE,
    BezierDataProp_MAX = BezierDataProp_value
};

inline const BezierDataProp (&EnumValuesBezierDataProp())[3]
{
    static const BezierDataProp values[] = {BezierDataProp_NONE, BezierDataProp_frames, BezierDataProp_value};
    return values;
}

inline const char *const *EnumNamesBezierDataProp()
{
    static const char *const names[4] = {"NONE", "frames", "value", nullptr};
    return names;
}

inline const char *EnumNameBezierDataProp(BezierDataProp e)
{
    if (::flatbuffers::IsOutRange(e, BezierDataProp_NONE, BezierDataProp_value))
        return "";
    const size_t index = static_cast<size_t>(e);
    return EnumNamesBezierDataProp()[index];
}

template <typename T>
struct BezierDataPropTraits {
    static const BezierDataProp enum_value = BezierDataProp_NONE;
};

template <>
struct BezierDataPropTraits<LottieFlatBuffer::BezierDataFrameArray> {
    static const BezierDataProp enum_value = BezierDataProp_frames;
};

template <>
struct BezierDataPropTraits<LottieFlatBuffer::BezierData> {
    static const BezierDataProp enum_value = BezierDataProp_value;
};

bool VerifyBezierDataProp(::flatbuffers::Verifier &verifier, const void *obj, BezierDataProp type);
bool VerifyBezierDataPropVector(::flatbuffers::Verifier &verifier,
                                const ::flatbuffers::Vector<::flatbuffers::Offset<void>> *values,
                                const ::flatbuffers::Vector<uint8_t> *types);

enum PositionProp : uint8_t {
    PositionProp_NONE = 0,
    PositionProp_frames = 1,
    PositionProp_value = 2,
    PositionProp_MIN = PositionProp_NONE,
    PositionProp_MAX = PositionProp_value
};

inline const PositionProp (&EnumValuesPositionProp())[3]
{
    static const PositionProp values[] = {PositionProp_NONE, PositionProp_frames, PositionProp_value};
    return values;
}

inline const char *const *EnumNamesPositionProp()
{
    static const char *const names[4] = {"NONE", "frames", "value", nullptr};
    return names;
}

inline const char *EnumNamePositionProp(PositionProp e)
{
    if (::flatbuffers::IsOutRange(e, PositionProp_NONE, PositionProp_value))
        return "";
    const size_t index = static_cast<size_t>(e);
    return EnumNamesPositionProp()[index];
}

template <typename T>
struct PositionPropTraits {
    static const PositionProp enum_value = PositionProp_NONE;
};

template <>
struct PositionPropTraits<LottieFlatBuffer::PositionFrameArray> {
    static const PositionProp enum_value = PositionProp_frames;
};

template <>
struct PositionPropTraits<LottieFlatBuffer::Point> {
    static const PositionProp enum_value = PositionProp_value;
};

bool VerifyPositionProp(::flatbuffers::Verifier &verifier, const void *obj, PositionProp type);
bool VerifyPositionPropVector(::flatbuffers::Verifier &verifier,
                              const ::flatbuffers::Vector<::flatbuffers::Offset<void>> *values,
                              const ::flatbuffers::Vector<uint8_t> *types);

enum FillRule : int8_t {
    FillRule_NON_ZERO = 0,
    FillRule_EVEN_ODD = 1,
    FillRule_MIN = FillRule_NON_ZERO,
    FillRule_MAX = FillRule_EVEN_ODD
};

inline const FillRule (&EnumValuesFillRule())[2]
{
    static const FillRule values[] = {FillRule_NON_ZERO, FillRule_EVEN_ODD};
    return values;
}

inline const char *const *EnumNamesFillRule()
{
    static const char *const names[3] = {"NON_ZERO", "EVEN_ODD", nullptr};
    return names;
}

inline const char *EnumNameFillRule(FillRule e)
{
    if (::flatbuffers::IsOutRange(e, FillRule_NON_ZERO, FillRule_EVEN_ODD))
        return "";
    const size_t index = static_cast<size_t>(e);
    return EnumNamesFillRule()[index];
}

enum LineCap : int8_t {
    LineCap_CAP_BUTT = 0,
    LineCap_CAP_ROUND = 1,
    LineCap_CAP_SQUARE = 2,
    LineCap_MIN = LineCap_CAP_BUTT,
    LineCap_MAX = LineCap_CAP_SQUARE
};

inline const LineCap (&EnumValuesLineCap())[3]
{
    static const LineCap values[] = {LineCap_CAP_BUTT, LineCap_CAP_ROUND, LineCap_CAP_SQUARE};
    return values;
}

inline const char *const *EnumNamesLineCap()
{
    static const char *const names[4] = {"CAP_BUTT", "CAP_ROUND", "CAP_SQUARE", nullptr};
    return names;
}

inline const char *EnumNameLineCap(LineCap e)
{
    if (::flatbuffers::IsOutRange(e, LineCap_CAP_BUTT, LineCap_CAP_SQUARE))
        return "";
    const size_t index = static_cast<size_t>(e);
    return EnumNamesLineCap()[index];
}

enum LineJoin : int8_t {
    LineJoin_JOIN_MITER = 0,
    LineJoin_JOIN_ROUND = 1,
    LineJoin_JOIN_BEVEL = 2,
    LineJoin_MIN = LineJoin_JOIN_MITER,
    LineJoin_MAX = LineJoin_JOIN_BEVEL
};

inline const LineJoin (&EnumValuesLineJoin())[3]
{
    static const LineJoin values[] = {LineJoin_JOIN_MITER, LineJoin_JOIN_ROUND, LineJoin_JOIN_BEVEL};
    return values;
}

inline const char *const *EnumNamesLineJoin()
{
    static const char *const names[4] = {"JOIN_MITER", "JOIN_ROUND", "JOIN_BEVEL", nullptr};
    return names;
}

inline const char *EnumNameLineJoin(LineJoin e)
{
    if (::flatbuffers::IsOutRange(e, LineJoin_JOIN_MITER, LineJoin_JOIN_BEVEL))
        return "";
    const size_t index = static_cast<size_t>(e);
    return EnumNamesLineJoin()[index];
}

enum GradientType : int8_t {
    GradientType_GRADIENT_LINEAR = 0,
    GradientType_GRADIENT_RADIAL = 1,
    GradientType_MIN = GradientType_GRADIENT_LINEAR,
    GradientType_MAX = GradientType_GRADIENT_RADIAL
};

inline const GradientType (&EnumValuesGradientType())[2]
{
    static const GradientType values[] = {GradientType_GRADIENT_LINEAR, GradientType_GRADIENT_RADIAL};
    return values;
}

inline const char *const *EnumNamesGradientType()
{
    static const char *const names[3] = {"GRADIENT_LINEAR", "GRADIENT_RADIAL", nullptr};
    return names;
}

inline const char *EnumNameGradientType(GradientType e)
{
    if (::flatbuffers::IsOutRange(e, GradientType_GRADIENT_LINEAR, GradientType_GRADIENT_RADIAL))
        return "";
    const size_t index = static_cast<size_t>(e);
    return EnumNamesGradientType()[index];
}

enum TrimType : int8_t {
    TrimType_Simultaneous = 0,
    TrimType_Individual = 1,
    TrimType_MIN = TrimType_Simultaneous,
    TrimType_MAX = TrimType_Individual
};

inline const TrimType (&EnumValuesTrimType())[2]
{
    static const TrimType values[] = {TrimType_Simultaneous, TrimType_Individual};
    return values;
}

inline const char *const *EnumNamesTrimType()
{
    static const char *const names[3] = {"Simultaneous", "Individual", nullptr};
    return names;
}

inline const char *EnumNameTrimType(TrimType e)
{
    if (::flatbuffers::IsOutRange(e, TrimType_Simultaneous, TrimType_Individual))
        return "";
    const size_t index = static_cast<size_t>(e);
    return EnumNamesTrimType()[index];
}

enum GraphicElement : uint8_t {
    GraphicElement_NONE = 0,
    GraphicElement_EllipseShape = 1,
    GraphicElement_RectShape = 2,
    GraphicElement_PathShape = 3,
    GraphicElement_FillStyle = 4,
    GraphicElement_StrokeStyle = 5,
    GraphicElement_GradientFillStyle = 6,
    GraphicElement_GradientStrokeStyle = 7,
    GraphicElement_TransformShape = 8,
    GraphicElement_TrimPath = 9,
    GraphicElement_GroupShape = 10,
    GraphicElement_MIN = GraphicElement_NONE,
    GraphicElement_MAX = GraphicElement_GroupShape
};

inline const GraphicElement (&EnumValuesGraphicElement())[11]
{
    static const GraphicElement values[] = {GraphicElement_NONE,
                                            GraphicElement_EllipseShape,
                                            GraphicElement_RectShape,
                                            GraphicElement_PathShape,
                                            GraphicElement_FillStyle,
                                            GraphicElement_StrokeStyle,
                                            GraphicElement_GradientFillStyle,
                                            GraphicElement_GradientStrokeStyle,
                                            GraphicElement_TransformShape,
                                            GraphicElement_TrimPath,
                                            GraphicElement_GroupShape};
    return values;
}

inline const char *const *EnumNamesGraphicElement()
{
    static const char *const names[12] = {"NONE",           "EllipseShape", "RectShape",         "PathShape",
                                          "FillStyle",      "StrokeStyle",  "GradientFillStyle", "GradientStrokeStyle",
                                          "TransformShape", "TrimPath",     "GroupShape",        nullptr};
    return names;
}

inline const char *EnumNameGraphicElement(GraphicElement e)
{
    if (::flatbuffers::IsOutRange(e, GraphicElement_NONE, GraphicElement_GroupShape))
        return "";
    const size_t index = static_cast<size_t>(e);
    return EnumNamesGraphicElement()[index];
}

template <typename T>
struct GraphicElementTraits {
    static const GraphicElement enum_value = GraphicElement_NONE;
};

template <>
struct GraphicElementTraits<LottieFlatBuffer::EllipseShape> {
    static const GraphicElement enum_value = GraphicElement_EllipseShape;
};

template <>
struct GraphicElementTraits<LottieFlatBuffer::RectShape> {
    static const GraphicElement enum_value = GraphicElement_RectShape;
};

template <>
struct GraphicElementTraits<LottieFlatBuffer::PathShape> {
    static const GraphicElement enum_value = GraphicElement_PathShape;
};

template <>
struct GraphicElementTraits<LottieFlatBuffer::FillStyle> {
    static const GraphicElement enum_value = GraphicElement_FillStyle;
};

template <>
struct GraphicElementTraits<LottieFlatBuffer::StrokeStyle> {
    static const GraphicElement enum_value = GraphicElement_StrokeStyle;
};

template <>
struct GraphicElementTraits<LottieFlatBuffer::GradientFillStyle> {
    static const GraphicElement enum_value = GraphicElement_GradientFillStyle;
};

template <>
struct GraphicElementTraits<LottieFlatBuffer::GradientStrokeStyle> {
    static const GraphicElement enum_value = GraphicElement_GradientStrokeStyle;
};

template <>
struct GraphicElementTraits<LottieFlatBuffer::TransformShape> {
    static const GraphicElement enum_value = GraphicElement_TransformShape;
};

template <>
struct GraphicElementTraits<LottieFlatBuffer::TrimPath> {
    static const GraphicElement enum_value = GraphicElement_TrimPath;
};

template <>
struct GraphicElementTraits<LottieFlatBuffer::GroupShape> {
    static const GraphicElement enum_value = GraphicElement_GroupShape;
};

bool VerifyGraphicElement(::flatbuffers::Verifier &verifier, const void *obj, GraphicElement type);
bool VerifyGraphicElementVector(::flatbuffers::Verifier &verifier,
                                const ::flatbuffers::Vector<::flatbuffers::Offset<void>> *values,
                                const ::flatbuffers::Vector<uint8_t> *types);

enum BlendMode : int8_t {
    BlendMode_Normal = 0,
    BlendMode_Overlay = 1,
    BlendMode_Multiply = 2,
    BlendMode_Add = 3,
    BlendMode_MIN = BlendMode_Normal,
    BlendMode_MAX = BlendMode_Add
};

inline const BlendMode (&EnumValuesBlendMode())[4]
{
    static const BlendMode values[] = {BlendMode_Normal, BlendMode_Overlay, BlendMode_Multiply, BlendMode_Add};
    return values;
}

inline const char *const *EnumNamesBlendMode()
{
    static const char *const names[5] = {"Normal", "Overlay", "Multiply", "Add", nullptr};
    return names;
}

inline const char *EnumNameBlendMode(BlendMode e)
{
    if (::flatbuffers::IsOutRange(e, BlendMode_Normal, BlendMode_Add))
        return "";
    const size_t index = static_cast<size_t>(e);
    return EnumNamesBlendMode()[index];
}

enum TextAlign : int8_t {
    TextAlign_AlignLeft = 1,
    TextAlign_AlignRight = 2,
    TextAlign_AlignCenter = 3,
    TextAlign_MIN = TextAlign_AlignLeft,
    TextAlign_MAX = TextAlign_AlignCenter
};

inline const TextAlign (&EnumValuesTextAlign())[3]
{
    static const TextAlign values[] = {TextAlign_AlignLeft, TextAlign_AlignRight, TextAlign_AlignCenter};
    return values;
}

inline const char *const *EnumNamesTextAlign()
{
    static const char *const names[4] = {"AlignLeft", "AlignRight", "AlignCenter", nullptr};
    return names;
}

inline const char *EnumNameTextAlign(TextAlign e)
{
    if (::flatbuffers::IsOutRange(e, TextAlign_AlignLeft, TextAlign_AlignCenter))
        return "";
    const size_t index = static_cast<size_t>(e) - static_cast<size_t>(TextAlign_AlignLeft);
    return EnumNamesTextAlign()[index];
}

enum Layer : uint8_t {
    Layer_NONE = 0,
    Layer_PrecompositionLayer = 1,
    Layer_SolidLayer = 2,
    Layer_ImageLayer = 3,
    Layer_NullLayer = 4,
    Layer_ShapeLayer = 5,
    Layer_TextLayer = 6,
    Layer_MIN = Layer_NONE,
    Layer_MAX = Layer_TextLayer
};

inline const Layer (&EnumValuesLayer())[7]
{
    static const Layer values[] = {Layer_NONE,      Layer_PrecompositionLayer, Layer_SolidLayer, Layer_ImageLayer,
                                   Layer_NullLayer, Layer_ShapeLayer,          Layer_TextLayer};
    return values;
}

inline const char *const *EnumNamesLayer()
{
    static const char *const names[8] = {"NONE",      "PrecompositionLayer", "SolidLayer", "ImageLayer",
                                         "NullLayer", "ShapeLayer",          "TextLayer",  nullptr};
    return names;
}

inline const char *EnumNameLayer(Layer e)
{
    if (::flatbuffers::IsOutRange(e, Layer_NONE, Layer_TextLayer))
        return "";
    const size_t index = static_cast<size_t>(e);
    return EnumNamesLayer()[index];
}

template <typename T>
struct LayerTraits {
    static const Layer enum_value = Layer_NONE;
};

template <>
struct LayerTraits<LottieFlatBuffer::PrecompositionLayer> {
    static const Layer enum_value = Layer_PrecompositionLayer;
};

template <>
struct LayerTraits<LottieFlatBuffer::SolidLayer> {
    static const Layer enum_value = Layer_SolidLayer;
};

template <>
struct LayerTraits<LottieFlatBuffer::ImageLayer> {
    static const Layer enum_value = Layer_ImageLayer;
};

template <>
struct LayerTraits<LottieFlatBuffer::NullLayer> {
    static const Layer enum_value = Layer_NullLayer;
};

template <>
struct LayerTraits<LottieFlatBuffer::ShapeLayer> {
    static const Layer enum_value = Layer_ShapeLayer;
};

template <>
struct LayerTraits<LottieFlatBuffer::TextLayer> {
    static const Layer enum_value = Layer_TextLayer;
};

bool VerifyLayer(::flatbuffers::Verifier &verifier, const void *obj, Layer type);
bool VerifyLayerVector(::flatbuffers::Verifier &verifier,
                       const ::flatbuffers::Vector<::flatbuffers::Offset<void>> *values,
                       const ::flatbuffers::Vector<uint8_t> *types);

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(4) Point FLATBUFFERS_FINAL_CLASS
{
private:
    float x_;
    float y_;

public:
    Point() : x_(0), y_(0) {}
    Point(float _x, float _y) : x_(::flatbuffers::EndianScalar(_x)), y_(::flatbuffers::EndianScalar(_y)) {}
    float x() const
    {
        return ::flatbuffers::EndianScalar(x_);
    }
    float y() const
    {
        return ::flatbuffers::EndianScalar(y_);
    }
};
FLATBUFFERS_STRUCT_END(Point, 8);

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(1) Color32 FLATBUFFERS_FINAL_CLASS
{
private:
    uint8_t r_;
    uint8_t g_;
    uint8_t b_;
    uint8_t a_;

public:
    Color32() : r_(0), g_(0), b_(0), a_(0) {}
    Color32(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a)
        : r_(::flatbuffers::EndianScalar(_r)),
          g_(::flatbuffers::EndianScalar(_g)),
          b_(::flatbuffers::EndianScalar(_b)),
          a_(::flatbuffers::EndianScalar(_a))
    {
    }
    uint8_t r() const
    {
        return ::flatbuffers::EndianScalar(r_);
    }
    uint8_t g() const
    {
        return ::flatbuffers::EndianScalar(g_);
    }
    uint8_t b() const
    {
        return ::flatbuffers::EndianScalar(b_);
    }
    uint8_t a() const
    {
        return ::flatbuffers::EndianScalar(a_);
    }
};
FLATBUFFERS_STRUCT_END(Color32, 4);

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(1) Color24 FLATBUFFERS_FINAL_CLASS
{
private:
    uint8_t r_;
    uint8_t g_;
    uint8_t b_;

public:
    Color24() : r_(0), g_(0), b_(0) {}
    Color24(uint8_t _r, uint8_t _g, uint8_t _b)
        : r_(::flatbuffers::EndianScalar(_r)),
          g_(::flatbuffers::EndianScalar(_g)),
          b_(::flatbuffers::EndianScalar(_b))
    {
    }
    uint8_t r() const
    {
        return ::flatbuffers::EndianScalar(r_);
    }
    uint8_t g() const
    {
        return ::flatbuffers::EndianScalar(g_);
    }
    uint8_t b() const
    {
        return ::flatbuffers::EndianScalar(b_);
    }
};
FLATBUFFERS_STRUCT_END(Color24, 3);

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(4) ColorStop FLATBUFFERS_FINAL_CLASS
{
private:
    float stop_;
    LottieFlatBuffer::Color32 color_;

public:
    ColorStop() : stop_(0), color_() {}
    ColorStop(float _stop, const LottieFlatBuffer::Color32 &_color)
        : stop_(::flatbuffers::EndianScalar(_stop)),
          color_(_color)
    {
    }
    float stop() const
    {
        return ::flatbuffers::EndianScalar(stop_);
    }
    const LottieFlatBuffer::Color32 &color() const
    {
        return color_;
    }
};
FLATBUFFERS_STRUCT_END(ColorStop, 8);

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(4) Rect FLATBUFFERS_FINAL_CLASS
{
private:
    LottieFlatBuffer::Point pos_;
    LottieFlatBuffer::Point size_;

public:
    Rect() : pos_(), size_() {}
    Rect(const LottieFlatBuffer::Point &_pos, const LottieFlatBuffer::Point &_size) : pos_(_pos), size_(_size) {}
    const LottieFlatBuffer::Point &pos() const
    {
        return pos_;
    }
    const LottieFlatBuffer::Point &size() const
    {
        return size_;
    }
};
FLATBUFFERS_STRUCT_END(Rect, 16);

struct Gradient FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef GradientBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE { VT_STOPS = 4 };
    const ::flatbuffers::Vector<const LottieFlatBuffer::ColorStop *> *stops() const
    {
        return GetPointer<const ::flatbuffers::Vector<const LottieFlatBuffer::ColorStop *> *>(VT_STOPS);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyOffsetRequired(verifier, VT_STOPS) &&
               verifier.VerifyVector(stops()) && verifier.EndTable();
    }
};

struct GradientBuilder {
    typedef Gradient Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_stops(::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::ColorStop *>> stops)
    {
        fbb_.AddOffset(Gradient::VT_STOPS, stops);
    }
    explicit GradientBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<Gradient> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<Gradient>(end);
        fbb_.Required(o, Gradient::VT_STOPS);
        return o;
    }
};

inline ::flatbuffers::Offset<Gradient> CreateGradient(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::ColorStop *>> stops = 0)
{
    GradientBuilder builder_(_fbb);
    builder_.add_stops(stops);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<Gradient> CreateGradientDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb, const std::vector<LottieFlatBuffer::ColorStop> *stops = nullptr)
{
    auto stops__ = stops ? _fbb.CreateVectorOfStructs<LottieFlatBuffer::ColorStop>(*stops) : 0;
    return LottieFlatBuffer::CreateGradient(_fbb, stops__);
}

struct FloatFrame FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef FloatFrameBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_TIME = 4,
        VT_START = 6,
        VT_HOLD = 8,
        VT_TIME_EASING = 10,
        VT_VALUE_EASING = 12
    };
    float time() const
    {
        return GetField<float>(VT_TIME, 0.0f);
    }
    float start() const
    {
        return GetField<float>(VT_START, 0.0f);
    }
    bool hold() const
    {
        return GetField<uint8_t>(VT_HOLD, 0) != 0;
    }
    const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *time_easing() const
    {
        return GetPointer<const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *>(VT_TIME_EASING);
    }
    const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *value_easing() const
    {
        return GetPointer<const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *>(VT_VALUE_EASING);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyField<float>(verifier, VT_TIME, 4) &&
               VerifyField<float>(verifier, VT_START, 4) && VerifyField<uint8_t>(verifier, VT_HOLD, 1) &&
               VerifyOffset(verifier, VT_TIME_EASING) && verifier.VerifyVector(time_easing()) &&
               VerifyOffset(verifier, VT_VALUE_EASING) && verifier.VerifyVector(value_easing()) && verifier.EndTable();
    }
};

struct FloatFrameBuilder {
    typedef FloatFrame Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_time(float time)
    {
        fbb_.AddElement<float>(FloatFrame::VT_TIME, time, 0.0f);
    }
    void add_start(float start)
    {
        fbb_.AddElement<float>(FloatFrame::VT_START, start, 0.0f);
    }
    void add_hold(bool hold)
    {
        fbb_.AddElement<uint8_t>(FloatFrame::VT_HOLD, static_cast<uint8_t>(hold), 0);
    }
    void add_time_easing(::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> time_easing)
    {
        fbb_.AddOffset(FloatFrame::VT_TIME_EASING, time_easing);
    }
    void add_value_easing(::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> value_easing)
    {
        fbb_.AddOffset(FloatFrame::VT_VALUE_EASING, value_easing);
    }
    explicit FloatFrameBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<FloatFrame> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<FloatFrame>(end);
        return o;
    }
};

inline ::flatbuffers::Offset<FloatFrame> CreateFloatFrame(
    ::flatbuffers::FlatBufferBuilder &_fbb, float time = 0.0f, float start = 0.0f, bool hold = false,
    ::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> time_easing = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> value_easing = 0)
{
    FloatFrameBuilder builder_(_fbb);
    builder_.add_value_easing(value_easing);
    builder_.add_time_easing(time_easing);
    builder_.add_start(start);
    builder_.add_time(time);
    builder_.add_hold(hold);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<FloatFrame> CreateFloatFrameDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb, float time = 0.0f, float start = 0.0f, bool hold = false,
    const std::vector<LottieFlatBuffer::Point> *time_easing = nullptr,
    const std::vector<LottieFlatBuffer::Point> *value_easing = nullptr)
{
    auto time_easing__ = time_easing ? _fbb.CreateVectorOfStructs<LottieFlatBuffer::Point>(*time_easing) : 0;
    auto value_easing__ = value_easing ? _fbb.CreateVectorOfStructs<LottieFlatBuffer::Point>(*value_easing) : 0;
    return LottieFlatBuffer::CreateFloatFrame(_fbb, time, start, hold, time_easing__, value_easing__);
}

struct FloatFrameArray FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef FloatFrameArrayBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE { VT_FRAMES = 4 };
    const ::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::FloatFrame>> *frames() const
    {
        return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::FloatFrame>> *>(
            VT_FRAMES);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyOffset(verifier, VT_FRAMES) && verifier.VerifyVector(frames()) &&
               verifier.VerifyVectorOfTables(frames()) && verifier.EndTable();
    }
};

struct FloatFrameArrayBuilder {
    typedef FloatFrameArray Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_frames(
        ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::FloatFrame>>> frames)
    {
        fbb_.AddOffset(FloatFrameArray::VT_FRAMES, frames);
    }
    explicit FloatFrameArrayBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<FloatFrameArray> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<FloatFrameArray>(end);
        return o;
    }
};

inline ::flatbuffers::Offset<FloatFrameArray> CreateFloatFrameArray(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::FloatFrame>>> frames = 0)
{
    FloatFrameArrayBuilder builder_(_fbb);
    builder_.add_frames(frames);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<FloatFrameArray> CreateFloatFrameArrayDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<::flatbuffers::Offset<LottieFlatBuffer::FloatFrame>> *frames = nullptr)
{
    auto frames__ = frames ? _fbb.CreateVector<::flatbuffers::Offset<LottieFlatBuffer::FloatFrame>>(*frames) : 0;
    return LottieFlatBuffer::CreateFloatFrameArray(_fbb, frames__);
}

struct FloatScalar FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef FloatScalarBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE { VT_VALUE = 4 };
    float value() const
    {
        return GetField<float>(VT_VALUE, 0.0f);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyField<float>(verifier, VT_VALUE, 4) && verifier.EndTable();
    }
};

struct FloatScalarBuilder {
    typedef FloatScalar Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_value(float value)
    {
        fbb_.AddElement<float>(FloatScalar::VT_VALUE, value, 0.0f);
    }
    explicit FloatScalarBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<FloatScalar> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<FloatScalar>(end);
        return o;
    }
};

inline ::flatbuffers::Offset<FloatScalar> CreateFloatScalar(::flatbuffers::FlatBufferBuilder &_fbb, float value = 0.0f)
{
    FloatScalarBuilder builder_(_fbb);
    builder_.add_value(value);
    return builder_.Finish();
}

struct IntegerFrame FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef IntegerFrameBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_TIME = 4,
        VT_START = 6,
        VT_HOLD = 8,
        VT_TIME_EASING = 10,
        VT_VALUE_EASING = 12
    };
    float time() const
    {
        return GetField<float>(VT_TIME, 0.0f);
    }
    uint32_t start() const
    {
        return GetField<uint32_t>(VT_START, 0);
    }
    bool hold() const
    {
        return GetField<uint8_t>(VT_HOLD, 0) != 0;
    }
    const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *time_easing() const
    {
        return GetPointer<const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *>(VT_TIME_EASING);
    }
    const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *value_easing() const
    {
        return GetPointer<const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *>(VT_VALUE_EASING);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyField<float>(verifier, VT_TIME, 4) &&
               VerifyField<uint32_t>(verifier, VT_START, 4) && VerifyField<uint8_t>(verifier, VT_HOLD, 1) &&
               VerifyOffset(verifier, VT_TIME_EASING) && verifier.VerifyVector(time_easing()) &&
               VerifyOffset(verifier, VT_VALUE_EASING) && verifier.VerifyVector(value_easing()) && verifier.EndTable();
    }
};

struct IntegerFrameBuilder {
    typedef IntegerFrame Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_time(float time)
    {
        fbb_.AddElement<float>(IntegerFrame::VT_TIME, time, 0.0f);
    }
    void add_start(uint32_t start)
    {
        fbb_.AddElement<uint32_t>(IntegerFrame::VT_START, start, 0);
    }
    void add_hold(bool hold)
    {
        fbb_.AddElement<uint8_t>(IntegerFrame::VT_HOLD, static_cast<uint8_t>(hold), 0);
    }
    void add_time_easing(::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> time_easing)
    {
        fbb_.AddOffset(IntegerFrame::VT_TIME_EASING, time_easing);
    }
    void add_value_easing(::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> value_easing)
    {
        fbb_.AddOffset(IntegerFrame::VT_VALUE_EASING, value_easing);
    }
    explicit IntegerFrameBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<IntegerFrame> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<IntegerFrame>(end);
        return o;
    }
};

inline ::flatbuffers::Offset<IntegerFrame> CreateIntegerFrame(
    ::flatbuffers::FlatBufferBuilder &_fbb, float time = 0.0f, uint32_t start = 0, bool hold = false,
    ::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> time_easing = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> value_easing = 0)
{
    IntegerFrameBuilder builder_(_fbb);
    builder_.add_value_easing(value_easing);
    builder_.add_time_easing(time_easing);
    builder_.add_start(start);
    builder_.add_time(time);
    builder_.add_hold(hold);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<IntegerFrame> CreateIntegerFrameDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb, float time = 0.0f, uint32_t start = 0, bool hold = false,
    const std::vector<LottieFlatBuffer::Point> *time_easing = nullptr,
    const std::vector<LottieFlatBuffer::Point> *value_easing = nullptr)
{
    auto time_easing__ = time_easing ? _fbb.CreateVectorOfStructs<LottieFlatBuffer::Point>(*time_easing) : 0;
    auto value_easing__ = value_easing ? _fbb.CreateVectorOfStructs<LottieFlatBuffer::Point>(*value_easing) : 0;
    return LottieFlatBuffer::CreateIntegerFrame(_fbb, time, start, hold, time_easing__, value_easing__);
}

struct IntegerFrameArray FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef IntegerFrameArrayBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE { VT_FRAMES = 4 };
    const ::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::IntegerFrame>> *frames() const
    {
        return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::IntegerFrame>> *>(
            VT_FRAMES);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyOffset(verifier, VT_FRAMES) && verifier.VerifyVector(frames()) &&
               verifier.VerifyVectorOfTables(frames()) && verifier.EndTable();
    }
};

struct IntegerFrameArrayBuilder {
    typedef IntegerFrameArray Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_frames(
        ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::IntegerFrame>>> frames)
    {
        fbb_.AddOffset(IntegerFrameArray::VT_FRAMES, frames);
    }
    explicit IntegerFrameArrayBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<IntegerFrameArray> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<IntegerFrameArray>(end);
        return o;
    }
};

inline ::flatbuffers::Offset<IntegerFrameArray> CreateIntegerFrameArray(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::IntegerFrame>>> frames = 0)
{
    IntegerFrameArrayBuilder builder_(_fbb);
    builder_.add_frames(frames);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<IntegerFrameArray> CreateIntegerFrameArrayDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<::flatbuffers::Offset<LottieFlatBuffer::IntegerFrame>> *frames = nullptr)
{
    auto frames__ = frames ? _fbb.CreateVector<::flatbuffers::Offset<LottieFlatBuffer::IntegerFrame>>(*frames) : 0;
    return LottieFlatBuffer::CreateIntegerFrameArray(_fbb, frames__);
}

struct IntegerScalar FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef IntegerScalarBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE { VT_VALUE = 4 };
    uint32_t value() const
    {
        return GetField<uint32_t>(VT_VALUE, 0);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyField<uint32_t>(verifier, VT_VALUE, 4) && verifier.EndTable();
    }
};

struct IntegerScalarBuilder {
    typedef IntegerScalar Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_value(uint32_t value)
    {
        fbb_.AddElement<uint32_t>(IntegerScalar::VT_VALUE, value, 0);
    }
    explicit IntegerScalarBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<IntegerScalar> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<IntegerScalar>(end);
        return o;
    }
};

inline ::flatbuffers::Offset<IntegerScalar> CreateIntegerScalar(::flatbuffers::FlatBufferBuilder &_fbb,
                                                                uint32_t value = 0)
{
    IntegerScalarBuilder builder_(_fbb);
    builder_.add_value(value);
    return builder_.Finish();
}

struct BezierData FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef BezierDataBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE { VT_CMD = 4, VT_DATA = 6 };
    const ::flatbuffers::Vector<uint8_t> *cmd() const
    {
        return GetPointer<const ::flatbuffers::Vector<uint8_t> *>(VT_CMD);
    }
    const ::flatbuffers::Vector<float> *data() const
    {
        return GetPointer<const ::flatbuffers::Vector<float> *>(VT_DATA);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyOffsetRequired(verifier, VT_CMD) && verifier.VerifyVector(cmd()) &&
               VerifyOffsetRequired(verifier, VT_DATA) && verifier.VerifyVector(data()) && verifier.EndTable();
    }
};

struct BezierDataBuilder {
    typedef BezierData Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_cmd(::flatbuffers::Offset<::flatbuffers::Vector<uint8_t>> cmd)
    {
        fbb_.AddOffset(BezierData::VT_CMD, cmd);
    }
    void add_data(::flatbuffers::Offset<::flatbuffers::Vector<float>> data)
    {
        fbb_.AddOffset(BezierData::VT_DATA, data);
    }
    explicit BezierDataBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<BezierData> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<BezierData>(end);
        fbb_.Required(o, BezierData::VT_CMD);
        fbb_.Required(o, BezierData::VT_DATA);
        return o;
    }
};

inline ::flatbuffers::Offset<BezierData> CreateBezierData(::flatbuffers::FlatBufferBuilder &_fbb,
                                                          ::flatbuffers::Offset<::flatbuffers::Vector<uint8_t>> cmd = 0,
                                                          ::flatbuffers::Offset<::flatbuffers::Vector<float>> data = 0)
{
    BezierDataBuilder builder_(_fbb);
    builder_.add_data(data);
    builder_.add_cmd(cmd);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<BezierData> CreateBezierDataDirect(::flatbuffers::FlatBufferBuilder &_fbb,
                                                                const std::vector<uint8_t> *cmd = nullptr,
                                                                const std::vector<float> *data = nullptr)
{
    auto cmd__ = cmd ? _fbb.CreateVector<uint8_t>(*cmd) : 0;
    auto data__ = data ? _fbb.CreateVector<float>(*data) : 0;
    return LottieFlatBuffer::CreateBezierData(_fbb, cmd__, data__);
}

struct VectorFrame FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef VectorFrameBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_TIME = 4,
        VT_START = 6,
        VT_HOLD = 8,
        VT_TIME_EASING = 10,
        VT_VALUE_EASING = 12
    };
    float time() const
    {
        return GetField<float>(VT_TIME, 0.0f);
    }
    const LottieFlatBuffer::Point *start() const
    {
        return GetStruct<const LottieFlatBuffer::Point *>(VT_START);
    }
    bool hold() const
    {
        return GetField<uint8_t>(VT_HOLD, 0) != 0;
    }
    const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *time_easing() const
    {
        return GetPointer<const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *>(VT_TIME_EASING);
    }
    const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *value_easing() const
    {
        return GetPointer<const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *>(VT_VALUE_EASING);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyField<float>(verifier, VT_TIME, 4) &&
               VerifyField<LottieFlatBuffer::Point>(verifier, VT_START, 4) &&
               VerifyField<uint8_t>(verifier, VT_HOLD, 1) && VerifyOffset(verifier, VT_TIME_EASING) &&
               verifier.VerifyVector(time_easing()) && VerifyOffset(verifier, VT_VALUE_EASING) &&
               verifier.VerifyVector(value_easing()) && verifier.EndTable();
    }
};

struct VectorFrameBuilder {
    typedef VectorFrame Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_time(float time)
    {
        fbb_.AddElement<float>(VectorFrame::VT_TIME, time, 0.0f);
    }
    void add_start(const LottieFlatBuffer::Point *start)
    {
        fbb_.AddStruct(VectorFrame::VT_START, start);
    }
    void add_hold(bool hold)
    {
        fbb_.AddElement<uint8_t>(VectorFrame::VT_HOLD, static_cast<uint8_t>(hold), 0);
    }
    void add_time_easing(::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> time_easing)
    {
        fbb_.AddOffset(VectorFrame::VT_TIME_EASING, time_easing);
    }
    void add_value_easing(::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> value_easing)
    {
        fbb_.AddOffset(VectorFrame::VT_VALUE_EASING, value_easing);
    }
    explicit VectorFrameBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<VectorFrame> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<VectorFrame>(end);
        return o;
    }
};

inline ::flatbuffers::Offset<VectorFrame> CreateVectorFrame(
    ::flatbuffers::FlatBufferBuilder &_fbb, float time = 0.0f, const LottieFlatBuffer::Point *start = nullptr,
    bool hold = false, ::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> time_easing = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> value_easing = 0)
{
    VectorFrameBuilder builder_(_fbb);
    builder_.add_value_easing(value_easing);
    builder_.add_time_easing(time_easing);
    builder_.add_start(start);
    builder_.add_time(time);
    builder_.add_hold(hold);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<VectorFrame> CreateVectorFrameDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb, float time = 0.0f, const LottieFlatBuffer::Point *start = nullptr,
    bool hold = false, const std::vector<LottieFlatBuffer::Point> *time_easing = nullptr,
    const std::vector<LottieFlatBuffer::Point> *value_easing = nullptr)
{
    auto time_easing__ = time_easing ? _fbb.CreateVectorOfStructs<LottieFlatBuffer::Point>(*time_easing) : 0;
    auto value_easing__ = value_easing ? _fbb.CreateVectorOfStructs<LottieFlatBuffer::Point>(*value_easing) : 0;
    return LottieFlatBuffer::CreateVectorFrame(_fbb, time, start, hold, time_easing__, value_easing__);
}

struct VectorFrameArray FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef VectorFrameArrayBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE { VT_FRAMES = 4 };
    const ::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::VectorFrame>> *frames() const
    {
        return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::VectorFrame>> *>(
            VT_FRAMES);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyOffsetRequired(verifier, VT_FRAMES) &&
               verifier.VerifyVector(frames()) && verifier.VerifyVectorOfTables(frames()) && verifier.EndTable();
    }
};

struct VectorFrameArrayBuilder {
    typedef VectorFrameArray Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_frames(
        ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::VectorFrame>>> frames)
    {
        fbb_.AddOffset(VectorFrameArray::VT_FRAMES, frames);
    }
    explicit VectorFrameArrayBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<VectorFrameArray> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<VectorFrameArray>(end);
        fbb_.Required(o, VectorFrameArray::VT_FRAMES);
        return o;
    }
};

inline ::flatbuffers::Offset<VectorFrameArray> CreateVectorFrameArray(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::VectorFrame>>> frames = 0)
{
    VectorFrameArrayBuilder builder_(_fbb);
    builder_.add_frames(frames);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<VectorFrameArray> CreateVectorFrameArrayDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<::flatbuffers::Offset<LottieFlatBuffer::VectorFrame>> *frames = nullptr)
{
    auto frames__ = frames ? _fbb.CreateVector<::flatbuffers::Offset<LottieFlatBuffer::VectorFrame>>(*frames) : 0;
    return LottieFlatBuffer::CreateVectorFrameArray(_fbb, frames__);
}

struct ColorFrame FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef ColorFrameBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_TIME = 4,
        VT_START = 6,
        VT_HOLD = 8,
        VT_TIME_EASING = 10,
        VT_VALUE_EASING = 12
    };
    float time() const
    {
        return GetField<float>(VT_TIME, 0.0f);
    }
    const LottieFlatBuffer::Color24 *start() const
    {
        return GetStruct<const LottieFlatBuffer::Color24 *>(VT_START);
    }
    bool hold() const
    {
        return GetField<uint8_t>(VT_HOLD, 0) != 0;
    }
    const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *time_easing() const
    {
        return GetPointer<const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *>(VT_TIME_EASING);
    }
    const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *value_easing() const
    {
        return GetPointer<const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *>(VT_VALUE_EASING);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyField<float>(verifier, VT_TIME, 4) &&
               VerifyField<LottieFlatBuffer::Color24>(verifier, VT_START, 1) &&
               VerifyField<uint8_t>(verifier, VT_HOLD, 1) && VerifyOffset(verifier, VT_TIME_EASING) &&
               verifier.VerifyVector(time_easing()) && VerifyOffset(verifier, VT_VALUE_EASING) &&
               verifier.VerifyVector(value_easing()) && verifier.EndTable();
    }
};

struct ColorFrameBuilder {
    typedef ColorFrame Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_time(float time)
    {
        fbb_.AddElement<float>(ColorFrame::VT_TIME, time, 0.0f);
    }
    void add_start(const LottieFlatBuffer::Color24 *start)
    {
        fbb_.AddStruct(ColorFrame::VT_START, start);
    }
    void add_hold(bool hold)
    {
        fbb_.AddElement<uint8_t>(ColorFrame::VT_HOLD, static_cast<uint8_t>(hold), 0);
    }
    void add_time_easing(::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> time_easing)
    {
        fbb_.AddOffset(ColorFrame::VT_TIME_EASING, time_easing);
    }
    void add_value_easing(::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> value_easing)
    {
        fbb_.AddOffset(ColorFrame::VT_VALUE_EASING, value_easing);
    }
    explicit ColorFrameBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<ColorFrame> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<ColorFrame>(end);
        return o;
    }
};

inline ::flatbuffers::Offset<ColorFrame> CreateColorFrame(
    ::flatbuffers::FlatBufferBuilder &_fbb, float time = 0.0f, const LottieFlatBuffer::Color24 *start = nullptr,
    bool hold = false, ::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> time_easing = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> value_easing = 0)
{
    ColorFrameBuilder builder_(_fbb);
    builder_.add_value_easing(value_easing);
    builder_.add_time_easing(time_easing);
    builder_.add_start(start);
    builder_.add_time(time);
    builder_.add_hold(hold);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<ColorFrame> CreateColorFrameDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb, float time = 0.0f, const LottieFlatBuffer::Color24 *start = nullptr,
    bool hold = false, const std::vector<LottieFlatBuffer::Point> *time_easing = nullptr,
    const std::vector<LottieFlatBuffer::Point> *value_easing = nullptr)
{
    auto time_easing__ = time_easing ? _fbb.CreateVectorOfStructs<LottieFlatBuffer::Point>(*time_easing) : 0;
    auto value_easing__ = value_easing ? _fbb.CreateVectorOfStructs<LottieFlatBuffer::Point>(*value_easing) : 0;
    return LottieFlatBuffer::CreateColorFrame(_fbb, time, start, hold, time_easing__, value_easing__);
}

struct ColorFrameArray FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef ColorFrameArrayBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE { VT_FRAMES = 4 };
    const ::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::ColorFrame>> *frames() const
    {
        return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::ColorFrame>> *>(
            VT_FRAMES);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyOffsetRequired(verifier, VT_FRAMES) &&
               verifier.VerifyVector(frames()) && verifier.VerifyVectorOfTables(frames()) && verifier.EndTable();
    }
};

struct ColorFrameArrayBuilder {
    typedef ColorFrameArray Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_frames(
        ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::ColorFrame>>> frames)
    {
        fbb_.AddOffset(ColorFrameArray::VT_FRAMES, frames);
    }
    explicit ColorFrameArrayBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<ColorFrameArray> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<ColorFrameArray>(end);
        fbb_.Required(o, ColorFrameArray::VT_FRAMES);
        return o;
    }
};

inline ::flatbuffers::Offset<ColorFrameArray> CreateColorFrameArray(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::ColorFrame>>> frames = 0)
{
    ColorFrameArrayBuilder builder_(_fbb);
    builder_.add_frames(frames);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<ColorFrameArray> CreateColorFrameArrayDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<::flatbuffers::Offset<LottieFlatBuffer::ColorFrame>> *frames = nullptr)
{
    auto frames__ = frames ? _fbb.CreateVector<::flatbuffers::Offset<LottieFlatBuffer::ColorFrame>>(*frames) : 0;
    return LottieFlatBuffer::CreateColorFrameArray(_fbb, frames__);
}

struct Color24Scalar FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef Color24ScalarBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE { VT_VALUE = 4 };
    const LottieFlatBuffer::Color24 *value() const
    {
        return GetStruct<const LottieFlatBuffer::Color24 *>(VT_VALUE);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyField<LottieFlatBuffer::Color24>(verifier, VT_VALUE, 1) &&
               verifier.EndTable();
    }
};

struct Color24ScalarBuilder {
    typedef Color24Scalar Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_value(const LottieFlatBuffer::Color24 *value)
    {
        fbb_.AddStruct(Color24Scalar::VT_VALUE, value);
    }
    explicit Color24ScalarBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<Color24Scalar> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<Color24Scalar>(end);
        return o;
    }
};

inline ::flatbuffers::Offset<Color24Scalar> CreateColor24Scalar(::flatbuffers::FlatBufferBuilder &_fbb,
                                                                const LottieFlatBuffer::Color24 *value = nullptr)
{
    Color24ScalarBuilder builder_(_fbb);
    builder_.add_value(value);
    return builder_.Finish();
}

struct GradientFrame FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef GradientFrameBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_TIME = 4,
        VT_START = 6,
        VT_HOLD = 8,
        VT_TIME_EASING = 10,
        VT_VALUE_EASING = 12
    };
    float time() const
    {
        return GetField<float>(VT_TIME, 0.0f);
    }
    const LottieFlatBuffer::Gradient *start() const
    {
        return GetPointer<const LottieFlatBuffer::Gradient *>(VT_START);
    }
    bool hold() const
    {
        return GetField<uint8_t>(VT_HOLD, 0) != 0;
    }
    const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *time_easing() const
    {
        return GetPointer<const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *>(VT_TIME_EASING);
    }
    const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *value_easing() const
    {
        return GetPointer<const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *>(VT_VALUE_EASING);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyField<float>(verifier, VT_TIME, 4) &&
               VerifyOffsetRequired(verifier, VT_START) && verifier.VerifyTable(start()) &&
               VerifyField<uint8_t>(verifier, VT_HOLD, 1) && VerifyOffset(verifier, VT_TIME_EASING) &&
               verifier.VerifyVector(time_easing()) && VerifyOffset(verifier, VT_VALUE_EASING) &&
               verifier.VerifyVector(value_easing()) && verifier.EndTable();
    }
};

struct GradientFrameBuilder {
    typedef GradientFrame Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_time(float time)
    {
        fbb_.AddElement<float>(GradientFrame::VT_TIME, time, 0.0f);
    }
    void add_start(::flatbuffers::Offset<LottieFlatBuffer::Gradient> start)
    {
        fbb_.AddOffset(GradientFrame::VT_START, start);
    }
    void add_hold(bool hold)
    {
        fbb_.AddElement<uint8_t>(GradientFrame::VT_HOLD, static_cast<uint8_t>(hold), 0);
    }
    void add_time_easing(::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> time_easing)
    {
        fbb_.AddOffset(GradientFrame::VT_TIME_EASING, time_easing);
    }
    void add_value_easing(::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> value_easing)
    {
        fbb_.AddOffset(GradientFrame::VT_VALUE_EASING, value_easing);
    }
    explicit GradientFrameBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<GradientFrame> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<GradientFrame>(end);
        fbb_.Required(o, GradientFrame::VT_START);
        return o;
    }
};

inline ::flatbuffers::Offset<GradientFrame> CreateGradientFrame(
    ::flatbuffers::FlatBufferBuilder &_fbb, float time = 0.0f,
    ::flatbuffers::Offset<LottieFlatBuffer::Gradient> start = 0, bool hold = false,
    ::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> time_easing = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> value_easing = 0)
{
    GradientFrameBuilder builder_(_fbb);
    builder_.add_value_easing(value_easing);
    builder_.add_time_easing(time_easing);
    builder_.add_start(start);
    builder_.add_time(time);
    builder_.add_hold(hold);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<GradientFrame> CreateGradientFrameDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb, float time = 0.0f,
    ::flatbuffers::Offset<LottieFlatBuffer::Gradient> start = 0, bool hold = false,
    const std::vector<LottieFlatBuffer::Point> *time_easing = nullptr,
    const std::vector<LottieFlatBuffer::Point> *value_easing = nullptr)
{
    auto time_easing__ = time_easing ? _fbb.CreateVectorOfStructs<LottieFlatBuffer::Point>(*time_easing) : 0;
    auto value_easing__ = value_easing ? _fbb.CreateVectorOfStructs<LottieFlatBuffer::Point>(*value_easing) : 0;
    return LottieFlatBuffer::CreateGradientFrame(_fbb, time, start, hold, time_easing__, value_easing__);
}

struct GradientFrameArray FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef GradientFrameArrayBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE { VT_FRAMES = 4 };
    const ::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::GradientFrame>> *frames() const
    {
        return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::GradientFrame>> *>(
            VT_FRAMES);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyOffsetRequired(verifier, VT_FRAMES) &&
               verifier.VerifyVector(frames()) && verifier.VerifyVectorOfTables(frames()) && verifier.EndTable();
    }
};

struct GradientFrameArrayBuilder {
    typedef GradientFrameArray Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_frames(
        ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::GradientFrame>>> frames)
    {
        fbb_.AddOffset(GradientFrameArray::VT_FRAMES, frames);
    }
    explicit GradientFrameArrayBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<GradientFrameArray> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<GradientFrameArray>(end);
        fbb_.Required(o, GradientFrameArray::VT_FRAMES);
        return o;
    }
};

inline ::flatbuffers::Offset<GradientFrameArray> CreateGradientFrameArray(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::GradientFrame>>> frames = 0)
{
    GradientFrameArrayBuilder builder_(_fbb);
    builder_.add_frames(frames);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<GradientFrameArray> CreateGradientFrameArrayDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<::flatbuffers::Offset<LottieFlatBuffer::GradientFrame>> *frames = nullptr)
{
    auto frames__ = frames ? _fbb.CreateVector<::flatbuffers::Offset<LottieFlatBuffer::GradientFrame>>(*frames) : 0;
    return LottieFlatBuffer::CreateGradientFrameArray(_fbb, frames__);
}

struct BezierDataFrame FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef BezierDataFrameBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_TIME = 4,
        VT_START = 6,
        VT_HOLD = 8,
        VT_TIME_EASING = 10,
        VT_VALUE_EASING = 12
    };
    float time() const
    {
        return GetField<float>(VT_TIME, 0.0f);
    }
    const LottieFlatBuffer::BezierData *start() const
    {
        return GetPointer<const LottieFlatBuffer::BezierData *>(VT_START);
    }
    bool hold() const
    {
        return GetField<uint8_t>(VT_HOLD, 0) != 0;
    }
    const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *time_easing() const
    {
        return GetPointer<const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *>(VT_TIME_EASING);
    }
    const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *value_easing() const
    {
        return GetPointer<const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *>(VT_VALUE_EASING);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyField<float>(verifier, VT_TIME, 4) &&
               VerifyOffsetRequired(verifier, VT_START) && verifier.VerifyTable(start()) &&
               VerifyField<uint8_t>(verifier, VT_HOLD, 1) && VerifyOffset(verifier, VT_TIME_EASING) &&
               verifier.VerifyVector(time_easing()) && VerifyOffset(verifier, VT_VALUE_EASING) &&
               verifier.VerifyVector(value_easing()) && verifier.EndTable();
    }
};

struct BezierDataFrameBuilder {
    typedef BezierDataFrame Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_time(float time)
    {
        fbb_.AddElement<float>(BezierDataFrame::VT_TIME, time, 0.0f);
    }
    void add_start(::flatbuffers::Offset<LottieFlatBuffer::BezierData> start)
    {
        fbb_.AddOffset(BezierDataFrame::VT_START, start);
    }
    void add_hold(bool hold)
    {
        fbb_.AddElement<uint8_t>(BezierDataFrame::VT_HOLD, static_cast<uint8_t>(hold), 0);
    }
    void add_time_easing(::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> time_easing)
    {
        fbb_.AddOffset(BezierDataFrame::VT_TIME_EASING, time_easing);
    }
    void add_value_easing(::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> value_easing)
    {
        fbb_.AddOffset(BezierDataFrame::VT_VALUE_EASING, value_easing);
    }
    explicit BezierDataFrameBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<BezierDataFrame> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<BezierDataFrame>(end);
        fbb_.Required(o, BezierDataFrame::VT_START);
        return o;
    }
};

inline ::flatbuffers::Offset<BezierDataFrame> CreateBezierDataFrame(
    ::flatbuffers::FlatBufferBuilder &_fbb, float time = 0.0f,
    ::flatbuffers::Offset<LottieFlatBuffer::BezierData> start = 0, bool hold = false,
    ::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> time_easing = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> value_easing = 0)
{
    BezierDataFrameBuilder builder_(_fbb);
    builder_.add_value_easing(value_easing);
    builder_.add_time_easing(time_easing);
    builder_.add_start(start);
    builder_.add_time(time);
    builder_.add_hold(hold);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<BezierDataFrame> CreateBezierDataFrameDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb, float time = 0.0f,
    ::flatbuffers::Offset<LottieFlatBuffer::BezierData> start = 0, bool hold = false,
    const std::vector<LottieFlatBuffer::Point> *time_easing = nullptr,
    const std::vector<LottieFlatBuffer::Point> *value_easing = nullptr)
{
    auto time_easing__ = time_easing ? _fbb.CreateVectorOfStructs<LottieFlatBuffer::Point>(*time_easing) : 0;
    auto value_easing__ = value_easing ? _fbb.CreateVectorOfStructs<LottieFlatBuffer::Point>(*value_easing) : 0;
    return LottieFlatBuffer::CreateBezierDataFrame(_fbb, time, start, hold, time_easing__, value_easing__);
}

struct BezierDataFrameArray FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef BezierDataFrameArrayBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE { VT_FRAMES = 4 };
    const ::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::BezierDataFrame>> *frames() const
    {
        return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::BezierDataFrame>> *>(
            VT_FRAMES);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyOffsetRequired(verifier, VT_FRAMES) &&
               verifier.VerifyVector(frames()) && verifier.VerifyVectorOfTables(frames()) && verifier.EndTable();
    }
};

struct BezierDataFrameArrayBuilder {
    typedef BezierDataFrameArray Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_frames(
        ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::BezierDataFrame>>> frames)
    {
        fbb_.AddOffset(BezierDataFrameArray::VT_FRAMES, frames);
    }
    explicit BezierDataFrameArrayBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<BezierDataFrameArray> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<BezierDataFrameArray>(end);
        fbb_.Required(o, BezierDataFrameArray::VT_FRAMES);
        return o;
    }
};

inline ::flatbuffers::Offset<BezierDataFrameArray> CreateBezierDataFrameArray(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::BezierDataFrame>>> frames = 0)
{
    BezierDataFrameArrayBuilder builder_(_fbb);
    builder_.add_frames(frames);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<BezierDataFrameArray> CreateBezierDataFrameArrayDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<::flatbuffers::Offset<LottieFlatBuffer::BezierDataFrame>> *frames = nullptr)
{
    auto frames__ = frames ? _fbb.CreateVector<::flatbuffers::Offset<LottieFlatBuffer::BezierDataFrame>>(*frames) : 0;
    return LottieFlatBuffer::CreateBezierDataFrameArray(_fbb, frames__);
}

struct PositionFrame FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef PositionFrameBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_TIME = 4,
        VT_START = 6,
        VT_HOLD = 8,
        VT_TIME_EASING = 10,
        VT_VALUE_EASING = 12,
        VT_IN_TAN = 14,
        VT_OUT_TAN = 16
    };
    uint16_t time() const
    {
        return GetField<uint16_t>(VT_TIME, 0);
    }
    const LottieFlatBuffer::Point *start() const
    {
        return GetStruct<const LottieFlatBuffer::Point *>(VT_START);
    }
    bool hold() const
    {
        return GetField<uint8_t>(VT_HOLD, 0) != 0;
    }
    const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *time_easing() const
    {
        return GetPointer<const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *>(VT_TIME_EASING);
    }
    const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *value_easing() const
    {
        return GetPointer<const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *>(VT_VALUE_EASING);
    }
    const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *in_tan() const
    {
        return GetPointer<const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *>(VT_IN_TAN);
    }
    const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *out_tan() const
    {
        return GetPointer<const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *>(VT_OUT_TAN);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyField<uint16_t>(verifier, VT_TIME, 2) &&
               VerifyField<LottieFlatBuffer::Point>(verifier, VT_START, 4) &&
               VerifyField<uint8_t>(verifier, VT_HOLD, 1) && VerifyOffset(verifier, VT_TIME_EASING) &&
               verifier.VerifyVector(time_easing()) && VerifyOffset(verifier, VT_VALUE_EASING) &&
               verifier.VerifyVector(value_easing()) && VerifyOffset(verifier, VT_IN_TAN) &&
               verifier.VerifyVector(in_tan()) && VerifyOffset(verifier, VT_OUT_TAN) &&
               verifier.VerifyVector(out_tan()) && verifier.EndTable();
    }
};

struct PositionFrameBuilder {
    typedef PositionFrame Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_time(uint16_t time)
    {
        fbb_.AddElement<uint16_t>(PositionFrame::VT_TIME, time, 0);
    }
    void add_start(const LottieFlatBuffer::Point *start)
    {
        fbb_.AddStruct(PositionFrame::VT_START, start);
    }
    void add_hold(bool hold)
    {
        fbb_.AddElement<uint8_t>(PositionFrame::VT_HOLD, static_cast<uint8_t>(hold), 0);
    }
    void add_time_easing(::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> time_easing)
    {
        fbb_.AddOffset(PositionFrame::VT_TIME_EASING, time_easing);
    }
    void add_value_easing(::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> value_easing)
    {
        fbb_.AddOffset(PositionFrame::VT_VALUE_EASING, value_easing);
    }
    void add_in_tan(::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> in_tan)
    {
        fbb_.AddOffset(PositionFrame::VT_IN_TAN, in_tan);
    }
    void add_out_tan(::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> out_tan)
    {
        fbb_.AddOffset(PositionFrame::VT_OUT_TAN, out_tan);
    }
    explicit PositionFrameBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<PositionFrame> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<PositionFrame>(end);
        return o;
    }
};

inline ::flatbuffers::Offset<PositionFrame> CreatePositionFrame(
    ::flatbuffers::FlatBufferBuilder &_fbb, uint16_t time = 0, const LottieFlatBuffer::Point *start = nullptr,
    bool hold = false, ::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> time_easing = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> value_easing = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> in_tan = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> out_tan = 0)
{
    PositionFrameBuilder builder_(_fbb);
    builder_.add_out_tan(out_tan);
    builder_.add_in_tan(in_tan);
    builder_.add_value_easing(value_easing);
    builder_.add_time_easing(time_easing);
    builder_.add_start(start);
    builder_.add_time(time);
    builder_.add_hold(hold);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<PositionFrame> CreatePositionFrameDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb, uint16_t time = 0, const LottieFlatBuffer::Point *start = nullptr,
    bool hold = false, const std::vector<LottieFlatBuffer::Point> *time_easing = nullptr,
    const std::vector<LottieFlatBuffer::Point> *value_easing = nullptr,
    const std::vector<LottieFlatBuffer::Point> *in_tan = nullptr,
    const std::vector<LottieFlatBuffer::Point> *out_tan = nullptr)
{
    auto time_easing__ = time_easing ? _fbb.CreateVectorOfStructs<LottieFlatBuffer::Point>(*time_easing) : 0;
    auto value_easing__ = value_easing ? _fbb.CreateVectorOfStructs<LottieFlatBuffer::Point>(*value_easing) : 0;
    auto in_tan__ = in_tan ? _fbb.CreateVectorOfStructs<LottieFlatBuffer::Point>(*in_tan) : 0;
    auto out_tan__ = out_tan ? _fbb.CreateVectorOfStructs<LottieFlatBuffer::Point>(*out_tan) : 0;
    return LottieFlatBuffer::CreatePositionFrame(_fbb, time, start, hold, time_easing__, value_easing__, in_tan__,
                                                 out_tan__);
}

struct PositionFrameArray FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef PositionFrameArrayBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE { VT_FRAMES = 4 };
    const ::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::PositionFrame>> *frames() const
    {
        return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::PositionFrame>> *>(
            VT_FRAMES);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyOffsetRequired(verifier, VT_FRAMES) &&
               verifier.VerifyVector(frames()) && verifier.VerifyVectorOfTables(frames()) && verifier.EndTable();
    }
};

struct PositionFrameArrayBuilder {
    typedef PositionFrameArray Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_frames(
        ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::PositionFrame>>> frames)
    {
        fbb_.AddOffset(PositionFrameArray::VT_FRAMES, frames);
    }
    explicit PositionFrameArrayBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<PositionFrameArray> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<PositionFrameArray>(end);
        fbb_.Required(o, PositionFrameArray::VT_FRAMES);
        return o;
    }
};

inline ::flatbuffers::Offset<PositionFrameArray> CreatePositionFrameArray(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::PositionFrame>>> frames = 0)
{
    PositionFrameArrayBuilder builder_(_fbb);
    builder_.add_frames(frames);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<PositionFrameArray> CreatePositionFrameArrayDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<::flatbuffers::Offset<LottieFlatBuffer::PositionFrame>> *frames = nullptr)
{
    auto frames__ = frames ? _fbb.CreateVector<::flatbuffers::Offset<LottieFlatBuffer::PositionFrame>>(*frames) : 0;
    return LottieFlatBuffer::CreatePositionFrameArray(_fbb, frames__);
}

struct SplitProp FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef SplitPropBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_X_TYPE = 4,
        VT_X = 6,
        VT_Y_TYPE = 8,
        VT_Y = 10
    };
    LottieFlatBuffer::FloatProp x_type() const
    {
        return static_cast<LottieFlatBuffer::FloatProp>(GetField<uint8_t>(VT_X_TYPE, 0));
    }
    const void *x() const
    {
        return GetPointer<const void *>(VT_X);
    }
    template <typename T>
    const T *x_as() const;
    const LottieFlatBuffer::FloatFrameArray *x_as_frames() const
    {
        return x_type() == LottieFlatBuffer::FloatProp_frames ?
                   static_cast<const LottieFlatBuffer::FloatFrameArray *>(x()) :
                   nullptr;
    }
    const LottieFlatBuffer::FloatScalar *x_as_value() const
    {
        return x_type() == LottieFlatBuffer::FloatProp_value ? static_cast<const LottieFlatBuffer::FloatScalar *>(x()) :
                                                               nullptr;
    }
    LottieFlatBuffer::FloatProp y_type() const
    {
        return static_cast<LottieFlatBuffer::FloatProp>(GetField<uint8_t>(VT_Y_TYPE, 0));
    }
    const void *y() const
    {
        return GetPointer<const void *>(VT_Y);
    }
    template <typename T>
    const T *y_as() const;
    const LottieFlatBuffer::FloatFrameArray *y_as_frames() const
    {
        return y_type() == LottieFlatBuffer::FloatProp_frames ?
                   static_cast<const LottieFlatBuffer::FloatFrameArray *>(y()) :
                   nullptr;
    }
    const LottieFlatBuffer::FloatScalar *y_as_value() const
    {
        return y_type() == LottieFlatBuffer::FloatProp_value ? static_cast<const LottieFlatBuffer::FloatScalar *>(y()) :
                                                               nullptr;
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyField<uint8_t>(verifier, VT_X_TYPE, 1) &&
               VerifyOffsetRequired(verifier, VT_X) && VerifyFloatProp(verifier, x(), x_type()) &&
               VerifyField<uint8_t>(verifier, VT_Y_TYPE, 1) && VerifyOffsetRequired(verifier, VT_Y) &&
               VerifyFloatProp(verifier, y(), y_type()) && verifier.EndTable();
    }
};

template <>
inline const LottieFlatBuffer::FloatFrameArray *SplitProp::x_as<LottieFlatBuffer::FloatFrameArray>() const
{
    return x_as_frames();
}

template <>
inline const LottieFlatBuffer::FloatScalar *SplitProp::x_as<LottieFlatBuffer::FloatScalar>() const
{
    return x_as_value();
}

template <>
inline const LottieFlatBuffer::FloatFrameArray *SplitProp::y_as<LottieFlatBuffer::FloatFrameArray>() const
{
    return y_as_frames();
}

template <>
inline const LottieFlatBuffer::FloatScalar *SplitProp::y_as<LottieFlatBuffer::FloatScalar>() const
{
    return y_as_value();
}

struct SplitPropBuilder {
    typedef SplitProp Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_x_type(LottieFlatBuffer::FloatProp x_type)
    {
        fbb_.AddElement<uint8_t>(SplitProp::VT_X_TYPE, static_cast<uint8_t>(x_type), 0);
    }
    void add_x(::flatbuffers::Offset<void> x)
    {
        fbb_.AddOffset(SplitProp::VT_X, x);
    }
    void add_y_type(LottieFlatBuffer::FloatProp y_type)
    {
        fbb_.AddElement<uint8_t>(SplitProp::VT_Y_TYPE, static_cast<uint8_t>(y_type), 0);
    }
    void add_y(::flatbuffers::Offset<void> y)
    {
        fbb_.AddOffset(SplitProp::VT_Y, y);
    }
    explicit SplitPropBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<SplitProp> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<SplitProp>(end);
        fbb_.Required(o, SplitProp::VT_X);
        fbb_.Required(o, SplitProp::VT_Y);
        return o;
    }
};

inline ::flatbuffers::Offset<SplitProp> CreateSplitProp(
    ::flatbuffers::FlatBufferBuilder &_fbb, LottieFlatBuffer::FloatProp x_type = LottieFlatBuffer::FloatProp_NONE,
    ::flatbuffers::Offset<void> x = 0, LottieFlatBuffer::FloatProp y_type = LottieFlatBuffer::FloatProp_NONE,
    ::flatbuffers::Offset<void> y = 0)
{
    SplitPropBuilder builder_(_fbb);
    builder_.add_y(y);
    builder_.add_x(x);
    builder_.add_y_type(y_type);
    builder_.add_x_type(x_type);
    return builder_.Finish();
}

struct Transform FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef TransformBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_ANCHOR_TYPE = 4,
        VT_ANCHOR = 6,
        VT_POSITION_TYPE = 8,
        VT_POSITION = 10,
        VT_SPLIT_POSITION = 12,
        VT_SCALE_TYPE = 14,
        VT_SCALE = 16,
        VT_ROTATION_TYPE = 18,
        VT_ROTATION = 20,
        VT_OPACITY_TYPE = 22,
        VT_OPACITY = 24,
        VT_SKEW_ANGLE_TYPE = 26,
        VT_SKEW_ANGLE = 28,
        VT_SKEW_AXIS_TYPE = 30,
        VT_SKEW_AXIS = 32
    };
    LottieFlatBuffer::VectorProp anchor_type() const
    {
        return static_cast<LottieFlatBuffer::VectorProp>(GetField<uint8_t>(VT_ANCHOR_TYPE, 0));
    }
    const void *anchor() const
    {
        return GetPointer<const void *>(VT_ANCHOR);
    }
    template <typename T>
    const T *anchor_as() const;
    const LottieFlatBuffer::VectorFrameArray *anchor_as_frames() const
    {
        return anchor_type() == LottieFlatBuffer::VectorProp_frames ?
                   static_cast<const LottieFlatBuffer::VectorFrameArray *>(anchor()) :
                   nullptr;
    }
    const LottieFlatBuffer::Point *anchor_as_value() const
    {
        return anchor_type() == LottieFlatBuffer::VectorProp_value ?
                   static_cast<const LottieFlatBuffer::Point *>(anchor()) :
                   nullptr;
    }
    LottieFlatBuffer::PositionProp position_type() const
    {
        return static_cast<LottieFlatBuffer::PositionProp>(GetField<uint8_t>(VT_POSITION_TYPE, 0));
    }
    const void *position() const
    {
        return GetPointer<const void *>(VT_POSITION);
    }
    template <typename T>
    const T *position_as() const;
    const LottieFlatBuffer::PositionFrameArray *position_as_frames() const
    {
        return position_type() == LottieFlatBuffer::PositionProp_frames ?
                   static_cast<const LottieFlatBuffer::PositionFrameArray *>(position()) :
                   nullptr;
    }
    const LottieFlatBuffer::Point *position_as_value() const
    {
        return position_type() == LottieFlatBuffer::PositionProp_value ?
                   static_cast<const LottieFlatBuffer::Point *>(position()) :
                   nullptr;
    }
    const LottieFlatBuffer::SplitProp *split_position() const
    {
        return GetPointer<const LottieFlatBuffer::SplitProp *>(VT_SPLIT_POSITION);
    }
    LottieFlatBuffer::VectorProp scale_type() const
    {
        return static_cast<LottieFlatBuffer::VectorProp>(GetField<uint8_t>(VT_SCALE_TYPE, 0));
    }
    const void *scale() const
    {
        return GetPointer<const void *>(VT_SCALE);
    }
    template <typename T>
    const T *scale_as() const;
    const LottieFlatBuffer::VectorFrameArray *scale_as_frames() const
    {
        return scale_type() == LottieFlatBuffer::VectorProp_frames ?
                   static_cast<const LottieFlatBuffer::VectorFrameArray *>(scale()) :
                   nullptr;
    }
    const LottieFlatBuffer::Point *scale_as_value() const
    {
        return scale_type() == LottieFlatBuffer::VectorProp_value ?
                   static_cast<const LottieFlatBuffer::Point *>(scale()) :
                   nullptr;
    }
    LottieFlatBuffer::FloatProp rotation_type() const
    {
        return static_cast<LottieFlatBuffer::FloatProp>(GetField<uint8_t>(VT_ROTATION_TYPE, 0));
    }
    const void *rotation() const
    {
        return GetPointer<const void *>(VT_ROTATION);
    }
    template <typename T>
    const T *rotation_as() const;
    const LottieFlatBuffer::FloatFrameArray *rotation_as_frames() const
    {
        return rotation_type() == LottieFlatBuffer::FloatProp_frames ?
                   static_cast<const LottieFlatBuffer::FloatFrameArray *>(rotation()) :
                   nullptr;
    }
    const LottieFlatBuffer::FloatScalar *rotation_as_value() const
    {
        return rotation_type() == LottieFlatBuffer::FloatProp_value ?
                   static_cast<const LottieFlatBuffer::FloatScalar *>(rotation()) :
                   nullptr;
    }
    LottieFlatBuffer::IntegerProp opacity_type() const
    {
        return static_cast<LottieFlatBuffer::IntegerProp>(GetField<uint8_t>(VT_OPACITY_TYPE, 0));
    }
    const void *opacity() const
    {
        return GetPointer<const void *>(VT_OPACITY);
    }
    template <typename T>
    const T *opacity_as() const;
    const LottieFlatBuffer::IntegerFrameArray *opacity_as_frames() const
    {
        return opacity_type() == LottieFlatBuffer::IntegerProp_frames ?
                   static_cast<const LottieFlatBuffer::IntegerFrameArray *>(opacity()) :
                   nullptr;
    }
    const LottieFlatBuffer::IntegerScalar *opacity_as_value() const
    {
        return opacity_type() == LottieFlatBuffer::IntegerProp_value ?
                   static_cast<const LottieFlatBuffer::IntegerScalar *>(opacity()) :
                   nullptr;
    }
    LottieFlatBuffer::FloatProp skew_angle_type() const
    {
        return static_cast<LottieFlatBuffer::FloatProp>(GetField<uint8_t>(VT_SKEW_ANGLE_TYPE, 0));
    }
    const void *skew_angle() const
    {
        return GetPointer<const void *>(VT_SKEW_ANGLE);
    }
    template <typename T>
    const T *skew_angle_as() const;
    const LottieFlatBuffer::FloatFrameArray *skew_angle_as_frames() const
    {
        return skew_angle_type() == LottieFlatBuffer::FloatProp_frames ?
                   static_cast<const LottieFlatBuffer::FloatFrameArray *>(skew_angle()) :
                   nullptr;
    }
    const LottieFlatBuffer::FloatScalar *skew_angle_as_value() const
    {
        return skew_angle_type() == LottieFlatBuffer::FloatProp_value ?
                   static_cast<const LottieFlatBuffer::FloatScalar *>(skew_angle()) :
                   nullptr;
    }
    LottieFlatBuffer::FloatProp skew_axis_type() const
    {
        return static_cast<LottieFlatBuffer::FloatProp>(GetField<uint8_t>(VT_SKEW_AXIS_TYPE, 0));
    }
    const void *skew_axis() const
    {
        return GetPointer<const void *>(VT_SKEW_AXIS);
    }
    template <typename T>
    const T *skew_axis_as() const;
    const LottieFlatBuffer::FloatFrameArray *skew_axis_as_frames() const
    {
        return skew_axis_type() == LottieFlatBuffer::FloatProp_frames ?
                   static_cast<const LottieFlatBuffer::FloatFrameArray *>(skew_axis()) :
                   nullptr;
    }
    const LottieFlatBuffer::FloatScalar *skew_axis_as_value() const
    {
        return skew_axis_type() == LottieFlatBuffer::FloatProp_value ?
                   static_cast<const LottieFlatBuffer::FloatScalar *>(skew_axis()) :
                   nullptr;
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyField<uint8_t>(verifier, VT_ANCHOR_TYPE, 1) &&
               VerifyOffset(verifier, VT_ANCHOR) && VerifyVectorProp(verifier, anchor(), anchor_type()) &&
               VerifyField<uint8_t>(verifier, VT_POSITION_TYPE, 1) && VerifyOffset(verifier, VT_POSITION) &&
               VerifyPositionProp(verifier, position(), position_type()) && VerifyOffset(verifier, VT_SPLIT_POSITION) &&
               verifier.VerifyTable(split_position()) && VerifyField<uint8_t>(verifier, VT_SCALE_TYPE, 1) &&
               VerifyOffset(verifier, VT_SCALE) && VerifyVectorProp(verifier, scale(), scale_type()) &&
               VerifyField<uint8_t>(verifier, VT_ROTATION_TYPE, 1) && VerifyOffset(verifier, VT_ROTATION) &&
               VerifyFloatProp(verifier, rotation(), rotation_type()) &&
               VerifyField<uint8_t>(verifier, VT_OPACITY_TYPE, 1) && VerifyOffset(verifier, VT_OPACITY) &&
               VerifyIntegerProp(verifier, opacity(), opacity_type()) &&
               VerifyField<uint8_t>(verifier, VT_SKEW_ANGLE_TYPE, 1) && VerifyOffset(verifier, VT_SKEW_ANGLE) &&
               VerifyFloatProp(verifier, skew_angle(), skew_angle_type()) &&
               VerifyField<uint8_t>(verifier, VT_SKEW_AXIS_TYPE, 1) && VerifyOffset(verifier, VT_SKEW_AXIS) &&
               VerifyFloatProp(verifier, skew_axis(), skew_axis_type()) && verifier.EndTable();
    }
};

template <>
inline const LottieFlatBuffer::VectorFrameArray *Transform::anchor_as<LottieFlatBuffer::VectorFrameArray>() const
{
    return anchor_as_frames();
}

template <>
inline const LottieFlatBuffer::Point *Transform::anchor_as<LottieFlatBuffer::Point>() const
{
    return anchor_as_value();
}

template <>
inline const LottieFlatBuffer::PositionFrameArray *Transform::position_as<LottieFlatBuffer::PositionFrameArray>() const
{
    return position_as_frames();
}

template <>
inline const LottieFlatBuffer::Point *Transform::position_as<LottieFlatBuffer::Point>() const
{
    return position_as_value();
}

template <>
inline const LottieFlatBuffer::VectorFrameArray *Transform::scale_as<LottieFlatBuffer::VectorFrameArray>() const
{
    return scale_as_frames();
}

template <>
inline const LottieFlatBuffer::Point *Transform::scale_as<LottieFlatBuffer::Point>() const
{
    return scale_as_value();
}

template <>
inline const LottieFlatBuffer::FloatFrameArray *Transform::rotation_as<LottieFlatBuffer::FloatFrameArray>() const
{
    return rotation_as_frames();
}

template <>
inline const LottieFlatBuffer::FloatScalar *Transform::rotation_as<LottieFlatBuffer::FloatScalar>() const
{
    return rotation_as_value();
}

template <>
inline const LottieFlatBuffer::IntegerFrameArray *Transform::opacity_as<LottieFlatBuffer::IntegerFrameArray>() const
{
    return opacity_as_frames();
}

template <>
inline const LottieFlatBuffer::IntegerScalar *Transform::opacity_as<LottieFlatBuffer::IntegerScalar>() const
{
    return opacity_as_value();
}

template <>
inline const LottieFlatBuffer::FloatFrameArray *Transform::skew_angle_as<LottieFlatBuffer::FloatFrameArray>() const
{
    return skew_angle_as_frames();
}

template <>
inline const LottieFlatBuffer::FloatScalar *Transform::skew_angle_as<LottieFlatBuffer::FloatScalar>() const
{
    return skew_angle_as_value();
}

template <>
inline const LottieFlatBuffer::FloatFrameArray *Transform::skew_axis_as<LottieFlatBuffer::FloatFrameArray>() const
{
    return skew_axis_as_frames();
}

template <>
inline const LottieFlatBuffer::FloatScalar *Transform::skew_axis_as<LottieFlatBuffer::FloatScalar>() const
{
    return skew_axis_as_value();
}

struct TransformBuilder {
    typedef Transform Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_anchor_type(LottieFlatBuffer::VectorProp anchor_type)
    {
        fbb_.AddElement<uint8_t>(Transform::VT_ANCHOR_TYPE, static_cast<uint8_t>(anchor_type), 0);
    }
    void add_anchor(::flatbuffers::Offset<void> anchor)
    {
        fbb_.AddOffset(Transform::VT_ANCHOR, anchor);
    }
    void add_position_type(LottieFlatBuffer::PositionProp position_type)
    {
        fbb_.AddElement<uint8_t>(Transform::VT_POSITION_TYPE, static_cast<uint8_t>(position_type), 0);
    }
    void add_position(::flatbuffers::Offset<void> position)
    {
        fbb_.AddOffset(Transform::VT_POSITION, position);
    }
    void add_split_position(::flatbuffers::Offset<LottieFlatBuffer::SplitProp> split_position)
    {
        fbb_.AddOffset(Transform::VT_SPLIT_POSITION, split_position);
    }
    void add_scale_type(LottieFlatBuffer::VectorProp scale_type)
    {
        fbb_.AddElement<uint8_t>(Transform::VT_SCALE_TYPE, static_cast<uint8_t>(scale_type), 0);
    }
    void add_scale(::flatbuffers::Offset<void> scale)
    {
        fbb_.AddOffset(Transform::VT_SCALE, scale);
    }
    void add_rotation_type(LottieFlatBuffer::FloatProp rotation_type)
    {
        fbb_.AddElement<uint8_t>(Transform::VT_ROTATION_TYPE, static_cast<uint8_t>(rotation_type), 0);
    }
    void add_rotation(::flatbuffers::Offset<void> rotation)
    {
        fbb_.AddOffset(Transform::VT_ROTATION, rotation);
    }
    void add_opacity_type(LottieFlatBuffer::IntegerProp opacity_type)
    {
        fbb_.AddElement<uint8_t>(Transform::VT_OPACITY_TYPE, static_cast<uint8_t>(opacity_type), 0);
    }
    void add_opacity(::flatbuffers::Offset<void> opacity)
    {
        fbb_.AddOffset(Transform::VT_OPACITY, opacity);
    }
    void add_skew_angle_type(LottieFlatBuffer::FloatProp skew_angle_type)
    {
        fbb_.AddElement<uint8_t>(Transform::VT_SKEW_ANGLE_TYPE, static_cast<uint8_t>(skew_angle_type), 0);
    }
    void add_skew_angle(::flatbuffers::Offset<void> skew_angle)
    {
        fbb_.AddOffset(Transform::VT_SKEW_ANGLE, skew_angle);
    }
    void add_skew_axis_type(LottieFlatBuffer::FloatProp skew_axis_type)
    {
        fbb_.AddElement<uint8_t>(Transform::VT_SKEW_AXIS_TYPE, static_cast<uint8_t>(skew_axis_type), 0);
    }
    void add_skew_axis(::flatbuffers::Offset<void> skew_axis)
    {
        fbb_.AddOffset(Transform::VT_SKEW_AXIS, skew_axis);
    }
    explicit TransformBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<Transform> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<Transform>(end);
        return o;
    }
};

inline ::flatbuffers::Offset<Transform> CreateTransform(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    LottieFlatBuffer::VectorProp anchor_type = LottieFlatBuffer::VectorProp_NONE,
    ::flatbuffers::Offset<void> anchor = 0,
    LottieFlatBuffer::PositionProp position_type = LottieFlatBuffer::PositionProp_NONE,
    ::flatbuffers::Offset<void> position = 0, ::flatbuffers::Offset<LottieFlatBuffer::SplitProp> split_position = 0,
    LottieFlatBuffer::VectorProp scale_type = LottieFlatBuffer::VectorProp_NONE, ::flatbuffers::Offset<void> scale = 0,
    LottieFlatBuffer::FloatProp rotation_type = LottieFlatBuffer::FloatProp_NONE,
    ::flatbuffers::Offset<void> rotation = 0,
    LottieFlatBuffer::IntegerProp opacity_type = LottieFlatBuffer::IntegerProp_NONE,
    ::flatbuffers::Offset<void> opacity = 0,
    LottieFlatBuffer::FloatProp skew_angle_type = LottieFlatBuffer::FloatProp_NONE,
    ::flatbuffers::Offset<void> skew_angle = 0,
    LottieFlatBuffer::FloatProp skew_axis_type = LottieFlatBuffer::FloatProp_NONE,
    ::flatbuffers::Offset<void> skew_axis = 0)
{
    TransformBuilder builder_(_fbb);
    builder_.add_skew_axis(skew_axis);
    builder_.add_skew_angle(skew_angle);
    builder_.add_opacity(opacity);
    builder_.add_rotation(rotation);
    builder_.add_scale(scale);
    builder_.add_split_position(split_position);
    builder_.add_position(position);
    builder_.add_anchor(anchor);
    builder_.add_skew_axis_type(skew_axis_type);
    builder_.add_skew_angle_type(skew_angle_type);
    builder_.add_opacity_type(opacity_type);
    builder_.add_rotation_type(rotation_type);
    builder_.add_scale_type(scale_type);
    builder_.add_position_type(position_type);
    builder_.add_anchor_type(anchor_type);
    return builder_.Finish();
}

struct EllipseShape FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef EllipseShapeBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_CLOCKWISE = 4,
        VT_POS_TYPE = 6,
        VT_POS = 8,
        VT_SIZE_TYPE = 10,
        VT_SIZE = 12
    };
    bool clockwise() const
    {
        return GetField<uint8_t>(VT_CLOCKWISE, 1) != 0;
    }
    LottieFlatBuffer::PositionProp pos_type() const
    {
        return static_cast<LottieFlatBuffer::PositionProp>(GetField<uint8_t>(VT_POS_TYPE, 0));
    }
    const void *pos() const
    {
        return GetPointer<const void *>(VT_POS);
    }
    template <typename T>
    const T *pos_as() const;
    const LottieFlatBuffer::PositionFrameArray *pos_as_frames() const
    {
        return pos_type() == LottieFlatBuffer::PositionProp_frames ?
                   static_cast<const LottieFlatBuffer::PositionFrameArray *>(pos()) :
                   nullptr;
    }
    const LottieFlatBuffer::Point *pos_as_value() const
    {
        return pos_type() == LottieFlatBuffer::PositionProp_value ?
                   static_cast<const LottieFlatBuffer::Point *>(pos()) :
                   nullptr;
    }
    LottieFlatBuffer::VectorProp size_type() const
    {
        return static_cast<LottieFlatBuffer::VectorProp>(GetField<uint8_t>(VT_SIZE_TYPE, 0));
    }
    const void *size() const
    {
        return GetPointer<const void *>(VT_SIZE);
    }
    template <typename T>
    const T *size_as() const;
    const LottieFlatBuffer::VectorFrameArray *size_as_frames() const
    {
        return size_type() == LottieFlatBuffer::VectorProp_frames ?
                   static_cast<const LottieFlatBuffer::VectorFrameArray *>(size()) :
                   nullptr;
    }
    const LottieFlatBuffer::Point *size_as_value() const
    {
        return size_type() == LottieFlatBuffer::VectorProp_value ?
                   static_cast<const LottieFlatBuffer::Point *>(size()) :
                   nullptr;
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyField<uint8_t>(verifier, VT_CLOCKWISE, 1) &&
               VerifyField<uint8_t>(verifier, VT_POS_TYPE, 1) && VerifyOffsetRequired(verifier, VT_POS) &&
               VerifyPositionProp(verifier, pos(), pos_type()) && VerifyField<uint8_t>(verifier, VT_SIZE_TYPE, 1) &&
               VerifyOffsetRequired(verifier, VT_SIZE) && VerifyVectorProp(verifier, size(), size_type()) &&
               verifier.EndTable();
    }
};

template <>
inline const LottieFlatBuffer::PositionFrameArray *EllipseShape::pos_as<LottieFlatBuffer::PositionFrameArray>() const
{
    return pos_as_frames();
}

template <>
inline const LottieFlatBuffer::Point *EllipseShape::pos_as<LottieFlatBuffer::Point>() const
{
    return pos_as_value();
}

template <>
inline const LottieFlatBuffer::VectorFrameArray *EllipseShape::size_as<LottieFlatBuffer::VectorFrameArray>() const
{
    return size_as_frames();
}

template <>
inline const LottieFlatBuffer::Point *EllipseShape::size_as<LottieFlatBuffer::Point>() const
{
    return size_as_value();
}

struct EllipseShapeBuilder {
    typedef EllipseShape Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_clockwise(bool clockwise)
    {
        fbb_.AddElement<uint8_t>(EllipseShape::VT_CLOCKWISE, static_cast<uint8_t>(clockwise), 1);
    }
    void add_pos_type(LottieFlatBuffer::PositionProp pos_type)
    {
        fbb_.AddElement<uint8_t>(EllipseShape::VT_POS_TYPE, static_cast<uint8_t>(pos_type), 0);
    }
    void add_pos(::flatbuffers::Offset<void> pos)
    {
        fbb_.AddOffset(EllipseShape::VT_POS, pos);
    }
    void add_size_type(LottieFlatBuffer::VectorProp size_type)
    {
        fbb_.AddElement<uint8_t>(EllipseShape::VT_SIZE_TYPE, static_cast<uint8_t>(size_type), 0);
    }
    void add_size(::flatbuffers::Offset<void> size)
    {
        fbb_.AddOffset(EllipseShape::VT_SIZE, size);
    }
    explicit EllipseShapeBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<EllipseShape> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<EllipseShape>(end);
        fbb_.Required(o, EllipseShape::VT_POS);
        fbb_.Required(o, EllipseShape::VT_SIZE);
        return o;
    }
};

inline ::flatbuffers::Offset<EllipseShape> CreateEllipseShape(
    ::flatbuffers::FlatBufferBuilder &_fbb, bool clockwise = true,
    LottieFlatBuffer::PositionProp pos_type = LottieFlatBuffer::PositionProp_NONE, ::flatbuffers::Offset<void> pos = 0,
    LottieFlatBuffer::VectorProp size_type = LottieFlatBuffer::VectorProp_NONE, ::flatbuffers::Offset<void> size = 0)
{
    EllipseShapeBuilder builder_(_fbb);
    builder_.add_size(size);
    builder_.add_pos(pos);
    builder_.add_size_type(size_type);
    builder_.add_pos_type(pos_type);
    builder_.add_clockwise(clockwise);
    return builder_.Finish();
}

struct RectShape FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef RectShapeBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_CLOCKWISE = 4,
        VT_POS_TYPE = 6,
        VT_POS = 8,
        VT_SIZE_TYPE = 10,
        VT_SIZE = 12,
        VT_RADIUS_TYPE = 14,
        VT_RADIUS = 16
    };
    bool clockwise() const
    {
        return GetField<uint8_t>(VT_CLOCKWISE, 1) != 0;
    }
    LottieFlatBuffer::PositionProp pos_type() const
    {
        return static_cast<LottieFlatBuffer::PositionProp>(GetField<uint8_t>(VT_POS_TYPE, 0));
    }
    const void *pos() const
    {
        return GetPointer<const void *>(VT_POS);
    }
    template <typename T>
    const T *pos_as() const;
    const LottieFlatBuffer::PositionFrameArray *pos_as_frames() const
    {
        return pos_type() == LottieFlatBuffer::PositionProp_frames ?
                   static_cast<const LottieFlatBuffer::PositionFrameArray *>(pos()) :
                   nullptr;
    }
    const LottieFlatBuffer::Point *pos_as_value() const
    {
        return pos_type() == LottieFlatBuffer::PositionProp_value ?
                   static_cast<const LottieFlatBuffer::Point *>(pos()) :
                   nullptr;
    }
    LottieFlatBuffer::VectorProp size_type() const
    {
        return static_cast<LottieFlatBuffer::VectorProp>(GetField<uint8_t>(VT_SIZE_TYPE, 0));
    }
    const void *size() const
    {
        return GetPointer<const void *>(VT_SIZE);
    }
    template <typename T>
    const T *size_as() const;
    const LottieFlatBuffer::VectorFrameArray *size_as_frames() const
    {
        return size_type() == LottieFlatBuffer::VectorProp_frames ?
                   static_cast<const LottieFlatBuffer::VectorFrameArray *>(size()) :
                   nullptr;
    }
    const LottieFlatBuffer::Point *size_as_value() const
    {
        return size_type() == LottieFlatBuffer::VectorProp_value ?
                   static_cast<const LottieFlatBuffer::Point *>(size()) :
                   nullptr;
    }
    LottieFlatBuffer::FloatProp radius_type() const
    {
        return static_cast<LottieFlatBuffer::FloatProp>(GetField<uint8_t>(VT_RADIUS_TYPE, 0));
    }
    const void *radius() const
    {
        return GetPointer<const void *>(VT_RADIUS);
    }
    template <typename T>
    const T *radius_as() const;
    const LottieFlatBuffer::FloatFrameArray *radius_as_frames() const
    {
        return radius_type() == LottieFlatBuffer::FloatProp_frames ?
                   static_cast<const LottieFlatBuffer::FloatFrameArray *>(radius()) :
                   nullptr;
    }
    const LottieFlatBuffer::FloatScalar *radius_as_value() const
    {
        return radius_type() == LottieFlatBuffer::FloatProp_value ?
                   static_cast<const LottieFlatBuffer::FloatScalar *>(radius()) :
                   nullptr;
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyField<uint8_t>(verifier, VT_CLOCKWISE, 1) &&
               VerifyField<uint8_t>(verifier, VT_POS_TYPE, 1) && VerifyOffsetRequired(verifier, VT_POS) &&
               VerifyPositionProp(verifier, pos(), pos_type()) && VerifyField<uint8_t>(verifier, VT_SIZE_TYPE, 1) &&
               VerifyOffsetRequired(verifier, VT_SIZE) && VerifyVectorProp(verifier, size(), size_type()) &&
               VerifyField<uint8_t>(verifier, VT_RADIUS_TYPE, 1) && VerifyOffsetRequired(verifier, VT_RADIUS) &&
               VerifyFloatProp(verifier, radius(), radius_type()) && verifier.EndTable();
    }
};

template <>
inline const LottieFlatBuffer::PositionFrameArray *RectShape::pos_as<LottieFlatBuffer::PositionFrameArray>() const
{
    return pos_as_frames();
}

template <>
inline const LottieFlatBuffer::Point *RectShape::pos_as<LottieFlatBuffer::Point>() const
{
    return pos_as_value();
}

template <>
inline const LottieFlatBuffer::VectorFrameArray *RectShape::size_as<LottieFlatBuffer::VectorFrameArray>() const
{
    return size_as_frames();
}

template <>
inline const LottieFlatBuffer::Point *RectShape::size_as<LottieFlatBuffer::Point>() const
{
    return size_as_value();
}

template <>
inline const LottieFlatBuffer::FloatFrameArray *RectShape::radius_as<LottieFlatBuffer::FloatFrameArray>() const
{
    return radius_as_frames();
}

template <>
inline const LottieFlatBuffer::FloatScalar *RectShape::radius_as<LottieFlatBuffer::FloatScalar>() const
{
    return radius_as_value();
}

struct RectShapeBuilder {
    typedef RectShape Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_clockwise(bool clockwise)
    {
        fbb_.AddElement<uint8_t>(RectShape::VT_CLOCKWISE, static_cast<uint8_t>(clockwise), 1);
    }
    void add_pos_type(LottieFlatBuffer::PositionProp pos_type)
    {
        fbb_.AddElement<uint8_t>(RectShape::VT_POS_TYPE, static_cast<uint8_t>(pos_type), 0);
    }
    void add_pos(::flatbuffers::Offset<void> pos)
    {
        fbb_.AddOffset(RectShape::VT_POS, pos);
    }
    void add_size_type(LottieFlatBuffer::VectorProp size_type)
    {
        fbb_.AddElement<uint8_t>(RectShape::VT_SIZE_TYPE, static_cast<uint8_t>(size_type), 0);
    }
    void add_size(::flatbuffers::Offset<void> size)
    {
        fbb_.AddOffset(RectShape::VT_SIZE, size);
    }
    void add_radius_type(LottieFlatBuffer::FloatProp radius_type)
    {
        fbb_.AddElement<uint8_t>(RectShape::VT_RADIUS_TYPE, static_cast<uint8_t>(radius_type), 0);
    }
    void add_radius(::flatbuffers::Offset<void> radius)
    {
        fbb_.AddOffset(RectShape::VT_RADIUS, radius);
    }
    explicit RectShapeBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<RectShape> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<RectShape>(end);
        fbb_.Required(o, RectShape::VT_POS);
        fbb_.Required(o, RectShape::VT_SIZE);
        fbb_.Required(o, RectShape::VT_RADIUS);
        return o;
    }
};

inline ::flatbuffers::Offset<RectShape> CreateRectShape(
    ::flatbuffers::FlatBufferBuilder &_fbb, bool clockwise = true,
    LottieFlatBuffer::PositionProp pos_type = LottieFlatBuffer::PositionProp_NONE, ::flatbuffers::Offset<void> pos = 0,
    LottieFlatBuffer::VectorProp size_type = LottieFlatBuffer::VectorProp_NONE, ::flatbuffers::Offset<void> size = 0,
    LottieFlatBuffer::FloatProp radius_type = LottieFlatBuffer::FloatProp_NONE, ::flatbuffers::Offset<void> radius = 0)
{
    RectShapeBuilder builder_(_fbb);
    builder_.add_radius(radius);
    builder_.add_size(size);
    builder_.add_pos(pos);
    builder_.add_radius_type(radius_type);
    builder_.add_size_type(size_type);
    builder_.add_pos_type(pos_type);
    builder_.add_clockwise(clockwise);
    return builder_.Finish();
}

struct PathShape FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef PathShapeBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_CLOCKWISE = 4,
        VT_BEZIER_TYPE = 6,
        VT_BEZIER = 8
    };
    bool clockwise() const
    {
        return GetField<uint8_t>(VT_CLOCKWISE, 1) != 0;
    }
    LottieFlatBuffer::BezierDataProp bezier_type() const
    {
        return static_cast<LottieFlatBuffer::BezierDataProp>(GetField<uint8_t>(VT_BEZIER_TYPE, 0));
    }
    const void *bezier() const
    {
        return GetPointer<const void *>(VT_BEZIER);
    }
    template <typename T>
    const T *bezier_as() const;
    const LottieFlatBuffer::BezierDataFrameArray *bezier_as_frames() const
    {
        return bezier_type() == LottieFlatBuffer::BezierDataProp_frames ?
                   static_cast<const LottieFlatBuffer::BezierDataFrameArray *>(bezier()) :
                   nullptr;
    }
    const LottieFlatBuffer::BezierData *bezier_as_value() const
    {
        return bezier_type() == LottieFlatBuffer::BezierDataProp_value ?
                   static_cast<const LottieFlatBuffer::BezierData *>(bezier()) :
                   nullptr;
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyField<uint8_t>(verifier, VT_CLOCKWISE, 1) &&
               VerifyField<uint8_t>(verifier, VT_BEZIER_TYPE, 1) && VerifyOffsetRequired(verifier, VT_BEZIER) &&
               VerifyBezierDataProp(verifier, bezier(), bezier_type()) && verifier.EndTable();
    }
};

template <>
inline const LottieFlatBuffer::BezierDataFrameArray *
PathShape::bezier_as<LottieFlatBuffer::BezierDataFrameArray>() const
{
    return bezier_as_frames();
}

template <>
inline const LottieFlatBuffer::BezierData *PathShape::bezier_as<LottieFlatBuffer::BezierData>() const
{
    return bezier_as_value();
}

struct PathShapeBuilder {
    typedef PathShape Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_clockwise(bool clockwise)
    {
        fbb_.AddElement<uint8_t>(PathShape::VT_CLOCKWISE, static_cast<uint8_t>(clockwise), 1);
    }
    void add_bezier_type(LottieFlatBuffer::BezierDataProp bezier_type)
    {
        fbb_.AddElement<uint8_t>(PathShape::VT_BEZIER_TYPE, static_cast<uint8_t>(bezier_type), 0);
    }
    void add_bezier(::flatbuffers::Offset<void> bezier)
    {
        fbb_.AddOffset(PathShape::VT_BEZIER, bezier);
    }
    explicit PathShapeBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<PathShape> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<PathShape>(end);
        fbb_.Required(o, PathShape::VT_BEZIER);
        return o;
    }
};

inline ::flatbuffers::Offset<PathShape> CreatePathShape(
    ::flatbuffers::FlatBufferBuilder &_fbb, bool clockwise = true,
    LottieFlatBuffer::BezierDataProp bezier_type = LottieFlatBuffer::BezierDataProp_NONE,
    ::flatbuffers::Offset<void> bezier = 0)
{
    PathShapeBuilder builder_(_fbb);
    builder_.add_bezier(bezier);
    builder_.add_bezier_type(bezier_type);
    builder_.add_clockwise(clockwise);
    return builder_.Finish();
}

struct FillStyle FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef FillStyleBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_OPACITY_TYPE = 4,
        VT_OPACITY = 6,
        VT_COLOR_TYPE = 8,
        VT_COLOR = 10,
        VT_RULE = 12
    };
    LottieFlatBuffer::IntegerProp opacity_type() const
    {
        return static_cast<LottieFlatBuffer::IntegerProp>(GetField<uint8_t>(VT_OPACITY_TYPE, 0));
    }
    const void *opacity() const
    {
        return GetPointer<const void *>(VT_OPACITY);
    }
    template <typename T>
    const T *opacity_as() const;
    const LottieFlatBuffer::IntegerFrameArray *opacity_as_frames() const
    {
        return opacity_type() == LottieFlatBuffer::IntegerProp_frames ?
                   static_cast<const LottieFlatBuffer::IntegerFrameArray *>(opacity()) :
                   nullptr;
    }
    const LottieFlatBuffer::IntegerScalar *opacity_as_value() const
    {
        return opacity_type() == LottieFlatBuffer::IntegerProp_value ?
                   static_cast<const LottieFlatBuffer::IntegerScalar *>(opacity()) :
                   nullptr;
    }
    LottieFlatBuffer::ColorProp color_type() const
    {
        return static_cast<LottieFlatBuffer::ColorProp>(GetField<uint8_t>(VT_COLOR_TYPE, 0));
    }
    const void *color() const
    {
        return GetPointer<const void *>(VT_COLOR);
    }
    template <typename T>
    const T *color_as() const;
    const LottieFlatBuffer::ColorFrameArray *color_as_frames() const
    {
        return color_type() == LottieFlatBuffer::ColorProp_frames ?
                   static_cast<const LottieFlatBuffer::ColorFrameArray *>(color()) :
                   nullptr;
    }
    const LottieFlatBuffer::Color24Scalar *color_as_color() const
    {
        return color_type() == LottieFlatBuffer::ColorProp_color ?
                   static_cast<const LottieFlatBuffer::Color24Scalar *>(color()) :
                   nullptr;
    }
    LottieFlatBuffer::FillRule rule() const
    {
        return static_cast<LottieFlatBuffer::FillRule>(GetField<int8_t>(VT_RULE, 0));
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyField<uint8_t>(verifier, VT_OPACITY_TYPE, 1) &&
               VerifyOffsetRequired(verifier, VT_OPACITY) && VerifyIntegerProp(verifier, opacity(), opacity_type()) &&
               VerifyField<uint8_t>(verifier, VT_COLOR_TYPE, 1) && VerifyOffsetRequired(verifier, VT_COLOR) &&
               VerifyColorProp(verifier, color(), color_type()) && VerifyField<int8_t>(verifier, VT_RULE, 1) &&
               verifier.EndTable();
    }
};

template <>
inline const LottieFlatBuffer::IntegerFrameArray *FillStyle::opacity_as<LottieFlatBuffer::IntegerFrameArray>() const
{
    return opacity_as_frames();
}

template <>
inline const LottieFlatBuffer::IntegerScalar *FillStyle::opacity_as<LottieFlatBuffer::IntegerScalar>() const
{
    return opacity_as_value();
}

template <>
inline const LottieFlatBuffer::ColorFrameArray *FillStyle::color_as<LottieFlatBuffer::ColorFrameArray>() const
{
    return color_as_frames();
}

template <>
inline const LottieFlatBuffer::Color24Scalar *FillStyle::color_as<LottieFlatBuffer::Color24Scalar>() const
{
    return color_as_color();
}

struct FillStyleBuilder {
    typedef FillStyle Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_opacity_type(LottieFlatBuffer::IntegerProp opacity_type)
    {
        fbb_.AddElement<uint8_t>(FillStyle::VT_OPACITY_TYPE, static_cast<uint8_t>(opacity_type), 0);
    }
    void add_opacity(::flatbuffers::Offset<void> opacity)
    {
        fbb_.AddOffset(FillStyle::VT_OPACITY, opacity);
    }
    void add_color_type(LottieFlatBuffer::ColorProp color_type)
    {
        fbb_.AddElement<uint8_t>(FillStyle::VT_COLOR_TYPE, static_cast<uint8_t>(color_type), 0);
    }
    void add_color(::flatbuffers::Offset<void> color)
    {
        fbb_.AddOffset(FillStyle::VT_COLOR, color);
    }
    void add_rule(LottieFlatBuffer::FillRule rule)
    {
        fbb_.AddElement<int8_t>(FillStyle::VT_RULE, static_cast<int8_t>(rule), 0);
    }
    explicit FillStyleBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<FillStyle> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<FillStyle>(end);
        fbb_.Required(o, FillStyle::VT_OPACITY);
        fbb_.Required(o, FillStyle::VT_COLOR);
        return o;
    }
};

inline ::flatbuffers::Offset<FillStyle> CreateFillStyle(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    LottieFlatBuffer::IntegerProp opacity_type = LottieFlatBuffer::IntegerProp_NONE,
    ::flatbuffers::Offset<void> opacity = 0, LottieFlatBuffer::ColorProp color_type = LottieFlatBuffer::ColorProp_NONE,
    ::flatbuffers::Offset<void> color = 0, LottieFlatBuffer::FillRule rule = LottieFlatBuffer::FillRule_NON_ZERO)
{
    FillStyleBuilder builder_(_fbb);
    builder_.add_color(color);
    builder_.add_opacity(opacity);
    builder_.add_rule(rule);
    builder_.add_color_type(color_type);
    builder_.add_opacity_type(opacity_type);
    return builder_.Finish();
}

struct StrokeStyle FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef StrokeStyleBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_OPACITY_TYPE = 4,
        VT_OPACITY = 6,
        VT_COLOR_TYPE = 8,
        VT_COLOR = 10,
        VT_WIDTH_TYPE = 12,
        VT_WIDTH = 14,
        VT_CAP = 16,
        VT_JOIN = 18,
        VT_MITER_LIMIT = 20
    };
    LottieFlatBuffer::IntegerProp opacity_type() const
    {
        return static_cast<LottieFlatBuffer::IntegerProp>(GetField<uint8_t>(VT_OPACITY_TYPE, 0));
    }
    const void *opacity() const
    {
        return GetPointer<const void *>(VT_OPACITY);
    }
    template <typename T>
    const T *opacity_as() const;
    const LottieFlatBuffer::IntegerFrameArray *opacity_as_frames() const
    {
        return opacity_type() == LottieFlatBuffer::IntegerProp_frames ?
                   static_cast<const LottieFlatBuffer::IntegerFrameArray *>(opacity()) :
                   nullptr;
    }
    const LottieFlatBuffer::IntegerScalar *opacity_as_value() const
    {
        return opacity_type() == LottieFlatBuffer::IntegerProp_value ?
                   static_cast<const LottieFlatBuffer::IntegerScalar *>(opacity()) :
                   nullptr;
    }
    LottieFlatBuffer::ColorProp color_type() const
    {
        return static_cast<LottieFlatBuffer::ColorProp>(GetField<uint8_t>(VT_COLOR_TYPE, 0));
    }
    const void *color() const
    {
        return GetPointer<const void *>(VT_COLOR);
    }
    template <typename T>
    const T *color_as() const;
    const LottieFlatBuffer::ColorFrameArray *color_as_frames() const
    {
        return color_type() == LottieFlatBuffer::ColorProp_frames ?
                   static_cast<const LottieFlatBuffer::ColorFrameArray *>(color()) :
                   nullptr;
    }
    const LottieFlatBuffer::Color24Scalar *color_as_color() const
    {
        return color_type() == LottieFlatBuffer::ColorProp_color ?
                   static_cast<const LottieFlatBuffer::Color24Scalar *>(color()) :
                   nullptr;
    }
    LottieFlatBuffer::FloatProp width_type() const
    {
        return static_cast<LottieFlatBuffer::FloatProp>(GetField<uint8_t>(VT_WIDTH_TYPE, 0));
    }
    const void *width() const
    {
        return GetPointer<const void *>(VT_WIDTH);
    }
    template <typename T>
    const T *width_as() const;
    const LottieFlatBuffer::FloatFrameArray *width_as_frames() const
    {
        return width_type() == LottieFlatBuffer::FloatProp_frames ?
                   static_cast<const LottieFlatBuffer::FloatFrameArray *>(width()) :
                   nullptr;
    }
    const LottieFlatBuffer::FloatScalar *width_as_value() const
    {
        return width_type() == LottieFlatBuffer::FloatProp_value ?
                   static_cast<const LottieFlatBuffer::FloatScalar *>(width()) :
                   nullptr;
    }
    LottieFlatBuffer::LineCap cap() const
    {
        return static_cast<LottieFlatBuffer::LineCap>(GetField<int8_t>(VT_CAP, 1));
    }
    LottieFlatBuffer::LineJoin join() const
    {
        return static_cast<LottieFlatBuffer::LineJoin>(GetField<int8_t>(VT_JOIN, 1));
    }
    float miter_limit() const
    {
        return GetField<float>(VT_MITER_LIMIT, 0.0f);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyField<uint8_t>(verifier, VT_OPACITY_TYPE, 1) &&
               VerifyOffsetRequired(verifier, VT_OPACITY) && VerifyIntegerProp(verifier, opacity(), opacity_type()) &&
               VerifyField<uint8_t>(verifier, VT_COLOR_TYPE, 1) && VerifyOffsetRequired(verifier, VT_COLOR) &&
               VerifyColorProp(verifier, color(), color_type()) && VerifyField<uint8_t>(verifier, VT_WIDTH_TYPE, 1) &&
               VerifyOffsetRequired(verifier, VT_WIDTH) && VerifyFloatProp(verifier, width(), width_type()) &&
               VerifyField<int8_t>(verifier, VT_CAP, 1) && VerifyField<int8_t>(verifier, VT_JOIN, 1) &&
               VerifyField<float>(verifier, VT_MITER_LIMIT, 4) && verifier.EndTable();
    }
};

template <>
inline const LottieFlatBuffer::IntegerFrameArray *StrokeStyle::opacity_as<LottieFlatBuffer::IntegerFrameArray>() const
{
    return opacity_as_frames();
}

template <>
inline const LottieFlatBuffer::IntegerScalar *StrokeStyle::opacity_as<LottieFlatBuffer::IntegerScalar>() const
{
    return opacity_as_value();
}

template <>
inline const LottieFlatBuffer::ColorFrameArray *StrokeStyle::color_as<LottieFlatBuffer::ColorFrameArray>() const
{
    return color_as_frames();
}

template <>
inline const LottieFlatBuffer::Color24Scalar *StrokeStyle::color_as<LottieFlatBuffer::Color24Scalar>() const
{
    return color_as_color();
}

template <>
inline const LottieFlatBuffer::FloatFrameArray *StrokeStyle::width_as<LottieFlatBuffer::FloatFrameArray>() const
{
    return width_as_frames();
}

template <>
inline const LottieFlatBuffer::FloatScalar *StrokeStyle::width_as<LottieFlatBuffer::FloatScalar>() const
{
    return width_as_value();
}

struct StrokeStyleBuilder {
    typedef StrokeStyle Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_opacity_type(LottieFlatBuffer::IntegerProp opacity_type)
    {
        fbb_.AddElement<uint8_t>(StrokeStyle::VT_OPACITY_TYPE, static_cast<uint8_t>(opacity_type), 0);
    }
    void add_opacity(::flatbuffers::Offset<void> opacity)
    {
        fbb_.AddOffset(StrokeStyle::VT_OPACITY, opacity);
    }
    void add_color_type(LottieFlatBuffer::ColorProp color_type)
    {
        fbb_.AddElement<uint8_t>(StrokeStyle::VT_COLOR_TYPE, static_cast<uint8_t>(color_type), 0);
    }
    void add_color(::flatbuffers::Offset<void> color)
    {
        fbb_.AddOffset(StrokeStyle::VT_COLOR, color);
    }
    void add_width_type(LottieFlatBuffer::FloatProp width_type)
    {
        fbb_.AddElement<uint8_t>(StrokeStyle::VT_WIDTH_TYPE, static_cast<uint8_t>(width_type), 0);
    }
    void add_width(::flatbuffers::Offset<void> width)
    {
        fbb_.AddOffset(StrokeStyle::VT_WIDTH, width);
    }
    void add_cap(LottieFlatBuffer::LineCap cap)
    {
        fbb_.AddElement<int8_t>(StrokeStyle::VT_CAP, static_cast<int8_t>(cap), 1);
    }
    void add_join(LottieFlatBuffer::LineJoin join)
    {
        fbb_.AddElement<int8_t>(StrokeStyle::VT_JOIN, static_cast<int8_t>(join), 1);
    }
    void add_miter_limit(float miter_limit)
    {
        fbb_.AddElement<float>(StrokeStyle::VT_MITER_LIMIT, miter_limit, 0.0f);
    }
    explicit StrokeStyleBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<StrokeStyle> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<StrokeStyle>(end);
        fbb_.Required(o, StrokeStyle::VT_OPACITY);
        fbb_.Required(o, StrokeStyle::VT_COLOR);
        fbb_.Required(o, StrokeStyle::VT_WIDTH);
        return o;
    }
};

inline ::flatbuffers::Offset<StrokeStyle> CreateStrokeStyle(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    LottieFlatBuffer::IntegerProp opacity_type = LottieFlatBuffer::IntegerProp_NONE,
    ::flatbuffers::Offset<void> opacity = 0, LottieFlatBuffer::ColorProp color_type = LottieFlatBuffer::ColorProp_NONE,
    ::flatbuffers::Offset<void> color = 0, LottieFlatBuffer::FloatProp width_type = LottieFlatBuffer::FloatProp_NONE,
    ::flatbuffers::Offset<void> width = 0, LottieFlatBuffer::LineCap cap = LottieFlatBuffer::LineCap_CAP_ROUND,
    LottieFlatBuffer::LineJoin join = LottieFlatBuffer::LineJoin_JOIN_ROUND, float miter_limit = 0.0f)
{
    StrokeStyleBuilder builder_(_fbb);
    builder_.add_miter_limit(miter_limit);
    builder_.add_width(width);
    builder_.add_color(color);
    builder_.add_opacity(opacity);
    builder_.add_join(join);
    builder_.add_cap(cap);
    builder_.add_width_type(width_type);
    builder_.add_color_type(color_type);
    builder_.add_opacity_type(opacity_type);
    return builder_.Finish();
}

struct GradientFillStyle FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef GradientFillStyleBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_OPACITY_TYPE = 4,
        VT_OPACITY = 6,
        VT_START_POS_TYPE = 8,
        VT_START_POS = 10,
        VT_END_POS_TYPE = 12,
        VT_END_POS = 14,
        VT_GRADIENT_TYPE = 16,
        VT_GRADIENT = 18,
        VT_TYPE = 20,
        VT_RULE = 22
    };
    LottieFlatBuffer::IntegerProp opacity_type() const
    {
        return static_cast<LottieFlatBuffer::IntegerProp>(GetField<uint8_t>(VT_OPACITY_TYPE, 0));
    }
    const void *opacity() const
    {
        return GetPointer<const void *>(VT_OPACITY);
    }
    template <typename T>
    const T *opacity_as() const;
    const LottieFlatBuffer::IntegerFrameArray *opacity_as_frames() const
    {
        return opacity_type() == LottieFlatBuffer::IntegerProp_frames ?
                   static_cast<const LottieFlatBuffer::IntegerFrameArray *>(opacity()) :
                   nullptr;
    }
    const LottieFlatBuffer::IntegerScalar *opacity_as_value() const
    {
        return opacity_type() == LottieFlatBuffer::IntegerProp_value ?
                   static_cast<const LottieFlatBuffer::IntegerScalar *>(opacity()) :
                   nullptr;
    }
    LottieFlatBuffer::VectorProp start_pos_type() const
    {
        return static_cast<LottieFlatBuffer::VectorProp>(GetField<uint8_t>(VT_START_POS_TYPE, 0));
    }
    const void *start_pos() const
    {
        return GetPointer<const void *>(VT_START_POS);
    }
    template <typename T>
    const T *start_pos_as() const;
    const LottieFlatBuffer::VectorFrameArray *start_pos_as_frames() const
    {
        return start_pos_type() == LottieFlatBuffer::VectorProp_frames ?
                   static_cast<const LottieFlatBuffer::VectorFrameArray *>(start_pos()) :
                   nullptr;
    }
    const LottieFlatBuffer::Point *start_pos_as_value() const
    {
        return start_pos_type() == LottieFlatBuffer::VectorProp_value ?
                   static_cast<const LottieFlatBuffer::Point *>(start_pos()) :
                   nullptr;
    }
    LottieFlatBuffer::VectorProp end_pos_type() const
    {
        return static_cast<LottieFlatBuffer::VectorProp>(GetField<uint8_t>(VT_END_POS_TYPE, 0));
    }
    const void *end_pos() const
    {
        return GetPointer<const void *>(VT_END_POS);
    }
    template <typename T>
    const T *end_pos_as() const;
    const LottieFlatBuffer::VectorFrameArray *end_pos_as_frames() const
    {
        return end_pos_type() == LottieFlatBuffer::VectorProp_frames ?
                   static_cast<const LottieFlatBuffer::VectorFrameArray *>(end_pos()) :
                   nullptr;
    }
    const LottieFlatBuffer::Point *end_pos_as_value() const
    {
        return end_pos_type() == LottieFlatBuffer::VectorProp_value ?
                   static_cast<const LottieFlatBuffer::Point *>(end_pos()) :
                   nullptr;
    }
    LottieFlatBuffer::GradientProp gradient_type() const
    {
        return static_cast<LottieFlatBuffer::GradientProp>(GetField<uint8_t>(VT_GRADIENT_TYPE, 0));
    }
    const void *gradient() const
    {
        return GetPointer<const void *>(VT_GRADIENT);
    }
    template <typename T>
    const T *gradient_as() const;
    const LottieFlatBuffer::GradientFrameArray *gradient_as_frames() const
    {
        return gradient_type() == LottieFlatBuffer::GradientProp_frames ?
                   static_cast<const LottieFlatBuffer::GradientFrameArray *>(gradient()) :
                   nullptr;
    }
    const LottieFlatBuffer::Gradient *gradient_as_value() const
    {
        return gradient_type() == LottieFlatBuffer::GradientProp_value ?
                   static_cast<const LottieFlatBuffer::Gradient *>(gradient()) :
                   nullptr;
    }
    LottieFlatBuffer::GradientType type() const
    {
        return static_cast<LottieFlatBuffer::GradientType>(GetField<int8_t>(VT_TYPE, 0));
    }
    LottieFlatBuffer::FillRule rule() const
    {
        return static_cast<LottieFlatBuffer::FillRule>(GetField<int8_t>(VT_RULE, 0));
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyField<uint8_t>(verifier, VT_OPACITY_TYPE, 1) &&
               VerifyOffsetRequired(verifier, VT_OPACITY) && VerifyIntegerProp(verifier, opacity(), opacity_type()) &&
               VerifyField<uint8_t>(verifier, VT_START_POS_TYPE, 1) && VerifyOffsetRequired(verifier, VT_START_POS) &&
               VerifyVectorProp(verifier, start_pos(), start_pos_type()) &&
               VerifyField<uint8_t>(verifier, VT_END_POS_TYPE, 1) && VerifyOffsetRequired(verifier, VT_END_POS) &&
               VerifyVectorProp(verifier, end_pos(), end_pos_type()) &&
               VerifyField<uint8_t>(verifier, VT_GRADIENT_TYPE, 1) && VerifyOffsetRequired(verifier, VT_GRADIENT) &&
               VerifyGradientProp(verifier, gradient(), gradient_type()) && VerifyField<int8_t>(verifier, VT_TYPE, 1) &&
               VerifyField<int8_t>(verifier, VT_RULE, 1) && verifier.EndTable();
    }
};

template <>
inline const LottieFlatBuffer::IntegerFrameArray *
GradientFillStyle::opacity_as<LottieFlatBuffer::IntegerFrameArray>() const
{
    return opacity_as_frames();
}

template <>
inline const LottieFlatBuffer::IntegerScalar *GradientFillStyle::opacity_as<LottieFlatBuffer::IntegerScalar>() const
{
    return opacity_as_value();
}

template <>
inline const LottieFlatBuffer::VectorFrameArray *
GradientFillStyle::start_pos_as<LottieFlatBuffer::VectorFrameArray>() const
{
    return start_pos_as_frames();
}

template <>
inline const LottieFlatBuffer::Point *GradientFillStyle::start_pos_as<LottieFlatBuffer::Point>() const
{
    return start_pos_as_value();
}

template <>
inline const LottieFlatBuffer::VectorFrameArray *
GradientFillStyle::end_pos_as<LottieFlatBuffer::VectorFrameArray>() const
{
    return end_pos_as_frames();
}

template <>
inline const LottieFlatBuffer::Point *GradientFillStyle::end_pos_as<LottieFlatBuffer::Point>() const
{
    return end_pos_as_value();
}

template <>
inline const LottieFlatBuffer::GradientFrameArray *
GradientFillStyle::gradient_as<LottieFlatBuffer::GradientFrameArray>() const
{
    return gradient_as_frames();
}

template <>
inline const LottieFlatBuffer::Gradient *GradientFillStyle::gradient_as<LottieFlatBuffer::Gradient>() const
{
    return gradient_as_value();
}

struct GradientFillStyleBuilder {
    typedef GradientFillStyle Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_opacity_type(LottieFlatBuffer::IntegerProp opacity_type)
    {
        fbb_.AddElement<uint8_t>(GradientFillStyle::VT_OPACITY_TYPE, static_cast<uint8_t>(opacity_type), 0);
    }
    void add_opacity(::flatbuffers::Offset<void> opacity)
    {
        fbb_.AddOffset(GradientFillStyle::VT_OPACITY, opacity);
    }
    void add_start_pos_type(LottieFlatBuffer::VectorProp start_pos_type)
    {
        fbb_.AddElement<uint8_t>(GradientFillStyle::VT_START_POS_TYPE, static_cast<uint8_t>(start_pos_type), 0);
    }
    void add_start_pos(::flatbuffers::Offset<void> start_pos)
    {
        fbb_.AddOffset(GradientFillStyle::VT_START_POS, start_pos);
    }
    void add_end_pos_type(LottieFlatBuffer::VectorProp end_pos_type)
    {
        fbb_.AddElement<uint8_t>(GradientFillStyle::VT_END_POS_TYPE, static_cast<uint8_t>(end_pos_type), 0);
    }
    void add_end_pos(::flatbuffers::Offset<void> end_pos)
    {
        fbb_.AddOffset(GradientFillStyle::VT_END_POS, end_pos);
    }
    void add_gradient_type(LottieFlatBuffer::GradientProp gradient_type)
    {
        fbb_.AddElement<uint8_t>(GradientFillStyle::VT_GRADIENT_TYPE, static_cast<uint8_t>(gradient_type), 0);
    }
    void add_gradient(::flatbuffers::Offset<void> gradient)
    {
        fbb_.AddOffset(GradientFillStyle::VT_GRADIENT, gradient);
    }
    void add_type(LottieFlatBuffer::GradientType type)
    {
        fbb_.AddElement<int8_t>(GradientFillStyle::VT_TYPE, static_cast<int8_t>(type), 0);
    }
    void add_rule(LottieFlatBuffer::FillRule rule)
    {
        fbb_.AddElement<int8_t>(GradientFillStyle::VT_RULE, static_cast<int8_t>(rule), 0);
    }
    explicit GradientFillStyleBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<GradientFillStyle> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<GradientFillStyle>(end);
        fbb_.Required(o, GradientFillStyle::VT_OPACITY);
        fbb_.Required(o, GradientFillStyle::VT_START_POS);
        fbb_.Required(o, GradientFillStyle::VT_END_POS);
        fbb_.Required(o, GradientFillStyle::VT_GRADIENT);
        return o;
    }
};

inline ::flatbuffers::Offset<GradientFillStyle> CreateGradientFillStyle(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    LottieFlatBuffer::IntegerProp opacity_type = LottieFlatBuffer::IntegerProp_NONE,
    ::flatbuffers::Offset<void> opacity = 0,
    LottieFlatBuffer::VectorProp start_pos_type = LottieFlatBuffer::VectorProp_NONE,
    ::flatbuffers::Offset<void> start_pos = 0,
    LottieFlatBuffer::VectorProp end_pos_type = LottieFlatBuffer::VectorProp_NONE,
    ::flatbuffers::Offset<void> end_pos = 0,
    LottieFlatBuffer::GradientProp gradient_type = LottieFlatBuffer::GradientProp_NONE,
    ::flatbuffers::Offset<void> gradient = 0,
    LottieFlatBuffer::GradientType type = LottieFlatBuffer::GradientType_GRADIENT_LINEAR,
    LottieFlatBuffer::FillRule rule = LottieFlatBuffer::FillRule_NON_ZERO)
{
    GradientFillStyleBuilder builder_(_fbb);
    builder_.add_gradient(gradient);
    builder_.add_end_pos(end_pos);
    builder_.add_start_pos(start_pos);
    builder_.add_opacity(opacity);
    builder_.add_rule(rule);
    builder_.add_type(type);
    builder_.add_gradient_type(gradient_type);
    builder_.add_end_pos_type(end_pos_type);
    builder_.add_start_pos_type(start_pos_type);
    builder_.add_opacity_type(opacity_type);
    return builder_.Finish();
}

struct GradientStrokeStyle FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef GradientStrokeStyleBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_WIDTH_TYPE = 4,
        VT_WIDTH = 6,
        VT_OPACITY_TYPE = 8,
        VT_OPACITY = 10,
        VT_START_POS_TYPE = 12,
        VT_START_POS = 14,
        VT_END_POS_TYPE = 16,
        VT_END_POS = 18,
        VT_GRADIENT_TYPE = 20,
        VT_GRADIENT = 22,
        VT_TYPE = 24,
        VT_CAP = 26,
        VT_JOIN = 28,
        VT_MITER_LIMIT = 30
    };
    LottieFlatBuffer::FloatProp width_type() const
    {
        return static_cast<LottieFlatBuffer::FloatProp>(GetField<uint8_t>(VT_WIDTH_TYPE, 0));
    }
    const void *width() const
    {
        return GetPointer<const void *>(VT_WIDTH);
    }
    template <typename T>
    const T *width_as() const;
    const LottieFlatBuffer::FloatFrameArray *width_as_frames() const
    {
        return width_type() == LottieFlatBuffer::FloatProp_frames ?
                   static_cast<const LottieFlatBuffer::FloatFrameArray *>(width()) :
                   nullptr;
    }
    const LottieFlatBuffer::FloatScalar *width_as_value() const
    {
        return width_type() == LottieFlatBuffer::FloatProp_value ?
                   static_cast<const LottieFlatBuffer::FloatScalar *>(width()) :
                   nullptr;
    }
    LottieFlatBuffer::IntegerProp opacity_type() const
    {
        return static_cast<LottieFlatBuffer::IntegerProp>(GetField<uint8_t>(VT_OPACITY_TYPE, 0));
    }
    const void *opacity() const
    {
        return GetPointer<const void *>(VT_OPACITY);
    }
    template <typename T>
    const T *opacity_as() const;
    const LottieFlatBuffer::IntegerFrameArray *opacity_as_frames() const
    {
        return opacity_type() == LottieFlatBuffer::IntegerProp_frames ?
                   static_cast<const LottieFlatBuffer::IntegerFrameArray *>(opacity()) :
                   nullptr;
    }
    const LottieFlatBuffer::IntegerScalar *opacity_as_value() const
    {
        return opacity_type() == LottieFlatBuffer::IntegerProp_value ?
                   static_cast<const LottieFlatBuffer::IntegerScalar *>(opacity()) :
                   nullptr;
    }
    LottieFlatBuffer::VectorProp start_pos_type() const
    {
        return static_cast<LottieFlatBuffer::VectorProp>(GetField<uint8_t>(VT_START_POS_TYPE, 0));
    }
    const void *start_pos() const
    {
        return GetPointer<const void *>(VT_START_POS);
    }
    template <typename T>
    const T *start_pos_as() const;
    const LottieFlatBuffer::VectorFrameArray *start_pos_as_frames() const
    {
        return start_pos_type() == LottieFlatBuffer::VectorProp_frames ?
                   static_cast<const LottieFlatBuffer::VectorFrameArray *>(start_pos()) :
                   nullptr;
    }
    const LottieFlatBuffer::Point *start_pos_as_value() const
    {
        return start_pos_type() == LottieFlatBuffer::VectorProp_value ?
                   static_cast<const LottieFlatBuffer::Point *>(start_pos()) :
                   nullptr;
    }
    LottieFlatBuffer::VectorProp end_pos_type() const
    {
        return static_cast<LottieFlatBuffer::VectorProp>(GetField<uint8_t>(VT_END_POS_TYPE, 0));
    }
    const void *end_pos() const
    {
        return GetPointer<const void *>(VT_END_POS);
    }
    template <typename T>
    const T *end_pos_as() const;
    const LottieFlatBuffer::VectorFrameArray *end_pos_as_frames() const
    {
        return end_pos_type() == LottieFlatBuffer::VectorProp_frames ?
                   static_cast<const LottieFlatBuffer::VectorFrameArray *>(end_pos()) :
                   nullptr;
    }
    const LottieFlatBuffer::Point *end_pos_as_value() const
    {
        return end_pos_type() == LottieFlatBuffer::VectorProp_value ?
                   static_cast<const LottieFlatBuffer::Point *>(end_pos()) :
                   nullptr;
    }
    LottieFlatBuffer::GradientProp gradient_type() const
    {
        return static_cast<LottieFlatBuffer::GradientProp>(GetField<uint8_t>(VT_GRADIENT_TYPE, 0));
    }
    const void *gradient() const
    {
        return GetPointer<const void *>(VT_GRADIENT);
    }
    template <typename T>
    const T *gradient_as() const;
    const LottieFlatBuffer::GradientFrameArray *gradient_as_frames() const
    {
        return gradient_type() == LottieFlatBuffer::GradientProp_frames ?
                   static_cast<const LottieFlatBuffer::GradientFrameArray *>(gradient()) :
                   nullptr;
    }
    const LottieFlatBuffer::Gradient *gradient_as_value() const
    {
        return gradient_type() == LottieFlatBuffer::GradientProp_value ?
                   static_cast<const LottieFlatBuffer::Gradient *>(gradient()) :
                   nullptr;
    }
    LottieFlatBuffer::GradientType type() const
    {
        return static_cast<LottieFlatBuffer::GradientType>(GetField<int8_t>(VT_TYPE, 0));
    }
    LottieFlatBuffer::LineCap cap() const
    {
        return static_cast<LottieFlatBuffer::LineCap>(GetField<int8_t>(VT_CAP, 1));
    }
    LottieFlatBuffer::LineJoin join() const
    {
        return static_cast<LottieFlatBuffer::LineJoin>(GetField<int8_t>(VT_JOIN, 1));
    }
    float miter_limit() const
    {
        return GetField<float>(VT_MITER_LIMIT, 0.0f);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyField<uint8_t>(verifier, VT_WIDTH_TYPE, 1) &&
               VerifyOffsetRequired(verifier, VT_WIDTH) && VerifyFloatProp(verifier, width(), width_type()) &&
               VerifyField<uint8_t>(verifier, VT_OPACITY_TYPE, 1) && VerifyOffsetRequired(verifier, VT_OPACITY) &&
               VerifyIntegerProp(verifier, opacity(), opacity_type()) &&
               VerifyField<uint8_t>(verifier, VT_START_POS_TYPE, 1) && VerifyOffsetRequired(verifier, VT_START_POS) &&
               VerifyVectorProp(verifier, start_pos(), start_pos_type()) &&
               VerifyField<uint8_t>(verifier, VT_END_POS_TYPE, 1) && VerifyOffsetRequired(verifier, VT_END_POS) &&
               VerifyVectorProp(verifier, end_pos(), end_pos_type()) &&
               VerifyField<uint8_t>(verifier, VT_GRADIENT_TYPE, 1) && VerifyOffsetRequired(verifier, VT_GRADIENT) &&
               VerifyGradientProp(verifier, gradient(), gradient_type()) && VerifyField<int8_t>(verifier, VT_TYPE, 1) &&
               VerifyField<int8_t>(verifier, VT_CAP, 1) && VerifyField<int8_t>(verifier, VT_JOIN, 1) &&
               VerifyField<float>(verifier, VT_MITER_LIMIT, 4) && verifier.EndTable();
    }
};

template <>
inline const LottieFlatBuffer::FloatFrameArray *GradientStrokeStyle::width_as<LottieFlatBuffer::FloatFrameArray>() const
{
    return width_as_frames();
}

template <>
inline const LottieFlatBuffer::FloatScalar *GradientStrokeStyle::width_as<LottieFlatBuffer::FloatScalar>() const
{
    return width_as_value();
}

template <>
inline const LottieFlatBuffer::IntegerFrameArray *
GradientStrokeStyle::opacity_as<LottieFlatBuffer::IntegerFrameArray>() const
{
    return opacity_as_frames();
}

template <>
inline const LottieFlatBuffer::IntegerScalar *GradientStrokeStyle::opacity_as<LottieFlatBuffer::IntegerScalar>() const
{
    return opacity_as_value();
}

template <>
inline const LottieFlatBuffer::VectorFrameArray *
GradientStrokeStyle::start_pos_as<LottieFlatBuffer::VectorFrameArray>() const
{
    return start_pos_as_frames();
}

template <>
inline const LottieFlatBuffer::Point *GradientStrokeStyle::start_pos_as<LottieFlatBuffer::Point>() const
{
    return start_pos_as_value();
}

template <>
inline const LottieFlatBuffer::VectorFrameArray *
GradientStrokeStyle::end_pos_as<LottieFlatBuffer::VectorFrameArray>() const
{
    return end_pos_as_frames();
}

template <>
inline const LottieFlatBuffer::Point *GradientStrokeStyle::end_pos_as<LottieFlatBuffer::Point>() const
{
    return end_pos_as_value();
}

template <>
inline const LottieFlatBuffer::GradientFrameArray *
GradientStrokeStyle::gradient_as<LottieFlatBuffer::GradientFrameArray>() const
{
    return gradient_as_frames();
}

template <>
inline const LottieFlatBuffer::Gradient *GradientStrokeStyle::gradient_as<LottieFlatBuffer::Gradient>() const
{
    return gradient_as_value();
}

struct GradientStrokeStyleBuilder {
    typedef GradientStrokeStyle Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_width_type(LottieFlatBuffer::FloatProp width_type)
    {
        fbb_.AddElement<uint8_t>(GradientStrokeStyle::VT_WIDTH_TYPE, static_cast<uint8_t>(width_type), 0);
    }
    void add_width(::flatbuffers::Offset<void> width)
    {
        fbb_.AddOffset(GradientStrokeStyle::VT_WIDTH, width);
    }
    void add_opacity_type(LottieFlatBuffer::IntegerProp opacity_type)
    {
        fbb_.AddElement<uint8_t>(GradientStrokeStyle::VT_OPACITY_TYPE, static_cast<uint8_t>(opacity_type), 0);
    }
    void add_opacity(::flatbuffers::Offset<void> opacity)
    {
        fbb_.AddOffset(GradientStrokeStyle::VT_OPACITY, opacity);
    }
    void add_start_pos_type(LottieFlatBuffer::VectorProp start_pos_type)
    {
        fbb_.AddElement<uint8_t>(GradientStrokeStyle::VT_START_POS_TYPE, static_cast<uint8_t>(start_pos_type), 0);
    }
    void add_start_pos(::flatbuffers::Offset<void> start_pos)
    {
        fbb_.AddOffset(GradientStrokeStyle::VT_START_POS, start_pos);
    }
    void add_end_pos_type(LottieFlatBuffer::VectorProp end_pos_type)
    {
        fbb_.AddElement<uint8_t>(GradientStrokeStyle::VT_END_POS_TYPE, static_cast<uint8_t>(end_pos_type), 0);
    }
    void add_end_pos(::flatbuffers::Offset<void> end_pos)
    {
        fbb_.AddOffset(GradientStrokeStyle::VT_END_POS, end_pos);
    }
    void add_gradient_type(LottieFlatBuffer::GradientProp gradient_type)
    {
        fbb_.AddElement<uint8_t>(GradientStrokeStyle::VT_GRADIENT_TYPE, static_cast<uint8_t>(gradient_type), 0);
    }
    void add_gradient(::flatbuffers::Offset<void> gradient)
    {
        fbb_.AddOffset(GradientStrokeStyle::VT_GRADIENT, gradient);
    }
    void add_type(LottieFlatBuffer::GradientType type)
    {
        fbb_.AddElement<int8_t>(GradientStrokeStyle::VT_TYPE, static_cast<int8_t>(type), 0);
    }
    void add_cap(LottieFlatBuffer::LineCap cap)
    {
        fbb_.AddElement<int8_t>(GradientStrokeStyle::VT_CAP, static_cast<int8_t>(cap), 1);
    }
    void add_join(LottieFlatBuffer::LineJoin join)
    {
        fbb_.AddElement<int8_t>(GradientStrokeStyle::VT_JOIN, static_cast<int8_t>(join), 1);
    }
    void add_miter_limit(float miter_limit)
    {
        fbb_.AddElement<float>(GradientStrokeStyle::VT_MITER_LIMIT, miter_limit, 0.0f);
    }
    explicit GradientStrokeStyleBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<GradientStrokeStyle> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<GradientStrokeStyle>(end);
        fbb_.Required(o, GradientStrokeStyle::VT_WIDTH);
        fbb_.Required(o, GradientStrokeStyle::VT_OPACITY);
        fbb_.Required(o, GradientStrokeStyle::VT_START_POS);
        fbb_.Required(o, GradientStrokeStyle::VT_END_POS);
        fbb_.Required(o, GradientStrokeStyle::VT_GRADIENT);
        return o;
    }
};

inline ::flatbuffers::Offset<GradientStrokeStyle> CreateGradientStrokeStyle(
    ::flatbuffers::FlatBufferBuilder &_fbb, LottieFlatBuffer::FloatProp width_type = LottieFlatBuffer::FloatProp_NONE,
    ::flatbuffers::Offset<void> width = 0,
    LottieFlatBuffer::IntegerProp opacity_type = LottieFlatBuffer::IntegerProp_NONE,
    ::flatbuffers::Offset<void> opacity = 0,
    LottieFlatBuffer::VectorProp start_pos_type = LottieFlatBuffer::VectorProp_NONE,
    ::flatbuffers::Offset<void> start_pos = 0,
    LottieFlatBuffer::VectorProp end_pos_type = LottieFlatBuffer::VectorProp_NONE,
    ::flatbuffers::Offset<void> end_pos = 0,
    LottieFlatBuffer::GradientProp gradient_type = LottieFlatBuffer::GradientProp_NONE,
    ::flatbuffers::Offset<void> gradient = 0,
    LottieFlatBuffer::GradientType type = LottieFlatBuffer::GradientType_GRADIENT_LINEAR,
    LottieFlatBuffer::LineCap cap = LottieFlatBuffer::LineCap_CAP_ROUND,
    LottieFlatBuffer::LineJoin join = LottieFlatBuffer::LineJoin_JOIN_ROUND, float miter_limit = 0.0f)
{
    GradientStrokeStyleBuilder builder_(_fbb);
    builder_.add_miter_limit(miter_limit);
    builder_.add_gradient(gradient);
    builder_.add_end_pos(end_pos);
    builder_.add_start_pos(start_pos);
    builder_.add_opacity(opacity);
    builder_.add_width(width);
    builder_.add_join(join);
    builder_.add_cap(cap);
    builder_.add_type(type);
    builder_.add_gradient_type(gradient_type);
    builder_.add_end_pos_type(end_pos_type);
    builder_.add_start_pos_type(start_pos_type);
    builder_.add_opacity_type(opacity_type);
    builder_.add_width_type(width_type);
    return builder_.Finish();
}

struct TrimPath FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef TrimPathBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_START_TYPE = 4,
        VT_START = 6,
        VT_END_TYPE = 8,
        VT_END = 10,
        VT_OFFSET_TYPE = 12,
        VT_OFFSET = 14,
        VT_TYPE = 16
    };
    LottieFlatBuffer::FloatProp start_type() const
    {
        return static_cast<LottieFlatBuffer::FloatProp>(GetField<uint8_t>(VT_START_TYPE, 0));
    }
    const void *start() const
    {
        return GetPointer<const void *>(VT_START);
    }
    template <typename T>
    const T *start_as() const;
    const LottieFlatBuffer::FloatFrameArray *start_as_frames() const
    {
        return start_type() == LottieFlatBuffer::FloatProp_frames ?
                   static_cast<const LottieFlatBuffer::FloatFrameArray *>(start()) :
                   nullptr;
    }
    const LottieFlatBuffer::FloatScalar *start_as_value() const
    {
        return start_type() == LottieFlatBuffer::FloatProp_value ?
                   static_cast<const LottieFlatBuffer::FloatScalar *>(start()) :
                   nullptr;
    }
    LottieFlatBuffer::FloatProp end_type() const
    {
        return static_cast<LottieFlatBuffer::FloatProp>(GetField<uint8_t>(VT_END_TYPE, 0));
    }
    const void *end() const
    {
        return GetPointer<const void *>(VT_END);
    }
    template <typename T>
    const T *end_as() const;
    const LottieFlatBuffer::FloatFrameArray *end_as_frames() const
    {
        return end_type() == LottieFlatBuffer::FloatProp_frames ?
                   static_cast<const LottieFlatBuffer::FloatFrameArray *>(end()) :
                   nullptr;
    }
    const LottieFlatBuffer::FloatScalar *end_as_value() const
    {
        return end_type() == LottieFlatBuffer::FloatProp_value ?
                   static_cast<const LottieFlatBuffer::FloatScalar *>(end()) :
                   nullptr;
    }
    LottieFlatBuffer::FloatProp offset_type() const
    {
        return static_cast<LottieFlatBuffer::FloatProp>(GetField<uint8_t>(VT_OFFSET_TYPE, 0));
    }
    const void *offset() const
    {
        return GetPointer<const void *>(VT_OFFSET);
    }
    template <typename T>
    const T *offset_as() const;
    const LottieFlatBuffer::FloatFrameArray *offset_as_frames() const
    {
        return offset_type() == LottieFlatBuffer::FloatProp_frames ?
                   static_cast<const LottieFlatBuffer::FloatFrameArray *>(offset()) :
                   nullptr;
    }
    const LottieFlatBuffer::FloatScalar *offset_as_value() const
    {
        return offset_type() == LottieFlatBuffer::FloatProp_value ?
                   static_cast<const LottieFlatBuffer::FloatScalar *>(offset()) :
                   nullptr;
    }
    LottieFlatBuffer::TrimType type() const
    {
        return static_cast<LottieFlatBuffer::TrimType>(GetField<int8_t>(VT_TYPE, 0));
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyField<uint8_t>(verifier, VT_START_TYPE, 1) &&
               VerifyOffsetRequired(verifier, VT_START) && VerifyFloatProp(verifier, start(), start_type()) &&
               VerifyField<uint8_t>(verifier, VT_END_TYPE, 1) && VerifyOffsetRequired(verifier, VT_END) &&
               VerifyFloatProp(verifier, end(), end_type()) && VerifyField<uint8_t>(verifier, VT_OFFSET_TYPE, 1) &&
               VerifyOffsetRequired(verifier, VT_OFFSET) && VerifyFloatProp(verifier, offset(), offset_type()) &&
               VerifyField<int8_t>(verifier, VT_TYPE, 1) && verifier.EndTable();
    }
};

template <>
inline const LottieFlatBuffer::FloatFrameArray *TrimPath::start_as<LottieFlatBuffer::FloatFrameArray>() const
{
    return start_as_frames();
}

template <>
inline const LottieFlatBuffer::FloatScalar *TrimPath::start_as<LottieFlatBuffer::FloatScalar>() const
{
    return start_as_value();
}

template <>
inline const LottieFlatBuffer::FloatFrameArray *TrimPath::end_as<LottieFlatBuffer::FloatFrameArray>() const
{
    return end_as_frames();
}

template <>
inline const LottieFlatBuffer::FloatScalar *TrimPath::end_as<LottieFlatBuffer::FloatScalar>() const
{
    return end_as_value();
}

template <>
inline const LottieFlatBuffer::FloatFrameArray *TrimPath::offset_as<LottieFlatBuffer::FloatFrameArray>() const
{
    return offset_as_frames();
}

template <>
inline const LottieFlatBuffer::FloatScalar *TrimPath::offset_as<LottieFlatBuffer::FloatScalar>() const
{
    return offset_as_value();
}

struct TrimPathBuilder {
    typedef TrimPath Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_start_type(LottieFlatBuffer::FloatProp start_type)
    {
        fbb_.AddElement<uint8_t>(TrimPath::VT_START_TYPE, static_cast<uint8_t>(start_type), 0);
    }
    void add_start(::flatbuffers::Offset<void> start)
    {
        fbb_.AddOffset(TrimPath::VT_START, start);
    }
    void add_end_type(LottieFlatBuffer::FloatProp end_type)
    {
        fbb_.AddElement<uint8_t>(TrimPath::VT_END_TYPE, static_cast<uint8_t>(end_type), 0);
    }
    void add_end(::flatbuffers::Offset<void> end)
    {
        fbb_.AddOffset(TrimPath::VT_END, end);
    }
    void add_offset_type(LottieFlatBuffer::FloatProp offset_type)
    {
        fbb_.AddElement<uint8_t>(TrimPath::VT_OFFSET_TYPE, static_cast<uint8_t>(offset_type), 0);
    }
    void add_offset(::flatbuffers::Offset<void> offset)
    {
        fbb_.AddOffset(TrimPath::VT_OFFSET, offset);
    }
    void add_type(LottieFlatBuffer::TrimType type)
    {
        fbb_.AddElement<int8_t>(TrimPath::VT_TYPE, static_cast<int8_t>(type), 0);
    }
    explicit TrimPathBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<TrimPath> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<TrimPath>(end);
        fbb_.Required(o, TrimPath::VT_START);
        fbb_.Required(o, TrimPath::VT_END);
        fbb_.Required(o, TrimPath::VT_OFFSET);
        return o;
    }
};

inline ::flatbuffers::Offset<TrimPath> CreateTrimPath(
    ::flatbuffers::FlatBufferBuilder &_fbb, LottieFlatBuffer::FloatProp start_type = LottieFlatBuffer::FloatProp_NONE,
    ::flatbuffers::Offset<void> start = 0, LottieFlatBuffer::FloatProp end_type = LottieFlatBuffer::FloatProp_NONE,
    ::flatbuffers::Offset<void> end = 0, LottieFlatBuffer::FloatProp offset_type = LottieFlatBuffer::FloatProp_NONE,
    ::flatbuffers::Offset<void> offset = 0, LottieFlatBuffer::TrimType type = LottieFlatBuffer::TrimType_Simultaneous)
{
    TrimPathBuilder builder_(_fbb);
    builder_.add_offset(offset);
    builder_.add_end(end);
    builder_.add_start(start);
    builder_.add_type(type);
    builder_.add_offset_type(offset_type);
    builder_.add_end_type(end_type);
    builder_.add_start_type(start_type);
    return builder_.Finish();
}

struct TransformShape FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef TransformShapeBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE { VT_TRANSFORM = 4 };
    const LottieFlatBuffer::Transform *transform() const
    {
        return GetPointer<const LottieFlatBuffer::Transform *>(VT_TRANSFORM);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyOffset(verifier, VT_TRANSFORM) &&
               verifier.VerifyTable(transform()) && verifier.EndTable();
    }
};

struct TransformShapeBuilder {
    typedef TransformShape Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_transform(::flatbuffers::Offset<LottieFlatBuffer::Transform> transform)
    {
        fbb_.AddOffset(TransformShape::VT_TRANSFORM, transform);
    }
    explicit TransformShapeBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<TransformShape> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<TransformShape>(end);
        return o;
    }
};

inline ::flatbuffers::Offset<TransformShape> CreateTransformShape(
    ::flatbuffers::FlatBufferBuilder &_fbb, ::flatbuffers::Offset<LottieFlatBuffer::Transform> transform = 0)
{
    TransformShapeBuilder builder_(_fbb);
    builder_.add_transform(transform);
    return builder_.Finish();
}

struct GroupShape FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef GroupShapeBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE { VT_SHAPES_TYPE = 4, VT_SHAPES = 6 };
    const ::flatbuffers::Vector<uint8_t> *shapes_type() const
    {
        return GetPointer<const ::flatbuffers::Vector<uint8_t> *>(VT_SHAPES_TYPE);
    }
    const ::flatbuffers::Vector<::flatbuffers::Offset<void>> *shapes() const
    {
        return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<void>> *>(VT_SHAPES);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyOffset(verifier, VT_SHAPES_TYPE) &&
               verifier.VerifyVector(shapes_type()) && VerifyOffset(verifier, VT_SHAPES) &&
               verifier.VerifyVector(shapes()) && VerifyGraphicElementVector(verifier, shapes(), shapes_type()) &&
               verifier.EndTable();
    }
};

struct GroupShapeBuilder {
    typedef GroupShape Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_shapes_type(::flatbuffers::Offset<::flatbuffers::Vector<uint8_t>> shapes_type)
    {
        fbb_.AddOffset(GroupShape::VT_SHAPES_TYPE, shapes_type);
    }
    void add_shapes(::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<void>>> shapes)
    {
        fbb_.AddOffset(GroupShape::VT_SHAPES, shapes);
    }
    explicit GroupShapeBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<GroupShape> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<GroupShape>(end);
        return o;
    }
};

inline ::flatbuffers::Offset<GroupShape> CreateGroupShape(
    ::flatbuffers::FlatBufferBuilder &_fbb, ::flatbuffers::Offset<::flatbuffers::Vector<uint8_t>> shapes_type = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<void>>> shapes = 0)
{
    GroupShapeBuilder builder_(_fbb);
    builder_.add_shapes(shapes);
    builder_.add_shapes_type(shapes_type);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<GroupShape> CreateGroupShapeDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb, const std::vector<uint8_t> *shapes_type = nullptr,
    const std::vector<::flatbuffers::Offset<void>> *shapes = nullptr)
{
    auto shapes_type__ = shapes_type ? _fbb.CreateVector<uint8_t>(*shapes_type) : 0;
    auto shapes__ = shapes ? _fbb.CreateVector<::flatbuffers::Offset<void>>(*shapes) : 0;
    return LottieFlatBuffer::CreateGroupShape(_fbb, shapes_type__, shapes__);
}

struct BaseLayer FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef BaseLayerBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_IN_FRAME = 4,
        VT_OUT_FRAME = 6,
        VT_START_FRAME = 8,
        VT_INDEX = 10,
        VT_PARENT = 12,
        VT_NAME = 14,
        VT_TRANSFORM = 16,
        VT_BLEND_MODE = 18
    };
    float in_frame() const
    {
        return GetField<float>(VT_IN_FRAME, 0.0f);
    }
    float out_frame() const
    {
        return GetField<float>(VT_OUT_FRAME, 0.0f);
    }
    float start_frame() const
    {
        return GetField<float>(VT_START_FRAME, 0.0f);
    }
    int32_t index() const
    {
        return GetField<int32_t>(VT_INDEX, -1);
    }
    int32_t parent() const
    {
        return GetField<int32_t>(VT_PARENT, -1);
    }
    const ::flatbuffers::String *name() const
    {
        return GetPointer<const ::flatbuffers::String *>(VT_NAME);
    }
    const LottieFlatBuffer::Transform *transform() const
    {
        return GetPointer<const LottieFlatBuffer::Transform *>(VT_TRANSFORM);
    }
    LottieFlatBuffer::BlendMode blend_mode() const
    {
        return static_cast<LottieFlatBuffer::BlendMode>(GetField<int8_t>(VT_BLEND_MODE, 0));
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyField<float>(verifier, VT_IN_FRAME, 4) &&
               VerifyField<float>(verifier, VT_OUT_FRAME, 4) && VerifyField<float>(verifier, VT_START_FRAME, 4) &&
               VerifyField<int32_t>(verifier, VT_INDEX, 4) && VerifyField<int32_t>(verifier, VT_PARENT, 4) &&
               VerifyOffset(verifier, VT_NAME) && verifier.VerifyString(name()) &&
               VerifyOffset(verifier, VT_TRANSFORM) && verifier.VerifyTable(transform()) &&
               VerifyField<int8_t>(verifier, VT_BLEND_MODE, 1) && verifier.EndTable();
    }
};

struct BaseLayerBuilder {
    typedef BaseLayer Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_in_frame(float in_frame)
    {
        fbb_.AddElement<float>(BaseLayer::VT_IN_FRAME, in_frame, 0.0f);
    }
    void add_out_frame(float out_frame)
    {
        fbb_.AddElement<float>(BaseLayer::VT_OUT_FRAME, out_frame, 0.0f);
    }
    void add_start_frame(float start_frame)
    {
        fbb_.AddElement<float>(BaseLayer::VT_START_FRAME, start_frame, 0.0f);
    }
    void add_index(int32_t index)
    {
        fbb_.AddElement<int32_t>(BaseLayer::VT_INDEX, index, -1);
    }
    void add_parent(int32_t parent)
    {
        fbb_.AddElement<int32_t>(BaseLayer::VT_PARENT, parent, -1);
    }
    void add_name(::flatbuffers::Offset<::flatbuffers::String> name)
    {
        fbb_.AddOffset(BaseLayer::VT_NAME, name);
    }
    void add_transform(::flatbuffers::Offset<LottieFlatBuffer::Transform> transform)
    {
        fbb_.AddOffset(BaseLayer::VT_TRANSFORM, transform);
    }
    void add_blend_mode(LottieFlatBuffer::BlendMode blend_mode)
    {
        fbb_.AddElement<int8_t>(BaseLayer::VT_BLEND_MODE, static_cast<int8_t>(blend_mode), 0);
    }
    explicit BaseLayerBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<BaseLayer> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<BaseLayer>(end);
        return o;
    }
};

inline ::flatbuffers::Offset<BaseLayer> CreateBaseLayer(
    ::flatbuffers::FlatBufferBuilder &_fbb, float in_frame = 0.0f, float out_frame = 0.0f, float start_frame = 0.0f,
    int32_t index = -1, int32_t parent = -1, ::flatbuffers::Offset<::flatbuffers::String> name = 0,
    ::flatbuffers::Offset<LottieFlatBuffer::Transform> transform = 0,
    LottieFlatBuffer::BlendMode blend_mode = LottieFlatBuffer::BlendMode_Normal)
{
    BaseLayerBuilder builder_(_fbb);
    builder_.add_transform(transform);
    builder_.add_name(name);
    builder_.add_parent(parent);
    builder_.add_index(index);
    builder_.add_start_frame(start_frame);
    builder_.add_out_frame(out_frame);
    builder_.add_in_frame(in_frame);
    builder_.add_blend_mode(blend_mode);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<BaseLayer> CreateBaseLayerDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb, float in_frame = 0.0f, float out_frame = 0.0f, float start_frame = 0.0f,
    int32_t index = -1, int32_t parent = -1, const char *name = nullptr,
    ::flatbuffers::Offset<LottieFlatBuffer::Transform> transform = 0,
    LottieFlatBuffer::BlendMode blend_mode = LottieFlatBuffer::BlendMode_Normal)
{
    auto name__ = name ? _fbb.CreateString(name) : 0;
    return LottieFlatBuffer::CreateBaseLayer(_fbb, in_frame, out_frame, start_frame, index, parent, name__, transform,
                                             blend_mode);
}

struct ShapeLayer FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef ShapeLayerBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE { VT_BASE = 4, VT_SHAPES_TYPE = 6, VT_SHAPES = 8 };
    const LottieFlatBuffer::BaseLayer *base() const
    {
        return GetPointer<const LottieFlatBuffer::BaseLayer *>(VT_BASE);
    }
    const ::flatbuffers::Vector<uint8_t> *shapes_type() const
    {
        return GetPointer<const ::flatbuffers::Vector<uint8_t> *>(VT_SHAPES_TYPE);
    }
    const ::flatbuffers::Vector<::flatbuffers::Offset<void>> *shapes() const
    {
        return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<void>> *>(VT_SHAPES);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyOffset(verifier, VT_BASE) && verifier.VerifyTable(base()) &&
               VerifyOffsetRequired(verifier, VT_SHAPES_TYPE) && verifier.VerifyVector(shapes_type()) &&
               VerifyOffsetRequired(verifier, VT_SHAPES) && verifier.VerifyVector(shapes()) &&
               VerifyGraphicElementVector(verifier, shapes(), shapes_type()) && verifier.EndTable();
    }
};

struct ShapeLayerBuilder {
    typedef ShapeLayer Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_base(::flatbuffers::Offset<LottieFlatBuffer::BaseLayer> base)
    {
        fbb_.AddOffset(ShapeLayer::VT_BASE, base);
    }
    void add_shapes_type(::flatbuffers::Offset<::flatbuffers::Vector<uint8_t>> shapes_type)
    {
        fbb_.AddOffset(ShapeLayer::VT_SHAPES_TYPE, shapes_type);
    }
    void add_shapes(::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<void>>> shapes)
    {
        fbb_.AddOffset(ShapeLayer::VT_SHAPES, shapes);
    }
    explicit ShapeLayerBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<ShapeLayer> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<ShapeLayer>(end);
        fbb_.Required(o, ShapeLayer::VT_SHAPES_TYPE);
        fbb_.Required(o, ShapeLayer::VT_SHAPES);
        return o;
    }
};

inline ::flatbuffers::Offset<ShapeLayer> CreateShapeLayer(
    ::flatbuffers::FlatBufferBuilder &_fbb, ::flatbuffers::Offset<LottieFlatBuffer::BaseLayer> base = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<uint8_t>> shapes_type = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<void>>> shapes = 0)
{
    ShapeLayerBuilder builder_(_fbb);
    builder_.add_shapes(shapes);
    builder_.add_shapes_type(shapes_type);
    builder_.add_base(base);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<ShapeLayer> CreateShapeLayerDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb, ::flatbuffers::Offset<LottieFlatBuffer::BaseLayer> base = 0,
    const std::vector<uint8_t> *shapes_type = nullptr, const std::vector<::flatbuffers::Offset<void>> *shapes = nullptr)
{
    auto shapes_type__ = shapes_type ? _fbb.CreateVector<uint8_t>(*shapes_type) : 0;
    auto shapes__ = shapes ? _fbb.CreateVector<::flatbuffers::Offset<void>>(*shapes) : 0;
    return LottieFlatBuffer::CreateShapeLayer(_fbb, base, shapes_type__, shapes__);
}

struct PrecompositionLayer FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef PrecompositionLayerBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_BASE = 4,
        VT_REF_ID = 6,
        VT_WIDTH = 8,
        VT_HEIGHT = 10
    };
    const LottieFlatBuffer::BaseLayer *base() const
    {
        return GetPointer<const LottieFlatBuffer::BaseLayer *>(VT_BASE);
    }
    const ::flatbuffers::String *ref_id() const
    {
        return GetPointer<const ::flatbuffers::String *>(VT_REF_ID);
    }
    uint16_t width() const
    {
        return GetField<uint16_t>(VT_WIDTH, 0);
    }
    uint16_t height() const
    {
        return GetField<uint16_t>(VT_HEIGHT, 0);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyOffset(verifier, VT_BASE) && verifier.VerifyTable(base()) &&
               VerifyOffset(verifier, VT_REF_ID) && verifier.VerifyString(ref_id()) &&
               VerifyField<uint16_t>(verifier, VT_WIDTH, 2) && VerifyField<uint16_t>(verifier, VT_HEIGHT, 2) &&
               verifier.EndTable();
    }
};

struct PrecompositionLayerBuilder {
    typedef PrecompositionLayer Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_base(::flatbuffers::Offset<LottieFlatBuffer::BaseLayer> base)
    {
        fbb_.AddOffset(PrecompositionLayer::VT_BASE, base);
    }
    void add_ref_id(::flatbuffers::Offset<::flatbuffers::String> ref_id)
    {
        fbb_.AddOffset(PrecompositionLayer::VT_REF_ID, ref_id);
    }
    void add_width(uint16_t width)
    {
        fbb_.AddElement<uint16_t>(PrecompositionLayer::VT_WIDTH, width, 0);
    }
    void add_height(uint16_t height)
    {
        fbb_.AddElement<uint16_t>(PrecompositionLayer::VT_HEIGHT, height, 0);
    }
    explicit PrecompositionLayerBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<PrecompositionLayer> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<PrecompositionLayer>(end);
        return o;
    }
};

inline ::flatbuffers::Offset<PrecompositionLayer> CreatePrecompositionLayer(
    ::flatbuffers::FlatBufferBuilder &_fbb, ::flatbuffers::Offset<LottieFlatBuffer::BaseLayer> base = 0,
    ::flatbuffers::Offset<::flatbuffers::String> ref_id = 0, uint16_t width = 0, uint16_t height = 0)
{
    PrecompositionLayerBuilder builder_(_fbb);
    builder_.add_ref_id(ref_id);
    builder_.add_base(base);
    builder_.add_height(height);
    builder_.add_width(width);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<PrecompositionLayer> CreatePrecompositionLayerDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb, ::flatbuffers::Offset<LottieFlatBuffer::BaseLayer> base = 0,
    const char *ref_id = nullptr, uint16_t width = 0, uint16_t height = 0)
{
    auto ref_id__ = ref_id ? _fbb.CreateString(ref_id) : 0;
    return LottieFlatBuffer::CreatePrecompositionLayer(_fbb, base, ref_id__, width, height);
}

struct NullLayer FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef NullLayerBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE { VT_BASE = 4 };
    const LottieFlatBuffer::BaseLayer *base() const
    {
        return GetPointer<const LottieFlatBuffer::BaseLayer *>(VT_BASE);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyOffset(verifier, VT_BASE) && verifier.VerifyTable(base()) &&
               verifier.EndTable();
    }
};

struct NullLayerBuilder {
    typedef NullLayer Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_base(::flatbuffers::Offset<LottieFlatBuffer::BaseLayer> base)
    {
        fbb_.AddOffset(NullLayer::VT_BASE, base);
    }
    explicit NullLayerBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<NullLayer> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<NullLayer>(end);
        return o;
    }
};

inline ::flatbuffers::Offset<NullLayer> CreateNullLayer(::flatbuffers::FlatBufferBuilder &_fbb,
                                                        ::flatbuffers::Offset<LottieFlatBuffer::BaseLayer> base = 0)
{
    NullLayerBuilder builder_(_fbb);
    builder_.add_base(base);
    return builder_.Finish();
}

struct TextDocument FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef TextDocumentBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_TEXT = 4,
        VT_COLOR = 6,
        VT_SIZE = 8,
        VT_HEIGHT = 10,
        VT_B_BOX = 12,
        VT_ALIGN = 14
    };
    const ::flatbuffers::String *text() const
    {
        return GetPointer<const ::flatbuffers::String *>(VT_TEXT);
    }
    const LottieFlatBuffer::Color24 *color() const
    {
        return GetStruct<const LottieFlatBuffer::Color24 *>(VT_COLOR);
    }
    uint16_t size() const
    {
        return GetField<uint16_t>(VT_SIZE, 0);
    }
    uint16_t height() const
    {
        return GetField<uint16_t>(VT_HEIGHT, 0);
    }
    const LottieFlatBuffer::Rect *b_box() const
    {
        return GetStruct<const LottieFlatBuffer::Rect *>(VT_B_BOX);
    }
    LottieFlatBuffer::TextAlign align() const
    {
        return static_cast<LottieFlatBuffer::TextAlign>(GetField<int8_t>(VT_ALIGN, 1));
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyOffset(verifier, VT_TEXT) && verifier.VerifyString(text()) &&
               VerifyField<LottieFlatBuffer::Color24>(verifier, VT_COLOR, 1) &&
               VerifyField<uint16_t>(verifier, VT_SIZE, 2) && VerifyField<uint16_t>(verifier, VT_HEIGHT, 2) &&
               VerifyField<LottieFlatBuffer::Rect>(verifier, VT_B_BOX, 4) &&
               VerifyField<int8_t>(verifier, VT_ALIGN, 1) && verifier.EndTable();
    }
};

struct TextDocumentBuilder {
    typedef TextDocument Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_text(::flatbuffers::Offset<::flatbuffers::String> text)
    {
        fbb_.AddOffset(TextDocument::VT_TEXT, text);
    }
    void add_color(const LottieFlatBuffer::Color24 *color)
    {
        fbb_.AddStruct(TextDocument::VT_COLOR, color);
    }
    void add_size(uint16_t size)
    {
        fbb_.AddElement<uint16_t>(TextDocument::VT_SIZE, size, 0);
    }
    void add_height(uint16_t height)
    {
        fbb_.AddElement<uint16_t>(TextDocument::VT_HEIGHT, height, 0);
    }
    void add_b_box(const LottieFlatBuffer::Rect *b_box)
    {
        fbb_.AddStruct(TextDocument::VT_B_BOX, b_box);
    }
    void add_align(LottieFlatBuffer::TextAlign align)
    {
        fbb_.AddElement<int8_t>(TextDocument::VT_ALIGN, static_cast<int8_t>(align), 1);
    }
    explicit TextDocumentBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<TextDocument> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<TextDocument>(end);
        return o;
    }
};

inline ::flatbuffers::Offset<TextDocument> CreateTextDocument(
    ::flatbuffers::FlatBufferBuilder &_fbb, ::flatbuffers::Offset<::flatbuffers::String> text = 0,
    const LottieFlatBuffer::Color24 *color = nullptr, uint16_t size = 0, uint16_t height = 0,
    const LottieFlatBuffer::Rect *b_box = nullptr,
    LottieFlatBuffer::TextAlign align = LottieFlatBuffer::TextAlign_AlignLeft)
{
    TextDocumentBuilder builder_(_fbb);
    builder_.add_b_box(b_box);
    builder_.add_color(color);
    builder_.add_text(text);
    builder_.add_height(height);
    builder_.add_size(size);
    builder_.add_align(align);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<TextDocument> CreateTextDocumentDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb, const char *text = nullptr,
    const LottieFlatBuffer::Color24 *color = nullptr, uint16_t size = 0, uint16_t height = 0,
    const LottieFlatBuffer::Rect *b_box = nullptr,
    LottieFlatBuffer::TextAlign align = LottieFlatBuffer::TextAlign_AlignLeft)
{
    auto text__ = text ? _fbb.CreateString(text) : 0;
    return LottieFlatBuffer::CreateTextDocument(_fbb, text__, color, size, height, b_box, align);
}

struct TextDocumentFrame FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef TextDocumentFrameBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_TIME = 4,
        VT_START = 6,
        VT_HOLD = 8,
        VT_TIME_EASING = 10,
        VT_VALUE_EASING = 12
    };
    float time() const
    {
        return GetField<float>(VT_TIME, 0.0f);
    }
    const LottieFlatBuffer::TextDocument *start() const
    {
        return GetPointer<const LottieFlatBuffer::TextDocument *>(VT_START);
    }
    bool hold() const
    {
        return GetField<uint8_t>(VT_HOLD, 0) != 0;
    }
    const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *time_easing() const
    {
        return GetPointer<const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *>(VT_TIME_EASING);
    }
    const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *value_easing() const
    {
        return GetPointer<const ::flatbuffers::Vector<const LottieFlatBuffer::Point *> *>(VT_VALUE_EASING);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyField<float>(verifier, VT_TIME, 4) &&
               VerifyOffsetRequired(verifier, VT_START) && verifier.VerifyTable(start()) &&
               VerifyField<uint8_t>(verifier, VT_HOLD, 1) && VerifyOffset(verifier, VT_TIME_EASING) &&
               verifier.VerifyVector(time_easing()) && VerifyOffset(verifier, VT_VALUE_EASING) &&
               verifier.VerifyVector(value_easing()) && verifier.EndTable();
    }
};

struct TextDocumentFrameBuilder {
    typedef TextDocumentFrame Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_time(float time)
    {
        fbb_.AddElement<float>(TextDocumentFrame::VT_TIME, time, 0.0f);
    }
    void add_start(::flatbuffers::Offset<LottieFlatBuffer::TextDocument> start)
    {
        fbb_.AddOffset(TextDocumentFrame::VT_START, start);
    }
    void add_hold(bool hold)
    {
        fbb_.AddElement<uint8_t>(TextDocumentFrame::VT_HOLD, static_cast<uint8_t>(hold), 0);
    }
    void add_time_easing(::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> time_easing)
    {
        fbb_.AddOffset(TextDocumentFrame::VT_TIME_EASING, time_easing);
    }
    void add_value_easing(::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> value_easing)
    {
        fbb_.AddOffset(TextDocumentFrame::VT_VALUE_EASING, value_easing);
    }
    explicit TextDocumentFrameBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<TextDocumentFrame> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<TextDocumentFrame>(end);
        fbb_.Required(o, TextDocumentFrame::VT_START);
        return o;
    }
};

inline ::flatbuffers::Offset<TextDocumentFrame> CreateTextDocumentFrame(
    ::flatbuffers::FlatBufferBuilder &_fbb, float time = 0.0f,
    ::flatbuffers::Offset<LottieFlatBuffer::TextDocument> start = 0, bool hold = false,
    ::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> time_easing = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<const LottieFlatBuffer::Point *>> value_easing = 0)
{
    TextDocumentFrameBuilder builder_(_fbb);
    builder_.add_value_easing(value_easing);
    builder_.add_time_easing(time_easing);
    builder_.add_start(start);
    builder_.add_time(time);
    builder_.add_hold(hold);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<TextDocumentFrame> CreateTextDocumentFrameDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb, float time = 0.0f,
    ::flatbuffers::Offset<LottieFlatBuffer::TextDocument> start = 0, bool hold = false,
    const std::vector<LottieFlatBuffer::Point> *time_easing = nullptr,
    const std::vector<LottieFlatBuffer::Point> *value_easing = nullptr)
{
    auto time_easing__ = time_easing ? _fbb.CreateVectorOfStructs<LottieFlatBuffer::Point>(*time_easing) : 0;
    auto value_easing__ = value_easing ? _fbb.CreateVectorOfStructs<LottieFlatBuffer::Point>(*value_easing) : 0;
    return LottieFlatBuffer::CreateTextDocumentFrame(_fbb, time, start, hold, time_easing__, value_easing__);
}

struct TextData FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef TextDataBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE { VT_FRAMES = 4 };
    const ::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::TextDocumentFrame>> *frames() const
    {
        return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::TextDocumentFrame>> *>(
            VT_FRAMES);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyOffsetRequired(verifier, VT_FRAMES) &&
               verifier.VerifyVector(frames()) && verifier.VerifyVectorOfTables(frames()) && verifier.EndTable();
    }
};

struct TextDataBuilder {
    typedef TextData Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_frames(
        ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::TextDocumentFrame>>> frames)
    {
        fbb_.AddOffset(TextData::VT_FRAMES, frames);
    }
    explicit TextDataBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<TextData> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<TextData>(end);
        fbb_.Required(o, TextData::VT_FRAMES);
        return o;
    }
};

inline ::flatbuffers::Offset<TextData> CreateTextData(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::TextDocumentFrame>>> frames = 0)
{
    TextDataBuilder builder_(_fbb);
    builder_.add_frames(frames);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<TextData> CreateTextDataDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<::flatbuffers::Offset<LottieFlatBuffer::TextDocumentFrame>> *frames = nullptr)
{
    auto frames__ = frames ? _fbb.CreateVector<::flatbuffers::Offset<LottieFlatBuffer::TextDocumentFrame>>(*frames) : 0;
    return LottieFlatBuffer::CreateTextData(_fbb, frames__);
}

struct TextLayer FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef TextLayerBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE { VT_BASE = 4, VT_DATA = 6 };
    const LottieFlatBuffer::BaseLayer *base() const
    {
        return GetPointer<const LottieFlatBuffer::BaseLayer *>(VT_BASE);
    }
    const LottieFlatBuffer::TextData *data() const
    {
        return GetPointer<const LottieFlatBuffer::TextData *>(VT_DATA);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyOffset(verifier, VT_BASE) && verifier.VerifyTable(base()) &&
               VerifyOffset(verifier, VT_DATA) && verifier.VerifyTable(data()) && verifier.EndTable();
    }
};

struct TextLayerBuilder {
    typedef TextLayer Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_base(::flatbuffers::Offset<LottieFlatBuffer::BaseLayer> base)
    {
        fbb_.AddOffset(TextLayer::VT_BASE, base);
    }
    void add_data(::flatbuffers::Offset<LottieFlatBuffer::TextData> data)
    {
        fbb_.AddOffset(TextLayer::VT_DATA, data);
    }
    explicit TextLayerBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<TextLayer> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<TextLayer>(end);
        return o;
    }
};

inline ::flatbuffers::Offset<TextLayer> CreateTextLayer(::flatbuffers::FlatBufferBuilder &_fbb,
                                                        ::flatbuffers::Offset<LottieFlatBuffer::BaseLayer> base = 0,
                                                        ::flatbuffers::Offset<LottieFlatBuffer::TextData> data = 0)
{
    TextLayerBuilder builder_(_fbb);
    builder_.add_data(data);
    builder_.add_base(base);
    return builder_.Finish();
}

struct ImageLayer FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef ImageLayerBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE { VT_BASE = 4, VT_INDEX = 6 };
    const LottieFlatBuffer::BaseLayer *base() const
    {
        return GetPointer<const LottieFlatBuffer::BaseLayer *>(VT_BASE);
    }
    int32_t index() const
    {
        return GetField<int32_t>(VT_INDEX, 0);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyOffset(verifier, VT_BASE) && verifier.VerifyTable(base()) &&
               VerifyField<int32_t>(verifier, VT_INDEX, 4) && verifier.EndTable();
    }
};

struct ImageLayerBuilder {
    typedef ImageLayer Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_base(::flatbuffers::Offset<LottieFlatBuffer::BaseLayer> base)
    {
        fbb_.AddOffset(ImageLayer::VT_BASE, base);
    }
    void add_index(int32_t index)
    {
        fbb_.AddElement<int32_t>(ImageLayer::VT_INDEX, index, 0);
    }
    explicit ImageLayerBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<ImageLayer> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<ImageLayer>(end);
        return o;
    }
};

inline ::flatbuffers::Offset<ImageLayer> CreateImageLayer(::flatbuffers::FlatBufferBuilder &_fbb,
                                                          ::flatbuffers::Offset<LottieFlatBuffer::BaseLayer> base = 0,
                                                          int32_t index = 0)
{
    ImageLayerBuilder builder_(_fbb);
    builder_.add_index(index);
    builder_.add_base(base);
    return builder_.Finish();
}

struct SolidLayer FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef SolidLayerBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_BASE = 4,
        VT_WIDTH = 6,
        VT_HEIGHT = 8,
        VT_COLOR = 10
    };
    const LottieFlatBuffer::BaseLayer *base() const
    {
        return GetPointer<const LottieFlatBuffer::BaseLayer *>(VT_BASE);
    }
    uint16_t width() const
    {
        return GetField<uint16_t>(VT_WIDTH, 0);
    }
    uint16_t height() const
    {
        return GetField<uint16_t>(VT_HEIGHT, 0);
    }
    const LottieFlatBuffer::Color24 *color() const
    {
        return GetStruct<const LottieFlatBuffer::Color24 *>(VT_COLOR);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyOffset(verifier, VT_BASE) && verifier.VerifyTable(base()) &&
               VerifyField<uint16_t>(verifier, VT_WIDTH, 2) && VerifyField<uint16_t>(verifier, VT_HEIGHT, 2) &&
               VerifyField<LottieFlatBuffer::Color24>(verifier, VT_COLOR, 1) && verifier.EndTable();
    }
};

struct SolidLayerBuilder {
    typedef SolidLayer Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_base(::flatbuffers::Offset<LottieFlatBuffer::BaseLayer> base)
    {
        fbb_.AddOffset(SolidLayer::VT_BASE, base);
    }
    void add_width(uint16_t width)
    {
        fbb_.AddElement<uint16_t>(SolidLayer::VT_WIDTH, width, 0);
    }
    void add_height(uint16_t height)
    {
        fbb_.AddElement<uint16_t>(SolidLayer::VT_HEIGHT, height, 0);
    }
    void add_color(const LottieFlatBuffer::Color24 *color)
    {
        fbb_.AddStruct(SolidLayer::VT_COLOR, color);
    }
    explicit SolidLayerBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<SolidLayer> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<SolidLayer>(end);
        return o;
    }
};

inline ::flatbuffers::Offset<SolidLayer> CreateSolidLayer(::flatbuffers::FlatBufferBuilder &_fbb,
                                                          ::flatbuffers::Offset<LottieFlatBuffer::BaseLayer> base = 0,
                                                          uint16_t width = 0, uint16_t height = 0,
                                                          const LottieFlatBuffer::Color24 *color = nullptr)
{
    SolidLayerBuilder builder_(_fbb);
    builder_.add_color(color);
    builder_.add_base(base);
    builder_.add_height(height);
    builder_.add_width(width);
    return builder_.Finish();
}

struct Asset FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef AssetBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE { VT_ID = 4, VT_LAYERS_TYPE = 6, VT_LAYERS = 8 };
    const ::flatbuffers::String *id() const
    {
        return GetPointer<const ::flatbuffers::String *>(VT_ID);
    }
    const ::flatbuffers::Vector<uint8_t> *layers_type() const
    {
        return GetPointer<const ::flatbuffers::Vector<uint8_t> *>(VT_LAYERS_TYPE);
    }
    const ::flatbuffers::Vector<::flatbuffers::Offset<void>> *layers() const
    {
        return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<void>> *>(VT_LAYERS);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyOffset(verifier, VT_ID) && verifier.VerifyString(id()) &&
               VerifyOffset(verifier, VT_LAYERS_TYPE) && verifier.VerifyVector(layers_type()) &&
               VerifyOffset(verifier, VT_LAYERS) && verifier.VerifyVector(layers()) &&
               VerifyLayerVector(verifier, layers(), layers_type()) && verifier.EndTable();
    }
};

struct AssetBuilder {
    typedef Asset Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_id(::flatbuffers::Offset<::flatbuffers::String> id)
    {
        fbb_.AddOffset(Asset::VT_ID, id);
    }
    void add_layers_type(::flatbuffers::Offset<::flatbuffers::Vector<uint8_t>> layers_type)
    {
        fbb_.AddOffset(Asset::VT_LAYERS_TYPE, layers_type);
    }
    void add_layers(::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<void>>> layers)
    {
        fbb_.AddOffset(Asset::VT_LAYERS, layers);
    }
    explicit AssetBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<Asset> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<Asset>(end);
        return o;
    }
};

inline ::flatbuffers::Offset<Asset> CreateAsset(
    ::flatbuffers::FlatBufferBuilder &_fbb, ::flatbuffers::Offset<::flatbuffers::String> id = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<uint8_t>> layers_type = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<void>>> layers = 0)
{
    AssetBuilder builder_(_fbb);
    builder_.add_layers(layers);
    builder_.add_layers_type(layers_type);
    builder_.add_id(id);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<Asset> CreateAssetDirect(::flatbuffers::FlatBufferBuilder &_fbb, const char *id = nullptr,
                                                      const std::vector<uint8_t> *layers_type = nullptr,
                                                      const std::vector<::flatbuffers::Offset<void>> *layers = nullptr)
{
    auto id__ = id ? _fbb.CreateString(id) : 0;
    auto layers_type__ = layers_type ? _fbb.CreateVector<uint8_t>(*layers_type) : 0;
    auto layers__ = layers ? _fbb.CreateVector<::flatbuffers::Offset<void>>(*layers) : 0;
    return LottieFlatBuffer::CreateAsset(_fbb, id__, layers_type__, layers__);
}

struct Animations FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
    typedef AnimationsBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
        VT_VERSION = 4,
        VT_LAYERS_TYPE = 6,
        VT_LAYERS = 8,
        VT_FRAME_RATE = 10,
        VT_START_FRAME = 12,
        VT_END_FRAME = 14,
        VT_WIDTH = 16,
        VT_HEIGHT = 18,
        VT_ASSETS = 20
    };
    const ::flatbuffers::String *version() const
    {
        return GetPointer<const ::flatbuffers::String *>(VT_VERSION);
    }
    const ::flatbuffers::Vector<uint8_t> *layers_type() const
    {
        return GetPointer<const ::flatbuffers::Vector<uint8_t> *>(VT_LAYERS_TYPE);
    }
    const ::flatbuffers::Vector<::flatbuffers::Offset<void>> *layers() const
    {
        return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<void>> *>(VT_LAYERS);
    }
    float frame_rate() const
    {
        return GetField<float>(VT_FRAME_RATE, 0.0f);
    }
    float start_frame() const
    {
        return GetField<float>(VT_START_FRAME, 0.0f);
    }
    float end_frame() const
    {
        return GetField<float>(VT_END_FRAME, 0.0f);
    }
    float width() const
    {
        return GetField<float>(VT_WIDTH, 0.0f);
    }
    float height() const
    {
        return GetField<float>(VT_HEIGHT, 0.0f);
    }
    const ::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::Asset>> *assets() const
    {
        return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::Asset>> *>(VT_ASSETS);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyOffset(verifier, VT_VERSION) && verifier.VerifyString(version()) &&
               VerifyOffsetRequired(verifier, VT_LAYERS_TYPE) && verifier.VerifyVector(layers_type()) &&
               VerifyOffsetRequired(verifier, VT_LAYERS) && verifier.VerifyVector(layers()) &&
               VerifyLayerVector(verifier, layers(), layers_type()) && VerifyField<float>(verifier, VT_FRAME_RATE, 4) &&
               VerifyField<float>(verifier, VT_START_FRAME, 4) && VerifyField<float>(verifier, VT_END_FRAME, 4) &&
               VerifyField<float>(verifier, VT_WIDTH, 4) && VerifyField<float>(verifier, VT_HEIGHT, 4) &&
               VerifyOffset(verifier, VT_ASSETS) && verifier.VerifyVector(assets()) &&
               verifier.VerifyVectorOfTables(assets()) && verifier.EndTable();
    }
};

struct AnimationsBuilder {
    typedef Animations Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_version(::flatbuffers::Offset<::flatbuffers::String> version)
    {
        fbb_.AddOffset(Animations::VT_VERSION, version);
    }
    void add_layers_type(::flatbuffers::Offset<::flatbuffers::Vector<uint8_t>> layers_type)
    {
        fbb_.AddOffset(Animations::VT_LAYERS_TYPE, layers_type);
    }
    void add_layers(::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<void>>> layers)
    {
        fbb_.AddOffset(Animations::VT_LAYERS, layers);
    }
    void add_frame_rate(float frame_rate)
    {
        fbb_.AddElement<float>(Animations::VT_FRAME_RATE, frame_rate, 0.0f);
    }
    void add_start_frame(float start_frame)
    {
        fbb_.AddElement<float>(Animations::VT_START_FRAME, start_frame, 0.0f);
    }
    void add_end_frame(float end_frame)
    {
        fbb_.AddElement<float>(Animations::VT_END_FRAME, end_frame, 0.0f);
    }
    void add_width(float width)
    {
        fbb_.AddElement<float>(Animations::VT_WIDTH, width, 0.0f);
    }
    void add_height(float height)
    {
        fbb_.AddElement<float>(Animations::VT_HEIGHT, height, 0.0f);
    }
    void add_assets(::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::Asset>>> assets)
    {
        fbb_.AddOffset(Animations::VT_ASSETS, assets);
    }
    explicit AnimationsBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<Animations> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<Animations>(end);
        fbb_.Required(o, Animations::VT_LAYERS_TYPE);
        fbb_.Required(o, Animations::VT_LAYERS);
        return o;
    }
};

inline ::flatbuffers::Offset<Animations> CreateAnimations(
    ::flatbuffers::FlatBufferBuilder &_fbb, ::flatbuffers::Offset<::flatbuffers::String> version = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<uint8_t>> layers_type = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<void>>> layers = 0, float frame_rate = 0.0f,
    float start_frame = 0.0f, float end_frame = 0.0f, float width = 0.0f, float height = 0.0f,
    ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<LottieFlatBuffer::Asset>>> assets = 0)
{
    AnimationsBuilder builder_(_fbb);
    builder_.add_assets(assets);
    builder_.add_height(height);
    builder_.add_width(width);
    builder_.add_end_frame(end_frame);
    builder_.add_start_frame(start_frame);
    builder_.add_frame_rate(frame_rate);
    builder_.add_layers(layers);
    builder_.add_layers_type(layers_type);
    builder_.add_version(version);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<Animations> CreateAnimationsDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb, const char *version = nullptr,
    const std::vector<uint8_t> *layers_type = nullptr, const std::vector<::flatbuffers::Offset<void>> *layers = nullptr,
    float frame_rate = 0.0f, float start_frame = 0.0f, float end_frame = 0.0f, float width = 0.0f, float height = 0.0f,
    const std::vector<::flatbuffers::Offset<LottieFlatBuffer::Asset>> *assets = nullptr)
{
    auto version__ = version ? _fbb.CreateString(version) : 0;
    auto layers_type__ = layers_type ? _fbb.CreateVector<uint8_t>(*layers_type) : 0;
    auto layers__ = layers ? _fbb.CreateVector<::flatbuffers::Offset<void>>(*layers) : 0;
    auto assets__ = assets ? _fbb.CreateVector<::flatbuffers::Offset<LottieFlatBuffer::Asset>>(*assets) : 0;
    return LottieFlatBuffer::CreateAnimations(_fbb, version__, layers_type__, layers__, frame_rate, start_frame,
                                              end_frame, width, height, assets__);
}

inline bool VerifyFloatProp(::flatbuffers::Verifier &verifier, const void *obj, FloatProp type)
{
    switch (type) {
        case FloatProp_NONE: {
            return true;
        }
        case FloatProp_frames: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::FloatFrameArray *>(obj);
            return verifier.VerifyTable(ptr);
        }
        case FloatProp_value: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::FloatScalar *>(obj);
            return verifier.VerifyTable(ptr);
        }
        default:
            return true;
    }
}

inline bool VerifyFloatPropVector(::flatbuffers::Verifier &verifier,
                                  const ::flatbuffers::Vector<::flatbuffers::Offset<void>> *values,
                                  const ::flatbuffers::Vector<uint8_t> *types)
{
    if (!values || !types)
        return !values && !types;
    if (values->size() != types->size())
        return false;
    for (::flatbuffers::uoffset_t i = 0; i < values->size(); ++i) {
        if (!VerifyFloatProp(verifier, values->Get(i), types->GetEnum<FloatProp>(i))) {
            return false;
        }
    }
    return true;
}

inline bool VerifyIntegerProp(::flatbuffers::Verifier &verifier, const void *obj, IntegerProp type)
{
    switch (type) {
        case IntegerProp_NONE: {
            return true;
        }
        case IntegerProp_frames: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::IntegerFrameArray *>(obj);
            return verifier.VerifyTable(ptr);
        }
        case IntegerProp_value: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::IntegerScalar *>(obj);
            return verifier.VerifyTable(ptr);
        }
        default:
            return true;
    }
}

inline bool VerifyIntegerPropVector(::flatbuffers::Verifier &verifier,
                                    const ::flatbuffers::Vector<::flatbuffers::Offset<void>> *values,
                                    const ::flatbuffers::Vector<uint8_t> *types)
{
    if (!values || !types)
        return !values && !types;
    if (values->size() != types->size())
        return false;
    for (::flatbuffers::uoffset_t i = 0; i < values->size(); ++i) {
        if (!VerifyIntegerProp(verifier, values->Get(i), types->GetEnum<IntegerProp>(i))) {
            return false;
        }
    }
    return true;
}

inline bool VerifyVectorProp(::flatbuffers::Verifier &verifier, const void *obj, VectorProp type)
{
    switch (type) {
        case VectorProp_NONE: {
            return true;
        }
        case VectorProp_frames: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::VectorFrameArray *>(obj);
            return verifier.VerifyTable(ptr);
        }
        case VectorProp_value: {
            return verifier.VerifyField<LottieFlatBuffer::Point>(static_cast<const uint8_t *>(obj), 0, 4);
        }
        default:
            return true;
    }
}

inline bool VerifyVectorPropVector(::flatbuffers::Verifier &verifier,
                                   const ::flatbuffers::Vector<::flatbuffers::Offset<void>> *values,
                                   const ::flatbuffers::Vector<uint8_t> *types)
{
    if (!values || !types)
        return !values && !types;
    if (values->size() != types->size())
        return false;
    for (::flatbuffers::uoffset_t i = 0; i < values->size(); ++i) {
        if (!VerifyVectorProp(verifier, values->Get(i), types->GetEnum<VectorProp>(i))) {
            return false;
        }
    }
    return true;
}

inline bool VerifyColorProp(::flatbuffers::Verifier &verifier, const void *obj, ColorProp type)
{
    switch (type) {
        case ColorProp_NONE: {
            return true;
        }
        case ColorProp_frames: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::ColorFrameArray *>(obj);
            return verifier.VerifyTable(ptr);
        }
        case ColorProp_color: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::Color24Scalar *>(obj);
            return verifier.VerifyTable(ptr);
        }
        default:
            return true;
    }
}

inline bool VerifyColorPropVector(::flatbuffers::Verifier &verifier,
                                  const ::flatbuffers::Vector<::flatbuffers::Offset<void>> *values,
                                  const ::flatbuffers::Vector<uint8_t> *types)
{
    if (!values || !types)
        return !values && !types;
    if (values->size() != types->size())
        return false;
    for (::flatbuffers::uoffset_t i = 0; i < values->size(); ++i) {
        if (!VerifyColorProp(verifier, values->Get(i), types->GetEnum<ColorProp>(i))) {
            return false;
        }
    }
    return true;
}

inline bool VerifyGradientProp(::flatbuffers::Verifier &verifier, const void *obj, GradientProp type)
{
    switch (type) {
        case GradientProp_NONE: {
            return true;
        }
        case GradientProp_frames: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::GradientFrameArray *>(obj);
            return verifier.VerifyTable(ptr);
        }
        case GradientProp_value: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::Gradient *>(obj);
            return verifier.VerifyTable(ptr);
        }
        default:
            return true;
    }
}

inline bool VerifyGradientPropVector(::flatbuffers::Verifier &verifier,
                                     const ::flatbuffers::Vector<::flatbuffers::Offset<void>> *values,
                                     const ::flatbuffers::Vector<uint8_t> *types)
{
    if (!values || !types)
        return !values && !types;
    if (values->size() != types->size())
        return false;
    for (::flatbuffers::uoffset_t i = 0; i < values->size(); ++i) {
        if (!VerifyGradientProp(verifier, values->Get(i), types->GetEnum<GradientProp>(i))) {
            return false;
        }
    }
    return true;
}

inline bool VerifyBezierDataProp(::flatbuffers::Verifier &verifier, const void *obj, BezierDataProp type)
{
    switch (type) {
        case BezierDataProp_NONE: {
            return true;
        }
        case BezierDataProp_frames: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::BezierDataFrameArray *>(obj);
            return verifier.VerifyTable(ptr);
        }
        case BezierDataProp_value: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::BezierData *>(obj);
            return verifier.VerifyTable(ptr);
        }
        default:
            return true;
    }
}

inline bool VerifyBezierDataPropVector(::flatbuffers::Verifier &verifier,
                                       const ::flatbuffers::Vector<::flatbuffers::Offset<void>> *values,
                                       const ::flatbuffers::Vector<uint8_t> *types)
{
    if (!values || !types)
        return !values && !types;
    if (values->size() != types->size())
        return false;
    for (::flatbuffers::uoffset_t i = 0; i < values->size(); ++i) {
        if (!VerifyBezierDataProp(verifier, values->Get(i), types->GetEnum<BezierDataProp>(i))) {
            return false;
        }
    }
    return true;
}

inline bool VerifyPositionProp(::flatbuffers::Verifier &verifier, const void *obj, PositionProp type)
{
    switch (type) {
        case PositionProp_NONE: {
            return true;
        }
        case PositionProp_frames: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::PositionFrameArray *>(obj);
            return verifier.VerifyTable(ptr);
        }
        case PositionProp_value: {
            return verifier.VerifyField<LottieFlatBuffer::Point>(static_cast<const uint8_t *>(obj), 0, 4);
        }
        default:
            return true;
    }
}

inline bool VerifyPositionPropVector(::flatbuffers::Verifier &verifier,
                                     const ::flatbuffers::Vector<::flatbuffers::Offset<void>> *values,
                                     const ::flatbuffers::Vector<uint8_t> *types)
{
    if (!values || !types)
        return !values && !types;
    if (values->size() != types->size())
        return false;
    for (::flatbuffers::uoffset_t i = 0; i < values->size(); ++i) {
        if (!VerifyPositionProp(verifier, values->Get(i), types->GetEnum<PositionProp>(i))) {
            return false;
        }
    }
    return true;
}

inline bool VerifyGraphicElement(::flatbuffers::Verifier &verifier, const void *obj, GraphicElement type)
{
    switch (type) {
        case GraphicElement_NONE: {
            return true;
        }
        case GraphicElement_EllipseShape: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::EllipseShape *>(obj);
            return verifier.VerifyTable(ptr);
        }
        case GraphicElement_RectShape: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::RectShape *>(obj);
            return verifier.VerifyTable(ptr);
        }
        case GraphicElement_PathShape: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::PathShape *>(obj);
            return verifier.VerifyTable(ptr);
        }
        case GraphicElement_FillStyle: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::FillStyle *>(obj);
            return verifier.VerifyTable(ptr);
        }
        case GraphicElement_StrokeStyle: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::StrokeStyle *>(obj);
            return verifier.VerifyTable(ptr);
        }
        case GraphicElement_GradientFillStyle: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::GradientFillStyle *>(obj);
            return verifier.VerifyTable(ptr);
        }
        case GraphicElement_GradientStrokeStyle: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::GradientStrokeStyle *>(obj);
            return verifier.VerifyTable(ptr);
        }
        case GraphicElement_TransformShape: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::TransformShape *>(obj);
            return verifier.VerifyTable(ptr);
        }
        case GraphicElement_TrimPath: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::TrimPath *>(obj);
            return verifier.VerifyTable(ptr);
        }
        case GraphicElement_GroupShape: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::GroupShape *>(obj);
            return verifier.VerifyTable(ptr);
        }
        default:
            return true;
    }
}

inline bool VerifyGraphicElementVector(::flatbuffers::Verifier &verifier,
                                       const ::flatbuffers::Vector<::flatbuffers::Offset<void>> *values,
                                       const ::flatbuffers::Vector<uint8_t> *types)
{
    if (!values || !types)
        return !values && !types;
    if (values->size() != types->size())
        return false;
    for (::flatbuffers::uoffset_t i = 0; i < values->size(); ++i) {
        if (!VerifyGraphicElement(verifier, values->Get(i), types->GetEnum<GraphicElement>(i))) {
            return false;
        }
    }
    return true;
}

inline bool VerifyLayer(::flatbuffers::Verifier &verifier, const void *obj, Layer type)
{
    switch (type) {
        case Layer_NONE: {
            return true;
        }
        case Layer_PrecompositionLayer: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::PrecompositionLayer *>(obj);
            return verifier.VerifyTable(ptr);
        }
        case Layer_SolidLayer: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::SolidLayer *>(obj);
            return verifier.VerifyTable(ptr);
        }
        case Layer_ImageLayer: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::ImageLayer *>(obj);
            return verifier.VerifyTable(ptr);
        }
        case Layer_NullLayer: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::NullLayer *>(obj);
            return verifier.VerifyTable(ptr);
        }
        case Layer_ShapeLayer: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::ShapeLayer *>(obj);
            return verifier.VerifyTable(ptr);
        }
        case Layer_TextLayer: {
            auto ptr = reinterpret_cast<const LottieFlatBuffer::TextLayer *>(obj);
            return verifier.VerifyTable(ptr);
        }
        default:
            return true;
    }
}

inline bool VerifyLayerVector(::flatbuffers::Verifier &verifier,
                              const ::flatbuffers::Vector<::flatbuffers::Offset<void>> *values,
                              const ::flatbuffers::Vector<uint8_t> *types)
{
    if (!values || !types)
        return !values && !types;
    if (values->size() != types->size())
        return false;
    for (::flatbuffers::uoffset_t i = 0; i < values->size(); ++i) {
        if (!VerifyLayer(verifier, values->Get(i), types->GetEnum<Layer>(i))) {
            return false;
        }
    }
    return true;
}

inline const LottieFlatBuffer::Animations *GetAnimations(const void *buf)
{
    return ::flatbuffers::GetRoot<LottieFlatBuffer::Animations>(buf);
}

inline const LottieFlatBuffer::Animations *GetSizePrefixedAnimations(const void *buf)
{
    return ::flatbuffers::GetSizePrefixedRoot<LottieFlatBuffer::Animations>(buf);
}

inline bool VerifyAnimationsBuffer(::flatbuffers::Verifier &verifier)
{
    return verifier.VerifyBuffer<LottieFlatBuffer::Animations>(nullptr);
}

inline bool VerifySizePrefixedAnimationsBuffer(::flatbuffers::Verifier &verifier)
{
    return verifier.VerifySizePrefixedBuffer<LottieFlatBuffer::Animations>(nullptr);
}

inline void FinishAnimationsBuffer(::flatbuffers::FlatBufferBuilder &fbb,
                                   ::flatbuffers::Offset<LottieFlatBuffer::Animations> root)
{
    fbb.Finish(root);
}

inline void FinishSizePrefixedAnimationsBuffer(::flatbuffers::FlatBufferBuilder &fbb,
                                               ::flatbuffers::Offset<LottieFlatBuffer::Animations> root)
{
    fbb.FinishSizePrefixed(root);
}

}  // namespace LottieFlatBuffer

#endif  // LOTT_FLATBUFFER_H
