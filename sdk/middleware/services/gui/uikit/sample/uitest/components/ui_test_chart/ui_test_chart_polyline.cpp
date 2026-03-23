/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "ui_test_chart_polyline.h"

namespace OHOS {
namespace {
static int16_t g_blank = 20;
} // namespace

void UITestChartPolyline::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        container_->SetHorizontalScrollState(false);
        container_->SetThrowDrag(true);
    }

    dataSerial_[0] = new UIChartDataSerial();
    dataSerial_[0]->SetMaxDataCount(7); // 7: number of data points
    // 0,2478: index, height; 1,2600: index, height; 2,3000:index, height; 3,3200:index, height; 4,3500:index, height
    Point pointArray[5] = {{0, 2478}, {1, 2600}, {2, 3000}, {3, 3200}, {4, 3500}};
    dataSerial_[0]->AddPoints(pointArray, 5); // 5: number of data points
    dataSerial_[0]->SetLineColor(Color::Red());
    dataSerial_[0]->SetFillColor(Color::Red());
    dataSerial_[0]->EnableGradient(true);

    dataSerial_[1] = new UIChartDataSerial();
    dataSerial_[1]->SetMaxDataCount(5); // 5: number of data points
    // 0, 2000: index, height; 1, 0: index, height; 2, 800:index, height; 3, 700:index, height; 4, 433:index, height
    Point pointArray1[5] = {{0, 2000}, {1, 0}, {2, 800}, {3, 700}, {4, 433}};
    dataSerial_[1]->AddPoints(pointArray1, 5); // 5: number of data points
    dataSerial_[1]->SetLineColor(Color::Green());
    dataSerial_[1]->SetFillColor(Color::Green());
    dataSerial_[1]->EnableGradient(true);

    dataSerial_[2] = new UIChartDataSerial(); // 2: array index
    dataSerial_[2]->SetMaxDataCount(5);       // 2: array index, 5: number of data points
    // 0, 100: index, height; 1, 200: index, height; 2, 300:index, height; 3, 400:index, height; 4, 500:index, height
    Point pointArray2[5] = {{0, 100}, {1, 200}, {2, 300}, {3, 400}, {4, 500}};
    dataSerial_[2]->AddPoints(pointArray2, 5);   // 2: array index, 5: number of data points
    dataSerial_[2]->SetLineColor(Color::Blue()); // 2: array index
    curDataIndex_ = 0;
}

void UITestChartPolyline::InnerDeleteChildren(UIView* view) const
{
    if (view == nullptr) {
        return;
    }
    while (view != nullptr) {
        UIView* tempView = view;
        view = view->GetNextSibling();
        if (tempView->IsViewGroup()) {
            InnerDeleteChildren(static_cast<UIViewGroup*>(tempView)->GetChildrenHead());
        }
        if (tempView->GetViewType() == UI_AXIS) {
            return;
        }
        if (tempView->GetParent()) {
            static_cast<UIViewGroup*>(tempView->GetParent())->Remove(tempView);
        }
        delete tempView;
        tempView = nullptr;
    }
}

void UITestChartPolyline::TearDown()
{
    chart_->ClearDataSerial();
    for (uint8_t i = 0; i < DATA_NUM; i++) {
        delete dataSerial_[i];
        dataSerial_[i] = nullptr;
    }
    InnerDeleteChildren(container_);
    container_ = nullptr;

    lastX_ = 0;
    lastY_ = 0;
    positionX_ = 0;
    positionY_ = 0;
}

const UIView* UITestChartPolyline::GetTestView()
{
    TestAddDataSerial();
    TestReverse();
    TestGradientBottom();
    TestAddPoints();
    return container_;
}

void UITestChartPolyline::TestAddDataSerial()
{
    chart_ = new UIChartPolyline();
    chart_->SetPosition(100, 100); // 100: x, 100: y
    chart_->SetWidth(300);       // 300: width
    chart_->SetHeight(150);      // 150: height

    UIXAxis& xAxis = chart_->GetXAxis();
    UIYAxis& yAxis = chart_->GetYAxis();
    xAxis.SetMarkNum(5);         // 5: number of scales
    xAxis.SetDataRange(0, 5);    // 0: minimum value, 5: maximum value
    yAxis.SetDataRange(0, 3500); // 0: minimum value, 3500: maximum value

    chart_->SetGradientOpacity(25, 127); // 25: min opacity, 127: max opacity
    chart_->AddDataSerial(dataSerial_[0]);
    curDataIndex_++;
    container_->Add(chart_);
    SetLastPos(chart_);

    addDataSerialBtn_ = new UILabelButton();
    deleteDataSerialBtn_ = new UILabelButton();
    clearDataSerialBtn_ = new UILabelButton();
    topPointBtn_ = new UILabelButton();
    bottomPointBtn_ = new UILabelButton();
    headPointBtn_ = new UILabelButton();

    positionX_ = VIEW_DISTANCE_TO_LEFT_SIDE;
    positionY_ = lastY_ + 10; // 10: increase y-coordinate
    SetUpButton(addDataSerialBtn_, "add data serial");
    positionX_ = addDataSerialBtn_->GetX() + addDataSerialBtn_->GetWidth() + g_blank;
    positionY_ = addDataSerialBtn_->GetY();
    SetUpButton(deleteDataSerialBtn_, "delete");
    positionX_ = deleteDataSerialBtn_->GetX() + deleteDataSerialBtn_->GetWidth() + g_blank;
    positionY_ = deleteDataSerialBtn_->GetY();
    SetUpButton(clearDataSerialBtn_, "clear");

    positionX_ = VIEW_DISTANCE_TO_LEFT_SIDE;
    SetUpButton(topPointBtn_, "max");
    positionX_ = topPointBtn_->GetX() + topPointBtn_->GetWidth() + g_blank;
    positionY_ = topPointBtn_->GetY();
    SetUpButton(bottomPointBtn_, "min");
    positionX_ = bottomPointBtn_->GetX() + bottomPointBtn_->GetWidth() + g_blank;
    positionY_ = bottomPointBtn_->GetY();
    SetUpButton(headPointBtn_, "new");
}

void UITestChartPolyline::TestReverse()
{
    reverseBtn_ = new UILabelButton();
    positionX_ = VIEW_DISTANCE_TO_LEFT_SIDE;
    SetUpButton(reverseBtn_, "reverse");
    SetLastPos(reverseBtn_);
}

void UITestChartPolyline::TestGradientBottom()
{
    gradientBottomBtn_ = new UILabelButton();
    positionX_ = reverseBtn_->GetX() + reverseBtn_->GetWidth() + g_blank;
    positionY_ = reverseBtn_->GetY();
    SetUpButton(gradientBottomBtn_, "fill bottom");
    SetLastPos(gradientBottomBtn_);
}

void UITestChartPolyline::TestAddPoints()
{
    addPointsBtn_ = new UILabelButton();
    positionX_ = gradientBottomBtn_->GetX() + gradientBottomBtn_->GetWidth() + g_blank;
    positionY_ = gradientBottomBtn_->GetY();
    SetUpButton(addPointsBtn_, "add points");
    SetLastPos(addPointsBtn_);
}

bool UITestChartPolyline::OnClick(UIView& view, const ClickEvent& event)
{
    UIChartDataSerial::PointStyle pointStyle;
    pointStyle.fillColor = Color::White();
    pointStyle.radius = 5; // 5: Inner radius
    pointStyle.strokeColor = Color::Red();
    pointStyle.strokeWidth = 2; // 2: border width

    if (&view == addDataSerialBtn_) {
        if (curDataIndex_ >= DATA_NUM) {
            return true;
        }
        chart_->AddDataSerial(dataSerial_[curDataIndex_]);
        curDataIndex_++;
    } else if (&view == deleteDataSerialBtn_) {
        if (curDataIndex_ <= 0) {
            return true;
        }
        chart_->DeleteDataSerial(dataSerial_[curDataIndex_ - 1]);
        curDataIndex_--;
    } else if (&view == clearDataSerialBtn_) {
        chart_->ClearDataSerial();
        curDataIndex_ = 0;
    } else if (&view == topPointBtn_) {
        dataSerial_[0]->EnableTopPoint(true);
        pointStyle.strokeColor = Color::Red();
        dataSerial_[0]->SetTopPointStyle(pointStyle);
    } else if (&view == bottomPointBtn_) {
        dataSerial_[0]->EnableBottomPoint(true);
        pointStyle.strokeColor = Color::Blue();
        dataSerial_[0]->SetBottomPointStyle(pointStyle);
    } else if (&view == headPointBtn_) {
        dataSerial_[0]->EnableHeadPoint(true);
        pointStyle.strokeColor = Color::Yellow();
        dataSerial_[0]->SetHeadPointStyle(pointStyle);
    } else if (&view == reverseBtn_) {
        chart_->EnableReverse(true);
    } else if (&view == gradientBottomBtn_) {
        chart_->SetGradientBottom(50); // 50: bottom of the filling range
    } else if (&view == addPointsBtn_) {
        Point pointArray[2] = {{4, 3500}, {5, 1700}}; // 2: count; 4, 3500: index, height; 5, 1700: index, height
        dataSerial_[0]->AddPoints(pointArray, 2); // 2: number of data points)
    }
    chart_->Invalidate();
    return true;
}

void UITestChartPolyline::SetUpButton(UILabelButton* btn, const char* title)
{
    if (btn == nullptr) {
        return;
    }
    container_->Add(btn);
    btn->SetPosition(positionX_, positionY_, 100, 50); // 100: button width, 50: height
    positionY_ += btn->GetHeight() + 5; // 5: increase height
    btn->SetText(title);
    btn->SetOnClickListener(this);
    container_->Invalidate();
}

void UITestChartPolyline::SetLastPos(UIView* view)
{
    if (view == nullptr) {
        return;
    }
    lastX_ = view->GetX();
    lastY_ = view->GetY() + view->GetHeight();
}
} // namespace OHOS