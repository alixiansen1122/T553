/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 */

#ifndef VIDEO_CARD_H
#define VIDEO_CARD_H

#include <string>
#include "main/dial/DialDataBase.h"
#include "components/ui_view_group.h"
#include "main/dial/OnDialDataUpdateListener.h"
#include "components/ui_card_page.h"
#include "components/ui_lite_surface_view.h"
#include "video_play_wrapper.h"
#include "main/dial/DialView.h"

namespace OHOS {
class VideoCard : public UICardPage {
public:
    VideoCard(const VideoCard &) = delete;
    VideoCard &operator=(const VideoCard &) = delete;

    VideoCard();

    ~VideoCard() override;

    /**
    * @brief  This function is invoked when the function SetHorCurrentPage() is called.
    */
    void OnActive(void) override;

    void OnInactive() override;

    void PreLoad(void) override;

    void ScrollBegin(bool isActive) override;

    void ScrollEnd(bool isActive) override;

    void SetFilePath(const std::string &path);

    void SetPreviewFile(const std::string &path);

    void OnPause(void) override;

private:
    void StopVideo();

    void StartVideo();

    void PauseVideo();

    void Deinit();

    void Init();

    UILiteSurfaceView *surfaceView_ = nullptr;
    MediaVideoPlay *video_ = nullptr;
    bool resPreloaded_ = false;
    bool hasPreview_ = false;
    std::string path_;
    std::string previewPath_;
};
} // OHOS
#endif // VIDEO_CARD_H
