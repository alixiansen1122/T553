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
#include "hals/gralloc_engines.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/graphic_math.h"
#include "graphic_service.h"
#include "common/lite_surface.h"

namespace OHOS {
LiteSurface::LiteSurface() : bufferCount_(0), queueSize_(0), size_(0),
    width_(0), height_(0), strideAlignment_(0), format_(PIXEL_FMT_BUTT),
    consumerListener_(nullptr), waitPrepareBuffer_(std::make_shared<bool>(false))
{
}

LiteSurface::~LiteSurface()
{
    ClearBuffers();
}

void LiteSurface::AsyncClearBuffers()
{
    std::weak_ptr<bool> wp = waitPrepareBuffer_;
    GraphicService::GetInstance()->PostGraphicEvent([this, wp]() {
        if (!wp.expired()) {
            ClearBuffers();
        }
    });
}

void LiteSurface::ClearBuffers()
{
    lock_.Lock();
    freeList_.Clear();
    dirtyList_.Clear();
    ListNode<BufferItem*>* node = buffers_.Head();
    while (node != buffers_.End()) {
        BufferItem* bufferItem = node->data_;
        GrallocEngines::GetInstance()->FreeBuffer(reinterpret_cast<uint8_t*>(bufferItem->buffer->addr));
        SurfaceBuffer* surfaceBuffer = bufferItem->buffer;
        surfaceBuffer->addr = 0;
        delete surfaceBuffer;
        surfaceBuffer = nullptr;
        delete bufferItem;
        bufferItem = nullptr;
        node = node->next_;
    }
    buffers_.Clear();
    bufferCount_ = 0;
    lock_.Unlock();
}

void LiteSurface::PrepareBuffers()
{
    bool waitPrepareBuffer = *waitPrepareBuffer_;
    if (!waitPrepareBuffer) {
        *waitPrepareBuffer_ = true;
        std::weak_ptr<bool> wp = waitPrepareBuffer_;
        GraphicService::GetInstance()->PostGraphicEvent([this, wp]() {
            if (!wp.expired()) {
                PrepareBuffersInner();
                *waitPrepareBuffer_ = false;
            }
        });
        GRAPHIC_LOGD("Post PrepareBuffers Event. BufferCount: %d, QueueSize: %d.", bufferCount_, queueSize_);
    } else {
        GRAPHIC_LOGW("Waiting Preparing buffer!");
    }
}

void LiteSurface::PrepareBuffersInner()
{
    if (bufferCount_ != 0) {
        GRAPHIC_LOGD("Already prepared buffers, bufferCount_ is %u", bufferCount_);
        return;
    }
    for (uint8_t i = 0; i < queueSize_; i++) {
        AddBuffer();
    }
}

void LiteSurface::AddBuffer()
{
    if (queueSize_ == bufferCount_) {
        GRAPHIC_LOGW("bufferCount is equal to queueSize. cannot add more buffer");
        return;
    }
    BufferItem* bufferItem = new BufferItem();
    if (bufferItem == nullptr) {
        GRAPHIC_LOGE("new BufferItem failed.");
        return;
    }

    uint8_t* addr = GrallocEngines::GetInstance()->AllocBufferBySize(size_,
        HBM_USE_ASSIGN_SIZE | HBM_USE_MEM_YUV);
    if (addr == nullptr) {
        GRAPHIC_LOGE("Alloc memory for buffer failed.\n");
        delete bufferItem;
        return;
    }

    SurfaceBuffer* surfaceBuffer = new SurfaceBuffer();
    if (surfaceBuffer == nullptr) {
        GRAPHIC_LOGE("new surfaceBuffer failed.");
        GrallocEngines::GetInstance()->FreeBuffer(addr);
        delete bufferItem;
        return;
    }

    bufferItem->buffer = surfaceBuffer;

    surfaceBuffer->addr = reinterpret_cast<uint32_t>(addr);
    surfaceBuffer->width = width_;
    surfaceBuffer->height = height_;
    surfaceBuffer->stride = GetStride();
    surfaceBuffer->format = static_cast<PixelFormat>(format_);

    freeList_.PushBack(surfaceBuffer);
    buffers_.PushBack(bufferItem);
    ++bufferCount_;
}

bool LiteSurface::CanRequest(uint8_t wait)
{
    bool res = false;
    if (!freeList_.IsEmpty()) {
        res = true;
        goto END;
    }
    if (bufferCount_ < queueSize_) {
        PrepareBuffers();
        res = false;
        goto END;
    }
    if (wait) {
        cond_.WaitTime(lock_, 16); // 16: 16ms
        res = true;
    }
END:
    return res;
}

BufferItem* LiteSurface::GetBufferItem(const SurfaceBuffer* buffer)
{
    if (buffer == nullptr) {
        return nullptr;
    }

    ListNode<BufferItem*>* cur = buffers_.Head();
    while (cur != buffers_.End()) {
        if ((cur->data_->buffer == buffer) && (cur->data_->buffer->addr == buffer->addr)) {
            return cur->data_;
        }
        cur = cur->next_;
    }
    return nullptr;
}

void LiteSurface::SetWidthAndHeight(uint32_t width, uint32_t height)
{
    if (width_ != 0 || height_ != 0) {
        GRAPHIC_LOGE("Can Only Set once! width is %u, height is %u.\n", width_, height_);
        return;
    }
    width_ = width;
    height_ = height;
}

uint32_t LiteSurface::GetWidth()
{
    return width_;
}

uint32_t LiteSurface::GetHeight()
{
    return height_;
}

void LiteSurface::SetFormat(uint32_t format)
{
    if (format_ != PIXEL_FMT_BUTT) {
        GRAPHIC_LOGE("Can Only Set once! Current format is %u.\n", format_);
        return;
    }
    format_ = format;
}

uint32_t LiteSurface::GetFormat()
{
    return format_;
}

void LiteSurface::SetSize(uint32_t size)
{
    if (size_ != 0) {
        GRAPHIC_LOGE("Can Only Set once! Current size is %u\n", size_);
        return;
    }
    size_ = size;
}

uint32_t LiteSurface::GetSize()
{
    return size_;
}

void LiteSurface::SetQueueSize(uint8_t queueSize)
{
    if (queueSize_ != 0) {
        GRAPHIC_LOGE("Can Only Set once! Current queueSize is %u\n", queueSize_);
        return;
    }
    if (queueSize > maxQueueSize_) {
        GRAPHIC_LOGE("queueSize must be smaller than %u. Set to %u instead.", maxQueueSize_, maxQueueSize_);
        queueSize_ = maxQueueSize_;
        return;
    }
    queueSize_ = queueSize;
}

uint8_t LiteSurface::GetQueueSize()
{
    return queueSize_;
}

void LiteSurface::RegisterConsumerListener(IBufferConsumerListener& listener)
{
    consumerListener_ = &listener;
}

void LiteSurface::UnregisterConsumerListener()
{
    consumerListener_ = nullptr;
}

void LiteSurface::SetStrideAlignment(uint32_t strideAlignment)
{
    if (strideAlignment_ != 0) {
        GRAPHIC_LOGE("Can Only Set once! Current alignment is %u\n", strideAlignment_);
        return;
    }
    strideAlignment_ = strideAlignment;
}

uint32_t LiteSurface::GetStrideAlignment()
{
    return strideAlignment_;
}

uint32_t LiteSurface::GetStride()
{
    if (width_ == 0) {
        GRAPHIC_LOGW("Width is not set, return 0.\n");
        return 0;
    }
    if (strideAlignment_ == 0) {
        GRAPHIC_LOGI("strideAlignment is not set, return width.\n");
        return width_;
    }
    return ((width_) + (strideAlignment_) - 1) & (~((strideAlignment_) - 1));
}

SurfaceBuffer* LiteSurface::AcquireBuffer()
{
    lock_.Lock();
    if (!dirtyList_.IsEmpty()) {
        SurfaceBuffer* surfaceBuffer = dirtyList_.Front();
        dirtyList_.PopFront();
        BufferItem* bufferItem = GetBufferItem(surfaceBuffer);
        if (bufferItem == nullptr) {
            GRAPHIC_LOGE("Acquired buffer is invalid: %p", surfaceBuffer);
            lock_.Unlock();
            return nullptr;
        }
        bufferItem->state = BufferState::BUFFER_STATE_ACQUIRE;
        lock_.Unlock();
        return surfaceBuffer;
    }
    lock_.Unlock();
    return nullptr;
}

bool LiteSurface::ReleaseBuffer(SurfaceBuffer* buffer)
{
    lock_.Lock();
    BufferItem* bufferItem = GetBufferItem(buffer);
    if (bufferItem == nullptr) {
        GRAPHIC_LOGE("Release invalid buffer: %p", buffer);
        lock_.Unlock();
        return false;
    }
    bufferItem->state = BufferState::BUFFER_STATE_RELEASE;
    freeList_.PushBack(buffer);
    cond_.Signal();
    lock_.Unlock();
    return true;
}

int32_t LiteSurface::FlushBuffer(SurfaceBuffer* buffer)
{
    lock_.Lock();
    BufferItem* bufferItem = GetBufferItem(buffer);
    if (bufferItem == nullptr) {
        GRAPHIC_LOGE("Flush invalid buffer: %p", buffer);
        lock_.Unlock();
        return -1;
    }
    bufferItem->state = BufferState::BUFFER_STATE_FLUSH;
    dirtyList_.PushBack(buffer);
    if (consumerListener_ != nullptr) {
        consumerListener_->OnBufferAvailable();
    }
    lock_.Unlock();
    return 0;
}

SurfaceBuffer* LiteSurface::RequestBuffer(uint8_t wait)
{
    if (queueSize_ == 0 || size_ == 0 || width_ == 0 || height_ == 0 || format_ == PIXEL_FMT_BUTT) {
        GRAPHIC_LOGE("cannot request buffer. queueSize or size or width or height or format is not set or is invalid.");
        return nullptr;
    }

    lock_.Lock();

    do {
        if (!CanRequest(wait)) {
            break;
        }
        if (!freeList_.IsEmpty()) {
            SurfaceBuffer* buffer = freeList_.Front();
            freeList_.PopFront();

            BufferItem* bufferItem = GetBufferItem(buffer);
            if (bufferItem == nullptr) {
                GRAPHIC_LOGE("Requested buffer is invalid: %p", buffer);
                lock_.Unlock();
                return nullptr;
            }
            bufferItem->state = BufferState::BUFFER_STATE_REQUEST;
            lock_.Unlock();
            return buffer;
        }
    } while (0);
    lock_.Unlock();
    return nullptr;
}

void LiteSurface::CancelBuffer(SurfaceBuffer* buffer)
{
    ReleaseBuffer(buffer);
}

SurfaceBuffer* LiteSurface::GetBackBuf()
{
    return dirtyList_.Back();
}
} // namespace OHOS
