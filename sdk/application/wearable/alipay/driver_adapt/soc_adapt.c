/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay soc adapt.
 * Author:
 * Create:
 */

#include "IIC_Master_Driver.h"
#include "product.h"
#include "i2c.h"
#include "errcode.h"
#include "iotsec.h"
#include "systick.h"
#include "gpio.h"
#include "cmsis_os2.h"

#define ALIPAY_SEC_I2C_MEMPOOL_SIZE 2048
#define ALIPAY_SEC_I2C_SLAVE_ADDR 100

uint8_t g_alipay_i2c_mempool[ALIPAY_SEC_I2C_MEMPOOL_SIZE] = {0};

unsigned char *IIC_Master_Init(void)
{
    errcode_t result;
    result = uapi_i2c_master_init(ALIPAY_SEC_I2C_INDEX, 100000, I2C_SPEED_MODE_SS); /* 100000: 100KHz */
    if (result != ERRCODE_SUCC) {
        MyPrintf("se i2c init fail! ret=0x%x", result);
        return NULL;
    }
    return g_alipay_i2c_mempool;
}

void IIC_Master_Send(unsigned char byAddr, unsigned char *pData, unsigned short wLen)
{
    errcode_t ret;
    uint32_t retry;
    i2c_data_t data;
    unused(byAddr);

    data.send_buf = pData;
    data.send_len = wLen;
    data.receive_buf = NULL;
    data.receive_len = 0;

    for (retry = 0; retry < 3; retry++) {
        ret = uapi_i2c_master_write(ALIPAY_SEC_I2C_INDEX, ALIPAY_SEC_I2C_SLAVE_ADDR, &data);
        if (ret == ERRCODE_SUCC) {
            break;
        }
    }

    if (ret != ERRCODE_SUCC) {
        MyPrintf("se i2c data write fail! ret=0x%x", ret);
    }
    return;
}

void IIC_Master_Receive(unsigned char byAddr, unsigned char *pData, unsigned short wLen)
{
    errcode_t ret;
    uint32_t retry;
    i2c_data_t data;
    unused(byAddr);

    data.send_buf = 0;
    data.send_len = 0;
    data.receive_buf = pData;
    data.receive_len = wLen;

    for (retry = 0; retry < 3; retry++) {
        ret = uapi_i2c_master_read(ALIPAY_SEC_I2C_INDEX, ALIPAY_SEC_I2C_SLAVE_ADDR, &data);
        if (ret == ERRCODE_SUCC) {
            break;
        }
    }

    if (ret != ERRCODE_SUCC) {
        MyPrintf("se i2c data read fail! ret=0x%x", ret);
    }

    return;
}

void Delay_Ms(unsigned char byMilliSec)
{
    osDelay(byMilliSec);
}

void Set_GPIO_State(unsigned char byState)
{
    uapi_gpio_set_val(ALIPAY_SEC_GPIO, byState);
}
