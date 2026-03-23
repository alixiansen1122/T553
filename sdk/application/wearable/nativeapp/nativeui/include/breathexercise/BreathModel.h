/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BreathModel
 * Create: 2025-5-13
 */
#ifndef BREATH_MODEL_H
#define BREATH_MODEL_H

#include <string>
#include "gfx_utils/graphic_math.h"
namespace OHOS {
class BreathModel {
public:
    static BreathModel &GetInstance();
    virtual ~BreathModel();

    uint8_t GetTrainTime();
    void SetTrainTime(uint16_t value);           // 设置呼吸训练时间
    uint8_t GetRhythm();
    void SetRhythm(uint16_t value);              // 设置呼吸节奏
    int32_t GetStartTime();
    void SetStartTime(int32_t value);            // 设置呼吸准备页面开始时间
    int32_t GetBeginTime();
    void SetBeginTime(int32_t value);            // 设置呼吸训练开始时间
    uint8_t GetFrequencyScore();
    void SetFrequencyScore();                    // 调用传感器接口实现呼吸数据，客户自实现
    uint8_t GetHeartRate();
    void SetHeartRate();                         // 调用传感器接口实现心率数据，客户自实现
    std::string GetTrainingDurationTime();
    void SetTrainingDurationTime(uint16_t data); // 增加训练总时长
    std::string GetTrainingCount();
    void SetTrainingCount();                     // 增加训练总次数
    float GetRotateStep();                       // 获取图片旋转步频
    char *GetRhythmText();                       // 获取呼吸频率节奏文本

private:
    BreathModel();
    BreathModel(const BreathModel &);
    BreathModel &operator=(const BreathModel &);
    uint8_t selectTrainTime_ = 0;
    uint8_t selectRhythm_ = 0;
    uint8_t trainingDurationTime_ = 0;
    uint8_t trainingCount_ = 0;
    int32_t startTime_ = 0;  // 用于记录五秒倒计时开始时间
    int32_t beginTime_ = 0;  // 用于记录训练倒计时开始时间
    uint8_t frequencyScore_ = 0;
    uint8_t heartRate_ = 0;
};
}

#endif