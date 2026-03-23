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

#ifndef FPS_TASK_H
#define FPS_TASK_H

#include "graphic_config.h"
#if ENABLE_DFX_CMD
#include <atomic>
#include "common/task_manager.h"
#include "components/ui_label.h"

namespace OHOS {
class FPSTask : public Task {
public:
    static FPSTask *GetInstance()
    {
        static FPSTask instance;
        return &instance;
    }

    void Callback() override;
    void ShowFPS(int left = 0, int top = 0);
    void HideFPS();
    void PrintFPS(uint16_t duration = 0);

private:
    FPSTask();
    ~FPSTask() {}
    UILabel *fpsLabel_ = nullptr;
    bool isFirstShow_ = true;
    bool isAdded_ = false;
    bool needShow_ = false;
    bool needPrint_ = false;
    uint32_t showStartTime_ = 0;
    uint32_t showCount_ = 0;
    uint32_t printStartTime_ = 0;
    uint32_t printCount_ = 0;
    uint32_t printDuration_ = 0;
    int16_t left_ = 0;
    int16_t top_ = 0;
};
}
#endif // ENABLE_DFX_CMD
#endif  // FPS_TASK_H