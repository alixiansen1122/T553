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

#include "graphic_service.h"
#include "common/graphic_startup.h"
#include "common/screen.h"
#include "common/task_manager.h"
#include "components/root_view.h"
#include "gfx_utils/graphic_log.h"
#include "graphic_config.h"
#include "hals/display_dev.h"
#if !defined(_WIN32)
#include "soc_lcd_api.h"
#endif
#include "graphic_thread.h"
#include "dock/input_device.h"
#include "dock/lite_touch_device.h"
#include "hals/lite_input_hal.h"
#include "font/ui_font_header.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "common/screen.h"
#include "components/root_view.h"
#ifdef SUPPORT_OHOSFWK
#include "screennotify/ui_screennotify.h"
#endif
#ifdef SUPPORT_GPU_JPEG
#include "drv_jpeg_intf.h"
#endif
#include "cmsis_os2.h"

VsyncCallback GraphicService::jsVsyncCallback_;
osal_atomic GraphicService::isNativeRunning_{1};
osal_atomic GraphicService::graphicTaskTriggeredCount_{0};
osal_atomic GraphicService::isScreenOn_{0};
OHOS::GraphicMutex GraphicService::rtcEventMutex_;
OHOS::GraphicCond GraphicService::rtcEventCond_;
OHOS::GraphicMutex GraphicService::jsScreenOffMutex_;
bool GraphicService::isJsTaskFinished_ = true;
#ifdef _WIN32
static uint32_t nativeUITaskId_ = 0xFFFFFFFF;
#endif
static ThreadId g_rtcEventThreadId;
static ThreadId g_mainThreadId;
GraphicService::GraphicService() {}

GraphicService::~GraphicService() {}

void GraphicService::RegJSVsyncCallback(VsyncCallback callback)
{
    jsVsyncCallback_ = std::move(callback);
}

bool GraphicService::SyncTriggerJSGraphicTask()
{
    if ((IsNativeRunning() == 1) || (GetGraphicTaskTriggeredCount() > 0) || (IsScreenOn() == 0)) {
        GRAPHIC_LOGI("Trigger JS Task failed. Native is running or Graphic Task is already triggered.");
        return false;
    }
    isJsTaskFinished_ = false;
    jsScreenOffMutex_.Lock();
    OHOS::TaskManager::GetInstance()->TaskHandler();
    jsScreenOffMutex_.Unlock();
    isJsTaskFinished_ = true;
    return true;
}

bool GraphicService::AsyncTriggerNativeGraphicTask()
{
    if (GraphicService::GetInstance()->GetGraphicTaskTriggeredCount() <= 1) {
        osal_atomic_inc(&graphicTaskTriggeredCount_);
        OHOS::GraphicEventHandler::GetInstance()->PostGraphicEvent(
            [] {
                if (GraphicService::GetInstance()->IsScreenOn() &&
                    isJsTaskFinished_ && // 避免JS->Native TaskderHander多线程重入
                    GraphicService::GetInstance()->IsNativeRunning()) { // 避免Native->JS 多线程问题
                    OHOS::TaskManager::GetInstance()->TaskHandler();
                } else {
                    GRAPHIC_LOGW("ignore redundant task when screen is off or js task is not finished\n");
                }
                GraphicService::GetInstance()->DecreaseTriggerCount();
            },
            false);
        return true;
    }
    return false;
}

void GraphicService::SetNativeUIRunning(bool isRunning)
{
    switchMutex_.Lock();
    if (isRunning) {
        osal_atomic_set(&isNativeRunning_, 1);
    } else {
        osal_atomic_set(&isNativeRunning_, 0);
        osal_atomic_set(&graphicTaskTriggeredCount_, 0);
    }
    switchMutex_.Unlock();
}

int GraphicService::IsNativeRunning() const
{
    return osal_atomic_read(&isNativeRunning_);
}

int GraphicService::GetGraphicTaskTriggeredCount() const
{
    return osal_atomic_read(&graphicTaskTriggeredCount_);
}

int GraphicService::IsScreenOn() const
{
    return osal_atomic_read(&isScreenOn_);
}

void GraphicService::DecreaseTriggerCount()
{
    osal_atomic_dec(&graphicTaskTriggeredCount_);
}

void GraphicService::PostGraphicEvent(const OHOS::GraphicEvent& event)
{
    OHOS::GraphicEventHandler::GetInstance()->PostGraphicEvent(event);
}

void GraphicService::NotifyScreenOn()
{
#ifdef SUPPORT_OHOSFWK
    notify_screen_on_event();
#endif
    OHOS::RootView::GetInstance()->Invalidate(); // full refresh

    osal_atomic_set(&isScreenOn_, 1);
}

void GraphicService::NotifyScreenOff()
{
#ifdef SUPPORT_OHOSFWK
    notify_screen_off_event();
#endif
    jsScreenOffMutex_.Lock();
    EnableAsyncMode(false); // flush pending frame if necessary
    jsScreenOffMutex_.Unlock();
    osal_atomic_set(&isScreenOn_, 0);
}

void GraphicService::GpuResetStart(void)
{
    isGpuReseting = true;
    OHOS::GraphicEventHandler::GetInstance()->ClearGraphicEvent();
    osal_atomic_set(&graphicTaskTriggeredCount_, 0);
#ifdef SUPPORT_GPU_JPEG
    uapi_jpeg_video_sub_reset();
#endif
}

void GraphicService::GpuResetEnd(void)
{
    isGpuReseting = false;
}

void GraphicService::ForceRefreshImmediately()
{
    using namespace OHOS;
    if (GraphicService::GetInstance()->GetGraphicTaskTriggeredCount() <= 0) {
        osal_atomic_inc(&graphicTaskTriggeredCount_);
        GraphicEventHandler::GetInstance()->PostGraphicEvent([] {
            if (GraphicService::GetInstance()->IsScreenOn() && isJsTaskFinished_) {
                TaskManager::GetInstance()->TaskHandler();
                LiteMGfxEngine::GetInstance()->SyncHwDraw();
            } else {
                GRAPHIC_LOGW("ignore redundant task when screen is off or js task is not finished");
            }
            GraphicService::GetInstance()->DecreaseTriggerCount();
        });
        return;
    }
    GraphicEventHandler::GetInstance()->PostGraphicEvent([]() {
        if (GraphicService::GetInstance()->IsScreenOn() && isJsTaskFinished_) {
            LiteMGfxEngine::GetInstance()->SyncHwDraw();
        }
    });
}

void GraphicService::ForceRefreshMore()
{
    using namespace OHOS;
    osal_atomic_inc(&graphicTaskTriggeredCount_);
    GraphicEventHandler::GetInstance()->PostGraphicEvent([] {
        if (GraphicService::GetInstance()->IsScreenOn() && isJsTaskFinished_) {
            RootView::GetInstance()->Invalidate();
            TaskManager::GetInstance()->TaskHandler();
            LiteMGfxEngine::GetInstance()->SyncHwDraw();
        } else {
            GRAPHIC_LOGW("ignore redundant task when screen is off or js task is not finished");
        }
        GraphicService::GetInstance()->DecreaseTriggerCount();
    });
}

void GraphicService::EnableAsyncMode(bool enable)
{
    OHOS::LiteMGfxEngine::GetInstance()->EnableAsyncMode(enable);
}

bool GraphicService::IsNativeUITask()
{
#ifdef _WIN32
    return nativeUITaskId_ == GetCurrentThreadId();
#else
    return (strcmp(osThreadGetName(osThreadGetId()), "GuiMainThread") == 0);
#endif
}

void GraphicService::NativeMainThread(void* args)
{
#ifdef _WIN32
    nativeUITaskId_ = GetCurrentThreadId();
#endif
    OHOS::GraphicEventHandler::GetInstance()->Run();
}

void GraphicService::UpdateGraphicEvent()
{
    if (isGpuReseting) {
        return;
    }
    rtcEventCond_.Signal();
}

void GraphicService::RtcEventThread(void* args)
{
    while (true) {
        rtcEventMutex_.Lock();
        rtcEventCond_.Wait(rtcEventMutex_);
        rtcEventMutex_.Unlock();
        if (GraphicService::GetInstance()->IsScreenOn() == 0) {
            continue;
        }

        if (GraphicService::GetInstance()->IsNativeRunning() == 1) {
            AsyncTriggerNativeGraphicTask();
        } else {
            jsVsyncCallback_();
        }
    }
}

void GraphicService::SetUpRootView(void)
{
    OHOS::RootView::GetInstance()->SetPosition(0, 0, OHOS::Screen::GetInstance().GetWidth(),
        OHOS::Screen::GetInstance().GetHeight());
    OHOS::RootView::GetInstance()->SetStyle(OHOS::STYLE_BACKGROUND_COLOR, OHOS::Color::Black().full);
}

bool GraphicService::InitGraphicService()
{
#if !defined(_WIN32)
    if (!uapi_is_lcd_connect()) {
        return false;
    }
#endif

    OHOS::GraphicStartUp::Init();
#if !defined(_WIN32)
    OHOS::LiteInputHal::GetInstance()->SetUp();
#endif
#ifndef QSPI_DISPLAY
    OHOS::BaseGfxEngine::GetInstance()->SetScreenShape(OHOS::CIRCLE);
#endif
    SetUpRootView();
    UIThreadAttr rtcAttr;
    rtcAttr.name = "RtcEventThread";
    rtcAttr.stackSize = 0x800u;
    rtcAttr.priority = static_cast<uint8_t>(osPriorityAboveNormal2);
    g_rtcEventThreadId = (ThreadId)ThreadCreate((Runnable)GraphicService::RtcEventThread, nullptr, &rtcAttr);
    if (g_rtcEventThreadId == nullptr) {
        GRAPHIC_LOGE("%s: create thread fail\n", __func__);
        return false;
    }

    UIThreadAttr mainAttr;
    mainAttr.name = "GuiMainThread";
    mainAttr.stackSize = 0x6000u;
    mainAttr.priority = static_cast<uint8_t>(osPriorityAboveNormal2);
    g_mainThreadId = (ThreadId)ThreadCreate((Runnable)GraphicService::NativeMainThread, nullptr, &mainAttr);
    if (g_mainThreadId == nullptr) {
        GRAPHIC_LOGE("create thread fail\n");
        return false;
    }
    return true;
}
