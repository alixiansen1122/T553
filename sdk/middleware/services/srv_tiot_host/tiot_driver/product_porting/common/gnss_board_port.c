/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description:  \n
 *
 * History: \n
 * 2023-06-13, Create file. \n
 */

#include "gnss_board_port.h"
#include "gpio.h"
#include "uart.h"
#include "pinctrl.h"
#include "soc_osal.h"
#include "gnss_board_port_config.h"

int32_t gnss_board_init(void *param)
{
    tiot_unused(param);
    const uint32_t *gnss_pins = g_gnss_board_hw_info.pm_info;
    /* power en */
    uint32_t power_ctrl_pin = gnss_pins[GNSS_PIN_POWER_CTRL];
    (void)uapi_pin_set_mode((pin_t)power_ctrl_pin, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    (void)uapi_pin_set_pull((pin_t)power_ctrl_pin, PIN_PULL_DOWN);
    (void)uapi_gpio_set_val((pin_t)power_ctrl_pin, GPIO_LEVEL_LOW);
    (void)uapi_gpio_set_dir((pin_t)power_ctrl_pin, GPIO_DIRECTION_OUTPUT);
    (void)uapi_pin_set_ds((pin_t)power_ctrl_pin, (pin_drive_strength_t)(PIN_DS_MAX - 1));

    /* host wakeup dev */
    uint32_t host_wakeup_device_pin = gnss_pins[GNSS_PIN_HOST_WAKEUP_DEVICE];
    if (host_wakeup_device_pin != TIOT_PIN_NONE) {
        (void)uapi_pin_set_mode((pin_t)host_wakeup_device_pin, (pin_mode_t)HAL_PIO_FUNC_GPIO);
        (void)uapi_pin_set_ds((pin_t)host_wakeup_device_pin, (pin_drive_strength_t)(PIN_DS_MAX - 1));
        (void)uapi_pin_set_pull((pin_t)host_wakeup_device_pin, PIN_PULL_DOWN);
        (void)uapi_gpio_set_val((pin_t)host_wakeup_device_pin, GPIO_LEVEL_LOW);
        (void)uapi_gpio_set_dir((pin_t)host_wakeup_device_pin, GPIO_DIRECTION_OUTPUT);
    }

    /* device wakeup host */
    uint32_t device_wakeup_host_pin = gnss_pins[GNSS_PIN_DEVICE_WAKEUP_HOST];
    if (device_wakeup_host_pin != TIOT_PIN_NONE) {
        (void)uapi_pin_set_mode((pin_t)device_wakeup_host_pin, (pin_mode_t)HAL_PIO_FUNC_GPIO);
        (void)uapi_pin_set_pull((pin_t)device_wakeup_host_pin, PIN_PULL_DOWN);
        (void)uapi_gpio_set_val((pin_t)device_wakeup_host_pin, GPIO_LEVEL_LOW);
        (void)uapi_gpio_set_dir((pin_t)device_wakeup_host_pin, GPIO_DIRECTION_INPUT);
    }
    /* UART pinmux已经在板级完成初始化, 或在UART open时进行初始化。 */
    return 0;
}

void gnss_board_deinit(void *param)
{
    tiot_unused(param);
    const uint32_t *gnss_pins = g_gnss_board_hw_info.pm_info;
    /* 确保PWREN管脚下拉 */
    (void)uapi_gpio_set_val((pin_t)gnss_pins[GNSS_PIN_POWER_CTRL], GPIO_LEVEL_LOW);
}

const gnss_board_info *gnss_board_get_info(void)
{
    return &g_gnss_board_info;
}
