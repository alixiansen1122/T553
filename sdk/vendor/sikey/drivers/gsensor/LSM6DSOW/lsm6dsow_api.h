#ifndef __LSM6DSOW_API_H__
#define __LSM6DSOW_API_H__

#include "stdio.h"
#include "i2c.h"
#include "debug_print.h"
#include "errcode.h"
#include "pinctrl_porting.h"

#define LSM6DSO_I2C_BUS                               I2C_BUS_1
#define LSM6DSO_DEVICE_ID                             0x6C

extern int lsm6dso_init(void);
extern void lsm6dso_read_sensor_data(float *acc_data, float *gyro_data);
extern bool g_sleep_flag;
#endif// __LSM6DSOW_API_H__
