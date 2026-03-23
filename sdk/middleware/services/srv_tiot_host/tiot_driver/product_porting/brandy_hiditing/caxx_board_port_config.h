/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: CAXX board port config header \n
 *
 * History: \n
 * 2024-01-03, Create file. \n
 */
#ifndef CAXX_BOARD_PORT_CONFIG_H
#define CAXX_BOARD_PORT_CONFIG_H

#include "caxx_board_port.h"
#include "gpio.h"
#include "i2c.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifndef CONFIG_FILE_BY_ARRAY
#define CAXX_FW_CFG_PATH  "/user/nfc/fw_img/fw.img"
#else
#define CAXX_FW_CFG_PATH  NULL
#endif

static caxx_board_hw_info g_caxx_board_hw_info = { I2C_BUS_3, { S_AGPIO_L32, S_AGPIO_L30, S_AGPIO_L31 } };
static caxx_board_info g_caxx_board_info = {
    .cfg_path = CAXX_FW_CFG_PATH,
    .hw_infos = &g_caxx_board_hw_info
};

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* CAXX_BOARD_PORT_CONFIG_H */