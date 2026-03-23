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

/**
 * @addtogroup UI_Utils
 * @{
 *
 * @brief Defines basic UI utils.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file mem_check.h
 *
 * @brief Provide dfx method for memory check.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_MEM_CHECK_H
#define GRAPHIC_LITE_MEM_CHECK_H

#include <cstdio>
#include <list>
#include "gfx_utils/mem_api.h"
#include "graphic_thread.h"
#include "graphic_mutex.h"
#include "soc_osal.h"

#if ENABLE_MEMORY_CHECK
namespace OHOS {
/**
 * @brief Memory alloc info.
 *
 */
struct MemAllocInfo {
    uintptr_t address;
    int32_t size;
    int16_t count;
    struct MemAllocInfo* next;
};

/**
 * @brief Memory statistics and check.
 *
 */
class MemCheck {
public:
    /**
     * @brief Get instance of memory check info.
     *
     * @returns instance of memory check info
     */
    static MemCheck* GetInstance()
    {
        static MemCheck instance;
        return &instance;
    }

    /**
     * @brief Add dynamic memory. You can call it when add dynamic memory.
     *
     * @param size Indicates the size of the memory to apply for.
     * @param address Indicates the address of the memory to apply for.
     * @since 1.0
     * @version 1.0
     */
    bool MemAdd(int size, uintptr_t address);

    /**
     * @brief Reduce dynamic memory. You can call it when reduce dynamic memory.
     *
     * @param address Indicates the address of the memory to apply for.
     * @since 1.0
     * @version 1.0
     */
    bool MemDelete(uintptr_t address);

    /**
     * @brief print current memory info
     *
     * @since 1.0
     * @version 1.0
     */
    void DumpMemInfo();

    /**
     * @brief Memory check deinit for dynamic memory. You must call this function in destroy application.
     *
     * @since 1.0
     * @version 1.0
     */
    void MemCheckInit();

    void EnableLeakCheck(bool enable);

    void Crash(int16_t cnt, int32_t sz)
    {
        crashCnt_ = cnt;
        leakSize_ = sz;
    }

private:
    MemCheck() {}
    ~MemCheck();
    MemCheck(const MemCheck &) = delete;
    MemCheck &operator=(const MemCheck &) = delete;

    bool InitMemInfoList();
    void ClearMemInfoList();

    MemAllocInfo* allocHead_ = nullptr;
    MemAllocInfo* allocTail_ = nullptr;
    GraphicMutex lock_;
    osal_atomic totalCnt_;
    bool enableLeakCheck_ = false;
    MemAllocInfo *leakHead_ = nullptr;
    int16_t crashCnt_ = -1;
    int16_t leakCount_ = 0;
    int32_t leakSize_ = 0;
};
} // namespace OHOS
#endif
#endif
/**
 * @}
 */
