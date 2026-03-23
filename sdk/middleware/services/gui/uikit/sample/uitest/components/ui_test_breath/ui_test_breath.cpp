/*
 * Copyright (c) 2023 CompanyNameMagicTag.
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

#include "ui_test_breath.h"
#include "common/image_cache_manager.h"
#include "animator/easing_equation.h"
namespace OHOS {
const static int16_t BUTTON_WIDTH = 50;
const static int16_t BUTTON_HEIGHT = 50;
const static int16_t INTERVAL_STEP = 80;
const Vector2<float> VIEW_CENTER = { 170, 170 };

void BreathAnimatorCallback::Callback(UIView *view)
{
    curTime = HALTick::GetInstance().GetTime() - startTimestamp;
    if (curTime >= durationTime) {
        curTime = durationTime;
        scale =
            static_cast<float>(EasingEquation::QuadEaseInOut(startVal, endVal, curTime, durationTime)) / durationTime;
        int16_t temp = startVal;
        startVal = endVal;
        endVal = temp;
        startTimestamp = HALTick::GetInstance().GetTime();
        isBreathIn = !isBreathIn;
        if (isBreathIn) {
            breath->SetText("吸气");
        } else {
            breath->SetText("呼气");
        }
    } else {
        scale =
            static_cast<float>(EasingEquation::QuadEaseInOut(startVal, endVal, curTime, durationTime)) / durationTime;
    }
    angle += step;
    if (angle <= -360) {  // 360: degrees per cycle
        angle = 0;
    }

    img_->Scale({ scale, scale }, VIEW_CENTER);
    img_->Rotate(angle, VIEW_CENTER);
    img_->Invalidate();
}

void UITestBreath::SetUp()
{
    container_ = new UIViewGroup();
    container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());

    ImageInfo* imgInfo = ImageCacheManager::GetInstance().LoadSingleRes(RES_PATH"breath.bin");
    if (imgInfo == nullptr) {
        GRAPHIC_LOGE("Load image failed!");
        return;
    }

    img_ = new UIImageView();
    img_->SetSrc(imgInfo);
    img_->SetPosition(57, 27, 340, 340); // 57, 27, 340, 340: x, y, w, h
    img_->SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    img_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    img_->SetAutoEnable(false);
    img_->SetResizeMode(UIImageView::ImageResizeMode::CENTER);
    container_->Add(img_);

    start_ = new UILabelButton();
    start_->SetPosition(180, 380, 100, 50); // 180, 380, 100, 50: left, right, width, height
    start_->SetText("Start");
    start_->SetOnClickListener(this);
    container_->Add(start_);
}

void UITestBreath::TearDown()
{
    if (ballAnimator_ != nullptr) {
        ballAnimator_->Stop();
        delete ballAnimator_;
        ballAnimator_ = nullptr;
    }
    if (callBack_ != nullptr) {
        delete callBack_;
        callBack_ = nullptr;
    }

    DeleteChildren(container_);
    ImageCacheManager::GetInstance().UnloadSingleRes(RES_PATH"breath.bin");
    container_ = nullptr;
    img_ = nullptr;
    breath_ = nullptr;
}

bool UITestBreath::OnClick(UIView& view, const ClickEvent& event)
{
    if (!animateStart_) {
        ballAnimator_->Start();
        animateStart_ = false;
        if (start_ != nullptr) {
            container_->Remove(start_);
            delete start_;
            start_ = nullptr;
        }
        breath_ = new UILabel();
        breath_->SetPosition(180, 380, 100, 50); // 180, 380, 100, 50: left, right, width, height
        breath_->SetText("呼气");
        breath_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 40); // 40: font size
        breath_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        breath_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
        container_->Add(breath_);
        callBack_->SetTextView(breath_);

        callBack_->startTimestamp = HALTick::GetInstance().GetTime();
        container_->Invalidate();
    }
    return true;
}

const UIView* UITestBreath::GetTestView()
{
    CaseInit();
    return container_;
}

void UITestBreath::CaseInit()
{
    callBack_ = new BreathAnimatorCallback(img_);
    if (callBack_ == nullptr) {
        GRAPHIC_LOGE("Tiger callBack new fail");
        return;
    }
    ballAnimator_ = new Animator(callBack_, img_, 0, true);
    if (ballAnimator_ == nullptr) {
        delete callBack_;
        callBack_ = nullptr;
        GRAPHIC_LOGE("tigerAnimator new fail");
        return;
    }
    animateStart_ = false;
}
}
