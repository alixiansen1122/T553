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

#include "ui_test_draw.h"
namespace OHOS {
const static int16_t GAP_10 = 10;
const static int16_t GAP_20 = 20;
const static int16_t COOR_50 = 50;
const static int16_t COOR_70 = 70;
const static int16_t COOR_100 = 100;
const static int16_t COOR_150 = 150;
const static int16_t COOR_200 = 200;
const static int16_t COOR_250 = 250;
const static int16_t COOR_300 = 300;
const static int16_t COOR_350 = 350;
const static int16_t COOR_380 = 380;
const static int16_t WIDTH_50 = 50;
const static int16_t HEIGHT_50 = 50;
void UITestDraw::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetThrowDrag(true);
        container_->SetHorizontalScrollState(false);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        positionX_ = 0;
        positionY_ = 0;
    }
}

void UITestDraw::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
}

const UIView* UITestDraw::GetTestView()
{
    TestDraw();
    return container_;
}

void UITestDraw::SetUpCanvas(UIView* canvas)
{
    if (canvas == nullptr) {
        return;
    }
    container_->Add(canvas);
    canvas->SetPosition(0, 0, Screen::GetInstance().GetWidth(), 800); // 800: height
    canvas->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    canvas->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
}

void UITestDraw::DrawLine(UICanvas* canvas, Paint& paint)
{
    paint.SetStrokeWidth(1);
    canvas->DrawLine({ COOR_100, COOR_350 }, { COOR_200, COOR_350 }, paint);
    canvas->DrawLine({ COOR_100, COOR_350 + GAP_10 }, { COOR_200, COOR_350 + GAP_20 }, paint);

    paint.SetOpacity(OPA_50);
    canvas->DrawLine({ COOR_250, COOR_350 }, { COOR_350, COOR_350 }, paint);
    canvas->DrawLine({ COOR_250, COOR_350 + GAP_10 }, { COOR_350, COOR_350 + GAP_20 }, paint);

    paint.SetOpacity(OPA_OPAQUE);
    paint.SetStrokeWidth(5); // 5: stroke width
    canvas->DrawLine({ COOR_100, COOR_380 }, { COOR_200, COOR_380 }, paint);
    canvas->DrawLine({ COOR_100, COOR_380 + GAP_10 }, { COOR_200, COOR_380 + GAP_20 }, paint);

    paint.SetOpacity(OPA_50);
    canvas->DrawLine({ COOR_250, COOR_380 }, { COOR_350, COOR_380 }, paint);
    canvas->DrawLine({ COOR_250, COOR_380 + GAP_10}, { COOR_350, COOR_380 + GAP_20}, paint);
}

void UITestDraw::DrawPathTriangle(UICanvas* canvas, Paint& paint)
{
    paint.SetStrokeColor(Color::Red());
    paint.SetStrokeWidth(10); // 10: stroke width
    paint.SetOpacity(OPA_OPAQUE);

    canvas->BeginPath();
    canvas->MoveTo({COOR_50, 450}); // 450: y
    canvas->LineTo({COOR_100, 550}); // 500: y
    canvas->LineTo({COOR_200, 450}); // 450: y
    canvas->ClosePath();
    canvas->DrawPath(paint);
}

void UITestDraw::TestDraw()
{
    UICanvas* canvas = new UICanvas();
    SetUpCanvas(canvas);
    Paint paint;
    paint.SetStrokeColor(Color::Red());

    canvas->DrawCurve({ COOR_100, COOR_50 }, { COOR_150, COOR_50 },
        { COOR_150, COOR_50 }, { COOR_150, COOR_100 }, paint);

    paint.SetStyle(Paint::PaintStyle::STROKE_STYLE);
    paint.SetStrokeColor(Color::Red());
    paint.SetStrokeWidth(10); // 10: stroke width
    canvas->DrawArc({ COOR_250, COOR_100 }, 50, 300, 360, paint); // 50: radius, 300: startAngle, 360: endAngle

    paint.SetStyle(Paint::PaintStyle::FILL_STYLE);
    paint.SetFillColor(Color::Yellow());
    canvas->DrawSector({ COOR_100, COOR_150 }, 50, 0, 60, paint); // 50: radius, 0: startAngle, 60: endAngle

    paint.SetFillColor(Color::Yellow());
    canvas->DrawCircle({ COOR_250, COOR_150 }, 30, paint); // 30: radius

    paint.SetStyle(Paint::PaintStyle::STROKE_FILL_STYLE);
    paint.SetFillColor(Color::Yellow());
    canvas->DrawRect({ COOR_100, COOR_200 }, WIDTH_50, HEIGHT_50, paint);

    UIView* view = new UIView();
    view->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
    view->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    view->SetStyle(STYLE_BORDER_RADIUS, 15); // 15: radius
    view->SetPosition(COOR_250, COOR_250, WIDTH_50, HEIGHT_50);
    container_->Add(view);

    DrawLine(canvas, paint);
    DrawPathTriangle(canvas, paint);
}
}