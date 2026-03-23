/*
 * Copyright (c) CompanyNameMagicTag 2024. All rights reserved.
 * Description: BackToHexagonsTransition
 * Author:
 * Create: 2024-04
 */

#include "BackToHexagonsTransition.h"
#include "TransitionManager.h"
#include "NativeAbility.h"
#include "components/root_view.h"
#include "applist/HexagonsDesktopFragment.h"
#include "hal_tick.h"
#include "wearable_log.h"
#ifdef JS_ENABLE
#include "abilityms_slite_client.h"
#endif
#include "TransitionUtils.h"

namespace OHOS {
REGIST_TRANSITION(TransitionType::TRANSITION_BACK_TO_HEXAGONS, BackToHexagonsTransition, 300, true, false);
static const float CURRENT_SCALE_START = 1.0f;
static const float TARGET_SCALE_END = 1.0f;

void BackToHexagonsTransition::OnTransitionStart(UIImageView* current, UIViewGroup* target)
{
    current_ = current;
    target_ = dynamic_cast<UICustomHexagonsList*>(TransitionUtils::GetViewAndTransparentizePath(target, UI_HEXAGONS_LIST));
    if ((target_ == nullptr) || (current_ == nullptr)) {
        return;
    }

#ifdef JS_ENABLE
    ElementName *elementName = OHOS::AbilityMsClient::GetInstance().GetTopAbility();
    bool backFromNative = (elementName != nullptr) && (strcmp(elementName->bundleName, "com.huawei.launcher") == 0);
#endif
    uint16_t sliceId = NativeAbility::GetInstance().GetPreSliceId();
    UIView* childView = target_->GetChildrenHead();
    while (childView != nullptr) {
        HexagonsItemView* itemView = dynamic_cast<HexagonsItemView*>(childView);
        if (itemView == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "child is not HexagonsItemView");
            break;
        }
#ifdef JS_ENABLE
        if ((backFromNative && (itemView->GetViewId() == sliceId)) ||
            (!backFromNative && (strcmp(itemView->GetAppUid(), elementName->bundleName) == 0))) {
            focusIndex_ = itemView->GetViewIndex();
            focusView_ = itemView;
            break;
        }
#endif
        childView = childView->GetNextSibling();
    }
#ifdef JS_ENABLE
    FreeElement(elementName);
#endif
    if (focusView_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "BackToHexagonsTransition no focus view, use default\n");
        focusView_ = target_->GetViewByIndex(0);
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "BackToHexagonsTransition focus %d", focusIndex_);

    target_->Scale(1.0f, focusIndex_);
    Vector2<int16_t> pos = target_->GetCurrentPosition(focusIndex_);
    int16_t deltaX = MATH_ROUND(target_->GetWidth() / 2.0f - pos.x_);
    int16_t deltaY = MATH_ROUND(target_->GetHeight() / 2.0f - pos.y_);
    target_->ScrollBy(deltaX, deltaY);
    target_->ReMeasure();
    Rect rect = focusView_->GetRect();
    float xScale = current->GetWidth() * 1.0f / rect.GetWidth();
    float yScale = current->GetHeight() * 1.0f / rect.GetHeight();
    tarScaleStart_ = FloatLessEqual(xScale, yScale) ? xScale : yScale;
    curScaleEnd_ = 1.0f / tarScaleStart_;
}

void BackToHexagonsTransition::TransitionAlg(uint32_t time)
{
    if ((current_ == nullptr) || (target_ == nullptr) || (focusView_ == nullptr)) {
        return;
    }
    float timeRatio = time * 1.0f / duration_;

    float curScale = timeRatio * (curScaleEnd_ - CURRENT_SCALE_START) + CURRENT_SCALE_START;
    current_->Scale(Vector2<float>(curScale, curScale), Vector2<float>(current_->GetWidth() / 2.0f,
        current_->GetHeight() / 2.0f));
    uint8_t opa = OPA_OPAQUE + timeRatio * (OPA_TRANSPARENT - OPA_OPAQUE);
    current_->SetOpaScale(opa);

    float targetScale = tarScaleStart_ + timeRatio * (TARGET_SCALE_END - tarScaleStart_);
    target_->Scale(targetScale, focusIndex_);
    focusView_->SetOpaScale(OPA_OPAQUE - opa);
}

void BackToHexagonsTransition::OnTransitionEnd()
{
    if ((target_ != nullptr) && (focusView_ != nullptr)) {
        target_->Scale(TARGET_SCALE_END, focusIndex_);
        focusView_->SetOpaScale(OPA_OPAQUE);
    }
    focusView_ = nullptr;
    focusIndex_ = 0;
}
} // namespace OHOS