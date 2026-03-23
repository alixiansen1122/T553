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

/**
 * @addtogroup UI_Components
 * @{
 *
 * @brief Defines UI components such as buttons, texts, images, lists, and progress bars.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file ui_canvas_ext.h
 *
 * @brief Defines the class to extend canvas/paint ability.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_UI_CANVAS_EXT_H
#define GRAPHIC_LITE_UI_CANVAS_EXT_H

#include "securec.h"
#include "components/ui_canvas.h"
#include "components/svg_data.h"
#include "gfx_utils/graphic_types.h"
#include "gfx_utils/list.h"
#include "gfx_utils/vector.h"
#include "securec.h"
#include "common/graphic_hardware_types.h"

namespace OHOS {
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
/**
 *
 * @brief Defines PaintExt to extend Paint ability, that support hardware paint.
 *
 * @since 1.0
 * @version 1.0
 */
class PaintExt : public Paint {
public:
    /**
     * @brief A constructor used to create an <b>PaintExt</b> instance.
     * @since 1.0
     * @version 1.0
     */
    PaintExt()
        : stops_(nullptr), stopCount_(0), gradientType_(GradientType::GRADIENT_BUTT), fillRule_(FillRule::NON_ZERO),
          gradientSpread_(GradientSpread::SPREAD_PAD), joinType_(JoinType::JOIN_MITER),
          miterLimit_(2), capType_(CAP_NONE), paintType_(PaintType::PAINT_COLOR) // 2: limit
    {
        memset_s(&linearGradient_, sizeof(linearGradient_), 0, sizeof(linearGradient_));
        memset_s(&radialGradient_, sizeof(radialGradient_), 0, sizeof(radialGradient_));
        memset_s(&sweepGradient_, sizeof(sweepGradient_), 0, sizeof(sweepGradient_));
        memset_s(&pattern_.img, sizeof(pattern_.img), 0, sizeof(pattern_.img));
        pattern_.color = 0;
        pattern_.wrap = PatternType::PATTERN_DISABLE;
    }

    /**
     * @brief A destructor used to delete the <b>PaintExt</b> instance.
     * @since 1.0
     * @version 1.0
     */
    ~PaintExt() override
    {
        if (stops_ != nullptr) {
            UIFree(stops_);
            stops_ = nullptr;
        }
    }

    /**
     * @brief Set the linear gradient info.
     *
     */
    void SetLinearGradient(const LinearGradient& linear, const ColorStop* stops, uint16_t stopCount)
    {
        SetColorStops(stops, stopCount);
        linearGradient_ = linear;
        gradientType_ = GradientType::GRADIENT_LINEAR;
        paintType_ = PaintType::PAINT_GRADIENT;
    }

    /**
     * @brief Set the radial gradient info.
     *
     */
    void SetRadialGradient(const RadialGradient& radial, const ColorStop* stops, uint16_t stopCount)
    {
        SetColorStops(stops, stopCount);
        radialGradient_ = radial;
        gradientType_ = GradientType::GRADIENT_RADIAL;
        paintType_ = PaintType::PAINT_GRADIENT;
    }

    /**
     * @brief Set the sweep gradient info.
     *
     */
    void SetSweepGradient(const SweepGradient& sweep, const ColorStop* stops, uint16_t stopCount)
    {
        SetColorStops(stops, stopCount);
        sweepGradient_ = sweep;
        gradientType_ = GradientType::GRADIENT_SWEEP;
        paintType_ = PaintType::PAINT_GRADIENT;
    }

    /**
     * @brief Set the fill rule object.
     *
     */
    void SetFillRule(const FillRule& fillRule)
    {
        fillRule_ = fillRule;
    }

    /**
     * @brief Set the gradient spread object.
     *
     */
    void SetGradientSpread(const GradientSpread& gradientSpread)
    {
        gradientSpread_ = gradientSpread;
    }

    /**
     * @brief Set the join Type.
     *
     */
    void SetJoinType(const JoinType& joinType)
    {
        joinType_ = joinType;
    }

    /**
     * @brief Set the miter limit.
     *
     */
    void SetMiterLimit(float miterLimit)
    {
        miterLimit_ = miterLimit;
    }

    /**
     * @brief Set the cap yype.
     *
     */
    void SetCapType(const CapType& capType)
    {
        capType_ = capType;
    }

    /**
     * @brief Set the antialiased flag.
     *
     */
    void SetAntialiased(bool antialiased)
    {
        antialiased_ = antialiased;
    }

    /**
     * @brief Set pattern params.
     * @param wrap means pattern fill type(fill, pad, repeat, reflect), color is used when wrap is fill mode,
              mat is used when pattern fill, img is image info for a picture.
     * @return No returns.
     */
    void SetWrapPattern(PatternType wrap, uint32_t color, Matrix3<float> mat, ImageInfo img)
    {
        pattern_.wrap = wrap;
        pattern_.color = color;
        pattern_.mat3 = mat;
        pattern_.img = img;
        paintType_ = PaintType::PAINT_PATTERN;
    }

    /**
     * @brief Set path data to the current path in the context.
     */
    void SetClipRect(Rect clipRect)
    {
        hasClipRect_ = true;
        clipRect_ = clipRect;
    }

    ColorStop* stops_;
    uint16_t stopCount_;
    GradientType gradientType_;
    LinearGradient linearGradient_;
    RadialGradient radialGradient_;
    SweepGradient sweepGradient_;
    FillRule fillRule_;
    GradientSpread gradientSpread_;
    JoinType joinType_;
    float miterLimit_;
    CapType capType_;
    bool antialiased_ = true;
    PaintType paintType_;
    PatternInfo pattern_;
    Rect clipRect_;
    bool hasClipRect_ = false;

private:
    void SetColorStops(const ColorStop* stops, uint16_t stopCount)
    {
        if (stops_ != nullptr) {
            UIFree(stops_);
        }
        stops_ = static_cast<ColorStop*>(UIMalloc(sizeof(ColorStop) * stopCount));
        if (stops_ == nullptr) {
#ifndef USE_DIAG_LOG
            GRAPHIC_LOGE("UIMalloc for stops failed.");
#endif
            return;
        }
        stopCount_ = stopCount;
        errno_t ret = memcpy_s(stops_, sizeof(ColorStop) * stopCount, stops, sizeof(ColorStop) * stopCount);
        if (ret != EOK) {
            UIFree(stops_);
            stops_ = nullptr;
        }
    }
};

/**
 * @brief Class to edtend UILabel ability.
 *
 * @since 1.0
 * @version 1.0
 */
class UICanvasExt : public UIView {
public:
    /**
     * @brief A constructor used to create an <b>AbstractAdapter</b> instance.
     *
     */
    UICanvasExt();

    /**
     * @brief A destructor used to delete the <b>UICanvasExt</b> instance.
     *
     */
    ~UICanvasExt() override;

    /**
     * @brief Get current view type.
     *
     */
    UIViewType GetViewType() const override
    {
        return UI_CANVAS_EXT;
    }

    /**
     * @brief Begin a path.
     *        This method would init a new path in the context.
     * @param paint Indicates the path style of the newly-created path in the context.
     * @return Returns index value that relates to an object. Returns -1 when any error occurs.
     */
    int16_t BeginPath(const PaintExt &paint);

    /**
     * @brief Set path data to the current path in the context.
     */
    void SetPathData(uint32_t cmdNum, const uint8_t* cmd, uint32_t dataNum, const float* data);

    /**
     * @brief Clear all paths, paints, and matrices, and invalidate the view.
     */
    void Clear();

    /**
     * @brief Set transformation matrix of a path object.
     * @param matrix Indicates a transformation matrix.
     * @param index Indicates a value that relates a matrix to a path object.
     *        This is the returned value of BeginPath or any DrawGraphics method(DrawLine, DrawCurve, etc.).
     *        Default value is -1, which relates to the last created path object.
     */
    void SetMatrix(Matrix3<float> matrix, int16_t index = -1);

    /**
     * @brief Set transformation matrix of a svg object.
     * @param matrix Indicates a transformation matrix.
     * @param index Indicates a value that relates a matrix to a svg object.
     *        This is the returned value of DrawSvg.
     */
    void SetSvgMatrix(Matrix3<float> matrix, int16_t index);

    /**
     * @brief Set drawing flag to true.
     */
    void DrawPath();

    /**
     * @brief Draws svg data.
     * @param path svg bin path.
     * @param path len
     */
    int16_t DrawSvg(const char* path);

    /**
     * @brief Draws a straight line from the coordinates of the start point.
     *
     * @param startPoint Indicates the coordinates of the start point.
     * @param endPoint   Indicates the coordinates of the end point.
     * @param paint      Indicates the straight line style. For details, see {@link PaintExt}.
     * @return Returns index value that relates to an object. Returns -1 when any error occurs.
     */
    int16_t DrawLine(const Point& startPoint, const Point& endPoint, const PaintExt& paint);

    /**
     * @brief Draws a cubic Bezier curve from the start point coordinates.
     *
     * @param startPoint Indicates the coordinates of the start point of the cubic Bezier curve.
     * @param control1   Indicates the coordinates of the first control point of the cubic Bezier curve.
     * @param control2   Indicates the coordinates of the second control point of the cubic Bezier curve.
     * @param endPoint   Indicates the coordinates of the end point of the cubic Bezier curve.
     * @param paint      Indicates the curve style. For details, see {@link PaintExt}.
     * @return Returns index value that relates to an object. Returns -1 when any error occurs.
     */
    int16_t DrawCurve(const Point& startPoint, const Point& control1, const Point& control2,
        const Point& endPoint, const PaintExt& paint);

    /**
     * @brief Draws a rectangle.
     *
     * @param startPoint Indicates the coordinates of the point at the upper left corner of the rectangle.
     * @param height     Indicates the height of the rectangle.
     * @param width      Indicates the width of the rectangle.
     * @param paint      Indicates the rectangle style. For details, see {@link PaintExt}.
     * @return Returns index value that relates to an object. Returns -1 when any error occurs.
     */
    int16_t DrawRect(const Point& startPoint, int16_t height, int16_t width, const PaintExt& paint,
        uint16_t radius = 0);

   /**
     * @brief Draws a circle.
     *
     * @param center Indicates the coordinates of the circle center.
     * @param radius Indicates the radius of the circle.
     * @param paint  Indicates the circle style. For details, see {@link PaintExt}.
     * @return Returns index value that relates to an object. Returns -1 when any error occurs.
     */
    int16_t DrawCircle(const Point& center, uint16_t radius, const PaintExt& paint);

    /**
     * @brief Draws a sector.
     *
     * When the start angle is smaller than the end angle, the sector is drawn clockwise.
     * Otherwise, the sector is drawn counterclockwise.
     *
     * @param center     Indicates the coordinates of the sector's center.
     * @param radius     Indicates the radius of the sector.
     * @param startAngle Indicates the start angle of the sector. Value <b>0</b> indicates the 12-o'clock direction,
     *                   and <b>90</b> indicates the 3-o'clock direction.
     * @param endAngle   Indicates the end angle of the sector. Value <b>0</b> indicates the 12-o'clock direction,
     *                   and <b>90</b> indicates the 3-o'clock direction.
     * @param paint      Indicates the sector style. For details, see {@link PaintExt}.
     * @return Returns index value that relates to an object. Returns -1 when any error occurs.
     */
    int16_t DrawSector(const Point& center, uint16_t radius,
        float startAngle, float endAngle, const PaintExt& paint);

    /**
     * @brief Draws an arc.
     *
     * When the start angle is smaller than the end angle, the sector is drawn clockwise.
     * Otherwise, the sector is drawn counterclockwise.
     *
     * @param center     Indicates the coordinates of the arc's center.
     * @param radius     Indicates the radius of the arc.
     * @param startAngle Indicates the start angle of the arc. Value <b>0</b> indicates the 12-o'clock direction,
     *                   and <b>90</b> indicates the 3-o'clock direction.
     * @param endAngle   Indicates the end angle of the arc. Value <b>0</b> indicates the 12-o'clock direction,
     *                   and <b>90</b> indicates the 3-o'clock direction.
     * @param paint      Indicates the arc style. For details, see {@link PaintExt}.
     * @return Returns index value that relates to an object. Returns -1 when any error occurs.
     */
    int16_t DrawArc(const Point& center, uint16_t radius,
        float startAngle, float endAngle, const PaintExt& paint);

    /**
     * @brief Draws an image.
     *
     * @param startPoint   Indicates the coordinates of the point at the upper left corner of the image.
     * @param image        Indicates the image bin path.
     * @param paint        Indicates the image style. For details, see {@link PaintExt}.
     * @return Returns index value that relates to an object. Returns -1 when any error occurs.
     */
    int16_t DrawImage(const Point& startPoint, const char* image, const PaintExt& paint);

    /**
     * @brief Defines the font style.
     */
    struct FontStyle {
        /** Text direction. For details, see {@link UITextLanguageDirect}. */
        UITextLanguageDirect direct;
        /** Text alignment mode. For details, see {@link UITextLanguageAlignment}. */
        UITextLanguageAlignment align;
        /** Font size */
        uint8_t fontSize;
        /** Letter-spacing */
        int16_t letterSpace;
        /** Font name */
        const char* fontName;
    };

    /**
     * @brief Draws text.
     *
     * @param startPoint Indicates the coordinates of the start point.
     * @param text       Indicates the pointer to the text content.
     * @param maxWidth   Indicates the maximum width of the text that can be displayed. If the maximum width is
     *                   exceeded, the text is truncated.
     * @param fontStyle  Indicates the text layout and font style. For details, see {@link FontStyle}.
     * @param paint      Indicates the text style. For details, see {@link Paint}.
     * @return Returns index value that relates to an object. Returns -1 when any error occurs.
     */
    int16_t DrawLabel(const Point& startPoint, const char* text, uint16_t maxWidth, const FontStyle& fontStyle,
        const PaintExt& paint);

    /**
     * @brief Callback for root view to draw the current content.
     *
     * @param gfxDstBuffer Gfx dest buffer to be rendered.
     * @param invalidatedArea Rect to be draw.
     */
    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

    void ReMeasure() override;

private:
    struct DrawCmd : public HeapBase {
        PaintExt* paint;
        void* param;
        uint16_t index;
        void(UICanvasExt::*DrawGraphics)(BufferInfo&, void*, PaintExt*, const Rect&, const Rect&, Matrix3<float>&);
        void(UICanvasExt::*DeleteParam)(void *);
    };

    List<DrawCmd> drawCmdList_;
    Path* curPath_;
    Graphic::Vector<SvgData*> svgObjList_;
    Graphic::Vector<SvgPath> svgPaths_;
    Graphic::Vector<Matrix3<float>> matrices_;
    bool isPathDrawable_;

    void ClearSvgData();
    int16_t InitGraphicPathInner(Path* path, const PaintExt& paint);
    void AddPaintToCmd(const PaintExt& paint, DrawCmd& cmd);
    void DrawPathInner(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea,
        Path* path, PaintExt* paint, Matrix3<float>* matrix);
    void DoDrawPath(BufferInfo& gfxDstBuffer, void* param, PaintExt* paint, const Rect& rect,
        const Rect& invalidatedArea, Matrix3<float>& matrix);
    void DoDrawImage(BufferInfo& gfxDstBuffer, void* param, PaintExt* paint, const Rect& rect,
        const Rect& invalidatedArea, Matrix3<float>& matrix);
    void DoDrawLabel(BufferInfo& gfxDstBuffer, void* param, PaintExt* paint, const Rect& rect,
        const Rect& invalidatedArea, Matrix3<float>& matrix);
    void DeletePath(void* param);
    void DeleteImageView(void* param);
    void DeleteLabel(void* param);

    SvgData* GetSvgData(const char* path);
    void PaintSetGradient(SvgData* svg, int16_t gradientIndex, PaintExt& paint, uint16_t index);
    void PaintSetClipPath(SvgData* svg, int16_t clipPathIndex, PaintExt& paint);
    ColorStop* OverlayOpacity(SvgData* svg, uint16_t stopIndex, uint16_t stopCount, uint16_t index);
    void PaintSetParam(SvgData* svg, PaintExt& paint, uint16_t index);
};
#endif
}
#endif
/**
 * @}
 */
