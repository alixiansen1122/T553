/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: TIOT board i2c port config header. \n
 *
 * History: \n
 * 2024-01-02, Create file. \n
 */
#ifndef TIOT_BOARD_I2C_PORT_CONFIG_H
#define TIOT_BOARD_I2C_PORT_CONFIG_H

#include "i2c.h"
#include "gpio.h"
#include "pinctrl.h"
#include "pinctrl_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @brief I2C pin type.
 */
enum tiot_board_i2c_type {
    I2C_PIN_TYPE_SCL,
    I2C_PIN_TYPE_SDA,
    I2C_PIN_TYPE_NUM
};

/**
 * @brief I2C board config.
 */
typedef struct {
    uint32_t pin[I2C_PIN_TYPE_NUM];    // I2C SCL_PIN, SDA_PIN
    uint32_t pinmux[I2C_PIN_TYPE_NUM]; // I2C SCL_PINMUX, SDA_PINMUX
    uint32_t baudrate;  // I2C master init baudrate
    uint32_t dev_addr;  // I2C slave device addr
    uint32_t hscode;    // I2C master hscode
} tiot_board_i2c_port;

static tiot_board_i2c_port g_tiot_board_i2c_port[] = {
    { /* I2C_BUS_ */
        .pin    = { S_MGPIO17, S_MGPIO18 },
        .pinmux = { HAL_PIO_FUNC_I2C3_M1, HAL_PIO_FUNC_I2C3_M1 },
        .baudrate = 400000,
        .dev_addr = 0x28,
        .hscode   = 0,
    }
};

/* 需要根据实际场景适配 */
static inline tiot_board_i2c_port *tiot_board_i2c_get_port(i2c_bus_t bus)
{
    if (bus == I2C_BUS_3) {
        return &g_tiot_board_i2c_port[0];
    }
    return NULL;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* TIOT_BOARD_I2C_PORT_CONFIG_H */