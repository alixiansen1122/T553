/**
 * @copyright   Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @brief       字节流的编解码接口
 */

#ifndef BSL_BYTES_H
#define BSL_BYTES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   uint8_t 字节流转 uint16_t 类型数据
 *
 * @attention data 不能为空
 *
 * @param   data [IN] uint8_t 字节流
 *
 * @return  uint16_t，转换后的数据
 */
static inline uint16_t BSL_ByteToUint16(const uint8_t *data)
{
    /** 第0字节左移8位，第1字节不变，或操作后得到 uint16_t */
    return ((uint16_t)data[0] << 8) | ((uint16_t)data[1]);
}

/**
 * @brief   uint16_t 类型数据转 uint8_t 字节流
 *
 * @attention data 不能为空
 *
 * @param   num [IN] 待转换的数据
 * @param   data [OUT] 转换后的数据
 */
static inline void BSL_Uint16ToByte(uint16_t num, uint8_t *data)
{
    /** 转换到字节流 */
    data[0] = (uint8_t)(num >> 8);    // 数据右移8位，放到第0字节
    data[1] = (uint8_t)(num & 0xffu); // 数据与上0xffu，放到第1字节
    return;
}

/**
 * @brief   uint8_t 字节流转 uint24_t 类型数据
 *
 * @attention data 不能为空
 *
 * @param   data [IN] uint8_t 字节流
 *
 * @return  uint24_t，转换后的数据
 */
static inline uint32_t BSL_ByteToUint24(const uint8_t *data)
{
    /** 第0字节左移16位，第1字节左移8位，第2字节不变，或操作后得到 uint24_t */
    return ((uint32_t)data[0] << 16) | ((uint32_t)data[1] << 8) | ((uint32_t)data[2]);
}

/**
 * @brief   uint24_t 类型数据转 uint8_t 字节流
 *
 * @attention data 不能为空
 *
 * @param   num [IN] 待转换的数据
 * @param   data [OUT] 转换后的数据
 */
static inline void BSL_Uint24ToByte(uint32_t num, uint8_t *data)
{
    /** 转换到字节流 */
    data[0] = (uint8_t)(num >> 16);   // 数据右移16位，放到第0字节
    data[1] = (uint8_t)(num >> 8);    // 数据右移8位，放到第1字节
    data[2] = (uint8_t)(num & 0xffu); // 数据与上0xffu，放到第2字节
    return;
}

/**
 * @brief   uint8_t 字节流转 uint32_t 类型数据
 *
 * @attention data 不能为空
 *
 * @param   data [IN]  uint8_t 字节流
 *
 * @return  uint32_t，转换后的数据
 */
static inline uint32_t BSL_ByteToUint32(const uint8_t *data)
{
    /** 第0字节左移24位，第1字节左移16位，第2字节左移8位，第3字节不变，或操作后得到 uint32_t */
    return ((uint32_t)data[0] << 24) | ((uint32_t)data[1] << 16) | ((uint32_t)data[2] << 8) | ((uint32_t)data[3]);
}

/**
 * @brief   uint8_t 字节流转 uint48_t 类型数据
 *
 * @attention data 不能为空
 *
 * @param   data [IN]  uint8_t 字节流
 *
 * @return  uint48_t，转换后的数据
 */
static inline uint64_t BSL_ByteToUint48(const uint8_t *data)
{
    /** 第0字节左移40位，第1字节左移32位，第2字节左移24位，第3字节左移16位，第4字节左移8位，第5字节不变，
       或操作后得到 uint48_t */
    return ((uint64_t)data[0] << 40) | ((uint64_t)data[1] << 32) | ((uint64_t)data[2] << 24) |
        ((uint64_t)data[3] << 16) | ((uint64_t)data[4] << 8) | ((uint64_t)data[5]);
}

/**
 * @brief   uint48_t 类型数据转 uint8_t 字节流
 *
 * @attention data 不能为空
 *
 * @param   num [IN] 待转换的数据
 * @param   data [OUT] 转换后的数据
 */
static inline void BSL_Uint48ToByte(uint64_t num, uint8_t *data)
{
    /** 转换到字节流 */
    data[0] = (uint8_t)(num >> 40);   // 数据右移40位，放到第0字节
    data[1] = (uint8_t)(num >> 32);   // 数据右移32位，放到第1字节
    data[2] = (uint8_t)(num >> 24);   // 数据右移24位，放到第2字节
    data[3] = (uint8_t)(num >> 16);   // 数据右移16位，放到第3字节
    data[4] = (uint8_t)(num >> 8);    // 数据右移8位，放到第4字节
    data[5] = (uint8_t)(num & 0xffu); // 数据与上0xffu，放到第5字节
    return;
}

/**
 * @brief   uint8_t 字节流转 uint64_t 类型数据
 *
 * @attention data 不能为空
 *
 * @param   data [IN] uint8_t 字节流
 *
 * @return  uint32_t，转换后的数据
 */
static inline uint64_t BSL_ByteToUint64(const uint8_t *data)
{
    /** 第0字节左移56位，第1字节左移48位，第2字节左移40位，第3字节左移32位，第4字节左移24位，第5字节左移16位，
        第6字节左移8位，第7字节不变，或操作后得到 uint64_t */
    return ((uint64_t)data[0] << 56) | ((uint64_t)data[1] << 48) | ((uint64_t)data[2] << 40) |
        ((uint64_t)data[3] << 32) | ((uint64_t)data[4] << 24) | ((uint64_t)data[5] << 16) |
        ((uint64_t)data[6] << 8) | ((uint64_t)data[7]);
}

/**
 * @brief   uint32_t 类型数据转 uint8_t 字节流
 *
 * @attention data 不能为空
 *
 * @param   num [IN] 待转换的数据
 * @param   data [OUT] 转换后的数据
 */
static inline void BSL_Uint32ToByte(uint32_t num, uint8_t *data)
{
    /** 转换到字节流 */
    data[0] = (uint8_t)(num >> 24);   // 数据右移24位，放到第0字节
    data[1] = (uint8_t)(num >> 16);   // 数据右移16位，放到第1字节
    data[2] = (uint8_t)(num >> 8);    // 数据右移8位，放到第2字节
    data[3] = (uint8_t)(num & 0xffu); // 数据与上0xffu，放到第3字节
    return;
}

/**
 * @brief   uint64_t 类型数据转 uint8_t 字节流
 *
 * @attention data 不能为空
 *
 * @param   num [IN] 待转换的数据
 * @param   data [OUT] 转换后的数据
 */
static inline void BSL_Uint64ToByte(uint64_t num, uint8_t *data)
{
    /** 转换到字节流 */
    data[0] = (uint8_t)(num >> 56);   // 数据右移56位，放到第0字节
    data[1] = (uint8_t)(num >> 48);   // 数据右移48位，放到第1字节
    data[2] = (uint8_t)(num >> 40);   // 数据右移40位，放到第2字节
    data[3] = (uint8_t)(num >> 32);   // 数据右移32位，放到第3字节
    data[4] = (uint8_t)(num >> 24);   // 数据右移24位，放到第4字节
    data[5] = (uint8_t)(num >> 16);   // 数据右移16位，放到第5字节
    data[6] = (uint8_t)(num >> 8);    // 数据右移8位，放到第6字节
    data[7] = (uint8_t)(num & 0xffu); // 数据与上0xffu，放到第7字节
    return;
}

#ifdef __cplusplus
}
#endif /* end __cplusplus */

#endif // BSL_BYTES_H
