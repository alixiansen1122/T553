/*
 * Copyright (c) @CompanyNameMagicTag. 2024. All rights reserved.
 */

#include "ui_test_particle.h"
#include "common/image_cache_manager.h"

namespace OHOS {
void UITestParticle::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIViewGroup();
    }
    container_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    container_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    container_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);

    if (particleView_ == nullptr) {
        particleView_ = new UIParticleView();
    }
    particleView_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());

    SetUpEnterParticleCells();

    particleView_->SetBirthRateRatio(1.5f);
    particleView_->SetLifetimeRatio(1.6f);
    particleView_->SetScaleRatio(1.2f);
    particleView_->SetVelocityRatio(0.6f);
    particleView_->SetTouchable(true);
    particleView_->SetDraggable(true);
    particleView_->SetOnClickListener(this);
    particleView_->SetOnDragListener(this);
    particleView_->Start();

    container_->Add(particleView_);
}

void UITestParticle::SetUpEnterParticleCells()
{
    ParticleCell* cell = particleView_->CreateParticleCell();
    if (cell == nullptr) {
        return;
    }
    ImageInfo* heart = ImageCacheManager::GetInstance().LoadSingleRes(RES_PATH"/A094_051_8888.bin");
    cell->SetSrc(heart);
    cell->SetSize(40, 40); // 40: size
    cell->SetSizeRange(10); // 10: size range
    cell->SetPos(Screen::GetInstance().GetWidth() / 2, -50); // 2: half, -50: y
    cell->SetPosRange(Screen::GetInstance().GetWidth() / 2, 0); // 2: half
    cell->SetLifetime(50); // 50: lifetime
    cell->SetVelocity(10.0f);
    cell->SetVelocityRange(1.0f);
    cell->SetAlpha(220, 20); // 220, 20: alpha
    cell->SetAlphaRange(10); // 10: alpha range
    cell->SetScaleRange(0.5f);
    cell->SetAcceleration(0, 0.2f);
    cell->SetEmissionAngle(90.0f);
    cell->SetEmissionRange(85.0f);
    cell->SetTotalPeriod(50); // 50: total period
    cell->SetSpin(10, ParticleCell::SpinMode::DIAGONAL_RL); // 10: spin

    ParticleCell* cell1 = particleView_->CreateParticleCell();
    if (cell1 == nullptr) {
        return;
    }
    ImageInfo* logo = ImageCacheManager::GetInstance().LoadSingleRes(RES_PATH"hexagons.bin");
    cell1->SetSrc(logo);
    cell1->SetSize(20, 20); // 20: size
    cell1->SetSizeRange(10); // 10: size range
    cell1->SetPos(Screen::GetInstance().GetWidth() / 2, -50); // 2: half, -50: y
    cell1->SetPosRange(Screen::GetInstance().GetWidth() / 2, 0); // 2: half
    cell1->SetLifetime(30); // 30: lifetime
    cell1->SetVelocity(5.0f);
    cell1->SetVelocityRange(1.0f);
    cell1->SetAlpha(200, 10); // 200, 10: alpha
    cell1->SetAlphaRange(10); // 10: alpha range
    cell1->SetScale(0.6f, 0.6f);
    cell1->SetScaleRange(0.2f);
    cell1->SetAcceleration(0, 0.3f);
    cell1->SetEmissionAngle(90.0f);
    cell1->SetEmissionRange(85.0f);
    cell1->SetTotalPeriod(60); // 60: total period
    cell1->SetSpin(10, ParticleCell::SpinMode::DIAGONAL_LR); // 10: spin
}

bool UITestParticle::OnDragStart(UIView& view, const DragEvent& event)
{
    dragCell_ = particleView_->CreateParticleCell();
    int16_t x = event.GetCurrentPos().x;
    int16_t y = event.GetCurrentPos().y;
    ImageInfo* heart = ImageCacheManager::GetInstance().LoadSingleRes(RES_PATH"/A094_051_8888.bin");
    dragCell_->SetSrc(heart);
    dragCell_->SetSize(30, 30); // 30: size
    dragCell_->SetSizeRange(5); // 5: size range
    dragCell_->SetPos(x, y);
    dragCell_->SetLifetime(10); // 10: lifetime
    dragCell_->SetVelocity(4.0f);
    dragCell_->SetVelocityRange(2.0f);
    dragCell_->SetAlpha(100, 50); // 100, 50: alpha
    dragCell_->SetAlphaRange(10); // 10: alpha range
    dragCell_->SetEmissionRange(360.0f);
    dragCell_->SetTotalPeriod(ParticleCell::PERIOD_FOREVER);
    dragCell_->SetSpin(10, ParticleCell::SpinMode::CENTER); // 10: spin
    return true;
}

bool UITestParticle::OnDrag(UIView& view, const DragEvent& event)
{
    if (dragCell_ == nullptr) {
        return false;
    }
    int16_t x = event.GetCurrentPos().x;
    int16_t y = event.GetCurrentPos().y;
    dragCell_->SetPos(x, y);
    return true;
}

bool UITestParticle::OnDragEnd(UIView& view, const DragEvent& event)
{
    if (dragCell_ == nullptr) {
        return false;
    }
    dragCell_->SetTotalPeriod(0);
    dragCell_ = nullptr;
    return true;
}

bool UITestParticle::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == particleView_) {
        int16_t x = event.GetCurrentPos().x;
        int16_t y = event.GetCurrentPos().y;

        ParticleCell* cell = particleView_->CreateParticleCell();
        if (cell == nullptr) {
            return false;
        }
        ImageInfo* heart = ImageCacheManager::GetInstance().LoadSingleRes(RES_PATH"/A094_051_8888.bin");
        cell->SetSrc(heart);
        cell->SetSize(40, 40); // 40: size
        cell->SetSizeRange(10); // 10: size range
        cell->SetPos(x, y);
        cell->SetPosRange(5, 5); // 5: pos range
        cell->SetLifetime(15); // 15: lifetime
        cell->SetBirthRate(2); // 2: birthrate
        cell->SetVelocity(8.0f);
        cell->SetVelocityRange(4.0f);
        cell->SetAlpha(150, 20); // 150, 20: alpha
        cell->SetAlphaRange(30); // 30: alpha range
        cell->SetEmissionRange(360.0f);
        cell->SetTotalPeriod(5); // 5: total period
        cell->SetSpin(10, ParticleCell::SpinMode::CENTER); // 10: spin
        return true;
    }

    return false;
}

void UITestParticle::TearDown()
{
    particleView_->ClearParticles();
    DeleteChildren(container_);
    container_ = nullptr;
    particleView_ = nullptr;
    dragCell_ = nullptr;
}

const UIView* UITestParticle::GetTestView()
{
    return container_;
}
}
