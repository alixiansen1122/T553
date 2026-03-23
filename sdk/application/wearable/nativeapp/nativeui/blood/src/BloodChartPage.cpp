/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BloodChartPage
 * Create: 2025-06
 */

#include <string>
#include <random>
#include <iostream>
#include <chrono>
#include "components/ui_list_nested.h"
#include "components/ui_chart.h"
#include "components/ui_scroll_view_nested.h"
#include "common/image_cache_manager.h"
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "wearable_log.h"
#include "main/LoadImg.h"
#include "AppViewIDs.h"
#include "ui_resource_blood.h"
#include "blood/BloodModel.h"
#include "blood/BloodChartPage.h"

namespace OHOS {
static constexpr uint16_t PERSEC_BLOOD_FONT_SIZE = 60;
static constexpr uint16_t PERSEC_BLOOD_X = 203;
static constexpr uint16_t PERSEC_BLOOD_Y = 40;
static constexpr uint16_t PERSEC_BLOOD_WIDTH = 260;
static constexpr uint16_t PERSEC_BLOOD_HEIGHT = 53;
static constexpr uint16_t PERSEC_BLOOD_TEXT_X = 188;
static constexpr uint16_t PERSEC_BLOOD_TEXT_Y = 96;
static constexpr uint16_t PERSEC_BLOOD_TEXT_HEIGHT = 64;
static constexpr uint16_t COORDINATE_POINT_X = 70;
static constexpr uint16_t COORDINATE_POINT_Y = 120;
static constexpr uint16_t COORDINATE_HEIGHT = 156;
static constexpr uint16_t COORDINATE_WIDTH = 330;
static constexpr uint16_t BLOOD_LIMIT_FONT_SIZE = 30;
static constexpr uint16_t TOP_TEXT_WIDTH = 232;
static constexpr uint16_t TOP_TEXT_HEIGHT = 76;
static constexpr uint16_t TOP_TEXT_X = 107;
static constexpr uint16_t TOP_TEXT_Y = 352;
static constexpr uint16_t BOTTOM_TEXT_WIDTH = 35;
static constexpr uint16_t BOTTOM_TEXT_HEIGHT = 36;
static constexpr uint16_t BLOOD_LIMIT_FONT_SIZE_ONE = 24;
static constexpr uint16_t BLOOD_LABLE_FONT_SIZE_ONE = 16;
static constexpr uint16_t BOTTOM_TEXT_WIDTH_ONE = 89;
static constexpr uint16_t BOTTOM_TEXT_HEIGHT_ONE = 29;
static constexpr uint16_t BOTTOM_TEXT_ONE_X = 188;
static constexpr uint16_t BOTTOM_TEXT_ONE_Y = 105;
static constexpr uint16_t LABLE_XAXIS1_POSION_X = 50;
static constexpr uint16_t LABLE_XAXIS2_POSION_X = 210;
static constexpr uint16_t LABLE_XAXIS3_POSION_X = 370;
static constexpr uint16_t LABLE_XAXIS_POSION_Y = 275;
static constexpr uint16_t BLOOD_XAXIS_SETMARKNUM = 40;
static constexpr uint16_t BLOOD_YAXIS_SETMARKNUM = 3;
static constexpr uint16_t BLOOD_XAXIS_RANGE_MAX = 4;
static constexpr uint16_t BLOOD_YAXIS_RANGE_MAX = 200;
static constexpr uint16_t BLOOD_STYLE_LINE_NUM = 3;
static constexpr uint16_t BLOOD_CHART_IMAGE_POSION_X = 15;
static constexpr uint16_t BLOOD_CHART_IMAGE_POSION_Y = 140;
static constexpr uint16_t BLOOD_BIND_IMAGE_X = 140;
static constexpr uint16_t BLOOD_BIND_IMAGE_Y = 50;
static constexpr uint16_t BLOOD_BIND_IMAGE_WIDTH = 50;
static constexpr uint16_t BLOOD_BIND_IMAGE_HEIGHT = 50;
static constexpr char *BLOOD_RETEST_BUTTON = "retestButton";

const int g_imageAnimatorInfo[BLOOD_ANIMATOR_IMAGE_NUM] = {
    IMAGE_BLOOD_FRAME00, IMAGE_BLOOD_FRAME01, IMAGE_BLOOD_FRAME02, IMAGE_BLOOD_FRAME03, IMAGE_BLOOD_FRAME04,
    IMAGE_BLOOD_FRAME05, IMAGE_BLOOD_FRAME06, IMAGE_BLOOD_FRAME07, IMAGE_BLOOD_FRAME08, IMAGE_BLOOD_FRAME09,
    IMAGE_BLOOD_FRAME10, IMAGE_BLOOD_FRAME11, IMAGE_BLOOD_FRAME12, IMAGE_BLOOD_FRAME13, IMAGE_BLOOD_FRAME14,
    IMAGE_BLOOD_FRAME15, IMAGE_BLOOD_FRAME16, IMAGE_BLOOD_FRAME17, IMAGE_BLOOD_FRAME18, IMAGE_BLOOD_FRAME19,
    IMAGE_BLOOD_FRAME20, IMAGE_BLOOD_FRAME21, IMAGE_BLOOD_FRAME22, IMAGE_BLOOD_FRAME23, IMAGE_BLOOD_FRAME24,
    IMAGE_BLOOD_FRAME25, IMAGE_BLOOD_FRAME26, IMAGE_BLOOD_FRAME27, IMAGE_BLOOD_FRAME28, IMAGE_BLOOD_FRAME29,
};

REGIST_SLICE_PAGE(VIEW_BLOOD, BLOOD_PAGES::BLOOD_CHART_PAGE, BloodChartPage, false);

BloodChartPage::BloodChartPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BloodChartPage::BloodChartPage");
}

void BloodChartPage::OnStart(void *data)
{
    group_ = new UIViewGroup();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new UIViewGroup fail");
        return;
    }
    group_->SetPosition(0, 0);
    group_->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);

    mainImg_ = new UIImageView();
    if (mainImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new mainImg_ fail");
        return;
    }
    mainImg_->SetPosition(0, 0);
    mainImg_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    LOADIMG::LoadImageViewImg(mainImg_, BLOOD_IMAGE, IMAGE_BLOOD_BG);
    group_->Add(mainImg_);

    labelTitle_ = new UILabel();
    if (labelTitle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelTitle_ fail");
        return;
    }
    labelTitle_->SetPosition(PERSEC_BLOOD_X, PERSEC_BLOOD_Y);
    labelTitle_->Resize(PERSEC_BLOOD_WIDTH, PERSEC_BLOOD_HEIGHT);
    labelTitle_->SetStyle(STYLE_TEXT_FONT, PERSEC_BLOOD_FONT_SIZE);
    labelTitle_->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    labelTitle_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelTitle_->SetRollSpeed(0);
    labelTitle_->SetFont(BOLD_VECTOR_FONT_FILENAME, PERSEC_BLOOD_FONT_SIZE);

    labelTestTime_ = new UILabel();
    if (labelTestTime_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelTestTime_ fail");
        return;
    }
    labelTestTime_->SetText("刚刚");
    labelTestTime_->SetPosition(BOTTOM_TEXT_ONE_X, BOTTOM_TEXT_ONE_Y, BOTTOM_TEXT_WIDTH_ONE, BOTTOM_TEXT_HEIGHT_ONE);
    labelTestTime_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BLOOD_LIMIT_FONT_SIZE_ONE);
    labelTestTime_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelTestTime_->SetStyle(STYLE_TEXT_COLOR, 0xFFCCCCCC);
    labelTestTime_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    bloodMeasurementInfo info = BloodModel::GetInstance().GetBlood();
    std::string strBlood = std::to_string(info.blood) + "%";
    labelTitle_->SetText(strBlood.c_str());

    dataSerial_ = new UIChartDataSerial();
    if (dataSerial_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new dataSerial_ fail");
        return;
    }
    dataSerial_->SetMaxDataCount(BLOOD_XAXIS_RANGE_MAX);
    // 打桩数据，真实测量数据的存储和更新需要客户自实现
    Point pointArray[BLOOD_XAXIS_RANGE_MAX] = {{0, info.blood}, {1, 75}, {2, 100}, {3, 80}};
    dataSerial_->AddPoints(pointArray, BLOOD_XAXIS_RANGE_MAX);
    dataSerial_->SetLineColor(Color::Green());
    dataSerial_->SetFillColor(Color::Green());
    dataSerial_->EnableGradient(true);

    chartPillar_ = new UIChartPillar();
    if (chartPillar_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new chartPillar_ fail");
        return;
    }
    chartPillar_->SetPosition(COORDINATE_POINT_X, COORDINATE_POINT_Y);
    chartPillar_->SetWidth(COORDINATE_WIDTH);
    chartPillar_->SetHeight(COORDINATE_HEIGHT);
    chartPillar_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    chartPillar_->SetStyle(STYLE_BACKGROUND_OPA, 0);
    chartPillar_->SetStyle(STYLE_LINE_WIDTH, BLOOD_STYLE_LINE_NUM);

    chartPillar_->GetXAxis().SetDataRange(0, BLOOD_XAXIS_RANGE_MAX);
    chartPillar_->GetXAxis().SetMarkNum(BLOOD_XAXIS_SETMARKNUM);
    chartPillar_->GetYAxis().SetDataRange(0, BLOOD_YAXIS_RANGE_MAX);
    chartPillar_->GetYAxis().SetMarkNum(BLOOD_YAXIS_SETMARKNUM);
    chartPillar_->GetYAxis().SetVisible(false);
    chartPillar_->AddDataSerial(dataSerial_);
    group_->Add(chartPillar_);

    chartImg_ = new UIImageView();
    if (chartImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new chartImg_ fail");
        return;
    }
    chartImg_->SetPosition(BLOOD_CHART_IMAGE_POSION_X, BLOOD_CHART_IMAGE_POSION_Y);
    LOADIMG::LoadImageViewImg(chartImg_, BLOOD_IMAGE, IMAGE_BLOOD_SCALE);
    chartImg_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    group_->Add(chartImg_);

    labelXAxisPointOne_ = new UILabel();
    if (labelXAxisPointOne_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelXAxisPointOne_ fail");
        return;
    }
    labelXAxisPointOne_->SetText("00:00");
    labelXAxisPointOne_->SetPosition(LABLE_XAXIS1_POSION_X, LABLE_XAXIS_POSION_Y, BOTTOM_TEXT_WIDTH_ONE,
                                     BOTTOM_TEXT_HEIGHT_ONE);
    labelXAxisPointOne_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BLOOD_LABLE_FONT_SIZE_ONE);
    group_->Add(labelXAxisPointOne_);

    labelXAxisPointTwo_ = new UILabel();
    if (labelXAxisPointTwo_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelXAxisPointTwo_ fail");
        return;
    }
    labelXAxisPointTwo_->SetText("12:00");
    labelXAxisPointTwo_->SetPosition(LABLE_XAXIS2_POSION_X, LABLE_XAXIS_POSION_Y, BOTTOM_TEXT_WIDTH_ONE,
                                     BOTTOM_TEXT_HEIGHT_ONE);
    labelXAxisPointTwo_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BLOOD_LABLE_FONT_SIZE_ONE);
    group_->Add(labelXAxisPointTwo_);

    labelXAxisPointThree_ = new UILabel();
    if (labelXAxisPointThree_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelXAxisPointThree_ fail");
        return;
    }
    labelXAxisPointThree_->SetText("24:00");
    labelXAxisPointThree_->SetPosition(LABLE_XAXIS3_POSION_X, LABLE_XAXIS_POSION_Y, BOTTOM_TEXT_WIDTH_ONE,
                                       BOTTOM_TEXT_HEIGHT_ONE);
    labelXAxisPointThree_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BLOOD_LABLE_FONT_SIZE_ONE);
    group_->Add(labelXAxisPointThree_);

    labelButtonRetest_ = new UILabelButton();
    if (labelButtonRetest_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelButtonRetest_ fail");
        return;
    }
    labelButtonRetest_->SetPosition(TOP_TEXT_X, TOP_TEXT_Y, TOP_TEXT_WIDTH, TOP_TEXT_HEIGHT);
    LOADIMG::LoadBtnImage(labelButtonRetest_, BLOOD_IMAGE, IMAGE_BLOOD_AGAIN, IMAGE_BLOOD_AGAIN);
    labelButtonRetest_->SetViewId(BLOOD_RETEST_BUTTON);
    labelButtonRetest_->SetOnClickListener(this);
    labelButtonRetest_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    labelButtonRetest_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    labelButtonRetest_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    labelButtonRetest_->SetVisible(true);
    group_->Add(labelButtonRetest_);

    group_->Add(labelTitle_);
    group_->Add(labelTestTime_);
    group_->SetOnDragListener(this);
    group_->SetDraggable(true);
    group_->SetTouchable(true);
    InitImageAnimator();

    AddViewToPageContainer(group_);
}

bool BloodChartPage::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if (event.GetDragDirection() == DragEvent::DIRECTION_TOP_TO_BOTTOM) {
        NativeAbility::GetInstance().SwitchPageInSlice(BLOOD_PAGES::BLOOD_RESULT_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    } else if (event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) {
        NativeAbility::GetInstance().SwitchPageInSlice(BLOOD_PAGES::BLOOD_MAIN_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool BloodChartPage::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    if (strcmp (view.GetViewId(), BLOOD_RETEST_BUTTON) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(BLOOD_PAGES::BLOOD_READY_PAGE,
                                                       TransitionType::TRANSITION_INVALID, true);
    }
    return true;
}

void BloodChartPage::InitImageAnimator()
{
    imageAnimator_ = new UIImageAnimatorView();
    if (imageAnimator_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new imageAnimator_ fail");
        return;
    }
    for (uint8_t i = 0; i < BLOOD_ANIMATOR_IMAGE_NUM; i++) {
        imageAnimatorImageInfo_[i].imageInfo =
            ImageCacheManager::GetInstance().LoadOneInMultiRes(g_imageAnimatorInfo[i], BLOOD_IMAGE);
        imageAnimatorImageInfo_[i].imageType = IMG_SRC_IMAGE_INFO;
        imageAnimatorImageInfo_[i].pos.x = BLOOD_BIND_IMAGE_X;
        imageAnimatorImageInfo_[i].pos.y = BLOOD_BIND_IMAGE_Y;
        imageAnimatorImageInfo_[i].width = BLOOD_BIND_IMAGE_WIDTH;
        imageAnimatorImageInfo_[i].height = BLOOD_BIND_IMAGE_HEIGHT;
    }

    imageAnimator_->SetPosition(PERSEC_BLOOD_TEXT_X, PERSEC_BLOOD_TEXT_Y);
    imageAnimator_->SetImageAnimatorSrc(imageAnimatorImageInfo_,
                                        BLOOD_ANIMATOR_IMAGE_NUM, BLOOD_ANIMATOR_IMAGE_UPDATE);
    imageAnimator_->SetRepeat(true);
    imageAnimator_->SetAutoEnable(false);
    imageAnimator_->SetResizeMode(UIImageView::FILL);
    imageAnimator_->Start();
    imageAnimator_->LayoutCenterOfParent();
    imageAnimator_->SetDraggable(true);
    group_->Add(imageAnimator_);
}

BloodChartPage::~BloodChartPage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }

    if (labelTitle_ != nullptr) {
        delete labelTitle_;
        labelTitle_ = nullptr;
    }

    if (labelButtonRetest_ != nullptr) {
        delete labelButtonRetest_;
        labelButtonRetest_ = nullptr;
    }

    if (labelTestTime_ != nullptr) {
        delete labelTestTime_;
        labelTestTime_ = nullptr;
    }

    if (dataSerial_ != nullptr) {
        delete dataSerial_;
        dataSerial_ = nullptr;
    }

    if (chartPillar_ != nullptr) {
        delete chartPillar_;
        chartPillar_ = nullptr;
    }

    if (labelXAxisPointOne_ != nullptr) {
        delete labelXAxisPointOne_;
        labelXAxisPointOne_ = nullptr;
    }

    if (labelXAxisPointTwo_ != nullptr) {
        delete labelXAxisPointTwo_;
        labelXAxisPointTwo_ = nullptr;
    }

    if (labelXAxisPointThree_ != nullptr) {
        delete labelXAxisPointThree_;
        labelXAxisPointThree_ = nullptr;
    }

    if (mainImg_ != nullptr) {
        delete mainImg_;
        mainImg_ = nullptr;
    }

    if (chartImg_ != nullptr) {
        delete chartImg_;
        chartImg_ = nullptr;
    }

    if (imageAnimator_ != nullptr) {
        delete imageAnimator_;
        imageAnimator_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadSingleRes(BLOOD_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BloodChartPage::~BloodChartPage");
}
}