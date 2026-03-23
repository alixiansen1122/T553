/*
 * Copyright (c) CompanyNameMagicTag. 2022-2022. All rights reserved.
 * Description: camera preview sample wrapper
 * Author: Media Software Group
 * Create: 2022-11-11
 */

#include "camera_preview_sample_wrapper.h"

#include <string>
#include <iostream>
#include <climits>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "source.h"
#include "player.h"
#include "format.h"
#include "securec.h"
#include "media_log.h"
#include "media_thread_adapt.h"

#ifdef ENABLE_UIKIT
#include "common/graphic_startup.h"
#include "common/screen.h"
#include "common/task_manager.h"
#include "components/root_view.h"
#include "gfx_utils/graphic_log.h"
#include "graphic_config.h"
#include "hals/display_dev.h"

#include "graphic_service.h"

#include "components/root_view.h"
#include "components/ui_lite_surface_view.h"

#include "surface.h"
#endif

#include "icamera_host.h"
#include "icamera_device.h"
#include "istream_operator.h"

#ifdef __cplusplus
extern "C" {
#endif

using namespace std;
using namespace OHOS;
using namespace OHOS::Media;
using namespace OHOS::Camera;

const static int16_t MEDIA_PATH_MAX = 260;
const static int32_t WAIT_SURFACE_INIT_SUCCESS_US = 100000;
const static int32_t WAIT_START_CAPTURE_SUCCESS_US = 100000;
const static int32_t WAIT_SURFACE_DEINIT_SUCCESS_US = 100000;
const static int16_t SURFACE_BYTE_ALIGNMENT = 128;
const static int16_t JPEG_WIDTH_BYTE_ALIGNMENT = 128;
const static int16_t JPEG_HEIGHT_BYTE_ALIGNMENT = 16;
const static int16_t JPEG_HEIGHT_DIVISOR = 2;
const static float YCBCR420SP_BUFFER_SIZE_COEFFICIENT = 1.5;
const static int16_t WIDTH = 454;
const static int16_t HEIGHT = 454;
const static uint32_t SLEEP_TIME_S = 30 * 60;
const static uint32_t TIME_OUT_LOOP_CNT = 100;

#define MEDIA_BYTE_ALIGN(value, align) (((value) + (align) - 1) & (~((align) - 1)))

typedef struct {
    bool surfaceInited;
#ifdef ENABLE_UIKIT
    Surface *surface;
    UILiteSurfaceView *surfaceView;
#endif
    char filePath[MEDIA_PATH_MAX];
    std::vector<int> streamIds;
    std::shared_ptr<ICameraHost> demoCameraHost;
    std::shared_ptr<ICameraDevice> demoCameraDevice;
    std::shared_ptr<IStreamOperator> streamOperator;
    std::shared_ptr<CaptureInfo> captureInfo ;
    MediaMutexHandle mutex;
    MediaThreadCondHandle cond;
} CameraPreviewSampleContext;

static CameraPreviewSampleContext *g_cameraPreviewContext = nullptr;

static int32_t SetStreamInfo(std::shared_ptr<IStreamOperator> &streamOperator, CameraPreviewSampleContext *context)
{
    std::shared_ptr<StreamInfo> streamInfo = std::make_shared<StreamInfo>();
    if (streamInfo == nullptr || streamInfo.get() == nullptr) {
        MEDIA_ERR_LOG("streamInfo is nullptr");
        return -1;
    }
    streamInfo->streamId_ = 0;
    streamInfo->format_ = 0;
    streamInfo->datasapce_ = 0;
    streamInfo->intent_ = PREVIEW;
    streamInfo->tunneledMode_ = 0;
    streamInfo->width_ = WIDTH;
    streamInfo->height_ = HEIGHT;
#ifdef ENABLE_UIKIT
    streamInfo->bufferQueue_ = context->surface;
#endif

    std::vector<std::shared_ptr<StreamInfo>> streamInfos;
    std::vector<std::shared_ptr<StreamInfo>>().swap(streamInfos);
    streamInfos.push_back(streamInfo);

    int32_t ret = streamOperator->CreateStreams(streamInfos);
    if (ret != 0) {
        MEDIA_ERR_LOG("demo test: CreatStream CreateStreams error");
        return -1;
    }
    context->streamIds.clear();
    context->streamIds.push_back(streamInfo->streamId_);
    return 0;
}

static int32_t SetCaptureInfo(std::shared_ptr<IStreamOperator> &streamOperator, CameraPreviewSampleContext *context)
{
    std::shared_ptr<CaptureInfo> captureInfo = std::make_shared<CaptureInfo>();
    if (captureInfo == nullptr || captureInfo.get() == nullptr) {
        MEDIA_ERR_LOG("captureInfo is nullptr");
        return -1;
    }
    captureInfo->streamIds_.clear();
    captureInfo->streamIds_.push_back(0);
    captureInfo->captureSetting_ = nullptr;
    captureInfo->enableShutterCallback_ = false;
    streamOperator->Capture(0, captureInfo, true);
    context->captureInfo = captureInfo;
    return 0;
}

static int32_t RunCameraPreview(CameraPreviewSampleContext *context)
{
    MEDIA_INFO_LOG("enter");
    std::shared_ptr<ICameraHost> demoCameraHost = ICameraHost::Get("test");
    if (demoCameraHost == nullptr || demoCameraHost.get() == nullptr) {
        MEDIA_ERR_LOG("demoCameraHost is nullptr");
        return -1;
    }
    vector<string> cameraIds;
    demoCameraHost->GetCameraIds(cameraIds);
    if (cameraIds.size() == 0) {
        MEDIA_ERR_LOG("cameraIds size is zero");
        return -1;
    }
    MEDIA_ERR_LOG("cameraIds size:%d, camera0:%s", cameraIds.size(), cameraIds[0].c_str());
    std::shared_ptr<ICameraDevice> demoCameraDevice = nullptr;
    demoCameraHost->OpenCamera(cameraIds[0], nullptr, demoCameraDevice);
    if (demoCameraDevice == nullptr || demoCameraDevice.get() == nullptr) {
        MEDIA_ERR_LOG("demoCameraDevice is nullptr");
        return -1;
    }

    MEDIA_INFO_LOG("url:%s", g_cameraPreviewContext->filePath);
    demoCameraDevice->UpdateSettings(g_cameraPreviewContext->filePath, strlen(g_cameraPreviewContext->filePath));
    std::shared_ptr<IStreamOperator> streamOperator = nullptr;
    demoCameraDevice->GetStreamOperator(nullptr, streamOperator);
    if (streamOperator == nullptr || streamOperator.get() == nullptr) {
        MEDIA_ERR_LOG("streamOperator is nullptr");
        return -1;
    }
    int32_t ret = SetStreamInfo(streamOperator, context);
    if (ret != 0) {
        return ret;
    }
    ret = streamOperator->CommitStreams(NORMAL, nullptr);
    if (ret != 0) {
        MEDIA_ERR_LOG("demo test: CommitStreams CommitStreams error");
        (void)streamOperator->ReleaseStreams(context->streamIds);
        return -1;
    }
    MEDIA_INFO_LOG("start capture");
    ret = SetCaptureInfo(streamOperator, context);
    if (ret != 0) {
        (void)streamOperator->ReleaseStreams(context->streamIds);
        return ret;
    }
    context->demoCameraHost = demoCameraHost;
    context->demoCameraDevice = demoCameraDevice;
    context->streamOperator = streamOperator;
    MEDIA_INFO_LOG("exit");
    return 0;
}

#ifdef ENABLE_UIKIT
static void SurfaceInitEvent(void)
{
    // 1. init root view
    RootView::GetInstance()->SetWidth(Screen::GetInstance().GetWidth());
    RootView::GetInstance()->SetHeight(Screen::GetInstance().GetHeight());
    RootView::GetInstance()->SetPosition(0, 0);
    RootView::GetInstance()->SetStyle(STYLE_BACKGROUND_OPA, 0);

    // 2. create surface view
    UILiteSurfaceView *surfaceView = new UILiteSurfaceView();
    if (surfaceView == nullptr) {
        MEDIA_ERR_LOG("surfaceView is nullptr");
        return;
    }
    surfaceView->SetViewId("sur");
    surfaceView->SetPosition(0, 0, WIDTH, HEIGHT);

    RootView::GetInstance()->Add(surfaceView);

    Surface *surface = surfaceView->GetSurface();
    surface->SetStrideAlignment(SURFACE_BYTE_ALIGNMENT);
    surface->SetWidthAndHeight(WIDTH, HEIGHT);
    surface->SetQueueSize(2); // 2:queueSize
    surface->SetFormat(PIXEL_FMT_YCBCR_420_SP);

    // jpeg decoder output buffer size calculation formula
    int32_t ySize = MEDIA_BYTE_ALIGN(WIDTH, JPEG_WIDTH_BYTE_ALIGNMENT) * HEIGHT;
    int32_t uvSize = MEDIA_BYTE_ALIGN(WIDTH, JPEG_WIDTH_BYTE_ALIGNMENT) *
        MEDIA_BYTE_ALIGN(HEIGHT, JPEG_HEIGHT_BYTE_ALIGNMENT) / JPEG_HEIGHT_DIVISOR;
    int32_t bufferSize = ySize + uvSize;

    surface->SetSize(bufferSize);

    g_cameraPreviewContext->surface = surface;
    g_cameraPreviewContext->surfaceView = surfaceView;
    g_cameraPreviewContext->surfaceInited = true;
}

static void SurfaceDeinitEvent(void)
{
    MEDIA_INFO_LOG("enter");

    RootView::GetInstance()->Remove(g_cameraPreviewContext->surfaceView);
    if (g_cameraPreviewContext->surfaceView != nullptr) {
        delete g_cameraPreviewContext->surfaceView;
        g_cameraPreviewContext->surfaceView = nullptr;
    }
    g_cameraPreviewContext->surfaceInited = false;

    MEDIA_INFO_LOG("exit");
}

static int32_t WaiteSurfaceInit(void)
{
    GraphicService::GetInstance()->PostGraphicEvent([] {
        SurfaceInitEvent();
    });

    uint32_t loopCnt = 0;
    while (!g_cameraPreviewContext->surfaceInited) {
        if (loopCnt > TIME_OUT_LOOP_CNT) {
            MEDIA_ERR_LOG("wait surface init time out!");
            return -1;
        }
        usleep(WAIT_SURFACE_INIT_SUCCESS_US);
        loopCnt++;
    }
    return 0;
}

static int32_t WaiteSurfaceDeInit(void)
{
    GraphicService::GetInstance()->PostGraphicEvent([] {
        SurfaceDeinitEvent();
    });
    uint32_t loopCnt = 0;
    while (g_cameraPreviewContext->surfaceInited) {
        if (loopCnt > TIME_OUT_LOOP_CNT) {
            MEDIA_ERR_LOG("wait surface deinit time out!");
            return -1;
        }
        usleep (WAIT_SURFACE_DEINIT_SUCCESS_US);
        loopCnt++;
    }
    return 0;
}
#endif

static void ExitCameraPreview(void)
{
    g_cameraPreviewContext->streamOperator->ReleaseStreams(g_cameraPreviewContext->captureInfo->streamIds_);
    g_cameraPreviewContext->streamOperator->CancelCapture(0);
#ifdef ENABLE_UIKIT
    WaiteSurfaceDeInit();
#endif
    (void)MediaThreadCondDestroy(&g_cameraPreviewContext->cond);
    (void)MediaMutexDestroy(&g_cameraPreviewContext->mutex);

    if (g_cameraPreviewContext != nullptr) {
        g_cameraPreviewContext->streamOperator.reset();
        g_cameraPreviewContext->demoCameraDevice.reset();
        g_cameraPreviewContext->demoCameraHost.reset();
        g_cameraPreviewContext->captureInfo.reset();
        free(g_cameraPreviewContext);
        g_cameraPreviewContext = nullptr;
    }
}

static void *CameraPreviewThread(void *priv)
{
    CameraPreviewSampleContext *context = (CameraPreviewSampleContext *)priv;
    int32_t ret = RunCameraPreview(context);
    if (ret != 0) {
        MEDIA_ERR_LOG("camera preview failed");
        return nullptr;
    }
    MediaMutexLock(context->mutex);
    MediaThreadCondWait(context->cond, context->mutex);
    ExitCameraPreview();
    MediaMutexUnLock(context->mutex);
    MEDIA_INFO_LOG("camera preview exit success!");
    return nullptr;
}

static void StartCameraPreview(const char *filePath)
{
    // 1. check filePath
    if (filePath == nullptr) {
        MEDIA_ERR_LOG("uri is nullptr");
        return;
    }
    if (strlen(filePath) > MEDIA_PATH_MAX) {
        MEDIA_ERR_LOG("file path too long");
        return;

    }
    if (memcpy_s(g_cameraPreviewContext->filePath, MEDIA_PATH_MAX, filePath, strlen(filePath) + 1) != EOK) {
        MEDIA_ERR_LOG("memcpy_s failed");
        return;
    }
    // 4. create video play thread
    MediaThreadattr attr = { "CameraPreviewThread", 0x2000, THREAD_SCHED_INVALID, 0, true };
    MediaThreadIdHandle taskHandle = nullptr;
#ifdef ENABLE_UIKIT
    if (WaiteSurfaceInit() != 0) {
        goto EXIT2;
    }
#endif
    taskHandle = MediaThreadCreate(CameraPreviewThread, g_cameraPreviewContext, &attr);
    if (taskHandle == nullptr) {
        MEDIA_ERR_LOG("create camera preview thread failed");
        goto EXIT1;
    }
    return;
EXIT1:
#ifdef ENABLE_UIKIT
    WaiteSurfaceDeInit();
EXIT2:
#endif
    (void)MediaThreadCondDestroy(&g_cameraPreviewContext->cond);
    (void)MediaMutexDestroy(&g_cameraPreviewContext->mutex);
    free(g_cameraPreviewContext);
    g_cameraPreviewContext = nullptr;
}

static void StopCameraPreview(void)
{
    // 1. loop sleep 100ms for start capture success
    if (g_cameraPreviewContext == nullptr) {
        MEDIA_ERR_LOG("camera preview context is null, execute play command first");
        return;
    }
    uint32_t loopCnt = 0;
    while (g_cameraPreviewContext->streamOperator == nullptr || g_cameraPreviewContext->captureInfo == nullptr) {
        if (loopCnt > TIME_OUT_LOOP_CNT) {
            MEDIA_ERR_LOG("wait camera preview init time out!");
            return;
        }
        usleep(WAIT_START_CAPTURE_SUCCESS_US);
        loopCnt++;
    }
    MediaMutexLock(g_cameraPreviewContext->mutex);
    MediaThreadCondSignal(g_cameraPreviewContext->cond);
    MediaMutexUnLock(g_cameraPreviewContext->mutex);
    MEDIA_INFO_LOG("stop success!");
}

static int32_t InitCameraPreviewContext(void)
{
    if (g_cameraPreviewContext != nullptr) {
        MEDIA_ERR_LOG("camera preview context is exit, execute stop command first");
        return -1;
    }
    g_cameraPreviewContext = (CameraPreviewSampleContext *)malloc(sizeof(CameraPreviewSampleContext));
    if (g_cameraPreviewContext == nullptr) {
        MEDIA_ERR_LOG("malloc failed");
        return -1;
    }
    if (memset_s(g_cameraPreviewContext, sizeof(CameraPreviewSampleContext), 0,
        sizeof(CameraPreviewSampleContext)) != EOK) {
        MEDIA_ERR_LOG("memset_s failed");
    }
    g_cameraPreviewContext->mutex = MediaMutexCreate(nullptr);
    if (g_cameraPreviewContext->mutex == nullptr) {
        MEDIA_ERR_LOG("create mutex failed");
        goto EXIT;
    }
    g_cameraPreviewContext->cond = MediaThreadCondCreate();
    if (g_cameraPreviewContext->cond == nullptr) {
        MEDIA_ERR_LOG("create thread cond failed");
        MediaMutexDestroy(&g_cameraPreviewContext->mutex);
        goto EXIT;
    }
    return 0;
EXIT:
    free(g_cameraPreviewContext);
    g_cameraPreviewContext = nullptr;
    return -1;
}

int32_t CameraPreviewSample(int32_t argc, const char *argv[])
{
    MEDIA_INFO_LOG("enter");
    if (argc < 1) {
        MEDIA_ERR_LOG("at least one parameter is required");
        return -1;
    }
    for (int32_t i = 0; i < argc; i++) {
        MEDIA_INFO_LOG("argv[%d]:%s", i, argv[i]);
    }

    if (strcmp(argv[0], "start") == 0) {
        if (InitCameraPreviewContext() != 0) {
            return -1;
        }
        StartCameraPreview(argv[1]);
    } else if (strcmp(argv[0], "stop") == 0) {
        StopCameraPreview();
    } else {
        MEDIA_ERR_LOG("unsupported parameter:%s", argv[0]);
    }
    // blocks until the stop command is entered after the start.
    // will free g_cameraPreviewContext in stop process.
    MEDIA_INFO_LOG("exit");
    return 0;
}

#ifdef __cplusplus
};
#endif
