/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: gui filter func.
 * Author:
 * Create:
 */

#include "power_display_service.h"
#include "display_filter.h"
#include "common_def.h"
#include "touch_screen_def.h"

static input_action_t filter_gui_execute_product_rule(screen_context_t *current, uint32_t event)
{
    unused(current);
    unused(event);
    return KEEP_CURRENT_STATE;
}

static input_action_t filter_gui_execute_key_event(screen_context_t *current, uint32_t type, uint32_t event)
{
    unused(current);
    unused(type);
    unused(event);
    return TURN_ON_SCREEN;
}

static input_action_t filter_gui_execute_touch_event(screen_context_t *current, uint32_t type, uint32_t event)
{
    unused(type);
    input_action_t ac = KEEP_CURRENT_STATE;
    if (event == MC_TP_PRESS) {
        current->press_state = true;
    } else if (event == MC_TP_RELEASE || event == MC_TP_COVER) {
        current->press_state = false;
    }

    screen_context_t *sc = power_display_get_screen_context();
    if (event == MC_TP_COVER) {
        if (sc->enable_aod) {
            if (sc->current_state == SCREEN_ON) {
                ac = ENTER_AMBIENT;
            } else if (sc->current_state == SCREEN_AMBIENT) {
                ac = TURN_ON_SCREEN;
            }
        } else {
            ac = TURN_OFF_SCREEN;
        }
    } else {
        if (sc->enable_aod) {
            if (sc->current_state == SCREEN_AMBIENT) {
                // The driver needs to implement the logic for reporting these events.
                if (event == MC_TP_SHORT_CLICK || event == MC_TP_DOUBLE_CLICK) {
                    ac = TURN_ON_SCREEN;
                }
            } else {
                ac = TURN_ON_SCREEN;
            }
        } else {
            ac = TURN_ON_SCREEN;
        }
    }
    POWER_DISPLAY_PRINT_DEBUG(0, "filter_gui_execute_touch_event,ac:%u.", ac);
    return ac;
}

static input_action_t filter_gui_execute_gesture_event(screen_context_t *current, uint32_t type, uint32_t event)
{
    unused(current);
    unused(type);
    unused(event);
    input_action_t ac = KEEP_CURRENT_STATE;
    return ac;
}

static input_action_t filter_gui_execute_fake_event(screen_context_t *current, uint32_t type, uint32_t event)
{
    unused(current);
    unused(type);
    input_action_t ac = KEEP_CURRENT_STATE;
    if (event == KEY_SHORT_PRESS) {
        ac = TURN_ON_SCREEN;
    } else if (event == COVER_SCREEN) {
        ac = TURN_OFF_SCREEN;
    }
    return ac;
}

static input_action_t filter_gui_execute_system_rule(screen_context_t *current, uint32_t event)
{
    input_action_t ac = KEEP_CURRENT_STATE;
    uint32_t gui_event_type = event & GUI_EVENT_TYPE_MASK;
    uint32_t gui_event = event & GUI_EVENT_ID_MASK;
    switch (gui_event_type) {
        case KEY:
            ac = filter_gui_execute_key_event(current, gui_event_type, gui_event);
            break;
        case TOUCH:
            ac = filter_gui_execute_touch_event(current, gui_event_type, gui_event);
            break;
        case GESTURE:
            ac = filter_gui_execute_gesture_event(current, gui_event_type, gui_event);
            break;
        case FAKE:
            ac = filter_gui_execute_fake_event(current, gui_event_type, gui_event);
            break;
        default:
            break;
    }
    return ac;
}

static event_filter_t g_event_gui_filter = {
    .system_rule = filter_gui_execute_system_rule,
    .product_rule = filter_gui_execute_product_rule,
};

event_filter_t *display_get_gui_filter(void)
{
    return &g_event_gui_filter;
}