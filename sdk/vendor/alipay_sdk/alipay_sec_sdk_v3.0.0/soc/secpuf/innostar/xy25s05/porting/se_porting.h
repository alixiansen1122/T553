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
* @file se_porting.h
*
* @brief 这里定义硬件平台的适配函数，包括：
*   - 跟安全芯片通信的接口函数
*   - 需要在HOST环境提供的辅助功能函数，包括延时、GPIO端口控制等。某些函数是必须的，某些函数由测试或演示功能调用，可根据需要实现。
*
*/
#ifndef _SE_PORTING_H_
#define _SE_PORTING_H_
#ifdef __cplusplus
extern "C" {
#endif

/* ===========================================  Includes  =========================================== */

#include <stdint.h>
#include "se_reg.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */


/* ====================================  Functions declaration  ===================================== */
/*!
* @brief 通信接口初始化
*
* @param[in] context:通信接口上下文数据
*
* @return status
*/
uint32 SE_COMM_Init(void *context);

/*!
* @brief 通信接口去初始化
*
* @param[in] context:通信接口上下文数据
*
* @return status
*/
uint32 SE_COMM_DeInit(void *context);

/*!
* @brief 写一个WORD
*
* @param[in] context:通信接口上下文数据
* @param[in] addr:address to write
* @param[in] data:data to write
*
* @return status
*/
uint32 SE_COMM_Write(void *context, uint32 addr,uint32 data);

/*!
* @brief 读一个WORD
*
* @param[in] context:通信接口上下文数据
* @param[in] addr:address to read
*
* @return status
*/
uint32 SE_COMM_Read(void *context, uint32 addr);

/*!
* @brief 写多个WORD
*
* @param[in] context:通信接口上下文数据
* @param[in] addr:address to write
* @param[in] data:data to write
* @param[in] len:data length to write
*
* @return status
*/
uint32 SE_COMM_MultiWrite(void *context, uint32 addr,uint32 *buf,uint32 len);

/*!
* @brief 读多个WORD
*
* @param[in] context:通信接口上下文数据
* @param[in] addr:address to read
* @param[in] buf:point to read buffer
* @param[in] len:length to read 
*
* @return status
*/
uint32 SE_COMM_MultiRead(void *context,uint32 addr,uint32 *buf,uint32 len);

/**
 * @brief 开始通信. 本函数实现中，需要对通信硬件做合适的配置，使得后续与安全芯片的通信可以正常进行。
 * 
 * @param context 
 */
void SE_COMM_Begin(void * context);
/**
 * @brief 结束通信. 本函数中，可以把与安全芯片的通信设施释放出来，用于其他功能。
 * 
 * @param context 
 */

void SE_COMM_End(void * context);

/** @addtogroup SE_HOST_ADAPTER
 * @brief HOST需要提供或实现的接口函数
  * @{
  */

/**
 * @brief delay us
 * @param uint32_t  count of us unit
 * @return (void)
 */
void Delayus(volatile uint32_t cnt);

/**
 * @brief Delay ms
 * @param uint32_t
 * @return (void)
 */
void Delayms(volatile uint32_t cnt);

/**
 * @brief 获取当前tick值
 * 
 * @return uint64_t 
 */
uint64_t CurrentTick(void);

/**
 * @brief 把tick值转换为us
 * 
 * @param ticks 
 * @return uint32_t 
 */
uint32_t TicksToUs(uint64_t ticks);

/**
 * @brief HOST端生成的随机数.
 * @details 这里的随机数，用于跟安全芯片的密码交互，因此不能由安全芯片本身产生.
 * 
 * @return uint32_t 
 */
uint32_t SE_Hal_GetRng(void);

/*!
* @brief 获取设备对象
*
* @return SE_DevConfigType
*/
SE_DevConfigType *SE_Hal_GetDev(void);



/**
  * @}
  */



#ifdef __cplusplus
}
#endif
#endif /* _SE_PORTING_H_ */
