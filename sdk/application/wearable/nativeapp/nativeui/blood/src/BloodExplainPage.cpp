/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BloodExplainPage
 * Create: 2025-06
 */

#include <string>
#include "ui_test_group.h"
#include "main/LoadImg.h"
#include "ui_resource_blood.h"
#include "blood/BloodPresenter.h"
#include "blood/BloodView.h"
#include "blood/BloodExplainPage.h"

namespace OHOS {
static BloodExplainPage *g_pBloodCalcPage = nullptr;
static const char *BLOOD_LABEL_ONE = (char *)"血氧饱和度";
static const char *BLOOD_LABEL_TWO =
    (char
         *)"血氧饱和度(SpO2)是血液中氧和血红蛋白(HbO2)的容量占全部血红蛋白的百分比，是维持人体正常机制的重要生理参数。";
static const char *BLOOD_LABEL_THREE = (char *)"测量须知";
static const char *BLOOD_LABEL_FOUR =
    (char *)"本设备支持血氧测量范围为70%~100%"
            "。在穿戴设备上启动血氧测量后，为确保测量准确，须按佩戴指引进行操作。受外界因素影响("
            "血流灌注低、手臂毛发多、有纹身、低温等)影响，测量精度可能会下降，测量结果仅供参考，不作为医疗诊断依据。";
static constexpr uint16_t BLOOD_FIRST_LABEL_FONT = 38;
static constexpr uint16_t BLOOD_LABEL_FIRST_X = 45;
static constexpr uint16_t BLOOD_LABEL_FIRST_Y = 40;
static constexpr uint16_t BLOOD_LABEL_SECOND_X = 45;
static constexpr uint16_t BLOOD_LABEL_SECOND_Y = 98;
static constexpr uint16_t BLOOD_LABEL_THIRD_X = 45;
static constexpr uint16_t BLOOD_LABEL_THIRD_Y = 514;
static constexpr uint16_t BLOOD_LABEL_FOUR_X = 45;
static constexpr uint16_t BLOOD_LABEL_FOUR_Y = 564;
static constexpr uint16_t BLOOD_LABEL_FIRST_WIDTH = 350;
static constexpr uint16_t BLOOD_LABEL_FIRST_HEIGHT = 400;
static constexpr uint16_t BLOOD_LABEL_POSITION_WIDTH = 200;
static constexpr uint16_t BLOOD_LABEL_POSITION_HEIGHT = 60;
static constexpr uint16_t BLOOD_LABEL2_POSITION_Y = 404;
static constexpr uint16_t BLOOD_BUTTONLEVEL_POSITION_X = 127;
static constexpr uint16_t BLOOD_BUTTONLABLE_POSITION_X = 217;
static constexpr uint16_t BLOOD_BUTTONLEVEL2_POSITION_Y = 440;
static constexpr uint16_t BLOOD_BUTTONLEVEL3_POSITION_Y = 474;
static constexpr uint16_t LIST_BLANK_SIZE = 50;
static constexpr uint16_t INDEX_0 = 0;
static constexpr uint16_t INDEX_1 = 1;
static constexpr uint16_t INDEX_2 = 2;
static constexpr uint16_t INDEX_3 = 3;
REGIST_SLICE_PAGE(VIEW_BLOOD, BLOOD_PAGES::BLOOD_EXPLAIN_PAGE, BloodExplainPage, false);

BloodExplainPage::BloodExplainPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BloodExplainPage::BloodExplainPage");
}

BloodExplainPage::~BloodExplainPage()
{
    if (scrollGroup_ != nullptr) {
        scrollGroup_->RemoveAll();
        delete scrollGroup_;
        scrollGroup_ = nullptr;
    }

    if (imgNormal_ != nullptr) {
        delete imgNormal_;
        imgNormal_ = nullptr;
    }

    if (imgGeneral_ != nullptr) {
        delete imgGeneral_;
        imgGeneral_ = nullptr;
    }

    if (imgAbnormal_ != nullptr) {
        delete imgAbnormal_;
        imgAbnormal_ = nullptr;
    }

    if (labelNormal_ != nullptr) {
        delete labelNormal_;
        labelNormal_ = nullptr;
    }

    if (labelGeneral_ != nullptr) {
        delete labelGeneral_;
        labelGeneral_ = nullptr;
    }

    if (labelAbnormal_ != nullptr) {
        delete labelAbnormal_;
        labelAbnormal_ = nullptr;
    }

    for (int i = 0; i < LABEL_DES_COUNT; i++) {
        if (labelDes_[i] != nullptr) {
            delete labelDes_[i];
            labelDes_[i] = nullptr;
        }
    }
    ImageCacheManager::GetInstance().UnloadSingleRes(BLOOD_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BloodExplainPage::~BloodExplainPage");
}

void BloodExplainPage::OnStart(void *data)
{
    scrollGroup_ = new UIScrollView();
    if (scrollGroup_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new scrollGroup_ fail");
        return;
    }
    scrollGroup_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    scrollGroup_->SetThrowDrag(true);
    scrollGroup_->SetScrollBlankSize(LIST_BLANK_SIZE);
    scrollGroup_->SetOnDragListener(this);
    scrollGroup_->SetDraggable(true);
    scrollGroup_->SetTouchable(true);
    if (!InitSimilarLabel(INDEX_0, BLOOD_LABEL_FIRST_X, BLOOD_LABEL_FIRST_Y, BLOOD_LABEL_ONE)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new InitSimilarLabel[INDEX_0] error");
    }
    if (!InitSimilarLabel(INDEX_1, BLOOD_LABEL_SECOND_X, BLOOD_LABEL_SECOND_Y, BLOOD_LABEL_TWO)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new InitSimilarLabel[INDEX_1] error");
    }
    if (!InitSimilarLabel(INDEX_2, BLOOD_LABEL_THIRD_X, BLOOD_LABEL_THIRD_Y, BLOOD_LABEL_THREE)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new InitSimilarLabel[INDEX_2] error");
    }
    if (!InitSimilarLabel(INDEX_3, BLOOD_LABEL_FOUR_X, BLOOD_LABEL_FOUR_Y, BLOOD_LABEL_FOUR)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new InitSimilarLabel[INDEX_3] error");
    }

    imgNormal_ = new UIImageView();
    if (imgNormal_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new imgNormal_ fail");
        return;
    }
    imgNormal_->SetPosition(BLOOD_BUTTONLEVEL_POSITION_X, BLOOD_LABEL2_POSITION_Y);
    LOADIMG::LoadImageViewImg(imgNormal_, BLOOD_IMAGE, IMAGE_BLOOD_NORMAL);
    scrollGroup_->Add(imgNormal_);

    labelNormal_ = new UILabel();
    if (labelNormal_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelNormal_ fail");
        return;
    }
    labelNormal_->SetText(">=94%");
    labelNormal_->SetPosition(BLOOD_BUTTONLABLE_POSITION_X, BLOOD_LABEL2_POSITION_Y);
    labelNormal_->Resize(BLOOD_LABEL_POSITION_WIDTH, BLOOD_LABEL_POSITION_HEIGHT);
    labelNormal_->SetTextColor(Color::White());
    scrollGroup_->Add(labelNormal_);

    imgGeneral_ = new UIImageView();
    if (imgGeneral_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new imgGeneral_ fail");
        return;
    }
    imgGeneral_->SetPosition(BLOOD_BUTTONLEVEL_POSITION_X, BLOOD_BUTTONLEVEL2_POSITION_Y);
    LOADIMG::LoadImageViewImg(imgGeneral_, BLOOD_IMAGE, IMAGE_BLOOD_COMMON);
    scrollGroup_->Add(imgGeneral_);

    labelGeneral_ = new UILabel();
    if (labelGeneral_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelGeneral_ fail");
        return;
    }
    labelGeneral_->SetText("70%~93%");
    labelGeneral_->SetPosition(BLOOD_BUTTONLABLE_POSITION_X, BLOOD_BUTTONLEVEL2_POSITION_Y);
    labelGeneral_->Resize(BLOOD_LABEL_POSITION_WIDTH, BLOOD_LABEL_POSITION_HEIGHT);
    labelGeneral_->SetTextColor(Color::White());
    scrollGroup_->Add(labelGeneral_);

    imgAbnormal_ = new UIImageView();
    if (imgAbnormal_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new imgAbnormal_ fail");
        return;
    }
    imgAbnormal_->SetPosition(BLOOD_BUTTONLEVEL_POSITION_X, BLOOD_BUTTONLEVEL3_POSITION_Y);
    LOADIMG::LoadImageViewImg(imgAbnormal_, BLOOD_IMAGE, IMAGE_BLOOD_ABNORMAL);
    scrollGroup_->Add(imgAbnormal_);

    labelAbnormal_ = new UILabel();
    if (labelAbnormal_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelAbnormal_ fail");
        return;
    }
    labelAbnormal_->SetText("<70%");
    labelAbnormal_->SetPosition(BLOOD_BUTTONLABLE_POSITION_X, BLOOD_BUTTONLEVEL3_POSITION_Y);
    labelAbnormal_->Resize(BLOOD_LABEL_POSITION_WIDTH, BLOOD_LABEL_POSITION_HEIGHT);
    labelAbnormal_->SetTextColor(Color::White());
    scrollGroup_->Add(labelAbnormal_);
    AddViewToPageContainer(scrollGroup_);
}

bool BloodExplainPage::InitSimilarLabel(int16 num, int16 labelX, int16 labelY, const char *labelText)
{
    if (num < 0 || num >= LABEL_DES_COUNT) {
        return false;
    }
    labelDes_[num] = new UILabel();
    if (labelDes_[num] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelDes_[%d] error", num);
        return false;
    }
    labelDes_[num]->SetPosition(labelX, labelY, BLOOD_LABEL_FIRST_WIDTH, BLOOD_LABEL_FIRST_HEIGHT);
    labelDes_[num]->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);
    labelDes_[num]->SetText(labelText);
    if (num == INDEX_0) {
        labelDes_[num]->SetFont(BOLD_VECTOR_FONT_FILENAME, BLOOD_FIRST_LABEL_FONT);
    } else {
        labelDes_[num]->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BLOOD_FIRST_LABEL_FONT);
    }
    labelDes_[num]->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    scrollGroup_->Add(labelDes_[num]);
    return true;
}

bool BloodExplainPage::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().SwitchPageInSlice(BLOOD_PAGES::BLOOD_RESULT_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}
}
