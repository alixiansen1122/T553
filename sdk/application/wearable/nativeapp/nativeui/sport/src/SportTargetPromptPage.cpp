/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportTargetPromptPage
 * Create: 2025-06-06
 */

#include <sys/time.h>
#include "wearable_log.h"
#include "UiConfig.h"
#include "main/LoadImg.h"
#include "ui_resource_sport.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "sport/SportView.h"
#include "sport/SportTargetPromptPage.h"

namespace OHOS {
static constexpr uint16_t PAGE_VISIBLE_SECOND = 3;
static constexpr uint16_t PROMPT_DISTANCE_INDEX = 0;
static constexpr uint16_t PROMPT_TIME_INDEX = 1;
static constexpr uint16_t PROMPT_LAP_INDEX = 2;
static constexpr uint16_t IMAGE_X = 46;
static constexpr uint16_t IMAGE_Y = 72;
static constexpr uint16_t IMAGE_COMPLETE_X = 85;
static constexpr uint16_t IMAGE_COMPLETE_Y = 32;
static constexpr uint16_t IMAGE_HEART_X = 167;
static constexpr uint16_t IMAGE_HEART_Y = 118;
static constexpr uint16_t LABEL_X = 25;
static constexpr uint16_t LABEL_Y = 250;
static constexpr uint16_t LABEL_W = 404;
static constexpr uint16_t LABEL_H = 49;
static constexpr uint16_t LABEL_FONT = 36;
static const char *PROMPT_HALF_CONTENT = "本次运动目标已完成50%";
static const char *PROMPT_FINISH_CONTENT = "本次运动目标已完成";
static const int PROPMT_IMAGES[][10] = {
    {IMAGE_SPORT_1KM, IMAGE_SPORT_3KM, IMAGE_SPORT_5KM, IMAGE_SPORT_7KM, IMAGE_SPORT_10KM, IMAGE_SPORT_15KM,
     IMAGE_SPORT_20KM, IMAGE_SPORT_25KM, IMAGE_SPORT_30KM, IMAGE_SPORT_40KM},
    {IMAGE_SPORT_10MIN, IMAGE_SPORT_20MIN, IMAGE_SPORT_30MIN, IMAGE_SPORT_60MIN, IMAGE_SPORT_120MIN, IMAGE_SPORT_180MIN,
     IMAGE_SPORT_200MIN},
    {IMAGE_SPORT_LAP1, IMAGE_SPORT_LAP2, IMAGE_SPORT_LAP4, IMAGE_SPORT_LAP6, IMAGE_SPORT_LAP8, IMAGE_SPORT_LAP10,
     IMAGE_SPORT_LAP12, IMAGE_SPORT_LAP16, IMAGE_SPORT_LAP20, IMAGE_SPORT_LAP30},
};

REGIST_SLICE_PAGE(VIEW_SPORT, SportPages::SPORT_PAGE_TARGET_PROMPT, SportTargetPromptPage, false);

void SportTargetPromptPage::SportPromptAnimatorCallback::Callback(UIView *view)
{
    UNUSED(view);

    time_t now = time(nullptr);
    if (now - page_->startTime_ > PAGE_VISIBLE_SECOND) {
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_DATA,
            TransitionType::TRANSITION_INVALID, false);
    }
}

SportTargetPromptPage::~SportTargetPromptPage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }
    if (promptIcon_ != nullptr) {
        delete promptIcon_;
        promptIcon_ = nullptr;
    }
    if (promptLabel_ != nullptr) {
        delete promptLabel_;
        promptLabel_ = nullptr;
    }
    if (animator_ != nullptr) {
        delete animator_;
        animator_ = nullptr;
    }
    if (callback_ != nullptr) {
        delete callback_;
        callback_ = nullptr;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SportTargetPromptPage::~SportTargetPromptPage");
}

void SportTargetPromptPage::CreatePromptView()
{
    promptIcon_ = new UIImageView();
    if (promptIcon_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportTargetPromptPage promptIcon_ new fail");
        return;
    }
    promptIcon_->SetPosition(IMAGE_X, IMAGE_Y);
    group_->Add(promptIcon_);

    promptLabel_ = new UILabel();
    if (promptLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportTargetPromptPage promptLabel_ new fail");
        return;
    }
    promptLabel_->SetPosition(LABEL_X, LABEL_Y, LABEL_W, LABEL_H);
    promptLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT);
    promptLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    group_->Add(promptLabel_);

    sport_info_t *sportInfo = SportModel::GetInstance().GetCurrSportInfo();
    if (sportInfo->promptType == SPORT_SET_TYPE_TARGET) {
        if (sportInfo->targetComplete) {
            promptLabel_->SetText(PROMPT_FINISH_CONTENT);
            promptIcon_->SetPosition(IMAGE_COMPLETE_X, IMAGE_COMPLETE_Y);
            LOADIMG::LoadImageViewImg(promptIcon_, SPORT_IMAGE, IMAGE_SPORT_FINISH);
        } else if (sportInfo->targetHalf) {
            promptLabel_->SetText(PROMPT_HALF_CONTENT);
            LOADIMG::LoadImageViewImg(promptIcon_, SPORT_IMAGE, IMAGE_SPORT_HALF);
        }
    } else if (sportInfo->promptType == SPORT_SET_TYPE_REMIND) {
        std::string promptStr;
        if (sportInfo->remindType == SPORT_REMIND_INTERVAL_DISTANCE) {
            SportType sportType = SportModel::GetInstance().GetSportType();
            if (sportType == SPORT_TYPE_INDOOR_SWIMMING) {
                promptStr = "第" + std::to_string(sportInfo->remindTimes) + "个" +
                            std::to_string(sportInfo->remindValue) + "趟已完成！";
                promptLabel_->SetText(promptStr.c_str());
                LOADIMG::LoadImageViewImg(promptIcon_, SPORT_IMAGE,
                                          PROPMT_IMAGES[PROMPT_LAP_INDEX][sportInfo->remindIndex]);
            } else {
                promptStr = "第" + std::to_string(sportInfo->remindTimes) + "个" +
                            std::to_string(sportInfo->remindValue) + "公里已完成！";
                promptLabel_->SetText(promptStr.c_str());
                LOADIMG::LoadImageViewImg(promptIcon_, SPORT_IMAGE,
                                          PROPMT_IMAGES[PROMPT_DISTANCE_INDEX][sportInfo->remindIndex]);
            }
        } else if (sportInfo->remindType == SPORT_REMIND_INTERVAL_TIME) {
            promptStr = "第" + std::to_string(sportInfo->remindTimes) + "个" + std::to_string(sportInfo->remindValue) +
                        "分钟已完成！";
            promptLabel_->SetText(promptStr.c_str());
            LOADIMG::LoadImageViewImg(promptIcon_, SPORT_IMAGE,
                                      PROPMT_IMAGES[PROMPT_TIME_INDEX][sportInfo->remindIndex]);
        }
    } else if (sportInfo->promptType == SPORT_SET_TYPE_HEARTRATE) {
        std::string promptStr = "当前心率" + std::to_string(sportInfo->data.heartRate) + "次/分！";
        promptLabel_->SetText(promptStr.c_str());
        promptIcon_->SetPosition(IMAGE_HEART_X, IMAGE_HEART_Y);
        LOADIMG::LoadImageViewImg(promptIcon_, SPORT_IMAGE, IMAGE_SPORT_HEART3);
    }
}

void SportTargetPromptPage::OnStart(void *data)
{
    startTime_ = time(nullptr);
    group_ = new UIViewGroup();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportTargetPromptPage group_ new fail");
        return;
    }
    group_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    group_->SetOnDragListener(this);
    group_->SetDraggable(true);
    group_->SetTouchable(true);

    CreatePromptView();
    AddViewToPageContainer(group_);

    callback_ = new SportPromptAnimatorCallback(this);
    if (callback_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportTargetPromptPage::OnStart callback_ new fail");
        return;
    }

    animator_ = new Animator(callback_, nullptr, 0, true);
    if (animator_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportTargetPromptPage::OnStart new Animator animator_ fail");
        return;
    }
}

void SportTargetPromptPage::OnPause()
{
    if (animator_ != nullptr) {
        if (animator_->GetState() != Animator::STOP) {
            animator_->Stop();
        }
    }
}

void SportTargetPromptPage::OnResume()
{
    if (animator_ != nullptr) {
        if (animator_->GetState() == Animator::STOP) {
            animator_->Start();
        }
    }
}

bool SportTargetPromptPage::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_DATA,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool SportTargetPromptPage::OnClick(UIView &view, const ClickEvent &event)
{
    return false;
}
}