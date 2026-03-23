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

#include "recent_app/RecentAppView.h"
#include "recent_app/RollerRecentAppFragment.h"
#include "wearable_log.h"
#include "common/image_cache_manager.h"
#include "common/screen.h"
#include "Slice.h"
#include "RecentManager.h"

namespace OHOS {

RecentAppView::~RecentAppView()
{
    if (loadingFragment_ != nullptr) {
        loadingFragment_->DestroyView();
        delete loadingFragment_;
        loadingFragment_ = nullptr;
    }
    if (rencentAppFragment_ != nullptr) {
        rencentAppFragment_->DestroyView();
        delete rencentAppFragment_;
        rencentAppFragment_ = nullptr;
    }
    if (loadingAnim_ != nullptr) {
        if (loadingAnim_->GetState() != Animator::STOP) {
            loadingAnim_->Stop();
        }
        delete loadingAnim_;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "RecentAppView::~RecentAppView");
}

void RecentAppView::OnStart(void)
{
    loadingAnimCb_.SetRecentAppView(this);

    if (loadingFragment_ == nullptr) {
        loadingFragment_ = new LoadingFragment();
    }
    loadingFragment_->GetFragmentView()->SetPosition(0, 0, GetRootContainer()->GetWidth(),
                                                     GetRootContainer()->GetHeight());
    loadingFragment_->CreateView();
    AddViewToRootContainer(loadingFragment_->GetFragmentView());
}

void RecentAppView::OnResume()
{
    if (viewiInitStatus_) {
        return;
    }

    if (!RecentManager::GetInstance()->IsUpdateCompleted()) {
        loadingAnim_ = new Animator(&loadingAnimCb_, nullptr, 0, true);
        if (loadingAnim_ == nullptr) {
            return;
        }
        loadingAnim_->Start();
    } else {
        LoadContent();
    }
    viewiInitStatus_ = true;
}

void RecentAppView::LoadContent()
{
    if (loadingAnim_ != nullptr) {
        loadingAnim_->Stop();
    }

    if (loadingFragment_ != nullptr) {
        loadingFragment_->GetFragmentView()->SetVisible(false);
    }
    if (rencentAppFragment_ == nullptr) {
        rencentAppFragment_ = new RollerRecentAppFragment();
    }
    if (rencentAppFragment_ != nullptr) {
        rencentAppFragment_->GetFragmentView()->SetPosition(0, 0, GetRootContainer()->GetWidth(),
                                                            GetRootContainer()->GetHeight());
        rencentAppFragment_->CreateView();
        AddViewToRootContainer(rencentAppFragment_->GetFragmentView());
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new RollerRecentAppFragment failed!");
    }
}

void RecentAppView::LoadingAnimCb::Callback(UIView* view)
{
    if (RecentManager::GetInstance()->IsUpdateCompleted()) {
        recentAppView_->LoadContent();
    }
}
}