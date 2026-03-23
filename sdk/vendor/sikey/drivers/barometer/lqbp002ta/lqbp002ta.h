#ifndef LQBP002TA_H
#define LQBP002TA_H
#include <stdio.h>
#include "i2c.h"
#include "i2c_porting.h"
#include "tcxo.h"

typedef unsigned char uint8;
typedef char int8;
typedef short int int16;
typedef int int32;
typedef unsigned int uint32;

#define HW_ADR 0x76
#define CONTINUOUS_PRESSURE 1
#define CONTINUOUS_TEMPERATURE 2
#define CONTINUOUS_P_AND_T 3
#define PRESSURE_SENSOR 0
#define TEMPERATURE_SENSOR 1
#define LQBP002TA_DEBUG
extern int32_t lqbp002ta_i2c_err;

struct lqbp002ta_calib_param_t
{
    int16 c0;
    int16 c1;
    int32 c00;
    int32 c10;
    int16 c01;
    int16 c11;
    int16 c20;
    int16 c21;
    int16 c30;
    int16 c31;
    int16 c40;
};

struct lqbp002ta_t
{
    struct lqbp002ta_calib_param_t calib_param; /**<calibration data*/
    uint8 chip_id;                              /**<chip id*/
    int32 i32rawPressure;
    int32 i32rawTemperature;
    int32 i32kP;
    int32 i32kT;
};

void lqbp002ta_init(void);
void lqbp002ta_rateset(uint8 iSensor, uint8 u8OverSmpl, uint8 u8SmplRate);
void lqbp002ta_start_temperature(void);
void lqbp002ta_start_pressure(void);
void lqbp002ta_start_continuous(uint8 mode);
void lqbp002ta_get_raw_temp(void);
void lqbp002ta_get_raw_pressure(void);
float lqbp002ta_get_temperature(void);
float lqbp002ta_get_pressure(void);
float lqbp002ta_get_data(void);
float pressure_convert_altitude(float Press,float Ref_P);
void lqbp002ta_soft_reset(void);

#endif
