/*
 * Copyright (c) @CompanyNameMagicTag. 2024. All rights reserved.
 */

#ifndef UI_TEST_PARTICLE_H
#define UI_TEST_PARTICLE_H

#include "ui_test.h"
#include "components/ui_particle_view.h"
#include "components/particle_cell.h"

namespace OHOS {
class UITestParticle : public UITest, public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    UITestParticle() {}
    ~UITestParticle() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
    bool OnDragStart(UIView& view, const DragEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;

private:
    void SetUpEnterParticleCells();
    UIParticleView* particleView_ = nullptr;
    ParticleCell* dragCell_ = nullptr;
    UIViewGroup* container_ = nullptr;
};
}
#endif