/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: timer filter func.
 * Author:
 * Create:
 */
#include "power_display_service.h"
#include "display_filter.h"
#include "common_def.h"

static input_action_t filter_timer_execute_product_rule(screen_context_t *current, uint32_t event)
{
    unused(current);
    unused(event);
    return KEEP_CURRENT_STATE;
}

static input_action_t filter_timer_execute_system_rule(screen_context_t *current, uint32_t event)
{
    input_action_t ac = KEEP_CURRENT_STATE;
    screen_context_t *sc = power_display_get_screen_context();
    if (event == SET_DEFAULT_TIMEOUT || event == SET_KEEP_ON_TIMEOUT) {
        POWER_DISPLAY_PRINT_ERR(0, "filter timer, current default timeout:%u, keepon time:%u, event:%u.",
            current->screen_off_timeout, current->keep_on_timeout, event);
        if (current->current_state == SCREEN_ON) {
            ac = RESET_TIMER;
        }
    } else if (event == DEFAULT_TIMER_TIME_OUT) {
        if (sc->press_state == 0 && sc->time_out_enable == true) {
            if (sc->enable_aod) {
                ac = ENTER_AMBIENT;
            } else {
                ac = TURN_OFF_SCREEN;
            }
        }
    } else if (event == KEEP_ON_TIMER_TIME_OUT) {
        ac = RESET_TIMER;
    }

    return ac;
}

static event_filter_t g_event_timer_filter = {
    .system_rule = filter_timer_execute_system_rule,
    .product_rule = filter_timer_execute_product_rule,
};

event_filter_t *display_get_timer_filter(void)
{
    return &g_event_timer_filter;
}
