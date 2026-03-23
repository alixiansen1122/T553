/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BreathExerPage
 * Create: 2025-5-13
 */
#include <sys/time.h>
#include <chrono>
#include <iostream>
#include <string>
#include <sys/time.h>
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "PageTransitionMgr.h"
#include "SlicePageFactory.h"
#include "UiConfig.h"
#include "common/image_cache_manager.h"
#include "components/root_view.h"
#include "components/ui_image_animator.h"
#include "gfx_utils/graphic_math.h"
#include "main/LoadImg.h"
#include "ohos_timer.h"
#include "ui_resource_breath.h"
#include "wearable_log.h"
#include "settings/model/SettingDisplayModel.h"
#include "breathexercise/BreathView.h"
#include "breathexercise/BreathExerPage.h"

namespace OHOS {
static constexpr int16_t BREATH_GROUP_SIZE = 454;
static constexpr int16_t BREATH_ZERO = 0;
static constexpr int16_t BREATH_ONE = 1;
static constexpr int16_t BREATH_TWO = 2;
static constexpr int16_t BREATH_THREE = 3;
static constexpr uint16_t IMAGEANIMATOERVIEW_SIZE = 80;
static constexpr uint16_t END_TIME = 20;
static constexpr int16_t BREATH_INHALE_POSITION_X = 180;
static constexpr int16_t BREATH_INHALE_POSITION_Y = 380;
static constexpr int16_t BREATH_INHALE_SIZE_WIDTH = 100;
static constexpr int16_t BREATH_INHALE_SIZE_HEIGHT = 50;
static constexpr int16_t BREATH_FRONT = 40;
static constexpr int16_t BREATH_HALF = 2;
static constexpr int16_t ONE_MINUTE = 60;
static constexpr float BREATH_DEFAULT_SCALE = 0.5f;
static constexpr float BREATH_DEFAULT_ANGLE = 30.0f;
static constexpr float BREATH_MID_ANGLE = 60.0f;
static constexpr float BREATH_MAX_ANGLE = 90.0f;
static constexpr float BREATH_MAX_FACTOR = 1.0f;
static constexpr float BREATH_MIN_FACTOR = 0.5f;
const Vector2<float> VIEW_CENTER = {227, 227};
enum class State { FadingOutFirst, FadingInSecond, FadingOutSecond, FadingInFirst } currentState;

REGIST_SLICE_PAGE(VIEW_BREATH, BREATH_PAGE::BREATH_EXER_VIEW, BreathExerPage, false);

BreathExerPage::BreathExerPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BreathExerPage::BreathExerPage");
}

BreathExerPage::~BreathExerPage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }

    if (mainImg_ != nullptr) {
        delete mainImg_;
        mainImg_ = nullptr;
    }

    if (romataImg_ != nullptr) {
        delete romataImg_;
        romataImg_ = nullptr;
    }

    if (romataImgVirtual_ != nullptr) {
        delete romataImgVirtual_;
        romataImgVirtual_ = nullptr;
    }

    if (breathInhale_ != nullptr) {
        delete breathInhale_;
        breathInhale_ = nullptr;
    }

    if (breathExhale_ != nullptr) {
        delete breathExhale_;
        breathExhale_ = nullptr;
    }

    if (animatorCallBack_ != nullptr) {
        delete animatorCallBack_;
        animatorCallBack_ = nullptr;
    }

    if (animator_ != nullptr) {
        delete animator_;
        animator_ = nullptr;
    }
    
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(BREATH_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BreathExerPage::~BreathExerPage");
}

void BreathExerPage::OnStart(void *data)
{
    group_ = new UIViewGroup();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new group_ ERROR");
        return;
    }
    group_->SetPosition(0, 0);
    group_->Resize(BREATH_GROUP_SIZE, BREATH_GROUP_SIZE);
    group_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    group_->SetTouchable(true);
    group_->SetDraggable(true);
    group_->SetOnDragListener(this);

    mainImg_ = new UIImageView();
    if (mainImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "mainImg_ new image fail");
        return;
    }
    mainImg_->SetPosition(0, 0);
    mainImg_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    LOADIMG::LoadImageViewImg(mainImg_, BREATH_IMAGE, IMAGE_BREATH_BG1);
    group_->Add(mainImg_);

    romataImg_ = new UIImageView();
    if (romataImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "mainImg_ new romataImg_ fail");
        return;
    }
    romataImg_->SetPosition(0, 0);
    romataImg_->SetStyle(STYLE_BACKGROUND_OPA, 0);
    romataImg_->Rotate(0, VIEW_CENTER);
    LOADIMG::LoadImageViewImg(romataImg_, BREATH_IMAGE, IMAGE_BREATH_ROMATA1);
    group_->Add(romataImg_);

    romataImgVirtual_ = new UIImageView();
    if (romataImgVirtual_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "mainImg_ new romataImgVirtual_ fail");
        return;
    }
    romataImgVirtual_->SetPosition(0, 0);
    romataImgVirtual_->SetStyle(STYLE_BACKGROUND_OPA, 0);
    romataImgVirtual_->Rotate(0, VIEW_CENTER);
    LOADIMG::LoadImageViewImg(romataImgVirtual_, BREATH_IMAGE, IMAGE_BREATH_ROMATA2);
    group_->Add(romataImgVirtual_);

    breathInhale_ = new UILabel();
    breathInhale_->SetPosition(BREATH_INHALE_POSITION_X, BREATH_INHALE_POSITION_Y, BREATH_INHALE_SIZE_WIDTH,
                               BREATH_INHALE_SIZE_HEIGHT);
    breathInhale_->SetText("吸气");
    breathInhale_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BREATH_FRONT);
    breathInhale_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    breathInhale_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    group_->Add(breathInhale_);

    breathExhale_ = new UILabel();
    breathExhale_->SetPosition(BREATH_INHALE_POSITION_X, BREATH_INHALE_POSITION_Y, BREATH_INHALE_SIZE_WIDTH,
                               BREATH_INHALE_SIZE_HEIGHT);
    breathExhale_->SetText("呼气");
    breathExhale_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BREATH_FRONT);
    breathExhale_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    breathExhale_->SetStyle(STYLE_TEXT_OPA, OPA_TRANSPARENT);
    group_->Add(breathExhale_);

    animatorCallBack_ = new BreathExerPageCallback(this);
    if (animatorCallBack_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new animatorCallBack_ ERROR");
        return;
    }
    animator_ = new Animator(animatorCallBack_, nullptr, 0, true);
    if (animator_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new animator_ ERROR");
        return;
    }

    currentState = State::FadingOutFirst;
    g_StartTime = BreathModel::GetInstance().GetBeginTime();
    rotateStep_ = BreathModel::GetInstance().GetRotateStep();
    flag_ = true;
    scale_ = BREATH_DEFAULT_SCALE;
    angle_ = BREATH_DEFAULT_ANGLE;
    animator_->Start();
    SettingDisplayModel::GetInstance().EnableSteadyOn();
    AddViewToPageContainer(group_);
}

void BreathExerPage::UpdateTextOpa()
{
    static float alpha = 0;
    float totalRotationSteps = (BREATH_MAX_ANGLE - BREATH_DEFAULT_ANGLE) / rotateStep_;
    float deltaAlpha = (float)OPA_OPAQUE / totalRotationSteps * BREATH_HALF;
    switch (currentState) {
        case State::FadingOutFirst:
            if (angle_ < BREATH_MID_ANGLE) {
                alpha += deltaAlpha;
                alpha = (alpha > (float)OPA_OPAQUE) ? (float)OPA_OPAQUE : alpha;
                breathInhale_->SetStyle(STYLE_TEXT_OPA, alpha);
                breathInhale_->SetVisible(true);
                breathExhale_->SetVisible(false);
            } else {
                currentState = State::FadingInSecond;
                alpha = OPA_OPAQUE;
            }
            break;
        case State::FadingInSecond:
            if (angle_ < BREATH_MAX_ANGLE) {
                alpha -= deltaAlpha;
                alpha = (alpha < 0.0f) ? 0.0f : alpha;
                breathInhale_->SetStyle(STYLE_TEXT_OPA, alpha);
                breathInhale_->SetVisible(true);
                breathExhale_->SetVisible(false);
            } else {
                currentState = State::FadingOutSecond;
                alpha = 0;
            }
            break;
        case State::FadingOutSecond:
            if (angle_ > BREATH_MID_ANGLE) {
                alpha += deltaAlpha;
                alpha = (alpha > (float)OPA_OPAQUE) ? ((float)OPA_OPAQUE) : alpha;
                breathExhale_->SetStyle(STYLE_TEXT_OPA, alpha);
                breathInhale_->SetVisible(false);
                breathExhale_->SetVisible(true);
            } else {
                currentState = State::FadingInFirst;
                alpha = OPA_OPAQUE;
            }
            break;
        case State::FadingInFirst:
            if (angle_ > BREATH_DEFAULT_ANGLE) {
                alpha -= deltaAlpha;
                alpha = (alpha < 0.0f) ? 0.0f : alpha;
                breathExhale_->SetStyle(STYLE_TEXT_OPA, alpha);
                breathInhale_->SetVisible(false);
                breathExhale_->SetVisible(true);
            } else {
                currentState = State::FadingOutFirst;
                alpha = 0;
            }
            break;
    }
}

void BreathExerPage::UpdateImageRotate()
{
    if (flag_) {
        angle_ += rotateStep_;
        scale_ += (BREATH_MAX_FACTOR - BREATH_MIN_FACTOR) / ((BREATH_MAX_ANGLE - BREATH_DEFAULT_ANGLE) / rotateStep_);
    } else {
        angle_ -= rotateStep_;
        scale_ -= (BREATH_MAX_FACTOR - BREATH_MIN_FACTOR) / ((BREATH_MAX_ANGLE - BREATH_DEFAULT_ANGLE) / rotateStep_);
    }
    scale_ = (scale_ > BREATH_MAX_FACTOR) ? BREATH_MAX_FACTOR : scale_;
    scale_ = (scale_ < BREATH_MIN_FACTOR) ? BREATH_MIN_FACTOR : scale_;

    if (angle_ >= BREATH_MAX_ANGLE) {
        angle_ = BREATH_MAX_ANGLE;
        flag_ = false;
    } else if (angle_ <= BREATH_DEFAULT_ANGLE) {
        angle_ = BREATH_DEFAULT_ANGLE;
        flag_ = true;
    }

    romataImgVirtual_->Scale(Vector2<float>{scale_, scale_}, VIEW_CENTER);
    romataImgVirtual_->Rotate(angle_, VIEW_CENTER);
    romataImgVirtual_->Invalidate();
    romataImg_->Scale(Vector2<float>{scale_, scale_}, VIEW_CENTER);
    romataImg_->Rotate(BREATH_MID_ANGLE - angle_, VIEW_CENTER);
    romataImg_->Invalidate();
}

void BreathExerPage::OnPause()
{
    animator_->Stop();
    SettingDisplayModel::GetInstance().DisableSteadyOn();
}

bool BreathExerPage::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        animator_->Stop();
        NativeAbility::GetInstance().SwitchPageInSlice(BREATH_PAGE::BREATH_MAIN_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

void BreathExerPage::BreathExerPageCallback::Callback(UIView *view)
{
    UNUSED(view);
    struct timeval currentTime;
    gettimeofday(&currentTime, nullptr);
    uint32_t breathingTime = (BreathModel::GetInstance().GetTrainTime() + 1) * ONE_MINUTE;
    if (breathingTime <= currentTime.tv_sec - g_StartTime) {
        BreathExerPage->animator_->Stop();
        NativeAbility::GetInstance().SwitchPageInSlice(BREATH_PAGE::BREATH_RESULT_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    } else {
        BreathExerPage->UpdateImageRotate();
        BreathExerPage->UpdateTextOpa();
    }
}

}