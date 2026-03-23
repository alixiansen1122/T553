/*
 * Copyright (c) 2024 CompanyNameMagicTag.
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

#ifndef UI_PARTICLE_VIEW_H
#define UI_PARTICLE_VIEW_H

#include "components/particle_cell.h"
#include "animator/animator.h"
#include "gfx_utils/vector.h"

namespace OHOS {

class UIParticleView : public UIViewGroup {
public:
    /**
     * @brief  Constructor of UIParticleView. Particle animation runs reapeatedly.
     */
    UIParticleView();

    /**
     * @brief  Constructor of UIParticleView. Can specify duration of particle animation, and whether to repeat it.
     */
    UIParticleView(uint32_t time, bool repeat);

    /**
     * @brief  Destructor of UIParticleView.
     */
    virtual ~UIParticleView() override;

    /**
     * @brief  Create a particle cell which defines properties of particles and will be managed by particle view.
     */
    ParticleCell* CreateParticleCell();

    /**
     * @brief  Set birth rate ratio, default value is 1. All particle cells' birth rate will be multiplied by this.
     */
    void SetBirthRateRatio(float ratio);

    /**
     * @brief  Set lifetime ratio, default value is 1. All particle cells' lifetime will be multiplied by this.
     */
    void SetLifetimeRatio(float ratio);

    /**
     * @brief  Set scale ratio, default value is 1. All particle cells' scale coefficient will be multiplied by this.
     */
    void SetScaleRatio(float ratio);

    /**
     * @brief  Set velocity ratio, default value is 1. All particle cells' velocity will be multiplied by this.
     */
    void SetVelocityRatio(float ratio);

    /**
     * @brief  Get birth rate ratio.
     */
    float GetBirthRateRatio();

    /**
     * @brief  Get lifetime ratio.
     */
    float GetLifetimeRatio();

    /**
     * @brief  Get scale ratio.
     */
    float GetScaleRatio();

    /**
     * @brief  Get velocity ratio.
     */
    float GetVelocityRatio();

    /**
     * @brief  Set duration of particle animation, in milliseconds.
     */
    void SetTime(uint32_t time);

    /**
     * @brief  Get duration of particle animation, in milliseconds.
     */
    uint32_t GetTime();

    /**
     * @brief  Returns whether to repeat particle animation.
     */
    bool IsRepeat();

    /**
     * @brief  Start particle animation.
     */
    void Start();

    /**
     * @brief  Pause particle animation.
     */
    void Pause();

    /**
     * @brief  Resume particle animation.
     */
    void Resume();

    /**
     * @brief  Stop particle animation. This will invoke ClearParticles.
     */
    void Stop();

    /**
     * @brief  Get particle animation's state.
     */
    uint8_t GetState();

    /**
     * @brief  Clear all particle cells and particles.
     */
    void ClearParticles();

    UIViewType GetViewType() const override
    {
        return UI_PARTICLE_VIEW;
    }

private:
    void ProcessParticleCells();

    class ParticleAnimatorCallback : public AnimatorCallback {
    public:
        ParticleAnimatorCallback() {}
        virtual ~ParticleAnimatorCallback() override {}
        void Callback(UIView* view) override;
        void OnStop(UIView& view) override;
    };

    Graphic::Vector<ParticleCell*> particleCells_;
    Animator animator_;
    ParticleAnimatorCallback animatorCallback_;

    float birthRateRatio_ = 1.0f;
    float lifetimeRatio_ = 1.0f;
    float scaleRatio_ = 1.0f;
    float velocityRatio_ = 1.0f;
};
}
#endif
