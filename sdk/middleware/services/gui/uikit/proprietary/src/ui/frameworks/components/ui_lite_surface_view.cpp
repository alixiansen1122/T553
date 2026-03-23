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

#include "imgdecode/image_load.h"
#include "components/root_view.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "graphic_service.h"
#include "draw/draw_rect.h"
#include "draw/draw_utils.h"
#include "dfx/dfx_frame_trace.h"
#include "gfx_utils/graphic_log.h"
#include "hals/display_dev.h"
#include "securec.h"
#include "yuv_to_rgb.h"
#include "common/low_power_manager.h"
#include "components/ui_lite_surface_view.h"


#ifndef UIKIT_SKIP_LINE
#define UIKIT_SKIP_LINE 1
#endif

namespace OHOS {
static constexpr int16_t YUV_WIDTH_ALIGNMENT = 128;
UILiteSurfaceView::UILiteSurfaceView() : isLayerInit_(false), flushCount_(0)
{
    surface_ = std::make_shared<LiteSurface>();
    exist_ = std::make_shared<bool>(true);
    if (surface_ == nullptr) {
        GRAPHIC_LOGE("new LiteSurface failed\n");
        return;
    }
    surface_->RegisterConsumerListener(*this);
}

UILiteSurfaceView::~UILiteSurfaceView()
{
    if (isLayerInit_) {
        DisplayDev::GetInstance()->CloseLayer(LAYER_1);
        DisplayDev::GetInstance()->SetLayerColorKey(LAYER_0, false);
    }

    surface_->UnregisterConsumerListener();
    LiteMGfxEngine::GetInstance()->ClearHardwareLayer(this);
    GraphicService::GetInstance()->PostGraphicEvent([] {
            if (LiteMGfxEngine::GetInstance()->GetHardwareLayer() == nullptr) {
                GraphicService::GetInstance()->EnableAsyncMode(true);
            }
        });

    if (preView_ != nullptr) {
        delete preView_;
        preView_ = nullptr;
    }
}

Surface* UILiteSurfaceView::GetSurface() const
{
    return surface_.get();
}

void UILiteSurfaceView::SetPosition(int16_t x, int16_t y, int16_t width, int16_t height)
{
    if (GetWidth() > 1 || GetHeight() > 1) {
        GRAPHIC_LOGE("SetPosition failed. width or height is already set.");
    } else {
        UIView::SetPosition(x, y, width, height);
    }
}

void UILiteSurfaceView::Resize(int16_t width, int16_t height)
{
    if (GetWidth() > 1 || GetHeight() > 1) {
        GRAPHIC_LOGE("Resize failed. width or height is already set.");
    } else {
        UIView::Resize(width, height);
    }
}

void UILiteSurfaceView::SetWidth(int16_t width)
{
    if (GetWidth() > 1) {
        GRAPHIC_LOGE("SetWidth failed. width is already set.");
    } else {
        UIView::SetWidth(width);
    }
}

void UILiteSurfaceView::SetHeight(int16_t height)
{
    if (GetHeight() > 1) {
        GRAPHIC_LOGE("SetHeight failed. height is already set.");
    } else {
        UIView::SetHeight(height);
    }
}

void UILiteSurfaceView::Flush()
{
    if (flushCount_ > 0) {
        lock_.Lock();
        --flushCount_;
        lock_.Unlock();
        currentBuffer_ = surface_->AcquireBuffer();
        if (currentBuffer_ == nullptr) {
            return;
        }

        if (!LowPowerManager::GetInstance()->IsLiteSurfaceInRootView()) {
            return;
        }

        LayerBuffer layer;
        (void)memset_s(&layer, sizeof(LayerBuffer), 0, sizeof(LayerBuffer));
        layer.width = static_cast<int32_t>(currentBuffer_->width);
        layer.height = static_cast<int32_t>(currentBuffer_->height);
        layer.pitch = static_cast<int32_t>(currentBuffer_->stride);
        layer.pixFormat = currentBuffer_->format;
        layer.data.phyAddr = currentBuffer_->addr;
        layer.data.virAddr = reinterpret_cast<void*>(currentBuffer_->addr);

        FRAME_TRACE_START(FrameTraceType::HW_DRAW_FLUSH);
        DisplayDev::GetInstance()->SetHardwareLayerBuffer(LAYER_1, BUFFER_0, layer);
        DisplayDev::GetInstance()->LcdFlush(LAYER_1);
        FRAME_TRACE_END(FrameTraceType::HW_DRAW_FLUSH, nullptr);
    }

    if (flushCount_ > 0) {
        Invalidate();
    }
}

void UILiteSurfaceView::PostFlush()
{
    if (currentBuffer_ != nullptr) {
        surface_->ReleaseBuffer(lastFlushedBuffer_);
        lastFlushedBuffer_ = currentBuffer_;
        currentBuffer_ = nullptr;
    }
}

void UILiteSurfaceView::SetSurfaceColorkey(ColorType color)
{
    colorKey_ = color;
}

void UILiteSurfaceView::InitLayer()
{
    Rect rect(GetRect());
    int16_t width = rect.GetWidth();
    int16_t height = rect.GetHeight();
    uint32_t fmt = surface_->GetFormat();
    uint32_t size = surface_->GetSize();
    uint8_t queueSize = surface_->GetQueueSize();
    if (width > 1 && height > 1 && size != 0 && queueSize != 0 && fmt != PIXEL_FMT_BUTT) {
        LayerInfo layerInfo;
        layerInfo.width = width;
        layerInfo.height = height;
        layerInfo.type = LAYER_TYPE_GRAPHIC;
        layerInfo.pixFormat = static_cast<PixelFormat>(surface_->GetFormat());
        DisplayDev::GetInstance()->OpenLayer(LAYER_1, layerInfo, 1);
        Rect dirtyRect(0, 0, width - 1, height - 1);
        DisplayDev::GetInstance()->SetLayerPositon(LAYER_1, rect.GetX(), rect.GetY());
        DisplayDev::GetInstance()->SetLayerDirtyRegion(LAYER_1, dirtyRect);
        DisplayDev::GetInstance()->SetLayerColorKey(LAYER_0, true, colorKey_.full);
        isLayerInit_ = true;
        layerRect_ = rect;
    }
}

#ifdef VERSION_IOT
bool UILiteSurfaceView::GetSurfaceBufferBitmap(ImageInfo& bitmap)
{
    if (lastFlushedBuffer_ == nullptr) {
        GRAPHIC_LOGE("lastFlushedBuffer_ is null.");
        return false;
    }
    CscYuvBuf yuv = {0};
    yuv.yAddr = reinterpret_cast<uint8_t*>(lastFlushedBuffer_->addr);
    yuv.uvAddr = static_cast<uint8_t*>(yuv.yAddr) + ALIGN_BYTE(lastFlushedBuffer_->width, YUV_WIDTH_ALIGNMENT) *
        lastFlushedBuffer_->height;
    yuv.width = lastFlushedBuffer_->width;
    yuv.height = lastFlushedBuffer_->height;
    yuv.yStride = ALIGN_BYTE(lastFlushedBuffer_->width, YUV_WIDTH_ALIGNMENT);
    yuv.uvStride = ALIGN_BYTE(lastFlushedBuffer_->width, YUV_WIDTH_ALIGNMENT);

    CscRgbBuf rgb = {0};
#if UIKIT_SKIP_LINE
    rgb.width = lastFlushedBuffer_->width / 2; // 2: half
    rgb.height = lastFlushedBuffer_->height / 2; // 2: half
#else
    rgb.width = lastFlushedBuffer_->width;
    rgb.height = lastFlushedBuffer_->height;
#endif
    rgb.stride = ALIGN_BYTE(rgb.width, BYTE_ALIGNMENT) * 3; // 3: 3 bytes for rgb888

    bitmap.header.colorMode = RGB888;
    bitmap.dataSize = rgb.stride * rgb.height;
    bitmap.header.width = rgb.width;
    bitmap.header.height = rgb.height;
    bitmap.header.compressMode = 0;
    bitmap.header.reserved = 0;
    rgb.addr = reinterpret_cast<uint8_t*>(ImageCacheMalloc(bitmap));
    if (rgb.addr == nullptr) {
        GRAPHIC_LOGE("UILiteSurfaceView::GetBitmap img alloc failed! size: 0x%x\n", bitmap.dataSize);
        return false;
    }

#if UIKIT_SKIP_LINE
    CscConvertYuvToRgbWithSkipLP(&rgb, &yuv);
#else
    CscConvertYuvToRgb(&rgb, &yuv);
#endif
    return true;
}

void UILiteSurfaceView::BlitBitmapToFb(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, ImageInfo& bitmap)
{
    BufferInfo src;
    src.rect = {0, 0, bitmap.header.width - 1, bitmap.header.height - 1};
    src.virAddr = static_cast<void*>(const_cast<uint8_t*>(bitmap.data));
    src.phyAddr = static_cast<void*>(const_cast<uint8_t*>(bitmap.phyAddr));
    src.stride = ALIGN_BYTE(bitmap.header.width, BYTE_ALIGNMENT) * 3; // 3: 3 bytes for rgb888;
    src.mode = RGB888;
    src.color = 0;
    src.width = bitmap.header.width;
    src.height = bitmap.header.height;
    src.compressMode = COMPRESS_MODE_NONE;

    Rect rect = GetRect();
    rect.Intersect(rect, invalidatedArea);
    BlendOption blendOption;
#if UIKIT_SKIP_LINE
    blendOption.transMap.Scale(Vector2<float>{2.0, 2.0}, Vector2<float>{0, 0}); // 2.0: scale 2
    blendOption.transMap.Translate(Vector2<int16_t>(GetRect().GetX(), GetRect().GetY()));
#endif
    blendOption.opacity = OPA_OPAQUE;
    blendOption.mode = BLEND_SRC_OVER;
    BaseGfxEngine::GetInstance()->Blit(gfxDstBuffer, {0, 0}, src, rect, blendOption);
    LiteMGfxEngine::GetInstance()->SyncHwDraw();
}
#endif

void UILiteSurfaceView::AdjustArea(Rect& rect)
{
    if (rect.GetX() % 2 != 0) { // 2: mod
        if (rect.GetX() < 0) {
            rect.SetX(rect.GetX() + 1);
        } else {
            rect.SetX(rect.GetX() - 1);
        }
    }

    if (rect.GetY() % 2 != 0) { // 2: mod
        if (rect.GetY() < 0) {
            rect.SetY(rect.GetY() + 1);
        } else {
            rect.SetY(rect.GetY() - 1);
        }
    }
}

void UILiteSurfaceView::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    if (surface_ == nullptr) {
        GRAPHIC_LOGE("surface is null\n");
        return;
    }

    if ((isStopped_ || lastFlushedBuffer_ == nullptr) && preView_ != nullptr) {
        const ImageInfo* info = preView_->GetImageInfo();
        Rect coords(GetX(), GetY(), GetX() + info->header.width - 1, GetY() + info->header.height - 1);
        preView_->DrawImage(gfxDstBuffer, coords, invalidatedArea, *style_, opaScale_);
        return;
    }

#ifdef VERSION_IOT
    if (lastFlushedBuffer_ != nullptr && RootView::GetInstance()->GetSnapshotFlag()) {
        ImageInfo bitmap;
        if (GetSurfaceBufferBitmap(bitmap)) {
            BlitBitmapToFb(gfxDstBuffer, invalidatedArea, bitmap);
            ImageCacheFree(bitmap);
            return;
        }
    }
#endif

    Style style = *style_;
    if (isLayerInit_ && !isStopped_) {
        style.bgColor_ = colorKey_;
    }

    layerRect_ = GetRect();
    AdjustArea(layerRect_);
    Rect dirtyRect(0, 0, layerRect_.GetWidth() - 1, layerRect_.GetHeight() - 1);
    DisplayDev::GetInstance()->SetLayerPositon(LAYER_1, layerRect_.GetX(), layerRect_.GetY());
    DisplayDev::GetInstance()->SetLayerDirtyRegion(LAYER_1, dirtyRect);

    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, GetRect(), invalidatedArea, style, OPA_OPAQUE);
}

void UILiteSurfaceView::OnBufferAvailable()
{
    lock_.Lock();
    SurfaceBuffer *back = surface_->GetBackBuf();
    bool isLastFrame = (back->format == PIXEL_FMT_BUTT);
    if (isLastFrame) {
        back->format = PIXEL_FMT_YCBCR_420_SP;
        flushCount_ = 0;
    } else {
        ++flushCount_;
    }
    lock_.Unlock();
    std::weak_ptr<bool> ws = exist_;
    if (isLastFrame) {
        isStopped_ = true;
        std::shared_ptr<LiteSurface> surface = surface_;
        GraphicService::GetInstance()->PostGraphicEvent([&, ws, surface] {
            if (!ws.expired()) {
                DisplayDev::GetInstance()->SetLayerColorKey(LAYER_0, false, 0);
                LiteMGfxEngine::GetInstance()->ClearHardwareLayer(this);
                if (LiteMGfxEngine::GetInstance()->GetHardwareLayer() == nullptr) {
                    GraphicService::GetInstance()->EnableAsyncMode(true);
                }
                currentBuffer_ = nullptr;
                lastFlushedBuffer_ = nullptr;
            }
            DisplayDev::GetInstance()->SetPendingReleaseSurface(surface);
        });
        GraphicService::GetInstance()->ForceRefreshMore();
    } else if (isStopped_) {
        isStopped_ = false;
        LowPowerManager::GetInstance()->ExitLowPower();
        GraphicService::GetInstance()->PostGraphicEvent([&, ws] {
            if (!ws.expired()) {
                if (!GraphicService::GetInstance()->IsScreenOn()) {
                    isStopped_ = true;  // Do not process video frame, when screen off and dpu suspended.
                    return;
                }
                LiteMGfxEngine::GetInstance()->EnableAsyncMode(false);
                LiteMGfxEngine::GetInstance()->AddHardwareLayer(this);
                DisplayDev::GetInstance()->SetLayerColorKey(LAYER_0, true, colorKey_.full);
                InitLayer();
            }
        });
    }

    GraphicService::GetInstance()->PostGraphicEvent([] { RootView::GetInstance()->Invalidate(); });
}

void UILiteSurfaceView::SetPreview(ImageInfo* info)
{
    if (info == nullptr) {
        return;
    }
    if (preView_ == nullptr) {
        preView_ = new Image();
    }
    if (preView_ != nullptr) {
        preView_->SetSrc(info);
    }
}

void UILiteSurfaceView::DumpImgInfo()
{
    if (preView_ != nullptr) {
        printf("view[%p]: viewType = %d\n", this, GetViewType());
        const ImageInfo* info = preView_->GetImageInfo();
        PrintImgInfo(info);
    }
}

std::string  UILiteSurfaceView::GetGuiInfo() const
{
    const int len = 256;
    char *buf = new (std::nothrow) char[len];
    if (buf == nullptr) {
        return std::string("");
    }
    memset_s(buf, len, 0, len);
    sprintf_s(buf, len, "stopped %d layerinit %d flushCnt %u lastbuf %p curbuf %p colorKey %x preView %p",
        isStopped_, isLayerInit_, flushCount_, lastFlushedBuffer_, currentBuffer_, colorKey_.full, preView_);
    std::string str(buf);
    delete []buf;
    return str;
}
}
