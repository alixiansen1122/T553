/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Board I2C port\n
 *
 * History: \n
 * 2024-01-05, Create file. \n
 */
#include "i2c.h"
#include "tiot_board_i2c_port_config.h"
#include "tiot_board_i2c_port.h"

#ifndef CONFIG_TIOT_NOT_CTRL_I2C_INIT
static void board_i2c_pin_init(tiot_board_i2c_port *port)
{
    /* I2C 引脚初始化. */
    uapi_pin_set_mode(port->pin[I2C_PIN_TYPE_SCL], port->pinmux[I2C_PIN_TYPE_SCL]);
    uapi_pin_set_mode(port->pin[I2C_PIN_TYPE_SDA], port->pinmux[I2C_PIN_TYPE_SDA]);
}
#endif

int32_t tiot_board_i2c_open(tiot_xmit *xmit, tiot_xmit_callbacks *cb)
{
    tiot_unused(cb);
    tiot_board_i2c_port *port = tiot_board_i2c_get_port((i2c_bus_t)xmit->id);
    if (!port) {
        return -1;
    }
    xmit->handle = port;
    /* I2C 初始化 */
#ifndef CONFIG_TIOT_NOT_CTRL_I2C_INIT
    board_i2c_pin_init(port);
    return uapi_i2c_master_init(xmit->id, port->baudrate, port->hscode);
#else
    return ERRCODE_SUCC;
#endif
}

void tiot_board_i2c_close(tiot_xmit *xmit)
{
#ifndef CONFIG_TIOT_NOT_CTRL_I2C_INIT
    uapi_i2c_deinit(xmit->id);
#else
    tiot_unused(xmit);
#endif
}

int32_t tiot_board_i2c_write(tiot_xmit *xmit, const uint8_t *buff, uint32_t len)
{
    i2c_data_t data;
    data.send_buf = (uint8_t *)buff;
    data.send_len = len;
    tiot_board_i2c_port *port = xmit->handle;
    errcode_t ret = uapi_i2c_master_write(xmit->id, port->dev_addr, &data);
    return (ret == ERRCODE_SUCC ? (int32_t)len : -1);
}

int32_t tiot_board_i2c_read(tiot_xmit *xmit, uint8_t *buff, uint32_t len)
{
    i2c_data_t data;
    data.receive_buf = (uint8_t *)buff;
    data.receive_len = len;
    tiot_board_i2c_port *port = xmit->handle;
    errcode_t ret = uapi_i2c_master_read(xmit->id, port->dev_addr, &data);
    return (ret == ERRCODE_SUCC ? (int32_t)len : -1);
}

int32_t tiot_board_i2c_set_config(tiot_xmit *xmit, tiot_i2c_config *config)
{
    if (!config) {
        return -1;
    }
    uapi_i2c_set_baudrate(xmit->id, config->speed);
    tiot_board_i2c_port *port = xmit->handle;
    port->dev_addr = config->addr;
    return 0;
}