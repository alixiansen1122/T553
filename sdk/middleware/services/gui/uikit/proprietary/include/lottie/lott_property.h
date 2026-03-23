/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: LottProperty
 * Author: Hisi Graphic Team
 * Created: 2025-5
 */

#ifndef LOTT_PROPERTY_H
#define LOTT_PROPERTY_H

#include "common/graphic_hardware_types.h"
#include "gfx_utils/color.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/graphic_math.h"
#include "gfx_utils/heap_base.h"
#include "gfx_utils/vector.h"
#include "lottie/lott_math.h"
#include "lottie/lott_interpolator.h"

namespace OHOS {

struct LottFloatFrame {
    float start; // keyframe value
    float time; // frame number
    LottInterpolator* interpolator;
    bool hold = false; //  do not interpolate
};

class LottFloatProp : public HeapBase {
public:
    LottFloatProp(float v) : value_(v), frames_(nullptr) {}
    LottFloatProp() : value_(0.0f), frames_(nullptr) {}
    ~LottFloatProp();
    LottFloatFrame* CreateFrame();
    void UpdateValue(float frameNo);

    float value_;
    bool isUpdateValue_ = false;

private:
    Graphic::Vector<LottFloatFrame*>* frames_;

    float Interpolate(LottFloatFrame* frame, LottFloatFrame* nextFrame, float frameNo);
};

struct LottIntegerFrame {
    int start; // keyframe value
    float time; // frame number
    LottInterpolator* interpolator;
    bool hold = false; //  do not interpolate
};

class LottIntegerProp : public HeapBase {
public:
    LottIntegerProp(int v) : value_(v), frames_(nullptr) {}
    LottIntegerProp() : value_(0), frames_(nullptr) {}
    ~LottIntegerProp();
    LottIntegerFrame* CreateFrame();
    void UpdateValue(float frameNo);

    int value_;
    bool isUpdateValue_ = false;

private:
    Graphic::Vector<LottIntegerFrame*>* frames_;

    int Interpolate(LottIntegerFrame* frame, LottIntegerFrame* nextFrame, float frameNo);
};

struct LottColorFrame {
    Color32 start; // keyframe value
    float time; // frame number
    LottInterpolator* interpolator;
    bool hold = false; //  do not interpolate
};

class LottColorProp : public HeapBase {
public:
    LottColorProp(Color32 v) : value_(v), frames_(nullptr) {}
    LottColorProp() : value_(Color::Black()), frames_(nullptr) {}
    ~LottColorProp();
    LottColorFrame* CreateFrame();
    void UpdateValue(float frameNo);

    Color32 value_;
    bool isUpdateValue_ = false;

private:
    Graphic::Vector<LottColorFrame*>* frames_;

    Color32 Interpolate(LottColorFrame* frame, LottColorFrame* nextFrame, float frameNo);
};

struct LottVectorFrame {
    Vector2<float> start; // keyframe value
    float time; // frame number
    LottInterpolator* interpolator;
    bool hold = false; //  do not interpolate
};

class LottVectorProp : public HeapBase {
public:
    LottVectorProp(Vector2<float> v) : value_(v), frames_(nullptr) {}
    LottVectorProp() : value_(Vector2<float>(0.0f, 0.0f)), frames_(nullptr) {}
    ~LottVectorProp();
    LottVectorFrame* CreateFrame();
    void UpdateValue(float frameNo);

    Vector2<float> value_;
    bool isUpdateValue_ = false;

private:
    Graphic::Vector<LottVectorFrame*>* frames_;

    Vector2<float> Interpolate(LottVectorFrame* frame, LottVectorFrame* nextFrame, float frameNo);
};

struct LottColorStopsFrame {
    Graphic::Vector<ColorStop> start; // keyframe value
    float time; // frame number
    LottInterpolator* interpolator;
    bool hold = false; //  do not interpolate
};

class LottColorStopsProp : public HeapBase {
public:
    LottColorStopsProp(Graphic::Vector<ColorStop> v) : value_(v), frames_(nullptr) {}
    LottColorStopsProp() : frames_(nullptr) {}
    ~LottColorStopsProp();
    LottColorStopsFrame* CreateFrame();
    void UpdateValue(float frameNo);

    Graphic::Vector<ColorStop> value_;
    bool isUpdateValue_ = false;

private:
    Graphic::Vector<ColorStop>* preValue_ = nullptr;
    Graphic::Vector<LottColorStopsFrame*>* frames_;

    void CompareValueWithPre(Graphic::Vector<ColorStop>* curValue);
};

struct LottPositionFrame {
    Vector2<float> start; // keyframe value
    float time; // frame number
    LottInterpolator* interpolator;
    float length;
    bool hasTangent = false;
    Vector2<float> outTangent, inTangent;
    bool hold = false; //  do not interpolate
};

class LottPositionProp : public HeapBase {
public:
    LottPositionProp(Vector2<float> v) : value_(v), frames_(nullptr) {}
    LottPositionProp() : value_(Vector2<float>(0.0f, 0.0f)), frames_(nullptr) {}
    ~LottPositionProp();
    LottPositionFrame* CreateFrame();
    float GetAngle(float frameNo);
    void UpdateValue(float frameNo);
    void PrepareLength();

    Vector2<float> value_;
    bool isUpdateValue_ = false;

private:
    Graphic::Vector<LottPositionFrame*>* frames_;

    Vector2<float> Interpolate(LottPositionFrame* frame, LottPositionFrame* nextFrame, float frameNo);
    float Angle(LottPositionFrame* frame, LottPositionFrame* nextFrame, float frameNo);
};

struct LottPathDataFrame {
    Path start; // keyframe value
    float time; // frame number
    LottInterpolator* interpolator;
    bool hold = false; //  do not interpolate
};

class LottPathDataProp : public HeapBase {
public:
    LottPathDataProp(Path v) : value_(v), innerPath_(Path{0, nullptr, 0, nullptr}), frames_(nullptr) {}
    LottPathDataProp() : value_(Path{0, nullptr, 0, nullptr}),
        innerPath_(Path{0, nullptr, 0, nullptr}), frames_(nullptr) {}
    ~LottPathDataProp();
    LottPathDataFrame* CreateFrame();
    void UpdateValue(float frameNo);

    Path value_;
    bool isUpdateValue_ = false;

private:
    Path* prePath_ = nullptr;
    Path innerPath_; // Store temporary path data to avoid allocate memory frequently
    Graphic::Vector<LottPathDataFrame*>* frames_;

    void CompareValueWithPre(Path* path);
    bool UpdatePath(LottPathDataFrame* curFrame, LottPathDataFrame* nextFrame, float time);
};

}
#endif // LOTT_PROPERTY_H
