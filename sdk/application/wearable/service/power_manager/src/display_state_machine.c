/**
 * Copyright (c) CompanyNameMagicTag 2021-2022. All rights reserved.
 * Description: Power display machine func.
 * Author:
 * Create:
 */

#include "cmsis_os2.h"
#include "systick.h"
#include "power_display_service.h"
#include "display_action.h"
#include "common_def.h"
#include "display_state_machine.h"

#define NULL_PTR 0
/* * 获取对应动作的掩码 */
#define GET_ACTION_MASK(action) (1 << ((uint32_t)(action)))
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

typedef struct state_change_info {
    screen_state_t new_state;
    screen_state_t old_state;
    uint16_t action_bitmap;
} state_change_info_t;

typedef struct screen_state_change_maxtrix {
    /* 切换之前的状态 */
    uint16_t oldState;
    /* 外部输入动作 */
    uint16_t input_action_bitmap;
    /* 切换后的状态 */
    uint16_t newState;
    /* 进入新的状态需要执行的动作 */
    uint16_t state_change_action_bitmap;
} screen_state_change_maxtrix_t;

static screen_state_change_maxtrix_t g_state_change_matrix[] = {
    // 当前状态为亮屏
    {
        SCREEN_ON,
        GET_ACTION_MASK(KEEP_CURRENT_STATE) |
        GET_ACTION_MASK(TURN_ON_SCREEN) |
        GET_ACTION_MASK(RESET_TIMER),
        SCREEN_ON,
        GET_ACTION_MASK(SET_DISPLAY_ON) |
        GET_ACTION_MASK(SET_TE_ON) |
        GET_ACTION_MASK(SET_DISPLAY_TO_NORMAL_MODE) |
        GET_ACTION_MASK(SET_TP_TO_NORMAL_WORK_MODE) |
        GET_ACTION_MASK(START_TIMER),
    },
    {
        SCREEN_ON,
        GET_ACTION_MASK(TURN_OFF_SCREEN),
        SCREEN_OFF,
        GET_ACTION_MASK(SET_DISPLAY_OFF) |
        GET_ACTION_MASK(SET_TE_OFF) |
        GET_ACTION_MASK(SET_TP_TO_SLEEP_MODE) |
        GET_ACTION_MASK(STOP_TIMER),
    },
    {
        SCREEN_ON,
        GET_ACTION_MASK(ENTER_AMBIENT),
        SCREEN_AMBIENT,
        GET_ACTION_MASK(SET_DISPLAY_ON) |
        GET_ACTION_MASK(SET_DISPLAY_TO_IDLE_MODE),
    },
    // 当前状态为灭屏
    {
        SCREEN_OFF,
        GET_ACTION_MASK(TURN_ON_SCREEN),
        SCREEN_ON,
        GET_ACTION_MASK(SET_DISPLAY_ON) |
        GET_ACTION_MASK(SET_TE_ON) |
        GET_ACTION_MASK(SET_TP_TO_NORMAL_WORK_MODE) |
        GET_ACTION_MASK(START_TIMER),
    },
    {
        SCREEN_OFF,
        GET_ACTION_MASK(ENTER_AMBIENT),
        SCREEN_AMBIENT,
        GET_ACTION_MASK(SET_DISPLAY_ON) |
        GET_ACTION_MASK(SET_DISPLAY_TO_IDLE_MODE) |
        GET_ACTION_MASK(SET_TE_ON),
    },
    {
        SCREEN_OFF,
        GET_ACTION_MASK(KEEP_CURRENT_STATE) |
        GET_ACTION_MASK(TURN_OFF_SCREEN) |
        GET_ACTION_MASK(RESET_TIMER),
        SCREEN_OFF,
        GET_ACTION_MASK(SET_DISPLAY_OFF) |
        GET_ACTION_MASK(SET_TE_OFF) |
        GET_ACTION_MASK(SET_TP_TO_SLEEP_MODE) |
        GET_ACTION_MASK(STOP_TIMER),
    },

    // 当前状态为Ambient
    {
        SCREEN_AMBIENT,
        GET_ACTION_MASK(TURN_ON_SCREEN),
        SCREEN_ON,
        GET_ACTION_MASK(SET_DISPLAY_ON) |
        GET_ACTION_MASK(SET_TE_ON) |
        GET_ACTION_MASK(SET_DISPLAY_TO_NORMAL_MODE) |
        GET_ACTION_MASK(SET_TP_TO_NORMAL_WORK_MODE) |
        GET_ACTION_MASK(START_TIMER),
    },
    {
        SCREEN_AMBIENT,
        GET_ACTION_MASK(KEEP_CURRENT_STATE) |
        GET_ACTION_MASK(ENTER_AMBIENT) |
        GET_ACTION_MASK(RESET_TIMER),
        SCREEN_AMBIENT,
        GET_ACTION_MASK(SET_DISPLAY_ON) |
        GET_ACTION_MASK(SET_DISPLAY_TO_IDLE_MODE)
    },
    {
        SCREEN_AMBIENT,
        GET_ACTION_MASK(TURN_OFF_SCREEN),
        SCREEN_OFF,
        GET_ACTION_MASK(SET_DISPLAY_OFF) |
        GET_ACTION_MASK(SET_TE_OFF) |
        GET_ACTION_MASK(SET_TP_TO_SLEEP_MODE) |
        GET_ACTION_MASK(STOP_TIMER),
    },
};

static void power_display_run_state_machine(uint32_t input_action_bitmap, state_change_info_t *info)
{
    screen_context_t *sc = power_display_get_screen_context();
    info->old_state = sc->current_state;
    /* 状态转移矩阵匹配不成功,默认不改变状态. */
    info->new_state = info->old_state;
    info->action_bitmap = 0;
    for (uint32_t i = 0; i < ARRAY_SIZE(g_state_change_matrix); i++) {
        if (g_state_change_matrix[i].oldState != info->old_state) {
            continue;
        }
        if ((input_action_bitmap & g_state_change_matrix[i].input_action_bitmap) != 0) {
            info->action_bitmap = g_state_change_matrix[i].state_change_action_bitmap;
            info->new_state = g_state_change_matrix[i].newState;
            break;
        }
    }
    if (info->new_state != info->old_state) {
        POWER_DISPLAY_PRINT_DEBUG(0, "input_action_bitmap:%x", input_action_bitmap);
    }
    return;
}

static void power_display_notify_graphic_engine(screen_state_t new_state, screen_state_t old_state)
{
    unused(new_state);
    unused(old_state);
    return;
}

static errcode_t power_display_do_state_change_action(uint16_t action_bitmap, screen_state_t new_state,
    event_type_t event_type, uint32_t event)
{
    POWER_DISPLAY_PRINT_DEBUG(0, "action in, action_bitmap:%u, new_state is %x,event_type %u, event:%u.", action_bitmap,
        new_state, event_type, event);
    int ret = action_display_execute_entry(action_bitmap, new_state, event_type, event);
    if (ret != 0) {
        POWER_DISPLAY_PRINT_ERR(0, "action_display_execute_entry failure, ret is %x.", ret);
        return ERRCODE_FAIL;
    }
    ret = action_touch_execute_entry(action_bitmap, new_state, event_type, event);
    if (ret != 0) {
        POWER_DISPLAY_PRINT_ERR(0, "action_touch_execute_entry failure, ret is %x.", ret);
        return ERRCODE_FAIL;
    }
    ret = action_timer_execute_entry(action_bitmap, new_state, event_type, event);
    if (ret != 0) {
        POWER_DISPLAY_PRINT_ERR(0, "action_timer_execute_entry failure, ret is %x.", ret);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

static bool power_display_ignore_timeout_event(event_type_t event_type, input_action_t action)
{
    if ((event_type == TIMER_EVENT) && (action == TURN_OFF_SCREEN) && (action_timer_get_timer_is_running() == true)) {
        return true;
    }
    return false;
}

errcode_t power_display_execute_state_change_action(event_type_t event_type, input_action_t action, uint32_t event)
{
    state_change_info_t info_p;
    if (power_display_ignore_timeout_event(event_type, action)) {
        return ERRCODE_FAIL;
    }
    power_display_run_state_machine(GET_ACTION_MASK(action), &info_p);
    power_display_notify_graphic_engine(info_p.new_state, info_p.old_state);
    errcode_t retcode = power_display_do_state_change_action(info_p.action_bitmap, info_p.new_state, event_type, event);
    if (retcode != ERRCODE_SUCC) {
        POWER_DISPLAY_PRINT_ERR(0, "power_display_do_state_change_action failure, ret is %x.", retcode);
        return retcode;
    }
    return ERRCODE_SUCC;
}
