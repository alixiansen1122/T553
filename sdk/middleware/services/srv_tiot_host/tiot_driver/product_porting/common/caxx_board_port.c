/**
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2025. All rights reserved.
*
* Description: Platform-related Board-Level Interfaces.
*
*/
#include "caxx_board_port.h"
#include "caxx_board_port_config.h"

#include "gpio.h"
#include "pinctrl.h"
#include "pinctrl_porting.h"

int32_t caxx_board_init(void *param)
{
    UNUSED(param);
    const uint32_t *caxx_pins = g_caxx_board_hw_info.pm_info;

    uint32_t power_ctrl_pin = caxx_pins[CAXX_PIN_POWER_CTRL];
    (void)uapi_pin_set_mode((pin_t)power_ctrl_pin, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    (void)uapi_pin_set_pull((pin_t)power_ctrl_pin, PIN_PULL_DOWN);
    (void)uapi_pin_set_ds((pin_t)power_ctrl_pin, (pin_drive_strength_t)(PIN_DS_MAX - 1));
    (void)uapi_gpio_set_dir((pin_t)power_ctrl_pin, GPIO_DIRECTION_OUTPUT);
    (void)uapi_gpio_set_val((pin_t)power_ctrl_pin, GPIO_LEVEL_LOW);

    uint32_t host_wakeup_dev_pin = caxx_pins[CAXX_PIN_HOST_WAKEUP_DEVICE];
    (void)uapi_pin_set_mode((pin_t)host_wakeup_dev_pin, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    (void)uapi_pin_set_pull((pin_t)host_wakeup_dev_pin, PIN_PULL_DOWN);
    (void)uapi_pin_set_ds((pin_t)host_wakeup_dev_pin, (pin_drive_strength_t)(PIN_DS_MAX - 1));
    (void)uapi_gpio_set_dir((pin_t)host_wakeup_dev_pin, GPIO_DIRECTION_OUTPUT);
    (void)uapi_gpio_set_val((pin_t)host_wakeup_dev_pin, GPIO_LEVEL_LOW);

    uint32_t dev_wakeup_host_pin = caxx_pins[CAXX_PIN_DEVICE_WAKEUP_HOST];
    (void)uapi_pin_set_mode((pin_t)dev_wakeup_host_pin, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    (void)uapi_pin_set_pull((pin_t)dev_wakeup_host_pin, PIN_PULL_DOWN);
    (void)uapi_pin_set_ds((pin_t)dev_wakeup_host_pin, (pin_drive_strength_t)(PIN_DS_MAX - 1));
    (void)uapi_gpio_set_dir((pin_t)dev_wakeup_host_pin, GPIO_DIRECTION_INPUT);
    (void)uapi_gpio_set_val((pin_t)dev_wakeup_host_pin, GPIO_LEVEL_LOW);

    return 0;
}

void caxx_board_deinit(void *param)
{
    tiot_unused(param);
    const uint32_t *caxx_pins = g_caxx_board_hw_info.pm_info;
    /* 确保PWREN管脚下拉 */
    (void)uapi_gpio_set_val((pin_t)caxx_pins[CAXX_PIN_POWER_CTRL], GPIO_LEVEL_LOW);
}

const caxx_board_info *caxx_board_get_info(void)
{
    return &g_caxx_board_info;
}