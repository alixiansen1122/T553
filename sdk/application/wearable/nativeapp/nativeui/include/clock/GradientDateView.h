/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: GradientDateView
 * Create: 2025-04
 */

#ifndef GRADIENT_DATE_VIEW_H
#define GRADIENT_DATE_VIEW_H

#include <vector>
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "main/dial/DialDigitalImgView.h"

namespace OHOS {

class GradientDateView : public UIViewGroup {
public:
    GradientDateView();
    ~GradientDateView();
    GradientDateView(const GradientDateView &) = delete;
    GradientDateView &operator=(const GradientDateView &) = delete;
    bool InitView(void);
    void UpdateTime(void);

private:
    bool LoadClockImageById(UIImageView* view, uint32_t resId);
    bool LoadGradientDateImages(std::vector<ImageInfo*> &imgInfos, std::vector<int> resIds, uint16_t length);
    void InitDigitalImgView(DialDigitalImgView* &dialView, Point pos, uint16_t integerLength,
        std::vector<ImageInfo*> &numImgInfo);
    void HandleDigitalData(DialDigitalImgView* digitalView, float data);
    void InitWeekImgView(void);
    void InitDateImgViews(void);
    void InitTimeImgViews(void);
    UIImageView weekImgView_;
    std::vector<ImageInfo*> weekInfo_;
    DialDigitalImgView* yearImgView_{nullptr};
    DialDigitalImgView* monthImgView_{nullptr};
    DialDigitalImgView* dayImgView_{nullptr};
    UIImageView dateSplitLeft_;
    UIImageView dateSplitRight_;
    std::vector<ImageInfo*> dateNumImgInfo_;
    DialDigitalImgView* hourImgView_{nullptr};
    DialDigitalImgView* minuteImgView_{nullptr};
    std::vector<ImageInfo*> timeNumImgInfo_;
    UIImageView timeSplit_;
    UIImageView* backgroudImage_;
    bool viewiInitStatus_{false};
}; // class GradientDateView

} // namespace OHOS

#endif // GRADIENT_DATE_VIEW_H