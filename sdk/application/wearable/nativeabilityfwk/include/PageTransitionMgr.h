/*
 * Copyright (c) CompanyNameMagicTag 2024. All rights reserved.
 * Description: PageTransitionMgr
 * Author:
 * Create: 2024-12
 */

#ifndef PAGE_TRANSITION_H
#define PAGE_TRANSITION_H
#include <functional>
#include <memory>
#include "animator/animator.h"
#include "gfx_utils/vector.h"
#include "graphic_mutex.h"
#include "TransitionCallback.h"
#include "TransitionType.h"
#include "gfx_utils/image_info.h"

namespace OHOS {
using TransitionAnimatorStopCallback = std::function<void()>;
using PendingTransition = std::function<void()>;
static const uint8_t MAX_SLIDE_BACK_NUM = 3;

class UIImageViewWithInfo : public UIImageView {
public:
    UIImageViewWithInfo() : UIImageView() {}
    virtual ~UIImageViewWithInfo()
    {
        UIViewGroup* parent = dynamic_cast<UIViewGroup*>(parent_);
        if (parent != nullptr) {
            parent->Remove(this);
        }
        const ImageInfo* info = GetImageInfo();
        if (info != nullptr) {
            ImageCacheFree(*const_cast<ImageInfo*>(info));
        }
    }
};

class PageTransitionMgr;
class PageTransAnimCallback : public AnimatorCallback {
public:
    PageTransAnimCallback(PageTransitionMgr* transMgr) : pageTransMgr_(transMgr) {}
    virtual ~PageTransAnimCallback() {}

    void Callback(UIView* view) override;
    void OnStop(UIView& view) override;
    void ForceStop()
    {
        needStopped_ = true;
    }
private:
    bool isStarted_ = false;
    bool needStopped_ = false;
    uint32_t startTime_ = 0;
    PageTransitionMgr* pageTransMgr_ = nullptr;
};
#ifdef JS_ENABLE
enum class AbilityType : uint8_t {
    NATIVE_ABILITY,
    JS_ABILITY,
    INVALID,
};
#endif
class PageTransitionMgr : public HeapBase {
public:
    friend class PageTransAnimCallback;
    static PageTransitionMgr& GetInstance()
    {
        static PageTransitionMgr instance;
        return instance;
    }
    // if return false, the transition will be cached, and should return immediately
    bool BeginTransition(TransitionCaller caller, TransitionTarget target, TransitionType type, bool enableSlideBack);
    void BeginBackTransition(TransitionCaller caller, TransitionTarget target);
    // this function is called by application when slice is on pause phase if it need to custom its slideback snapshot
    void SaveSlideBackSnapShotIfNecessary(UIView* view);
    UIImageView* GetTopSlideBackImage();
    /**
     * @brief Check whether slide back is enabled currently, equivalent to:
     *             View::GetBackDragListener()
     *        Note: Don't use canSlideBack_ because it will be reset after transition
     */
    bool CanSlideBack()
    {
        return (GetTopSlideBackImage() != nullptr);
    }
    // remove top slice back image from rootview and delete it
    void ClearTopSlideBackImage();
    void SaveCurrentSourceIfNecessary(UIView* view);
    void SaveTargetSource(UIView* view);
    void StartTransAnim(const TransitionAnimatorStopCallback& transition);
    void SyncStopAnimator();
    void AsyncStopAnimator();
    void EndTransition(bool success);
    bool IsCurrentSnapshotEnabled()
    {
        return isCurrentSnapShotEnabled_;
    }
    bool IsTransitionRunning()
    {
        return isTransitionRunning_;
    }
    bool IsTransAnimNeeded()
    {
        return isTransAnimNeeded_;
    }
    bool IsTransAnimRunning()
    {
        return animator_ == nullptr ? false : (animator_->GetState() != Animator::STOP);
    }
    // if IsBackTransitionRunning_ is true, isTransitionRunning_ must be true
    bool IsBackTransitionRunning()
    {
        return transitionCaller_ == TransitionCaller::BACK_TRANSITION;
    }

    /**
     * @brief   Set current app's recent img source. Should be invoked in OnResume method.
     */
    void SetRecentAppImg(uint32_t resId, std::string file);

    /**
     * @brief   Set min lasting period of an app so that it could be added to recent manager.
     */
    void SetRecentMinPeriod(uint32_t minPeriod);

private:
    PageTransitionMgr() {};
    ~PageTransitionMgr() {};
    void SkipTranAnim();
    void SaveSlideBackSnapShotInner(UIView* view, bool custom);
    void SaveCurrentSnapShotInner(UIView* view);
    void ClearSlideBackHistory();
    bool SnapshotRecentSource(UIView* view);
    void AddRecentAppIfNecessary();
    bool FillRecentSnapshot(bool& needToFreeSnapshot);
    Animator* animator_ = nullptr;
    PageTransAnimCallback* animatorCallback_ = nullptr;
    std::shared_ptr<UIImageViewWithInfo> currentSlideBackSnapshot_ = nullptr;
    std::shared_ptr<UIImageViewWithInfo> currentSnapshot_ = nullptr;
    std::shared_ptr<UIImageViewWithInfo> targetSnapshot_ = nullptr;
    ImageInfo recentSnapshot_ = {0};
    uint32_t recentAppResId_ = 0;
    std::string recentAppFile_ = "";
    std::shared_ptr<UIImageViewWithInfo> slideBackHistroy_[MAX_SLIDE_BACK_NUM] = {nullptr};
    uint8_t curSlideBackNum_ = 0;
    UIViewGroup* currentView_ = nullptr;
    UIViewGroup* targetView_ = nullptr;
    TransitionCallback* transitionCallback_ = nullptr;
    TransitionAnimatorStopCallback animStopCallback_;
#ifdef JS_ENABLE
    AbilityType lastAbility_{AbilityType::INVALID};
#endif
    TransitionCaller transitionCaller_{TransitionCaller::INVALID};
    TransitionCaller slideBackCaller_{TransitionCaller::INVALID};
    TransitionTarget slideBackTarget_{{0, nullptr, 0}};
    bool isTransAnimNeeded_ = false;
    bool isCurrentSnapShotEnabled_ = false;
    bool isTargetSnapShotEnabled_ = false;
    bool isTransitionRunning_ = false;
    bool canSlideBack_ = false;
    GraphicMutex operationLock_;
    void ClearTransitionResource();
    ImageInfo RectifySnapshot(ImageInfo& dstInfo);
    uint32_t lastTransitionTime_ = 0;
    uint32_t recentMinPeriod_ = 0;
    bool recentMinPeriodSatisfied_ = true;
    bool isSnapshotRecent_ = false;
    uint16_t recentSliceId_ = 0; // invalid slice
    uint32_t recentTargetId_ = 0; // invalid slice
};
} // namespace OHOS
#endif // PAGE_TRANSITION_MGR_H