/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: action timer func.
 * Author:
 * Create:
 */
#include "cmsis_os2.h"
#include "power_display_service.h"
#include "display_action.h"
#include "common_def.h"
#include "systick.h"

typedef struct screen_timer {
    osTimerId_t timer_id;
    int32_t interval; /* duration in ms */
} screen_timer_t;

typedef enum displasy_timer_index {
    DISPLAY_AUTO_POWER_OFF = 0,
    DISPLAY_TIMER_NUM,
} displasy_timer_index_t;

typedef enum timer_type {
    DEFAULT_SCREEN_TIMER = 0,
    SLICE_TIMER,
    KEEP_ON_TIMER,
    KILL_APP_TIMER,
    INVALID,
} timer_type_t;

static osTimerId_t g_action_timer_id = NULL;
static timer_type_t g_action_timer_type = INVALID;
static uint64_t g_action_timer_start_stamp = 0;
static screen_timer_t g_action_screen_timer[DISPLAY_TIMER_NUM];
static bool g_action_timer_running = false;

uint64_t action_timer_get_start_stamp(void)
{
    return g_action_timer_start_stamp;
}

bool action_timer_get_timer_is_running(void)
{
    return g_action_timer_running;
}

static void action_timer_timeout_call_back(void *argument)
{
    unused(argument);
    power_display_svr_api_t *display_api = power_display_svr_get_api();
    screen_context_t *sc = power_display_get_screen_context();
    g_action_screen_timer[0].interval = 0;
    POWER_DISPLAY_PRINT_DEBUG(0, "stop_timer:in,state:%u, enable:%u, keepontime:%u, type:%u.", sc->press_state,
        sc->time_out_enable, sc->keep_on_timeout, g_action_timer_type);
    switch (g_action_timer_type) {
        case DEFAULT_SCREEN_TIMER:
            display_api->screen_timeout_func(DEFAULT_TIMER_TIME_OUT);
            break;
        case KEEP_ON_TIMER:
            display_api->screen_timeout_func(KEEP_ON_TIMER_TIME_OUT);
            break;
        default:
            break;
    }
    g_action_timer_running = false;
    g_action_timer_type = INVALID;

    return;
}

static int32_t action_timer_stop_wrapper_timer(void)
{
    if (g_action_screen_timer[0].interval == 0) {
        return 0;
    }
    g_action_screen_timer[0].interval = 0;
    osStatus_t ret = osTimerStop(g_action_screen_timer[0].timer_id);
    if (ret != osOK) {
        POWER_DISPLAY_PRINT_ERR(0, "stop_timer:stop failed,ret = %u.\r\n", ret);
    }
    screen_context_t *sc = power_display_get_screen_context();
    sc->keep_on_timeout = 0;
    g_action_timer_running = false;
    g_action_timer_type = INVALID;
    return ret;
}

static int32_t action_timer_create_wrapper_timer(void)
{
    if (g_action_screen_timer[0].timer_id != NULL) {
        return 0;
    }
    g_action_screen_timer[0].timer_id = osTimerNew(action_timer_timeout_call_back, osTimerOnce, (void *)0, NULL);
    if (g_action_screen_timer[0].timer_id == NULL) {
        POWER_DISPLAY_PRINT_ERR(0, "create timer failure.");
        return -1;
    }

    return 0;
}

static int32_t action_timer_start_wrapper_timer()
{
    screen_context_t *sc = power_display_get_screen_context();

    if (sc->keep_on_timeout != 0 && g_action_timer_type == KEEP_ON_TIMER) {
        return 0;
    }
    if (osTimerIsRunning(g_action_screen_timer[0].timer_id)) {
        osTimerStop(g_action_screen_timer[0].timer_id);
    }

    if (sc->keep_on_timeout != 0) {
        g_action_timer_type = KEEP_ON_TIMER;
        g_action_screen_timer[0].interval = sc->keep_on_timeout;
    } else {
        g_action_timer_type = DEFAULT_SCREEN_TIMER;
        g_action_screen_timer[0].interval = sc->screen_off_timeout;
    }

    int32_t retcode = osTimerStart(g_action_screen_timer[0].timer_id, g_action_screen_timer[0].interval);
    if (retcode != 0) {
        POWER_DISPLAY_PRINT_ERR(0, "start timer failure,ret: %u.", retcode);
        return retcode;
    }
    g_action_timer_running = true;
    g_action_timer_start_stamp = uapi_systick_get_ms();
    return retcode;
}

int32_t action_timer_execute_entry(uint16_t action_bitmap, uint16_t new_state, event_type_t type, uint32_t event)
{
    unused(new_state);
    unused(type);
    unused(event);
    int32_t ret = 0;
    screen_context_t *sc = power_display_get_screen_context();
    if (sc->time_out_enable == false) {
        return 0;
    }
    POWER_DISPLAY_PRINT_DEBUG(0, "action_timer_execute_entry in");
    if (action_timer_create_wrapper_timer() != 0) {
        return -1;
    }

    if ((action_bitmap & GET_ACTION_MASK(START_TIMER)) != 0) {
        ret = action_timer_start_wrapper_timer();
    }

    if ((action_bitmap & GET_ACTION_MASK(STOP_TIMER)) != 0) {
        ret = action_timer_stop_wrapper_timer();
    }

    return ret;
}