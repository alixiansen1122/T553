/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file noasm_ghash.c
 * Description: ghash算法实现, C实现，对应有相应的汇编加速实现
 * Author: linkaixin
 * Create: 2022-05-13
 * Modification History
 *  DATE        NAME             DESCRIPTION
 *  --------------------------------------------------------------------------
 *  2022-05-13  linkaixin        gcm模式
 */
#ifndef HITLS_CRYPTO_NO_GCM
#include "modes_local.h"
#include "crypt_utils.h"
#include "sal_mem.h"

/* table[i] = (P^4)*i, P = 0x4000000000000000, i = 0...16 */
static const uint64_t TABLE_P4_BITS[16] = {
    0x0000000000000000, 0x1c20000000000000, 0x3840000000000000, 0x2460000000000000,
    0x7080000000000000, 0x6ca0000000000000, 0x48c0000000000000, 0x54e0000000000000,
    0xe100000000000000, 0xfd20000000000000, 0xd940000000000000, 0xc560000000000000,
    0x9180000000000000, 0x8da0000000000000, 0xa9c0000000000000, 0xb5e0000000000000
};

// 计算 n*H n为0 .... 16
void GcmTableGen4bit(uint8_t key[GCM_BLOCKSIZE], MODES_GCM_GF128 hTable[16])
{
    uint32_t i;
    uint32_t j;
    const uint64_t r = 0xE100000000000000;
    hTable[0].h = 0;
    hTable[0].l = 0;
    hTable[8].h = Uint64FromBeBytes(key); // 表的中间项(16 / 2 == 8)为H本身
    hTable[8].l = Uint64FromBeBytes(key + sizeof(uint64_t)); // 表的中间项(16 / 2 == 8)为H本身

    for (i = 4; i > 0; i >>= 1) { // 4bit表，先计算2^n项的值
        hTable[i].l = (hTable[ i * 2].h << 63) | (hTable[ i * 2].l >> 1); // 循环右移1bit h的高1位于l的低63位结合
        hTable[i].h = (hTable[ i * 2].h >> 1) ^ ((hTable[ i * 2].l & 1) * r); // 2^n项的值
    }
    for (i = 1; i < 16; i <<= 1) { // 16个表项
        for (j = 1; j < i; j++) {
            hTable[i + j].h = hTable[i].h ^ hTable[j].h;
            hTable[i + j].l = hTable[i].l ^ hTable[j].l;
        }
    }
}

// 计算 t = t * H
void GcmHashMultiBlock(uint8_t t[GCM_BLOCKSIZE], const MODES_GCM_GF128 hTable[16], const uint8_t *in, uint32_t inLen)
{
    MODES_GCM_GF128 x;
    uint8_t r;
    uint8_t h, l, tag;   // 密文信息，摘要信息，非敏感信息
    const uint8_t *tempIn = in;
    for (uint32_t i = 0; i < inLen; i += GCM_BLOCKSIZE) {
        uint8_t cnt = GCM_BLOCKSIZE - 1;
        x.h = 0;
        x.l = 0;
        while (1) {
            tag = t[cnt] ^ tempIn[cnt];

            l = tag & 0xf;
            h = (tag >> 4) & 0xf; // 高4bit
            x.h ^= hTable[l].h;
            x.l ^= hTable[l].l;

            r = (x.l & 0xf);
            x.l  = (x.h << 60) | (x.l >> 4); // 循环右移4bit h的高4位于l的低60位结合
            x.h  = (x.h >> 4); // 循环右移4bit
            x.h ^= TABLE_P4_BITS[r];

            x.h ^= hTable[h].h;
            x.l ^= hTable[h].l;
            if (cnt == 0) {
                break;
            }
            cnt--;
            r = (x.l & 0xf);
            x.l  = (x.h << 60) | (x.l >> 4); // 循环右移4bit h的高4位于l的低60位结合
            x.h  = (x.h >> 4); // 循环右移4bit
            x.h ^= TABLE_P4_BITS[r];
        }
        tempIn += GCM_BLOCKSIZE;
        Uint64ToBeBytes(x.h, t);        // 高8位
        Uint64ToBeBytes(x.l, t + 8);    // 低8位
    }
    // 敏感信息清除
    SAL_CleanseData(&x, sizeof(MODES_GCM_GF128));
    SAL_CleanseData(&r, sizeof(uint8_t));
}
#endif