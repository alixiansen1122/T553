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

#include "ui_test_canvas_ext.h"

namespace OHOS {
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
const static int16_t WIDTH_400 = 400;
const static int16_t HEIGHT_800 = 800;
const static int16_t HEIGHT_400 = 400;
const static int16_t X_300 = 300;
const static int16_t WIDTH_100 = 100;
const static int16_t HEIGHT_50 = 50;

void SectorAnimatorCallback::Callback(UIView *view)
{
    UICanvasExt* canvas = dynamic_cast<UICanvasExt*>(view);
    if (canvas == nullptr) {
        GRAPHIC_LOGE("callback view is not UICanvasExt");
        return;
    }
    canvas->Clear();
    sectorAngle_ += angleStep_;
    sectorRange_ += rangeStep_;
    if (FloatMore(sectorRange_, CIRCLE_IN_DEGREE)) {
        sectorRange_ = 1;
    }
    if (FloatMore(sectorAngle_, 3600.f)) { // 3600.f: to avoid float overflow
        sectorAngle_ = -3600.0f; // 3600.f: to avoid float overflow
    }
    // 300: x coordinates, 250: y coordinates, 10 ahead degree to avoid boundary problem
    SweepGradient sweep = {200, 250, static_cast<int16_t>(sectorAngle_ - 10)};
    PaintExt paint;
    ColorStop stops[2] = {{0, 0x00FFFF00}, {1, 0xFFFFFF00}};
    paint.SetStyle(Paint::PaintStyle::FILL_STYLE);
    paint.SetSweepGradient(sweep, stops, 2); // 2: sweep stops count
    paint.SetAntialiased(true);

    // 300, 250, 130, sector parameters: x , y, radius
    canvas->DrawSector({200, 250}, 130, sectorAngle_, sectorAngle_ + sectorRange_, paint);
}

void UITestCanvasExt::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetHorizontalScrollState(false);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    }

    if (canvas_ == nullptr) {
        canvas_ = new UICanvasExt();
    }
    canvas_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), HEIGHT_800);
    canvas_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    canvas_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    container_->Add(canvas_);

    SetUpBtn(addSquareBtn_, "Add Square", X_300, 0); // 0: Y coordinate
    SetUpBtn(addSectorBtn_, "Add Sector", X_300, 60); // 60: Y coordinate
    SetUpBtn(clearBtn_, "Clear", X_300, 120); // 120: Y coordinate
    SetUpBtn(transBtn_, "Y: +100", X_300, 180); // 180: Y coordinate
    SetUpBtn(scaleBtn_, "Scale 0.5", X_300, 240); // 240: Y coordinate
    SetUpBtn(rotateBtn_, "Rotate 90", X_300, 300); // 300: Y coordinate
    SetUpBtn(matrixBtn_, "Matrix", X_300, 360); // 360: Y coordinate
    SetUpBtn(paddingBtn_, "padding", X_300, 420); // 420: Y coordinate
    sectorCallBack_ = new SectorAnimatorCallback();
    if (sectorCallBack_ == nullptr) {
        GRAPHIC_LOGE("CallBack new fail");
        return;
    }
    sectorAnimator_ = new Animator(sectorCallBack_, canvas_, 0, true);
    if (sectorAnimator_ == nullptr) {
        delete sectorCallBack_;
        sectorCallBack_ = nullptr;
        GRAPHIC_LOGE("Animator new fail");
        return;
    }
}

void UITestCanvasExt::SetUpBtn(UILabelButton*& btn, const char* text, int16_t x, int16_t y)
{
    if ((text == nullptr) || (container_ == nullptr)) {
        return;
    }

    if (btn == nullptr) {
        btn = new UILabelButton();
    }

    btn->SetPosition(x, y, WIDTH_100, HEIGHT_50);
    btn->SetText(text);
    btn->SetOnClickListener(this);
    container_->Add(btn);
}

void UITestCanvasExt::TearDown()
{
    DeleteChildren(container_);
    if (sectorCallBack_ != nullptr) {
        delete sectorCallBack_;
        sectorCallBack_ = nullptr;
    }
    if (sectorAnimator_ != nullptr) {
        delete sectorAnimator_;
        sectorAnimator_ = nullptr;
    }
    container_ = nullptr;
    canvas_ = nullptr;
    addSectorBtn_ = nullptr;
    addSquareBtn_ = nullptr;
    clearBtn_ = nullptr;
    transBtn_ = nullptr;
    scaleBtn_ = nullptr;
    rotateBtn_ = nullptr;
    matrixBtn_ = nullptr;
    paddingBtn_ = nullptr;
    matrixIndex_ = -1;
    img_ = nullptr;
    ImageCacheManager::GetInstance().UnloadSingleRes(RES_PATH"haha.bin");
}

void UITestCanvasExt::Draw123(UICanvasExt* canvas)
{
    Add1(canvas);

    Add2(canvas);
    Matrix3<float> mat2(1, 0, 0, 0, 1, 0, 0, 50, 1); // 50: Y trans by 50
    canvas->SetMatrix(mat2);

    Add3(canvas);
    Matrix3<float> mat3(1, 0, 0, 0, 1, 0, 0, 100, 1); // 100: Y trans by 100
    canvas->SetMatrix(mat3);

    canvas->DrawPath();
}

void UITestCanvasExt::DrawSquare(UICanvasExt* canvas)
{
    PaintExt paint;
    paint.SetStyle(Paint::PaintStyle::FILL_STYLE);
    paint.SetFillColor(Color::White());
    paint.SetOpacity(255); // 255: opa

    canvas->BeginPath(paint);

    uint8_t cmds[5] = {CMD_MOVE_TO, CMD_HLINE_TO, CMD_VLINE_TO, CMD_HLINE_TO, CMD_CLOSE}; // 5: cmd count
    // 250, 70, 260, 80, 250: coordinates.
    float data[5] = {250, 70, 260, 80, 250}; // 5: data count
    canvas->SetPathData(5, cmds, 5, data);  // 5: cmd / data count

    canvas->DrawPath();
}

bool UITestCanvasExt::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == addSquareBtn_) {
        sectorAnimator_->Stop();
        DrawSquare(canvas_);
    } else if (&view == clearBtn_) {
        sectorAnimator_->Stop();
        canvas_->Clear();
    } else if (&view == transBtn_) {
        canvas_->Translate({0, 100}); // 100: y offset
    } else if (&view == scaleBtn_) {
        canvas_->Scale({0.5, 0.5}, {0, 0}); // 0.5: scale by 0.5
    } else if (&view == rotateBtn_) {
        canvas_->Rotate(90, {200, 200}); // 90: rotate angle, 200, 200: rotate pivot
    } else if (&view == matrixBtn_) {
        if (matrixIndex_ != -1) {
            static int para = 1;
            Matrix3<float> mat(1 + 0.5 * para, 0, 0, 0, 1, 0, 0, para * 50, 1); // 0.5: scale, 50: Y trans by 50
            canvas_->SetMatrix(mat, matrixIndex_);
            para = (para + 1) % 2; // 2: num
        }
    } else if (&view == addSectorBtn_) {
        DrawSector(canvas_);
    } else if (&view == paddingBtn_) {
        static int para = 1;
        canvas_->SetStyle(STYLE_PADDING_LEFT, para * 50); // 50: padding left
        canvas_->SetStyle(STYLE_PADDING_TOP, para * 50); // 50: padding top
        container_->Invalidate();
        para = (para + 1) % 2; // 2: num
    }
    return true;
}

const UIView* UITestCanvasExt::GetTestView()
{
    DrawArc(canvas_);
    DrawRect(canvas_);
    DrawRoundRect(canvas_);
    DrawLine(canvas_);
    DrawCurve(canvas_);
    Draw123(canvas_);
    DrawPattern(canvas_);
    DrawImage(canvas_);
    DrawLabel(canvas_);
    return container_;
}

void UITestCanvasExt::Add1(UICanvasExt* canvas)
{
    PaintExt paint;
    paint.SetStyle(Paint::PaintStyle::STROKE_FILL_STYLE);
    paint.SetFillColor(Color::Yellow());
    paint.SetStrokeColor(Color::Red());
    paint.SetStrokeWidth(2); // 2: stroke width
    paint.SetOpacity(100); // 100: opa

    canvas->BeginPath(paint);
    uint8_t cmds[5] = {CMD_MOVE_TO, CMD_HLINE_TO, CMD_VLINE_TO, CMD_LINE_TO, CMD_CLOSE};  // 5: cmd count
    // 100, 20, 110, 80, 100, 80: coordinates
    float data[6] = {100, 20, 110, 80, 100, 80}; // 6: data count
    canvas->SetPathData(5, cmds, 6, data); // 5: cmd count, 6: data count
    canvas->DrawPath();
}

void UITestCanvasExt::Add2(UICanvasExt* canvas)
{
    PaintExt paint;
    paint.SetStyle(Paint::PaintStyle::STROKE_STYLE);
    paint.SetStrokeColor(Color::Green());
    paint.SetStrokeWidth(5); // 5: stroke width
    paint.SetOpacity(150); // 150: opa

    canvas->BeginPath(paint);
    uint8_t cmds[4] = {CMD_MOVE_TO, CMD_HLINE_TO, CMD_CUBIC_TO, CMD_LINE_TO}; // 4: cmd count
    // 150, 20, 180, 180, 30, 150, 70, 150, 80, 180, 80: coordinates
    float data[11] = {150, 20, 180, 180, 30, 150, 70, 150, 80, 180, 80}; // 11: data count
    canvas->SetPathData(4, cmds, 11, data); // 4: cmd count, 11: data count
    canvas->DrawPath();
}

void UITestCanvasExt::Add3(UICanvasExt* canvas)
{
    PaintExt paint;
    paint.SetStyle(Paint::PaintStyle::STROKE_STYLE);
    paint.SetStrokeColor(Color::Orange());
    paint.SetStrokeWidth(3); // 3: stroke width
    paint.SetOpacity(200); // 200: opa

    canvas->BeginPath(paint);
    // 7: cmd count.
    uint8_t cmds[7] = {CMD_MOVE_TO, CMD_QUARD_TO, CMD_VLINE_TO, CMD_HLINE_TO, CMD_MOVE_TO, CMD_VLINE_TO, CMD_HLINE_TO};
    // 200, 30, 215, 20, 230, 30, 50, 200, 230, 50, 80, 200: coordinates.
    float data[12] = {200, 30, 215, 20, 230, 30, 50, 200, 230, 50, 80, 200}; // 12: data count.
    canvas->SetPathData(7, cmds, 12, data); // 7: cmd count, 12: data count
    canvas->DrawPath();
}

void UITestCanvasExt::DrawLine(UICanvasExt* canvas)
{
    PaintExt paint;
    paint.SetStyle(Paint::PaintStyle::STROKE_STYLE);
    paint.SetStrokeWidth(20); // 20: stroke width
    paint.SetStrokeColor(Color::Blue());
    paint.SetOpacity(OPA_50);
    paint.SetCapType(CAP_ROUND);
    paint.SetGradientSpread(GradientSpread::SPREAD_REFLECT);
    LinearGradient linear = {0, 0, 0, 300}; // 300: linear param
    ColorStop stops[3] = {{0, Color::White().full}, {0.3, Color::Blue().full}, {1, Color::Green().full}};
    paint.SetLinearGradient(linear, stops, 3); // 3: count
    canvas->DrawLine({150, 100}, {200, 300}, paint); // 150, 100, 200, 300: coordinate
    Matrix3<float> mat3(1, 0, 0, 0, 1, 0, 0, 100, 1); // 100: Y trans by 100
    canvas->SetMatrix(mat3);
}

void UITestCanvasExt::DrawCurve(UICanvasExt* canvas)
{
    PaintExt paint;
    paint.SetStyle(Paint::PaintStyle::STROKE_STYLE);
    paint.SetStrokeWidth(30); // 30: stroke width
    paint.SetOpacity(OPA_50);
    paint.SetStrokeColor(Color::Olive());
    paint.SetJoinType(JoinType::JOIN_BEVEL);
    paint.SetGradientSpread(GradientSpread::SPREAD_PAD);
    paint.SetAntialiased(false);
    SweepGradient sweep = {130, 200, 90}; // 130, 200, 90: radial param
    ColorStop stops[3] = {{0, Color::White().full}, {0.3, Color::Blue().full}, {1, Color::Green().full}};
    paint.SetSweepGradient(sweep, stops, 3); // 3: count
    // 100, 200, 120, 100, 150, 300, 180, 200: coordinate
    canvas->DrawCurve({100, 200}, {120, 100}, {150, 300}, {180, 200}, paint);
}

void UITestCanvasExt::DrawRect(UICanvasExt* canvas)
{
    PaintExt paint;
    paint.SetStyle(Paint::PaintStyle::STROKE_FILL_STYLE);
    paint.SetStrokeWidth(30); // 30: stroke width
    paint.SetStrokeColor(Color::Cyan());
    paint.SetCapType(CAP_ROUND);
    paint.SetOpacity(5); // 5: opa
    paint.SetFillColor(Color::Silver());
    paint.SetMiterLimit(1);
    paint.SetFillRule(FillRule::EVEN_ODD);
    paint.SetGradientSpread(GradientSpread::SPREAD_REPEAT);
    paint.SetAntialiased(true);
    RadialGradient radial = {150, 150, 100, 100, 30}; // 150, 150, 100, 100, 30: radial param
    ColorStop stops[3] = {{0, Color::White().full}, {0.3, Color::Blue().full}, {1, Color::Green().full}};
    paint.SetRadialGradient(radial, stops, 3); // 3: count
    int16_t index = canvas->DrawRect({100, 100}, 100, 100, paint); // 100 : coordinate
    if (matrixIndex_ == -1) {
        matrixIndex_ = index;
    }
}

void UITestCanvasExt::DrawRoundRect(UICanvasExt* canvas)
{
    PaintExt paint;
    paint.SetStyle(Paint::PaintStyle::STROKE_FILL_STYLE);
    paint.SetStrokeWidth(10); // 10: stroke width
    paint.SetStrokeColor(Color::Purple());
    paint.SetOpacity(125); // 125: opa
    paint.SetFillColor(Color::Navy());
    paint.SetFillRule(FillRule::EVEN_ODD);
    paint.SetAntialiased(true);
    int16_t index = canvas->DrawRect({240, 220}, 50, 50, paint, 10); // 240, 220, 50, 10 : coordinate, radius
    if (matrixIndex_ == -1) {
        matrixIndex_ = index;
    }
}

void UITestCanvasExt::DrawArc(UICanvasExt* canvas)
{
    PaintExt paint;
    paint.SetStyle(Paint::PaintStyle::STROKE_STYLE);
    paint.SetStrokeWidth(2); // 2: stroke width
    paint.SetOpacity(OPA_50);
    paint.SetStrokeColor(Color::Blue());
    paint.SetJoinType(JoinType::JOIN_BEVEL);
    paint.SetGradientSpread(GradientSpread::SPREAD_PAD);
    paint.SetAntialiased(false);
    paint.SetStrokeColor(Color::Red());

    int16_t cx = 100; // 100: center x
    int16_t cy = 300; // 300: center y
    canvas->DrawLine({cx, cy}, {cx, static_cast<int16_t>(cy - 50)}, paint); // 50: y offset

    paint.SetStrokeColor(Color::Blue());
    canvas->DrawArc({cx, cy}, 10, 0, 90, paint); // 10, 0, 90: radius, startAngle, endAngle
    paint.SetStrokeColor(Color::Yellow());
    canvas->DrawArc({cx, cy}, 10, 90, 180, paint); // 10, 90, 180: radius, startAngle, endAngle
    paint.SetStrokeColor(Color::Green());
    canvas->DrawArc({cx, cy}, 10, 180, 360, paint); // 10, 180, 360: radius, startAngle, endAngle

    paint.SetStrokeColor(Color::Orange());
    paint.SetOpacity(100); // 100: opa
    canvas->DrawArc({cx, cy}, 20, 290, 30, paint); // 20, 290, 30: radius, startAngle, endAngle

    paint.SetStrokeColor(Color::Orange());
    paint.SetOpacity(250); // 250: opa
    canvas->DrawArc({cx, cy}, 25, 10, 360, paint); // 25, 10, 360: radius, startAngle, endAngle

    paint.SetStrokeColor(Color::Green());
    paint.SetOpacity(OPA_OPAQUE);
    canvas->DrawArc({cx, cy}, 30, 10, 390, paint); // 30, 10, 390: radius, startAngle, endAngle
    canvas->DrawArc({cx, cy}, 35, 380, 270, paint); // 35, 380, 270: radius, startAngle, endAngle
    canvas->DrawArc({cx, cy}, 40, 540, 630, paint); // 40, 540, 630: radius, startAngle, endAngle

    cx += 100; // 100: offset
    paint.SetStyle(Paint::PaintStyle::FILL_STYLE);
    paint.SetFillColor(Color::Red());
    paint.SetOpacity(OPA_OPAQUE);
    canvas->DrawArc({cx, cy}, 40, 270, 360, paint); // 40, 270, 360: radius, startAngle, endAngle
    canvas->DrawArc({cx, cy}, 40, 0, 90, paint); // 40, 0, 90: radius, startAngle, endAngle
    canvas->DrawArc({cx, cy}, 40, 90, 180, paint); // 40, 90, 180: radius, startAngle, endAngle
    canvas->DrawArc({cx, cy}, 40, 180, 270, paint); // 40, 180, 270: radius, startAngle, endAngle
}

void UITestCanvasExt::DrawSector(UICanvasExt* canvas)
{
    if (sectorAnimator_ != nullptr) {
        sectorAnimator_->Start();
    }
}

void UITestCanvasExt::DrawPattern(UICanvasExt* canvas)
{
    PaintExt paint;
    paint.SetStyle(Paint::PaintStyle::FILL_STYLE);
    img_ = ImageCacheManager::GetInstance().LoadSingleRes(RES_PATH"haha.bin");
    if (img_ == nullptr) {
        GRAPHIC_LOGE("Load image failed!");
        return;
    }
    Matrix3<float> patternMat1(1, 0, 0, 0, 1, 0, 0, 0, 1);
    paint.SetWrapPattern(PatternType::PATTERN_PAD, Color::Yellow().full, patternMat1, *img_);
    canvas->DrawArc({0, 0}, 80, 0, 360, paint); // 80, 0, 360: radius, startAngle, endAngle
    Matrix3<float> mat1(1, 0, 0, 0, 1, 0, 100, 400, 1); // 100, 400: x/y axis translate
    canvas->SetMatrix(mat1);

    Matrix3<float> patternMat2(1, 0, 0, 0, 1, 0, 50, 50, 1); // 50, 50: x/y axis translate
    paint.SetWrapPattern(PatternType::PATTERN_FILL, Color::Yellow().full, patternMat2, *img_);
    canvas->DrawRect({0, 0}, 160, 160, paint); // 160, 160: left, top, width, height
    Matrix3<float> mat2(1, 0, 0, 0, 1, 0, 250, 370, 1); // 250, 370: x/y axis translate
    canvas->SetMatrix(mat2);

    paint.SetStyle(Paint::PaintStyle::STROKE_STYLE);
    paint.SetStrokeWidth(10); // 10: stroke width
    Matrix3<float> patternMat3(1, 0, 0, 0, 1, 0, 0, 0, 1);
    paint.SetWrapPattern(PatternType::PATTERN_FILL, Color::Yellow().full, patternMat3, *img_);
    canvas->DrawArc({0, 0}, 40, 0, 360, paint); // 40, 0, 360: radius, startAngle, endAngle
    Matrix3<float> mat3(1, 0, 0, 0, 1, 0, 100, 500, 1); // 100, 500: x/y axis translate
    canvas->SetMatrix(mat3);
}

void UITestCanvasExt::DrawImage(UICanvasExt* canvas)
{
    PaintExt paint;
    canvas->DrawImage({200, 400}, RES_PATH"PROGRESS_BAR_TMAGE.bin", paint); // 200, 400: startPoint
    Matrix3<float> mat(1, 0, 0, 0, 1, 0, 50, 0, 1); // 50: x axis translate
    canvas->SetMatrix(mat);

    paint.Scale(0.5, 0.5); // 0.5: scale factor
    paint.Rotate(30, 200, 400); // 30: rotate angle, 200, 400: coordinates
    paint.Translate(0, 50); // 50: y axis translate
    canvas->DrawImage({200, 400}, RES_PATH"haha.bin", paint); // 200, 400: startPoint
}

void UITestCanvasExt::DrawLabel(UICanvasExt* canvas)
{
    PaintExt paint;
    paint.SetFillColor(Color::Blue());
    UICanvasExt::FontStyle fontStyle;
    fontStyle.align = TEXT_ALIGNMENT_LEFT;
    fontStyle.direct = TEXT_DIRECT_LTR;
    fontStyle.fontName = DEFAULT_VECTOR_FONT_FILENAME;
    fontStyle.fontSize = 10;    // 10: font size
    fontStyle.letterSpace = 5; // 5 letter space
    // {180, 30}: start point coordinates, 100: max width
    canvas->DrawLabel({180, 30}, "canvasext label绘制", 100, fontStyle, paint);
    Matrix3<float> rotate =  Matrix3<float>::Rotate(45, {180.0f, 30.0f}); // 180.0f, 30.0f: coordinates, 45: angle
    canvas->SetMatrix(rotate);
}

#endif
}
