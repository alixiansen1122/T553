/*
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: FootballDesktopFragment.h
 * Author:
 * Create: 2025-07-04
 */

#ifndef FOOTBALL_DESKTOP_FRAGMENT_H
#define FOOTBALL_DESKTOP_FRAGMENT_H

#include "UIDesktopFragment.h"
#include "components/ui_icosahedron_view.h"

namespace OHOS {
class FootballAnimatorCallback;
class FootballItemView;
class FootballDesktopFragment : public UIDesktopFragment {
public:
    bool OnClick(UIView &view, const ClickEvent &event) override;
    void AddAppItemToList(const AppItem &item) override;
    void ClearAppItemToList() override;
    void RefreshAppList() override;

protected:
    void OnCreateView(void* data) override;
    void OnDestroyView() override;
    void OnResumeView() override;

private:
    UIIcosahedronView* footballList_ = nullptr;
    List<FootballItemView*> footballItemList_;
    ImageInfo* footballDefaultImg_ = nullptr;
    FootballAnimatorCallback* callback_ = nullptr;
    Animator* enterAnimator_ = nullptr;
};

class FootballAnimatorCallback : public AnimatorCallback {
public:
    explicit FootballAnimatorCallback(UIView* view, int16_t startPos, int16_t endPos)
        : startPos_(startPos),
          endPos_(endPos),
          startTime_(0),
          animator_(new Animator(this, view, 1000, false)) {}  // 1000:duration of animator_, in milliseconds.

    ~FootballAnimatorCallback() override
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
    int16_t startPos_;
    int16_t endPos_;
    uint32_t startTime_;
    bool isStartAnim_ = false;
    Animator* animator_;
};

class FootballItemView : public UIImageView {
public:
    explicit FootballItemView() {}
    ~FootballItemView() override;
    bool SetItemInfo(const AppItem &itemInfo);
    AppViewId GetViewId();
    const char *GetAppUid();
private:
    FootballItemView(const FootballItemView &);
    FootballItemView &operator = (const FootballItemView &);
    AppItem itemInfo_;
};
}

#endif // FOOTBALL_DESKTOP_FRAGMENT_H
