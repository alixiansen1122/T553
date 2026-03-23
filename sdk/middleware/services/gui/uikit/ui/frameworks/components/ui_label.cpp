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

#include <cmath>
#include "hal_tick.h"
#include "components/ui_label.h"
#include "components/root_view.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "font/ui_font.h"
#include "gfx_utils/graphic_log.h"
#include "themes/theme_manager.h"

namespace OHOS {
class LabelAnimator : public Animator, public AnimatorCallback {
public:
    LabelAnimator(uint16_t textX, uint16_t labelX, int16_t startPos, UIView* view)
        : Animator(this, view, 0, true),
          startPos_(startPos),
          textWidth_(textX),
          labelWidth_(labelX),
          offsetX_(startPos),
          waitCount_(ANIM_WAIT_COUNT),
          speed_(0),
          preRunTime_(0),
          decimal_(0)
    {
    }

    virtual ~LabelAnimator() {}

    int16_t GetStartPos() const
    {
        return startPos_;
    }

    void SetStartPos(int16_t pos)
    {
        startPos_ = pos;
    }

    void SetMarqueeBlankWidth(uint16_t size)
    {
        marqueeBlankWidth_ = size;
    }

    void UpdateWidth(uint16_t textWidth, uint16_t labelWidth)
    {
        textWidth_ = textWidth;
        labelWidth_ = labelWidth;
        waitCount_ = ANIM_WAIT_COUNT;
        preRunTime_ = 0;
        decimal_ = 0;
        offsetX_ = startPos_;
        static_cast<UILabel*>(view_)->offsetX_ = offsetX_;
        view_->Invalidate();
    }

    void Callback(UIView* view) override
    {
        UILabel* label = dynamic_cast<UILabel*>(view);
        if (label == nullptr) {
            return;
        }

        // Check setAnimState_ and Pause or Stop animation if neccessary.
        if (label->CheckAnimState()) {
            return;
        }

        // If not intersected with rootview, pause animator.
        Rect rect = view->GetRect();
        Rect rootRect = RootView::GetInstance()->GetRect();
        if (label != nullptr && !rect.Intersect(rect, rootRect)) {
            if (label->hasAnimator_ && label->animator_.animator->GetState() == Animator::START) {
                label->animator_.animator->Pause();
                return;
            }
        }

        switch (animatorMode_) {
            case UILabel::LINE_BREAK_MARQUEE:
                MarqueeCallback(view);
                break;
            case UILabel::LINE_BREAK_OSCILLATION:
                OscillationCallback(view);
                break;
            default:
                GRAPHIC_LOGE("erorr animation!");
                break;
        }
    }

    void SetAnimatorSpeed(uint16_t animSpeed)
    {
        speed_ = animSpeed;
        decimal_ = 0;
    }

    void SetAnimatorMode(UILabel::LineBreakMode mode)
    {
        animatorMode_ = mode;
    }

private:
    void MarqueeCallback(UIView* view)
    {
        if (view == nullptr) {
            return;
        }

        uint32_t curTime = GetRunTime();
        if (waitCount_ > 0) {
            waitCount_--;
            preRunTime_ = curTime;
            return;
        }
        if (curTime == preRunTime_) {
            return;
        }
        uint32_t time = (curTime > preRunTime_) ? (curTime - preRunTime_) : (UINT32_MAX - preRunTime_ + curTime);
        // 1000: 1000 milliseconds is 1 second
        float floatStep = (static_cast<float>(time * speed_) / 1000) + decimal_;
        uint16_t integerStep = static_cast<uint16_t>(floatStep);
        decimal_ = floatStep - integerStep;
        preRunTime_ = curTime;

        if (integerStep != 0) {
            offsetX_ -= integerStep;
        } else {
            offsetX_ -= 1; // At lest move one pixel
            decimal_ = 0;
        }
        // the range of offsetX_ => [-textWidth_ - marqueeBlankWidth_, labelWidth_]
        offsetX_ = offsetX_ % (textWidth_ + marqueeBlankWidth_);

        /*
         * the cyclicText always appears a fixed lengths behind original Text. 
         * tx = x + textWidth_ + marqueeBlankWidth_
         * when x == labelWidth_, set x = 0
         */ 
        if (offsetX_ == labelWidth_) { 
            offsetX_ = 0;
        }

        static_cast<UILabel*>(view)->nextOffsetX_ = offsetX_ + textWidth_ + marqueeBlankWidth_;
        static_cast<UILabel*>(view)->offsetX_ = offsetX_;
        view->Invalidate();
    }

    void OscillationCallback(UIView* view)
    {
        if (view == nullptr) {
            return;
        }
        if (waitCount_ > 0) {
            waitCount_--;
            return;
        }
        if (startFlag_) {
            startTime_ = HALTick::GetInstance().GetTime();
            startFlag_ = false;
        }

        uint32_t time = HALTick::GetInstance().GetElapseTime(startTime_);
        float x0 = startPos_;
        float x1 = 1.0 * labelWidth_ - textWidth_;
        float factor = 1000; // 1000: slow down the speed
        float v = speed_ / factor;
        v = x1 - x0 > 0 ? v : -v;
        float t = time;
        float t1 = (x1 - x0) / v;
        float period = 2.0 * t1; // 2: double
        t = t - floorf(t / period) * period;
        float x = 0;
        if (t < t1) {
            x = v * t + x0;
        } else {
            x = -v * (t - t1) + x1;
        }

        UILabel *label = dynamic_cast<UILabel*>(view);
        if (label != nullptr) {
            label->offsetX_ = MATH_ROUND(x);
            label->Invalidate();
        }
    }

    static constexpr uint8_t ANIM_WAIT_COUNT = 50;
    int16_t startPos_;
    uint16_t textWidth_;
    uint16_t labelWidth_;
    int16_t offsetX_;
    uint16_t waitCount_;
    uint16_t speed_;
    uint32_t preRunTime_;
    bool startFlag_ = true;
    uint32_t startTime_ = 0;
    float decimal_;
    uint16_t marqueeBlankWidth_;
    UILabel::LineBreakMode animatorMode_ = UILabel::LINE_BREAK_MARQUEE;
};

inline uint16_t CalcBlankSpaceWidth(uint16_t num)
{
    return num * UIFont::GetInstance()->GetWidth((uint32_t)(' '), 0);
}

UILabel::UILabel()
    : labelText_(nullptr),
      needRefresh_(false),
      useTextColor_(false),
      hasAnimator_(false),
      lineBreakMode_(LINE_BREAK_ELLIPSIS),
      ellipsisIndex_(Text::TEXT_ELLIPSIS_END_INV),
      truncateIndex_(Text::TEXT_TRUNCATE_END_INV),
      offsetX_(0),
      nextOffsetX_(0),
      textColor_(Color::White()),
      animator_{nullptr}
{
    Theme* theme = ThemeManager::GetInstance().GetCurrent();
    Style& style = (theme != nullptr) ? (theme->GetLabelStyle()) : (StyleDefault::GetLabelStyle());
    UIView::SetStyle(style);
    animator_.speed = DEFAULT_ANIMATOR_SPEED;
}

UILabel::~UILabel()
{
    if (hasAnimator_) {
        delete animator_.animator;
        animator_.animator = nullptr;
        hasAnimator_ = false;
    }
    if (labelText_ != nullptr) {
        delete labelText_;
        labelText_ = nullptr;
    }
}

void UILabel::InitLabelText()
{
    if (labelText_ == nullptr) {
        labelText_ = new Text();
    }
}

int16_t UILabel::GetWidth()
{
    InitLabelText();
    if (needRefresh_ && labelText_->IsExpandWidth()) {
        ReMeasure();
    }
    return UIView::GetWidth();
}

int16_t UILabel::GetHeight()
{
    InitLabelText();
    if (needRefresh_ && labelText_->IsExpandHeight()) {
        ReMeasure();
    }
    return UIView::GetHeight();
}

void UILabel::SetStyle(uint8_t key, int64_t value)
{
    UIView::SetStyle(key, value);
    RefreshLabel();
}

void UILabel::SetText(const char* text)
{
    InitLabelText();
    labelText_->SetText(text);
    if (labelText_->IsNeedRefresh()) {
        RefreshLabel();
    }
}

void UILabel::SetLineBreakMode(const uint8_t lineBreakMode)
{
    InitLabelText();
    if ((lineBreakMode >= LINE_BREAK_MAX) || (lineBreakMode_ == lineBreakMode)) {
        return;
    }
    lineBreakMode_ = lineBreakMode;
    if ((lineBreakMode_ == LINE_BREAK_ADAPT) || (lineBreakMode_ == LINE_BREAK_STRETCH) ||
        (lineBreakMode_ == LINE_BREAK_MARQUEE) || (lineBreakMode_ == LINE_BREAK_OSCILLATION)) {
        labelText_->SetExpandWidth(true);
    } else {
        labelText_->SetExpandWidth(false);
    }
    if ((lineBreakMode_ == LINE_BREAK_ADAPT) || (lineBreakMode_ == LINE_BREAK_WRAP)) {
        labelText_->SetExpandHeight(true);
    } else {
        labelText_->SetExpandHeight(false);
    }
    if (lineBreakMode_ != LINE_BREAK_MARQUEE && lineBreakMode_ != LINE_BREAK_OSCILLATION) {
        offsetX_ = 0;
        if (hasAnimator_) {
            animator_.animator->Stop();
        }
    }
    needRefresh_ = true;
    ReMeasure();
    RefreshLabel();
}

void UILabel::SetAlign(UITextLanguageAlignment horizontalAlign, UITextLanguageAlignment verticalAlign)
{
    InitLabelText();
    labelText_->SetAlign(horizontalAlign, verticalAlign);
    horAlign_ = horizontalAlign;
    if (labelText_->IsNeedRefresh()) {
        RefreshLabel();
    }
}

void UILabel::SetFontId(uint8_t fontId)
{
    InitLabelText();
    labelText_->SetFontId(fontId);
    if (labelText_->IsNeedRefresh()) {
        RefreshLabel();
    }
}

void UILabel::SetFont(const char* name, uint8_t size)
{
    InitLabelText();
    labelText_->SetFont(name, size);
    if (labelText_->IsNeedRefresh()) {
        RefreshLabel();
    }
}

uint16_t UILabel::GetTextWidth()
{
    InitLabelText();
    if (labelText_->IsNeedRefresh()) {
        ReMeasure();
    }
    return labelText_->GetTextSize().x;
}

uint16_t UILabel::GetTextHeight()
{
    InitLabelText();
    if (labelText_->IsNeedRefresh()) {
        ReMeasure();
    }
    return labelText_->GetTextSize().y;
}

void UILabel::SetWidth(int16_t width)
{
    if (GetWidth() != width) {
        UIView::SetWidth(width);
        RefreshLabel();
    }
}

void UILabel::SetHeight(int16_t height)
{
    if (GetHeight() != height) {
        UIView::SetHeight(height);
        RefreshLabel();
    }
}

void UILabel::Resize(int16_t width, int16_t height)
{
    if (GetWidth() != width || GetHeight() != height) {
        UIView::SetWidth(width);
        UIView::SetHeight(height);
        RefreshLabel();
    }
}

void UILabel::RefreshLabel()
{
    Invalidate();
    ellipsisIndex_ = Text::TEXT_ELLIPSIS_END_INV;
    truncateIndex_ = Text::TEXT_TRUNCATE_END_INV;
    if (!needRefresh_) {
        needRefresh_ = true;
    }
}

void UILabel::ReMeasure()
{
    if (setAnimState_ == Animator::START) {
        if (lineBreakMode_ == LINE_BREAK_OSCILLATION || lineBreakMode_ == LINE_BREAK_MARQUEE) {
            Rect rect = GetRect();
            Rect rootRect = RootView::GetInstance()->GetRect();
            if (rect.Intersect(rect, rootRect)) {
                if (hasAnimator_ && animator_.animator->GetState() == Animator::PAUSE) {
                    // resume animator if now intersected with rootview.
                    animator_.animator->Resume();
                }
            }
        }
    }

    if (!needRefresh_) {
        return;
    }
    needRefresh_ = false;
    InitLabelText();
    Style style = GetStyleConst();
    style.textColor_ = GetTextColor();
    bool flag = false;
    if ((transMap_ != nullptr) && !transMap_->IsInvalid()) {
        transMap_->SetInvalid(true);
        flag = true;
    }
    labelText_->ReMeasureTextSize(GetContentRect(), style);
    Point textSize = labelText_->GetTextSize();
    switch (lineBreakMode_) {
        case LINE_BREAK_ADAPT:
            Resize(textSize.x, textSize.y);
            needRefresh_ = false;
            break;
        case LINE_BREAK_STRETCH:
            SetWidth(textSize.x);
            needRefresh_ = false;
            break;
        case LINE_BREAK_WRAP:
            SetHeight(textSize.y);
            needRefresh_ = false;
            break;
        case LINE_BREAK_ELLIPSIS:
            labelText_->SetTruncateMode(false);
            ellipsisIndex_ = labelText_->GetEllipsisIndex(GetContentRect(), style);
            labelText_->ReMeasureTextWidthInEllipsisMode(GetContentRect(), style, ellipsisIndex_);
            break;
        case LINE_BREAK_MARQUEE:
            RemeasureForAnimation(textSize.x, LINE_BREAK_MARQUEE);
            break;
        case LINE_BREAK_OSCILLATION:
            RemeasureForAnimation(textSize.x, LINE_BREAK_OSCILLATION);
            break;
        case LINE_BREAK_TRUNCATE:
            labelText_->SetTruncateMode(true);
            truncateIndex_ = labelText_->GetTruncateIndex(GetContentRect(), style);
            labelText_->ReMeasureTextWidthInEllipsisMode(GetContentRect(), style, truncateIndex_);
            break;
        default:
            break;
    }
    if ((transMap_ != nullptr) && flag) {
        transMap_->SetInvalid(false);
    }
}

std::string UILabel::GetGuiInfo() const
{
    const size_t size = 256;
    char *buf = new char[size];
    if (buf == nullptr) {
        return "";
    }
    if (labelText_ == nullptr) {
        delete []buf;
        return "";
    }
    memset_s(buf, size, 0, size);
    sprintf_s(buf, size, "text: %s, breakmode %u, font id: %u",
        labelText_->GetText(), lineBreakMode_, labelText_->GetFontId());
    std::string res(buf);
    delete []buf;
    return res;
}

bool UILabel::CheckAnimState()
{
    if (setAnimState_ == Animator::START) {
        return false;
    }

    if (hasAnimator_) {
        Animator* anim = animator_.animator;
        if (setAnimState_ == Animator::PAUSE && anim->GetState() == Animator::START) {
            anim->Pause();
        } else if (setAnimState_ == Animator::STOP && anim->GetState() != Animator::STOP) {
            anim->Stop();
        }
    }
    return true;
}

void UILabel::RemeasureForAnimation(int16_t textWidth, LineBreakMode mode)
{
    if (setAnimState_ != Animator::START) {
        return;
    }
    int16_t rectWidth = GetWidth();

    if (textWidth > rectWidth) {
        offsetX_ = GetRollStartPos();
        nextOffsetX_ = offsetX_ + textWidth + CalcBlankSpaceWidth(marqueeBlankNum_);
        if (labelText_->GetDirect() == TEXT_DIRECT_RTL) {
            labelText_->SetAlign(TEXT_ALIGNMENT_RIGHT, labelText_->GetVerAlign());
        } else {
            labelText_->SetAlign(TEXT_ALIGNMENT_LEFT, labelText_->GetVerAlign());
        }
        if (hasAnimator_) {
            static_cast<LabelAnimator*>(animator_.animator)->UpdateWidth(textWidth, rectWidth);
        } else {
            LabelAnimator* animator = new LabelAnimator(textWidth, rectWidth, offsetX_, this);
            if (animator == nullptr) {
                GRAPHIC_LOGE("new LabelAnimator fail");
                return;
            }
            animator->SetAnimatorMode(mode);
            animator->SetAnimatorSpeed(animator_.speed);
            animator_.animator = animator;
            hasAnimator_ = true;
        }
        static_cast<LabelAnimator*>(animator_.animator)->SetMarqueeBlankWidth(CalcBlankSpaceWidth(marqueeBlankNum_));
        if (animator_.animator->GetState() == Animator::PAUSE) {
            animator_.animator->Resume();
        } else {
            animator_.animator->Start();
        }
    } else {
        offsetX_ = 0;
        nextOffsetX_ = rectWidth; // Set text outside the display area
        labelText_->SetAlign(horAlign_, labelText_->GetVerAlign());
        if (hasAnimator_) {
            animator_.animator->Stop();
        }
    }
}

void UILabel::SetRollStartPos(int16_t pos)
{
    if (hasAnimator_) {
        static_cast<LabelAnimator*>(animator_.animator)->SetStartPos(pos);
    } else {
        animator_.pos = pos;
    }
}

int16_t UILabel::GetRollStartPos() const
{
    return hasAnimator_ ? static_cast<LabelAnimator*>(animator_.animator)->GetStartPos() : animator_.pos;
}

void UILabel::SetRollSpeed(uint16_t speed)
{
    if (hasAnimator_) {
        static_cast<LabelAnimator*>(animator_.animator)->SetAnimatorSpeed(speed);
    } else {
        animator_.speed = speed;
    }
}

void UILabel::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    InitLabelText();
    uint8_t opa = GetMixOpaScale();
    Style style = GetStyleConst();
    style.textColor_ = GetTextColor();
    Rect origContentRect = GetOrigRect();
    if (transMap_ != nullptr) {
        transMap_->SetTransMapRect(origContentRect);
        labelText_->SetTransformMap(*transMap_);
    }
    LiteMGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, GetOrigRect(), invalidatedArea, *style_, opa, transMap_);
    origContentRect.SetX(origContentRect.GetX() + style_->paddingLeft_ + style_->borderWidth_);
    origContentRect.SetY(origContentRect.GetY() + style_->paddingTop_ + style_->borderWidth_);
    origContentRect.SetWidth(GetWidth());
    origContentRect.SetHeight(GetHeight());
    if (lineBreakMode_ != LINE_BREAK_TRUNCATE) {
        labelText_->OnDraw(gfxDstBuffer, invalidatedArea, GetOrigRect(),
                           origContentRect, offsetX_, style, ellipsisIndex_, opa);
        // when tOffsetX is a valid value, use nextOffsetX_ to redraw the text from the right side to left (achieve a looping display effect).
        if (lineBreakMode_ == LINE_BREAK_MARQUEE && nextOffsetX_ <= GetWidth()) {
            labelText_->OnDraw(gfxDstBuffer, invalidatedArea, GetOrigRect(),
                           origContentRect, nextOffsetX_, style, ellipsisIndex_, opa);
        }
    } else {
        labelText_->OnDraw(gfxDstBuffer, invalidatedArea, GetOrigRect(),
                           origContentRect, offsetX_, style, truncateIndex_, opa);
    }
}

void UILabel::SetAnimState(uint8_t state)
{
    if (setAnimState_ == state) {
        return;
    }

    switch (state) {
        case Animator::START:
            setAnimState_ = Animator::START;
            break;
        case Animator::STOP:
            setAnimState_ = Animator::STOP;
            break;
        case Animator::PAUSE:
            setAnimState_ = Animator::PAUSE;
            break;
        default:
            setAnimState_ = Animator::START;
            GRAPHIC_LOGE("Invalid state. Set to Animator::START instead!");
            break;
    }
}

uint8_t UILabel::GetAnimRealState()
{
    if (!hasAnimator_) {
        GRAPHIC_LOGE("Do not have animator.");
        return Animator::STOP;
    }
    return animator_.animator->GetState();
}
} // namespace OHOS
