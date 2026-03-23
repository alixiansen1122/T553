/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: gnss board port config \n
 *
 * History: \n
 * 2023-11-15, Create file. \n
 */
#ifndef GNSS_BOARD_PORT_CONFIG_H
#define GNSS_BOARD_PORT_CONFIG_H

#include "gnss_board_port.h"
#include "gpio.h"
#include "uart.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define BIN_FILE_PATH  "/user/gnss/firmware/gnss_config.bin"

#ifdef BOARD_EZM833
static const gnss_board_hw_info g_gnss_board_hw_info = { UART_BUS_0, { S_AGPIO_L32, S_AGPIO_R3, S_AGPIO_R10 } };
#elif defined(SUPPORT_GNSS_FEATURE)
#if !defined(CONFIG_GNSS_HSUART)
static const gnss_board_hw_info g_gnss_board_hw_info = { UART_BUS_0, { S_AGPIO_L8, S_AGPIO_R0, ULP_GPIO0 } };
#else
#ifdef CONFIG_PRODUCT_EVB_DITING
static const gnss_board_hw_info g_gnss_board_hw_info = { UART_BUS_1, { S_AGPIO_R2, S_AGPIO_R0, S_AGPIO_R1  } };
#else
static const gnss_board_hw_info g_gnss_board_hw_info = { UART_BUS_1, { S_AGPIO_L8, S_AGPIO_R0, ULP_GPIO0 } };
#endif
#endif
#else
static const gnss_board_hw_info g_gnss_board_hw_info = { UART_BUS_0, { S_MGPIO10, S_AGPIO_R0, ULP_GPIO0 } };
#endif

static const gnss_board_info g_gnss_board_info = {
#ifdef CONFIG_FILE_BY_ARRAY
    .cfg_path = NULL,
#else
    .cfg_path = BIN_FILE_PATH,
#endif
    .hw_infos = &g_gnss_board_hw_info
};

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
