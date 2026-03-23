/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#include "graphic_service_wrapper.h"
#include "lv_mutex.h"
#include "cmsis_os2.h"
#include "rtc.h"
#include "lvgl.h"
#include "lv_conf_ext.h"
#if LV_USE_LOW_POWER_MANAGER
#include "lv_low_power_manager.h"
#endif
#include "lv_img_cache_manager.h"
#ifdef SUPPORT_GPU_JPEG
#include "drv_jpeg_intf.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
static const uint32_t SECOND_TO_MILLISECOND = 1000;
static uint32_t g_taskHandlerEventCount = 0;
static bool g_isScreenOn = false;
static LvMutex g_screenMutex;
static LvMutex g_taskHandlerEventMutex;
static rtc_handle_t g_rtcHandle = NULL;
static uint32_t g_timeInterval = 16;
static uint32_t g_frequence = 60;
static bool g_initialized = false;
static bool g_fullRefreshFlag = false;
static void* g_mainThreadId = NULL;
static void RtcCallback(void* args);
static LvMutex g_eventQueueMutex;
static LvCondition g_eventQueueCond;
static lv_ll_t g_eventQueueList;
static void* g_rtcThreadId = NULL;
static LvMutex g_rtcEventMutex;
static LvCondition g_rtcEventCond;
static bool g_isGpuReseting = false;

typedef struct {
    GraphicEventCb eventCb;
    void* userData;
} GraphicEvent;

static void InitModule(void)
{
    _lv_ll_init(&g_eventQueueList, sizeof(GraphicEvent));
    LvInitMutex(&g_eventQueueMutex);
    LvInitCondition(&g_eventQueueCond);
    LvInitMutex(&g_screenMutex);
    LvInitMutex(&g_taskHandlerEventMutex);
    LvInitMutex(&g_rtcEventMutex);
    LvInitCondition(&g_rtcEventCond);

    /* init lvgl system */
    lv_init();
#if LV_USE_VGU
    /* init vgu module */
    LvVguModuleInit();
#endif
    /* init disp devices */
    LvPortDispInit(RESOLUTION_WIDTH, RESOLUTION_HEIGHT);
    /* init input devices */
    LvPortIndevInit();
#if LV_USE_IMG_EXT
    LvFileDecoderInit();
#if LV_USE_HW_JPEG_DECODER
    LvHwJpegDecoderInit();
#endif
    LvImgCacheManagerInit();
#endif
}

static void RunEventQueue(void)
{
    while (true) {
        LvLock(&g_eventQueueMutex);
        if (_lv_ll_is_empty(&g_eventQueueList)) {
#if LV_USE_LOW_POWER_MANAGER
            TryToEnterLowPower();
#endif
            LvWaitOnCondition(&g_eventQueueMutex, &g_eventQueueCond);
        }
        GraphicEvent* head = _lv_ll_get_head(&g_eventQueueList);
        if (head == NULL) {
            LvUnlock(&g_eventQueueMutex);
            continue;
        }
        _lv_ll_remove(&g_eventQueueList, head);
        LvUnlock(&g_eventQueueMutex);
        head->eventCb(head->userData);
        lv_mem_free(head);
    }
}

void PostGraphicEvent(GraphicEventCb eventCb, void* userData, bool exitLowPower)
{
    if (eventCb == NULL) {
        return;
    }
    LvLock(&g_eventQueueMutex);
    GraphicEvent* tail = _lv_ll_ins_tail(&g_eventQueueList);
    if (tail == NULL) {
        LV_LOGE("[%s: %d] _lv_ll_ins_tail failed.\n", __FUNCTION__, __LINE__);
        return;
    }
    tail->eventCb = eventCb;
    tail->userData = userData;
    LvSignal(&g_eventQueueCond);
    LvUnlock(&g_eventQueueMutex);
    if (exitLowPower) {
#if LV_USE_LOW_POWER_MANAGER
        ExitLowPower();
#endif
    }
}

static void ClearEventQueue(void)
{
    LvLock(&g_eventQueueMutex);
    _lv_ll_clear(&g_eventQueueList);
    LvUnlock(&g_eventQueueMutex);
}

void GpuResetStart(void)
{
    g_isGpuReseting = true;
    ClearEventQueue();
    LvLock(&g_taskHandlerEventMutex);
    g_taskHandlerEventCount = 0;
    LvUnlock(&g_taskHandlerEventMutex);
#ifdef SUPPORT_GPU_JPEG
    uapi_jpeg_video_sub_reset();
#endif
}

void GpuResetEnd(void)
{
    g_isGpuReseting = false;
}

void SetFrequence(uint32_t freq)
{
    if ((freq == 0) || (g_frequence == freq)) {
        return;
    }

#if LV_USE_ASYNC_MODE
    if (freq == 60) { // 60: fps
        LvEnableAsyncMode(true);
    } else {
        LvEnableAsyncMode(false);
    }
#endif
    g_frequence = freq;
    g_timeInterval = SECOND_TO_MILLISECOND / freq;
    uapi_rtc_start(g_rtcHandle, g_timeInterval, (rtc_callback_t)RtcCallback, 0);
}

void NotifyScreenOn(void)
{
    LvLock(&g_screenMutex);
    if (g_isScreenOn) {
        LvUnlock(&g_screenMutex);
        return;
    }
    g_isScreenOn = true;
    g_fullRefreshFlag = true;
    LvUnlock(&g_screenMutex);
    SetFrequence(60); // 60: fps

    if (g_rtcHandle != NULL) {
        uapi_rtc_start(g_rtcHandle, g_timeInterval, (rtc_callback_t)RtcCallback, 0);
    }
}

void NotifyScreenOff(void)
{
    LvLock(&g_screenMutex);
    if (!g_isScreenOn) {
        LvUnlock(&g_screenMutex);
        return;
    }
    g_isScreenOn = false;
    LvUnlock(&g_screenMutex);

    ClearEventQueue();
    LvLock(&g_taskHandlerEventMutex);
    g_taskHandlerEventCount = 0;
    LvUnlock(&g_taskHandlerEventMutex);

    if (g_rtcHandle != NULL) {
        uapi_rtc_stop(g_rtcHandle);
    }
}

static void TaskHandler(void* userData)
{
    (void)userData;
    LvLock(&g_screenMutex);
    if (g_fullRefreshFlag) {
        lv_obj_invalidate(lv_scr_act());
        lv_obj_invalidate(lv_layer_top());
        lv_obj_invalidate(lv_layer_sys());
        g_fullRefreshFlag = false;
    }
    LvUnlock(&g_screenMutex);
    lv_task_handler();
    LvLock(&g_taskHandlerEventMutex);
    if (g_taskHandlerEventCount > 0) {
        g_taskHandlerEventCount--;
    }
    LvUnlock(&g_taskHandlerEventMutex);
}

static bool PostTaskHandlerEvent(void)
{
    LvLock(&g_taskHandlerEventMutex);
    if (g_taskHandlerEventCount <= 1) {
        g_taskHandlerEventCount++;
        LvUnlock(&g_taskHandlerEventMutex);
        PostGraphicEvent(TaskHandler, NULL, false);
        return true;
    }
    LvUnlock(&g_taskHandlerEventMutex);
    return false;
}

static void RtcCallback(void* args)
{
    uapi_rtc_start(g_rtcHandle, g_timeInterval, (rtc_callback_t)RtcCallback, 0);
    if (!g_isGpuReseting) {
        LvSignal(&g_rtcEventCond);
    }
}

static void MainLoop(void* args)
{
    RunEventQueue();
}

static void RtcEventThread(void* args)
{
    while (true) {
        LvLock(&g_rtcEventMutex);
        LvWaitOnCondition(&g_rtcEventMutex, &g_rtcEventCond);
        LvLock(&g_screenMutex);
        if (!g_isScreenOn) {
            LvUnlock(&g_screenMutex);
            LvUnlock(&g_rtcEventMutex);
            continue;
        }
        LvUnlock(&g_screenMutex);
        LvUnlock(&g_rtcEventMutex);
        PostTaskHandlerEvent();
    }
}

void GraphicStartMain(void)
{
    if (g_initialized) {
        return;
    }
    InitModule();

    /* Create main task thread */
    osThreadAttr_t osAttr = { 0 };
    osAttr.stack_size = 0x6000u; // 0x6000u: stack size
    osAttr.name = "lvgl_main_loop";
    osAttr.priority = osPriorityAboveNormal2;
    g_mainThreadId = osThreadNew((osThreadFunc_t)MainLoop, NULL, &osAttr);
    if (g_mainThreadId == NULL) {
        LV_LOGE("[%s:%d] create thread fail!!\n", __FUNCTION__, __LINE__);
        return;
    }

    osThreadAttr_t rtcAttr = { 0 };
    rtcAttr.stack_size = 0x6000u; // 0x6000u: stack size
    rtcAttr.name = "lvgl_rtc_event_loop";
    rtcAttr.priority = osPriorityAboveNormal2;
    g_rtcThreadId = osThreadNew((osThreadFunc_t)RtcEventThread, NULL, &rtcAttr);
    if (g_rtcThreadId == NULL) {
        LV_LOGE("[%s:%d] create thread fail!!\n", __FUNCTION__, __LINE__);
        return;
    }

    /* start RTC timer */
    uapi_rtc_create(RTC_0, &g_rtcHandle);
    if (g_rtcHandle == NULL) {
        LV_LOGE("[%s:%d] create RTC timer fail!!\n", __FUNCTION__, __LINE__);
        return;
    }
    uapi_rtc_start(g_rtcHandle, g_timeInterval, (rtc_callback_t)RtcCallback, 0);
    g_initialized = true;
}

#ifdef __cplusplus
};
#endif
