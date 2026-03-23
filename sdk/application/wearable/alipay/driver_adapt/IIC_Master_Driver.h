/**
 * Copyright (c) CompanyNameMagicTag 2021-2022. All rights reserved.
 * Description: Alipay sec soc driver adapt.
 * Author:
 * Create:
 */

#include <stdint.h>

#ifndef ALIPAY_SOC_DRIVER_ADAPT_H
#define ALIPAY_SOC_DRIVER_ADAPT_H

void HS_IIC_Init(void);

unsigned char *IIC_Master_Init(void);

void IIC_Master_Send(unsigned char byAddr, unsigned char *pData, unsigned short wLen);

void IIC_Master_Receive(unsigned char byAddr, unsigned char *pData, unsigned short wLen);

#endif
