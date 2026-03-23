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

#ifndef RECENT_APP_VIEW_H
#define RECENT_APP_VIEW_H

#include "View.h"
#include "RecentAppPresenter.h"
#include "LoadingFragment.h"
#include "animator/animator.h"

namespace OHOS {
class RecentAppView : public View<RecentAppPresenter> {
public:
    RecentAppView() {}
    ~RecentAppView() override;

    void OnStart() override;
    void OnResume();

private:
    class LoadingAnimCb : public AnimatorCallback {
    public:
        LoadingAnimCb() {}
        ~LoadingAnimCb() override {}
        void Callback(UIView* view) override;
        void SetRecentAppView(RecentAppView* recentAppView)
        {
            recentAppView_ = recentAppView;
        }
    private:
        RecentAppView* recentAppView_;
    };

    void LoadContent();
    bool viewiInitStatus_ = false;
    LoadingAnimCb loadingAnimCb_;
    Animator* loadingAnim_ = nullptr;
    LoadingFragment *loadingFragment_ = nullptr;
    UIFragment *rencentAppFragment_ = nullptr;
};
} // namespace OHOS
#endif // RECENT_APP_VIEW_H