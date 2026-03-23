/*
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: voice assistant
 * Author: CompanyName
 * Create: 2022-01-18
 */

#ifndef VOCASSIST_PRESENTER_H
#define VOCASSIST_PRESENTER_H

#include "Presenter.h"
#include "NativeAbility.h"
#include "offlinevoice/VocassistView.h"
#include "offlinevoice/MainVocassistView.h"

namespace OHOS {
class MainVocassistView;
class VocassistPresenter : public Presenter<MainVocassistView>, public UIView::OnClickListener,
    public UIView::OnDragListener, public UIImageAnimatorView::AnimatorStopListener {
public:
    VocassistPresenter();
    ~VocassistPresenter();
    void OnResume() override;
    void OnStop() override;
    static VocassistPresenter *GetInstance();
private:
    bool OnClick(UIView& view, const ClickEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    void OnAnimatorStop(UIView& view) override;
    void VocAssistDeal(int flag);
};
}

#endif