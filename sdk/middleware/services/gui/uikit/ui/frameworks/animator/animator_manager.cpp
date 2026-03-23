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

#include "animator/animator_manager.h"

#include "common/task_manager.h"
#include "hal_tick.h"

namespace OHOS {
AnimatorManager* AnimatorManager::GetInstance()
{
    static AnimatorManager animatorManager;
    return &animatorManager;
}

void AnimatorManager::Init()
{
    SetPeriod(0); // Animator的默认处理周期受RTC/TE刷新周期控制
    Task::Init();
}

void AnimatorManager::Add(Animator* animator)
{
    if (animator == nullptr) {
        return;
    }

    ListNode<Animator*>* pos = list_.Begin();
    while (pos != list_.End()) {
        if (pos->data_ == animator) {
            GRAPHIC_LOGI("do not add animator multi times");
            return;
        }
        pos = pos->next_;
    }

    list_.PushBack(animator);
}

void AnimatorManager::Remove(const Animator* animator)
{
    if (animator == nullptr) {
        return;
    }
    ListNode<Animator*>* pos = list_.Begin();
    while (pos != list_.End()) {
        if (pos->data_ == animator) {
            pos->data_ = nullptr;
            return;
        }
        pos = pos->next_;
    }
}

bool AnimatorManager::IsActive()
{
    ListNode<Animator*>* pos = list_.Begin();
    Animator* animator = nullptr;

    while (pos != list_.End()) {
        animator = pos->data_;
        if ((animator != nullptr) && (animator->GetState() == Animator::START)) {
            return true;
        }
        pos = pos->next_;
    }
    return false;
}

void AnimatorManager::AnimatorTask()
{
    ListNode<Animator*>* pos = list_.Begin();
    Animator* animator = nullptr;

    activeNum_ = 0;
    while (pos != list_.End()) {
        animator = pos->data_;
        if (animator == nullptr) {
            // clean up animator list
            pos = list_.Remove(pos);
            continue;
        }
        if (animator->GetState() == Animator::START) {
            activeNum_++;
            animator->Run();
        }
        pos = pos->next_;
    }
}

uint16_t AnimatorManager::GetTotalAnimatorNum()
{
    return list_.Size();
}

uint16_t AnimatorManager::GetActiveAnimatorNum()
{
    return activeNum_;
}
} // namespace OHOS
