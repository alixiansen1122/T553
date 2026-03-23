/*
 * Copyright (c) @CompanyNameMagicTag. 2024. All rights reserved.
 */

#ifndef UI_HEXAGONS_LIST_EXT_H
#define UI_HEXAGONS_LIST_EXT_H
#include "components/ui_hexagons_list.h"

namespace OHOS {
class UICustomHexagonsList : public UIHexagonsList {
public:
    UICustomHexagonsList() {}
    ~UICustomHexagonsList() override;
    UICustomHexagonsList(const UICustomHexagonsList &) = delete;
    UICustomHexagonsList &operator=(const UICustomHexagonsList &) = delete;

#if ENABLE_ROTATE_INPUT
    bool OnRotateStartEvent(const RotateEvent& event) override;

    bool OnRotateEvent(const RotateEvent& event) override;

    bool OnRotateEndEvent(const RotateEvent& event) override;
#endif
    void StartRotateAnimator(uint8_t center, int16_t xDistance, int16_t yDistance, float startScale, float endScale);
    void StopRotateAnimator();
    float GetScaleInSmallState()
    {
        return scaleInSmallState_;
    }

protected:
    class RotateAnimatorCallback : public ListAnimatorCallback {
    public:
        RotateAnimatorCallback();
        ~RotateAnimatorCallback() override {}
        RotateAnimatorCallback(const RotateAnimatorCallback &) = delete;
        RotateAnimatorCallback &operator=(const RotateAnimatorCallback &) = delete;

        void SetDuration(uint16_t times)
        {
            totalTime_ = times;
        }

        void SetCenterIndex(uint8_t center)
        {
            centerIndex_ = center;
        }

        void SetRotateRange(int16_t xDistance, int16_t yDistance, float startScale, float endScale)
        {
            xDistance_ = xDistance;
            yDistance_ = yDistance;
            startScale_ = startScale;
            endScale_ = endScale;
        }

        void ResetCallback()
        {
            curTime_ = 0;
            totalTime_ = 0;
            xDistance_ = 0;
            prevX_ = 0;
            yDistance_ = 0;
            prevY_ = 0;
            startScale_ = 0.0f;
            endScale_ = 0.0f;
            centerIndex_ = 0;
        }

        void Callback(UIView* view) override;

    private:
        uint16_t curTime_;
        uint16_t totalTime_;
        int16_t xDistance_;
        int16_t yDistance_;
        int16_t prevX_;
        int16_t prevY_;
        float startScale_;
        float endScale_;
        uint8_t centerIndex_;
    };
private:
    RotateAnimatorCallback rotateCallback_;
    Animator* rotateAnimator_ = nullptr;
    float scaleInSmallState_ = 0.42f;
    uint8_t rotateCenterIndexInNormal_ = 0;
};
}
#endif // UI_HEXAGONS_LIST_EXT_H