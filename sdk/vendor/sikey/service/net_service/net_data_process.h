#ifndef PET_NECKLACE_PROTOCOL_H
#define PET_NECKLACE_PROTOCOL_H

#include <stdlib.h>
#include "app_at_process.h"

// 消息类型枚举
typedef enum
{
    REAL_TIME_ACTION,
    REGULAR_TIME_ACTION,
    PET_NECKLACE_ELECTRICITY,
    PET_NECKLACE_SIX,
    REPLACE_MODEL,
    MODEL_VERSION,
    BUZZER,
    REPEATER_UPGRADATION,
    SYNCHRONOUS_TIME,
    SPEECH,
    ACTIVE_LOCATION,
    SEARCH_LOCATION,
    RESTART,
    SHUTDOWN,
    PET_NECKLACE_CHARGING,
    NECKLACE_OTA,
    GET_AIR_PRESSURE,
    GET_WIFILIST,
    DORMANCY,
    PAQICHONG_DEBUG,
    UNKNOWN_TYPE
} MessageType;

extern bool location_flag;
/**
 * @brief 将字符串转换为消息类型枚举
 *
 * @param type_str 类型字符串
 * @return MessageType 对应的枚举值
 */
MessageType get_message_type(const char* type_str);
int32_t ws_report_real_time_action(int64_t posture, uint32_t count);
int32_t ws_report_regular_time_action(void);
int32_t ws_report_location(void);
uint8_t ws_report_chg_status(uint8_t chargeStatus);
int32_t ws_report_wifilist(void);
int32_t ws_report_model_version(void);
int32_t ws_report_air_pressure(void);
int32_t ws_report_debug_info(char *buf);
void ws_record_action(int64_t posture, uint32_t count);
void unpack_data(const char *json_str);
#define NET_DATA_PRINT(s, ...)       factory_test_print(FT_RETURN_SUCC, s, ##__VA_ARGS__)
#endif // PET_NECKLACE_PROTOCOL_H