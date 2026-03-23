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

#include "dfx/fps_task.h"
#if ENABLE_DFX_CMD
#include <cmath>
#include <string>
#include "graphic_config.h"
#include "core/render_manager.h"
#include "common/screen.h"
#include "common/text.h"
#include "components/root_view.h"
#include "dfx/fps_task.h"
#include "hal_tick.h"

namespace OHOS {
static constexpr int16_t FPS_LABEL_FONT_SIZE = 30;
static constexpr int16_t FPS_LABEL_WIDTH = 70;
static constexpr int16_t FPS_LABEL_HEIGHT = 40;
static constexpr int16_t DEFAULT_FPS_LABEL_LEFT = 192;
static constexpr int16_t DEFAULT_FPS_LABEL_TOP = 180;
static constexpr uint32_t PRINT_TIME_PERIOD = 1000; // in ms
static constexpr uint32_t SHOW_TIME_PERIOD = 1000; // in ms
static constexpr uint32_t MILLESECOND_PER_SECOND = 1000; // in ms

FPSTask::FPSTask()
{
    uint32_t minPeriod = MATH_MIN(PRINT_TIME_PERIOD, SHOW_TIME_PERIOD);
    SetPeriod(minPeriod);
    Init();
}

void FPSTask::Callback()
{
    static uint16_t lastFps = 0;
    float fps = RenderManager::GetInstance().GetFPS();
    if (needShow_) {
        if (fpsLabel_ == nullptr) {
            return;
        }
        // always ensure that fpsLabel is on the top
        if (fpsLabel_ != RootView::GetInstance()->GetAppView()->GetChildrenTail()) {
            if (isAdded_) {
                RootView::GetInstance()->Remove(fpsLabel_);
            }
            RootView::GetInstance()->Add(fpsLabel_);
            isAdded_ = true;
        }
        uint32_t showElapse = HALTick::GetInstance().GetElapseTime(showStartTime_);
        if (showElapse >= (SHOW_TIME_PERIOD * showCount_)) {
            showCount_++;
            // use floor to decrease the influence of fpsLabel
            uint16_t fpsInt = static_cast<uint16_t>(std::floor(fps));
            if (fpsInt != lastFps) {
                fpsLabel_->SetText(std::to_string(fpsInt).c_str());
                lastFps = fpsInt;
            }
            fpsLabel_->SetPosition(left_, top_);
            fpsLabel_->Invalidate();
        }
    } else {
        if (isAdded_) {
            RootView::GetInstance()->Remove(fpsLabel_);
            RootView::GetInstance()->InvalidateRect(fpsLabel_->GetRect());
            isAdded_ = false;
        }
    }

    if (needPrint_) {
        uint32_t printElapse = HALTick::GetInstance().GetElapseTime(printStartTime_);
        if (printElapse >= (PRINT_TIME_PERIOD * printCount_)) {
            printCount_++;
            GRAPHIC_LOGE("Current FPS is %f", fps);
        }
        if (printElapse >= printDuration_) {
            needPrint_ = false;
        }
    }
}

void FPSTask::ShowFPS(int left, int top)
{
    if (fpsLabel_ == nullptr) {
        fpsLabel_ = new UILabel();
        fpsLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FPS_LABEL_FONT_SIZE);
        fpsLabel_->SetTextColor(Color::Red());
        fpsLabel_->SetWidth(FPS_LABEL_WIDTH);
        fpsLabel_->SetHeight(FPS_LABEL_HEIGHT);
        fpsLabel_->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER,
            UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
    }
    if (isFirstShow_) {
        left_ = ((left <= 0 || left >= Screen::GetInstance().GetWidth()) ? DEFAULT_FPS_LABEL_LEFT : left);
        top_ = ((top <= 0 || top >=  Screen::GetInstance().GetHeight()) ? DEFAULT_FPS_LABEL_TOP : top);
        isFirstShow_ = false;
    }
    showCount_ = 0;
    showStartTime_ = HALTick::GetInstance().GetTime();
    needShow_ = true;
}

void FPSTask::HideFPS()
{
    needShow_ = false;
}

void FPSTask::PrintFPS(uint16_t duration)
{
    // memory reorder will be ignored here in order to not affect performance
    printDuration_ = duration * MILLESECOND_PER_SECOND;
    printCount_ = 0;
    printStartTime_ = HALTick::GetInstance().GetTime();
    needPrint_ = true;
}
}
#endif