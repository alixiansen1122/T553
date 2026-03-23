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

#include "hals/display_dev.h"
#include <securec.h>
#include "gfx_utils/color.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/graphic_math.h"
#include "gfx_utils/mem_api.h"
#include "graphic_config.h"
#include "hals/gralloc_engines.h"
#include "dfx/dfx_frame_trace.h"
#include "imgdecode/image_load.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "draw/hw_draw_utils.h"

#ifndef _WIN32
#include "tcxo.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "soc_mipi_tx.h"
#ifdef __cplusplus
};
#endif
#endif

namespace OHOS {
DisplayDev::DisplayDev() : isDisplayDevInited_(false), devId_(0), layerFuncs_(nullptr), exitUlps_(false)
{
    for (uint8_t i = LAYER_0; i < LAYER_MAX; i++) {
        currentBufferId_[i] = 0;
        layer_[i].layerHdl = 0;
        (void)memset_s(&layer_[i].layerInfo, sizeof(LayerInfo), 0, sizeof(LayerInfo));
        layer_[i].bufferCount = 0;
        layer_[i].buffer = nullptr;
        (void)memset_s(&layer_[i].rotateInfo, sizeof(LayerInfo), 0, sizeof(LayerInfo));
        layer_[i].rotate = nullptr;
        layer_[i].rotateType = LayerRotateType::LAYER_ROTATE_NONE;
    }
}

void DisplayDev::LcdFlush(uint8_t layerId)
{
    if (layerId < LAYER_0 || layerId > LAYER_MAX) {
        GRAPHIC_LOGE("Invalid layerId[%u].", layerId);
        return;
    }
    if (layerFuncs_->Flush != nullptr) {
        if (layerId == LAYER_MAX) {
            TryExitUlps(true);
            if (layerFuncs_->Flush(0, 0, nullptr) != DISPLAY_SUCCESS) {
                GRAPHIC_LOGE("flush failed.");
            }
            exitUlps_ = false;
            if (pendingSurface_ != nullptr) {
                pendingSurface_->ClearBuffers();
                pendingSurface_ = nullptr;
            }
            return;
        }
        uint8_t currentBufferIndex = currentBufferId_[layerId];
        LayerBuffer *buffer = nullptr;
        if (layer_[layerId].rotate != nullptr) {
            buffer = &layer_[layerId].rotate[currentBufferIndex];
        } else {
            buffer = &layer_[layerId].buffer[currentBufferIndex];
        }
        SET_TRACE_FLUSH_TYPE(!((buffer->data.phyAddr >= PSRAM_BASE_ADDR) &&
                               (buffer->data.phyAddr < PSRAM_BASE_ADDR + PSRAM_LENTH)));
        if (layerFuncs_->Flush(devId_, layer_[layerId].layerHdl, buffer) !=
            DISPLAY_SUCCESS) {
            GRAPHIC_LOGE("flush failed.");
            return;
        }
    }
}

void DisplayDev::WaitForVBlank()
{
    if (layerFuncs_->WaitForVBlank != nullptr) {
        if (layerFuncs_->WaitForVBlank(devId_, layer_[LAYER_0].layerHdl, 0) != DISPLAY_SUCCESS) {
            GRAPHIC_LOGE("WaitForVBlank failed.");
            return;
        }
    }
}

LayerRotateType DisplayDev::GetLayerRotateType(uint8_t layerId)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX) {
        GRAPHIC_LOGE("Invalid layerId[%u].", layerId);
        return LayerRotateType::LAYER_ROTATE_BUTT;
    }
    return layer_[layerId].rotateType;
}

LayerBuffer* DisplayDev::GetCurrentBuffer(uint8_t layerId)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX) {
        GRAPHIC_LOGE("Invalid layerId[%u].", layerId);
        return nullptr;
    }
    if (layer_[layerId].bufferCount == 0) {
        GRAPHIC_LOGE("layer[%u] is not opened.", layerId);
        return nullptr;
    }
    return &layer_[layerId].buffer[currentBufferId_[layerId]];
}

LayerBuffer* DisplayDev::GetNextBuffer(uint8_t layerId)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX) {
        GRAPHIC_LOGE("Invalid layerId[%u].", layerId);
        return nullptr;
    }
    if (layer_[layerId].bufferCount == 0) {
        GRAPHIC_LOGE("layer[%u] is not opened.", layerId);
        return nullptr;
    }
    uint8_t currentBufferId = currentBufferId_[layerId];
    uint8_t nextBufferId;
    if (currentBufferId + 1 < layer_[layerId].bufferCount) {
        nextBufferId = currentBufferId + 1;
    } else {
        nextBufferId = 0;
    }
    return &layer_[layerId].buffer[nextBufferId];
}

LayerBuffer* DisplayDev::GetCurrentRotateBuffer(uint8_t layerId)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX) {
        GRAPHIC_LOGE("Invalid layerId[%u].", layerId);
        return nullptr;
    }
    if (layer_[layerId].bufferCount == 0) {
        GRAPHIC_LOGE("layer[%u] is not opened.", layerId);
        return nullptr;
    }
    if (layer_[layerId].rotate == nullptr) {
        GRAPHIC_LOGE("rotateBuffer is nullptr!");
        return nullptr;
    }
    return &layer_[layerId].rotate[currentBufferId_[layerId]];
}

void DisplayDev::GetRotatePosition(int32_t width, int32_t height, int32_t &x, int32_t &y, LayerRotateType type) const
{
    int32_t outX = x;
    int32_t outY = y;
    switch (type) {
        case LayerRotateType::LAYER_ROTATE_90: {
            outX = height - y;
            outY = x;
            break;
        }
        case LayerRotateType::LAYER_ROTATE_180: {
            outX = width - x;
            outY = height - y;
            break;
        }
        case LayerRotateType::LAYER_ROTATE_270: {
            outX = y;
            outY = width - x;
            break;
        }
        default:
            return;
    }
    x = outX;
    y = outY;
}

void DisplayDev::GetRotateRect(int32_t width, int32_t height, Rect &rect, LayerRotateType type) const
{
    Rect outRect = rect;
    int32_t x = rect.GetLeft();
    int32_t y = rect.GetTop();
    switch (type) {
        case LayerRotateType::LAYER_ROTATE_90: {
            x = rect.GetLeft();
            y = rect.GetBottom();
            outRect.SetWidth(rect.GetHeight());
            outRect.SetHeight(rect.GetWidth());
            GetRotatePosition(width, height, x, y, type);
            break;
        }
        case LayerRotateType::LAYER_ROTATE_180: {
            x = rect.GetRight();
            y = rect.GetBottom();
            GetRotatePosition(width, height, x, y, type);
            break;
        }
        case LayerRotateType::LAYER_ROTATE_270: {
            x = rect.GetRight();
            y = rect.GetTop();
            outRect.SetWidth(rect.GetHeight());
            outRect.SetHeight(rect.GetWidth());
            GetRotatePosition(width, height, x, y, type);
            break;
        }
        default:
            return;
    }
    outRect.SetPosition(x, y);
    rect = outRect;
}

bool DisplayDev::SwapBuffer(uint8_t layerId)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX) {
        GRAPHIC_LOGE("Invalid layerId[%u].", layerId);
        return false;
    }
    if (layer_[layerId].bufferCount < 2) { // 2: buffer count
        GRAPHIC_LOGE("layer[%u] is not opened or only has 1 buffer.", layerId);
        return false;
    }
    uint8_t currentBufferId = currentBufferId_[layerId];
    if (currentBufferId + 1 < layer_[layerId].bufferCount) {
        currentBufferId_[layerId] = currentBufferId + 1;
    } else {
        currentBufferId_[layerId] = 0;
    }
    return true;
}

void DisplayDev::SetPendingReleaseSurface(std::shared_ptr<LiteSurface> surface)
{
    pendingSurface_ = surface;
}

static uint32_t GetAlignedStride(uint32_t width, PixelFormat fmt, uint8_t cmpMode)
{
    uint32_t bpp;
    uint32_t stride;
    switch (fmt) {
        case PIXEL_FMT_RGBA_8888:
            if (cmpMode == COMPRESS_MODE_HFBC) {
                bpp = 12; /* 12 bits */
            } else if (cmpMode == COMPRESS_MODE_HFBC_ABYPASS) {
                bpp = 16; /* 16 bits */
            } else {
                bpp = 32; /* 32 bits */
            }
            break;
        case PIXEL_FMT_RGBA_5551:
        case PIXEL_FMT_RGB_565:
            bpp = 16; /* 16 bits */
            break;
        case PIXEL_FMT_RGB_888:
            bpp = (cmpMode != COMPRESS_MODE_NONE) ? 8 : 24; /* 8,24 bits */
            break;
        case PIXEL_FMT_A1:
            stride = width >> SHIFT_3;
            if (width & 0x7) {
                stride++;
            }
            stride = ALIGN_BYTE(stride, BYTE_ALIGNMENT);
            return stride;
        default:
            bpp = 32; /* 32 bits */
            break;
    }
    stride = ALIGN_BYTE(width, BYTE_ALIGNMENT) * bpp / STRIDE_DIVIDER;
    if (cmpMode != COMPRESS_MODE_NONE) {
        stride = stride * 4; /* 4: compress coef */
    }
    return stride;
}

uint32_t DisplayDev::CalcStride(uint32_t width, PixelFormat fmt, uint8_t cmpMode)
{
    uint32_t bitPerPixel;
    uint32_t stride;
    switch (fmt) {
        case PIXEL_FMT_RGBA_8888:
            bitPerPixel = 32; // 32: bit per pixel
            break;
        case PIXEL_FMT_RGBA_5551:
        case PIXEL_FMT_RGB_565:
            bitPerPixel = 16; // 16: bit per pixel
            break;
        case PIXEL_FMT_RGB_888:
            bitPerPixel = 24; // 24: bit per pixel
            break;
        case PIXEL_FMT_A1:
        case PIXEL_FMT_A2:
        case PIXEL_FMT_A4:
        case PIXEL_FMT_A8:
            bitPerPixel = 8; /* 8 bits */
            break;
        default:
            GRAPHIC_LOGE("unsupported PixelFormat: %d, just return 0.", fmt);
            return 0;
    }

    switch (cmpMode) {
        case COMPRESS_MODE_NONE:
            stride = ALIGN_BYTE(width, 16) * bitPerPixel / 8; /** 16-byte aligned  8 bits per byte */
            break;
        case COMPRESS_MODE_HFBC:
        case COMPRESS_MODE_HFBC_ABYPASS:
            stride = GetAlignedStride(width, fmt, cmpMode);
            break;
        default:
            GRAPHIC_LOGE("unsupported compress mode: %d, just return 0.", cmpMode);
            return 0;
    }

    return stride;
}

uint32_t DisplayDev::CalcSize(uint32_t stride, uint32_t height, uint8_t cmpMode)
{
    uint32_t size;
    switch (cmpMode) {
        case COMPRESS_MODE_NONE:
            size = stride * height;
            break;
        default:
            GRAPHIC_LOGE("unsupported compress mode: %d, just return 0.", cmpMode);
            return 0;
    }

    return size;
}

bool DisplayDev::OpenLayer(uint8_t layerId, const LayerInfo& info, uint8_t bufferCount, LayerRotateType rotateType)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX || bufferCount == 0 || bufferCount > BUFFER_MAX) {
        GRAPHIC_LOGE("Invalid layerId[%d] or invalid bufferCount[%d].", layerId, bufferCount);
        return false;
    }

    if (layer_[layerId].bufferCount != 0) {
        GRAPHIC_LOGD("layer[%u] is already opened with bufferCount[%u].", layerId, layer_[layerId].bufferCount);
        return false;
    }

    if (layerFuncs_->CreateLayer == nullptr) {
        GRAPHIC_LOGE("OpenLayer is nullptr");
        return false;
    }

    uint32_t size = bufferCount * sizeof(LayerBuffer);
    layer_[layerId].buffer = (LayerBuffer*)UIMalloc(size);
    (void)memset_s(layer_[layerId].buffer, size, 0, size);
    if (layer_[layerId].buffer == nullptr) {
        GRAPHIC_LOGE("new LayerBuffer failed.");
        return false;
    }

    LayerInfo outInfo = info;
    layer_[layerId].layerInfo = outInfo;
    layer_[layerId].bufferCount = bufferCount;
    layer_[layerId].rotateType = rotateType;
    layer_[layerId].pos.x = 0;
    layer_[layerId].pos.y = 0;
    if (rotateType != LayerRotateType::LAYER_ROTATE_NONE) {
        layer_[layerId].rotate = (LayerBuffer*)UIMalloc(size);
        if (layer_[layerId].rotate == nullptr) {
            UIFree(layer_[layerId].buffer);
            GRAPHIC_LOGE("new rotate layerBuffer failed.");
            layer_[layerId].buffer = nullptr;
            return false;
        }
        if (rotateType == LayerRotateType::LAYER_ROTATE_90 || rotateType == LayerRotateType::LAYER_ROTATE_270) {
            outInfo.height = info.width;
            outInfo.width = info.height;
            layer_[layerId].rotateInfo = outInfo;
        } else {
            layer_[layerId].rotateInfo = outInfo;
        }
    }

    if (layerFuncs_->CreateLayer(devId_, &outInfo, &layer_[layerId].layerHdl) != DISPLAY_SUCCESS) {
        UIFree(layer_[layerId].buffer);
        layer_[layerId].buffer = nullptr;
        if (layer_[layerId].rotate != nullptr) {
            UIFree(layer_[layerId].rotate);
            layer_[layerId].rotate = nullptr;
        }
        layer_[layerId].layerHdl = 0;
        layer_[layerId].bufferCount = 0;
        layer_[layerId].rotateType = LayerRotateType::LAYER_ROTATE_NONE;
        (void)memset_s(&layer_[layerId].layerInfo, sizeof(LayerInfo), 0, sizeof(LayerInfo));
        GRAPHIC_LOGE("OpenLayer failed. LayerId: %u.", layerId);
        return false;
    }

    return true;
}

void DisplayDev::CloseLayer(uint8_t layerId)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX) {
        GRAPHIC_LOGE("Invalid layerId[%u].", layerId);
        return;
    }

    if (layer_[layerId].bufferCount == 0) {
        GRAPHIC_LOGE("layer[%u] is already closed.", layerId);
        return;
    }

    if (layerFuncs_->CloseLayer == nullptr) {
        GRAPHIC_LOGE("CloseLayer is null.");
        return;
    }
    if (layerFuncs_->CloseLayer(devId_, layer_[layerId].layerHdl) != DISPLAY_SUCCESS) {
        GRAPHIC_LOGE("CloseLayer %u failed.", layerId);
        return;
    }

    UIFree(layer_[layerId].buffer);
    layer_[layerId].buffer = nullptr;
    if (layer_[layerId].rotate) {
        UIFree(layer_[layerId].rotate);
        layer_[layerId].rotate = nullptr;
    }

    layer_[layerId].layerHdl = 0;
    layer_[layerId].bufferCount = 0;
    layer_[layerId].rotateType = LayerRotateType::LAYER_ROTATE_NONE;
    layer_[layerId].pos.x = 0;
    layer_[layerId].pos.y = 0;
    (void)memset_s(&layer_[layerId].layerInfo, sizeof(LayerInfo), 0, sizeof(LayerInfo));
}

static void ClearLayerBuffer(LayerBuffer& buffer)
{
    buffer.fenceId = 0;
    buffer.width = 0;
    buffer.height = 0;
    buffer.pitch = 0;
    buffer.pixFormat = PIXEL_FMT_BUTT;
    buffer.data.phyAddr = 0;
    buffer.data.virAddr = nullptr;
}

bool DisplayDev::IsUlpsEnabled(void)
{
#if ENABLE_ULPS
    return true;
#else
    return false;
#endif
}

bool DisplayDev::TryExitUlps(bool isFlushing)
{
    if (IsUlpsEnabled() && !exitUlps_) {
#ifndef _WIN32
        uapi_mipi_tx_exit_ulps();
        if (isFlushing) {
            uapi_tcxo_delay_ms(1);
        }
#endif
        exitUlps_ = true;
        return true;
    }
    return false;
}

bool DisplayDev::IsDoubleBuffer(void)
{
#if DOUBLE_BUFFER
    return true;
#else
    return false;
#endif
}

bool DisplayDev::AllocBuffer(LayerBuffer *layerBuffer, const LayerInfo &layerInfo, uint8_t count, bool isFb)
{
    if (layerBuffer == nullptr || count == 0) {
        return false;
    }

    uint32_t stride = CalcStride(layerInfo.width, layerInfo.pixFormat);
    if (stride == 0) {
        return false;
    }

    uint8_t index = 0;
    while (index < count) {
        AllocInfo info;
        if (isFb) {
#if ENABLE_GMMU
            info.usage = HBM_USE_MEM_FB;
#else
            info.expectedSize = stride * (uint16_t)layerInfo.height;
            info.usage = HBM_USE_ASSIGN_SIZE | HBM_USE_MEM_FB;
#endif
        } else {
            info.usage = HBM_USE_ASSIGN_SIZE;
            info.expectedSize = stride * (uint16_t)layerInfo.height;
        }

        GrallocBuffer buffer;
        if (!GrallocEngines::GetInstance()->AllocBuffer(info, buffer)) {
            GRAPHIC_LOGE("AllocMem failed.");
            FreeBuffer(layerBuffer, index);
            return false;
        }

        layerBuffer[index].data.virAddr = buffer.virAddr;
        layerBuffer[index].data.phyAddr = reinterpret_cast<uint64_t>(buffer.virAddr);
        layerBuffer[index].width = layerInfo.width;
        layerBuffer[index].height = layerInfo.height;
        if (isFb) {
#if ENABLE_GMMU
            layerBuffer[index].pitch = static_cast<int32_t>(buffer.stride);
#else
            layerBuffer[index].pitch = (int32_t)stride;
#endif
        } else {
            layerBuffer[index].pitch = (int32_t)stride;
        }
        layerBuffer[index].pixFormat = layerInfo.pixFormat;
        ++index;
    }

    return true;
}

void DisplayDev::FreeBuffer(LayerBuffer *buffer, uint8_t count)
{
    if (buffer == nullptr || count == 0) {
        return;
    }

    for (uint8_t i = 0; i < count; i++) {
        GrallocEngines::GetInstance()->FreeBuffer(reinterpret_cast<uint8_t*>(buffer[i].data.virAddr));
        ClearLayerBuffer(buffer[i]);
    }
}

bool DisplayDev::AllocLayerBuffer(uint8_t layerId)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX || layer_[layerId].bufferCount == 0) {
        GRAPHIC_LOGE("Invalid layerId[%u] or layer is not opened.", layerId);
        return false;
    }

    if (layer_[layerId].buffer[0].data.virAddr != nullptr) {
        GRAPHIC_LOGE("layer[%u] : Memory of buffers is already allocated.", layerId);
        return false;
    }

    if (layer_[layerId].rotate != nullptr) {
        if (!AllocBuffer(layer_[layerId].buffer, layer_[layerId].layerInfo, layer_[layerId].bufferCount)) {
            return false;
        }
        if (!AllocBuffer(layer_[layerId].rotate, layer_[layerId].rotateInfo, layer_[layerId].bufferCount, false)) {
            FreeBuffer(layer_[layerId].buffer, layer_[layerId].bufferCount);
            return false;
        }
    } else {
        if (!AllocBuffer(layer_[layerId].buffer, layer_[layerId].layerInfo, layer_[layerId].bufferCount)) {
            return false;
        }
    }
    return true;
}

void DisplayDev::FreeLayerBuffer(uint8_t layerId)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX || layer_[layerId].bufferCount == 0) {
        GRAPHIC_LOGE("Invalid layerId[%u] or layer is not opened.", layerId);
        return;
    }

    if (layer_[layerId].buffer[0].data.virAddr == nullptr) {
        GRAPHIC_LOGE("layer[%u] : Memory is already released.", layerId);
        return;
    }

    FreeBuffer(layer_[layerId].buffer, layer_[layerId].bufferCount);
    if (layer_[layerId].rotate != nullptr) {
        FreeBuffer(layer_[layerId].rotate, layer_[layerId].bufferCount);
    }
}

void DisplayDev::SetLayerVisible(uint8_t layerId, bool visible)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX) {
        GRAPHIC_LOGE("Invalid layerId[%u].", layerId);
        return;
    }

    if (layerFuncs_->SetLayerVisible != nullptr) {
        if (layerFuncs_->SetLayerVisible(devId_, layer_[layerId].layerHdl, visible) !=
            DISPLAY_SUCCESS) {
            GRAPHIC_LOGE("SetLayerVisible failed. LayerId: %u, visible: %d.", layerId, visible);
        }
    }
}

void DisplayDev::SetLayerDirtyRegion(uint8_t layerId, const Rect& rect)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX) {
        GRAPHIC_LOGE("Invalid layerId[%u].", layerId);
        return;
    }

    if (layerFuncs_->SetLayerDirtyRegion != nullptr) {
        FRAME_TRACE_SET_RECT(rect);
        Rect outRect = rect;
        GetRotateRect(
            layer_[layerId].layerInfo.width, layer_[layerId].layerInfo.height, outRect, layer_[layerId].rotateType);
        IRect iRect = {outRect.GetX(), outRect.GetY(), outRect.GetWidth(), outRect.GetHeight()};
        if (layerFuncs_->SetLayerDirtyRegion(devId_, layer_[layerId].layerHdl, &iRect) !=
            DISPLAY_SUCCESS) {
            GRAPHIC_LOGE("SetLayerDirtyRegion failed. LayerId: %u.", layerId);
        }
    }
}

void DisplayDev::SetLayerColorKey(uint8_t layerId, bool enColorKey, uint32_t color)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX) {
        GRAPHIC_LOGE("Invalid layerId[%u].", layerId);
        return;
    }

    if (layerFuncs_->SetLayerColorKey != nullptr) {
        if (layerFuncs_->SetLayerColorKey(devId_, layer_[layerId].layerHdl, enColorKey, color) !=
            DISPLAY_SUCCESS) {
            GRAPHIC_LOGE("SetLayerColorKey failed. LayerId: %u, enColorKey: %d.", layerId, enColorKey);
        }
    }
}

void DisplayDev::SetLayerZOrder(uint8_t layerId, uint8_t order)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX) {
        GRAPHIC_LOGE("Invalid layerId[%u].", layerId);
        return;
    }

    if (layerFuncs_->SetLayerZorder != nullptr) {
        if (layerFuncs_->SetLayerZorder(devId_, layer_[layerId].layerHdl, order) !=
            DISPLAY_SUCCESS) {
            GRAPHIC_LOGE("SetLayerZOrder failed. LayerId: %u, order: %u.", layerId, order);
        }
    }
}

void DisplayDev::SetLayerPositon(uint8_t layerId, int16_t x, int16_t y)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX) {
        GRAPHIC_LOGE("Invalid layerId[%u].", layerId);
        return;
    }

    layer_[layerId].pos.x = x;
    layer_[layerId].pos.y = y;
    if (layerFuncs_->SetLayerSize != nullptr) {
        IRect rect = {x, y, 0, 0};
        if (layerFuncs_->SetLayerSize(devId_, layer_[layerId].layerHdl, &rect) != DISPLAY_SUCCESS) {
            GRAPHIC_LOGE("SetLayerSize failed.");
        }
    }
}

void DisplayDev::SetHardwareLayerBuffer(uint8_t layerId, uint8_t bufferId, const LayerBuffer& layer)
{
    if (layerId < LAYER_0 || layerId >= LAYER_MAX || bufferId >= layer_[layerId].bufferCount) {
        GRAPHIC_LOGE("Invalid layerId[%u] or invliad bufferId[%u].", layerId, bufferId);
        return;
    }

    if (layerId == LAYER_0) {
        GRAPHIC_LOGE("Not allowed to set LAYER_0.");
    } else {
        layer_[layerId].buffer[bufferId].data.virAddr = layer.data.virAddr;
        layer_[layerId].buffer[bufferId].data.phyAddr = layer.data.phyAddr;
        layer_[layerId].buffer[bufferId].width = layer.width;
        layer_[layerId].buffer[bufferId].height = layer.height;
        layer_[layerId].buffer[bufferId].pitch = layer.pitch;
        layer_[layerId].buffer[bufferId].pixFormat = layer.pixFormat;
    }
}

void DisplayDev::DisplayDevInit()
{
    if (isDisplayDevInited_) {
        GRAPHIC_LOGE("DisplayDev is already inited.");
        return;
    }

    if (LayerInitialize(&layerFuncs_) != DISPLAY_SUCCESS) {
        GRAPHIC_LOGE("layer initialize failed.");
        return;
    }

    if (layerFuncs_->InitDisplay == nullptr || layerFuncs_->InitDisplay(devId_) != DISPLAY_SUCCESS) {
        GRAPHIC_LOGE("InitDisplay is nullptr or InitDisplay failed.");
        return;
    }

    LayerInfo layerInfo;
    LayerRotateType type = static_cast<LayerRotateType>(HW_ROTATE_ANGLE);
    layerInfo.width = HORIZONTAL_RESOLUTION;
    layerInfo.height = VERTICAL_RESOLUTION;
    layerInfo.pixFormat = layerPixFmt_;
    layerInfo.bpp = layerBpp_;
    layerInfo.type = LAYER_TYPE_GRAPHIC;

    uint8_t bufferCount = IsDoubleBuffer() ? 2 : 1;
    if (!OpenLayer(LAYER_0, layerInfo, bufferCount, type)) {
        GRAPHIC_LOGE("OpenLayer failed.");
        return;
    }

    if (!AllocLayerBuffer(LAYER_0)) {
        CloseLayer(LAYER_0);
        GRAPHIC_LOGE("AllocLayerBuffer failed.");
        return;
    }

    Rect dirtyRect;
    dirtyRect.SetRect(0, 0, layer_[LAYER_0].layerInfo.width - 1, layer_[LAYER_0].layerInfo.height - 1);
    SetLayerDirtyRegion(LAYER_0, dirtyRect);
    isDisplayDevInited_ = true;
}

void DisplayDev::DisplayDevClose()
{
    if (layerFuncs_ == nullptr || layerFuncs_->CloseLayer == nullptr || layerFuncs_->DeinitDisplay == nullptr) {
        return;
    }

    FreeLayerBuffer(LAYER_0);
    CloseLayer(LAYER_0);
    if (layerFuncs_->DeinitDisplay(devId_) != DISPLAY_SUCCESS) {
        GRAPHIC_LOGE("DeinitDisplay failed.");
        return;
    }
    if (LayerUninitialize(layerFuncs_) != DISPLAY_SUCCESS) {
        GRAPHIC_LOGE("LayerUninitialize failed.");
        return;
    }
    isDisplayDevInited_ = false;
}

#if defined(HW_ROTATE_ANGLE)
bool LiteMGfxEngine::RotateBufferIfNecessary()
{
    LayerRotateType rotateType = DisplayDev::GetInstance()->GetLayerRotateType(LAYER_0);
    if (rotateType == LayerRotateType::LAYER_ROTATE_NONE) {
        return true;
    }
    LayerBuffer* rotateBuffer = DisplayDev::GetInstance()->GetCurrentBuffer(LAYER_0);
    LayerBuffer* buffer = DisplayDev::GetInstance()->GetCurrentRotateBuffer(LAYER_0);
    if (rotateBuffer == nullptr || rotateBuffer->data.virAddr == nullptr ||
        buffer == nullptr || buffer->data.virAddr == nullptr) {
        return false;
    }

    BufferInfo src;
    src.rect = {0, 0, static_cast<int16_t>(rotateBuffer->width - 1), static_cast<int16_t>(rotateBuffer->height - 1)};
    src.virAddr = rotateBuffer->data.virAddr;
    src.phyAddr = src.virAddr;
    src.stride = rotateBuffer->pitch;
    src.mode = HWDrawUtils::ConvertPixelFmt2ColorMode(rotateBuffer->pixFormat);
    src.color = 0;
    src.width = rotateBuffer->width;
    src.height = rotateBuffer->height;
    src.compressMode = COMPRESS_MODE_NONE;

    BufferInfo dst;
    dst.rect = {0, 0, static_cast<int16_t>(buffer->width - 1), static_cast<int16_t>(buffer->height - 1)};
    dst.virAddr = buffer->data.virAddr;
    dst.phyAddr = dst.virAddr;
    dst.stride = buffer->pitch;
    dst.mode = HWDrawUtils::ConvertPixelFmt2ColorMode(buffer->pixFormat);
    dst.color = 0;
    dst.width = buffer->width;
    dst.height = buffer->height;
    dst.compressMode = COMPRESS_MODE_NONE;

    BlendOption blendOption;

    switch (rotateType) {
        case LayerRotateType::LAYER_ROTATE_90: {
            float rotAngle = 90;
            blendOption.transMap.Rotate(rotAngle, Vector2<float>{0, 0});
            blendOption.transMap.Translate(Vector2<int16_t>{static_cast<int16_t>(src.height), 0});
            break;
        }
        case LayerRotateType::LAYER_ROTATE_180: {
            float rotAngle = 180;
            blendOption.transMap.Rotate(rotAngle, Vector2<float>{0, 0});
            blendOption.transMap.Translate(
                Vector2<int16_t>{static_cast<int16_t>(src.width), static_cast<int16_t>(src.height)});
            break;
        }
        case LayerRotateType::LAYER_ROTATE_270: {
            float rotAngle = 270;
            blendOption.transMap.Rotate(rotAngle, Vector2<float>{0, 0});
            blendOption.transMap.Translate(Vector2<int16_t>{0, static_cast<int16_t>(src.width)});
            break;
        }
        default:
            GRAPHIC_LOGE("error rotate type %d", rotateType);
            return false;
    }

    blendOption.opacity = OPA_OPAQUE;
    blendOption.mode = BLEND_SRC_OVER;
    BaseGfxEngine::GetInstance()->Blit(dst, {0, 0}, src, dst.rect, blendOption);

    return true;
}
#endif
} // namespace OHOS
