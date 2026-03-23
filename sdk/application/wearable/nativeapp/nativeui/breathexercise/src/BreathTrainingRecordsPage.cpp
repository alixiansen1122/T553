/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BreathTrainingRecordsPage
 * Create: 2025-5-13
 */
#include <sstream>
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "TransitionType.h"
#include "UiConfig.h"
#include "components/root_view.h"
#include "gfx_utils/graphic_math.h"
#include "main/LoadImg.h"
#include "ui_resource_breath.h"
#include "wearable_log.h"
#include "breathexercise/BreathView.h"
#include "breathexercise/BreathTrainingRecordsPage.h"

namespace OHOS {
static constexpr uint16_t BREATH_TRAINING_RECORDS_X = 97;
static constexpr uint16_t BREATH_TRAINING_RECORDS_Y = 34;
static constexpr uint16_t BREATH_TRAINING_RECORDS_W = 260;
static constexpr uint16_t BREATH_TRAINING_RECORDS_H = 53;
static constexpr uint16_t BREATH_TRAINING_RECORDS_FONT_SIZE = 38;
static constexpr uint16_t BREATH_TRAINING_TOTALDURATION_X = 236;
static constexpr uint16_t BREATH_TRAINING_TOTALDURATION_Y = 139;
static constexpr uint16_t BREATH_TRAINING_TOTALDURATION_W = 96;
static constexpr uint16_t BREATH_TRAINING_TOTALDURATION_H = 30;
static constexpr uint16_t BREATH_TRAINING_FONT_SIZE = 24;
static constexpr uint16_t BREATH_TRAINING_DURATION_MINUTE_X = 290;
static constexpr uint16_t BREATH_TRAINING_DURATION_MINUTE_Y = 185;
static constexpr uint16_t BREATH_TRAINING_DURATION_MINUTE_W = 48;
static constexpr uint16_t BREATH_TRAINING_DURATION_MINUTE_H = 29;
static constexpr uint16_t BREATH_TRAINING_TOTAL_NUMBLE_OF_TIMES_X = 236;
static constexpr uint16_t BREATH_TRAINING_TOTAL_NUMBLE_OF_TIMES_Y = 268;
static constexpr uint16_t BREATH_TRAINING_TOTAL_NUMBLE_OF_TIMES_W = 96;
static constexpr uint16_t BREATH_TRAINING_TOTAL_NUMBLE_OF_TIMES_H = 30;
static constexpr uint16_t BREATH_TRAINING_NUMBER_OF_TIMES_X = 290;
static constexpr uint16_t BREATH_TRAINING_NUMBER_OF_TIMES_Y = 315;
static constexpr uint16_t BREATH_TRAINING_NUMBER_OF_TIMES_W = 29;
static constexpr uint16_t BREATH_TRAINING_NUMBER_OF_TIMES_H = 29;
static constexpr uint16_t BREATH_TRAINING_TIME_X = 236;
static constexpr uint16_t BREATH_TRAINING_TIME_Y = 171;
static constexpr uint16_t BREATH_TRAINING_TIME_W = 49;
static constexpr uint16_t BREATH_TRAINING_TIME_H = 49;
static constexpr uint16_t BREATH_TRAINING_FONT_MAX_SIZE = 40;
static constexpr uint16_t BREATH_TRAINING_SCOUR_X = 236;
static constexpr uint16_t BREATH_TRAINING_SCOUR_Y = 303;
static constexpr uint16_t BREATH_TRAINING_SCOUR_W = 49;
static constexpr uint16_t BREATH_TRAINING_SCOUR_H = 49;
static constexpr uint16_t BREATH_TRAINING_SIZE = 454;
static constexpr uint16_t BREATH_TRAINING_BACKGROUND2_X = 65;
static constexpr uint16_t BREATH_TRAINING_BACKGROUND2_Y = 174;
static constexpr uint16_t BREATH_TRAINING_BACKGROUND2_W = 119;
static constexpr uint16_t BREATH_TRAINING_BACKGROUND2_H = 120;
static constexpr uint16_t BREATH_TRAINING_BACKGROUND3_X = 215;
static constexpr uint16_t BREATH_TRAINING_BACKGROUND3_Y = 123;
static constexpr uint16_t BREATH_TRAINING_BACKGROUND3_W = 171;
static constexpr uint16_t BREATH_TRAINING_BACKGROUND3_H = 108;
static constexpr uint16_t BREATH_TRAINING_BACKGROUND4_X = 215;
static constexpr uint16_t BREATH_TRAINING_BACKGROUND4_Y = 252;
static constexpr uint16_t TEXT_OPACITY = 147;
static constexpr uint16_t TEXT_OPACITY_BACK = 27;
static constexpr uint16_t Y_OFFSET = 50;

REGIST_SLICE_PAGE(VIEW_BREATH, BREATH_PAGE::BREATH_TRAINING_VIEW, BreathTrainingRecordsPage, false);

BreathTrainingRecordsPage::BreathTrainingRecordsPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BreathTrainingRecordsPage::BreathTrainingRecordsPage");
}

BreathTrainingRecordsPage::~BreathTrainingRecordsPage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }
    if (labelTimeDes_ != nullptr) {
        delete labelTimeDes_;
        labelTimeDes_ = nullptr;
    }
    if (labelTimeUnit_ != nullptr) {
        delete labelTimeUnit_;
        labelTimeUnit_ = nullptr;
    }
    if (labelCountDes_ != nullptr) {
        delete labelCountDes_;
        labelCountDes_ = nullptr;
    }
    if (labelCountUnit_ != nullptr) {
        delete labelCountUnit_;
        labelCountUnit_ = nullptr;
    }
    if (labelTitle_ != nullptr) {
        delete labelTitle_;
        labelTitle_ = nullptr;
    }
    if (labelTimeValue_ != nullptr) {
        delete labelTimeValue_;
        labelTimeValue_ = nullptr;
    }
    if (labelCountValue_ != nullptr) {
        delete labelCountValue_;
        labelCountValue_ = nullptr;
    }
    if (bgImg1_ != nullptr) {
        delete bgImg1_;
        bgImg1_ = nullptr;
    }
    if (bgImg2_ != nullptr) {
        delete bgImg2_;
        bgImg2_ = nullptr;
    }
    if (bgImg3_ != nullptr) {
        delete bgImg3_;
        bgImg3_ = nullptr;
    }
    if (bgImg4_ != nullptr) {
        delete bgImg4_;
        bgImg4_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(BREATH_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BreathTrainingRecordsPage::~BreathTrainingRecordsPage");
}

void BreathTrainingRecordsPage::OnStart(void *data)
{
    group_ = new UIViewGroup();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new group_ ERROR");
        return;
    }
    group_->SetPosition(0, 0);
    group_->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    group_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    group_->SetTouchable(true);
    group_->SetDraggable(true);
    group_->SetOnDragListener(this);

    bgImg1_ = new UIImageView();
    if (bgImg1_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new bgImg1_ ERROR");
        return;
    }
    bgImg1_->SetPosition(0, 0);
    bgImg1_->Resize(BREATH_TRAINING_SIZE, BREATH_TRAINING_SIZE);
    LOADIMG::LoadImageViewImg(bgImg1_, BREATH_IMAGE, IMAGE_BREATH_BG3);
    group_->Add(bgImg1_);

    bgImg2_ = new UIImageView();
    if (bgImg2_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new bgImg2_ ERROR");
        return;
    }
    bgImg2_->SetPosition(BREATH_TRAINING_BACKGROUND2_X, BREATH_TRAINING_BACKGROUND2_Y);
    bgImg2_->Resize(BREATH_TRAINING_BACKGROUND2_W, BREATH_TRAINING_BACKGROUND2_H);
    LOADIMG::LoadImageViewImg(bgImg2_, BREATH_IMAGE, IMAGE_BREATH_RESULT);
    group_->Add(bgImg2_);

    bgImg3_ = new UIImageView();
    if (bgImg3_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new bgImg3_ ERROR");
        return;
    }
    bgImg3_->SetPosition(BREATH_TRAINING_BACKGROUND3_X, BREATH_TRAINING_BACKGROUND3_Y);
    bgImg3_->Resize(BREATH_TRAINING_BACKGROUND3_W, BREATH_TRAINING_BACKGROUND3_H);
    LOADIMG::LoadImageViewImg(bgImg3_, BREATH_IMAGE, IMAGE_BREATH_BG4);
    bgImg3_->SetStyle(STYLE_BACKGROUND_OPA, TEXT_OPACITY_BACK);
    group_->Add(bgImg3_);

    bgImg4_ = new UIImageView();
    if (bgImg4_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new bgImg4_ ERROR");
        return;
    }
    bgImg4_->SetPosition(BREATH_TRAINING_BACKGROUND4_X, BREATH_TRAINING_BACKGROUND4_Y);
    bgImg4_->Resize(BREATH_TRAINING_BACKGROUND3_W, BREATH_TRAINING_BACKGROUND3_H);
    LOADIMG::LoadImageViewImg(bgImg4_, BREATH_IMAGE, IMAGE_BREATH_BG4);
    bgImg4_->SetStyle(STYLE_BACKGROUND_OPA, TEXT_OPACITY_BACK);
    group_->Add(bgImg4_);

    labelTitle_ = new UILabel();
    if (labelTitle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelTitle_ ERROR");
        return;
    }
    labelTitle_->SetPosition(BREATH_TRAINING_RECORDS_X, BREATH_TRAINING_RECORDS_Y);
    labelTitle_->Resize(BREATH_TRAINING_RECORDS_W, BREATH_TRAINING_RECORDS_H);
    labelTitle_->SetText("训练记录");
    labelTitle_->SetTextColor(Color::White());
    labelTitle_->SetFont(BOLD_VECTOR_FONT_FILENAME, BREATH_TRAINING_RECORDS_FONT_SIZE);
    labelTitle_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    group_->Add(labelTitle_);

    labelTimeDes_ = new UILabel();
    if (labelTimeDes_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelTimeDes_ ERROR");
        return;
    }
    labelTimeDes_->SetPosition(BREATH_TRAINING_TOTALDURATION_X, BREATH_TRAINING_TOTALDURATION_Y);
    labelTimeDes_->Resize(BREATH_TRAINING_TOTALDURATION_W, BREATH_TRAINING_TOTALDURATION_H);
    labelTimeDes_->SetText("累计时长");
    labelTimeDes_->SetTextColor(Color::White());
    labelTimeDes_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BREATH_TRAINING_FONT_SIZE);
    labelTimeDes_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelTimeDes_->SetStyle(STYLE_TEXT_OPA, TEXT_OPACITY);
    group_->Add(labelTimeDes_);

    labelTimeUnit_ = new UILabel();
    if (labelTimeUnit_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelTimeUnit_ ERROR");
        return;
    }
    labelTimeUnit_->SetPosition(BREATH_TRAINING_DURATION_MINUTE_X, BREATH_TRAINING_DURATION_MINUTE_Y);
    labelTimeUnit_->Resize(BREATH_TRAINING_DURATION_MINUTE_W, BREATH_TRAINING_DURATION_MINUTE_H);
    labelTimeUnit_->SetText("分钟");
    labelTimeUnit_->SetTextColor(Color::White());
    labelTimeUnit_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BREATH_TRAINING_FONT_SIZE);
    labelTimeUnit_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelTimeUnit_->SetStyle(STYLE_TEXT_OPA, TEXT_OPACITY);
    group_->Add(labelTimeUnit_);

    labelCountDes_ = new UILabel();
    if (labelCountDes_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelCountDes_ ERROR");
        return;
    }
    labelCountDes_->SetPosition(BREATH_TRAINING_TOTAL_NUMBLE_OF_TIMES_X, BREATH_TRAINING_TOTAL_NUMBLE_OF_TIMES_Y);
    labelCountDes_->Resize(BREATH_TRAINING_TOTAL_NUMBLE_OF_TIMES_W, BREATH_TRAINING_TOTAL_NUMBLE_OF_TIMES_H);
    labelCountDes_->SetText("累计次数");
    labelCountDes_->SetTextColor(Color::White());
    labelCountDes_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BREATH_TRAINING_FONT_SIZE);
    labelCountDes_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelCountDes_->SetStyle(STYLE_TEXT_OPA, TEXT_OPACITY);
    group_->Add(labelCountDes_);

    labelCountUnit_ = new UILabel();
    if (labelCountUnit_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelCountUnit_ ERROR");
        return;
    }
    labelCountUnit_->SetPosition(BREATH_TRAINING_NUMBER_OF_TIMES_X, BREATH_TRAINING_NUMBER_OF_TIMES_Y);
    labelCountUnit_->Resize(BREATH_TRAINING_NUMBER_OF_TIMES_W, BREATH_TRAINING_NUMBER_OF_TIMES_H);
    labelCountUnit_->SetText("次");
    labelCountUnit_->SetTextColor(Color::White());
    labelCountUnit_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BREATH_TRAINING_FONT_SIZE);
    labelCountUnit_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelCountUnit_->SetStyle(STYLE_TEXT_OPA, TEXT_OPACITY);
    group_->Add(labelCountUnit_);

    labelTimeValue_ = new UILabel();
    if (labelTimeValue_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelTimeValue_ ERROR");
        return;
    }
    labelTimeValue_->SetPosition(BREATH_TRAINING_TIME_X, BREATH_TRAINING_TIME_Y);
    labelTimeValue_->Resize(BREATH_TRAINING_TIME_W, BREATH_TRAINING_TIME_H);
    labelTimeValue_->SetTextColor(Color::White());
    labelTimeValue_->SetFont(BOLD_VECTOR_FONT_FILENAME, BREATH_TRAINING_FONT_MAX_SIZE);
    labelTimeValue_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelTimeValue_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    group_->Add(labelTimeValue_);

    labelCountValue_ = new UILabel();
    if (labelCountValue_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelCountValue_ ERROR");
        return;
    }
    labelCountValue_->SetPosition(BREATH_TRAINING_SCOUR_X, BREATH_TRAINING_SCOUR_Y);
    labelCountValue_->Resize(BREATH_TRAINING_SCOUR_W, BREATH_TRAINING_SCOUR_H);
    labelCountValue_->SetTextColor(Color::White());
    labelCountValue_->SetFont(BOLD_VECTOR_FONT_FILENAME, BREATH_TRAINING_FONT_MAX_SIZE);
    labelCountValue_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelCountValue_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    group_->Add(labelCountValue_);

    AddViewToPageContainer(group_);
}

void BreathTrainingRecordsPage::OnResume()
{
    labelTimeValue_->SetText((BreathModel::GetInstance().GetTrainingDurationTime().c_str()));
    labelCountValue_->SetText((BreathModel::GetInstance().GetTrainingCount().c_str()));
}

bool BreathTrainingRecordsPage::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_TOP_TO_BOTTOM) && (event.GetStartPoint().y > Y_OFFSET)) {
        NativeAbility::GetInstance().SwitchPageInSlice(BREATH_PAGE::BREATH_MAIN_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    } else if (event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) {
        NativeAbility::GetInstance().SwitchPageInSlice(BREATH_PAGE::BREATH_MAIN_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}
}