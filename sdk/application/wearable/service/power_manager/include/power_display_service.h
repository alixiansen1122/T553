/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: Power display service header file.
 * Author:
 * Create:
 */

#ifndef POWERMGR_DISPLAY_SVR_H
#define POWERMGR_DISPLAY_SVR_H

#include "stdint.h"
#include "stdbool.h"
#include "errcode.h"
#if !defined(SW_UART_DEBUG) && !defined(SW_RTT_DEBUG)
#include "soc_diag_util.h"
#include "log_oam_logger.h"
#include "log_def.h"
#else
#include "debug_print.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* * 以4个比特存储输入事件类型  */
#define GUI_EVENT_TYPE_BITS_SIZE 4

/* * 输入事件类型掩码 */
#define GUI_EVENT_TYPE_MASK ((uint32_t)(0xF << GUI_EVENT_ID_BITS_SIZE))

/* * 以28个比特存储输入事件ID  */
#define GUI_EVENT_ID_BITS_SIZE 28

/* * 输入事件ID掩码 */
#define GUI_EVENT_ID_MASK (~(uint32_t)(0xF << GUI_EVENT_ID_BITS_SIZE))

/* * 获取对应动作的掩码 */
#define GET_ACTION_MASK(action) (1 << ((uint32_t)(action)))

#if !defined(SW_UART_DEBUG) && !defined(SW_RTT_DEBUG)
#define POWER_DISPLAY_PRINT_ERR(log_num, fmt, arg...) uapi_diag_error_log(0, fmt, ##arg)
#define POWER_DISPLAY_PRINT_WARN(log_num, fmt, arg...) uapi_diag_warning_log(0, fmt, ##arg)
#define POWER_DISPLAY_PRINT_INFO(log_num, fmt, arg...) uapi_diag_info_log(0, fmt, ##arg)
#ifdef WEARABLE_LOG_DEBUG
#define POWER_DISPLAY_PRINT_DEBUG(log_num, fmt, arg...) uapi_diag_debug_log(0, fmt, ##arg)
#else
#define POWER_DISPLAY_PRINT_DEBUG(log_num, fmt, arg...)
#endif
#else
#define POWER_DISPLAY_PRINT_ERR(log_num, fmt, arg...) PRINT("[POWER_MANAGER E/] %s: " fmt "\r\n", __FUNCTION__, ##arg)
#define POWER_DISPLAY_PRINT_WARN(log_num, fmt, arg...) PRINT("[POWER_MANAGER W/] %s: " fmt "\r\n", __FUNCTION__, ##arg)
#define POWER_DISPLAY_PRINT_INFO(log_num, fmt, arg...) PRINT("[POWER_MANAGER I/] %s: " fmt "\r\n", __FUNCTION__, ##arg)
#ifdef WEARABLE_LOG_DEBUG
#define POWER_DISPLAY_PRINT_DEBUG(log_num, fmt, arg...) PRINT("[POWER_MANAGER D/] %s: " fmt "\r\n", __FUNCTION__, ##arg)
#else
#define POWER_DISPLAY_PRINT_DEBUG(log_num, fmt, arg...)
#endif
#endif

/**
 * @brief 屏幕状态
 */
typedef enum screen_state {
    /* * 屏幕处于亮屏状态 */
    SCREEN_ON = 0,
    /* * 屏幕处于灭屏状态 */
    SCREEN_OFF,
    /* * 屏幕处于Ambient状态 */
    SCREEN_AMBIENT,
    /* * 屏幕状态Size */
    SCREEN_STATE_SIZE,
} screen_state_t;

/**
 * @brief 状态切换动作定义.
 */
typedef enum input_action {
    /* * 保持当前状态不变,如果不需要改变其他规则决定的屏幕状态，则返回此值 */
    KEEP_CURRENT_STATE = 0,
    /* * 亮屏 */
    TURN_ON_SCREEN,
    /* * 灭屏 */
    TURN_OFF_SCREEN,
    /* * 进入Ambient模式 */
    ENTER_AMBIENT,
    /* * 重新计时,用于按照当前超时时间延长亮屏时间 */
    RESET_TIMER,
} input_action_t;

/**
 * @brief 状态进入需要执行的原子动作,一次可以有多个动作
 */
typedef enum state_change_action {
    /* * 上电LCD */
    SET_DISPLAY_ON = 0,
    /* * 下电LCD */
    SET_DISPLAY_OFF,
    /* * 打开TE */
    SET_TE_ON,
    /* * 关闭TE */
    SET_TE_OFF,
    /* * 进入IDLE模式 */
    SET_DISPLAY_TO_IDLE_MODE,
    /* * 退出IDLE模式 */
    SET_DISPLAY_TO_NORMAL_MODE,
    /* * 设置TP为正常工作模式 */
    SET_TP_TO_NORMAL_WORK_MODE,
    /* * 设置TP为不报点模式 */
    SET_TP_TO_SLEEP_MODE,
    /* * 设置TP为只上报PRESS、RELEASE模式 */
    SET_TP_TO_STANDBY_MODE,
    /* * 启动或重启定时器 */
    START_TIMER,
    /* * 停止定时器 */
    STOP_TIMER,
} state_change_action_t;

typedef enum brightness_mode {
    /* * 手动模式, 用户设置亮度 */
    MANUAL = 0,
    /* * 自动模式, 屏幕根据环境光自动调节亮度 */
    AUTO,
} brightness_mode_t;

/**
 * @brief 过滤器类型
 */
typedef enum event_type {
    /* * 应用事件，比如应用退出、进入事件 */
    SLICE_EVENT = 0,
    /* * GUI引擎事件，比如点击、滑动、长按,包含手势 */
    GUI_EVENT,
    /* * 定时器消息,超时定时器发消息灭屏 */
    TIMER_EVENT,
    /* * 产品自定义实现的事件勿扰场景下来电和消息 */
    OTHER_EVENT,
    /* * 事件类型Size */
    EVENT_TYPE_SIZE,
} event_type_t;

/**
 * @brief GUI引擎事件类型，Touch\按键\手势枚举值按照分段使用，
 */
typedef enum gui_event_type {
    /* * Touch */
    TOUCH = (0 << GUI_EVENT_ID_BITS_SIZE),
    /* * 按键 */
    KEY = (1 << GUI_EVENT_ID_BITS_SIZE),
    /* * 手势 */
    GESTURE = (2 << GUI_EVENT_ID_BITS_SIZE),
    /* * 模拟输入动作 */
    FAKE = (3 << GUI_EVENT_ID_BITS_SIZE),
} gui_event_type_t;

/**
 * @brief 模拟输入事件
 */
typedef enum fake_input_event_type {
    /* * 模拟按键亮屏 */
    KEY_SHORT_PRESS = 0,
    /* * 模拟盖屏灭屏 */
    COVER_SCREEN,
} fake_input_event_type_t;

/**
 * @brief  屏幕管理错误码。
 * @endif
 */
typedef enum {
    PD_SUCC = 0,
} power_display_err_t;

/**
 * @brief Power Manager service 对外消息类型
 */
typedef enum message_type {
    TURN_ON_SCREEN_CMD = 0,
    TURN_OFF_SCREEN_CMD,
    SET_AUTO_OFF_TIMEOUT_CMD,
    SET_AUTO_OFF_FUNC_CMD,
    SET_KEEP_SCREEN_ON_TIMEOUT_CMD,
    SET_BRIGHTNESS_CMD,
    SET_BRIGHTNESS_MODE_CMD,
    GUI_EVENT_CMD,
    OTHER_EVENT_CMD,
    TIMER_EVENT_CMD,
    INVALID_CMD_TYPE,
} message_type_t;

/**
 * @brief 定时器操作类型
 */
typedef enum timer_event_type {
    /* * 常亮灭屏定时器超时 */
    KEEP_ON_TIMER_TIME_OUT = 0,
    /* * 应用灭屏定时器超时 */
    SLICE_TIMER_TIME_OUT,
    /* * 系统默认灭屏定时器超时 */
    DEFAULT_TIMER_TIME_OUT,
    /* * 应用设置系统默认超时时间 */
    SET_DEFAULT_TIMEOUT,
    /* * 应用设置超时时间 */
    SET_SLICE_TIMEOUT,
    /* * 应用设置常亮屏 */
    SET_KEEP_ON_TIMEOUT,

} timer_event_type_t;

/**
 * @brief 屏幕管理上下文,记录关键配置信息,传递给过滤器使用
 */
typedef struct screen_context {
    uint32_t screen_off_timeout;
    uint32_t keep_on_timeout;
    screen_state_t current_state;
    brightness_mode_t brightness_mode;
    uint8_t brightness_value;
    bool time_out_enable;
    uint32_t last_gui_event_stamp;
    bool press_state;
    uint32_t screen_on_time_interval;
    uint32_t screen_off_time_interval;
    bool enable_aod;
} screen_context_t;

/**
 * @brief 输入事件回调函数
 */
typedef void (*event_callback_func)(uint32_t event, uint32_t event_value);

/**
 * @brief 过滤器动作执行函数,根据当前屏幕状态以及事件,返回对屏幕状态的控制操作.
 *
 * @param current 当前屏幕状态 @see ScreenState.
 * @param event 当前事件类型,每个过滤器有自己的定义.
 * @return InputAction 屏幕状态转换动作, @see InputAction.
 */
typedef input_action_t (*execute_rule)(screen_context_t *current, uint32_t event);

/*
 * 屏幕电源管理控制服务对外接口
 */
typedef struct power_display_svr_api {
    /* *
     * @brief 控制屏幕进入亮屏状态.
     *
     * @return 返回 ERRCODE_SUCC 表示亮屏操作成功; 否则返回错误原因.
     */
    errcode_t (*turn_on_screen)(void);

    /* *
     * @brief 控制屏幕进入黑屏状态.
     *
     * @return 返回 ERRCODE_SUCC 表示灭屏操作成功; 否则返回错误原因.
     */
    errcode_t (*turn_off_screen)(void);

    /* *
     * @brief 获取当前屏幕状态.
     *
     * @return 返回screen_state_t @see screen_state_t.
     */
    screen_state_t (*get_screen_state)(void);

    /* *
     * @brief 应用设置屏幕超时时间.
     *
     * @param timeout 无操作情况下亮屏自动进入其他状态的超时时间,单位ms.
     * @return 返回 ERRCODE_SUCC 表示操作成功; 否则返回错误原因.
     */
    errcode_t (*set_screen_auto_off_timeout)(uint32_t timeout);

    /* *
     * @brief 控制自动亮、灭屏控制功能是否生效
     *
     * 关闭自动亮灭屏功能后：
     * PowerManager服务不再处理按键、TP等输入事件触发亮、灭屏.
     * 不再支持超时时间设置.
     * 应用主动调用TurnOnScreen亮屏 @see TurnOnScreen.
     * 应用主动调用TurnOffScreen灭屏 @see TurnOffScreen.
     *
     * @param enable 是否开启自动亮、灭屏功能
     * @return 返回 errcode_t 表示操作成功; 否则返回错误原因.
     */
    errcode_t (*set_auto_timeout_function)(bool enable);

    /* *
     * @brief 应用设置亮屏时间.
     *
     * @param time 保持亮屏时间，此时误操作情况下不会灭屏.
     * @return 返回 ERRCODE_SUCC 表示操作成功; 否则返回错误原因.
     */
    errcode_t (*set_screen_set_keepon_timeout)(uint32_t time);
    /* *
     * @brief 设置屏幕亮度调整模式
     *
     * @param mode 屏幕亮度调整模式.@see brightness_mode_t
     * @return 返回 ERRCODE_SUCC 表示操作成功; 否则返回错误原因.
     */
    errcode_t (*set_brightness_mode)(brightness_mode_t mode);

    /* *
     * @brief 获取屏幕亮度调整模式.
     *
     * @return 屏幕亮度调整模式. @see brightness_mode_t.
     */
    brightness_mode_t (*get_brightness_mode)(void);

    /* *
     * @brief 应用设置自定义亮度值.
     *
     * 应用设置的亮度值会覆盖系统默认亮度设置.
     *
     * @param brightness 屏幕亮度值.
     * @return 返回 ERRCODE_SUCC 表示操作成功; 否则返回错误原因.
     */
    errcode_t (*set_brightness)(uint8_t brightness);

    /* *
     * @brief 获取应用设置屏幕亮度值.
     *
     * @return 应用设置的屏幕亮度值.
     */
    uint8_t (*get_brightness)(void);

    /* *
     * @brief 用户注册自定义的显示控制规则.
     *
     * 当GUI引擎事件过滤器和手势事件过滤器中的规则无法满足需求时\n
     * 用户可以注册自定义的规则来决定GUI引擎事件以及手势事件如何处理.
     * 如低电场景抬腕不亮屏通过向手势事件过滤器添加自定义规则实现.
     * 只支持添加一个.
     *
     * @param type 过滤器类型,当前仅支持对GUI引擎事件过滤器和手势事件过滤器插入规则.
     * @param EventFilterRule 过滤器规则.
     * @return 返回 ERRCODE_SUCC 表示操作成功; 否则返回错误原因.
     */
    errcode_t (*register_filter_rule)(event_type_t type, execute_rule rule_func);

    /* *
     * @brief 注册按键事件回调函数
     *
     * @param callbackFunc 事件回调函数
     * @return 返回 ERRCODE_SUCC 表示操作成功; 否则返回错误原因.
     */
    errcode_t (*register_key_event_cb)(event_callback_func callbackFunc);

    /* *
     * @brief 注册Touch事件回调函数
     *
     * @param callbackFunc 事件回调函数
     * @return 返回 ERRCODE_SUCC 表示操作成功; 否则返回错误原因.
     */
    errcode_t (*register_touch_event_cb)(event_callback_func callbackFunc);

    /* *
     * @brief 注册Rotate事件回调函数
     *
     * @param callbackFunc 事件回调函数
     * @return 返回 ERRCODE_SUCC 表示操作成功; 否则返回错误原因.
     */
    errcode_t (*register_rotate_event_cb)(event_callback_func callbackFunc);

    /* *
     * @brief 定时器超时处理函数
     *
     * @param 超时事件
     * @return 返回 ERRCODE_SUCC 表示操作成功; 否则返回错误原因.
     */
    errcode_t (*screen_timeout_func)(uint32_t event);
} power_display_svr_api_t;

/**
 * @brief 获取显示功耗管理服务API接口.
 *
 * @return 显示功耗管理服务API接口.
 */
const power_display_svr_api_t *power_display_svr_get_api(void);

/**
 * @brief 显示功耗管理服务消息处理线程.
 *
 * @return 返回 ERRCODE_SUCC 表示操作成功; 否则返回错误原因.
 */
errcode_t power_display_msg_thread(void *data);

/**
 * @brief  显示功耗管理服务任务初始化接口.
 *
 * @return 返回 ERRCODE_SUCC 表示操作成功; 否则返回错误原因.
 */
errcode_t power_display_task_init(void);

/**
 * @brief 获取显示功耗管理服务API接口.
 *
 * @return 屏幕信息上下文.
 */
screen_context_t *power_display_get_screen_context(void);

void power_display_gui_enable_aod(bool enable);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif