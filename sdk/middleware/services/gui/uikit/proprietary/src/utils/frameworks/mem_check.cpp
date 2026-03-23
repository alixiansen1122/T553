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

#include "gfx_utils/mem_check.h"
#if ENABLE_MEMORY_CHECK
#include <cstdio>
#include "gfx_utils/graphic_log.h"
#include "hals/gralloc_engines.h"
#ifdef __LITEOS__
#include "los_exc.h"
#endif

namespace OHOS {
MemCheck::~MemCheck()
{
    ClearMemInfoList();
}

void MemCheck::MemCheckInit()
{
    GRAPHIC_LOGI("MemCheck::MemCheckInit");
}

bool MemCheck::InitMemInfoList()
{
    if ((allocHead_ != nullptr) || (allocTail_ != nullptr)) {
        GRAPHIC_LOGE("allocHead_ or allocTail_ is already inited.");
        return false;
    }
    MemAllocInfo* allocNode = (MemAllocInfo*)malloc(sizeof(MemAllocInfo));
    if (allocNode == nullptr) {
        GRAPHIC_LOGE("malloc for allocNode failed!");
        return false;
    }
    allocNode->address = 0;
    allocNode->next = nullptr;
    allocNode->size = 0;

    allocHead_ = allocNode;
    allocTail_ = allocNode;
    return true;
}

void MemCheck::ClearMemInfoList()
{
    if (allocHead_ == nullptr) {
        return;
    }
    MemAllocInfo* allocPtr = allocHead_;
    while (allocPtr != nullptr && allocPtr->next != nullptr) {
        MemAllocInfo* allocNode = allocPtr->next;
        allocPtr->next = allocPtr->next->next;
        free(allocNode);
        allocNode = nullptr;
    }
    free(allocHead_);
    allocHead_ = nullptr;
    allocTail_ = nullptr;
}

void MemCheck::EnableLeakCheck(bool enable)
{
    lock_.Lock();
    if (enableLeakCheck_ == enable) {
        GRAPHIC_LOGE("Already set leak check: %d", enableLeakCheck_);
        lock_.Unlock();
        return;
    }
    if (enable) {
        if (!InitMemInfoList()) {
            GRAPHIC_LOGE("EnableLeakCheck failed!");
            lock_.Unlock();
            return;
        }
        leakCount_ = 0;
        leakHead_ = nullptr;
        GRAPHIC_LOGP("-----Begin Memory Leak Check----");
    } else {
        GRAPHIC_LOGP("total count: %d", osal_atomic_read(&totalCnt_));
        MemAllocInfo *info = leakHead_;
        for (; info != nullptr; info = info->next) {
            GRAPHIC_LOGE("leakcheck: [%d] addr %p size %d", info->count, info->address, info->size);
        }
        crashCnt_ = -1;
        leakSize_ = 0;
        ClearMemInfoList();
        GRAPHIC_LOGP("-----End Memory Leak Check----");
    }
    enableLeakCheck_ = enable;
    lock_.Unlock();
}

bool MemCheck::MemAdd(int size, uintptr_t address)
{
    osal_atomic_inc(&totalCnt_);

    lock_.Lock();
    if (!enableLeakCheck_) {
        lock_.Unlock();
        return false;
    }

    MemAllocInfo* allocNode = (MemAllocInfo*)malloc(sizeof(MemAllocInfo));
    if (allocNode == nullptr) {
        GRAPHIC_LOGE("MemAdd malloc failed");
        lock_.Unlock();
        return false;
    }

    allocNode->address = address;
    allocNode->size = size;
    allocNode->next = nullptr;
    allocNode->count = -1;

    allocTail_->next = allocNode;
    allocTail_ = allocNode;

    leakCount_++;
    allocNode->count = leakCount_;
    if (leakHead_ == nullptr) {
        leakHead_ = allocNode;
    }
    if (leakCount_ == crashCnt_) {
        if (leakSize_ != allocNode->size) {
            GRAPHIC_LOGE("size not right!");
        }
        lock_.Unlock();
#ifdef __LITEOS__
#ifdef LOSCFG_BACKTRACE
        GRAPHIC_LOGE("Mem leak! Check backtrace!");
        LOS_BackTrace();
#else
        LOS_Panic("mem leak here!\n");
#endif
#endif
    }

    lock_.Unlock();
    return true;
}

bool MemCheck::MemDelete(uintptr_t address)
{
    osal_atomic_dec(&totalCnt_);

    lock_.Lock();
    if (!enableLeakCheck_) {
        lock_.Unlock();
        return false;
    }

    if (address == 0) {
        GRAPHIC_LOGE("MemDelete address is nullptr\n");
        lock_.Unlock();
        return false;
    }

    MemAllocInfo* allocPtr = allocHead_;
    while (allocPtr->next != nullptr) {
        if (allocPtr->next->address == address) {
            MemAllocInfo *cur = allocPtr->next;
            leakHead_ = (leakHead_ == cur) ? cur->next : leakHead_;
            MemAllocInfo* tmpInfo = allocPtr->next;
            allocPtr->next = allocPtr->next->next;
            if (allocPtr->next == nullptr) {
                allocTail_ = allocPtr;
            }
            free(tmpInfo);
            tmpInfo = nullptr;
            break;
        }
        allocPtr = allocPtr->next;
    }
    lock_.Unlock();

    return true;
}

void MemCheck::DumpMemInfo()
{
    printf("current malloc/new memory:\n");
    printf("total count: %d\n", osal_atomic_read(&totalCnt_));
    printf("--------\n");
    GrallocEngines::GetInstance()->DumpMem();
}
} // namespace OHOS
#endif
