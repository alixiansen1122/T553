/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * @file sha2_core.h
 * Description: sha2 内部函数
 */
#ifndef SHA2_CORE_H
#define SHA2_CORE_H
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

#ifndef U64
#define U64(v) (uint64_t)(v)
#endif

void SHA256CompressMultiBlocks(uint32_t hash[8], const uint8_t *in, uint32_t num);
void SHA512CompressMultiBlocks(uint64_t hash[8], const uint8_t *bl, uint32_t bcnt);

#ifdef __cplusplus
}
#endif

#endif
