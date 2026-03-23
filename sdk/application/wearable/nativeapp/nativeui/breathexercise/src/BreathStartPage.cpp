/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BreathStartPage
 * Create: 2025-5-13
 */
#include <sys/time.h>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/time.h>
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "PageTransitionMgr.h"
#include "SlicePageFactory.h"
#include "TransitionType.h"
#include "UiConfig.h"
#include "components/root_view.h"
#include "gfx_utils/graphic_math.h"
#include "main/LoadImg.h"
#include "ohos_timer.h"
#include "ui_resource_breath.h"
#include "wearable_log.h"
#ifdef BUILD_IN_LINUX
#include "localtime.h"
#endif
#include "breathexercise/BreathModel.h"
#include "breathexercise/BreathView.h"
#include "breathexercise/BreathStartPage.h"

namespace OHOS {
static constexpr uint16_t BREATH_START_TITLE_X = 97;
static constexpr uint16_t BREATH_START_TITLE_Y = 40;
static constexpr uint16_t BREATH_START_TITLE_W = 260;
static constexpr uint16_t BREATH_START_TITLE_H = 53;
static constexpr uint16_t BREATH_START_TITLE_FONT_SIZE = 36;
static constexpr uint16_t BREATH_START_CENTERIMG_X = 88;
static constexpr uint16_t BREATH_START_CENTERIMG_Y = 88;
static constexpr uint16_t BREATH_START_CENTERIMG_W = 279;
static constexpr uint16_t BREATH_START_CENTERIMG_H = 280;
static constexpr uint16_t BREATH_START_GUIDE_X = 126;
static constexpr uint16_t BREATH_START_GUIDE_Y = 354;
static constexpr uint16_t BREATH_START_GUIDE_W = 202;
static constexpr uint16_t BREATH_START_GUIDE_H = 68;
static constexpr uint16_t BREATH_START_GUIDE_FONT_SIZE = 24;
static constexpr uint16_t BREATH_GROUP_SIZE = 466;
static constexpr uint16_t TEXT_OPACITY = 147;
static constexpr uint16_t COUNTDOWN = 5;
static constexpr uint16_t SIZE = 64;

REGIST_SLICE_PAGE(VIEW_BREATH, BREATH_PAGE::BREATH_START_VIEW, BreathStartPage, false);

BreathStartPage::BreathStartPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BreathStartPage::BreathStartPage");
}

BreathStartPage::~BreathStartPage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }

    if (titleLable_ != nullptr) {
        delete titleLable_;
        titleLable_ = nullptr;
    }

    if (guideLable_ != nullptr) {
        delete guideLable_;
        guideLable_ = nullptr;
    }

    if (centerImg_ != nullptr) {
        delete centerImg_;
        centerImg_ = nullptr;
    }

    if (bgImg_ != nullptr) {
        delete bgImg_;
        bgImg_ = nullptr;
    }

    if (animator_ != nullptr) {
        delete animator_;
        animator_ = nullptr;
    }

    if (animatorCallBack_ != nullptr) {
        delete animatorCallBack_;
        animatorCallBack_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(BREATH_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BreathStartPage::~BreathStartPage");
}

void BreathStartPage::OnStart(void *data)
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

    bgImg_ = new UIImageView();
    if (bgImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new bgImg_ ERROR");
        return;
    }
    bgImg_->SetPosition(0, 0);
    bgImg_->Resize(BREATH_GROUP_SIZE, BREATH_GROUP_SIZE);
    LOADIMG::LoadImageViewImg(bgImg_, BREATH_IMAGE, IMAGE_BREATH_BG3);
    group_->Add(bgImg_);

    titleLable_ = new UILabel();
    if (titleLable_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new titleLable_ ERROR");
        return;
    }
    titleLable_->SetPosition(BREATH_START_TITLE_X, BREATH_START_TITLE_Y);
    titleLable_->Resize(BREATH_START_TITLE_W, BREATH_START_TITLE_H);
    titleLable_->SetText("请保持静止");
    titleLable_->SetTextColor(Color::White());
    titleLable_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    titleLable_->SetFont(BOLD_VECTOR_FONT_FILENAME, BREATH_START_TITLE_FONT_SIZE);
    group_->Add(titleLable_);

    centerImg_ = new UIImageView();
    if (centerImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new centerImg ERROR");
        return;
    }
    centerImg_->SetPosition(BREATH_START_CENTERIMG_X, BREATH_START_CENTERIMG_Y);
    centerImg_->Resize(BREATH_START_CENTERIMG_W, BREATH_START_CENTERIMG_H);
    LOADIMG::LoadImageViewImg(centerImg_, BREATH_IMAGE, IMAGE_BREATH_CENTER1);
    group_->Add(centerImg_);

    guideLable_ = new UILabel();
    if (guideLable_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new guideLable_ ERROR");
        return;
    }
    guideLable_->SetPosition(BREATH_START_GUIDE_X, BREATH_START_GUIDE_Y);
    guideLable_->Resize(BREATH_START_GUIDE_W, BREATH_START_GUIDE_H);
    guideLable_->SetTextColor(Color::White());
    guideLable_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    guideLable_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BREATH_START_GUIDE_FONT_SIZE);
    guideLable_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    guideLable_->SetStyle(STYLE_TEXT_OPA, TEXT_OPACITY);
    group_->Add(guideLable_);

    animatorCallBack_ = new BreathStartCallback(this);
    if (animatorCallBack_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new animatorCallBack_ ERROR");
        return;
    }
    animator_ = new Animator(animatorCallBack_, nullptr, 0, true);
    if (animator_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new animator_ ERROR");
        return;
    }
    animator_->Start();
    AddViewToPageContainer(group_);
}

void BreathStartPage::BreathStartCallback::Callback(UIView *view)
{
    UNUSED(view);
#ifdef _WIN32
    struct timeval tv;
    gettimeofday(&tv, NULL);
#else
    struct timeval64 tv;
    gettimeofday64(&tv, NULL);
#endif
    int32_t start_time = BreathModel::GetInstance().GetStartTime();
    int32_t remTime = COUNTDOWN - (tv.tv_sec - start_time);
    if (remTime <= 0) {
        BreathStartPage->animator_->Stop();
#ifdef _WIN32
        struct timeval time;
        gettimeofday(&time, NULL);
#else
        struct timeval64 time;
        gettimeofday64(&time, NULL);
#endif
        uint32_t breathing_time = time.tv_sec;
        BreathModel::GetInstance().SetBeginTime(breathing_time);
        NativeAbility::GetInstance().SwitchPageInSlice(BREATH_PAGE::BREATH_EXER_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    } else {
        char szText[SIZE] = "";
        sprintf_s(szText, sizeof(szText), "%d秒后跟随训练指引开始吸气和呼气", remTime);
        BreathStartPage->guideLable_->SetText(szText);
    }
}

void BreathStartPage::OnPause()
{
    animator_->Stop();
}

bool BreathStartPage::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().SwitchPageInSlice(BREATH_PAGE::BREATH_MAIN_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

void BreathStartPage::OnTimer() {}

}