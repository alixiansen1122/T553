  /*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: SphereDesktopFragment
 * Author: Hisi Graphic Team
 * Created: 2025-09
 */

#ifndef SPHERE_DESKTOP_FRAGMENT_H
#define SPHERE_DESKTOP_FRAGMENT_H

#include "UIDesktopFragment.h"
#include "components/ui_sphere_view.h"
#include "components/ui_transform_group.h"

namespace OHOS {
class SphereAnimatorCallback;
class SphereItemView;
class SphereDesktopFragment : public UIDesktopFragment {
public:
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnRotate(UIView& view, const RotateEvent& event) override;
    void AddAppItemToList(const AppItem &item) override;
    void ClearAppItemToList() override;
    void RefreshAppList() override;

protected:
    void OnCreateView(void* data) override;
    void OnDestroyView() override;
    void OnResumeView() override;

private:
    UISphereView* sphereView_{nullptr};
    List<SphereItemView*> listSphereView_;
    SphereAnimatorCallback* callback_ = nullptr;
    Animator* enterAnimator_ = nullptr;
};

class SphereAnimatorCallback : public AnimatorCallback {
public:
    explicit SphereAnimatorCallback(UIView* view, int16_t width, int16_t height)
        : width_(width),
          height_(height),
          startTime_(0),
          passTime_(0),
          angle_(0),
          animator_(new Animator(this, view, 600, false)) {}  // 600 ms:duration of animator_

    ~SphereAnimatorCallback() override
    {
        if (animator_ != nullptr) {
            delete animator_;
            animator_ = nullptr;
        }
    }

    void Callback(UIView* view) override;

    Animator* GetAnimator() const
    {
        return animator_;
    }

    void SetStartTime(uint32_t time)
    {
        isStartAnim_ = false;
        startTime_ = time;
    }
protected:
    int16_t width_;
    int16_t height_;
    uint32_t startTime_;
    uint32_t passTime_;
    bool isStartAnim_ = false;
    int16_t angle_;
    Animator* animator_;
};

class SphereItemView : public UITransformGroup {
public:
    explicit SphereItemView() {};
    ~SphereItemView();
    bool SetItemInfo(const AppItem &itemInfo);
    AppViewId GetViewId();
    const char *GetAppUid();
private:
    SphereItemView(const SphereItemView &);
    SphereItemView &operator = (const SphereItemView &);
    UIImageView* imageView_{nullptr};
    AppItem itemInfo_;
};
}

#endif // SPHERE_DESKTOP_FRAGMENT_H
