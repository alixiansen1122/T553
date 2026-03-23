/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file crypt_utils.h
 * Description: 公共头文件
 * Author: zhangqiankun
 * Create: 2022-3-5
 * Modification History
 *  DATE        NAME             DESCRIPTION
 *  --------------------------------------------------------------------------
 *  2022-3-5  zhangqiankun        创建公共头文件
 */

#ifndef CRYPT_UTILS_H
#define CRYPT_UTILS_H

#include <stdint.h>
#include "crypt_algid.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define PUT_UINT32_BE(v, p, i)                                         \
do {                                                                   \
    (p)[(i) + 0] = (uint8_t)((v) >> 24); /* 24: 4th byte */      \
    (p)[(i) + 1] = (uint8_t)((v) >> 16); /* 16: 3rd byte */      \
    (p)[(i) + 2] = (uint8_t)((v) >>  8); /*  8: 2nd byte */      \
    (p)[(i) + 3] = (uint8_t)((v) >>  0); /*  0: 1st byte */      \
} while (0)

#define GET_UINT32_BE(p, i)                                            \
(                                                                      \
    ((uint32_t)(p)[(i) + 0] << 24) | /* 24: 4th byte */                \
    ((uint32_t)(p)[(i) + 1] << 16) | /* 16: 3rd byte */                \
    ((uint32_t)(p)[(i) + 2] <<  8) | /*  8: 2nd byte */                \
    ((uint32_t)(p)[(i) + 3] <<  0)   /*  0: 1st byte */                \
)

#define PUT_UINT32_LE(v, p, i)                                         \
do {                                                                   \
    (p)[(i) + 3] = (uint8_t)((v) >> 24); /* 24: 4th byte */      \
    (p)[(i) + 2] = (uint8_t)((v) >> 16); /* 16: 3rd byte */      \
    (p)[(i) + 1] = (uint8_t)((v) >>  8); /*  8: 2nd byte */      \
    (p)[(i) + 0] = (uint8_t)((v) >>  0); /*  0: 1st byte */      \
} while (0)

#define PUT_UINT64_LE(v, p, i) do {                                    \
    (p)[(i) + 7] = (uint8_t)((v) >> 56); /* 56: 8th byte */            \
    (p)[(i) + 6] = (uint8_t)((v) >> 48); /* 48: 7rd byte */            \
    (p)[(i) + 5] = (uint8_t)((v) >> 40); /* 40: 6nd byte */            \
    (p)[(i) + 4] = (uint8_t)((v) >> 32); /* 32: 5st byte */            \
    (p)[(i) + 3] = (uint8_t)((v) >> 24); /* 24: 4th byte */            \
    (p)[(i) + 2] = (uint8_t)((v) >> 16); /* 16: 3rd byte */            \
    (p)[(i) + 1] = (uint8_t)((v) >>  8); /*  8: 2nd byte */            \
    (p)[(i) + 0] = (uint8_t)((v) >>  0); /*  0: 1st byte */            \
} while (0)

#define GET_UINT64_LE(p, i)                                            \
(                                                                      \
    ((uint64_t)(p)[(i) + 7] << 56) | ((uint64_t)(p)[(i) + 6] << 48) |  \
    ((uint64_t)(p)[(i) + 5] << 40) | ((uint64_t)(p)[(i) + 4] << 32) |  \
    ((uint64_t)(p)[(i) + 3] << 24) | ((uint64_t)(p)[(i) + 2] << 16) |  \
    ((uint64_t)(p)[(i) + 1] <<  8) | ((uint64_t)(p)[(i) + 0] <<  0)    \
)

/**
 * 判断是否满足条件condition，如果满足则跳转到EXIT标签
 */
#define GOTO_EXIT_IF(condition) \
    do {                        \
        if (condition) {        \
            goto EXIT;          \
        }                       \
    } while (0)

/**
 * func返回值不为CRYPT_SUCCESS跳转到错误退出位置
 */
#define GOTO_ERR_IF(func, ret) do { \
        (ret) = (func); \
        if ((ret) != CRYPT_SUCCESS) { \
            BSL_ERR_PUSH_ERROR((ret)); \
            goto ERR; \
        } \
    } while (0)

/**
 * @brief 将两个数组的数据进行异或运算
 *
 * @param a [IN] 输入数据a
 * @param b [IN] 输入数据b
 * @param r [out] 输出结果数据
 * @param len [IN] 输出结果数据长度
 */
#define DATA_XOR(a, b, r, len)       \
    do {                             \
        uint32_t subscript;          \
        for (subscript = 0; subscript < (len); subscript++) { \
            (r)[subscript] = (a)[subscript] ^ (b)[subscript]; \
        }                             \
    } while (0)

/**
 * @brief 将两个数组的数据每次32bit进行异或运算, 调用者保证输入输出是32bit的整数倍.地址为4字节对齐才做类型转换
 *
 * @param a [IN] 输入数据a
 * @param b [IN] 输入数据b
 * @param r [out] 输出结果数据
 * @param len [IN] 输出结果数据长度
 */
#define DATA32_XOR(a, b, r, len)                                \
    do {                                                        \
        uint32_t ii;                                            \
        uintptr_t aPtr = (uintptr_t)(a);                        \
        uintptr_t bPtr = (uintptr_t)(b);                        \
        uintptr_t rPtr = (uintptr_t)(r);                        \
        if (((aPtr & 0x3) != 0) || ((bPtr & 0x3) != 0) || ((rPtr & 0x3) != 0)) {     \
            for (ii = 0; ii < (len); ii++) {                    \
                (r)[ii] = (a)[ii] ^ (b)[ii];                    \
            }                                                   \
        } else {                                                \
            for (ii = 0; ii < (len); ii += 4) {                 \
                *(uint32_t *)((r) + ii) = (*(const uint32_t *)((a) + ii)) ^ (*(const uint32_t *)((b) + ii)); \
            }                                                   \
        }                                                       \
    } while (0)

/**
 * @brief 将两个数组的数据每次64bit进行异或运算, 调用者保证输入输出是64bit的整数倍。地址为8字节对齐才做类型转换
 *
 * @param a [IN] 输入数据a
 * @param b [IN] 输入数据b
 * @param r [out] 输出结果数据
 * @param len [IN] 输出结果数据长度
 */
#define DATA64_XOR(a, b, r, len)                                \
    do {                                                        \
        uint32_t ii;                                            \
        uintptr_t aPtr = (uintptr_t)(a);                        \
        uintptr_t bPtr = (uintptr_t)(b);                        \
        uintptr_t rPtr = (uintptr_t)(r);                        \
        if (((aPtr & 0x7) != 0) || ((bPtr & 0x7) != 0) || ((rPtr & 0x7) != 0)) {     \
            for (ii = 0; ii < (len); ii++) {                    \
                (r)[ii] = (a)[ii] ^ (b)[ii];                    \
            }                                                   \
        } else {                                                \
            for (ii = 0; ii < (len); ii += 8) {                 \
                *(uint64_t *)((r) + ii) = (*(const uint64_t *)((a) + ii)) ^ (*(const uint64_t *)((b) + ii)); \
            }                                                   \
        }                                                       \
    } while (0)

/* Assumes that x is uint32_t and 0 < n < 32 */
#define ROTL32(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

#define ROTR64(x, n) (((x) << (64 - (n))) | ((x) >> (n))) // Assumes that x is uint64_t and 0 < n < 64

#define IS_BUF_NON_ZERO(out, outLen)  (((out) != NULL) && ((outLen) > 0))
#define CRYPT_IS_BUF_NON_ZERO(out, outLen)  (((out) != NULL) && ((outLen) > 0))
#define CRYPT_CHECK_DATA_INVALID(d) (((d)->data == NULL && (d)->len != 0))
#define CRYPT_IsDataNull(d) ((d) == NULL || (d)->data == NULL || (d)->len == 0)
#define CRYPT_IN_RANGE(x, range) ((x) >= (range)->min && (x) <= (range)->max)
#define CRYPT_CHECK_BUF_INVALID(buf, len) (((buf) == NULL && (len) != 0))
#define CRYPT_SWAP32(x) ((((x) & 0xff000000) >> 24) | \
                         (((x) & 0x00ff0000) >> 8) | \
                         (((x) & 0x0000ff00) << 8) | \
                         (((x) & 0x000000ff) << 24))
#ifdef HITLS_BIG_ENDIAN

#define CRYPT_HTONL(x) (x)

// 将p + i解释为小端序, p的类型需要为uint8_t*
#define GET_UINT32_LE(p, i)                                            \
(                                                                      \
    ((uint32_t)((const uint8_t *)(p))[(i) + 3] << 24) | /* 24: 4th byte */                \
    ((uint32_t)((const uint8_t *)(p))[(i) + 2] << 16) | /* 16: 3rd byte */                \
    ((uint32_t)((const uint8_t *)(p))[(i) + 1] <<  8) | /*  8: 2nd byte */                \
    ((uint32_t)((const uint8_t *)(p))[(i) + 0] <<  0)   /*  0: 1st byte */                \
)

// 小端序转换位主机序
#define CRYPT_LE32TOH(x)    CRYPT_SWAP32(x)
// 主机序转换为小端序
#define CRYPT_HTOLE32(x)    CRYPT_SWAP32(x)

#else

#define CRYPT_HTONL(x) CRYPT_SWAP32(x)

// 将p + i解释为小端序
#define GET_UINT32_LE(p, i)         \
(                                   \
    (((uintptr_t)(p) & 0x7) != 0) ? ((uint32_t)((const uint8_t *)(p))[(i) + 3] << 24) | /* 24: 4th byte */      \
                                    ((uint32_t)((const uint8_t *)(p))[(i) + 2] << 16) | /* 16: 3rd byte */      \
                                    ((uint32_t)((const uint8_t *)(p))[(i) + 1] <<  8) | /*  8: 2nd byte */      \
                                    ((uint32_t)((const uint8_t *)(p))[(i) + 0] <<  0)   /*  0: 1st byte */      \
                                  : (*(uint32_t *)((uint8_t *)(uintptr_t)(p) + (i)))                            \
)

// 小端序转换位主机序
#define CRYPT_LE32TOH(x)    (x)
// 主机序转换为小端序
#define CRYPT_HTOLE32(x)    (x)

#endif

#ifdef HITLS_BIG_ENDIAN

// 将p + i解释为小端序, p的类型需要为uint8_t*
#define GET_UINT16_LE(p, i)                                            \
(                                                                      \
    ((uint16_t)((const uint8_t *)(p))[(i) + 1] <<  8) | /*  8: 2nd byte */                \
    ((uint16_t)((const uint8_t *)(p))[(i) + 0] <<  0)   /*  0: 1st byte */                \
)
#else
// 将p + i解释为小端序
#define GET_UINT16_LE(p, i)         \
(                                   \
    (((uintptr_t)(p) & 0x7) != 0) ? ((uint16_t)((const uint8_t *)(p))[(i) + 1] <<  8) | /*  8: 2nd byte */      \
                                    ((uint16_t)((const uint8_t *)(p))[(i) + 0] <<  0)   /*  0: 1st byte */      \
                                  : (*(uint16_t *)((uint8_t *)(uintptr_t)(p) + (i)))                            \
)
#endif

/*
 * 64-bit integer manipulation functions (big endian)
 */
static inline uint64_t Uint64FromBeBytes(const uint8_t *bytes)
{
    return (((uint64_t)bytes[0] << 56) |    // 第0位左移7个字节(56)
            ((uint64_t)bytes[1] << 48) |    // 第1位左移6个字节(48)
            ((uint64_t)bytes[2] << 40) |    // 第2位左移5个字节(40)
            ((uint64_t)bytes[3] << 32) |    // 第3位左移4个字节(32)
            ((uint64_t)bytes[4] << 24) |    // 第4位左移3个字节(24)
            ((uint64_t)bytes[5] << 16) |    // 第5位左移2个字节(16)
            ((uint64_t)bytes[6] << 8) |     // 第6位左移1个字节(8)
            (uint64_t)bytes[7]);            // 第7位可直接使用
}

static inline void Uint64ToBeBytes(uint64_t v, uint8_t *bytes)
{
    bytes[0] = (uint8_t)(v >> 56);  // 右移7个字节(56)
    bytes[1] = (uint8_t)(v >> 48);  // 右移6个字节(48)
    bytes[2] = (uint8_t)(v >> 40);  // 第2位右移5个字节(40)
    bytes[3] = (uint8_t)(v >> 32);  // 第3位右移4个字节(32)
    bytes[4] = (uint8_t)(v >> 24);  // 第4位右移3个字节(24)
    bytes[5] = (uint8_t)(v >> 16);  // 第5位右移2个字节(16)
    bytes[6] = (uint8_t)(v >> 8);   // 第6位右移1个字节(8)
    bytes[7] = (uint8_t)(v & 0xffu);    // 第7位使用0xffu将其他位置0
}

uint32_t GetMdSizeById(CRYPT_MD_AlgId id);

static inline uint32_t Uint32ConstTimeMsb(uint32_t a)
{
    // 31 == (4 * 8 - 1)
    return 0u - (a >> 31);
}

static inline uint32_t Uint32ConstTimeIsZero(uint32_t a)
{
    return Uint32ConstTimeMsb(~a & (a - 1));
}

static inline uint32_t Uint32ConstTimeEqual(uint32_t a, uint32_t b)
{
    return Uint32ConstTimeIsZero(a ^ b);
}
// (mask & a) | (~mask & b)
static inline uint32_t Uint32ConstTimeSelect(uint32_t mask, uint32_t a, uint32_t b)
{
    return ((mask) & a) | ((~mask) & b);
}

static inline uint32_t Uint32ConstTimeLt(uint32_t a, uint32_t b)
{
    return Uint32ConstTimeMsb(a ^ ((a ^ b) | ((a - b) ^ a)));
}

static inline uint32_t Uint32ConstTimeGt(uint32_t a, uint32_t b)
{
    return ~Uint32ConstTimeLt(a, b);
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CRYPT_UTILS_H
