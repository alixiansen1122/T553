/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file crypt_util_rand.h
 * Description: 公共随机数注册头文件
 * Author: panxinrong
 * Create: 2022-5-14
 * Modification History
 *  DATE        NAME             DESCRIPTION
 *  --------------------------------------------------------------------------
 *  2022-5-14  panxinrong        创建
 */

#ifndef CRYPT_UTIL_RAND_H
#define CRYPT_UTIL_RAND_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t (*CRYPT_RandFunc)(uint8_t *rand, uint32_t randLen);

/**
 * @brief   随机数注册
 *
 * @param   func [IN] 随机数获取接口
 */
void CRYPT_RandRegist(CRYPT_RandFunc func);

/**
 * @brief   生成随机数
 *
 * @param   rand [OUT] 随机数buffer
 * @param   randLen [IN] 随机数长度
 *
 * @retval  CRYPT_SUCCESS 生成随机数成功
 * @retval  CRYPT_NO_REGIST_RAND 随机数未注册
 * @retval  注册的随机数生成失败返回的错误
 */
int32_t CRYPT_Rand(uint8_t *rand, uint32_t randLen);

#ifdef __cplusplus
}
#endif

#endif