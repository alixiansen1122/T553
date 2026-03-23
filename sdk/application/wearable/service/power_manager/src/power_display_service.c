/**
 * Copyright (c) CompanyNameMagicTag 2021-2022. All rights reserved.
 * Description: Power display service.
 * Author:
 * Create:
 */

#include "power_display_service.h"
#include "cmsis_os2.h"
#include "systick.h"
#include "display_filter.h"
#include "soc_errno.h"
#include "osal_msgqueue.h"
#include "osal_task.h"
#include "input_manager.h"
#include "common_def.h"
#include "soc_lcd_api.h"
#include "securec.h"
#include "input_app.h"
#include "touch_screen_def.h"
#include "display_state_machine.h"
#include "touch_screen_def.h"
#ifdef SUPPORT_OHOSFWK
#include "graphic_service_wrapper.h"
#endif

#define DISPLAY_AUTO_POWER_OFF_PERIOD_MS 5000
#define DISPLAY_DEFAULT_BRIGHTNESS 50
#define POWER_DISPLAY_QUEUE_MAX_SIZE 64
#define POWER_DISPLAY_MAX_SIZE 0x10
#define POWER_DISPLAY_MAX_SET_TIME (1000 * 60 * 60)
#define POWER_DISPLAY_ID_LEN sizeof(uint32_t)
#define CLEAR_OBJ(stru_ptr) ((*(stru_ptr)) = (__typeof__(*(stru_ptr))) { 0 })

typedef errcode_t (*message_handle)(uint32_t message_type, uint32_t message_value);

static screen_context_t g_screen_context = {
    .screen_off_timeout = DISPLAY_AUTO_POWER_OFF_PERIOD_MS,
    .keep_on_timeout = 0,
    .current_state = SCREEN_OFF,
    .brightness_mode = DISPLAY_DEFAULT_BRIGHTNESS,
    .brightness_value = 50,
    .time_out_enable = true,
    .last_gui_event_stamp = 0,
    .press_state = 0,
    .enable_aod = false,
};

#ifdef SUPPORT_OHOSFWK
void power_display_turnon_handle_ext(void *data);
void power_display_turnoff_handle_ext(void *data);
void power_display_set_screenoff_timeout_handle_ext(void *data);
void power_display_set_screenoff_switch_handle_ext(void *data);
void power_display_set_keepon_timeout_handle_ext(void *data);
void power_display_set_brightness_handle_ext(void *data);
void power_display_gui_cmd_handle_ext(void *data);
void power_display_timer_cmd_handle_ext(void *data);
#endif

static unsigned long g_power_display_queue_id = 0xFFFFFFFF;
static uint32_t g_power_recv_msg_num = 0;
bool g_discard_all_events = false;
static uint32_t g_power_tp_press_num = false;
event_filter_t *g_event_filter_map[EVENT_TYPE_SIZE] = { NULL };

static unsigned long power_display_get_msg_queue_id(void)
{
    return g_power_display_queue_id;
}

/* 发送消息接口 */
static errcode_t power_display_msg_write(uint32_t msg_id, uint8_t *msg, uint16_t msg_len, bool wait)
{
    unsigned int timeout = (wait) ? OSAL_MSGQ_WAIT_FOREVER : 0;
    uint8_t msg_data[POWER_DISPLAY_MAX_SIZE + POWER_DISPLAY_ID_LEN];

    if (osal_msg_queue_is_full(power_display_get_msg_queue_id())) {
        return ERRCODE_FAIL;
    }

    *(uint32_t *)msg_data = msg_id;

    if ((msg != NULL) && (memcpy_s(&msg_data[POWER_DISPLAY_ID_LEN], POWER_DISPLAY_MAX_SIZE, msg, msg_len) != EOK)) {
        return ERRCODE_FAIL;
    }

    return osal_msg_queue_write_copy(power_display_get_msg_queue_id(), msg_data,
        POWER_DISPLAY_MAX_SIZE + POWER_DISPLAY_ID_LEN, timeout);
}

static errcode_t power_display_turn_on_screen(void)
{
#ifndef SUPPORT_OHOSFWK
    message_type_t mt = TURN_ON_SCREEN_CMD;
    uint32_t event_info = 0;
    errcode_t ret = power_display_msg_write(mt, (uint8_t *)&event_info, sizeof(uint32_t), false);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
#else
    PostGraphicEvent(power_display_turnon_handle_ext, NULL);
#endif
    return ERRCODE_SUCC;
}

static errcode_t power_display_turn_off_screen(void)
{
#ifndef SUPPORT_OHOSFWK
    message_type_t mt = TURN_OFF_SCREEN_CMD;
    uint32_t event_info = 0;
    errcode_t ret = power_display_msg_write(mt, (uint8_t *)&event_info, sizeof(uint32_t), false);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
#else
    PostGraphicEvent(power_display_turnoff_handle_ext, NULL);
#endif
    return ERRCODE_SUCC;
}

static screen_state_t power_display_get_screen_state(void)
{
    return g_screen_context.current_state;
}

static errcode_t power_display_set_screen_auto_off_timeout(uint32_t timeout)
{
    if (g_screen_context.time_out_enable == false) {
        return ERRCODE_FAIL;
    }
    if ((timeout < DISPLAY_AUTO_POWER_OFF_PERIOD_MS) || (timeout > POWER_DISPLAY_MAX_SET_TIME)) {
        POWER_DISPLAY_PRINT_ERR(0, "Setting the screen timeout needs to be within this range[%u(s),%u(min)].\r\n ",
            DISPLAY_AUTO_POWER_OFF_PERIOD_MS, POWER_DISPLAY_MAX_SET_TIME / (1000 * 60));
        return ERRCODE_FAIL;
    }
    message_type_t mt = SET_AUTO_OFF_TIMEOUT_CMD;
    uint32_t event_info = timeout;
#ifndef SUPPORT_OHOSFWK
    errcode_t ret = power_display_msg_write(mt, (uint8_t *)&event_info, sizeof(uint32_t), false);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
#else
    uint32_t *data = (uint32_t *)malloc(sizeof(uint32_t));
    if (data != NULL) {
        *data = event_info;
        PostGraphicEvent(power_display_set_screenoff_timeout_handle_ext, data);
    }
#endif
    return ERRCODE_SUCC;
}

static errcode_t power_display_set_auto_timeout_function(bool enable)
{
    message_type_t mt = SET_AUTO_OFF_FUNC_CMD;
    uint32_t event_info = enable;
#ifndef SUPPORT_OHOSFWK
    errcode_t ret = power_display_msg_write(mt, (uint8_t *)&event_info, sizeof(uint32_t), false);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
#else
    uint32_t *data = (uint32_t *)malloc(sizeof(uint32_t));
    if (data != NULL) {
        *data = event_info;
        PostGraphicEvent(power_display_set_screenoff_switch_handle_ext, data);
    }
#endif
    return ERRCODE_SUCC;
}

static errcode_t power_display_set_keepon_timeout(uint32_t time)
{
    if (g_screen_context.keep_on_timeout != 0 && time != 0) {
        POWER_DISPLAY_PRINT_ERR(0, "current has been set %u,need to wait for timeout or set 0 to cancel. \r\n",
            g_screen_context.keep_on_timeout);
        return ERRCODE_FAIL;
    }
    message_type_t mt = SET_KEEP_SCREEN_ON_TIMEOUT_CMD;
    uint32_t real_time = time > POWER_DISPLAY_MAX_SET_TIME ? POWER_DISPLAY_MAX_SET_TIME : time;
    uint32_t event_info = real_time;
#ifndef SUPPORT_OHOSFWK
    errcode_t ret = power_display_msg_write(mt, (uint8_t *)&event_info, sizeof(uint32_t), false);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
#else
    uint32_t *data = (uint32_t *)malloc(sizeof(uint32_t));
    if (data != NULL) {
        *data = event_info;
        PostGraphicEvent(power_display_set_keepon_timeout_handle_ext, data);
    }
#endif
    return ERRCODE_SUCC;
}

static errcode_t power_display_set_brightness_mode(brightness_mode_t mode)
{
    g_screen_context.brightness_mode = mode;
    /* 当前没有适配环境光，不支持自动调节 */
    return ERRCODE_SUCC;
}

static brightness_mode_t power_display_get_brightness_mode(void)
{
    return g_screen_context.brightness_mode;
}

static errcode_t power_display_set_brightness(uint8_t brightness)
{
    message_type_t mt = SET_BRIGHTNESS_CMD;
    uint32_t event_info = brightness;
#ifndef SUPPORT_OHOSFWK
    errcode_t ret = power_display_msg_write(mt, (uint8_t *)&event_info, sizeof(uint32_t), false);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
#else
    uint32_t *data = (uint32_t *)malloc(sizeof(uint32_t));
    if (data != NULL) {
        *data = event_info;
        PostGraphicEvent(power_display_set_brightness_handle_ext, data);
    }
#endif
    return ERRCODE_SUCC;
}

static uint8_t power_display_get_brightness(void)
{
    return g_screen_context.brightness_value;
}

static errcode_t power_display_register_filter_rule(event_type_t type, execute_rule rule_func)
{
    if (type >= EVENT_TYPE_SIZE) {
        return ERRCODE_FAIL;
    }
    if (g_event_filter_map[type] == NULL) {
        return ERRCODE_FAIL;
    }
    g_event_filter_map[type]->product_rule = rule_func;
    return ERRCODE_SUCC;
}

static errcode_t power_display_register_key_event_cb(event_callback_func callbackFunc)
{
    unused(callbackFunc);
    return ERRCODE_SUCC;
}

static errcode_t power_display_register_touch_event_cb(event_callback_func callbackFunc)
{
    unused(callbackFunc);
    return ERRCODE_SUCC;
}

static errcode_t power_display_register_rotate_event_cb(event_callback_func callbackFunc)
{
    unused(callbackFunc);
    return ERRCODE_SUCC;
}

static void power_display_key_event_cb(uint32_t event, uint32_t event_value)
{
    unused(event);
    unused(event_value);
    message_type_t mt = GUI_EVENT_CMD;
    uint32_t event_info = KEY;
#ifndef SUPPORT_OHOSFWK
    errcode_t ret = power_display_msg_write(mt, (uint8_t *)&event_info, sizeof(uint32_t), false);
    if (ret != ERRCODE_SUCC) {
        POWER_DISPLAY_PRINT_ERR(0, "key_event send msg fail, ret:%u.", ret);
    }
#else
    uint32_t *data = (uint32_t *)malloc(sizeof(uint32_t));
    if (data != NULL) {
        *data = event_info;
        PostGraphicEvent(power_display_gui_cmd_handle_ext, data);
    }
#endif
    return;
}

static void power_display_touch_event_cb(uint32_t event, uint32_t event_value)
{
    unused(event_value);
    message_type_t mt = GUI_EVENT_CMD;
    uint32_t event_info = TOUCH;
    if (event == MC_TP_PRESS || event == MC_TP_MOVE) {
        event_info += MC_TP_PRESS;
        g_power_tp_press_num++;
    } else if (event == MC_TP_RELEASE) {
        event_info += MC_TP_RELEASE;
        g_power_tp_press_num = 0;
    } else {
        return;
    }
    if (g_power_tp_press_num > 1) {
        return;
    }
#ifndef SUPPORT_OHOSFWK
    errcode_t ret = power_display_msg_write(mt, (uint8_t *)&event_info, sizeof(uint32_t), false);
    if (ret != ERRCODE_SUCC) {
        POWER_DISPLAY_PRINT_ERR(0, "touch_event send msg fail, ret:%u.", ret);
    }
#else
    uint32_t *data = (uint32_t *)malloc(sizeof(uint32_t));
    if (data != NULL) {
        *data = event_info;
        PostGraphicEvent(power_display_gui_cmd_handle_ext, data);
    }
#endif
    return;
}

static errcode_t power_display_timeout_event_proc(uint32_t event)
{
    message_type_t mt = TIMER_EVENT_CMD;
    uint32_t event_info = event;
#ifndef SUPPORT_OHOSFWK
    errcode_t ret = power_display_msg_write(mt, (uint8_t *)&event_info, sizeof(uint32_t), false);
    if (ret != ERRCODE_SUCC) {
        POWER_DISPLAY_PRINT_ERR(0, "power_display_timeout_event_proc send msg fail, ret:%u.", ret);
        return ERRCODE_FAIL;
    }
#else
    uint32_t *data = (uint32_t *)malloc(sizeof(uint32_t));
    if (data != NULL) {
        *data = event_info;
        PostGraphicEvent(power_display_timer_cmd_handle_ext, data);
    }
#endif
    return ERRCODE_SUCC;
}

static void power_display_save_gui_event_stamp(input_action_t action, event_type_t event_type)
{
    unused(event_type);
    if ((action == TURN_ON_SCREEN) && (event_type == GUI_EVENT)) {
        g_screen_context.last_gui_event_stamp = uapi_systick_get_ms();
    }
    return;
}

screen_context_t *power_display_get_screen_context(void)
{
    return &g_screen_context;
}

static errcode_t power_display_execute_filter_rules(event_type_t event_type, uint32_t event)
{
    if (g_discard_all_events) {
        return ERRCODE_FAIL;
    }
    if (event_type >= EVENT_TYPE_SIZE) {
        return ERRCODE_FAIL;
    }
    event_filter_t *filter = g_event_filter_map[event_type];
    screen_context_t *sc = power_display_get_screen_context();
    input_action_t ac = KEEP_CURRENT_STATE;
    if (filter == NULL) {
        return ERRCODE_FAIL;
    }
    if (filter->system_rule != NULL) {
        ac = filter->system_rule(sc, event);
    }
    if (ac == KEEP_CURRENT_STATE) {
        return ERRCODE_SUCC;
    }
    power_display_save_gui_event_stamp(ac, event_type);
    return power_display_execute_state_change_action(event_type, ac, event);
}
errcode_t power_display_turnon_handle(uint32_t message_type, uint32_t message_value)
{
    unused(message_type);
    unused(message_value);
    uint32_t keyid = FAKE;
    keyid += KEY_SHORT_PRESS;
    return power_display_execute_filter_rules(GUI_EVENT, keyid);
}

#ifdef SUPPORT_OHOSFWK
void power_display_turnon_handle_ext(void *data)
{
    unused(data);
    errcode_t ret = power_display_turnon_handle(TURN_ON_SCREEN_CMD, 0);
    POWER_DISPLAY_PRINT_DEBUG(0, "display_turnon ret:%u.", ret);
}
#endif

errcode_t power_display_turnoff_handle(uint32_t message_type, uint32_t message_value)
{
    unused(message_type);
    unused(message_value);
    uint32_t keyid = FAKE;
    keyid += COVER_SCREEN;
    return power_display_execute_filter_rules(GUI_EVENT, keyid);
}

#ifdef SUPPORT_OHOSFWK
void power_display_turnoff_handle_ext(void *data)
{
    unused(data);
    errcode_t ret = power_display_turnoff_handle(TURN_OFF_SCREEN_CMD, 0);
    POWER_DISPLAY_PRINT_DEBUG(0, "display_turnoff ret:%u.", ret);
}
#endif

errcode_t power_display_set_screenoff_timeout_handle(uint32_t message_type, uint32_t message_value)
{
    unused(message_type);
    g_screen_context.screen_off_timeout = message_value;
    return power_display_execute_filter_rules(TIMER_EVENT, SET_DEFAULT_TIMEOUT);
}

#ifdef SUPPORT_OHOSFWK
void power_display_set_screenoff_timeout_handle_ext(void *data)
{
    uint32_t *message_value = (uint32_t *)data;
    errcode_t ret = power_display_set_screenoff_timeout_handle(SET_AUTO_OFF_TIMEOUT_CMD, *message_value);
    POWER_DISPLAY_PRINT_DEBUG(0, "display_set_screenoff_timeout ret:%u.", ret);
    free(message_value);
}
#endif

errcode_t power_display_set_screenoff_switch_handle(uint32_t message_type, uint32_t message_value)
{
    unused(message_type);
    g_screen_context.time_out_enable = (bool)message_value;
    if(message_value == 1){
        return power_display_execute_filter_rules(TIMER_EVENT, SET_DEFAULT_TIMEOUT);
    }
    return ERRCODE_SUCC;
}

#ifdef SUPPORT_OHOSFWK
void power_display_set_screenoff_switch_handle_ext(void *data)
{
    uint32_t *message_value = (uint32_t *)data;
    errcode_t ret = power_display_set_screenoff_switch_handle(SET_AUTO_OFF_FUNC_CMD, *message_value);
    POWER_DISPLAY_PRINT_DEBUG(0, "display_set_screenoff_switch ret:%u.", ret);
    free(message_value);
}
#endif

errcode_t power_display_set_keepon_timeout_handle(uint32_t message_type, uint32_t message_value)
{
    unused(message_type);
    g_screen_context.keep_on_timeout = message_value;
    return power_display_execute_filter_rules(TIMER_EVENT, SET_KEEP_ON_TIMEOUT);
}

#ifdef SUPPORT_OHOSFWK
void power_display_set_keepon_timeout_handle_ext(void *data)
{
    uint32_t *message_value = (uint32_t *)data;
    errcode_t ret = power_display_set_keepon_timeout_handle(SET_KEEP_SCREEN_ON_TIMEOUT_CMD, *message_value);
    POWER_DISPLAY_PRINT_DEBUG(0, "display_set_keepon_timeout ret:%u.", ret);
    free(message_value);
}
#endif

errcode_t power_display_set_brightness_handle(uint32_t message_type, uint32_t message_value)
{
    unused(message_type);
    ext_errno ret = uapi_lcd_set_brightness(message_value);
    if (ret != 0) {
        POWER_DISPLAY_PRINT_ERR(0, "power_display_set_brightness fail, ret:%u.", ret);
        return ERRCODE_FAIL;
    }
    g_screen_context.brightness_value = message_value;
    // to do 通知亮度刷新
    return ERRCODE_SUCC;
}

#ifdef SUPPORT_OHOSFWK
void power_display_set_brightness_handle_ext(void *data)
{
    uint32_t *message_value = (uint32_t *)data;
    errcode_t ret = power_display_set_brightness_handle(SET_BRIGHTNESS_CMD, *message_value);
    POWER_DISPLAY_PRINT_DEBUG(0, "display_set_brightness ret:%u.", ret);
    free(message_value);
}
#endif

errcode_t power_display_set_brightness_mode_handle(uint32_t message_type, uint32_t message_value)
{
    unused(message_type);
    unused(message_value);
    return ERRCODE_SUCC;
}

errcode_t power_display_gui_cmd_handle(uint32_t message_type, uint32_t message_value)
{
    unused(message_type);
    return power_display_execute_filter_rules(GUI_EVENT, message_value);
}

#ifdef SUPPORT_OHOSFWK
void power_display_gui_cmd_handle_ext(void *data)
{
    uint32_t *message_value = (uint32_t *)data;
    errcode_t ret = power_display_gui_cmd_handle(GUI_EVENT_CMD, *message_value);
    POWER_DISPLAY_PRINT_DEBUG(0, "display_gui_cmd ret:%u.", ret);
    free(message_value);
}
#endif

errcode_t power_display_other_cmd_handle(uint32_t message_type, uint32_t message_value)
{
    unused(message_type);
    return power_display_execute_filter_rules(OTHER_EVENT, message_value);
}

errcode_t power_display_timer_cmd_handle(uint32_t message_type, uint32_t message_value)
{
    unused(message_type);
    if (message_value == KEEP_ON_TIMER_TIME_OUT) {
        g_screen_context.keep_on_timeout = 0;
    }
    return power_display_execute_filter_rules(TIMER_EVENT, message_value);
}

#ifdef SUPPORT_OHOSFWK
void power_display_timer_cmd_handle_ext(void *data)
{
    uint32_t *message_value = (uint32_t *)data;
    errcode_t ret = power_display_timer_cmd_handle(TIMER_EVENT_CMD, *message_value);
    POWER_DISPLAY_PRINT_DEBUG(0, "display_timer_cm ret:%u.", ret);
    free(message_value);
}
#endif

static const power_display_svr_api_t g_power_display_svr_api = {
    .turn_on_screen = power_display_turn_on_screen,
    .turn_off_screen = power_display_turn_off_screen,
    .get_screen_state = power_display_get_screen_state,
    .set_screen_auto_off_timeout = power_display_set_screen_auto_off_timeout,
    .set_auto_timeout_function = power_display_set_auto_timeout_function,
    .set_screen_set_keepon_timeout = power_display_set_keepon_timeout,
    .set_brightness_mode = power_display_set_brightness_mode,
    .get_brightness_mode = power_display_get_brightness_mode,
    .set_brightness = power_display_set_brightness,
    .get_brightness = power_display_get_brightness,
    .register_filter_rule = power_display_register_filter_rule,
    .register_key_event_cb = power_display_register_key_event_cb,
    .register_touch_event_cb = power_display_register_touch_event_cb,
    .register_rotate_event_cb = power_display_register_rotate_event_cb,
    .screen_timeout_func = power_display_timeout_event_proc,
};

const power_display_svr_api_t *power_display_svr_get_api(void)
{
    return &g_power_display_svr_api;
}

static const message_handle g_power_manager_msg_handler[INVALID_CMD_TYPE] = {
    power_display_turnon_handle,
    power_display_turnoff_handle,
    power_display_set_screenoff_timeout_handle,
    power_display_set_screenoff_switch_handle,
    power_display_set_keepon_timeout_handle,
    power_display_set_brightness_handle,
    power_display_set_brightness_mode_handle,
    power_display_gui_cmd_handle,
    power_display_other_cmd_handle,
    power_display_timer_cmd_handle,
};

static int32_t power_display_msg_proc(uint32_t msg_id, uint8_t *data, uint16_t size)
{
    g_power_recv_msg_num++;
    if (msg_id >= INVALID_CMD_TYPE || data == NULL || size < (sizeof(uint32_t))) {
        POWER_DISPLAY_PRINT_ERR(0, "msg proc fail, msg_id is %u.", msg_id);
        return -1;
    }
    uint32_t event_info = *(uint32_t *)data;
    POWER_DISPLAY_PRINT_DEBUG(0, "power_display_msg_proc recv msg_id:%u,event:%u.", msg_id, event_info);

    if (g_power_manager_msg_handler[msg_id] != NULL) {
        g_power_manager_msg_handler[msg_id](msg_id, event_info);
    }
    return ERRCODE_SUCC;
}

errcode_t power_display_msg_thread(void *data)
{
    uint8_t msg_data[POWER_DISPLAY_MAX_SIZE + POWER_DISPLAY_ID_LEN];
    uint32_t msg_data_size = sizeof(msg_data);
    uint32_t msg_id;

    unused(data);

    while (1) {
        (void)memset_s(msg_data, sizeof(msg_data), 0, sizeof(msg_data));
        osal_msg_queue_read_copy(g_power_display_queue_id, msg_data, &msg_data_size, OSAL_MSGQ_WAIT_FOREVER);
        msg_id = *((uint32_t *)&msg_data[0]);
        power_display_msg_proc(msg_id, &msg_data[POWER_DISPLAY_ID_LEN], POWER_DISPLAY_MAX_SIZE);
    }

    return ERRCODE_SUCC;
}

errcode_t power_display_task_init(void)
{
    errcode_t ret;
    ret = osal_msg_queue_create("power_display", POWER_DISPLAY_QUEUE_MAX_SIZE, &g_power_display_queue_id, 0,
        POWER_DISPLAY_MAX_SIZE + POWER_DISPLAY_ID_LEN);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t uapi_power_display_init(void)
{
    g_event_filter_map[GUI_EVENT] = display_get_gui_filter();
    g_event_filter_map[TIMER_EVENT] = display_get_timer_filter();
    g_event_filter_map[SLICE_EVENT] = display_get_slice_filter();
    ts_register_power_manager_cb(power_display_touch_event_cb);
    button_register_power_display_cb(power_display_key_event_cb);
    return ERRCODE_SUCC;
}

void power_display_gui_enable_aod(bool enable)
{
    g_screen_context.enable_aod = enable;
}
