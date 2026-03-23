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

#include "graphic_service_wrapper.h"
#if !defined(_WIN32)
#include "rtc.h"
#endif
#include "graphic_service.h"
#include "gfx_utils/graphic_log.h"
#include "common/low_power_manager.h"
#include "common/image_cache_manager.h"
#include "common/screen.h"
#include "components/root_view.h"
#include "components/ui_image_view.h"

#ifdef __cplusplus
extern "C" {
#endif
static const uint32_t SECOND_TO_MILLISECOND = 1000;
static const int32_t FULL_FREQUENCE = 60;
#if !defined(_WIN32)
static rtc_handle_t g_rtcHandle = nullptr;
#endif
static int32_t g_timeInterval = -1;
static int32_t g_freq = 0;
static bool g_initialized = false;
bool g_isScreenOn = false;
static bool g_showLogo = false;
static OHOS::UIImageView* g_logo = nullptr;
static OHOS::ImageInfo* g_imgInfo = nullptr;

#ifdef QSPI_DISPLAY
#define IMAGE_OPEN_LOGO_PATH "/user/res/OPEN_LOGO_QSPI.bin"
#else
#define IMAGE_OPEN_LOGO_PATH "/user/res/OPEN_LOGO.bin"
#endif

static void UikitRTCCallBack(uintptr_t data)
{
    (void)(data);
    if (!g_initialized || !g_isScreenOn) {
        GRAPHIC_LOGE("RTC should not be started when g_isScreenOn:%d g_initialized:%d", g_isScreenOn, g_initialized);
        return;
    }
#if !defined(_WIN32)
    uapi_rtc_start(g_rtcHandle, g_timeInterval, UikitRTCCallBack, 0);
#endif
    GraphicService::GetInstance()->UpdateGraphicEvent();
}

void SetFrequenceInner(int freq)
{
    if (!g_isScreenOn) {
        return;
    }

    if (freq > FULL_FREQUENCE) {
        freq = FULL_FREQUENCE;
    }

    if ((g_freq != freq) && (freq != 0)) {
        g_freq = freq;
        g_timeInterval = SECOND_TO_MILLISECOND / freq;
        GRAPHIC_LOGI("set freq: %d, time: %dms\n", freq, g_timeInterval);
        if (freq == FULL_FREQUENCE) {
            if (!OHOS::LowPowerManager::GetInstance()->IsLiteSurfaceInRootView()) {
                GraphicService::GetInstance()->EnableAsyncMode(true);
            }
        } else {
            GraphicService::GetInstance()->EnableAsyncMode(false);
        }
#if !defined(_WIN32)
        uapi_rtc_start(g_rtcHandle, g_timeInterval, UikitRTCCallBack, 0);
#endif
    }
}

int GetFrequence(void)
{
    return g_freq;
}

int IsNativeRunning(void)
{
    return GraphicService::GetInstance()->IsNativeRunning();
}

void SetFrequence(int freq)
{
    if (freq <= 0 || freq > FULL_FREQUENCE) {
        GRAPHIC_LOGE("freq %d should be in (0, 60]", freq);
        return;
    }
    GraphicService::GetInstance()->PostGraphicEvent([=] {
        OHOS::LowPowerManager::GetInstance()->SetMaxFrequence(freq);
    });
}

void NotifyScreenOn(void)
{
    GraphicService::GetInstance()->NotifyScreenOn();
#if !defined(_WIN32)
    if (g_rtcHandle == nullptr) {
        return;
    }
#endif
    if (!g_isScreenOn) {
        g_isScreenOn = true;
        OHOS::LowPowerManager::GetInstance()->ExitLowPower();
    }
}

void NotifyScreenOff(void)
{
#if !defined(_WIN32)
    if (g_rtcHandle != nullptr) {
        uapi_rtc_stop(g_rtcHandle);
        g_freq = 0;
        g_timeInterval = -1;
    }
#endif
    g_isScreenOn = false;
    GraphicService::GetInstance()->NotifyScreenOff();
}

void InitGraphicService(void)
{
    if (g_initialized) {
        return;
    }

#if !defined(_WIN32)
    /* create RTC timer */
    uapi_rtc_create(RTC_0, &g_rtcHandle);
    if (g_rtcHandle == nullptr) {
        GRAPHIC_LOGE("create RTC timer fail!!");
        return;
    }
#endif

    if (!GraphicService::GetInstance()->InitGraphicService()) {
        GRAPHIC_LOGE("InitGraphicService fail!!");
#if !defined(_WIN32)
        uapi_rtc_stop(g_rtcHandle);
        uapi_rtc_delete(g_rtcHandle);
#endif
        return;
    }
    g_initialized = true;
}

void ShowGraphicLogo()
{
    if (g_showLogo) {
        return;
    }
    g_showLogo = true;
    g_logo = new OHOS::UIImageView();
    g_logo->SetPosition(0, 0, OHOS::Screen::GetInstance().GetWidth(), OHOS::Screen::GetInstance().GetHeight());
    g_logo->SetAutoEnable(false);
    g_logo->SetResizeMode(OHOS::UIImageView::CENTER);
    g_imgInfo = OHOS::ImageCacheManager::GetInstance().LoadSingleRes(IMAGE_OPEN_LOGO_PATH);
    g_logo->SetSrc(g_imgInfo);
    GraphicService::GetInstance()->PostGraphicEvent(
        [] {
            OHOS::RootView::GetInstance()->Add(g_logo);
            OHOS::RootView::GetInstance()->Invalidate();
        });
}

void HideGraphicLogo()
{
    if (!g_showLogo) {
        return;
    }
    g_showLogo = false;
    GraphicService::GetInstance()->PostGraphicEvent(
        [] {
            if (g_logo != nullptr) {
                OHOS::RootView::GetInstance()->Remove(g_logo);
                delete g_logo;
                g_logo = nullptr;
                OHOS::ImageCacheManager::GetInstance().UnloadSingleRes(IMAGE_OPEN_LOGO_PATH);
                g_imgInfo = nullptr;
            }
        });
}

void PostGraphicEvent(GraphicEventCb eventCb, void* userData)
{
    GraphicService::GetInstance()->PostGraphicEvent(
        [=] {
            eventCb(userData);
            });
}

void GpuResetStart(void)
{
    GraphicService::GetInstance()->GpuResetStart();
}

void GpuResetEnd(void)
{
    GraphicService::GetInstance()->GpuResetEnd();
}

#ifdef __cplusplus
};
#endif
