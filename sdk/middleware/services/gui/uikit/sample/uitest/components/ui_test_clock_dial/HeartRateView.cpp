/*
 * Copyright (c) 2022 CompanyNameMagicTag.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <securec.h>
#include "common/screen.h"
#include "UiConfig.h"
#include "ui_resource_image.h"
#include "common/image_cache_manager.h"
#include "HeartRateView.h"

namespace OHOS {
static HeartRateView *g_pHeartRateView = nullptr;

// 背景坐标图形
static constexpr uint16_t ACT_IMAGE_X = 0;                               // 图片坐标起点
static constexpr uint16_t ACT_IMAGE_Y = 115;                             // 图片坐标起点
static constexpr uint16_t ACT_IMAGE_COOR_X = 45;                         // 200刻度线到最左端的像素点
static constexpr uint16_t ACT_IMAGE_COOR_Y = 145;                        // 200刻度线到顶端的像素点
static constexpr uint16_t ACT_IMAGE_COOR_PIXELS = 160;                   // 实际背景坐标0到200心率值的实际像素点
static constexpr uint16_t ACT_IMAGE_COOR_VALUE = HEART_RATE_LIMIT_UPPER; // 图片刻度表示的心率范围
static constexpr uint16_t ACT_IMAGE_COOR_WIDTH = 454;
static constexpr uint16_t ACT_IMAGE_COOR_HEIGHT = 220;

// 心电图坐标适配
static constexpr uint16_t COORDINATE_VALUE = 5; // 余量
static constexpr uint16_t COORDINATE_POINT_X = (ACT_IMAGE_COOR_X - COORDINATE_VALUE);
static constexpr uint16_t COORDINATE_POINT_Y = (ACT_IMAGE_COOR_Y - COORDINATE_VALUE * 2);
static constexpr uint16_t COORDINATE_HEIGHT = (ACT_IMAGE_COOR_PIXELS + COORDINATE_VALUE * 2);
static constexpr uint16_t COORDINATE_WIDTH = (HEART_AVE_NUM + COORDINATE_VALUE * 2); // 360点，每点对应240秒（4分钟）
static constexpr uint16_t COORDINATE_GRAD_MIN_OPA = 25;
static constexpr uint16_t COORDINATE_GRAD_MAX_OPA = 180;

// 心形心跳
static constexpr uint16_t HEARTBEAT_IMAGE_WIDTH = 92;
static constexpr uint16_t HEARTBEAT_IMAGE_HEIGHT = 92;
static constexpr uint16_t HEARTBEAT_IMAGE_X = 130;
static constexpr uint16_t HEARTBEAT_IMAGE_Y = 25;
static constexpr uint16_t HEARTBEAT_IMAGE_NUM = 2;
static constexpr uint16_t HEARTBEAT_DELAY = 50;

// 最高心率
static constexpr uint16_t TOP_IMAGE_WIDTH = 32;
static constexpr uint16_t TOP_IMAGE_HEIGHT = 32;
static constexpr uint16_t TOP_IMAGE_X = 120;
static constexpr uint16_t TOP_IMAGE_Y = 340;
static constexpr uint16_t TOP_STROKE_WIDTH = 1;
static constexpr uint16_t TOP_RADIUS = 4;

static constexpr uint16_t HEART_RATE_LIMIT_FONT_SIZE = 30;
static constexpr uint16_t TOP_TEXT_WIDTH = 80;
static constexpr uint16_t TOP_TEXT_HEIGHT = (HEART_RATE_LIMIT_FONT_SIZE + 5);
static constexpr uint16_t TOP_TEXT_X = 160;
static constexpr uint16_t TOP_TEXT_Y = 337;

// 最低心率
static constexpr uint16_t BOTTOM_IMAGE_WIDTH = 32;
static constexpr uint16_t BOTTOM_IMAGE_HEIGHT = 32;
static constexpr uint16_t BOTTOM_IMAGE_X = 250;
static constexpr uint16_t BOTTOM_IMAGE_Y = 340;
static constexpr uint16_t BOTTOM_STROKE_WIDTH = 1;
static constexpr uint16_t BOTTOM_RADIUS = 3;

static constexpr uint16_t BOTTOM_TEXT_WIDTH = 80;
static constexpr uint16_t BOTTOM_TEXT_HEIGHT = (HEART_RATE_LIMIT_FONT_SIZE + 5);
static constexpr uint16_t BOTTOM_TEXT_X = 290;
static constexpr uint16_t BOTTOM_TEXT_Y = 337;

// 静息心率
static constexpr uint16_t REST_HEART_RATE_FONT_SIZE = 40;
static constexpr uint16_t REST_HEART_RATE_X = 195;
static constexpr uint16_t REST_HEART_RATE_Y = 380;
static constexpr uint16_t REST_HEART_RATE_WIDTH = 95;
static constexpr uint16_t REST_HEART_RATE_HEIGHT = (REST_HEART_RATE_FONT_SIZE + 5);

static constexpr uint16_t REST_HEART_TEXT_FONT_SIZE = 30;
static constexpr uint16_t REST_HEART_TEXT_X1 = 120;
static constexpr uint16_t REST_HEART_TEXT_Y1 = 390;
static constexpr uint16_t REST_HEART_TEXT_WIDTH1 = 75;
static constexpr uint16_t REST_HEART_TEXT_HEIGHT1 = (REST_HEART_TEXT_FONT_SIZE + 5);

static constexpr uint16_t REST_HEART_TEXT_X2 = 265;
static constexpr uint16_t REST_HEART_TEXT_Y2 = 390;
static constexpr uint16_t REST_HEART_TEXT_WIDTH2 = 75;
static constexpr uint16_t REST_HEART_TEXT_HEIGHT2 = (REST_HEART_TEXT_FONT_SIZE + 5);

// 每秒心率
static constexpr uint16_t PERSEC_HEART_RATE_FONT_SIZE = 45;
static constexpr uint16_t PERSEC_HEART_RATE_X = 225;
static constexpr uint16_t PERSEC_HEART_RATE_Y = 55;
static constexpr uint16_t PERSEC_HEART_RATE_WIDTH = 90;
static constexpr uint16_t PERSEC_HEART_RATE_HEIGHT = (PERSEC_HEART_RATE_FONT_SIZE + 5);

static constexpr uint16_t PERSEC_HEART_TEXT_FONT_SIZE = 30;
static constexpr uint16_t PERSEC_HEART_TEXT_X = 310;
static constexpr uint16_t PERSEC_HEART_TEXT_Y = 75;
static constexpr uint16_t PERSEC_HEART_TEXT_WIDTH = 75;
static constexpr uint16_t PERSEC_HEART_TEXT_HEIGHT = (PERSEC_HEART_TEXT_FONT_SIZE + 5);

// 心跳image
static ImageAnimatorInfo g_heartBeatImageInfo[HEARTBEAT_IMAGE_NUM] = {
    {PNG_A004_070_IMAGE_PATH, {HEARTBEAT_IMAGE_X, HEARTBEAT_IMAGE_Y}, HEARTBEAT_IMAGE_WIDTH, HEARTBEAT_IMAGE_HEIGHT,
        IMG_SRC_FILE_PATH},
    {PNG_A004_071_IMAGE_PATH, {HEARTBEAT_IMAGE_X, HEARTBEAT_IMAGE_Y}, HEARTBEAT_IMAGE_WIDTH, HEARTBEAT_IMAGE_HEIGHT,
        IMG_SRC_FILE_PATH},
};

HeartRateView::HeartRateView() : dataNume(0), currIndex(0), serialIndex(0), maxRate(0), minRate(HEART_RATE_LIMIT_UPPER)
{
    GRAPHIC_LOGD("HeartRateView::HeartRateView\n");
    (void)memset_s(rateDataTmp, sizeof(Point) * HEART_AVE_NUM, INVALID_HEART_VALUE, sizeof(Point) * HEART_AVE_NUM);
    InitHeartRateData();
    g_pHeartRateView = this;
}

HeartRateView::~HeartRateView()
{
    GRAPHIC_LOGD("HeartRateView::~HeartRateView\n");
    if (initViewStatus) {
        int32_t i;
        RemoveAll();
        delete imageViewCoordinate;
        imageViewCoordinate = nullptr;

        chart->ClearDataSerial();
        for (i = 0; i < HEART_AVE_NUM; i++) {
            delete dataSerial[i];
            dataSerial[i] = nullptr;
        }
        delete chart;
        chart = nullptr;
        delete imageViewTop;
        imageViewTop = nullptr;
        delete imageViewBottom;
        imageViewBottom = nullptr;
        delete dataTopAndbottom;
        dataTopAndbottom = nullptr;
        imageAnimator->Stop();
        delete imageAnimator;
        imageAnimator = nullptr;
    }
    g_pHeartRateView = nullptr;
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(HEART_IAMGES);
}

HeartRateView *HeartRateView::GetInstance(void)
{
    return g_pHeartRateView;
}

int32_t HeartRateView::InitHeartRateData(void) // 初始化心率数据，将存放心率数据的有效字节设置为无效值
{
    int32_t ret;

    heartRateData[0].x = INVALID_HEART_VALUE;
    heartRateData[0].y = INVALID_HEART_VALUE;

    ret = memset_s(heartRateData, sizeof(Point) * HEART_AVE_NUM, INVALID_HEART_VALUE, sizeof(Point) * HEART_AVE_NUM);
    if (ret != 0) {
        GRAPHIC_LOGE("HeartRateView::[InitHeartRateData] memset_s heartRateData fail！ ret = %d\n", ret);
    }
    return ret;
}

int32_t HeartRateView::SetHeartRateData(Point *rateData, uint32_t rateDataSize)
{
    int ret;

    if (rateData == nullptr) {
        return false;
    }

    if (rateDataSize > sizeof(Point) * HEART_AVE_NUM) {
        rateDataSize = sizeof(Point) * HEART_AVE_NUM;
    }
    InitHeartRateData();
    ret = memcpy_s(heartRateData, sizeof(Point) * HEART_AVE_NUM, rateData, rateDataSize);
    if (ret != EOK) {
        GRAPHIC_LOGE("HeartRateView::[HeartRateDataProc] memcpy_s heartRateData fail！ ret = %d\n", ret);
        return false;
    }

    return true;
}


int32_t HeartRateView::HeartRateDataProc(void)
{
    int32_t i = 0;
    int32_t ret = 0;
    Point datain[HEART_AVE_NUM];

    ret = memcpy_s(datain, sizeof(Point) * HEART_AVE_NUM, heartRateData, sizeof(Point) * HEART_AVE_NUM);
    if (ret != EOK) {
        GRAPHIC_LOGE("HeartRateView::[HeartRateDataProc] memcpy_s rateDataTmp fail！ ret = %d\n", ret);
        return false;
    }

    ret = memset_s(rateDataTmp, sizeof(Point) * HEART_AVE_NUM, INVALID_HEART_VALUE, sizeof(Point) * HEART_AVE_NUM);
    if (ret != EOK) {
        GRAPHIC_LOGE("HeartRateView::[HeartRateDataProc] memset_s rateDataTmp fail！ ret = %d\n", ret);
        return false;
    }

    dataNume = 0;
    for (i = 0; i < HEART_AVE_NUM; i++) {
        if (datain[i].x >= 0 && datain[i].x < HEART_AVE_NUM && datain[i].y > 0 &&
            datain[i].y <= HEART_RATE_LIMIT_UPPER) {
            rateDataTmp[dataNume].x = datain[i].x + COORDINATE_VALUE;
            rateDataTmp[dataNume].y = datain[i].y * ACT_IMAGE_COOR_PIXELS / ACT_IMAGE_COOR_VALUE;
            dataNume++;
        }
    }
    return true;
}

void HeartRateView::SetLabel(UILabel *label, LabelInfo labelInfo)
{
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, labelInfo.fontSize);
    label->SetPosition(labelInfo.x, labelInfo.y);
    label->Resize(labelInfo.width, labelInfo.height);
    label->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
    Add(label);
}

bool HeartRateView::LoadAppImages(std::string file, UIImageView *view, uint32_t resId)
{
    ImageInfo* image = ImageCacheManager::GetInstance().LoadOneInMultiRes(resId, file);
    if (image == nullptr) {
        GRAPHIC_LOGE("LoadAppImages %x fail\n", resId);
        return false;
    }
    view->SetSrc(image);
    return true;
}

void HeartRateView::HeartRateLabelDisplay(void)
{
    LabelInfo perSecLabel = { PERSEC_HEART_RATE_FONT_SIZE, PERSEC_HEART_RATE_X, PERSEC_HEART_RATE_Y,
                              PERSEC_HEART_RATE_WIDTH, PERSEC_HEART_RATE_HEIGHT };
    LabelInfo perSecTextLabel = { PERSEC_HEART_TEXT_FONT_SIZE, PERSEC_HEART_TEXT_X, PERSEC_HEART_TEXT_Y,
                                  PERSEC_HEART_TEXT_WIDTH, PERSEC_HEART_TEXT_HEIGHT };

    LabelInfo topLabel = { HEART_RATE_LIMIT_FONT_SIZE, TOP_TEXT_X, TOP_TEXT_Y, TOP_TEXT_WIDTH, TOP_TEXT_HEIGHT };
    LabelInfo bottomLabel = { HEART_RATE_LIMIT_FONT_SIZE, BOTTOM_TEXT_X, BOTTOM_TEXT_Y, BOTTOM_TEXT_WIDTH,
                              BOTTOM_TEXT_HEIGHT };

    LabelInfo restRateLabel = { REST_HEART_RATE_FONT_SIZE, REST_HEART_RATE_X, REST_HEART_RATE_Y, REST_HEART_RATE_WIDTH,
                                REST_HEART_RATE_HEIGHT };
    LabelInfo restRateTextLabel1 = { REST_HEART_TEXT_FONT_SIZE, REST_HEART_TEXT_X1, REST_HEART_TEXT_Y1,
                                     REST_HEART_TEXT_WIDTH1, REST_HEART_TEXT_HEIGHT1 };
    LabelInfo restRateTextLabel2 = { REST_HEART_TEXT_FONT_SIZE, REST_HEART_TEXT_X2, REST_HEART_TEXT_Y2,
                                     REST_HEART_TEXT_WIDTH2, REST_HEART_TEXT_HEIGHT2 };

    topRate = new UILabel();
    bottomRate = new UILabel();
    ratePerSec = new UILabel();
    ratePerSecText = new UILabel();
    restRate = new UILabel();
    restRateText1 = new UILabel();
    restRateText2 = new UILabel();

    if (topRate == nullptr || bottomRate == nullptr || ratePerSec == nullptr || ratePerSecText == nullptr ||
        restRate == nullptr || restRateText1 == nullptr || restRateText2 == nullptr) {
        GRAPHIC_LOGE("HeartRateView::[HeartRateLabelDisplay] new UILabel fail\n");
    }

    // 实时心率
    SetLabel(ratePerSec, perSecLabel);
    SetLabel(ratePerSecText, perSecTextLabel);
    ratePerSec->SetText("--");
    ratePerSecText->SetText("次/分");

    // 最高最低心率
    SetLabel(topRate, topLabel);
    SetLabel(bottomRate, bottomLabel);
    topRate->SetText("--");
    bottomRate->SetText("--");

    // 静息心率
    SetLabel(restRate, restRateLabel);
    SetLabel(restRateText1, restRateTextLabel1);
    SetLabel(restRateText2, restRateTextLabel2);
    restRateText1->SetText("静息");
    restRate->SetText("--");
    restRateText2->SetText("次/分");
}

void HeartRateView::HeartRateCoordinate(void)
{
    GRAPHIC_LOGD("HeartRateView::HeartRateCoordinate start\n");
    // 坐标图片
    imageViewCoordinate = new UIImageView();
    if (imageViewCoordinate == nullptr) {
        GRAPHIC_LOGE("HeartRateView::[HeartRateCoordinate] new imageViewCoordinate fail\n");
        return;
    }
    imageViewCoordinate->SetPosition(ACT_IMAGE_X, ACT_IMAGE_Y);
    imageViewCoordinate->SetWidth(ACT_IMAGE_COOR_WIDTH);
    imageViewCoordinate->SetHeight(ACT_IMAGE_COOR_HEIGHT);
    LoadAppImages(HEART_IAMGES, imageViewCoordinate, HEART_HEARTRATECOOR);
    Add(imageViewCoordinate);

    //  心电图坐标适配
    chart = new UIChartPolyline();
    if (chart == nullptr) {
        GRAPHIC_LOGE("HeartRateView::[HeartRateCoordinate] new chart fail\n");
        return;
    }
    chart->SetPosition(COORDINATE_POINT_X, COORDINATE_POINT_Y);
    chart->SetWidth(COORDINATE_WIDTH);
    chart->SetHeight(COORDINATE_HEIGHT);
    chart->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    chart->SetStyle(STYLE_BACKGROUND_OPA, 0);
    chart->SetStyle(STYLE_LINE_WIDTH, 1); // line width

    UIXAxis &xAxis = chart->GetXAxis();
    UIYAxis &yAxis = chart->GetYAxis();
    xAxis.SetDataRange(0, COORDINATE_WIDTH); // 24小时对应的秒数
    yAxis.SetDataRange(0, COORDINATE_HEIGHT);
    xAxis.SetLineColor(Color::GetColorFromRGBA(0, 0, 0, 0));
    yAxis.SetLineColor(Color::GetColorFromRGBA(0, 0, 0, 0));
    chart->SetGradientOpacity(COORDINATE_GRAD_MIN_OPA, COORDINATE_GRAD_MAX_OPA);
    Add(chart);
}

void HeartRateView::InsertTopAndBottomRateImage(void)
{
    GRAPHIC_LOGD("HeartRateView::InsertTopAndBottomRateImage start\n");
    imageViewTop = new UIImageView();
    imageViewBottom = new UIImageView();
    if (imageViewTop == nullptr || imageViewBottom == nullptr) {
        GRAPHIC_LOGE("HeartRateView::[InsertTopAndBottomRateImage] new imageViewTop or imageViewBottom fail\n");
        return;
    }
    imageViewTop->SetPosition(TOP_IMAGE_X, TOP_IMAGE_Y);
    imageViewTop->SetWidth(TOP_IMAGE_WIDTH);
    imageViewTop->SetHeight(TOP_IMAGE_HEIGHT);
    LoadAppImages(HEART_IAMGES, imageViewTop, HEART_HEARTRATE_TOP);
    Add(imageViewTop);
    imageViewBottom->SetPosition(BOTTOM_IMAGE_X, BOTTOM_IMAGE_Y);
    imageViewBottom->SetWidth(BOTTOM_IMAGE_WIDTH);
    imageViewBottom->SetHeight(BOTTOM_IMAGE_HEIGHT);
    LoadAppImages(HEART_IAMGES, imageViewBottom, HEART_HEARTRATE_BOTTOM);
    Add(imageViewBottom);
}

void HeartRateView::UpdataTopAndBottomRate(void)
{
    uint16_t bottom = 0;
    uint16_t top = 0;
    if (chart == nullptr) {
        return;
    }
    if (dataTopAndbottom != nullptr) {
        chart->DeleteDataSerial(dataTopAndbottom);
        delete dataTopAndbottom;
        dataTopAndbottom = nullptr;
    }
    dataTopAndbottom = new UIChartDataSerial();
    if (dataTopAndbottom == nullptr) {
        return;
    }
    if (dataNume != 1) {
        dataTopAndbottom->SetMaxDataCount(dataNume);        // number of data points
        dataTopAndbottom->AddPoints(rateDataTmp, dataNume); // number of data points
    } else {
        Point tmp[NUM_2];
        tmp[0].x = tmp[1].x = rateDataTmp[0].x;
        tmp[0].y = tmp[1].y = rateDataTmp[0].y;
        dataTopAndbottom->SetMaxDataCount(NUM_2); // number of data points
        dataTopAndbottom->AddPoints(tmp, NUM_2);  // number of data points
    }
    dataTopAndbottom->SetLineColor(Color::GetColorFromRGBA(0, 0, 0, 0)); // 设置透明
    chart->AddDataSerial(dataTopAndbottom);
    chart->Invalidate();

    UIChartDataSerial::PointStyle pointStyle;
    // 最低心跳
    dataTopAndbottom->EnableBottomPoint(true);
    pointStyle.radius = BOTTOM_RADIUS; // Inner radius
    pointStyle.fillColor = Color::Gray();
    pointStyle.strokeWidth = BOTTOM_STROKE_WIDTH; // border width
    pointStyle.strokeColor = Color::Black();
    dataTopAndbottom->SetBottomPointStyle(pointStyle);
    chart->Invalidate();

    bottom = dataTopAndbottom->GetValleyData() * ACT_IMAGE_COOR_VALUE / ACT_IMAGE_COOR_PIXELS;
    // 最高心跳
    dataTopAndbottom->EnableTopPoint(true);
    pointStyle.fillColor = Color::Red();
    pointStyle.radius = TOP_RADIUS; // Inner radius
    pointStyle.strokeColor = Color::Black();
    pointStyle.strokeWidth = TOP_STROKE_WIDTH; // border width
    dataTopAndbottom->SetTopPointStyle(pointStyle);
    chart->Invalidate();

    top = dataTopAndbottom->GetPeakData() * ACT_IMAGE_COOR_VALUE / ACT_IMAGE_COOR_PIXELS;
    if (top <= HEART_RATE_LIMIT_UPPER) {
        maxRate = (maxRate >= top) ? maxRate : top;
    }
    if (bottom != 0) {
        minRate = (minRate <= bottom) ? minRate : bottom;
    }
}

void HeartRateView::ElectrocardiogramOnePoint(Point *rateData, uint16_t index)
{
    Point tmp[NUM_2];

    if (rateData == nullptr || chart == nullptr || index >= HEART_AVE_NUM || serialIndex >= HEART_AVE_NUM) {
        return;
    }

    tmp[0].x = tmp[1].x = rateData[index].x;
    tmp[0].y = tmp[1].y = rateData[index].y;

    dataSerial[serialIndex] = new UIChartDataSerial();
    if (dataSerial[serialIndex] == nullptr) {
        GRAPHIC_LOGE("HeartRateView::[ElectrocardiogramOnePoint] new dataSerial[%u] fail\n", serialIndex);
        serialIndex++;
        return;
    }

    dataSerial[serialIndex]->SetMaxDataCount(HEART_AVE_NUM); // 设置最大显示的像素点
    dataSerial[serialIndex]->AddPoints(tmp, NUM_2);
    dataSerial[serialIndex]->SetLineColor(Color::Red());
    dataSerial[serialIndex]->SetFillColor(Color::Red());
    dataSerial[serialIndex]->EnableGradient(true);

    chart->AddDataSerial(dataSerial[serialIndex]);
    chart->Invalidate();
    serialIndex++;
}

void HeartRateView::ElectrocardiogramMultiPoint(Point *rateData, uint16_t rateDataNume)
{
    uint16_t i = 0;
    uint16_t j = 0;
    uint16_t tmp = 1;
    if (rateData == nullptr || chart == nullptr || rateDataNume > HEART_AVE_NUM) {
        return;
    }

    while (i < rateDataNume) {
        while ((j + 1 < rateDataNume) && (rateData[j + 1].x - rateData[j].x <= NUM_4)) { // 15分钟对应的秒数
            tmp++;
            j++;
        }
        if (tmp == 1) {
            ElectrocardiogramOnePoint(rateData, i);
        } else {
            if (serialIndex >= HEART_AVE_NUM) {
                return;
            }
            if (serialIndex > 0 && dataSerial[serialIndex - 1] != nullptr && i == 0) {
                dataSerial[serialIndex - 1]->AddPoints(rateData + 1, tmp - 1); // number of data points
                chart->Invalidate();
            } else {
                dataSerial[serialIndex] = new UIChartDataSerial();
                if (dataSerial[serialIndex] == nullptr) {
                    GRAPHIC_LOGE("HeartRateView::[ElectrocardiogramMultiPoint] new dataSerial[%u] fail\n", serialIndex);
                    serialIndex++;
                    return;
                }

                dataSerial[serialIndex]->SetMaxDataCount(HEART_AVE_NUM); // number of data points
                dataSerial[serialIndex]->AddPoints(rateData + i, tmp);   // number of data points
                dataSerial[serialIndex]->SetLineColor(Color::Red());
                dataSerial[serialIndex]->SetFillColor(Color::Red());
                dataSerial[serialIndex]->EnableGradient(true);

                chart->AddDataSerial(dataSerial[serialIndex]);
                chart->Invalidate();
                serialIndex++;
            }
        }
        i += tmp;
        j = i;
        tmp = 1;
    }
}

void HeartRateView::HeartRateElectrocardiogram(void)
{
    if (dataNume == 0) {
        return;
    }
    serialIndex = 0;
    if (dataNume != 1) {
        ElectrocardiogramMultiPoint(rateDataTmp, dataNume); // 多个点
    } else {
        ElectrocardiogramOnePoint(rateDataTmp, 0); // 一个点
    }
    currIndex = dataNume;
}

void HeartRateView::HeartbeatImageAnimator()
{
    imageAnimator = new UIImageAnimatorView();
    if (imageAnimator == nullptr) {
        GRAPHIC_LOGE("HeartRateView::[HeartbeatImageAnimator] new imageAnimator fail\n");
        return;
    }
    imageAnimator->SetPosition(HEARTBEAT_IMAGE_X, HEARTBEAT_IMAGE_Y, HEARTBEAT_IMAGE_WIDTH, HEARTBEAT_IMAGE_HEIGHT);
    imageAnimator->SetImageAnimatorSrc(g_heartBeatImageInfo, HEARTBEAT_IMAGE_NUM, HEARTBEAT_DELAY);
    imageAnimator->SetRepeat(true);
    Add(imageAnimator);
    imageAnimator->Start();
}

void HeartRateView::HeartbeatImageAnimatorStart(void)
{
    if (imageAnimator != nullptr) {
        imageAnimator->Start();
    }
}

void HeartRateView::HeartbeatImageAnimatorStop(void)
{
    if (imageAnimator != nullptr) {
        imageAnimator->Stop();
    }
}

bool HeartRateView::InitView()
{
    HeartRateDataProc();
    HeartRateLabelDisplay();
    HeartbeatImageAnimator();
    HeartRateCoordinate();
    InsertTopAndBottomRateImage();
    HeartRateElectrocardiogram();
    UpdataTopAndBottomRate();
    HeartbeatImageAnimatorStop();
    InitRefreshInterface();
    initViewStatus = true;
    return true;
}

void HeartRateView::InitRefreshInterface(void)
{
    GRAPHIC_LOGD("HeartRateView::HeartRateView::InitRefreshInterface\n");
    static Point rateData[5] = {{1, 1}, {5, 5}, {6, 6}, {9, 9}, {3, 3}}; // 3, 5, 6, 9: raw data for polyline
    SetHeartRateData(rateData, sizeof(rateData));
    uint16_t maxRateLocal = 50; // 50 heart rate up bound
    uint16_t minRateLocal = 10; // 10 heart rate low bound
    RefreshElectrocardiogram();
    if (maxRateLocal > 0 && maxRateLocal < HEART_RATE_LIMIT_UPPER) {
        RefreshHeartRateMax(maxRateLocal);
    }
    if (minRateLocal > 0 && minRateLocal < HEART_RATE_LIMIT_UPPER) {
        RefreshHeartRateMin(minRateLocal);
    }
}

// 刷新心率曲线
void HeartRateView::RefreshElectrocardiogram(void)
{
    int32_t ret;
    uint16_t tmp;
    uint16_t count;
    Point *rate = nullptr;
    Point modPoint;

    ret = HeartRateDataProc();
    if (ret != true) {
        GRAPHIC_LOGE("HeartRateView::[RefreshElectrocardiogram()] fail\n");
        return;
    }
    rate = rateDataTmp;

    if (chart == nullptr) {
        return;
    }

    if (currIndex >= HEART_AVE_NUM) {
        currIndex = HEART_AVE_NUM;
    }
    if (dataNume > currIndex) {
        tmp = dataNume - currIndex;
        if (currIndex > 0) {
            rate += (currIndex - 1);
            tmp += 1;
        }
        ElectrocardiogramMultiPoint(rate, tmp);
        currIndex = dataNume;
    } else if (dataNume == currIndex) { // 当前点平均心率刷新
        if (serialIndex > 0 && serialIndex <= HEART_AVE_NUM && dataSerial[serialIndex - 1] != nullptr) {
            modPoint.x = rate[currIndex - 1].x;
            modPoint.y = rate[currIndex - 1].y;

            count = dataSerial[serialIndex - 1]->GetDataCount();
            dataSerial[serialIndex - 1]->ModifyPoint(count - 1, modPoint);
            chart->Invalidate();
        }
    } else {
        return;
    }
    UpdataTopAndBottomRate(); // 最高、最低心率
}

// 清空心率曲线
void HeartRateView::ClearElectrocardiogram(void)
{
    int32_t i;
    dataNume = 0;
    currIndex = 0;
    maxRate = 0;
    minRate = HEART_RATE_LIMIT_UPPER;
    InitHeartRateData();

    if (chart == nullptr) {
        GRAPHIC_LOGE("chart == nullptr\n");
        return;
    }

    if (ratePerSec != nullptr && topRate != nullptr && bottomRate != nullptr && restRate != nullptr) {
        ratePerSec->SetText("--");
        topRate->SetText("--");
        bottomRate->SetText("--");
        restRate->SetText("--");
    }

    chart->DeleteDataSerial(dataTopAndbottom);
    delete dataTopAndbottom;
    dataTopAndbottom = nullptr;
    for (i = 0; i < serialIndex; i++) {
        delete dataSerial[i];
        dataSerial[i] = nullptr;
    }
    chart->ClearDataSerial();

    serialIndex = 0;
    chart->Invalidate();
}

// 刷新每秒上报的心率
void HeartRateView::RefreshHeartRatePerSec(uint16_t value)
{
    char heartRate[NUM_4] = "--";
    int32_t ret = 0;

    if (ratePerSec == nullptr) {
        return;
    }

    if (value == 0) {
        ratePerSec->SetText("--");
        HeartbeatImageAnimatorStop();
        return;
    }
    HeartbeatImageAnimatorStart();
    if (value > NUM_99) {
        ret = sprintf_s(heartRate, sizeof(heartRate), "%u", value);
    } else {
        ret = sprintf_s(heartRate, sizeof(heartRate), " %u", value);
    }
    if (ret > EOK) {
        ratePerSec->SetText(heartRate);
    }
}

// 刷新最大心率
void HeartRateView::RefreshHeartRateMax(uint16_t max)
{
    char top[NUM_4] = "--";
    int32_t ret = 0;
    maxRate = (maxRate >= max) ? maxRate : max;
    if (topRate == nullptr) {
        return;
    }
    if (maxRate <= HEART_RATE_LIMIT_LOWER) {
        topRate->SetText("--");
        return;
    }
    ret = sprintf_s(top, sizeof(top), "%u", maxRate);
    if (ret > EOK) {
        topRate->SetText(top);
    }
}

// 刷新最小心率
void HeartRateView::RefreshHeartRateMin(uint16_t min)
{
    char bottom[NUM_4] = "--";
    int32_t ret = 0;

    minRate = (minRate <= min) ? minRate : min;
    if (bottomRate == nullptr) {
        return;
    }
    if (minRate >= HEART_RATE_LIMIT_UPPER) {
        bottomRate->SetText("--");
        return;
    }
    ret = sprintf_s(bottom, sizeof(bottom), "%u", minRate);
    if (ret > EOK) {
        bottomRate->SetText(bottom);
    }
}

// 刷新静息心率
void HeartRateView::RefreshRestHeartRate(uint16_t value)
{
    char heartRate[NUM_4] = "--";
    int32_t ret = 0;

    if (restRate == nullptr) {
        return;
    }

    if (value == 0 || value == static_cast<uint16_t>(0xFFFF)) { // 无效值
        restRate->SetText("--");
    } else {
        if (value > NUM_99) {
            ret = sprintf_s(heartRate, sizeof(heartRate), "%u", value);
        } else {
            ret = sprintf_s(heartRate, sizeof(heartRate), " %u", value);
        }

        if (ret > EOK) {
            restRate->SetText(heartRate);
        }
    }
}
} // namespace OHOS
