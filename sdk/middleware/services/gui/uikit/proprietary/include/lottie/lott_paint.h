/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef LOTT_PAINT_H
#define LOTT_PAINT_H

#include "components/ui_view.h"
#include "common/graphic_hardware_types.h"
#include "gfx_utils/style.h"
#include "gfx_utils/heap_base.h"
#include "lottie/lott_property.h"
#include "hals/gralloc_engines.h"

namespace OHOS {
const uint8_t LOTT_OPA_MAX = 100; // lott opa range is [0, 100]
const float LOTT_OPA_PCT_MULTIPLIER = 0.01f;

class LottGradient : public HeapBase {
public:
    GradientSpread spread_;
    LottVectorProp start_;
    LottVectorProp end_;
    LottFloatProp height_;
    LottFloatProp angle_;
    LottIntegerProp opa_;
    LottColorStopsProp stops_;
    GradientType type_; // only support linear and radial

    void Update(float frameNo);
};

class LottFill : public HeapBase {
public:
    LottFill();
    virtual ~LottFill();

    LottIntegerProp opa_;
    LottColorProp color_;
    FillRule fillRule_;
    LottGradient* gradient_ = nullptr;

    void Update(float frameNo);
};

class LottStroke : public HeapBase {
public:
    LottStroke();
    virtual ~LottStroke();

    LottIntegerProp opa_;
    LottColorProp color_;
    LottFloatProp width_;
    float miterLimit_;
    JoinType join_;
    CapType cap_;
    LottGradient* gradient_ = nullptr;

    void Update(float frameNo);
};

class LottPaint : public HeapBase {
public:
    LottPaint();
    virtual ~LottPaint();
    void Update(float frameNo);

    LottFill* fill_ = nullptr;
    LottStroke* stroke_ = nullptr;
};

struct LottGradientValue {
    float startX;
    float startY;
    float endX;
    float endY;
    float height;
    float angle;
    ColorStop* stops = nullptr;
    uint16_t stopNum;
    GradientType type;

    ~LottGradientValue()
    {
        if (stops != nullptr) {
            UIFree(stops);
        }
    }
};

struct LottFillValue {
    uint32_t color;
    FillRule fillRule;
    LottGradientValue* gradient = nullptr;

    ~LottFillValue()
    {
        if (gradient != nullptr) {
            delete gradient;
        }
    }
};

struct LottStrokeValue {
    uint32_t color;
    float width;
    float miterLimit;
    JoinType join;
    CapType cap;
    LottGradientValue* gradient = nullptr;

    ~LottStrokeValue()
    {
        if (gradient != nullptr) {
            delete gradient;
        }
    }
};

struct LottPaintValue {
    LottFillValue* fill = nullptr;
    LottStrokeValue* stroke = nullptr;

    ~LottPaintValue()
    {
        if (fill != nullptr) {
            delete fill;
        }

        if (stroke != nullptr) {
            delete stroke;
        }
    }

    uint32_t GetMixedColor(uint32_t color, uint8_t opa)
    {
        const uint8_t alphaShift = 24; // 24: alpha shift
        uint8_t alpha = (color >> alphaShift) * (opa * LOTT_OPA_PCT_MULTIPLIER);
        uint32_t mixedColor = (alpha << alphaShift) | (color & 0x00FFFFFF);
        return mixedColor;
    }

    void ConvertGradient(LottGradientValue* value, LottGradient* gradient, uint8_t opa)
    {
        value->startX = gradient->start_.value_.x_;
        value->startY = gradient->start_.value_.y_;
        value->endX = gradient->end_.value_.x_;
        value->endY = gradient->end_.value_.y_;
        value->height = gradient->height_.value_;
        value->angle = gradient->angle_.value_;

        if (gradient->stops_.value_.Size() == 0) {
            return;
        }

        Graphic::Vector<ColorStop>* colorStops = &gradient->stops_.value_;

        if (value->stops != nullptr && value->stopNum != colorStops->Size()) {
            UIFree(value->stops);
            value->stops = nullptr;
            value->stopNum = 0;
        }

        uint32_t size = sizeof(ColorStop) * colorStops->Size();
        if (value->stops == nullptr) {
            value->stops = static_cast<ColorStop*>(UIMalloc(size));
            if (value->stops == nullptr) {
                return;
            }
        }

        memcpy_s(value->stops, size, colorStops->Begin(), size);
        value->stopNum = colorStops->Size();

        for (uint32_t i = 0; i < value->stopNum; i++) {
            value->stops[i].color = GetMixedColor(value->stops[i].color, opa);
        }
        value->type = gradient->type_;
    }

    bool FillValue(LottFill* lottFill, uint8_t opa)
    {
        if (lottFill == nullptr) {
            return false;
        }

        if (fill == nullptr) {
            fill = new LottFillValue;
            if (fill == nullptr) {
                return false;
            }
            fill->gradient = nullptr;
        }

        uint8_t lottOpa = lottFill->opa_.value_ * opa * LOTT_OPA_PCT_MULTIPLIER;
        fill->color = GetMixedColor(lottFill->color_.value_.full, lottOpa);
        fill->fillRule = lottFill->fillRule_;

        if (lottFill->gradient_ != nullptr) {
            if (fill->gradient == nullptr) {
                fill->gradient = new LottGradientValue;
                if (fill->gradient == nullptr) {
                    delete fill;
                    fill = nullptr;
                    return false;
                }
                fill->gradient->stops = nullptr;
                fill->gradient->stopNum = 0;
            }
            ConvertGradient(fill->gradient, lottFill->gradient_, opa);
        }
        return true;
    }

    bool StrokeValue(LottStroke* lottStroke, uint8_t opa)
    {
        if (lottStroke == nullptr) {
            return false;
        }

        if (stroke == nullptr) {
            stroke = new LottStrokeValue;
            if (stroke == nullptr) {
                return false;
            }
            stroke->gradient = nullptr;
        }

        uint8_t lottOpa = lottStroke->opa_.value_ * opa * LOTT_OPA_PCT_MULTIPLIER;
        stroke->color = GetMixedColor(lottStroke->color_.value_.full, lottOpa);
        stroke->width = lottStroke->width_.value_;
        stroke->miterLimit = lottStroke->miterLimit_;
        stroke->join = lottStroke->join_;
        stroke->cap = lottStroke->cap_;

        if (lottStroke->gradient_ != nullptr) {
            if (stroke->gradient == nullptr) {
                stroke->gradient = new LottGradientValue;
                if (stroke->gradient == nullptr) {
                    delete stroke;
                    stroke = nullptr;
                    return false;
                }
                stroke->gradient->stops = nullptr;
                stroke->gradient->stopNum = 0;
            }
            ConvertGradient(stroke->gradient, lottStroke->gradient_, opa);
        }
        return true;
    }

    bool UpdatePaint(LottPaint* paint, uint8_t opa)
    {
        if (paint == nullptr) {
            return false;
        }
        FillValue(paint->fill_, opa);
        StrokeValue(paint->stroke_, opa);
        return true;
    }

    bool DeepCopyGradient(const LottGradientValue* src, LottGradientValue*& dst)
    {
        if (src->stops == nullptr) {
            return false;
        }
        if (dst == nullptr) {
            dst = new LottGradientValue;
            if (dst == nullptr) {
                return false;
            }
            dst->stops = nullptr;
            dst->stopNum = 0;
        }

        dst->startX = src->startX;
        dst->startY = src->startY;
        dst->endX = src->endX;
        dst->endY = src->endY;
        dst->height = src->height;
        dst->angle = src->angle;
        dst->type = src->type;

        if ((dst->stops != nullptr) && (dst->stopNum != src->stopNum)) {
            UIFree(dst->stops);
            dst->stops = nullptr;
            dst->stopNum = 0;
        }

        uint32_t size = sizeof(ColorStop) * src->stopNum;
        if (dst->stops == nullptr) {
            dst->stops = static_cast<ColorStop*>(UIMalloc(size));
            if (dst->stops == nullptr) {
                return false;
            }
        }
        memcpy_s(dst->stops, size, src->stops, size);
        dst->stopNum = src->stopNum;
        return true;
    }

    bool DeepCopy(LottPaintValue& value)
    {
        if (value.fill != nullptr) {
            if (fill == nullptr) {
                fill = new LottFillValue;
                if (fill == nullptr) {
                    return false;
                }
                fill->gradient = nullptr;
            }

            fill->color = value.fill->color;
            fill->fillRule = value.fill->fillRule;

            if (value.fill->gradient != nullptr) {
                DeepCopyGradient(value.fill->gradient, fill->gradient);
            }
        }

        if (value.stroke != nullptr) {
            if (stroke == nullptr) {
                stroke = new LottStrokeValue;
                if (stroke == nullptr) {
                    return false;
                }
                stroke->gradient = nullptr;
            }

            stroke->color = value.stroke->color;
            stroke->width = value.stroke->width;
            stroke->miterLimit = value.stroke->miterLimit;
            stroke->join = value.stroke->join;
            stroke->cap = value.stroke->cap;

            if (value.stroke->gradient != nullptr) {
                DeepCopyGradient(value.stroke->gradient, stroke->gradient);
            }
        }
        return true;
    }
};

}
#endif // LOTT_PAINT_H