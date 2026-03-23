/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: action touch func.
 * Author:
 * Create:
 */

#include "power_display_service.h"
#include "display_action.h"
#include "common_def.h"

int32_t action_touch_execute_entry(uint16_t action_bitmap, uint16_t new_state, event_type_t type, uint32_t event)
{
    unused(type);
    unused(event);
    screen_context_t *sc = power_display_get_screen_context();
    if (new_state == sc->current_state) {
        return 0;
    } else {
        POWER_DISPLAY_PRINT_ERR(0, "entry:action_bitmap:%u,current state:%u. \r\n", action_bitmap, sc->current_state);
    }
    errcode_t ret;
    if ((action_bitmap & GET_ACTION_MASK(SET_TP_TO_NORMAL_WORK_MODE)) != 0) {
        // rerserve
    }

    if ((action_bitmap & GET_ACTION_MASK(SET_TP_TO_SLEEP_MODE)) != 0) {
        // rerserve
    }

    if ((action_bitmap & GET_ACTION_MASK(SET_TP_TO_STANDBY_MODE)) != 0) {
        // rerserve
    }
    return 0;
}