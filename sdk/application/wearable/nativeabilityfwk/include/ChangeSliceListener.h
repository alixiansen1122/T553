/*
 * Copyright (c) CompanyNameMagicTag.
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

#ifndef CHANGE_SLICE_LISTENER_H
#define CHANGE_SLICE_LISTENER_H
#include <cstdint>

#include "AppViewIDs.h"
#include "components/ui_view.h"
#include "NativeAbility.h"

namespace OHOS {
class ChangeSliceListener : public UIView::OnTouchListener {
public:
    ChangeSliceListener(AppViewId nextAppId)
    {
        nextAppId_ = nextAppId;
    }
    ~ChangeSliceListener() {}

    bool OnPress(UIView &view, const PressEvent &event)
    {
        (void)view;
        (void)event;
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ChangeSliceListener, nextAppId_=%d", nextAppId_);
        NativeAbility::GetInstance().ChangeSlice(nextAppId_);
        return true;
    }
    bool OnRelease(UIView &view, const ReleaseEvent &event)
    {
        (void)view;
        (void)event;
        return true;
    }
    bool OnCancel(UIView &view, const CancelEvent &event)
    {
        (void)view;
        (void)event;
        return true;
    }

private:
     uint16_t nextAppId_;
};
}
#endif
