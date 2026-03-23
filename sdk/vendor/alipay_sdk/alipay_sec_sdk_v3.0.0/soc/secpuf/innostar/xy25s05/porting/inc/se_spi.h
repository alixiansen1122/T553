/* Copyright Statement:
*
* This software/firmware and related documentation ("Innostar Software") are
* protected under relevant copyright laws. The information contained herein is
* confidential and proprietary to Innostar Inc. and/or its licensors. Without
* the prior written permission of Innostar inc. and/or its licensors, any
* reproduction, modification, use or disclosure of Innostar Software, and
* information contained herein, in whole or in part, shall be strictly
* prohibited.
*
* Innostar Inc. (C) 2020. All rights reserved.
*
* BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
* THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("INNOSTAR SOFTWARE")
* RECEIVED FROM INNOSTAR AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
* ON AN "AS-IS" BASIS ONLY. INNOSTAR EXPRESSLY DISCLAIMS ANY AND ALL
* WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
* WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
* NONINFRINGEMENT. NEITHER DOES INNOSTAR PROVIDE ANY WARRANTY WHATSOEVER WITH
* RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
* INCORPORATED IN, OR SUPPLIED WITH THE INNOSTAR SOFTWARE, AND RECEIVER AGREES
* TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
* RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
* OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN INNOSTAR
* SOFTWARE. INNOSTAR SHALL ALSO NOT BE RESPONSIBLE FOR ANY INNOSTAR SOFTWARE
* RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
* STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND INNOSTAR'S
* ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE INNOSTAR SOFTWARE
* RELEASED HEREUNDER WILL BE, AT INNOSTAR'S OPTION, TO REVISE OR REPLACE THE
* INNOSTAR SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
* CHARGE PAID BY RECEIVER TO INNOSTAR FOR SUCH INNOSTAR SOFTWARE AT ISSUE.
*/

/*!
* @file se_spi.h
*
* @brief This file provides spi header function .
*
*/

#ifndef _SE_SPI_H_
#define _SE_SPI_H_
#ifdef __cplusplus
extern "C" {
#endif

/* ===========================================  Includes  =========================================== */
#include "se_reg.h"
#include "se_delay.h"


/* ============================================  Define  ============================================ */


/* ====================================  Functions declaration  ===================================== */

/*!
* @brief SPI Initialize
*
* @param[in] devConfig:struct to device configuration
*
* @return status
*/
uint32 SE_SPI_Init(SE_DevConfigType *devConfig);

/*!
* @brief SPI De-Initialize
*
* @param[in] devConfig:struct to device configuration
*
* @return status
*/
uint32 SE_SPI_DeInit(SE_DevConfigType *devConfig);

/*!
* @brief SPI Search Device Number and Store Device ID
*
* @param[in] devIdBuf: point to store device ID
*
* @return Device Number
*/
uint32 SE_SPI_SearchDevice(uint64 *devIdBuf);

/*!
* @brief SPI Enable,Reset CS PIN to enable SPI Communication
*
* @param[in] devConfig:struct to device configuration
*
* @return none
*/
void SE_SPI_Enable(SE_DevConfigType *devConfig);

/*!
* @brief SPI Disable,Set CS PIN to disable SPI Communication
*
* @param[in] devConfig:struct to device configuration
*
* @return none
*/
void SE_SPI_Disable(SE_DevConfigType *devConfig);

/*!
* @brief SPI Write
*
* @param[in] devConfig:struct to device configuration
* @param[in] addr:address to write
* @param[in] data:data to write
*
* @return status
*/
uint32 SE_SPI_Write(SE_DevConfigType *devConfig,uint32 addr,uint32 data);

/*!
* @brief SPI Read
*
* @param[in] devConfig:struct to device configuration
* @param[in] addr:address to read
*
* @return status
*/
uint32 SE_SPI_Read(SE_DevConfigType *devConfig,uint32 addr);

/*!
* @brief SPI Multi-Write
*
* @param[in] devConfig:struct to device configuration
* @param[in] addr:address to write
* @param[in] data:data to write
* @param[in] len:data length to write
*
* @return status
*/
uint32 SE_SPI_MultiWrite(SE_DevConfigType *devConfig,uint32 addr,uint32 *buf,uint32 len);

/*!
* @brief SPI Multi-Read
*
* @param[in] devConfig:struct to device configuration
* @param[in] addr:address to read
* @param[in] buf:point to read buffer
* @param[in] len:length to read 
*
* @return status
*/
uint32 SE_SPI_MultiRead(SE_DevConfigType *devConfig,uint32 addr,uint32 *buf,uint32 len);

#ifdef __cplusplus
}
#endif
#endif /* _SE_SPI_H_ */

