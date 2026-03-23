/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Trim path Test
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#include "ui_test_trim_path.h"
#include "lottie/lott_trim_path.h"
#include "draw/hw_draw_utils.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "gfx_utils/mem_check.h"

namespace OHOS {
static const int32_t TRIM_RANGE_MIN = 0;
static const int32_t TRIM_RANGE_MAX = 100;
static const int16_t SLIDER_WIDTH = 130;
static const int16_t SLIDER_HEIGHT = 30;
static const int16_t SLIDER_VALID_HEIGHT = 4;
static const int16_t KNOB_WIDTH = 8;

void UITestTrimPath::InitPath()
{
    Path sectorPath = {0};
    ArcInfo arcInfo = {{100, 150}, {0, 0}, 100, 0, 90, nullptr}; // 100, 15, 90: arc info paramters
    if (!HWDrawUtils::GetInstance()->ObtainSectorPath(arcInfo, sectorPath.cmdNum, sectorPath.cmds,
        sectorPath.dataNum, sectorPath.data)) {
        GRAPHIC_LOGE("alloc data failed");
        return;
    }

    Path cirlePath = {0};
    arcInfo = {{270, 80}, {0, 0}, 50, 0, 0, nullptr}; // 270, 80, 50: arc info paramters
    if (!HWDrawUtils::GetInstance()->ObtainCirclePath(arcInfo, cirlePath.cmdNum, cirlePath.cmds,
        cirlePath.dataNum, cirlePath.data)) {
        GRAPHIC_LOGE("alloc data failed");
        HWDrawUtils::GetInstance()->FreeBufferForPath(&sectorPath);
        return;
    }

    path_.cmdNum = sectorPath.cmdNum + cirlePath.cmdNum;
    path_.dataNum = sectorPath.dataNum + cirlePath.dataNum;
    const uint8_t fSize = sizeof(float);
    HWDrawUtils::GetInstance()->AllocBufferForPath(&path_, path_.cmdNum, path_.dataNum);
    memcpy_s(path_.cmds, path_.cmdNum, sectorPath.cmds, sectorPath.cmdNum);
    memcpy_s(path_.cmds + sectorPath.cmdNum, cirlePath.cmdNum, cirlePath.cmds, cirlePath.cmdNum);
    memcpy_s(path_.data, path_.dataNum * fSize, sectorPath.data, sectorPath.dataNum * fSize);
    memcpy_s(path_.data + sectorPath.dataNum, cirlePath.dataNum * fSize, cirlePath.data, cirlePath.dataNum * fSize);
    HWDrawUtils::GetInstance()->FreeBufferForPath(&sectorPath);
    HWDrawUtils::GetInstance()->FreeBufferForPath(&cirlePath);

    paint_.SetStyle(Paint::PaintStyle::STROKE_STYLE);
    paint_.SetStrokeColor(Color::Green());
    paint_.SetStrokeWidth(5); // 5: stroke width
    canvas_->BeginPath(paint_);
    canvas_->SetPathData(path_.cmdNum, path_.cmds, path_.dataNum, path_.data);
    canvas_->DrawPath();
}

void UITestTrimPath::InitSlider(UISlider*& slider, int16_t x, int16_t y)
{
    slider = new UISlider();
    if (slider == nullptr) {
        GRAPHIC_LOGE("new UISlider failed\n");
        return;
    }
    slider->SetPosition(x, y, SLIDER_WIDTH, SLIDER_HEIGHT);
    slider->SetRange(TRIM_RANGE_MIN, TRIM_RANGE_MAX);
    slider->SetValidHeight(SLIDER_VALID_HEIGHT);
    slider->SetValidWidth(SLIDER_WIDTH - 10); // 10, reserve width
    slider->SetKnobWidth(KNOB_WIDTH);
    slider->SetSliderRadius(5, 5); // 5:background radius, 5:foreground radius
    slider->SetKnobStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
    slider->SetBackgroundStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    slider->SetBackgroundStyle(STYLE_BACKGROUND_OPA, 90); // 90: opa
    slider->SetDirection(UISlider::Direction::DIR_LEFT_TO_RIGHT);
    slider->SetSliderEventListener(this);
    container_->Add(slider);
}

void UITestTrimPath::InitLabel(UILabel*& label, const Rect& rect, const char* str)
{
    label = new UILabel();
    if (label == nullptr) {
        GRAPHIC_LOGE("new UILabel failed\n");
        return;
    }
    label->SetPosition(rect.GetLeft(), rect.GetTop(), rect.GetWidth(), rect.GetHeight());
    label->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    label->SetText(str);
    container_->Add(label);
}

void UITestTrimPath::SetUp()
{
    MemCheck::GetInstance()->DumpMemInfo();
    if (container_ != nullptr) {
        return;
    }
    container_ = new UIViewGroup();
    container_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());

    canvas_ = new UICanvasExt();
    canvas_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), 250); // 250: canvas height
    canvas_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    container_->Add(canvas_);

    InitLabel(startLabel_, Rect(20, 280, 70, 310), "Start:"); // 20, 280, 70, 310: coordinate
    InitSlider(startSlider_, 70, 280); // 70, 280: coordinate

    InitLabel(endLabel_, Rect(220, 280, 270, 310), "End:"); // 220, 280, 270, 310: coordinate
    InitSlider(endSlider_, 270, 280); // 260, 280: coordinate
    endSlider_->SetValue(TRIM_RANGE_MAX);

    InitLabel(offsetLabel_, Rect(110, 330, 170, 360), "Offset:"); // 110, 330, 170, 360: coordinate
    InitSlider(offsetSlider_, 170, 330); // 180, 330: coordinate

    InitLabel(parallelLabel_, Rect(200, 380, 260, 440), "Parallel:"); // 200, 380, 260, 440: coordinate
    parallelCheckBox_ = new UICheckBox();
    parallelCheckBox_->SetPosition(260, 380, 50, 50); // 260, 380, 50, 50: coordinate
    parallelCheckBox_->SetState(UICheckBox::UICheckBoxState::UNSELECTED);
    parallelCheckBox_->SetOnChangeListener(this);
    container_->Add(parallelCheckBox_);

    InitPath();
}

void UITestTrimPath::TearDown()
{
    UIViewGroup::RemoveAndDeleteAllRecursively(container_);
    HWDrawUtils::GetInstance()->FreeBufferForPath(&path_);
    lottTrimPath_.ClearPath();
    MemCheck::GetInstance()->DumpMemInfo();
}

void UITestTrimPath::TrimPath()
{
    canvas_->Clear();
    canvas_->BeginPath(paint_);
    lottTrimPath_.SetPath(path_);
    Path trimedPath = lottTrimPath_.TrimPath(startValue_, endValue_, offsetValue_, isParallel_);
    // sync because trimed path is drawing and needs to be updated
    LiteMGfxEngine::GetInstance()->SyncHwDraw();
    canvas_->SetPathData(trimedPath.cmdNum, trimedPath.cmds, trimedPath.dataNum, trimedPath.data);
    canvas_->DrawPath();
}

bool UITestTrimPath::OnChange(UICheckBox::UICheckBoxState state)
{
    isParallel_ = (state == UICheckBox::UICheckBoxState::SELECTED);
    TrimPath();
    return true;
}

void UITestTrimPath::OnChange(int32_t value)
{
    startValue_ = startSlider_->GetValue() * 1.0f / TRIM_RANGE_MAX;
    endValue_ = endSlider_->GetValue() * 1.0f / TRIM_RANGE_MAX;
    offsetValue_ = offsetSlider_->GetValue() * 1.0f / TRIM_RANGE_MAX;
    TrimPath();
}

const UIView* UITestTrimPath::GetTestView()
{
    return container_;
}
}