/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: UITestScatterPlot
 * Author: Hisi Graphic Team
 * Created: 2025-9
 */

#include "ui_test_scatter_plot.h"

namespace OHOS {
namespace {
static int16_t g_blank = 20;
} // namespace

void UITestScatterPlot::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        container_->SetHorizontalScrollState(false);
        container_->SetThrowDrag(true);
    }

    for (uint8_t i = 0; i < DATA_NUM; i++) {
        dataSerial_[i] = new UIScatterPlotDataSerial();
        dataSerial_[i]->SetMaxDataCount(10); // 10: max data count
    }

    UIChartDataSerial::PointStyle pointStyle;
    pointStyle.radius = 5; // 5: inner radius
    pointStyle.strokeWidth = 0; // 0: stroke width

    // 5: cnt; {1, 2600}, {2, 3000}, {3, 3200}, {4, 2700}, {5, 2100} : point array [index, value]
    Point pointArray[5] = {{1, 2600}, {2, 3000}, {3, 3200}, {4, 2700}, {5, 2100}};
    dataSerial_[0]->AddPoints(pointArray, 5); // 5: number of data points

    pointStyle.fillColor = Color::Silver();
    dataSerial_[0]->SetPointStyle(pointStyle);

    // 5: cnt; {1, 900}, {2, 1200}, {3, 700}, {4, 300}, {5, 1700} : point array [index, value]
    Point pointArray1[5] = {{1, 900}, {2, 1200}, {3, 700}, {4, 300}, {5, 1700}};
    dataSerial_[1]->AddPoints(pointArray1, 5); // 5: number of data points

    pointStyle.fillColor = Color::Yellow();
    dataSerial_[1]->SetPointStyle(pointStyle);

    // 5: cnt; {1, 500}, {2, 700}, {3, 400}, {4, 1100}, {5, 1500} : point array [index, value]
    Point pointArray2[5] = {{1, 1500}, {2, 1800}, {3, 1400}, {4, 1100}, {5, 800}};
    dataSerial_[2]->AddPoints(pointArray2, 5);   // 2: array index, 5: number of data points

    pointStyle.fillColor = Color::Magenta();
    dataSerial_[2]->SetPointStyle(pointStyle); // 2: array index

    curDataIndex_ = 0;
}

void UITestScatterPlot::InnerDeleteChildren(UIView* view) const
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
            continue;
        }
        if (tempView->GetParent() != nullptr) {
            static_cast<UIViewGroup*>(tempView->GetParent())->Remove(tempView);
        }
        delete tempView;
        tempView = nullptr;
    }
}

void UITestScatterPlot::TearDown()
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

const UIView* UITestScatterPlot::GetTestView()
{
    TestAddDataSerial();
    TestReverse();
    TestAddPoints();
    return container_;
}

void UITestScatterPlot::TestAddDataSerial()
{
    chart_ = new UIScatterPlot();
    chart_->SetPosition(100, 100); // 100: x, 100: y
    chart_->SetWidth(300);       // 300: width
    chart_->SetHeight(150);      // 150: height

    UIXAxis& xAxis = chart_->GetXAxis();
    UIYAxis& yAxis = chart_->GetYAxis();
    xAxis.SetMarkNum(5);         // 5: number of scales
    xAxis.SetDataRange(0, 10);    // 0: minimum value, 10: maximum value
    yAxis.SetDataRange(0, 3500); // 0: minimum value, 3500: maximum value

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
    SetUpButton(headPointBtn_, "head");
}

void UITestScatterPlot::TestReverse()
{
    reverseBtn_ = new UILabelButton();
    positionX_ = VIEW_DISTANCE_TO_LEFT_SIDE;
    SetUpButton(reverseBtn_, "reverse");
    SetLastPos(reverseBtn_);
}

void UITestScatterPlot::TestAddPoints()
{
    addPointsBtn_ = new UILabelButton();
    positionX_ = reverseBtn_->GetX() + reverseBtn_->GetWidth() + g_blank;
    positionY_ = reverseBtn_->GetY();
    SetUpButton(addPointsBtn_, "add points");
    SetLastPos(addPointsBtn_);
}

bool UITestScatterPlot::OnClick(UIView& view, const ClickEvent& event)
{
    UIChartDataSerial::PointStyle pointStyle;
    pointStyle.fillColor = Color::Silver();
    pointStyle.radius = 5; // 5: Inner radius
    pointStyle.strokeWidth = 2; // 2: stroke width

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
        pointStyle.strokeColor = Color::Green();
        dataSerial_[0]->SetTopPointStyle(pointStyle);
        printf("EnableTopPoint!\n");
    } else if (&view == bottomPointBtn_) {
        dataSerial_[0]->EnableBottomPoint(true);
        pointStyle.strokeColor = Color::Red();
        dataSerial_[0]->SetBottomPointStyle(pointStyle);
        printf("EnableBottomPoint!\n");
    } else if (&view == headPointBtn_) {
        dataSerial_[0]->EnableHeadPoint(true);
        pointStyle.strokeColor = Color::Purple();
        dataSerial_[0]->SetHeadPointStyle(pointStyle);
        printf("EnableHeadPoint!\n");
    } else if (&view == reverseBtn_) {
        chart_->EnableReverse(true);
        printf("EnableReverse!\n");
    } else if (&view == addPointsBtn_) {
        Point pointArray[2] = {{6, 2800}, {7, 2100}}; // 2: count; 6, 2800: index, height; 7, 2100: index, height
        dataSerial_[0]->AddPoints(pointArray, 2); // 2: number of data points
    }
    chart_->Invalidate();
    return true;
}

void UITestScatterPlot::SetUpButton(UILabelButton* btn, const char* title)
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

void UITestScatterPlot::SetLastPos(UIView* view)
{
    if (view == nullptr) {
        return;
    }
    lastX_ = view->GetX();
    lastY_ = view->GetY() + view->GetHeight();
}
} // namespace OHOS