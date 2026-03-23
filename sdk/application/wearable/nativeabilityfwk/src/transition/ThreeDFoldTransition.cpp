/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ThreeDFoldTransition
 * Create: 2025-04
 */

#include "ThreeDFoldTransition.h"
#include "animator/easing_equation.h"
#include "TransitionManager.h"
#include "common/screen.h"
#include "wearable_log.h"
namespace OHOS {
static constexpr int16_t DURA_RANGE = 200;
static const float ROTATE_END = 90.0f;
static constexpr int16_t ROTATE_RANGE_MAX = 90;
static constexpr int16_t CAMERA_DISTANCE = 500;

REGIST_TRANSITION(TransitionType::TRANSITION_3DFOLD, ThreeDFoldTransition, DURA_RANGE, true, true);
void ThreeDFoldTransition::OnTransitionStart(UIImageView* current, UIImageView* target)
{
    if ((current == nullptr) || (target == nullptr)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "ThreeDFoldTransition no current or target\n");
        return;
    }
    current_ = current;
    target_ = target;
    current_->SetVisible(true);
    target_->SetVisible(true);

    current_->SetCameraPosition(Vector2<float>(current_->GetWidth(), current_->GetHeight() / 2)); // 2: half
    current_->SetCameraDistance(CAMERA_DISTANCE);
 
    target_->SetCameraPosition(Vector2<float>(0, target_->GetHeight() / 2)); // 2: half
    target_->SetCameraDistance(CAMERA_DISTANCE);
}

void ThreeDFoldTransition::TransitionAlg(uint32_t time)
{
    if ((current_ == nullptr) || (target_ == nullptr)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "ThreeDFoldTransition no current or target\n");
        return;
    }

    int16_t width = Screen::GetInstance().GetWidth();
    float moveOffset = time * 1.0f / (float)DURA_RANGE;
    float angle = ROTATE_RANGE_MAX * moveOffset;

    Vector3<float> currRotateStart = {current_->GetWidth(), current_->GetHeight(), 0};
    Vector3<float> currRotateEnd = {current_->GetWidth(), 0, 0};
    Vector3<float> targRotateStart = {0, 0, 0};
    Vector3<float> targRotateEnd = {0, target_->GetHeight(), 0};

    current_->Rotate(angle, currRotateStart, currRotateEnd);
    current_->SetPosition(-moveOffset * width, current_->GetY());

    target_->Rotate(ROTATE_RANGE_MAX - angle, targRotateStart, targRotateEnd);
    target_->SetPosition((width - moveOffset * width), target_->GetY());
}

void ThreeDFoldTransition::OnTransitionEnd()
{
    if ((current_ == nullptr) || (target_ == nullptr)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "ThreeDFoldTransition no current or target\n");
        return;
    }
    current_->SetVisible(false);
    target_->SetVisible(true);
}
} // namespace OHOS
