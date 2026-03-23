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
 * @file is_secure_storage.h
 *
 * @brief This file provides functions for secure storage application.
 *
 */
#ifndef _IS_SECURE_STORAGE_H_
#define _IS_SECURE_STORAGE_H_
#ifdef __cplusplus
extern "C"
{
#endif

    /* ===========================================  Includes  =========================================== */
#include <stdint.h>


    /* ============================================  Define  ============================================ */




    /* ===========================================  Typedef  ============================================ */
/**
 * @brief 接口函数返回值类型
 * 
 */
typedef enum SS_STATUS_TYPE_t{
    SST_SUCCESS=0,/*!< 执行成功 */
    SST_FAIL=-1,/*!< 执行失败 */
    SST_NOT_ALLOCED_BLOCK=-2,/*!< 尚未分配的数据BLOCK */
    SST_TRY_ANOTHER_BLOCK=-3,/*!< 当前block写入失败，尝试另外block */
    SST_BAD_STORAGE=-4,/*!<  已经损坏无法修复的存储 */
    SST_DS_OVER_FLOW=-5,/*!< 存储空间溢出 */
     
}SS_STATUS_TYPE;

/**
 * @brief 安全存储芯片生命周期类型 
 * 
 */
typedef enum
{
    SS_OPMODE_TEST = 0,/*!< 芯片测试模式 */
		SS_OPMODE_OEM,/*!< OEM模式 */
		SS_OPMODE_USER,/*!< 用户模式 */
		SS_OPMODE_FAIL,/*!< 失效模式 */
		SS_OPMODE_DEBUG,/*!< 调试模式 */
		SS_OPMODE_MAX
} SS_OPMODE_TYPE;

    /* ====================================  Functions declaration  ===================================== */
/**
 * @brief 安全存储功能模块初始化. 失败的话后续其他调用都会失败
 * 
 * @return SS_STATUS_TYPE 
 */
SS_STATUS_TYPE SS_Init(void);

/**
 * @brief 安全存储芯片复位后需要调用本函数
 * 
 * @return SS_STATUS_TYPE 
 */
SS_STATUS_TYPE SS_AfterReset(void);



/*!
* @brief 获取当前生命周期
*
*
* @return mode
*/
SS_OPMODE_TYPE SS_GetOperationMode(void);

/**
 * @brief  获取SN
 * 
 * @param[out]  buf 存放SN,必须是16字节的buffer
 *                         
 * @return 执行结果:
 *          - SST_SUCCESS : 成功
 *          - SST_FAIL : 失败
 */
SS_STATUS_TYPE SS_GetSN(uint8_t *buf16bytes);

/**
 * @brief 与MCU绑定，生命周期进入USER MODE;
 *  本函数会生成认证密钥并写入,配置安全存储访问密码
 * 
 * @param[in]  oempwd 访问密码; OME模式的芯片，需要OEM密码
 * @param[in] rootKey32bytes  根密钥，建议每颗MCU都不同
 * @param[in] mcuid    MCU标识
 * @param[in] mcuidLen MCU标识长度(字节数)
 * @return SS_STATUS_TYPE 
 */
SS_STATUS_TYPE SS_BondWithMcu(const uint32_t oempwd[4],const uint8_t *rootKey32bytes,const uint8_t *mcuid,uint32_t mcuidLen);
/**
 * @brief  写入OTP数据块。只能在OEM模式下写入。
 *  如果芯片处于USER模式，不能写入; 设备证书，使用本接口写入
 * 
 * @param[in]  pwd 访问密码; OME模式的芯片，需要OEM密码;  如果密码无效，就不能写入;
 * @param[in]  dataKey 数据的键值
 * @param[in]  data 要写入的数据块
 * @param[in]  dataSize 要写入的数据块的字节数
 * 
 *                         
 * @return 执行结果:
 *          - SST_SUCCESS : 成功
 *          - SST_FAIL : 失败
 */
SS_STATUS_TYPE SS_OEMOTP_Write(const uint32_t oempwd[4],const uint8_t* dataKey,const uint8_t* data,uint32_t dataLen);



/**
 * @brief  读取OTP数据块，在OEM模式
 * 
 * @param[in]  pwd 访问密码; OME模式的芯片，需要OEM密码;
 * @param[in]  dataKey 数据的键值
 * @param[out]  buf 存放读取的数据
 * @param[in]  bufSize buf的字节数
 * @param[out]  pReadLen 实际读取到的字节数
 *                          
 * @return 执行结果:
 *          - SST_SUCCESS : 成功
 *          - SST_FAIL : 失败
 */
SS_STATUS_TYPE SS_OEMOTP_Read(const uint32_t oempwd[4],const uint8_t* dataKey,uint8_t* buf,uint32_t bufSize,uint32_t *pReadLen);

/**
 * @brief  在USER 模式下，读取数据
 * 
 * @param[in] rootKey32bytes  根密钥
 * @param[in] mcuid  MCU标识
 * @param[in] mcuidLen  MCU标识长度(字节数)
 * @param dataKey 数据的键值
 * @param buf   存放读取的数据
 * @param bufSize buf的字节数
 * @param pReadLen 实际读取到的字节数
 * @return SS_STATUS_TYPE 
 *          - SST_SUCCESS : 成功
 *          - SST_FAIL : 失败
 */
SS_STATUS_TYPE SS_USEROTP_Read(const uint8_t *rootKey32bytes,const uint8_t *mcuid,uint32_t mcuidLen,
    const uint8_t* dataKey,uint8_t* buf,uint32_t bufSize,uint32_t *pReadLen);


/**
 * @brief  写入FTP(少量多次写入)数据块
 * 
 * @param[in] rootKey32bytes  根密钥
 * @param[in] mcuid  MCU标识
 * @param[in] mcuidLen  MCU标识长度(字节数)
 * @param[in]  key 数据的键值
 * @param[in]  data 要写入的数据块
 * @param[in]  dataSize 要写入的数据块的自己数
 * 
 *                         
 * @return 执行结果:
 *          - SST_SUCCESS : 成功
 *          - SST_FAIL : 失败
 */
SS_STATUS_TYPE SS_USERFTP_Write(const uint8_t *rootKey32bytes,const uint8_t *mcuid,uint32_t mcuidLen,
    const uint8_t* key,const uint8_t* data,uint32_t dataSize);

/**
 * @brief  读取FTP(少量多次写入)数据块
 * 
 * @param[in] rootKey32bytes  根密钥
 * @param[in] mcuid  MCU标识
 * @param[in] mcuidLen  MCU标识长度(字节数)
 * @param[in]  dataKey 数据的键值
 * @param[out]  buf 存放读取的数据
 * @param[in]  bufSize buf的字节数
 * @param[out]  pReadLen 实际读取到的字节数
 * 
 *                         
 * @return 执行结果:
 *          - SST_SUCCESS : 成功
 *          - SST_FAIL : 失败
 */
SS_STATUS_TYPE SS_USERFTP_Read(const uint8_t *rootKey32bytes,const uint8_t *mcuid,uint32_t mcuidLen,
    const uint8_t* dataKey,uint8_t* buf,uint32_t bufSize,uint32_t *pReadLen);

/**
 * @brief 删除数据
 * 
 * @param[in] rootKey32bytes  根密钥
 * @param[in] mcuid  MCU标识
 * @param[in] mcuidLen  MCU标识长度(字节数)
 * @param[in]  dataKey 数据的键值
 * @return SS_STATUS_TYPE 
 *          - SST_SUCCESS : 成功
 *          - SST_FAIL : 失败
 */
SS_STATUS_TYPE SS_USERFTP_Delete(const uint8_t *rootKey32bytes,const uint8_t *mcuid,uint32_t mcuidLen,
    const uint8_t* dataKey);

/**
 * @brief 清除所有数据
 * 
 * @param[in] rootKey32bytes  根密钥
 * @param[in] mcuid  MCU标识
 * @param[in] mcuidLen  MCU标识长度(字节数)
 * @return SS_STATUS_TYPE 
 *          - SST_SUCCESS : 成功
 *          - SST_FAIL : 失败
 */
SS_STATUS_TYPE SS_USERFTP_DeleteAll(const uint8_t *rootKey32bytes,const uint8_t *mcuid,uint32_t mcuidLen);


/**
 * @brief 获取数据状态
 * 
 * @param[in] rootKey32bytes  根密钥
 * @param[in] mcuid  MCU标识
 * @param[in] mcuidLen  MCU标识长度(字节数)
 * @param[in] dataKey 
 * @param[out] pDataBytes 
 * @return SS_STATUS_TYPE 
 *          - SST_SUCCESS : 数据存在
 *          - SST_FAIL : 芯片失效
 *          - SST_NOT_ALLOCED_BLOCK 数据不存在
 */
SS_STATUS_TYPE SS_GetDataStat(const uint8_t *rootKey32bytes,const uint8_t *mcuid,uint32_t mcuidLen,
    const uint8_t* dataKey,uint32_t *pDataBytes);
/**
 * @brief 做认证和登录。调用完后，会保持登录状态。 需要 调用 SS_ExitSecureSession 结束登录状态。
 *  入参包含 rootKey32bytes mcuid 的函数，会判断登录状态，如果尚未进入SecureSession,内部会调用本函数执行认证和登录
 * 
 * @param rootKey32bytes 
 * @param mcuid 
 * @param mcuidLen 
 * @return SS_STATUS_TYPE 
 */
SS_STATUS_TYPE SS_EnterSecureSession(const uint8_t *rootKey32bytes,const uint8_t *mcuid,uint32_t mcuidLen);

/**
 * @brief 结束当前的安全会话.使得后续无法访问安全存储区域，除非重新进行认证授权.
 * 
 */
void SS_ExitSecureSession(void);

/**
 * @brief 获取随机数
 * 
 * @return uint32_t 
 */
uint32_t SS_GET_RNG(void);

SS_STATUS_TYPE SS_TryBond(const uint32_t oempwd[4],const uint8_t *rootKey32bytes,const uint8_t *mcuid,uint32_t mcuidLen);

#ifdef __cplusplus
}
#endif
#endif /* _LIFE_SPAN_H_ */
