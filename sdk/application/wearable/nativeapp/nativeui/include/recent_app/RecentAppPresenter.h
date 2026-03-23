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

#ifndef RECENT_APP_PRESENTER_H
#define RECENT_APP_PRESENTER_H

#include "Presenter.h"
#include "components/ui_view.h"

namespace OHOS {
class RecentAppView;
class RecentAppPresenter : public Presenter<RecentAppView> {
public:
    RecentAppPresenter() {}
    ~RecentAppPresenter() override {}
    void OnResume() override;
};
}

#endif // RECENT_APP_PRESENTER_H