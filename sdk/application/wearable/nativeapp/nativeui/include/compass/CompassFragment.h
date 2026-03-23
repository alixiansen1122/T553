/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: CompassView.h
 * Author:
 * Create: 2021-12-21
 */

#ifndef COMPASS_VIEW_H
#define COMPASS_VIEW_H

#include "components/ui_label.h"
#include "components/ui_image_view.h"
#include "components/ui_fragment.h"

namespace OHOS {
class CompassFragment : public UIFragment {
public:
    void OnCreateView(void* data) override;
    void OnResumeView() override;
    void OnPauseView() override;
    void OnDestroyView() override;
    void Update(void);

private:
    bool InitCompassImage(void);
    bool LoadCompassImage(UIImageView *view, uint32_t resId);
    UILabel *directionLabel_{nullptr};
    UILabel *angleLabel_{nullptr};
    UIImageView *bgImg_{nullptr};
    UIImageView *fgImg_{nullptr};
    UIImageView *trianImg_{nullptr};
    bool initialized_{false};
};
}
#endif