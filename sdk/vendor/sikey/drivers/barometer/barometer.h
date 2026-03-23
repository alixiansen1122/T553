#ifndef __BAROMETER_H__
#define __BAROMETER_H__

#include "stdio.h"
#include "i2c.h"
#include "debug_print.h"
#include "errcode.h"
#include "pinctrl_porting.h"

#define BAROMETER_PRINT(fmt, ...) PRINT("[BAROMETER] " fmt, ##__VA_ARGS__)
int barometer_init(void);
float barometer_read_data(void);

#endif// __BAROMETER_H__