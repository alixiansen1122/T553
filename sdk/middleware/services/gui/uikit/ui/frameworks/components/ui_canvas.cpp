/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "components/ui_canvas.h"
#include "common/image.h"
#include "draw/draw_arc.h"
#include "draw/draw_image.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "gfx_utils/graphic_log.h"
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
#include "common/graphic_hardware_types.h"
#include "draw/hw_draw_utils.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "hals/gralloc_engines.h"
#endif
namespace OHOS {
void Paint::Scale(float scaleX, float scaleY)
{
    this->scaleRadioX_ *= scaleX;
    this->scaleRadioY_ *= scaleX;
    if (rotateAngle_ > 0.0f || rotateAngle_ < 0) {
        transfrom_.Rotate(-rotateAngle_ * UI_PI / BOXER);
        transfrom_.Scale(scaleX, scaleY);
        transfrom_.Rotate(rotateAngle_ * UI_PI / BOXER);
    } else {
        transfrom_.Scale(scaleX, scaleY);
    }
}

void Paint::Rotate(float angle)
{
    transfrom_.Rotate(angle * UI_PI / BOXER);
    rotateAngle_ += angle;
}

void Paint::Rotate(float angle, int16_t x, int16_t y)
{
    transfrom_.Translate(-x, -y);
    transfrom_.Rotate(angle * UI_PI / BOXER);
    rotateAngle_ += angle;
    transfrom_.Translate(x, y);
}

void Paint::Translate(int16_t x, int16_t y)
{
    transfrom_.Translate(x, y);
    this->translationX_ += x;
    this->translationY_ += y;
}

void Paint::SetTransform(float scaleX, float shearX, float shearY,
                         float scaleY, int16_t transLateX, int16_t transLateY)
{
    transfrom_.Reset();
    rotateAngle_ = 0;
    Transform(scaleX, shearX, shearY, scaleY, transLateX, transLateY);
}

void Paint::Transform(float scaleX, float shearX, float shearY, float scaleY, int16_t transLateX, int16_t transLateY)
{
    this->translationX_ += transLateX;
    this->translationY_ += transLateY;
    transLateX += transfrom_.GetData()[2]; // 2: index
    transLateY += transfrom_.GetData()[5]; // 5: index
    transfrom_.Translate(-transfrom_.GetData()[2], -transfrom_.GetData()[5]); // 2 5: index
    Scale(scaleX, scaleY);
    transfrom_.Translate(transLateX, transLateY);
    transfrom_.SetData(1, transfrom_.GetData()[1] + shearX);
    transfrom_.SetData(3, transfrom_.GetData()[3] + shearY); // 3: index
}

UICanvas::UICanvasPath::~UICanvasPath()
{
    points_.Clear();
    cmd_.Clear();
    arcParam_.Clear();
}

void UICanvas::BeginPath()
{
    /* If the previous path is not added to the drawing linked list, it should be destroyed directly. */
    if (path_ != nullptr && path_->strokeCount_ == 0) {
        delete path_;
        path_ = nullptr;
    }

    path_ = new UICanvasPath();
    if (path_ == nullptr) {
        GRAPHIC_LOGE("new UICanvasPath fail");
        return;
    }
}

void UICanvas::MoveTo(const Point& point)
{
    if (path_ == nullptr) {
        return;
    }

    path_->startPos_ = point;
    /* If the previous command is also CMD_MOVE_TO, the previous command is overwritten. */
    if ((path_->cmd_.Size() != 0) && (path_->cmd_.Tail()->data_ == SWPathCmd::SW_CMD_MOVE_TO)) {
        path_->points_.Tail()->data_ = point;
        return;
    }
    path_->points_.PushBack(point);
    path_->cmd_.PushBack(SWPathCmd::SW_CMD_MOVE_TO);
}

void UICanvas::LineTo(const Point& point)
{
    if (path_ == nullptr) {
        return;
    }

    path_->points_.PushBack(point);
    if (path_->cmd_.Size() == 0) {
        path_->startPos_ = point;
        path_->cmd_.PushBack(SWPathCmd::SW_CMD_MOVE_TO);
    } else {
        path_->cmd_.PushBack(SWPathCmd::SW_CMD_LINE_TO);
    }
}

void UICanvas::ArcTo(const Point& center, uint16_t radius, int16_t startAngle, int16_t endAngle)
{
    if (path_ == nullptr) {
        return;
    }

    /*
     * If there is no command before SWPathCmd::SW_CMD_ARC, only the arc is drawn. If there is a command in front of
     * SWPathCmd::SW_CMD_ARC, the start point of arc must be connected to the end point of the path.
     */
    float sinma = radius * Sin(startAngle);
    float cosma = radius * Sin(QUARTER_IN_DEGREE - startAngle);
    if (path_->cmd_.Size() != 0) {
        path_->points_.PushBack({MATH_ROUND(center.x + sinma), MATH_ROUND(center.y - cosma)});
        path_->cmd_.PushBack(SWPathCmd::SW_CMD_LINE_TO);
    } else {
        path_->startPos_ = {MATH_ROUND(center.x + sinma), MATH_ROUND(center.y - cosma)};
    }

    /* If the ARC scan range exceeds 360 degrees, the end point of the path is the position of the start angle. */
    if (MATH_ABS(startAngle - endAngle) < CIRCLE_IN_DEGREE) {
        sinma = radius * Sin(endAngle);
        cosma = radius * Sin(QUARTER_IN_DEGREE - endAngle);
    }
    path_->points_.PushBack({MATH_ROUND(center.x + sinma), MATH_ROUND(center.y - cosma)});
    path_->cmd_.PushBack(SWPathCmd::SW_CMD_ARC);

    int16_t start;
    int16_t end;
    if (startAngle > endAngle) {
        start = endAngle;
        end = startAngle;
    } else {
        start = startAngle;
        end = endAngle;
    }

    DrawArc::GetInstance()->GetDrawRange(start, end);
    ArcParam param;
    param.center = center;
    param.radius = radius;
    param.startAngle = start;
    param.endAngle = end;
    path_->arcParam_.PushBack(param);
}

void UICanvas::AddRect(const Point& point, int16_t height, int16_t width)
{
    if (path_ == nullptr) {
        return;
    }

    MoveTo(point);
    LineTo({static_cast<int16_t>(point.x + width), point.y});
    LineTo({static_cast<int16_t>(point.x + width), static_cast<int16_t>(point.y + height)});
    LineTo({point.x, static_cast<int16_t>(point.y + height)});
    ClosePath();
}

void UICanvas::ClosePath()
{
    if ((path_ == nullptr) || (path_->cmd_.Size() == 0)) {
        return;
    }

    path_->points_.PushBack(path_->startPos_);
    path_->cmd_.PushBack(SWPathCmd::SW_CMD_CLOSE);
}

UICanvas::~UICanvas()
{
    if ((path_ != nullptr) && (path_->strokeCount_ == 0)) {
        delete path_;
        path_ = nullptr;
    }

    void* param = nullptr;
    ListNode<DrawCmd>* curDraw = drawCmdList_.Begin();
    for (; curDraw != drawCmdList_.End(); curDraw = curDraw->next_) {
        param = curDraw->data_.param;
        curDraw->data_.DeleteParam(param);
        curDraw->data_.param = nullptr;
    }
    drawCmdList_.Clear();
}

void UICanvas::Clear()
{
    if ((path_ != nullptr) && (path_->strokeCount_ == 0)) {
        delete path_;
        path_ = nullptr;
    }

    void* param = nullptr;
    ListNode<DrawCmd>* curDraw = drawCmdList_.Begin();
    for (; curDraw != drawCmdList_.End(); curDraw = curDraw->next_) {
        param = curDraw->data_.param;
        curDraw->data_.DeleteParam(param);
        curDraw->data_.param = nullptr;
    }
    drawCmdList_.Clear();
    Invalidate();
}

void UICanvas::DrawLine(const Point& endPoint, const Paint& paint)
{
    DrawLine(startPoint_, endPoint, paint);
}

void UICanvas::DrawLine(const Point& startPoint, const Point& endPoint, const Paint& paint)
{
    LineParam* lineParam = new LineParam;
    if (lineParam == nullptr) {
        GRAPHIC_LOGE("new LineParam fail");
        return;
    }
    lineParam->start = startPoint;
    lineParam->end = endPoint;

    DrawCmd cmd;
    cmd.paint = paint;
    cmd.param = lineParam;
    cmd.DeleteParam = DeleteLineParam;
    cmd.DrawGraphics = DoDrawLine;
    drawCmdList_.PushBack(cmd);

    Invalidate();
    SetStartPosition(endPoint);
}

void UICanvas::DrawCurve(const Point& control1, const Point& control2, const Point& endPoint, const Paint& paint)
{
    DrawCurve(startPoint_, control1, control2, endPoint, paint);
}

void UICanvas::DrawCurve(const Point& startPoint,
                         const Point& control1,
                         const Point& control2,
                         const Point& endPoint,
                         const Paint& paint)
{
    CurveParam* curveParam = new CurveParam;
    if (curveParam == nullptr) {
        GRAPHIC_LOGE("new CurveParam fail");
        return;
    }
    curveParam->start = startPoint;
    curveParam->control1 = control1;
    curveParam->control2 = control2;
    curveParam->end = endPoint;

    DrawCmd cmd;
    cmd.paint = paint;
    if (paint.GetStrokeWidth() > MAX_CURVE_WIDTH) {
        cmd.paint.SetStrokeWidth(MAX_CURVE_WIDTH);
    }
    cmd.param = curveParam;
    cmd.DeleteParam = DeleteCurveParam;
    cmd.DrawGraphics = DoDrawCurve;
    drawCmdList_.PushBack(cmd);

    Invalidate();
    SetStartPosition(endPoint);
}

#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
void UICanvas::DrawRectWithPath(const Point& startPoint, int16_t height, int16_t width, const Paint& paint)
{
    Path* path = new Path();
    (void)memset_s(path, sizeof(Path), 0, sizeof(Path));

    path->dataNum = 5; // 5: data num
    if (!HWDrawUtils::GetInstance()->AllocBufferForData(path->dataNum, path->data)) {
        delete path;
        return;
    }

    path->data[0] = startPoint.x;
    path->data[1] = startPoint.y;
    path->data[2] = startPoint.x + width - 1; // 2: index
    path->data[3] = startPoint.y + height - 1; // 3: index
    path->data[4] = startPoint.x; // 4: index

    path->cmdNum = 5; // 5: cmd num
    AllocInfo cmdInfo;
    cmdInfo.expectedSize = sizeof(uint8_t) * path->cmdNum;
    cmdInfo.usage = HBM_USE_ASSIGN_SIZE | HBM_USE_MEM_PATH;
    GrallocBuffer cmdBuffer;
    if (!GrallocEngines::GetInstance()->AllocBuffer(cmdInfo, cmdBuffer)) {
        GrallocEngines::GetInstance()->FreeBuffer(reinterpret_cast<uint8_t*>(path->data));
        delete path;
        GRAPHIC_LOGE("Alloc buffer for cmd failed.");
        return;
    }
    path->cmds = (uint8_t *)cmdBuffer.virAddr;
    path->cmds[0] = CMD_MOVE_TO;
    path->cmds[1] = CMD_HLINE_TO;
    path->cmds[2] = CMD_VLINE_TO; // 2: index
    path->cmds[3] = CMD_HLINE_TO; // 3: index
    path->cmds[4] = CMD_CLOSE; // 4: index

    DrawCmd cmd;
    cmd.paint = paint;
    cmd.param = path;
    cmd.DeleteParam = DeleteRectParam;
    cmd.DrawGraphics = DoDrawRect;
    drawCmdList_.PushBack(cmd);

    Invalidate();
}
#endif

void UICanvas::DeleteRectParam(void* param)
{
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
    LiteMGfxEngine::GetInstance()->SyncHwDraw();

    Path* path = static_cast<Path*>(param);
    if (path != nullptr) {
        if (path->cmds != nullptr) {
            GrallocEngines::GetInstance()->FreeBuffer(reinterpret_cast<uint8_t*>(path->cmds));
        }
        if (path->data != nullptr) {
            GrallocEngines::GetInstance()->FreeBuffer(reinterpret_cast<uint8_t*>(path->data));
        }
        delete path;
    }
#else
    RectParam* rectParam = static_cast<RectParam*>(param);
    delete rectParam;
#endif
}

void UICanvas::DrawRect(const Point& startPoint, int16_t height, int16_t width, const Paint& paint)
{
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
    return DrawRectWithPath(startPoint, height, width, paint);
#endif
    if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::STROKE_STYLE) {
        RectParam* rectParam = new RectParam;
        if (rectParam == nullptr) {
            GRAPHIC_LOGE("new RectParam fail");
            return;
        }
        rectParam->start = startPoint;
        rectParam->height = height;
        rectParam->width = width;

        DrawCmd cmd;
        cmd.paint = paint;
        cmd.param = rectParam;
        cmd.DeleteParam = DeleteRectParam;
        cmd.DrawGraphics = DoDrawRect;
        drawCmdList_.PushBack(cmd);
    }

    if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::FILL_STYLE) {
        RectParam* rectParam = new RectParam;
        if (rectParam == nullptr) {
            GRAPHIC_LOGE("new RectParam fail");
            return;
        }
        rectParam->start = startPoint;
        rectParam->height = height;
        rectParam->width = width;

        DrawCmd cmd;
        cmd.paint = paint;
        cmd.param = rectParam;
        cmd.DeleteParam = DeleteRectParam;
        cmd.DrawGraphics = DoFillRect;
        drawCmdList_.PushBack(cmd);
    }

    Invalidate();
}

void UICanvas::DrawCircle(const Point& center, uint16_t radius, const Paint& paint)
{
    CircleParam* circleParam = new CircleParam;
    if (circleParam == nullptr) {
        GRAPHIC_LOGE("new CircleParam fail");
        return;
    }
    circleParam->center = center;
    circleParam->radius = radius;

    DrawCmd cmd;
    cmd.paint = paint;
    cmd.param = circleParam;
    cmd.DeleteParam = DeleteCircleParam;
    cmd.DrawGraphics = DoDrawCircle;
    drawCmdList_.PushBack(cmd);

    Invalidate();
}

void UICanvas::DrawSector(const Point& center,
                          uint16_t radius,
                          int16_t startAngle,
                          int16_t endAngle,
                          const Paint& paint)
{
    if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::FILL_STYLE) {
        Paint innerPaint = paint;
        innerPaint.SetStyle(Paint::PaintStyle::STROKE_STYLE);
        innerPaint.SetStrokeWidth(radius);
        innerPaint.SetStrokeColor(paint.GetFillColor());
        radius >>= 1;
        DrawArc(center, radius, startAngle, endAngle, innerPaint);
    }
}

void UICanvas::DrawArc(const Point& center, uint16_t radius, int16_t startAngle, int16_t endAngle, const Paint& paint)
{
    if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::STROKE_STYLE) {
        ArcParam* arcParam = new ArcParam;
        if (arcParam == nullptr) {
            GRAPHIC_LOGE("new ArcParam fail");
            return;
        }
        arcParam->center = center;
        arcParam->radius = radius;

        int16_t start;
        int16_t end;
        if (startAngle > endAngle) {
            start = endAngle;
            end = startAngle;
        } else {
            start = startAngle;
            end = endAngle;
        }

        DrawArc::GetInstance()->GetDrawRange(start, end);
        arcParam->startAngle = start;
        arcParam->endAngle = end;

        DrawCmd cmd;
        cmd.paint = paint;
        cmd.param = arcParam;
        cmd.DeleteParam = DeleteArcParam;
        cmd.DrawGraphics = DoDrawArc;
        drawCmdList_.PushBack(cmd);

        Invalidate();
    }
}

void UICanvas::DrawLabel(const Point& startPoint,
                         const char* text,
                         uint16_t maxWidth,
                         const FontStyle& fontStyle,
                         const Paint& paint)
{
    if (text == nullptr) {
        return;
    }
    if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::FILL_STYLE) {
        UILabel* label = new UILabel();
        if (label == nullptr) {
            GRAPHIC_LOGE("new UILabel fail");
            return;
        }
        label->SetLineBreakMode(UILabel::LINE_BREAK_CLIP);
        label->SetPosition(startPoint.x, startPoint.y);
        label->SetWidth(maxWidth);
        label->SetHeight(GetHeight());
        label->SetText(text);
        label->SetFont(fontStyle.fontName, fontStyle.fontSize);
        label->SetAlign(fontStyle.align);
        label->SetDirect(fontStyle.direct);
        label->SetStyle(STYLE_LETTER_SPACE, fontStyle.letterSpace);
        label->SetStyle(STYLE_TEXT_COLOR, paint.GetFillColor().full);
        label->SetStyle(STYLE_TEXT_OPA, paint.GetOpacity());

        DrawCmd cmd;
        cmd.param = label;
        cmd.DeleteParam = DeleteLabel;
        cmd.DrawGraphics = DoDrawLabel;
        drawCmdList_.PushBack(cmd);

        Invalidate();
    }
}

void UICanvas::DrawImage(const Point& startPoint, const char* image, const Paint& paint)
{
    if (image == nullptr) {
        return;
    }

    if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::FILL_STYLE) {
        UIImageView* imageView = new UIImageView();
        if (imageView == nullptr) {
            GRAPHIC_LOGE("new UIImageView fail");
            return;
        }
        imageView->SetPosition(startPoint.x, startPoint.y);
        imageView->SetSrc(image);

        DrawCmd cmd;
        cmd.paint = paint;
        cmd.param = imageView;
        cmd.DeleteParam = DeleteImageView;
        cmd.DrawGraphics = DoDrawImage;
        drawCmdList_.PushBack(cmd);

        Invalidate();
        SetStartPosition(startPoint);
    }
}

void UICanvas::DrawImage(const Point& startPoint, const char* image,
                         const Paint& paint, int16_t width, int16_t height)
{
    if (image == nullptr) {
        return;
    }
    if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::FILL_STYLE) {
        UIImageView* imageView = new UIImageView();
        if (imageView == nullptr) {
            GRAPHIC_LOGE("new UIImageView fail");
            return;
        }
        imageView->SetPosition(startPoint.x, startPoint.y);
        imageView->SetSrc(image);
        float scaleX = 1.0;
        float scaleY = 1.0;
        if (width > 0 && imageView->GetWidth() > 0) {
            scaleX = (float)width / (float)imageView->GetWidth();
        }
        if (height > 0 && imageView->GetHeight() > 0) {
            scaleY = (float)height / (float)imageView->GetHeight();
        }
        DrawCmd cmd;
        cmd.paint = paint;
        cmd.paint.Scale(scaleX, scaleY);
        cmd.param = imageView;
        cmd.DeleteParam = DeleteImageView;
        cmd.DrawGraphics = DoDrawImage;
        drawCmdList_.PushBack(cmd);

        Invalidate();
        SetStartPosition(startPoint);
    }

    Invalidate();
    SetStartPosition(startPoint);
}

void UICanvas::DrawPath(const Paint& paint)
{
    if ((path_ == nullptr) || (path_->cmd_.Size() == 0)) {
        return;
    }

    path_->strokeCount_++;
    PathParam* param = new PathParam;
    if (param == nullptr) {
        GRAPHIC_LOGE("new PathParam fail");
        return;
    }
    param->path = path_;
    param->count = path_->cmd_.Size();

    DrawCmd cmd;
    cmd.paint = paint;
    cmd.param = param;
    cmd.DeleteParam = DeletePathParam;
    cmd.DrawGraphics = DoDrawPath;
    drawCmdList_.PushBack(cmd);
    Invalidate();
}

void UICanvas::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    Rect rect = GetOrigRect();
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, rect, invalidatedArea, *style_, opaScale_);

    void* param = nullptr;
    ListNode<DrawCmd>* curDraw = drawCmdList_.Begin();
    Rect coords = GetOrigRect();
    Rect trunc = invalidatedArea;
    if (trunc.Intersect(trunc, coords)) {
        for (; curDraw != drawCmdList_.End(); curDraw = curDraw->next_) {
            param = curDraw->data_.param;
            curDraw->data_.DrawGraphics(gfxDstBuffer, param, curDraw->data_.paint, rect, trunc, *style_);
        }
    }
}

void UICanvas::GetAbsolutePosition(const Point& prePoint, const Rect& rect, const Style& style, Point& point)
{
    point.x = prePoint.x + rect.GetLeft() + style.paddingLeft_ + style.borderWidth_;
    point.y = prePoint.y + rect.GetTop() + style.paddingTop_ + style.borderWidth_;
}

void UICanvas::DoDrawLine(BufferInfo& gfxDstBuffer,
                          void* param,
                          const Paint& paint,
                          const Rect& rect,
                          const Rect& invalidatedArea,
                          const Style& style)
{
    if (param == nullptr) {
        return;
    }
    LineParam* lineParam = static_cast<LineParam*>(param);
    Point start;
    Point end;
    GetAbsolutePosition(lineParam->start, rect, style, start);
    GetAbsolutePosition(lineParam->end, rect, style, end);

    BaseGfxEngine::GetInstance()->DrawLine(gfxDstBuffer, start, end, invalidatedArea, paint.GetStrokeWidth(),
                                           paint.GetStrokeColor(), paint.GetOpacity());
}

void UICanvas::DoDrawCurve(BufferInfo& gfxDstBuffer,
                           void* param,
                           const Paint& paint,
                           const Rect& rect,
                           const Rect& invalidatedArea,
                           const Style& style)
{
    if (param == nullptr) {
        return;
    }
    CurveParam* curveParam = static_cast<CurveParam*>(param);
    Point start;
    Point end;
    Point control1;
    Point control2;
    GetAbsolutePosition(curveParam->start, rect, style, start);
    GetAbsolutePosition(curveParam->end, rect, style, end);
    GetAbsolutePosition(curveParam->control1, rect, style, control1);
    GetAbsolutePosition(curveParam->control2, rect, style, control2);

    BaseGfxEngine::GetInstance()->DrawCubicBezier(gfxDstBuffer, start, control1, control2, end, invalidatedArea,
                                                  paint.GetStrokeWidth(), paint.GetStrokeColor(), paint.GetOpacity());
}

#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
static void ConvertMatrix(const Matrix3<float>& src, VGUMatrix3& dst)
{
    memset_s(&dst, sizeof(VGUMatrix3), 0, sizeof(VGUMatrix3));
    const float* data = src.GetData();
    dst.m[0][0] = data[0]; // 0, 0: index
    dst.m[0][1] = data[1]; // 0, 1: index
    dst.m[0][2] = data[2]; // 0, 2, 2: index
    dst.m[1][0] = data[3]; // 1, 3: index
    dst.m[1][1] = data[4]; // 1, 4: index
    dst.m[1][2] = data[5]; // 1, 2, 5: index
    dst.m[2][0] = data[6]; // 2, 6: index
    dst.m[2][1] = data[7]; // 2, 1, 7: index
    dst.m[2][2] = data[8]; // 2, 8: index
    return;
}

static void HandleStyleAndGradient(VGUPaintStyle& style, const Paint* paint, uint32_t color)
{
    uint8_t alpha = ((color >> 24) * paint->GetOpacity()) / OPA_OPAQUE; // 24: alpha shift
    uint32_t mixedColor = (alpha << 24) | (color & 0x00FFFFFF); // 24: alpha shift
    style.color = mixedColor;
    style.type = VGU_PAINT_COLOR;
}

void UICanvas::DoDrawRectWithPath(BufferInfo& gfxDstBuffer,
                          void* param,
                          const Paint& paint,
                          const Rect& rect,
                          const Rect& invalidatedArea,
                          const Style& style)
{
    if (param == nullptr) {
        return;
    }
    Path* path = static_cast<Path*>(param);

    VGUMatrix3 mat;
    auto paintMatrix = static_cast<Matrix3<float>>(paint.GetTransAffine());
    Matrix3<float> translateMatrix(1, 0, rect.GetX(), 0, 1, rect.GetY(), 0, 0, 1);
    paintMatrix = paintMatrix * translateMatrix;
    ConvertMatrix(paintMatrix, mat);

    uintptr_t vguPath = LiteMGfxEngine::GetInstance()->InitPath(true);
    LiteMGfxEngine::GetInstance()->SetPathData(vguPath, path->cmdNum, path->cmds, path->dataNum, path->data);
    if (paint.GetStyle() == Paint::PaintStyle::FILL_STYLE) {
        VGUFillAttr fillAttr;
        fillAttr.rule = VGU_RULE_WINDING;
        VGUPaintStyle style = {{0}};
        HandleStyleAndGradient(style, &paint, paint.GetFillColor().full);
        fillAttr.style = &style;
        LiteMGfxEngine::GetInstance()->RenderPath(gfxDstBuffer, vguPath, invalidatedArea, nullptr, &fillAttr, &mat);
    } else {
        VGUStrokeAttr strokeAttr = {VGU_LINECAP_BUTT, VGU_LINE_JOIN_MITER, 2, paint.GetStrokeWidth()}; // 2:miter limit
        VGUPaintStyle strokeStyle = {{0}};
        strokeAttr.style = &strokeStyle;
        HandleStyleAndGradient(strokeStyle, &paint, paint.GetStrokeColor().full);
        if (paint.GetStyle() == Paint::PaintStyle::STROKE_STYLE) {
            LiteMGfxEngine::GetInstance()->RenderPath(gfxDstBuffer, vguPath, invalidatedArea, &strokeAttr, nullptr, &mat);
        } else {
            VGUFillAttr fillAttr;
            fillAttr.rule = VGU_RULE_WINDING;
            VGUPaintStyle fillStyle = {{0}};
            HandleStyleAndGradient(fillStyle, &paint, paint.GetFillColor().full);
            fillAttr.style = &fillStyle;
            LiteMGfxEngine::GetInstance()->RenderPath(gfxDstBuffer, vguPath,
                invalidatedArea, &strokeAttr, &fillAttr, &mat);
        }
    }
}
#endif

void UICanvas::DoDrawRect(BufferInfo& gfxDstBuffer,
                          void* param,
                          const Paint& paint,
                          const Rect& rect,
                          const Rect& invalidatedArea,
                          const Style& style)
{
    if (param == nullptr) {
        return;
    }
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
    return DoDrawRectWithPath(gfxDstBuffer, param, paint, rect, invalidatedArea, style);
#endif
    RectParam* rectParam = static_cast<RectParam*>(param);
    Style drawStyle = StyleDefault::GetDefaultStyle();
    drawStyle.bgColor_ = paint.GetStrokeColor();
    drawStyle.bgOpa_ = paint.GetOpacity();
    drawStyle.borderRadius_ = 0;

    int16_t lineWidth = static_cast<int16_t>(paint.GetStrokeWidth());
    Point start;
    GetAbsolutePosition(rectParam->start, rect, style, start);

    int16_t x = start.x - lineWidth / 2; // 2: half
    int16_t y = start.y - lineWidth / 2; // 2: half
    Rect coords;
    if ((rectParam->height <= lineWidth) || (rectParam->width <= lineWidth)) {
        coords.SetPosition(x, y);
        coords.SetHeight(rectParam->height + lineWidth);
        coords.SetWidth(rectParam->width + lineWidth);
        BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, coords, invalidatedArea, drawStyle, OPA_OPAQUE);
        return;
    }

    coords.SetPosition(x, y);
    coords.SetHeight(lineWidth);
    coords.SetWidth(rectParam->width);
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, coords, invalidatedArea, drawStyle, OPA_OPAQUE);

    coords.SetPosition(x + rectParam->width, y);
    coords.SetHeight(rectParam->height);
    coords.SetWidth(lineWidth);
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, coords, invalidatedArea, drawStyle, OPA_OPAQUE);

    coords.SetPosition(x, y + lineWidth);
    coords.SetHeight(rectParam->height);
    coords.SetWidth(lineWidth);
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, coords, invalidatedArea, drawStyle, OPA_OPAQUE);

    coords.SetPosition(x + lineWidth, y + rectParam->height);
    coords.SetHeight(lineWidth);
    coords.SetWidth(rectParam->width);
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, coords, invalidatedArea, drawStyle, OPA_OPAQUE);
}

void UICanvas::DoFillRect(BufferInfo& gfxDstBuffer,
                          void* param,
                          const Paint& paint,
                          const Rect& rect,
                          const Rect& invalidatedArea,
                          const Style& style)
{
    if (param == nullptr) {
        return;
    }
    RectParam* rectParam = static_cast<RectParam*>(param);
    uint8_t enableStroke = static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::STROKE_STYLE;
    int16_t lineWidth = enableStroke ? paint.GetStrokeWidth() : 0;
    if ((rectParam->height <= lineWidth) || (rectParam->width <= lineWidth)) {
        return;
    }
    Point start;
    GetAbsolutePosition(rectParam->start, rect, style, start);

    Rect coords;
    coords.SetPosition(start.x + (lineWidth + 1) / 2, start.y + (lineWidth + 1) / 2); // 2: half
    coords.SetHeight(rectParam->height - lineWidth);
    coords.SetWidth(rectParam->width - lineWidth);

    Style drawStyle = StyleDefault::GetDefaultStyle();
    drawStyle.bgColor_ = paint.GetFillColor();
    drawStyle.bgOpa_ = paint.GetOpacity();
    drawStyle.borderRadius_ = 0;
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, coords, invalidatedArea, drawStyle, OPA_OPAQUE);
}

void UICanvas::DoDrawCircle(BufferInfo& gfxDstBuffer,
                            void* param,
                            const Paint& paint,
                            const Rect& rect,
                            const Rect& invalidatedArea,
                            const Style& style)
{
    if (param == nullptr) {
        return;
    }
    CircleParam* circleParam = static_cast<CircleParam*>(param);

    Style drawStyle = StyleDefault::GetDefaultStyle();
    drawStyle.lineOpa_ = paint.GetOpacity();

    ArcInfo arcInfo = {{0}};
    arcInfo.imgPos = Point{0, 0};
    arcInfo.startAngle = 0;
    arcInfo.endAngle = CIRCLE_IN_DEGREE;
    GetAbsolutePosition(circleParam->center, rect, style, arcInfo.center);
    uint8_t enableStroke = static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::STROKE_STYLE;
    uint16_t halfLineWidth = enableStroke ? (paint.GetStrokeWidth() >> 1) : 0;
    if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::FILL_STYLE) {
        arcInfo.radius = circleParam->radius - halfLineWidth;
        drawStyle.lineWidth_ = arcInfo.radius;
        drawStyle.lineColor_ = paint.GetFillColor();
        BaseGfxEngine::GetInstance()->DrawArc(gfxDstBuffer, arcInfo, invalidatedArea, drawStyle, OPA_OPAQUE,
                                              CapType::CAP_NONE);
    }

    if (enableStroke) {
        arcInfo.radius = circleParam->radius + halfLineWidth - 1;
        drawStyle.lineWidth_ = static_cast<int16_t>(paint.GetStrokeWidth());
        drawStyle.lineColor_ = paint.GetStrokeColor();
        BaseGfxEngine::GetInstance()->DrawArc(gfxDstBuffer, arcInfo, invalidatedArea, drawStyle, OPA_OPAQUE,
                                              CapType::CAP_NONE);
    }
}

void UICanvas::DoDrawArc(BufferInfo& gfxDstBuffer,
                         void* param,
                         const Paint& paint,
                         const Rect& rect,
                         const Rect& invalidatedArea,
                         const Style& style)
{
    if (param == nullptr) {
        return;
    }
    ArcParam* arcParam = static_cast<ArcParam*>(param);

    ArcInfo arcInfo = {{0}};
    arcInfo.imgPos = Point{0, 0};
    arcInfo.startAngle = arcParam->startAngle;
    arcInfo.endAngle = arcParam->endAngle;
    Style drawStyle = StyleDefault::GetDefaultStyle();
    drawStyle.lineWidth_ = static_cast<int16_t>(paint.GetStrokeWidth());
    drawStyle.lineColor_ = paint.GetStrokeColor();
    drawStyle.lineOpa_ = paint.GetOpacity();
    arcInfo.radius = arcParam->radius + ((paint.GetStrokeWidth() + 1) >> 1);

    GetAbsolutePosition(arcParam->center, rect, style, arcInfo.center);
    BaseGfxEngine::GetInstance()->DrawArc(gfxDstBuffer, arcInfo, invalidatedArea, drawStyle, OPA_OPAQUE,
                                          CapType::CAP_NONE);
}

void UICanvas::DoDrawImage(BufferInfo& gfxDstBuffer,
                           void* param,
                           const Paint& paint,
                           const Rect& rect,
                           const Rect& invalidatedArea,
                           const Style& style)
{
    if (param == nullptr) {
        return;
    }
    UIImageView* imageView = static_cast<UIImageView*>(param);
    Point startPos = {imageView->GetX(), imageView->GetY()};
    Point start;
    GetAbsolutePosition({startPos.x, startPos.y}, rect, style, start);
    imageView->SetPosition(start.x, start.y);
    if (!paint.GetTransAffine().IsIdentity()) {
        float angle = paint.GetRotateAngle();
        imageView->Rotate(MATH_ROUND(angle), Vector2<float>(0, 0));
        imageView->Translate(Vector3<int16_t>(paint.GetTranslateX(), paint.GetTranslateY(), 1));
        Vector2<float> scale(static_cast<float>(paint.GetScaleX()), static_cast<float>(paint.GetScaleY()));
        imageView->Scale(scale, Vector2<float>(0, 0));
        TransAffine affine = paint.GetTransAffine();
        imageView->Shear(Vector2<float>(affine.GetData()[1], 0), Vector2<float>(affine.GetData()[3], 0),
            Vector2<float>(0, 0));
    }
    imageView->OnDraw(gfxDstBuffer, invalidatedArea);
    imageView->SetPosition(startPos.x, startPos.y);
}

void UICanvas::DoDrawLabel(BufferInfo& gfxDstBuffer,
                           void* param,
                           const Paint& paint,
                           const Rect& rect,
                           const Rect& invalidatedArea,
                           const Style& style)
{
    if (param == nullptr) {
        return;
    }
    UILabel* label = static_cast<UILabel*>(param);
    Point startPos = {label->GetX(), label->GetY()};
    Point start;
    GetAbsolutePosition({startPos.x, startPos.y}, rect, style, start);
    label->SetPosition(start.x, start.y);
    label->OnDraw(gfxDstBuffer, invalidatedArea);
    label->SetPosition(startPos.x, startPos.y);
}

void UICanvas::DoDrawLineJoin(BufferInfo& gfxDstBuffer,
                              const Point& center,
                              const Rect& invalidatedArea,
                              const Paint& paint)
{
    ArcInfo arcinfo = {{0}};
    arcinfo.center = center;
    arcinfo.imgPos = Point{0, 0};
    arcinfo.radius = (paint.GetStrokeWidth() + 1) >> 1;
    arcinfo.startAngle = 0;
    arcinfo.endAngle = CIRCLE_IN_DEGREE;

    Style style;
    style.lineColor_ = paint.GetStrokeColor();
    style.lineWidth_ = static_cast<int16_t>(paint.GetStrokeWidth());
    style.lineOpa_ = OPA_OPAQUE;
    BaseGfxEngine::GetInstance()->DrawArc(gfxDstBuffer, arcinfo, invalidatedArea, style, OPA_OPAQUE,
                                          CapType::CAP_NONE);
}

void UICanvas::DoDrawPath(BufferInfo& gfxDstBuffer,
                          void* param,
                          const Paint& paint,
                          const Rect& rect,
                          const Rect& invalidatedArea,
                          const Style& style)
{
    if (param == nullptr) {
        return;
    }
    PathParam* pathParam = static_cast<PathParam*>(param);
    const UICanvasPath* path = pathParam->path;
    if (path == nullptr) {
        return;
    }
    Point pathEnd = {COORD_MIN, COORD_MIN};

    ListNode<Point>* pointIter = path->points_.Begin();
    ListNode<ArcParam>* arcIter = path->arcParam_.Begin();
    ListNode<SWPathCmd>* iter = path->cmd_.Begin();
    for (uint16_t i = 0; (i < pathParam->count) && (iter != path->cmd_.End()); i++, iter = iter->next_) {
        switch (iter->data_) {
            case SWPathCmd::SW_CMD_MOVE_TO: {
                pointIter = pointIter->next_;
                break;
            }
            case SWPathCmd::SW_CMD_LINE_TO: {
                Point start = pointIter->prev_->data_;
                Point end = pointIter->data_;
                pointIter = pointIter->next_;
                if ((start.x == end.x) && (start.y == end.y)) {
                    break;
                }

                GetAbsolutePosition(start, rect, style, start);
                GetAbsolutePosition(end, rect, style, end);
                BaseGfxEngine::GetInstance()->DrawLine(gfxDstBuffer, start, end, invalidatedArea,
                                                       paint.GetStrokeWidth(), paint.GetStrokeColor(), OPA_OPAQUE);
                if ((pathEnd.x == start.x) && (pathEnd.y == start.y)) {
                    DoDrawLineJoin(gfxDstBuffer, start, invalidatedArea, paint);
                }
                pathEnd = end;
                break;
            }
            case SWPathCmd::SW_CMD_ARC: {
                ArcInfo arcInfo = {{0}};
                arcInfo.imgPos = Point{0, 0};
                arcInfo.startAngle = arcIter->data_.startAngle;
                arcInfo.endAngle = arcIter->data_.endAngle;
                Style drawStyle = StyleDefault::GetDefaultStyle();
                drawStyle.lineWidth_ = static_cast<int16_t>(paint.GetStrokeWidth());
                drawStyle.lineColor_ = paint.GetStrokeColor();
                drawStyle.lineOpa_ = OPA_OPAQUE;
                arcInfo.radius = arcIter->data_.radius + ((paint.GetStrokeWidth() + 1) >> 1);

                GetAbsolutePosition(arcIter->data_.center, rect, style, arcInfo.center);
                BaseGfxEngine::GetInstance()->DrawArc(gfxDstBuffer, arcInfo, invalidatedArea, drawStyle, OPA_OPAQUE,
                                                      CapType::CAP_NONE);
                if (pointIter != path->points_.Begin()) {
                    DoDrawLineJoin(gfxDstBuffer, pathEnd, invalidatedArea, paint);
                }

                GetAbsolutePosition(pointIter->data_, rect, style, pathEnd);
                pointIter = pointIter->next_;
                arcIter = arcIter->next_;
                break;
            }
            case SWPathCmd::SW_CMD_CLOSE: {
                Point start = pointIter->prev_->data_;
                Point end = pointIter->data_;
                GetAbsolutePosition(start, rect, style, start);
                GetAbsolutePosition(end, rect, style, end);
                if ((start.x != end.x) || (start.y != end.y)) {
                    BaseGfxEngine::GetInstance()->DrawLine(gfxDstBuffer, start, end, invalidatedArea,
                                                           paint.GetStrokeWidth(), paint.GetStrokeColor(), OPA_OPAQUE);
                    if ((pathEnd.x == start.x) && (pathEnd.y == start.y)) {
                        DoDrawLineJoin(gfxDstBuffer, start, invalidatedArea, paint);
                    }
                    pathEnd = end;
                }

                if ((pathEnd.x == end.x) && (pathEnd.y == end.y)) {
                    DoDrawLineJoin(gfxDstBuffer, end, invalidatedArea, paint);
                }
                pointIter = pointIter->next_;
                break;
            }
            default:
                break;
        }
    }
}
} // namespace OHOS
