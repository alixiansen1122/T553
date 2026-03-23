/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: UITestCross
 * Author:
 * Create: 2023-09
 */

#ifndef UI_TEST_CROSS_H
#define UI_TEST_CROSS_H

#include "ui_test.h"
#include "common/screen.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_cross_view.h"
#include "components/ui_lite_surface_view.h"
#include "swipe_effect/card_flip_effect.h"
#include "ui_test_clock_dial/PullUpDownView.h"
#ifndef _WIN32
#include "video_play_wrapper.h"
#endif

namespace OHOS {
class UITestCross : public UITest, UIView::OnClickListener {
public:
    class VideoAnimatorCallback : public AnimatorCallback {
    public:
        explicit VideoAnimatorCallback(UILabel *label) : label_(label) {}
        ~VideoAnimatorCallback() override {}
        void Callback(UIView *view) override;

    private:
        UILabel* label_ = nullptr;
    };

#ifndef _WIN32
    class VideoPage : public UICardPage {
    public:
        VideoPage(const VideoPage &) = delete;
        VideoPage &operator=(const VideoPage &) = delete;
        VideoPage(void) {}
        ~VideoPage() override;
        void ScrollBegin(bool isActive) override;
        void ScrollEnd(bool isActive) override;
        void CoverBegin(bool isCovered) override;
        void CoverEnd(bool isCovered) override;
        void OnCovered() override;
        void OnUncovered() override;
        void PageInit(void);
        void StartVideo(void);

    private:
        UILabel* label_ = nullptr;
        ColorType colorKey_;
        UILiteSurfaceView* surfaceView_ = nullptr;
        VideoAnimatorCallback* callBack_ = nullptr;
        Animator* animator_ = nullptr;
        MediaVideoPlay *videoPlay_ = nullptr;
        ImageInfo* previewInfo_ = nullptr;
    };
#endif
    UITestCross() {}
    ~UITestCross() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    UICrossView* container_ = nullptr;
    UICardPage* card1_ = nullptr;
    UICardPage* card2_ = nullptr;
    UICardPage* card3_ = nullptr;
    UICardPage* card4_ = nullptr;
    PullUpDownView *up_ = nullptr;
    UICardPage *down_ = nullptr;
    CardFlipEffect *callback_ = nullptr;
#ifndef _WIN32
    VideoPage* video_ = nullptr;
#endif
    void SetUpPage(UICardPage** card, uint32_t resId);
};
}
#endif
