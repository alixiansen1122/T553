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

#ifndef PARTICLE_CELL_H
#define PARTICLE_CELL_H

#include "animator/easing_equation.h"
#include "components/particle.h"
#include "gfx_utils/vector.h"
#include "components/ui_view_group.h"

namespace OHOS {
class ParticleCell : public HeapBase {
public:
    ParticleCell() {}
    virtual ~ParticleCell();

    constexpr static int16_t PERIOD_FOREVER = -1;

    /**
     * @brief   Set image for particles.
     */
    void SetSrc(const ImageInfo* src);

    /**
     * @brief   Set particles' initial size.
     */
    void SetSize(int16_t width, int16_t height);

    /**
     * @brief   Set particles' size range, default value is 0.
     */
    void SetSizeRange(int16_t range);

    /**
     * @brief   Set particles' initial position.
     */
    void SetPos(int16_t x, int16_t y);

    /**
     * @brief   Set particles' initial position, default value is 0.
     */
    void SetPosRange(int16_t xRange, int16_t yRange);

    /**
     * @brief   Set particles' lifetime.
     */
    void SetLifetime(int16_t lifetime);

    /**
     * @brief   Set particles' lifetime range, default value is 0.
     */
    void SetLifetimeRange(int16_t range);

    /**
     * @brief   Set particles' birth rate, that is how many particles will be produces per period.
     */
    void SetBirthRate(int16_t birthRate);

    /**
     * @brief   Set particles' velocity in emitting direction.
     */
    void SetVelocity(float velocity);

    /**
     * @brief   Set particles' velocity range, default value is 0.
     */
    void SetVelocityRange(float range);

    /**
     * @brief   Set particles' acceleration.
     */
    void SetAcceleration(float x, float y);

    /**
     * @brief   Set particles' emission angle, valid range should be [0, 360].
     *          3 o'clock direction is defined as 0, and moves in clockwise direction.
     */
    void SetEmissionAngle(float angle);

    /**
     * @brief   Set particles' emission angle range.
     */
    void SetEmissionRange(float range);

    /**
     * @brief   Set particles' scale.
     */
    void SetScale(float scaleX, float scaleY);

    /**
     * @brief   Set particles' scale range.
     */
    void SetScaleRange(float range);

    /**
     * @brief   Set scale easing function which specifies the rate of change of scale over time.
     */
    void SetScaleFunc(EasingFunc func);

    /**
     * @brief   Set particles' initial alpha and ending alpha.
     */
    void SetAlpha(int16_t startAlpha, int16_t finalAlpha);

    /**
     * @brief   Set particles' alpha range.
     */
    void SetAlphaRange(int16_t range);

    /**
     * @brief   Set alpha easing function which specifies the rate of change of alpha over time.
     */
    void SetAlphaFunc(EasingFunc func);

    /**
     * @brief   Defines particles' spinning mode.
     */
    typedef enum {
        CENTER,    // spin around particle center
        DIAGONAL_LR,    // spin around the diagonal from TopLeft to BottomRight
        DIAGONAL_RL,    // spin around the diagonal from TopRight to BottomLeft
    } SpinMode;

    /**
     * @brief   Set particles' spinning angle and spinning mode.
     */
    void SetSpin(float spin, ParticleCell::SpinMode mode = ParticleCell::SpinMode::CENTER);

    /**
     * @brief   Set particles' spinning angle range.
     */
    void SetSpinRange(float range);

    /**
     * @brief   Return whether this particle cell has particles.
     */
    bool HasParticles();

    /**
     * @brief   Set particle cell's total period to produce particles. PERIOD_FOREVER means forever.
     */
    void SetTotalPeriod(int16_t period);

    /**
     * @brief   Set particle cell's total period.
     */
    int16_t GetTotalPeriod()
    {
        return totalPeriod_;
    }

    /**
     * @brief   Get particle cell's current period.
     */
    int16_t GetCurrentPeriod()
    {
        return curPeriod_;
    }

    /**
     * @brief   Get image for particles.
     */
    const ImageInfo* GetSrc()
    {
        return &imageInfo_;
    }

    /**
     * @brief   Clear particles.
     */
    void ClearParticles();

private:
    friend class UIParticleView;
    bool ProduceParticles(UIViewGroup* parent);
    void UpdateParticles();
    float AdjustAngle(float angle);

    ImageInfo imageInfo_ = {0};
    int16_t width_ = 1;
    int16_t height_ = 1;
    int16_t sizeRange_ = 0;

    int16_t x_ = 0;
    int16_t y_ = 0;
    int16_t xRange_ = 0;
    int16_t yRange_ = 0;

    int16_t birthRate_ = 1;
    int16_t lifetime_ = 1;
    int16_t lifeRange_ = 0;

    float scaleX_ = 1.0f;
    float scaleY_ = 1.0f;
    float scaleRange_ = 0.0f;
    EasingFunc scaleFunc_ = EasingEquation::LinearEaseNone;

    int16_t startAlpha_ = OPA_OPAQUE;
    int16_t finalAlpha_ = 0;
    int16_t alphaRange_ = 0;
    EasingFunc alphaFunc_ = EasingEquation::LinearEaseNone;

    float velocity_ = 0.0f;
    float velocityRange_ = 0.0f;
    float accelerationX_ = 0.0f;
    float accelerationY_ = 0.0f;

    float emissionAngle_ = 0.0f;
    float emissionRange_ = 0.0f;

    int16_t totalPeriod_ = 0;
    int16_t curPeriod_ = 0;

    float spin_ = 0.0f;
    float spinRange_ = 0.0f;
    SpinMode spinMode_ = SpinMode::CENTER;

    bool ProduceSingleParticle(UIViewGroup* parent);
    Graphic::Vector<Particle*> particles_;
};
}
#endif
