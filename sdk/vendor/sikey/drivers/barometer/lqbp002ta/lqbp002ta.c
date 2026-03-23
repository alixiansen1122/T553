/*******************************************************************************
 * Copyright (C), 2000-2016,  Electronic Technology Co., Ltd.
 * FileName: LQBP002TA.C
 * Author:
 * Version: V 1.0 2024-1-12
 * Date:    V 1.0 2024-1-12
 * Description:Define the operation interface of LQBP002TA
 * History:
 * Others:
 *******************************************************************************/
#ifndef LQBP002TA_C
#define LQBP002TA_C
#include "barometer/barometer.h"
#include "lqbp002ta.h"
#include "sk_board.h"
static struct lqbp002ta_t lqbp002ta;
static struct lqbp002ta_t *p_lqbp002ta;

void lqbp002ta_write(uint8 hwadr, uint8 regadr, uint8 val);
uint8 lqbp002ta_read(uint8 hwadr, uint8 regadr);
void lqbp002ta_get_calib_param(void);

int32_t lqbp002ta_i2c_err = 0;
/*****************************************************************************
 Function: lqbp002ta_write
 Description: this function will write data to specofic register through software I2C bus
 Input:  uint8 hwadr   hardware I2C address
         uint8 regadr  register address
         uint8 val     write-in value
 Output:
 Return:
 Calls:
 Called By:
*****************************************************************************/
void lqbp002ta_write(uint8 hwadr, uint8 regaddr, uint8 val)
{
    int32_t ret = 0;
    i2c_data_t data = {0};
    uint8_t send_buf[2];

    // 准备要发送的数据
    send_buf[0] = regaddr;
    send_buf[1] = val;

    data.send_buf = send_buf;
    data.send_len = 2;
    data.receive_buf = NULL;
    data.receive_len = 0;

    ret = uapi_i2c_master_write(I2C_BUS_1, hwadr, &data);
    if (ret != 0)
    {
        BAROMETER_PRINT("lqbp002ta_write reg 0x%x failed! ret = %p\n", regaddr, ret);
        lqbp002ta_i2c_err = -1;
        return;
    }
    lqbp002ta_i2c_err = 0;
}

/*****************************************************************************
 Function: lqbp002ta_read
 Description: this function will read register data through software I2C bus
 Input: uint8 hwadr   hardware I2C address
        uint8 regadr  register address
 Output:
 Return: uint8 readout value
 Calls:
 Called By:
*****************************************************************************/
uint8 lqbp002ta_read(uint8 hwadr, uint8 regaddr)
{
    i2c_data_t data = {0};
    int32_t ret = 0;
    uint8_t val = 0;

    data.send_buf = &regaddr;
    data.send_len = 1; /* 1：reg data len */
    data.receive_buf = &val;
    data.receive_len = 1;

    ret = uapi_i2c_master_writeread(I2C_BUS_1, hwadr, &data);
    if (ret != 0)
    {
        BAROMETER_PRINT("lqbp002ta_read reg 0x%x failed! ret = %p\n", regaddr, ret);
        lqbp002ta_i2c_err = -1;
        return 0;
    }
    lqbp002ta_i2c_err = 0;
    return val;
}

void read_chip_id(void)
{
    p_lqbp002ta->chip_id = lqbp002ta_read(HW_ADR, 0x0D);
    BAROMETER_PRINT("check chip id =0x%x\n", p_lqbp002ta->chip_id);
    if (p_lqbp002ta->chip_id == 0x11)
    set_device_state(DEVICE_BAROMETER);
}

uint8 is_lqbp002ta(void)
{
    if (p_lqbp002ta->chip_id == 0x11)
        return 1;

    else
        return 0;
}

/*****************************************************************************
 Function: lqbp002ta_init
 Description: initialization
 Input: void
 Output:
 Return: void
 Calls:
 Called By:
*****************************************************************************/
void lqbp002ta_init(void)
{

    float fSplTemp;
	float fSplPressure;
    p_lqbp002ta = &lqbp002ta; /* read Chip Id */
    p_lqbp002ta->i32rawPressure = 0;
    p_lqbp002ta->i32rawTemperature = 0;
    // p_lqbp002ta->chip_id = 0x11;
    read_chip_id();
    lqbp002ta_get_calib_param();
    // sampling rate = 1Hz; Pressure oversample = 2;
    lqbp002ta_rateset(PRESSURE_SENSOR, 4, 8);
    // sampling rate = 1Hz; Temperature oversample = 2;
    lqbp002ta_rateset(TEMPERATURE_SENSOR, 2, 4);
    lqbp002ta_start_continuous(CONTINUOUS_P_AND_T);
    // Start background measurement


    lqbp002ta_get_raw_temp();
	lqbp002ta_get_raw_pressure();
    fSplTemp = lqbp002ta_get_temperature();
	fSplPressure = lqbp002ta_get_pressure();

    #ifdef LQBP002TA_DEBUG
        printf("TA:0x00:%02x H\n",lqbp002ta_read(HW_ADR, 0x00));
        printf("TA:0x01:%02x H\n",lqbp002ta_read(HW_ADR, 0x01));
        printf("TA:0x02:%02x H\n",lqbp002ta_read(HW_ADR, 0x02));
        printf("TA:0x03:%02x H\n",lqbp002ta_read(HW_ADR, 0x03));
        printf("TA:0x04:%02x H\n",lqbp002ta_read(HW_ADR, 0x04));
        printf("TA:0x05:%02x H\n",lqbp002ta_read(HW_ADR, 0x05));
		printf("TA:0x06:%02x H\n",lqbp002ta_read(HW_ADR, 0x06));
		printf("TA:0x07:%02x H\n",lqbp002ta_read(HW_ADR, 0x07));
		printf("TA:0x08:%02x H\n",lqbp002ta_read(HW_ADR, 0x08));
		printf("TA:0x09:%02x H\n",lqbp002ta_read(HW_ADR, 0x09));
		printf("TA:0x0A:%02x H\n",lqbp002ta_read(HW_ADR, 0x0A));
		printf("TA:0x0B:%02x H\n",lqbp002ta_read(HW_ADR, 0x0B));
		printf("TA:0x0C:%02x H\n",lqbp002ta_read(HW_ADR, 0x0C));
		printf("TA:0x0D:%02x H\n",lqbp002ta_read(HW_ADR, 0x0D));

		printf("TA:temp_raw:%f\n",p_lqbp002ta->i32rawTemperature);
		printf("TA:press_raw:%f\n",p_lqbp002ta->i32rawPressure);

     #endif

		printf( "TA:T: %f C  P: %f Pa\n",fSplTemp,fSplPressure);
		printf( "TA:altitude: %f m  ---------\n\r",pressure_convert_altitude(fSplPressure,101325));
}

/*****************************************************************************
 Function: lqbp002ta_rateset
 Description: set sample rate and over sample rate per second for specific sensor
 Input:     uint8 u8OverSmpl  oversample rate         Maximal = 128
            uint8 u8SmplRate  sample rate(Hz) Maximal = 128
            uint8 iSensor     0: Pressure; 1: Temperature
 Output:
 Return: void
 Calls:
 Called By:
*****************************************************************************/
void lqbp002ta_rateset(uint8 iSensor, uint8 u8SmplRate, uint8 u8OverSmpl)
{
    uint8 reg = 0;
    int32 i32kPkT = 0;
    switch (u8SmplRate)
    {
    case 2:
        reg |= (1 << 4);
        break;
    case 4:
        reg |= (2 << 4);
        break;
    case 8:
        reg |= (3 << 4);
        break;
    case 16:
        reg |= (4 << 4);
        break;
    case 32:
        reg |= (5 << 4);
        break;
    case 64:
        reg |= (6 << 4);
        break;
    case 128:
        reg |= (7 << 4);
        break;
    case 1:
    default:
        break;
    }
    switch (u8OverSmpl)
    {
    case 2:
        reg |= 1;
        i32kPkT = 1572864;
        break;
    case 4:
        reg |= 2;
        i32kPkT = 3670016;
        break;
    case 8:
        reg |= 3;
        i32kPkT = 7864320;
        break;
    case 16:
        i32kPkT = 253952;
        reg |= 4;
        break;
    case 32:
        i32kPkT = 516096;
        reg |= 5;
        break;
    case 64:
        i32kPkT = 1040384;
        reg |= 6;
        break;
    case 128:
        i32kPkT = 2088960;
        reg |= 7;
        break;
    case 1:
    default:
        i32kPkT = 524288;
        break;
    }

    if (iSensor == PRESSURE_SENSOR)
    {
        p_lqbp002ta->i32kP = i32kPkT;
        lqbp002ta_write(HW_ADR, 0x06, reg);
        if (u8OverSmpl > 8)
        {
            reg = lqbp002ta_read(HW_ADR, 0x09);
            lqbp002ta_write(HW_ADR, 0x09, reg | 0x04);
        }
        else
        {
            reg = lqbp002ta_read(HW_ADR, 0x09);
            lqbp002ta_write(HW_ADR, 0x09, reg & (~0x04));
        }
    }
    if (iSensor == TEMPERATURE_SENSOR)
    {
        p_lqbp002ta->i32kT = i32kPkT;

        if (is_lqbp002ta())
            lqbp002ta_write(HW_ADR, 0x07, reg);
        else
            lqbp002ta_write(HW_ADR, 0x07, reg | 0x80); // Using mems temperature

        if (u8OverSmpl > 8)
        {
            reg = lqbp002ta_read(HW_ADR, 0x09);
            lqbp002ta_write(HW_ADR, 0x09, reg | 0x08);
        }
        else
        {
            reg = lqbp002ta_read(HW_ADR, 0x09);
            lqbp002ta_write(HW_ADR, 0x09, reg & (~0x08));
        }
    }

#ifdef LQBP002TA_DEBUG
		printf("TA:tmp_osr_scale_coeff:%d\n",p_lqbp002ta->i32kT);
		printf("TA:prs_osr_scale_coeff:%d\n",p_lqbp002ta->i32kP);
#endif

}

/*****************************************************************************
 Function: lqbp002ta_get_calib_param
 Description: obtain the calibrated coefficient
 Input: void
 Output:
 Return: void
 Calls:
 Called By:
*****************************************************************************/
void lqbp002ta_get_calib_param(void)
{
    uint8 h;
    uint8 m;
    uint8 l;
    h = lqbp002ta_read(HW_ADR, 0x10);
    l = lqbp002ta_read(HW_ADR, 0x11);
    p_lqbp002ta->calib_param.c0 = (int16)h << 4 | l >> 4;
    p_lqbp002ta->calib_param.c0 = (p_lqbp002ta->calib_param.c0 & 0x0800) ? (0xF000 | p_lqbp002ta->calib_param.c0) : p_lqbp002ta->calib_param.c0;
    h = lqbp002ta_read(HW_ADR, 0x11);
    l = lqbp002ta_read(HW_ADR, 0x12);
    p_lqbp002ta->calib_param.c1 = (int16)(h & 0x0F) << 8 | l;
    p_lqbp002ta->calib_param.c1 = (p_lqbp002ta->calib_param.c1 & 0x0800) ? (0xF000 | p_lqbp002ta->calib_param.c1) : p_lqbp002ta->calib_param.c1;
    h = lqbp002ta_read(HW_ADR, 0x13);
    m = lqbp002ta_read(HW_ADR, 0x14);
    l = lqbp002ta_read(HW_ADR, 0x15);
    p_lqbp002ta->calib_param.c00 = (int32)h << 12 | (int32)m << 4 | (int32)l >> 4;
    p_lqbp002ta->calib_param.c00 = (p_lqbp002ta->calib_param.c00 & 0x080000) ? (0xFFF00000 | p_lqbp002ta->calib_param.c00) : p_lqbp002ta->calib_param.c00;
    h = lqbp002ta_read(HW_ADR, 0x15);
    m = lqbp002ta_read(HW_ADR, 0x16);
    l = lqbp002ta_read(HW_ADR, 0x17);
    p_lqbp002ta->calib_param.c10 = (int32)(h & 0x0F) << 16 | (int32)m << 8 | l;
    p_lqbp002ta->calib_param.c10 = (p_lqbp002ta->calib_param.c10 & 0x080000) ? (0xFFF00000 | p_lqbp002ta->calib_param.c10) : p_lqbp002ta->calib_param.c10;
    h = lqbp002ta_read(HW_ADR, 0x18);
    l = lqbp002ta_read(HW_ADR, 0x19);
    p_lqbp002ta->calib_param.c01 = (int16)h << 8 | l;
    h = lqbp002ta_read(HW_ADR, 0x1A);
    l = lqbp002ta_read(HW_ADR, 0x1B);
    p_lqbp002ta->calib_param.c11 = (int16)h << 8 | l;
    h = lqbp002ta_read(HW_ADR, 0x1C);
    l = lqbp002ta_read(HW_ADR, 0x1D);
    p_lqbp002ta->calib_param.c20 = (int16)h << 8 | l;
    h = lqbp002ta_read(HW_ADR, 0x1E);
    l = lqbp002ta_read(HW_ADR, 0x1F);
    p_lqbp002ta->calib_param.c21 = (int16)h << 8 | l;
    h = lqbp002ta_read(HW_ADR, 0x20);
    l = lqbp002ta_read(HW_ADR, 0x21);
    p_lqbp002ta->calib_param.c30 = (int16)h << 8 | l;
    if (is_lqbp002ta())
    {
        h = lqbp002ta_read(HW_ADR, 0x22);
        l = lqbp002ta_read(HW_ADR, 0x23);
        p_lqbp002ta->calib_param.c31 = (int16)h << 4 | l >> 4;
        p_lqbp002ta->calib_param.c31 = (p_lqbp002ta->calib_param.c31 & 0x0800) ? (0xF000 | p_lqbp002ta->calib_param.c31) : p_lqbp002ta->calib_param.c31;
        h = lqbp002ta_read(HW_ADR, 0x23);
        l = lqbp002ta_read(HW_ADR, 0x24);
        p_lqbp002ta->calib_param.c40 = (int16)(h & 0x0F) << 8 | l;
        p_lqbp002ta->calib_param.c40 = (p_lqbp002ta->calib_param.c40 & 0x0800) ? (0xF000 | p_lqbp002ta->calib_param.c40) : p_lqbp002ta->calib_param.c40;
    }
#ifdef LQBP002TA_DEBUG

	printf("TA:C0:%d\n",p_lqbp002ta->calib_param.c0);
	printf("TA:C1:%d\n",p_lqbp002ta->calib_param.c1);
	printf("TA:C00:%d\n",p_lqbp002ta->calib_param.c00);
	printf("TA:C10:%d\n",p_lqbp002ta->calib_param.c10);
	printf("TA:C01:%d\n",p_lqbp002ta->calib_param.c01);
	printf("TA:C11:%d\n",p_lqbp002ta->calib_param.c11);
	printf("TA:C20:%d\n",p_lqbp002ta->calib_param.c20);
	printf("TA:C21:%d\n",p_lqbp002ta->calib_param.c21);
	printf("TA:C30:%d\n",p_lqbp002ta->calib_param.c30);
	printf("TA:C31:%d\n",p_lqbp002ta->calib_param.c31);
	printf("TA:C40:%d\n",p_lqbp002ta->calib_param.c40);

#endif
}

/*****************************************************************************
 Function: lqbp002ta_start_temperature
 Description: start one measurement for temperature
 Input: void
 Output:
 Return: void
 Calls:
 Called By:
*****************************************************************************/
void lqbp002ta_start_temperature(void)
{
    lqbp002ta_write(HW_ADR, 0x08, 0x02);
}

/*****************************************************************************
 Function: lqbp002ta_start_pressure
 Description: start one measurement for pressure
 Input: void
 Output:
 Return: void
 Calls:
 Called By:
*****************************************************************************/

void lqbp002ta_start_pressure(void)
{
    lqbp002ta_write(HW_ADR, 0x08, 0x01);
}
/*****************************************************************************
 Function: lqbp002ta_start_continuous
 Description: Select mode for the continuously measurement
 Input: uint8 mode  1: pressure; 2: temperature; 3: pressure and temperature
 Output:
 Return: void
 Calls:
 Called By:
*****************************************************************************/
void lqbp002ta_start_continuous(uint8 mode)
{
    lqbp002ta_write(HW_ADR, 0x08, mode + 4);
}

void lqbp002ta_stop(void)
{
    lqbp002ta_write(HW_ADR, 0x08, 0);
}

/*****************************************************************************
 Function: lqbp002ta_soft_reset
 Description: Pressure sensor soft_reset
 Input:
 Output:
 Return: void
 Calls:
 Called By:
 Noted that:After soft reset the pressure sensor need to reinitialize
 注意：软复位之后，建议延时20ms之后，重新初始化，读取校准系数等。
*****************************************************************************/
void lqbp002ta_soft_reset(void)
{
    lqbp002ta_write(HW_ADR, 0x0C, 0x89);
}

/*****************************************************************************
 Function: lqbp002ta_get_raw_temp
 Description:obtain the original temperature value and turn them into 32bits-integer
 Input: void
 Output:
 Return: void
 Calls:
 Called By:
*****************************************************************************/
void lqbp002ta_get_raw_temp(void)
{
    uint8 reg_addr[3]={0x03,0x04,0x05};
    uint8 rx_data[3];
    int ret;
    i2c_data_t data = {0};
    for(int i=0;i<3;i++)
    {
    data.send_buf = &reg_addr[i];
    data.send_len = 1;
    data.receive_buf = &rx_data[i];
    data.receive_len = 1;

    ret = uapi_i2c_master_writeread(I2C_BUS_1, HW_ADR, &data);

    if (ret != 0)
    {
        BAROMETER_PRINT("lqbp002ta_get_raw_temp reg 0x%x failed, ret = %p\n", reg_addr, ret);
        lqbp002ta_i2c_err = -1;
        //return;
    }
    lqbp002ta_i2c_err = 0;
     }
    p_lqbp002ta->i32rawTemperature = ((int32_t)rx_data[0] << 16) |
                                     ((int32_t)rx_data[1] << 8) |
                                     ((int32_t)rx_data[2]);
    p_lqbp002ta->i32rawTemperature = (p_lqbp002ta->i32rawTemperature & 0x800000) ? (0xFF000000 | p_lqbp002ta->i32rawTemperature) : p_lqbp002ta->i32rawTemperature;
    printf("TA:raw_temp:%d\n",p_lqbp002ta->i32rawTemperature);
}

/*****************************************************************************
 Function: lqbp002ta_get_raw_pressure
 Description: obtain the original pressure value and turn them into 32bits-integer
 Input: void
 Output:
 Return: void
 Calls:
 Called By:
*****************************************************************************/
void lqbp002ta_get_raw_pressure(void)
{
    uint8 reg_addr[3] = {0x00,0x01,0x02};
    uint8 rx_data[3];
    int ret;
    i2c_data_t data = {0};
    for(int i=0;i<3;i++)
    {
    data.send_buf = &reg_addr[i];
    data.send_len = 1;
    data.receive_buf = &rx_data[i];
    data.receive_len = 1;

    ret = uapi_i2c_master_writeread(I2C_BUS_1, HW_ADR, &data);

    if (ret != 0)
    {
        BAROMETER_PRINT("lqbp002ta_get_raw_pressure reg 0x%x failed, ret = %p\n", reg_addr[i], ret);
        lqbp002ta_i2c_err = -1;
        return;
    }
    lqbp002ta_i2c_err = 0;
    printf("TA pressure :rx_data:%d\n",rx_data[i]);
    }
    p_lqbp002ta->i32rawPressure = ((int32_t)rx_data[0] << 16) |
                                  ((int32_t)rx_data[1] << 8) |
                                  ((int32_t)rx_data[2]);

    p_lqbp002ta->i32rawPressure = (p_lqbp002ta->i32rawPressure & 0x800000) ? (0xFF000000 | p_lqbp002ta->i32rawPressure) : p_lqbp002ta->i32rawPressure;
    printf("TA:raw_pressure:%d\n",p_lqbp002ta->i32rawPressure);
}

/*****************************************************************************
 Function: lqbp002ta_get_temperature
 Description:  return calibrated temperature value base on original value.
 Input: void
 Output:
 Return: void
 Calls:
 Called By:
*****************************************************************************/
float lqbp002ta_get_temperature(void)
{
    float fTCompensate;
    float fTsc;

    fTsc = p_lqbp002ta->i32rawTemperature / (float)p_lqbp002ta->i32kT;
    fTCompensate = p_lqbp002ta->calib_param.c0 * 0.5 + p_lqbp002ta->calib_param.c1 * fTsc;
    return fTCompensate;
}

/*****************************************************************************
 Function: lqbp002ta_get_pressure
 Description: return calibrated pressure value base on original value.
 Input: void
 Output:
 Return: void
 Calls:
 Called By:
*****************************************************************************/

float lqbp002ta_get_pressure(void)
{
    float fTsc, fPsc;
    float qua2, qua3;
    float fPCompensate;

    fTsc = p_lqbp002ta->i32rawTemperature / (float)p_lqbp002ta->i32kT;
    fPsc = p_lqbp002ta->i32rawPressure / (float)p_lqbp002ta->i32kP;

    if (is_lqbp002ta())
    {
        qua2 = p_lqbp002ta->calib_param.c10 + fPsc * (p_lqbp002ta->calib_param.c20 + fPsc * (p_lqbp002ta->calib_param.c30 + fPsc * p_lqbp002ta->calib_param.c40));
        qua3 = fTsc * fPsc * (p_lqbp002ta->calib_param.c11 + fPsc * (p_lqbp002ta->calib_param.c21 + fPsc * p_lqbp002ta->calib_param.c31));
    }
    else
    {
        qua2 = p_lqbp002ta->calib_param.c10 + fPsc * (p_lqbp002ta->calib_param.c20 + fPsc * p_lqbp002ta->calib_param.c30);
        qua3 = fTsc * fPsc * (p_lqbp002ta->calib_param.c11 + fPsc * p_lqbp002ta->calib_param.c21);
    }

    fPCompensate = p_lqbp002ta->calib_param.c00 + fPsc * qua2 + fTsc * p_lqbp002ta->calib_param.c01 + qua3;
    return fPCompensate;
}



float lqbp002ta_get_data(void)
{
    float pressure = 0.0;
    lqbp002ta_get_raw_pressure();
    pressure = lqbp002ta_get_pressure();
    return pressure;
}

float pressure_convert_altitude(float Press,float Ref_P)
{
	return 44330 * (1 - powf((Press/Ref_P),(1/5.255)));
}

#endif
