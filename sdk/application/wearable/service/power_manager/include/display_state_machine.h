/**
 * Copyright (c) CompanyNameMagicTag 2021-2022. All rights reserved.
 * Description: Power display sm header file.
 * Author:
 * Create:
 */

#ifndef POWERMGR_DISPLAY_SM_H
#define POWERMGR_DISPLAY_SM_H

#include "power_display_service.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @brief 显示状态变化迁移函数.
 * @param event_type_t 触发的事件类型，@see event_type_t.
 * @param action 状态切换动作，@see input_action_t.
 * @param event 触发的具体事件.
 * @return 返回 ERRCODE_SUCC 表示操作成功; 否则返回错误原因.
 */
errcode_t power_display_execute_state_change_action(event_type_t event_type, input_action_t action, uint32_t event);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


#endif