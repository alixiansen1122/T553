#ifndef __GSENSOR_H__
#define __GSENSOR_H__

#include "stdio.h"
#include "i2c.h"
#include "debug_print.h"
#include "errcode.h"
#include "pinctrl_porting.h"

#define GSENSOR_ENABLE_DEBUG 0
#if GSENSOR_ENABLE_DEBUG
#define GSENSOR_PRINT(fmt, ...) printf("[GSENSOR] " fmt, ##__VA_ARGS__)
#else
#define GSENSOR_PRINT(fmt, ...)
#endif

extern int gsensor_init(void);
extern void gsensor_read_data(int16_t *acc_data, int16_t *gyro_data);
extern int32_t gsensor_calibrate(void);


#endif // __GSENSOR_H__
