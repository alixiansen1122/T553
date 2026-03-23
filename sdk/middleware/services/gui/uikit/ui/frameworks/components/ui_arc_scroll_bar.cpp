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

#include "components/ui_arc_scroll_bar.h"
#include "gfx_utils/graphic_types.h"
#include "draw/hw_draw_utils.h"
#include "hals/gralloc_engines.h"
#include "engines/gfx/lite_m_gfx_engine.h"

namespace {
constexpr uint16_t RIGHT_SIDE_START_ANGLE_IN_DEGREE = 60;
constexpr uint16_t RIGHT_SIDE_END_ANGLE_IN_DEGREE = 120;
constexpr uint16_t LEFT_SIDE_START_ANGLE_IN_DEGREE = 240;
constexpr uint16_t LEFT_SIDE_END_ANGLE_IN_DEGREE = 300;
constexpr uint16_t SCROLL_BAR_MIN_ARC = 10;
} // namespace

namespace OHOS {
UIArcScrollBar::UIArcScrollBar()
    : radius_(0),
      width_(0),
      startAngle_(RIGHT_SIDE_START_ANGLE_IN_DEGREE),
      endAngle_(RIGHT_SIDE_END_ANGLE_IN_DEGREE),
      center_({0, 0}),
      side_(SCROLL_BAR_RIGHT_SIDE)
{
    backgroundStyle_.lineCap_ = VGU_LINECAP_ROUND;
    foregroundStyle_.lineCap_ = VGU_LINECAP_ROUND;
}

void UIArcScrollBar::SetPosition(int16_t x, int16_t y, int16_t width, int16_t radius)
{
    if ((width > 0) && (radius > 0)) {
        center_.x = x;
        center_.y = y;
        width_ = width;
        radius_ = radius;
    }
}

void UIArcScrollBar::SetScrollBarSide(uint8_t side)
{
    if (side == SCROLL_BAR_RIGHT_SIDE) {
        startAngle_ =  RIGHT_SIDE_START_ANGLE_IN_DEGREE;
        endAngle_ = RIGHT_SIDE_END_ANGLE_IN_DEGREE;
    } else {
        startAngle_ =  LEFT_SIDE_START_ANGLE_IN_DEGREE;
        endAngle_ = LEFT_SIDE_END_ANGLE_IN_DEGREE;
    }
    side_ = side;
}

void UIArcScrollBar::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, uint8_t backgroundOpa)
{
    // 8: Shift right 8 bits
    backgroundOpa = (backgroundOpa == OPA_OPAQUE) ? opacity_ : (static_cast<uint16_t>(backgroundOpa) * opacity_) >> 8;
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
    ClearPath();
#endif
    DrawBackground(gfxDstBuffer, invalidatedArea, backgroundOpa);
    DrawForeground(gfxDstBuffer, invalidatedArea, backgroundOpa);
}

#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
void UIArcScrollBar::ClearPath()
{
    for (int i = 0; i <= 1; i++) {
        if (cmds_[i] != nullptr) {
            GrallocEngines::GetInstance()->FreeBuffer(reinterpret_cast<uint8_t*>(cmds_[i]));
            cmds_[i] = nullptr;
        }

        if (pathData_[i] != nullptr) {
            GrallocEngines::GetInstance()->FreeBuffer(reinterpret_cast<uint8_t*>(pathData_[i]));
            pathData_[i] = nullptr;
        }
    }
}

void UIArcScrollBar::DrawArcByHWPath(BufferInfo &dst, ArcInfo arc, uint8_t barLayer, const Rect &mask,
    uint16_t opa)
{
    if (!mask.IsValid() || mask.GetWidth() <= 0 || mask.GetHeight() <= 0) {
        return;
    }
    uint32_t cmdNum;
    uint32_t dataNum;
    Style *style;
    if (barLayer == 0) {
        style = &backgroundStyle_;
    } else {
        style = &foregroundStyle_;
    }
    arc.radius -= style->lineWidth_ / 2; // 2: radius is half of width
    if (!HWDrawUtils::GetInstance()->ObtainArcPath(arc, cmdNum, cmds_[barLayer], dataNum, pathData_[barLayer])) {
        return;
    }
    uintptr_t vguPath = LiteMGfxEngine::GetInstance()->InitPath(true);
    LiteMGfxEngine::GetInstance()->SetPathData(vguPath, cmdNum, cmds_[barLayer], dataNum, pathData_[barLayer]);
    VGUStrokeAttr strokeAttr = {static_cast<VGULineCap>(style->lineCap_), VGU_LINE_JOIN_MITER , 1, style->lineWidth_};
    VGUPaintStyle strokeStyle = {{0}};
    opa = (opa == OPA_OPAQUE) ? style->lineOpa_ : ((opa * style->lineOpa_) >> 8); // 8: divided by 256
    uint8_t alpha = ((style->lineColor_.full >> 24) * opa) / OPA_OPAQUE;         // 24: alpha shift
    uint32_t mixedColor = (alpha << 24) | (style->lineColor_.full & 0x00FFFFFF); // 24: alpha shift
    strokeStyle.color = mixedColor;
    strokeStyle.type = VGU_PAINT_COLOR;
    strokeAttr.style = &strokeStyle;
    LiteMGfxEngine::GetInstance()->RenderPath(dst, vguPath, mask, &strokeAttr, nullptr, nullptr);
}
#endif
void UIArcScrollBar::DrawForeground(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, uint8_t backgroundOpa)
{
    float foregoundAngleRange = foregroundProportion_ * (endAngle_ - startAngle_);
    if (foregoundAngleRange < SCROLL_BAR_MIN_ARC) {
        foregoundAngleRange = SCROLL_BAR_MIN_ARC;
    }
    float startAngle;
    float endAngle;
    float minAngle;
    float maxAngle;
    if (side_ == SCROLL_BAR_RIGHT_SIDE) {
        minAngle = startAngle_;
        maxAngle = endAngle_ - foregoundAngleRange;
        startAngle = minAngle + scrollProgress_ * (maxAngle - minAngle);
        endAngle = startAngle + foregoundAngleRange;
    } else {
        maxAngle = endAngle_;
        minAngle = startAngle_ + foregoundAngleRange;
        endAngle = maxAngle - scrollProgress_ * (maxAngle - minAngle);
        startAngle = endAngle - foregoundAngleRange;
    }
    if ((startAngle > endAngle_) || (endAngle < startAngle_)) {
        return;
    }
    ArcInfo arcInfo = {{0}};
    arcInfo.radius = radius_;
    arcInfo.center = center_;
    arcInfo.startAngle = MATH_MAX(startAngle, static_cast<float>(startAngle_));
    arcInfo.endAngle = MATH_MIN(endAngle, static_cast<float>(endAngle_));
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
    DrawArcByHWPath(gfxDstBuffer, arcInfo, 1, invalidatedArea, backgroundOpa);
#else
    BaseGfxEngine::GetInstance()->DrawArc(gfxDstBuffer, arcInfo, invalidatedArea, foregroundStyle_, backgroundOpa,
                                          foregroundStyle_.lineCap_);
#endif
}

void UIArcScrollBar::DrawBackground(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, uint8_t backgroundOpa)
{
    ArcInfo arcInfo = {{0}};
    arcInfo.radius = radius_;
    arcInfo.center = center_;
    arcInfo.startAngle = startAngle_;
    arcInfo.endAngle = endAngle_;
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
    DrawArcByHWPath(gfxDstBuffer, arcInfo, 0, invalidatedArea, backgroundOpa);
#else
    BaseGfxEngine::GetInstance()->DrawArc(gfxDstBuffer, arcInfo, invalidatedArea, backgroundStyle_, backgroundOpa,
                                          backgroundStyle_.lineCap_);
#endif
}
} // namespace OHOS
