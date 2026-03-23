/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: HeartRateResultGroup
 * Create: 2025-06
 */

#include <string>
#include "components/ui_list_nested.h"
#include "components/ui_chart.h"
#include "components/ui_scroll_view_nested.h"
#include "common/image_cache_manager.h"
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "wearable_log.h"
#include "main/LoadImg.h"
#include "AppViewIDs.h"
#include "ui_resource_heartRate.h"
#include "heartrate/HeartRatePresenter.h"
#include "heartrate/HeartRateModel.h"
#include "heartrate/HeartRateResultGroup.h"

static constexpr uint16_t PERSEC_HEART_RATE_FONT_SIZE = 38;
static constexpr uint16_t PERSEC_HEART_RATE_X = 195;
static constexpr uint16_t PERSEC_HEART_RATE_Y = 40;
static constexpr uint16_t PERSEC_HEART_RATE_WIDTH = 260;
static constexpr uint16_t PERSEC_HEART_RATE_HEIGHT = 53;
static constexpr uint16_t PERSEC_HEART_TEXT_FONT_SIZE = 56;
static constexpr uint16_t PERSEC_HEART_TEXT_X = 188;
static constexpr uint16_t PERSEC_HEART_TEXT_Y = 96;
static constexpr uint16_t PERSEC_HEART_TEXT_WIDTH = 80;
static constexpr uint16_t PERSEC_HEART_TEXT_HEIGHT = 64;
static constexpr uint16_t PERSEC_HEART_TEXT_FONT_SIZE_ONE = 21;
static constexpr uint16_t PERSEC_HEART_TEXT_ONE_X = 259;
static constexpr uint16_t PERSEC_HEART_TEXT_ONE_Y = 128;
static constexpr uint16_t PERSEC_HEART_TEXT_WIDTH_ONE = 55;
static constexpr uint16_t PERSEC_HEART_TEXT_HEIGHT_ONE = 24;
static constexpr uint16_t COORDINATE_POINT_X = 70;
static constexpr uint16_t COORDINATE_POINT_Y = 207;
static constexpr uint16_t COORDINATE_HEIGHT = 156;
static constexpr uint16_t COORDINATE_WIDTH = 330;
static constexpr uint16_t HEART_RATE_LIMIT_FONT_SIZE = 25;
static constexpr uint16_t HEART_RATE_X_FONT_SIZE = 16;
static constexpr uint16_t TOP_TEXT_WIDTH = 52;
static constexpr uint16_t TOP_TEXT_HEIGHT = 36;
static constexpr uint16_t TOP_TEXT_X = 177;
static constexpr uint16_t TOP_TEXT_Y = 392;
static constexpr uint16_t BOTTOM_TEXT_WIDTH = 35;
static constexpr uint16_t BOTTOM_TEXT_HEIGHT = 36;
static constexpr uint16_t BOTTOM_TEXT_X = 274;
static constexpr uint16_t BOTTOM_TEXT_Y = 392;
static constexpr uint16_t HEART_RATE_LIMIT_FONT_SIZE_ONE = 24;
static constexpr uint16_t BOTTOM_TEXT_WIDTH_ONE = 89;
static constexpr uint16_t BOTTOM_TEXT_HEIGHT_ONE = 29;
static constexpr uint16_t BOTTOM_TEXT_ONE_X = 188;
static constexpr uint16_t BOTTOM_TEXT_ONE_Y = 160;
static constexpr uint16_t ANIMATOR_IMAGE_X = 120;
static constexpr uint16_t ANIMATOR_IMAGE_Y = 98;
static constexpr uint16_t ANIMATOR_IMAGE_WIDTH = 60;
static constexpr uint16_t ANIMATOR_IMAGE_HEIGHT = 60;
static constexpr uint16_t HEART_LABLE_ONE_POSION_X = 63;
static constexpr uint16_t HEART_LABLE_THREE_POSION_X = 210;
static constexpr uint16_t HEART_LABLE_FIVE_POSION_X = 350;
static constexpr uint16_t HEART_LABLE_POSION_X = 10;
static constexpr uint16_t HEART_LABLE_POSION_Y = 369;
static constexpr uint16_t HEART_LABLE1_POSION_Y = 320;
static constexpr uint16_t HEART_LABLE2_POSION_Y = 290;
static constexpr uint16_t HEART_LABLE3_POSION_Y = 260;
static constexpr uint16_t HEART_LABLE4_POSION_Y = 230;
static constexpr uint16_t IMAGE_RED_TEXT_X = 157;
static constexpr uint16_t IMAGE_RED_TEXT_Y = 407;
static constexpr uint16_t IMAGE_BACK_TEXT_X = 254;
static constexpr uint16_t IMAGE_BACK_TEXT_Y = 407;
static constexpr uint16_t XAXIS_SETMARKNUM = 40;
static constexpr uint16_t XAXIS_LINENUM = 3;
static constexpr uint16_t YAXIS_SETMARKNUM = 5;
static constexpr uint16_t XAXIS_SETDATARANGE = 45;
static constexpr uint16_t YAXIS_SETDATARANGE = 200;
static constexpr uint16_t CHART_LINE_NUM_ONE = 10;
static constexpr uint16_t CHART_LINE_NUM_TWO = 60;

namespace OHOS {
const int g_imageAnimatorInfo[HEARTRATE_ANIMATOR_IMAGE_NUM] = {
    IMAGE_HEART_RATE_FRAME00, IMAGE_HEART_RATE_FRAME01, IMAGE_HEART_RATE_FRAME02, IMAGE_HEART_RATE_FRAME03,
    IMAGE_HEART_RATE_FRAME04, IMAGE_HEART_RATE_FRAME05, IMAGE_HEART_RATE_FRAME06, IMAGE_HEART_RATE_FRAME07,
    IMAGE_HEART_RATE_FRAME08, IMAGE_HEART_RATE_FRAME09, IMAGE_HEART_RATE_FRAME10, IMAGE_HEART_RATE_FRAME11,
    IMAGE_HEART_RATE_FRAME12, IMAGE_HEART_RATE_FRAME13, IMAGE_HEART_RATE_FRAME14, IMAGE_HEART_RATE_FRAME15,
    IMAGE_HEART_RATE_FRAME16, IMAGE_HEART_RATE_FRAME17, IMAGE_HEART_RATE_FRAME18, IMAGE_HEART_RATE_FRAME19,
    IMAGE_HEART_RATE_FRAME20, IMAGE_HEART_RATE_FRAME21, IMAGE_HEART_RATE_FRAME22, IMAGE_HEART_RATE_FRAME23,
    IMAGE_HEART_RATE_FRAME24, IMAGE_HEART_RATE_FRAME25, IMAGE_HEART_RATE_FRAME26, IMAGE_HEART_RATE_FRAME27,
    IMAGE_HEART_RATE_FRAME28, IMAGE_HEART_RATE_FRAME29,
};

HeartRateResultGroup::HeartRateResultGroup()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HeartRateResultGroup::HeartRateResultGroup");
    InitPage();
}

void HeartRateResultGroup::InitPage()
{
    SetPosition(0, 0);
    Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);

    mainImg_ = new UIImageView();
    if (mainImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new mainImg_ fail");
        return;
    }
    mainImg_->SetPosition(0, 0);
    mainImg_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    LOADIMG::LoadImageViewImg(mainImg_, HEART_RATE_IMAGE, IMAGE_HEART_RATE_BG2);
    Add(mainImg_);

    labelTitle_ = new UILabel();
    if (labelTitle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelTitle_ fail");
        return;
    }
    labelTitle_->SetText("心率");
    labelTitle_->SetPosition(PERSEC_HEART_RATE_X, PERSEC_HEART_RATE_Y, PERSEC_HEART_RATE_WIDTH,
                             PERSEC_HEART_RATE_HEIGHT);
    labelTitle_->SetFont(BOLD_VECTOR_FONT_FILENAME, PERSEC_HEART_RATE_FONT_SIZE);
    Add(labelTitle_);

    labelHeartRateValue_ = new UILabel();
    if (labelHeartRateValue_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelHeartRateValue_ fail");
        return;
    }
    labelHeartRateValue_->SetPosition(PERSEC_HEART_TEXT_X, PERSEC_HEART_TEXT_Y, PERSEC_HEART_TEXT_WIDTH,
                                      PERSEC_HEART_TEXT_HEIGHT);
    labelHeartRateValue_->SetFont(BOLD_VECTOR_FONT_FILENAME, PERSEC_HEART_TEXT_FONT_SIZE);
    heartRateMeasurementInfo info = HeartRateModel::GetInstance().GetHeartRate();
    std::string strHeartRate = std::to_string(info.heartRate);
    labelHeartRateValue_->SetText(strHeartRate.c_str());
    Add(labelHeartRateValue_);

    labelUnitText_ = new UILabel();
    if (labelUnitText_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelUnitText_ fail");
        return;
    }
    labelUnitText_->SetText("次/分");
    labelUnitText_->SetPosition(PERSEC_HEART_TEXT_ONE_X, PERSEC_HEART_TEXT_ONE_Y, PERSEC_HEART_TEXT_WIDTH_ONE,
                                PERSEC_HEART_TEXT_HEIGHT_ONE);
    labelUnitText_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, PERSEC_HEART_TEXT_FONT_SIZE_ONE);
    Add(labelUnitText_);

    dataSerial_ = new UIChartDataSerial();
    if (dataSerial_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new dataSerial_ fail");
        return;
    }
    dataSerial_->SetMaxDataCount(XAXIS_SETDATARANGE);
    // 打桩数据，真实测量数据的存储和更新需要客户自实现
    Point pointArray[XAXIS_SETDATARANGE] = {
        {0, 100},  {1, 110},  {2, 112},  {3, 114},  {4, 116},  {5, 118},  {6, 120},  {7, 122},  {8, 124},
        {9, 126},  {10, 128}, {11, 140}, {12, 140}, {13, 160}, {14, 160}, {15, 160}, {16, 176}, {17, 180},
        {18, 192}, {19, 160}, {20, 176}, {21, 172}, {22, 172}, {23, 148}, {24, 146}, {25, 144}, {26, 120},
        {27, 110}, {28, 110}, {29, 108}, {30, 106}, {31, 104}, {32, 102}, {33, 88},  {34, 86},  {35, 78},
        {36, 78},  {37, 78},  {38, 80},  {39, 80},  {40, 82},  {41, 82},  {42, 88},  {43, 90},  {44, 96}};
    dataSerial_->AddPoints(pointArray, XAXIS_SETDATARANGE);
    dataSerial_->SetLineColor(Color::Red());
    dataSerial_->SetFillColor(Color::Red());
    dataSerial_->EnableGradient(true);

    chartPolyline_ = new UIChartPolyline();
    if (chartPolyline_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new chartPolyline_ fail");
        return;
    }
    chartPolyline_->SetPosition(COORDINATE_POINT_X, COORDINATE_POINT_Y);
    chartPolyline_->SetWidth(COORDINATE_WIDTH);
    chartPolyline_->SetHeight(COORDINATE_HEIGHT);
    chartPolyline_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    chartPolyline_->SetStyle(STYLE_BACKGROUND_OPA, 0);
    chartPolyline_->SetStyle(STYLE_LINE_WIDTH, XAXIS_LINENUM);

    UIXAxis &xAxis = chartPolyline_->GetXAxis();
    UIYAxis &yAxis = chartPolyline_->GetYAxis();
    xAxis.SetDataRange(0, XAXIS_SETDATARANGE);
    yAxis.SetDataRange(0, YAXIS_SETDATARANGE);
    xAxis.SetMarkNum(XAXIS_SETMARKNUM);
    yAxis.SetMarkNum(YAXIS_SETMARKNUM);
    xAxis.SetStyle(STYLE_LINE_COLOR, 0x14FF5F5F5);
    yAxis.SetStyle(STYLE_LINE_COLOR, 0x14FF5F5F5);
    chartPolyline_->SetGradientOpacity(CHART_LINE_NUM_ONE, CHART_LINE_NUM_TWO);
    chartPolyline_->AddDataSerial(dataSerial_);
    Add(chartPolyline_);

    labelXAxisPointOne_ = new UILabel();
    if (labelXAxisPointOne_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelXAxisPointOne_ fail");
        return;
    }
    labelXAxisPointOne_->SetText("00:00");
    labelXAxisPointOne_->SetPosition(HEART_LABLE_ONE_POSION_X, HEART_LABLE_POSION_Y, TOP_TEXT_WIDTH, TOP_TEXT_HEIGHT);
    labelXAxisPointOne_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, HEART_RATE_X_FONT_SIZE);
    Add(labelXAxisPointOne_);

    labelXAxisPointTwo_ = new UILabel();
    if (labelXAxisPointTwo_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelXAxisPointTwo_ fail");
        return;
    }
    labelXAxisPointTwo_->SetText("12:00");
    labelXAxisPointTwo_->SetPosition(HEART_LABLE_THREE_POSION_X, HEART_LABLE_POSION_Y, TOP_TEXT_WIDTH, TOP_TEXT_HEIGHT);
    labelXAxisPointTwo_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, HEART_RATE_X_FONT_SIZE);
    Add(labelXAxisPointTwo_);
    labelXAxisPointThree_ = new UILabel();
    if (labelXAxisPointThree_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelXAxisPointThree_ fail");
        return;
    }
    labelXAxisPointThree_->SetText("24:00");
    labelXAxisPointThree_->SetPosition(HEART_LABLE_FIVE_POSION_X, HEART_LABLE_POSION_Y, TOP_TEXT_WIDTH,
                                       TOP_TEXT_HEIGHT);
    labelXAxisPointThree_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, HEART_RATE_X_FONT_SIZE);
    Add(labelXAxisPointThree_);

    labelYAxisPointOne_ = new UILabel();
    if (labelYAxisPointOne_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelYAxisPointOne_ fail");
        return;
    }
    labelYAxisPointOne_->SetText("50");
    labelYAxisPointOne_->SetPosition(HEART_LABLE_POSION_X, HEART_LABLE1_POSION_Y, TOP_TEXT_WIDTH, TOP_TEXT_HEIGHT);
    labelYAxisPointOne_->SetAlign(TEXT_ALIGNMENT_RIGHT);
    labelYAxisPointOne_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, HEART_RATE_X_FONT_SIZE);
    Add(labelYAxisPointOne_);

    labelYAxisPointTwo_ = new UILabel();
    if (labelYAxisPointTwo_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelYAxisPointTwo_ fail");
        return;
    }
    labelYAxisPointTwo_->SetText("100");
    labelYAxisPointTwo_->SetPosition(HEART_LABLE_POSION_X, HEART_LABLE2_POSION_Y, TOP_TEXT_WIDTH, TOP_TEXT_HEIGHT);
    labelYAxisPointTwo_->SetAlign(TEXT_ALIGNMENT_RIGHT);
    labelYAxisPointTwo_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, HEART_RATE_X_FONT_SIZE);
    Add(labelYAxisPointTwo_);

    labelYAxisPointThree_ = new UILabel();
    if (labelYAxisPointThree_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelYAxisPointThree_ fail");
        return;
    }
    labelYAxisPointThree_->SetText("150");
    labelYAxisPointThree_->SetPosition(HEART_LABLE_POSION_X, HEART_LABLE3_POSION_Y, TOP_TEXT_WIDTH, TOP_TEXT_HEIGHT);
    labelYAxisPointThree_->SetAlign(TEXT_ALIGNMENT_RIGHT);
    labelYAxisPointThree_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, HEART_RATE_X_FONT_SIZE);
    Add(labelYAxisPointThree_);

    labelYAxisPointFour_ = new UILabel();
    if (labelYAxisPointFour_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelYAxisPointFour_ fail");
        return;
    }
    labelYAxisPointFour_->SetText("200");
    labelYAxisPointFour_->SetPosition(HEART_LABLE_POSION_X, HEART_LABLE4_POSION_Y, TOP_TEXT_WIDTH, TOP_TEXT_HEIGHT);
    labelYAxisPointFour_->SetAlign(TEXT_ALIGNMENT_RIGHT);
    labelYAxisPointFour_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, HEART_RATE_X_FONT_SIZE);
    Add(labelYAxisPointFour_);

    labelMaxValue_ = new UILabel();
    if (labelMaxValue_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelMaxValue_ fail");
        return;
    }
    
    // 打桩数据，后续需要根据测量结果修改最大值显示
    labelMaxValue_->SetText("148");
    labelMaxValue_->SetPosition(TOP_TEXT_X, TOP_TEXT_Y, TOP_TEXT_WIDTH, TOP_TEXT_HEIGHT);
    labelMaxValue_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, HEART_RATE_LIMIT_FONT_SIZE);
    Add(labelMaxValue_);

    imageRedArrow_ = new UIImageView();
    if (imageRedArrow_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new imageRedArrow_ fail");
        return;
    }
    imageRedArrow_->SetPosition(IMAGE_RED_TEXT_X, IMAGE_RED_TEXT_Y);
    imageRedArrow_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    LOADIMG::LoadImageViewImg(imageRedArrow_, HEART_RATE_IMAGE, IMAGE_HEART_RATE_UP);
    Add(imageRedArrow_);
    labelMinValue_ = new UILabel();
    if (labelMinValue_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelMinValue_ fail");
        return;
    }

    // 打桩数据，后续需要根据测量结果修改最小值显示
    labelMinValue_->SetText("59");
    labelMinValue_->SetPosition(BOTTOM_TEXT_X, BOTTOM_TEXT_Y, BOTTOM_TEXT_WIDTH, BOTTOM_TEXT_HEIGHT);
    labelMinValue_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, HEART_RATE_LIMIT_FONT_SIZE);
    Add(labelMinValue_);

    imageBlackArrow_ = new UIImageView();
    if (imageBlackArrow_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new imageBlackArrow_ fail");
        return;
    }
    imageBlackArrow_->SetPosition(IMAGE_BACK_TEXT_X, IMAGE_BACK_TEXT_Y);
    imageBlackArrow_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    LOADIMG::LoadImageViewImg(imageBlackArrow_, HEART_RATE_IMAGE, IMAGE_HEART_RATE_DOWN);
    Add(imageBlackArrow_);

    labelLastMeasureTime_ = new UILabel();
    if (labelLastMeasureTime_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelLastMeasureTime_ fail");
        return;
    }
    labelLastMeasureTime_->SetText("刚刚");
    labelLastMeasureTime_->SetPosition(BOTTOM_TEXT_ONE_X, BOTTOM_TEXT_ONE_Y, BOTTOM_TEXT_WIDTH_ONE,
                                       BOTTOM_TEXT_HEIGHT_ONE);
    labelLastMeasureTime_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, HEART_RATE_LIMIT_FONT_SIZE_ONE);
    labelLastMeasureTime_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelLastMeasureTime_->SetStyle(STYLE_TEXT_COLOR, 0xFFCCCCCC);
    labelLastMeasureTime_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    Add(labelLastMeasureTime_);

    SetDraggable(true);
    SetTouchable(true);
    InitImageAnimator();
}

HeartRateResultGroup::~HeartRateResultGroup()
{
    RemoveAll();

    if (labelTitle_ != nullptr) {
        delete labelTitle_;
        labelTitle_ = nullptr;
    }
    if (labelHeartRateValue_ != nullptr) {
        delete labelHeartRateValue_;
        labelHeartRateValue_ = nullptr;
    }
    if (chartPolyline_ != nullptr) {
        delete chartPolyline_;
        chartPolyline_ = nullptr;
    }
    if (labelLastMeasureTime_ != nullptr) {
        delete labelLastMeasureTime_;
        labelLastMeasureTime_ = nullptr;
    }
    if (labelMaxValue_ != nullptr) {
        delete labelMaxValue_;
        labelMaxValue_ = nullptr;
    }
    if (labelMinValue_ != nullptr) {
        delete labelMinValue_;
        labelMinValue_ = nullptr;
    }
    if (labelUnitText_ != nullptr) {
        delete labelUnitText_;
        labelUnitText_ = nullptr;
    }
    if (imageAnimator_ != nullptr) {
        delete imageAnimator_;
        imageAnimator_ = nullptr;
    }
    if (dataSerial_ != nullptr) {
        delete dataSerial_;
        dataSerial_ = nullptr;
    }
    if (imageRedArrow_ != nullptr) {
        delete imageRedArrow_;
        imageRedArrow_ = nullptr;
    }
    if (imageBlackArrow_ != nullptr) {
        delete imageBlackArrow_;
        imageBlackArrow_ = nullptr;
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
    if (labelYAxisPointOne_ != nullptr) {
        delete labelYAxisPointOne_;
        labelYAxisPointOne_ = nullptr;
    }
    if (labelYAxisPointTwo_ != nullptr) {
        delete labelYAxisPointTwo_;
        labelYAxisPointTwo_ = nullptr;
    }
    if (labelYAxisPointThree_ != nullptr) {
        delete labelYAxisPointThree_;
        labelYAxisPointThree_ = nullptr;
    }
    if (labelYAxisPointFour_ != nullptr) {
        delete labelYAxisPointFour_;
        labelYAxisPointFour_ = nullptr;
    }
    if (mainImg_ != nullptr) {
        delete mainImg_;
        mainImg_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(HEART_RATE_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HeartRateResultGroup::~HeartRateResultGroup");
}

void HeartRateResultGroup::InitImageAnimator()
{
    imageAnimator_ = new UIImageAnimatorView();
    if (imageAnimator_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new imageAnimator_ fail");
        return;
    }
    for (uint8_t i = 0; i < HEARTRATE_ANIMATOR_IMAGE_NUM; i++) {
        imageAnimatorImageInfo_[i].imageInfo =
            ImageCacheManager::GetInstance().LoadOneInMultiRes(g_imageAnimatorInfo[i], HEART_RATE_IMAGE);
        imageAnimatorImageInfo_[i].imageType = IMG_SRC_IMAGE_INFO;
        imageAnimatorImageInfo_[i].pos.x = ANIMATOR_IMAGE_X;
        imageAnimatorImageInfo_[i].pos.y = ANIMATOR_IMAGE_Y;
        imageAnimatorImageInfo_[i].width = ANIMATOR_IMAGE_WIDTH;
        imageAnimatorImageInfo_[i].height = ANIMATOR_IMAGE_HEIGHT;
    }

    imageAnimator_->SetPosition(PERSEC_HEART_TEXT_X, PERSEC_HEART_TEXT_Y);
    imageAnimator_->SetImageAnimatorSrc(imageAnimatorImageInfo_, HEARTRATE_ANIMATOR_IMAGE_NUM,
                                        HEARTRATE_ANIMATOR_IMAGE_UPDATE);
    imageAnimator_->SetRepeat(true);
    imageAnimator_->SetAutoEnable(false);
    imageAnimator_->SetResizeMode(UIImageView::FILL);
    imageAnimator_->Start();
    imageAnimator_->LayoutCenterOfParent();
    imageAnimator_->SetDraggable(true);
    Add(imageAnimator_);
}
}