/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: Power display action header file.
 * Author:
 * Create:
 */
#ifndef OHOS_UI_SCREENNOTIFY_H
#define OHOS_UI_SCREENNOTIFY_H
#include "ohos_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SCREEN_STATUS_ON,
    SCREEN_STATUS_OFF,
    SCREEN_STATUS_MAX,
} ScreenStatus;

typedef void (*ScreenStatusNotifyCallback)(ScreenStatus status);
void registerNotifyScreenStatus(ScreenStatusNotifyCallback callback);
void deregisterNotifyScreenStatus(ScreenStatusNotifyCallback callback);

void notify_screen_on_event(void);
void notify_screen_off_event(void);
void notify_screen_aod_on_event(void);
void notify_screen_aod_off_event(void);

/**
 * @brief 设置灭屏后slice的存活期，需要在slice对应的Presentor的OnStart方法中调用
 * 默认存活期是5秒，灭屏超过5秒后再次亮屏会跳回主界面
 * @param time_sec 设置的存活期时间
 */
void set_back_to_home_interval(uint32_t time_sec);

/**
 * @brief 获取灭屏后slice的存活期，需要在slice对应的Presentor的OnStart方法中调用
 * 默认存活期是5秒，灭屏超过5秒后再次亮屏会跳回主界面
 * @param time_sec 获取的slice灭屏后的存活期时间
 */
uint32_t get_back_to_home_interval(void);

/**
 * @brief 设置进入AOD后返回上一个应用的可返回时间间隔，需要在slice对应的Presentor的OnStart方法中调用
 * 默认可返回时间间隔是5秒，进入AOD超过5秒后再次亮屏会跳回主界面
 * @param time_sec 设置的可返回时间间隔
 */
void set_back_to_pre_slice_interval(uint32_t time_sec);

/**
 * @brief 获取进入AOD后返回上一个应用的可返回时间间隔，需要在slice对应的Presentor的OnStart方法中调用
 * 默认可返回时间间隔是5秒，进入AOD超过5秒后再次亮屏会跳回主界面
 * @param time_sec 获取的可返回时间间隔
 */
uint32_t get_back_to_pre_slice_interval(void);

#ifdef __cplusplus
}
#endif

#endif // OHOS_UI_SCREENNOTIFY_H
