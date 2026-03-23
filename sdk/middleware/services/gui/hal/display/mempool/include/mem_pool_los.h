/*
 * Copyright (c) 2022 CompanyNameMagicTag.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MEM_POOL_LOS_H
#define MEM_POOL_LOS_H
#if defined(__LITEOS_M__) || defined(__FREERTOS__)
#include <stdint.h>
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @brief Register a memory pool.
 * @param [in] Pointer to memory.
 * @param [in] Size of cache memory in bytes.
 * @param [in] Maxmum number of dynamic alloc allowed, 0 means no constraint.
 * @param [in] Minimum size of dynamic alloc allowed, 0 means no constraint.
 * @return uint8_t*
 */
uint8_t *LosRegisterMemPool(uint8_t *pool, uint32_t poolSize, uint32_t maxItemCount, uint32_t minItemSize);

/**
 * @brief UnRegister memory pool.
 * @param [in] uint8_t* pool id.
 */
void LosUnregisterMemPool(uint8_t *poolID);

/**
 * @brief Allocate a memory dynamically.
 * @param [in] uint8_t* pool id.
 * @param [in] allocSize size of the memory requested in bytes.
 */
uint8_t *LosAllocMem(uint8_t *poolID, uint32_t allocSize);

/**
 * @brief Free an allocated memory.
 * @param [in] uint8_t* pool id.
 * @param [in] addr memory addr.
 */
void LosFreeMem(uint8_t *poolID, uint8_t *addr);
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif
#endif
