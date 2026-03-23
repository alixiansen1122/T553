/*
 * Copyright (c) CompanyNameMagicTag 2025. All rights reserved.
 * Description: Slice Page
 * Author: Hisi Graphic Team
 * Create: 2025-4
 */
#ifndef SLICE_PAGE_H
#define SLICE_PAGE_H
#include "components/ui_view_group.h"
#include "Presenter.h"

namespace OHOS {
class SlicePageBase {
public:
    SlicePageBase() : pageContainer_(nullptr), backDragListener_(nullptr) {}

    virtual ~SlicePageBase() = default;

    virtual void OnStart(void* data) = 0;

    virtual void OnResume() {}

    virtual void OnPause() {}

    virtual void OnStop() {}

    UIViewGroup* GetSlicePageContainer()
    {
        return pageContainer_;
    }

    BackDragListener* GetBackDragListener()
    {
        return backDragListener_;
    }

    void AddViewToPageContainer(UIView* view);

    virtual void AttachPresenter(PresenterBase* p) = 0;

    SlicePageBase(const SlicePageBase&) = delete;
    SlicePageBase& operator=(const SlicePageBase&) = delete;
    SlicePageBase(SlicePageBase&&) = delete;
    SlicePageBase& operator=(SlicePageBase&&) = delete;
    uint16_t pageId_;

private:
    friend class SlicePageManager;
    void SetupView(bool canBack);
    void TearDownView();
    UIViewGroup* pageContainer_;
    BackDragListener* backDragListener_;
};

template<class P>
class SlicePage : public SlicePageBase {
public:
    void AttachPresenter(PresenterBase* p) override
    {
        presenter_ = dynamic_cast<P*>(p);
    }
protected:
    P* presenter_ = nullptr;
};
}
#endif