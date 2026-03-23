/*
 * Copyright (c) CompanyNameMagicTag 2025. All rights reserved.
 * Description: DialOHSliceId
 * Author:
 * Create: 2025-02-15
 */


#ifndef DIALOHSLICEID_H
#define DIALOHSLICEID_H

#include <unordered_map>
#include "AppViewIDs.h"

namespace OHOS {
enum class DialOHSliceId : uint16_t {
    DUAL_TIME_ZONES = 0,
    HEART_RATE = 1,
    STANDING_TIMES = 2,
    STRESS = 3,
    MODERATE_TO_VIGOROUS_INTENSITY_TIME = 4,
    WEATHER = 5,
    AIR_QUALITY_ONLY_CHINA = 6,
    ATMOSPHERIC_PRESSURE = 7,
    ELEVATION_HEIGHT = 8,
    SUNRISE_SUNSET_TIME = 9,
    DISTANCE = 10,
    SLEEP = 11,
    EXERCISE = 12,
    PAYMENT_ONLY_CHINA = 13,
    ALARM_CLOCK = 14,
    STOPWATCH = 15,
    TIMER = 16,
    EXERCISE_RECORD = 17,
    TRAINING_STATUS = 18,
    ACTIVITY_RECORD = 19,
    AEROBIC_EXERCISE = 20,
    CONTACTS = 21,
    MUSIC = 22,
    COMPASS = 23,
    CALL_LOG = 24,
    BLOOD_OXYGEN_SATURATION_ONLY_CHINA = 25,
    ULTRAVIOLET_RAYS_ONLY_CHINA = 26,
    PHASE_OF_THE_MOON = 27,
    STAIR_CLIMBING = 28,
    BLOOD_PRESSURE = 29,
    MESSAGE_LIST = 30,
    DIAL = 31,
    AUDIO_SWITCH = 32,
}; // enum class DialOHSliceId

struct HashOhSliceId {
    size_t operator()(DialOHSliceId id) const
    {
        return std::hash<uint16_t>()(static_cast<uint16_t>(id));
    }
};

const std::unordered_map<DialOHSliceId, AppViewId, HashOhSliceId> g_sliceIdConverter = {
    {DialOHSliceId::COMPASS, VIEW_COMPASS},
    {DialOHSliceId::DIAL, VIEW_CALLER_SAMPLE},
    {DialOHSliceId::ALARM_CLOCK, VIEW_MAIN_ALARM},
    {DialOHSliceId::AUDIO_SWITCH, VIEW_VOLUME},
};
} // namespace OHOS
#endif // DIALOHSLICEID_H
