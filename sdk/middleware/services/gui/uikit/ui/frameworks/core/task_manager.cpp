/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "common/task_manager.h"
#if CAPABILITY_SW_DFX_FRAME_TRACE
#include "dfx/dfx_frame_trace.h"
#endif

#include "gfx_utils/graphic_log.h"
#include "hal_tick.h"

namespace OHOS {
TaskManager* TaskManager::GetInstance()
{
    static TaskManager taskManager;
    return &taskManager;
}
void TaskManager::Add(Task* task)
{
    if (task == nullptr) {
        return;
    }

    ListNode<Task*>* pos = list_.Begin();
    while (pos != list_.End()) {
        if (pos->data_ == task) {
            GRAPHIC_LOGI("do not add task multi times");
            return;
        }
        pos = pos->next_;
    }

    list_.PushFront(task);
}

void TaskManager::Remove(Task* task)
{
    if (task == nullptr) {
        return;
    }
    ListNode<Task*>* pos = list_.Begin();
    while (pos != list_.End()) {
        if (pos->data_ == task) {
            list_.Remove(pos);
            return;
        }
        pos = pos->next_;
    }
}

void TaskManager::TaskHandler()
{
    if (!canTaskRun_) {
        return;
    }

    if (isHandlerRunning_) {
        return;
    }
    isHandlerRunning_ = true;

    ListNode<Task*>* node = list_.Begin();

    while (node != list_.End()) {
        Task* currentTask = node->data_;
#if CAPABILITY_SW_DFX_FRAME_TRACE
        TASK_TRACE_START();
        FRAME_TRACE_BEGIN_TRACE();
#endif
        currentTask->TaskExecute();
#if CAPABILITY_SW_DFX_FRAME_TRACE
        FRAME_TRACE_SHOW();
        FRAME_TRACE_END_TRACE();
        TASK_TRACE_END();
#endif

        node = node->next_;
    }

    isHandlerRunning_ = false;
}

uint32_t TaskManager::GetMinimumCustomerTaskPeriod()
{
    if (list_.Size() <= 3) { // 3: only has 3 main task for uikit
        return 0;
    } else {
        uint32_t period = 0xffff;
        int count = 0;
        int num = list_.Size() - 3;
        ListNode<Task*>* node = list_.Begin();
        while (count < num) {
            Task* currentTask = node->data_;
            if (currentTask->GetPeriod() < period && currentTask->GetPeriod() >= DEFAULT_TASK_PERIOD) {
                period = currentTask->GetPeriod();
            }
            count++;
            node = node->next_;
        }
        if (period == 0xffff) {
            return 0;
        } else {
            return period;
        }
    }
}

uint16_t TaskManager::GetTotalTaskNum()
{
    return list_.Size();
}

/**
 * @brief TaskManager::ResetTaskHandlerMutex Reset the running flag.
 *
 * Because the rending implementation is shared between native and third-party APP under liteos-m, and for
 * some exception cases (the app is destroyed unexpectedly during the task handling process), as the task
 * handling process will set the flag to true for avoiding reentry of the handling process itself, and as
 * TaskManager is one single instance, so must reset this flag to false for other task's normal rendring
 * process, for example, native app.
 */
void TaskManager::ResetTaskHandlerMutex()
{
    isHandlerRunning_ = false;
}
} // namespace OHOS
