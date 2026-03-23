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

#ifndef UI_TEST_VIDEO_PLAY_H
#define UI_TEST_VIDEO_PLAY_H

#include "ui_test.h"
#include "common/screen.h"
#include "components/root_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_lite_surface_view.h"
#include "animator/animator.h"
#include "video_play_wrapper.h"

namespace OHOS {
class VideoAnimatorCallback : public AnimatorCallback {
public:
    explicit VideoAnimatorCallback(UILabel *label) : label_(label) {}
    ~VideoAnimatorCallback() override {}
    void Callback(UIView *view) override;

private:
    UILabel* label_ = nullptr;
};

class UITestVideoPlay : public UITest, public UIView::OnClickListener {
public:
    UITestVideoPlay() {}
    ~UITestVideoPlay() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    ColorType colorKey_;
    UIScrollView *container_ = nullptr;
    UILabel *label_ = nullptr;
    UILabelButton *button1_ = nullptr; // Refer to start and stop
    UILabelButton *button2_ = nullptr; // Refer to pause and resume
    UILiteSurfaceView *surfaceView_ = nullptr;
    VideoAnimatorCallback *callBack_ = nullptr;
    Animator *animator_ = nullptr;
    MediaVideoPlay *videoPlay_ = nullptr;
};
}
#endif // UI_TEST_VIDEO_PLAY_H
