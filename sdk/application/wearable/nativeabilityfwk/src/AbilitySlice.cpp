/*
 * Copyright (c) CompanyNameMagicTag 2025. All rights reserved.
 * Description: AbilitySlice
 * Author: Hisi Graphic Team
 * Create: 2025-4
 */

#include "AbilitySlice.h"
#include "components/root_view.h"
#include "dock/focus_manager.h"
#include "wearable_log.h"
#include "PageTransitionMgr.h"
#include "NativeAbility.h"
#include "View.h"
#include "Presenter.h"
#include "key/KeyInputListener.h"
#include "common/image_cache_manager.h"

namespace OHOS {
const uint16_t SLICE_MASK = 0xFFFF;
const uint32_t PAGE_OFFSET = 16;

void AbilitySlice::OnStart(void* data)
{
    if ((presenter_ == nullptr) || (view_ == nullptr)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "Presenter or View  was nullptr for slice %s", sliceId_);
        return;
    }

    // if back to current slice, delete slide back ImageView of current slice firstly
    if (PageTransitionMgr::GetInstance().IsBackTransitionRunning()) {
        PageTransitionMgr::GetInstance().ClearTopSlideBackImage();
    }
    presenter_->Attach(view_);
    view_->Attach(presenter_);
    slicePageMgr_.AttachVP(presenter_, view_);

    view_->SetupView();
    view_->OnStart();
    view_->OnStart(data);
    slicePageMgr_.StartSlicePage(data);
    presenter_->OnStart();
    presenter_->OnStart(data);
    state_ = SliceState::START;
}

void AbilitySlice::OnResume()
{
    if ((state_ != SliceState::START) && (state_ != SliceState::PAUSE)) { // avoid repeated resume
        return;
    }
    if ((presenter_ == nullptr) || (view_ == nullptr)) {
        return;
    }

    if (view_->GetRootContainer()->GetParent() == nullptr) {
        RootView::GetInstance()->Add(view_->GetRootContainer());
    }
    // Reset the default key listener
    if (!ImageCacheManager::GetInstance().IsInAod()) {
        RootView::GetInstance()->SetOnKeyActListener(KeyInputListener::GetInstance());
    }
    NativeAbility::GetInstance().SetCurSlicePriority(gSliceDefaultPriority);

    UIView* backView = PageTransitionMgr::GetInstance().GetTopSlideBackImage();
    BackDragListener* backDragListener = view_->GetBackDragListener();
    if ((backView != nullptr) && ((backDragListener != nullptr))) {
        backDragListener->SetCurrentAndTargetView(view_->GetRootContainer(), backView);
        backDragListener->SetParentView(RootView::GetInstance());
        backDragListener->SetDragToTargetAction([]() { NativeAbility::GetInstance().BackToPreSlice(); });
        view_->GetRootContainer()->SetTouchable(true);
        view_->GetRootContainer()->SetDraggable(true);
        view_->GetRootContainer()->SetOnDragListener(backDragListener);
    }

    slicePageMgr_.ResumeSlicePage();
    presenter_->OnResume();
    view_->Draw();
    state_ = SliceState::RESUME;
}

void AbilitySlice::OnPause()
{
    if (state_ != SliceState::RESUME) { // avoid repeated pause
        return;
    }
    if ((presenter_ == nullptr) || (view_ == nullptr)) {
        return;
    }

    if (slicePageMgr_.IsAnimatorRunning()) {
        slicePageMgr_.StopSwitchAnimator();
    }
    slicePageMgr_.PauseSlicePage();
    RootView::GetInstance()->ClearOnKeyActListener();
    BackDragListener* backDragListener = view_->GetBackDragListener();
    if (backDragListener != nullptr) {
        backDragListener->ClearDragState();
    }
    view_->GetRootContainer()->SetOnDragListener(nullptr);

    FocusManager::GetInstance()->ClearFocus();
    presenter_->OnPause();
    state_ = SliceState::PAUSE;
}

void AbilitySlice::OnStop()
{
    if ((presenter_ == nullptr) || (view_ == nullptr)) {
        return;
    }

    slicePageMgr_.StopSlicePage();
    view_->TearDownView();
    view_->OnStop();
    delete view_;
    view_ = nullptr;

    presenter_->OnStop();
    delete presenter_;
    presenter_ = nullptr;

    state_ = SliceState::STOP;
}

UIViewGroup* AbilitySlice::GetSliceContainer()
{
    if (view_ != nullptr) {
        return view_->GetRootContainer();
    } else {
        return nullptr;
    }
}

bool AbilitySlice::ChangeSlicePage(uint16_t pageId, void* data, TransitionType type, bool canBack)
{
    return slicePageMgr_.ChangeSlicePage(pageId, data, type, canBack);
}

bool AbilitySlice::BackToPrevSlicePage()
{
    return slicePageMgr_.BackToPrevSlicePage();
}

bool AbilitySlice::BackToCachedSlicePage(uint16_t pageId)
{
    return slicePageMgr_.BackToCachedSlicePage(pageId);
}
}