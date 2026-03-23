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

#ifndef PARTICLE_H
#define PARTICLE_H

#include "components/ui_image_view.h"

namespace OHOS {
class Particle : public UIImageView {
public:
    Particle() {}
    virtual ~Particle() override {}

    void SetLife(int16_t life)
    {
        curLife_ = life;
    }

    void SetMaxLife(int16_t maxLife)
    {
        maxLife_ = maxLife;
    }

    void SetFinalScale(float scaleX, float scaleY)
    {
        scaleX_ = scaleX;
        scaleY_ = scaleY;
    }

    void SetAlpha(int16_t startAlpha, int16_t finalAlpha)
    {
        startAlpha_ = startAlpha;
        finalAlpha_ = finalAlpha;
    }

    void SetVelocity(float velocityX, float velocityY)
    {
        velocityX_ = velocityX;
        velocityY_ = velocityY;
    }

    int16_t GetLife()
    {
        return curLife_;
    }

    int16_t GetMaxLife()
    {
        return maxLife_;
    }

    int16_t GetStartAlpha()
    {
        return startAlpha_;
    }

    int16_t GetFinalAlpha()
    {
        return finalAlpha_;
    }

    float GetScaleX()
    {
        return scaleX_;
    }

    float GetScaleY()
    {
        return scaleY_;
    }

    float GetVelocityX()
    {
        return velocityX_;
    }

    float GetVelocityY()
    {
        return velocityY_;
    }

    float GetSpin()
    {
        return spin_;
    }

    void SetSpin(float spin)
    {
        spin_ = spin;
    }

private:
    int16_t curLife_ = 0;
    int16_t maxLife_ = 0;
    float scaleX_ = 1.0f;
    float scaleY_ = 1.0f;
    int16_t startAlpha_ = 0;
    int16_t finalAlpha_ = 0;
    float velocityX_ = 0.0f;
    float velocityY_ = 0.0f;
    float spin_ = 0.0f;
};
}
#endif
