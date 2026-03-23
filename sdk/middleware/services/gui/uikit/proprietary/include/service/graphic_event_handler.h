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

#ifndef OHOS_GRAPHIC_EVENT_HANDLER_H
#define OHOS_GRAPHIC_EVENT_HANDLER_H
#include <functional>
#include <queue>
#include "graphic_mutex.h"

namespace OHOS {
using GraphicEvent = std::function<void()>;

class GraphicEventHandler {
public:
    static GraphicEventHandler* GetInstance()
    {
        static GraphicEventHandler instance;
        return &instance;
    }

    /**
     * @brief Begin to handle tasks in eventQueue sequently.
     *        If eventQueue is empty, it will wait until a new event was posted.
     */
    void Run();

    /**
     * @brief Post a task to the eventQueue.
     * @param event, indicates the task to post.
     */
    void PostGraphicEvent(const GraphicEvent& event, bool exitLowPower = true);
    void ClearGraphicEvent();

private:
    std::queue<GraphicEvent> eventQueue_;
    GraphicMutex queueMutex_;
    GraphicCond pthreadCond_;

    GraphicEventHandler();
    virtual ~GraphicEventHandler();
};
} // namespace OHOS
#endif // OHOS_GRAPHIC_EVENT_HANDLER_H
