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
 * @addtogroup Graphic_Service
 * @{
 *
 * @brief Defines graphic service api for customer.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef OHOS_GRAPHIC_SERVICE_H
#define OHOS_GRAPHIC_SERVICE_H
#include "graphic_event_handler.h"
#include "soc_osal.h"

/**
* @ignore
*/
using VsyncCallback = std::function<void()>;
/**
* @brief Provide api for customer to access graphic service.
*
* @since 1.0
* @version 1.0
*/
class GraphicService {
public:
    static GraphicService* GetInstance()
    {
        static GraphicService instance;
        return &instance;
    }

    /**
     * @brief Register a VsyncCallback for JS APP, which will be called every vsync period.
     * @param callback, a function registered for later callback.
     */
    void RegJSVsyncCallback(VsyncCallback callback);

    /**
     * @brief Handle tasks triggered by JS APP synchronously.
     */
    bool SyncTriggerJSGraphicTask();

    /**
     * @brief Handle tasks triggered by Native APP in an asynchronous thread.
     */
    static bool AsyncTriggerNativeGraphicTask();

    /**
     * @brief Update graphic event.
     */
    void UpdateGraphicEvent();

    /**
     * @brief Set Native UI running state.
     * @param running, indicates whether Native UI is Running or not.
     */
    void SetNativeUIRunning(bool isRunning);

    /**
     * @brief Return whether Native UI is Running or not.
     */
    int IsNativeRunning() const;

    /**
     * @brief Return whether current task is native ui task
     */
    static bool IsNativeUITask();

    /**
     * @brief Return whether screen is on or not.
     */
    int IsScreenOn() const;

    /**
     * @brief Return graphic task triggered count.
     */
    int GetGraphicTaskTriggeredCount() const;

    /**
     * @brief Post a task to an asynchronous thread, which would handle posted tasks sequently.
     * @param event, indicates the task to post.
     */
    void PostGraphicEvent(const OHOS::GraphicEvent& event);

    /**
     * @brief Notify the Screen is on.
     */
    void NotifyScreenOn();

    /**
     * @brief Notify the Screen is off.
     */
    void NotifyScreenOff();

    /**
     * @brief Set async render mode, default false.
     * @note 1, This api need to be called in uikit main thread;
     *       2, You need disable async mode before free resources.
     */
    void EnableAsyncMode(bool enable);

    /**
     * @brief start up graphic service.
     */
    bool InitGraphicService();

    /**
    * @brief GPU Reset start.
    */
    void GpuResetStart(void);

    /**
    * @brief GPU Reset end.
    */
    void GpuResetEnd(void);

    void ForceRefreshImmediately();

    void ForceRefreshMore();

    static void NativeMainThread(void* args);
    static void RtcEventThread(void* args);

    OHOS::GraphicMutex switchMutex_; // ensure that js syncHwDraw and native SetNativeUIRunning run in sync
private:
    friend class GraphicEventHandler;
    static VsyncCallback jsVsyncCallback_;
    static osal_atomic isNativeRunning_;
    static osal_atomic graphicTaskTriggeredCount_;
    static osal_atomic isScreenOn_;
    static OHOS::GraphicMutex rtcEventMutex_;
    static OHOS::GraphicMutex jsScreenOffMutex_;
    static OHOS::GraphicCond rtcEventCond_;
    static bool isJsTaskFinished_;  // ensure that native task run after js task is finished
    bool isGpuReseting = false;
    void DecreaseTriggerCount();
    void SetUpRootView();
    GraphicService();
    virtual ~GraphicService();
};
#endif // OHOS_GRAPHIC_SERVICE_H
/**
 * @}
 */
