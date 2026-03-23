/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: slice filter func.
 * Author:
 * Create:
 */
#include "power_display_service.h"
#include "display_filter.h"
#include "common_def.h"

static input_action_t filter_slice_execute_product_rule(screen_context_t *current, uint32_t event)
{
    unused(current);
    unused(event);
    return KEEP_CURRENT_STATE;
}

static input_action_t filter_slice_execute_system_rule(screen_context_t *current, uint32_t event)
{
    unused(event);
    input_action_t ac = KEEP_CURRENT_STATE;
    if (current->current_state == SCREEN_ON) {
        ac = RESET_TIMER;
    }
    return ac;
}

static event_filter_t g_event_slice_filter = {
    .system_rule = filter_slice_execute_system_rule,
    .product_rule = filter_slice_execute_product_rule,
};

event_filter_t *display_get_slice_filter(void)
{
    return &g_event_slice_filter;
}