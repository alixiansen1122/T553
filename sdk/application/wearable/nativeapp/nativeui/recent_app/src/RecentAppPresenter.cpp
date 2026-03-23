/*
 * Copyright (c) 2025 CompanyNameMagicTag.
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

#include "recent_app/RecentAppPresenter.h"
#include "recent_app/RecentAppView.h"
#include "RecentManager.h"
#include "NativeRegisterManager.h"
#include "msg_center_customer.h"

namespace OHOS {

REGIST_SLICE(VIEW_RECENT_APP, RecentAppView, RecentAppPresenter);

void RecentAppPresenter::OnResume()
{
    if (view_ != nullptr) {
        view_->OnResume();
    }
}
}
