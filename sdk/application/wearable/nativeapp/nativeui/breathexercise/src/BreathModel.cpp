/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BreathModel
 * Create: 2025-5-13
 */
#include "main/HealthModel.h"
#include "UiConfig.h"
#include "common/image_cache_manager.h"
#include "uiservice/ui_service.h"
#include "wearable_log.h"
#include "breathexercise/BreathTimeSetPage.h"
#include "breathexercise/BreathModel.h"


namespace OHOS {
static constexpr char *SPEED_FASTER_LITTLE = "稍快";
static constexpr char *SPEED_SOOTHING = "舒缓";
static constexpr char *SPEED_SLOWING = "缓慢";
static constexpr uint16_t MINUTE_0_DATA = 0;
static constexpr uint16_t MINUTE_1_DATA = 1;
static constexpr uint16_t MINUTE_2_DATA = 2;
static constexpr uint16_t Frequency_Score_LOWER = 60;
static constexpr uint16_t Frequency_Score_RANGE = 40;
static constexpr uint16_t Heart_Rate_LOWER = 90;
static constexpr uint16_t Heart_Rate_RANGE = 10;
static constexpr uint16_t ONE_MINUTE = 60;
static constexpr uint16_t TWO_MINUTES = 120;
static constexpr uint16_t THREE_MINUTES = 180;
static constexpr uint8_t FREQUENCY_8 = 0;
static constexpr uint8_t FREQUENCY_7 = 1;
static constexpr uint8_t FREQUENCY_6 = 2;
static constexpr float BREATH_ROTETAESTEP_6 = 0.295f;
static constexpr float BREATH_ROTETAESTEP_7 = 0.347f;
static constexpr float BREATH_ROTETAESTEP_8 = 0.393f;

BreathModel::BreathModel() {}

BreathModel::~BreathModel() {}

BreathModel &BreathModel::GetInstance()
{
    static BreathModel instance;
    return instance;
}

uint8_t BreathModel::GetTrainTime()
{
    return selectTrainTime_;
}

void BreathModel::SetTrainTime(uint16_t value)
{
    selectTrainTime_ = value;
}

uint8_t BreathModel::GetRhythm()
{
    return selectRhythm_;
}

void BreathModel::SetRhythm(uint16_t value)
{
    selectRhythm_ = value;
}

int32_t BreathModel::GetStartTime()
{
    return startTime_;
}

void BreathModel::SetStartTime(int32_t value)
{
    startTime_ = value;
}

int32_t BreathModel::GetBeginTime()
{
    return beginTime_;
}

void BreathModel::SetBeginTime(int32_t value)
{
    beginTime_ = value;
}

uint8_t BreathModel::GetFrequencyScore()
{
    if (frequencyScore_ == 0) {
        BreathModel::GetInstance().SetFrequencyScore();
        return frequencyScore_;
    }
    return frequencyScore_;
}

void BreathModel::SetFrequencyScore()
{
    frequencyScore_ = Frequency_Score_LOWER + rand() % Frequency_Score_RANGE;
}

std::string BreathModel::GetTrainingDurationTime()
{
    return std::to_string(trainingDurationTime_);
}

void BreathModel::SetTrainingDurationTime(uint16_t selectTrainTime)
{
    trainingDurationTime_ += selectTrainTime;
}

std::string BreathModel::GetTrainingCount()
{
    return std::to_string(trainingCount_);
}

void BreathModel::SetTrainingCount()
{
    trainingCount_++;
}

uint8_t BreathModel::GetHeartRate()
{
    if (heartRate_ == 0) {
        BreathModel::GetInstance().SetHeartRate();
        return heartRate_;
    }
    return heartRate_;
}

void BreathModel::SetHeartRate()
{
    heartRate_ = Heart_Rate_LOWER + rand() % Heart_Rate_RANGE;
}

float BreathModel::GetRotateStep()
{
    switch (selectRhythm_) {
        case FREQUENCY_6:
            return BREATH_ROTETAESTEP_6;
        case FREQUENCY_7:
            return BREATH_ROTETAESTEP_7;
        case FREQUENCY_8:
            return BREATH_ROTETAESTEP_8;
        default:
            return BREATH_ROTETAESTEP_6;
    }
}

char *BreathModel::GetRhythmText()
{
    if (selectRhythm_ == MINUTE_0_DATA) {
        return SPEED_FASTER_LITTLE;
    } else if (selectRhythm_ == MINUTE_1_DATA) {
        return SPEED_SOOTHING;
    } else if (selectRhythm_ == MINUTE_2_DATA) {
        return SPEED_SLOWING;
    }
    return nullptr;
}
}  // namespace OHOS