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
* @file se_hal.h
*
* @brief This file provides register/NVM write/register header function .
*
*/
#ifndef _SE_HAL_H_
#define _SE_HAL_H_
#ifdef __cplusplus
extern "C" {
#endif

/* ===========================================  Includes  =========================================== */

#include "se_config.h"
#include "se_spi.h"

/* ============================================  Define  ============================================ */

/* ====================================  Functions declaration  ===================================== */
/*!
* @brief Hal Interface Initialize
*
* @param[in] devConfig:struct to device configuration
*
* @return status
*/
uint32 SE_Hal_Init(SE_DevConfigType *devConfig);

/*!
* @brief Hal Interface De-Initialize
*
* @param[in] devConfig:struct to device configuration
*
* @return status
*/
uint32 SE_Hal_DeInit(SE_DevConfigType *devConfig);

/*!
* @brief Hal Search Device Id
*
* @param[in] devIdBuf:point to restore DeviceId
*
* @return status
*/
uint32 SE_Hal_SearchDevice(uint64 *devIdBuf);

/*!
* @brief write register
*
* @param[in] devConfig:struct to device configuration
* @param[in] addr:address to write
* @param[in] data:data to write
*
* @return status
*/
uint32 SE_Hal_WriteReg(SE_DevConfigType *devConfig,uint32 addr,uint32 data);

/*!
* @brief read register
*
* @param[in] devConfig:struct to device configuration
* @param[in] addr:address to read
*
* @return read data
*/
uint32 SE_Hal_ReadReg(SE_DevConfigType *devConfig,uint32 addr);

/*!
* @brief multi write register
*
* @param[in] devConfig:struct to device configuration
* @param[in] addr:start address to write
* @param[in] buf:point to write data buffer
* @param[in] len:length to write
*
* @return status
*/
uint32 SE_Hal_MultiWriteReg(SE_DevConfigType *devConfig, uint32 addr,uint32 *buf,uint32 len);

/*!
* @brief multi read register
*
* @param[in] devConfig:struct to device configuration
* @param[in] addr:start address to read
* @param[in] buf:point to store read data buffer
* @param[in] len:length to read
*
* @return status
*/
uint32 SE_Hal_MultiReadReg(SE_DevConfigType *devConfig, uint32 addr, uint32 *buf,uint32 len);

/*!
* @brief write RRAM
*
* @param[in] devConfig:struct to device configuration
* @param[in] addr:address to write
* @param[in] data:data to write
*
* @return status
*/
uint32 SE_Hal_WriteRram(SE_DevConfigType *devConfig,uint32 addr,uint32 data);

/*!
* @brief read Rram
*
* @param[in] devConfig:struct to device configuration
* @param[in] addr:address to read
*
* @return read data
*/
uint32 SE_Hal_ReadRram(SE_DevConfigType *devConfig,uint32 addr);

/*!
* @brief multi write RRAM
*
* @param[in] devConfig:struct to device configuration
* @param[in] addr:start address to write
* @param[in] buf:point to write data buffer
* @param[in] len:length to write
*
* @return status
*/
uint32 SE_Hal_MultiWriteRram(SE_DevConfigType *devConfig, uint32 addr, uint32 *buf, uint32 len);

/*!
* @brief multi read RRAM
*
* @param[in] devConfig:struct to device configuration
* @param[in] addr:start address to read
* @param[in] buf:point to store read data buffer
* @param[in] len:length to read
*
* @return status
*/
uint32 SE_Hal_MultiReadRram(SE_DevConfigType *devConfig, uint32 addr, uint32 *buf, uint32 len);

/*!
* @brief write data to input buffer
*
* @param[in] devConfig:struct to device configuration
* @param[in] addrOffset:input buffer start address to write
* @param[in] buf:point to write data buffer
* @param[in] len:write data length
*
* @return status
*/
uint32 SE_Hal_WriteInputBuffer(SE_DevConfigType *devConfig,uint32 addrOffset,uint32 *buf,uint32 len);

/*!
* @brief Read data from input buffer
*
* @param[in] devConfig:struct to device configuration
* @param[in] addrOffset:input buffer start address to read
* @param[in] buf:point to store read data buffer
* @param[in] len:read data length
*
* @return status
*/
uint32 SE_Hal_ReadInputBuffer(SE_DevConfigType *devConfig,uint32 addrOffset,uint32 *buf,uint32 len);

/*!
* @brief write data to output buffer
*
* @param[in] devConfig:struct to device configuration
* @param[in] addrOffset:output buffer start address to write
* @param[in] buf:point to write data buffer
* @param[in] len:write data length
*
* @return status
*/
uint32 SE_Hal_WriteOutputBuffer(SE_DevConfigType *devConfig,uint32 addrOffset,uint32 *buf,uint32 len);

/*!
* @brief Read data from output buffer
*
* @param[in] devConfig:struct to device configuration
* @param[in] addrOffset:output buffer start address to read
* @param[in] buf:point to store read data buffer
* @param[in] len:read data length
*
* @return status
*/
uint32 SE_Hal_ReadOutputBuffer(SE_DevConfigType *devConfig,uint32 addrOffset,uint32 *buf,uint32 len);

/*!
* @brief write data to key buffer
*
* @param[in] devConfig:struct to device configuration
* @param[in] buf:point to write data buffer
*
* @return status
*/
uint32 SE_Hal_WriteKeyBuffer(SE_DevConfigType *devConfig, uint32 *buf);

/*!
* @brief Read data from key buffer
*
* @param[in] devConfig:struct to device configuration
* @param[in] buf:point to store read data buffer
*
* @return status
*/
uint32 SE_Hal_ReadKeyBuffer(SE_DevConfigType *devConfig, uint32 *buf);


#ifdef __cplusplus
}
#endif
#endif /* _SE_HAL_H_ */
