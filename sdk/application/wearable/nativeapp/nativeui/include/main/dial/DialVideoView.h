/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef DIAL_VIDEO_VIEW
#define DIAL_VIDEO_VIEW

#include <cstdint>
#include "components/ui_lite_surface_view.h"
#include "video_play_wrapper.h"

namespace OHOS {
class DialVideoView : public UILiteSurfaceView {
public:
    DialVideoView();
    ~DialVideoView() override;
    bool SetVideoRes(FILE* fp, uint32_t offset);
    bool StartPlay();
    bool ResumePlay();
    bool PausePlay();
    bool StopPlay();
    bool IsPaused();
    bool IsPlayed();
    bool IsStopped();
    MediaVideoPlay* GetVideoPlay();

private:
    FILE* fp_ = nullptr;
    uint32_t offset_ = 0;
    MediaVideoPlay* videoPlayer_ = nullptr;
};
}
#endif