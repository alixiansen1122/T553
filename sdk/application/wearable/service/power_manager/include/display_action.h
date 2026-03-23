/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: Power display action header file.
 * Author:
 * Create:
 */
#ifndef POWERMGR_DISPLAY_ACTION_H
#define POWERMGR_DISPLAY_ACTION_H

#include "power_display_service.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @brief 显示功能执行入口.
 * @param action_bitmap 表示状态发生变化需要执行的动作，每一bit位表示具体动作.
 * @param new_state 表示状态发生变化后的新状态.
 * @param event_type_t 触发的事件类型，@see event_type_t.
 * @param event_type_t 触发的具体事件.
 * @return 返回 ERRCODE_SUCC 表示操作成功; 否则返回错误原因.
 */
int32_t action_display_execute_entry(uint16_t action_bitmap, uint16_t new_state, event_type_t type, uint32_t event);

/**
 * @brief 定时器功能执行入口.
 * @param action_bitmap 表示状态发生变化需要执行的动作，每一bit位表示具体动作.
 * @param new_state 表示状态发生变化后的新状态.
 * @param event_type_t 触发的事件类型，@see event_type_t.
 * @param event_type_t 触发的具体事件.
 * @return 返回 ERRCODE_SUCC 表示操作成功; 否则返回错误原因.
 */
int32_t action_timer_execute_entry(uint16_t action_bitmap, uint16_t new_state, event_type_t type, uint32_t event);

/**
 * @brief 触屏功能执行入口.
 * @param action_bitmap 表示状态发生变化需要执行的动作，每一bit位表示具体动作.
 * @param new_state 表示状态发生变化后的新状态.
 * @param event_type_t 触发的事件类型，@see event_type_t.
 * @param event_type_t 触发的具体事件.
 * @return 返回 ERRCODE_SUCC 表示操作成功; 否则返回错误原因.
 */
int32_t action_touch_execute_entry(uint16_t action_bitmap, uint16_t new_state, event_type_t type, uint32_t event);

/**
 * @brief 获取定时运行状态.
 * @return 返回 true 表示定时器正在运行; false 表示定时器当前为停止状态.
 */
bool action_timer_get_time_is_running(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif



#endif