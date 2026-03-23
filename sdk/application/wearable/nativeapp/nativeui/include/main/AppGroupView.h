/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: APPVEIW
 * Author:
 * Create: 2021-10-15
 */

#ifndef APP_GROUP_VIEW_H
#define APP_GROUP_VIEW_H

#include <string>
#include "View.h"
#include "components/ui_card_page.h"
#include "common/screen.h"

namespace OHOS {
class AppGroupView : public UICardPage {
public:
    AppGroupView()
    {
        SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    }
    ~AppGroupView() override {}
    virtual bool InitView(void *caller)=0;
    bool initViewStatus{false};
};
} // namespace OHOS
#endif // APP_GROUP_VIEW_H
