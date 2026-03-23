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

#ifndef HEART_RATE_VIEW_H
#define HEART_RATE_VIEW_H

#include "components/ui_image_animator.h"
#include "components/ui_chart.h"
#include "AppGroupView.h"

namespace OHOS {
constexpr uint16_t HEART_AVE_NUM = 360;
constexpr uint16_t HEART_RATE_LIMIT_UPPER = 200;
constexpr uint16_t HEART_RATE_LIMIT_LOWER = 0;
constexpr uint16_t INVALID_HEART_VALUE = static_cast<uint16_t>(0xFFFF);

constexpr uint16_t NUM_1 = 1;
constexpr uint16_t NUM_2 = 2;
constexpr uint16_t NUM_3 = 3;
constexpr uint16_t NUM_4 = 4;
constexpr uint16_t NUM_5 = 5;
constexpr uint16_t NUM_7 = 7;
constexpr uint16_t NUM_16 = 16;
constexpr uint16_t NUM_99 = 99;

class HeartRateView : public AppGroupView {
public:
    HeartRateView();
    HeartRateView(const HeartRateView &) = delete;
    HeartRateView operator=(const HeartRateView &) = delete;
    ~HeartRateView() override;
    static HeartRateView *GetInstance(void);

    bool InitView() override;
    int32_t SetHeartRateData(Point *rateData, uint32_t rateDataSize); // 设置心率数据，用于画心率曲线
    void RefreshElectrocardiogram(void); // 刷新心率曲线
    void ClearElectrocardiogram(void); // 清空心率曲线
    void RefreshHeartRatePerSec(uint16_t value); // 刷新每秒心率
    void RefreshHeartRateMax(uint16_t max); // 刷新心率最大值
    void RefreshHeartRateMin(uint16_t min); // 刷新心率最小值
    void RefreshRestHeartRate(uint16_t value); // 刷新静息心率
    void HeartbeatImageAnimatorStart(void); // 心形动画启动
    void HeartbeatImageAnimatorStop(void); // 心形动画停止
    void InitRefreshInterface(void);

private:
    struct LabelInfo {
        uint8_t fontSize;
        int16_t x;
        int16_t y;
        int16_t width;
        int16_t height;
    };

    int32_t InitHeartRateData(void);
    void SetLabel(UILabel *label, LabelInfo labelInfo);
    void HeartRateLabelDisplay(void);
    void HeartRateCoordinate(void);
    void HeartRateElectrocardiogram(void);
    void InsertTopAndBottomRateImage(void);
    void UpdataTopAndBottomRate(void);
    void ElectrocardiogramOnePoint(Point *rateData, uint16_t index);
    void ElectrocardiogramMultiPoint(Point *rateData, uint16_t rateDataNume);
    int32_t HeartRateDataProc(void);
    void HeartbeatImageAnimator(void);

private:
    Point heartRateData[HEART_AVE_NUM];
    Point rateDataTmp[HEART_AVE_NUM];

    UIChartPolyline *chart{nullptr};
    uint16_t dataNume; // 心率曲线记录数据总个数
    uint16_t currIndex; // 当前已显示心率曲线心率个数
    UIChartDataSerial *dataSerial[HEART_AVE_NUM]{nullptr};
    UIChartDataSerial *dataTopAndbottom{nullptr};
    UIImageView *imageViewTop{nullptr};
    UIImageView *imageViewBottom{nullptr};
    uint16_t serialIndex;
    UIImageAnimatorView *imageAnimator{nullptr};
    UIImageView *imageViewCoordinate{nullptr};

    UILabel *topRate{nullptr};
    UILabel *bottomRate{nullptr};
    UILabel *ratePerSec{nullptr};
    UILabel *ratePerSecText{nullptr};
    UILabel *restRate{nullptr};
    UILabel *restRateText1{nullptr};
    UILabel *restRateText2{nullptr};

    uint16_t maxRate;
    uint16_t minRate;

    bool LoadAppImages(std::string file, UIImageView *view, uint32_t resId);
};
} // namespace OHOS
#endif // HEART_RATE_VIEW_H
