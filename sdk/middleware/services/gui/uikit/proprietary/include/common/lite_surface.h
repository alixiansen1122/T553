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
 * @addtogroup UI_Common
 * @{
 *
 * @brief Defines common UI capabilities, such as image and text processing.
 *
 */

/**
 * @file lite_surface.h
 *
 * @brief Declares the <b>LiteSurface</b> class of the graphics module, which provides functions for media display
 *
 */


#ifndef GRAPHIC_LITE_LITE_SURFACE_H
#define GRAPHIC_LITE_LITE_SURFACE_H

#include <pthread.h>
#include <memory>
#include "graphic_utils.h"
#include "gfx_utils/list.h"
#include "gfx_utils/pixel_format_utils.h"
#include "ibuffer_consumer_listener.h"
#include "surface.h"
#include "graphic_mutex.h"
#include "common/graphic_cond_ext.h"

namespace OHOS {
enum class BufferState {
    BUFFER_STATE_NONE = 0,
    BUFFER_STATE_REQUEST,
    BUFFER_STATE_FLUSH,
    BUFFER_STATE_ACQUIRE,
    BUFFER_STATE_RELEASE
};

struct BufferItem {
    SurfaceBuffer* buffer;
    BufferState state;
};

/**
 *
 * @brief Declares the <b>LiteSurface</b> class of the graphics module, which provides functions for media display
 *
 */
class LiteSurface : public Surface {
public:
    /**
     * @brief A constructor used to init the <b>LiteSurface</b> instance.
     * This function will set default value for  Variables.
     * @since 1.0
     * @version 1.0
     */
    LiteSurface();

    /**
     * @brief A destructor used to delete the <b>LiteSurface</b> instance.
     * This function releases the LiteSurface and all buffers applied for the LiteSurface.
     * @since 1.0
     * @version 1.0
     */
    ~LiteSurface() override;

    /**
     * @brief Sets the number of buffers that can be allocated to the LiteSurface.
     * The max is 3. if queueSize is greater than 3, it would be set to 3 instead.
     * can only be called once during its lifetime.
     * @param queueSize Indicates the number of buffers to set.
     * @since 1.0
     * @version 1.0
     */
    void SetQueueSize(uint8_t queueSize) override;

    /**
     * @brief Obtains the number of LiteSurface buffers that can be allocated to the LiteSurface.
     * @return Returns the number of buffers.
     * @since 1.0
     * @version 1.0
     */
    uint8_t GetQueueSize() override;

    /**
     * @brief Sets the width and height of the LiteSurface for calculating its stride and size.
     * can only be called once during its lifetime.
     * @param width Indicates the LiteSurface width, in pixels.
     * @param height Indicates the LiteSurface height, in pixels.
     * @since 1.0
     * @version 1.0
     */
    void SetWidthAndHeight(uint32_t width, uint32_t height) override;

    /**
     * @brief Obtains the width of the LiteSurface.
     * @return Returns the LiteSurface width, in pixels.
     * @since 1.0
     * @version 1.0 */
    uint32_t GetWidth() override;

    /**
     * @brief Obtains the height of the LiteSurface.
     * @return Returns the LiteSurface height, in pixels.
     * @since 1.0
     * @version 1.0
     */
    uint32_t GetHeight() override;

    /**
     * @brief Sets the pixel format of the LiteSurface. For details, see {@link PixelFormat}.
     * Can only be called once during its lifetime.
     * @param format Indicates the pixel format to be set.
     * @since 1.0
     * @version 1.0
     */
    void SetFormat(uint32_t format) override;

    /**
     * @brief Obtains the pixel format of the LiteSurface. For details, see {@link PixelFormat}.
     * @return Returns the pixel format.
     * @since 1.0
     * @version 1.0
     */
    uint32_t GetFormat() override;

    /**
     * @brief Sets the usage scenario of the buffer. Physically contiguous memory and virtual memory (by default)
     * are supported. By default, virtual memory is allocated.
     * @param usage Indicates the usage scenario of the buffer. For details, see {@link BUFFER_CONSUMER_USAGE}.
     * @since 1.0
     * @version 1.0
     */
    void SetUsage(uint32_t usage) override
    {
        UNUSED(usage);
    }

    /**
     * @brief Obtains the usage scenario of the buffer. Physically contiguous memory and virtual memory are supported.
     * @return Returns the usage scenario of the buffer. For details, see {@link BUFFER_CONSUMER_USAGE}.
     * @since 1.0
     * @version 1.0
     */
    uint32_t GetUsage() override
    {
        return 0;
    }

    /**
     * @brief Sets LiteSurface user data, which is stored in the format of <key, value>.
     * @param key Indicates the key of a key-value pair to store.
     * @param value Indicates the value of the key-value pair to store.
     * @since 1.0
     * @version 1.0
     */
    void SetUserData(const std::string& key, const std::string& value) override
    {
        UNUSED(key);
        UNUSED(value);
    }

    /**
     * @brief Obtains LiteSurface user data.
     * @param key Indicates the key of a key-value pair for which the value is to be obtained.
     * @return Returns the value of the key-value pair obtained.
     * @since 1.0
     * @version 1.0
     */
    std::string GetUserData(const std::string& key) override
    {
        UNUSED(key);
        return "";
    }

    /**
     * @brief Registers a consumer listener.
     * When a buffer is placed in the dirty queue, {@link OnBufferAvailable} is called to notify consumers.
     * If the listener is repeatedly registered, only the latest one is retained.
     * @param IBufferConsumerListener Indicates the listener to register.
     * @since 1.0
     * @version 1.0
     */
    void RegisterConsumerListener(IBufferConsumerListener& listener) override;

    /**
     * @brief Unregisters the consumer listener.
     * After the listener is unregistered, no callback is triggered when a buffer is placed in the dirty queue.
     * @since 1.0
     * @version 1.0
     */
    void UnregisterConsumerListener() override;

    /**
     * @brief Sets the number of bytes for stride alignment.
     * @param strideAlignment Indicates the number of bytes for stride alignment.
     * @since 1.0
     * @version 1.0
     */
    void SetStrideAlignment(uint32_t strideAlignment) override;

    /**
     * @brief Obtains the number of bytes for stride alignment. By default, 4-byte aligned is used.
     * @return Returns the number of bytes for stride alignment.
     * @since 1.0
     * @version 1.0
     */
    uint32_t GetStrideAlignment() override;

    /**
     * @brief Obtains the stride of the LiteSurface.
     * @return Returns the stride.
     * @since 1.0
     * @version 1.0
     */
    uint32_t GetStride() override;

    /**
     * @brief Sets the size of the shared memory to allocate.
     * @param size Indicates the size of the shared memory.
     * @since 1.0
     * @version 1.0
     */
    void SetSize(uint32_t size) override;

    /**
     * @brief Obtains the size of the shared memory to allocate.
     * @return Returns the size of the shared memory.
     * @since 1.0
     * @version 1.0
     */
    uint32_t GetSize() override;

    /**
     * @brief Obtains a buffer to write data.
     * @param wait Specifies whether the function waits for an available buffer. If <b>wait</b> is <b>1</b>,
     * the function waits until there is an available buffer in the free queue before returning a pointer.
     * If the <b>wait</b> is <b>0</b>, the function does not wait and returns <b>nullptr</b> if there is no buffer
     * in the free queue. The default value is <b>0</b>.
     * @return Returns the pointer to the buffer if the operation is successful; returns <b>nullptr</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    SurfaceBuffer* RequestBuffer(uint8_t wait = 0) override;

    /**
     * @brief Flushes a buffer to the dirty queue for consumers to use.
     * @param SurfaceBuffer Indicates the pointer to the buffer flushed by producers.
     * @return Returns <b>0</b> if the operation is successful; returns <b>-1</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t FlushBuffer(SurfaceBuffer* buffer) override;

    /**
     * @brief Obtains a buffer.
     * Consumers can use this function to obtain the buffer placed in the dirty queue by producers.
     * If there is no buffer in the queue, <b>nullptr</b> is returned.
     * @return Returns the pointer to the {@link SurfaceBuffer} object.
     * @since 1.0
     * @version 1.0
     */
    SurfaceBuffer* AcquireBuffer() override;

    /**
     * @brief Releases the consumed buffer.
     * After a consumer has used a {@link SurfaceBuffer} object, the consumer can release it through
     * {@link ReleaseBuffer}. The released object is placed into the free queue so that producers can
     * apply for the object.
     * @param SurfaceBuffer Indicates the pointer to the buffer released.
     * @return Returns <b>true</b> if the buffer is released; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool ReleaseBuffer(SurfaceBuffer* buffer) override;

    /**
     * @brief producer cancels a buffer to the free queue.
     * @param SurfaceBuffer Indicates the pointer to the buffer to be released by producers.
     * @since 1.0
     * @version 1.0
     */
    void CancelBuffer(SurfaceBuffer* buffer) override;

    /**
     * @brief Prepare video buffers. This should be called before playing video.
     * @since 1.0
     * @version 1.0
     */
    void PrepareBuffers();

    void AsyncClearBuffers();

    void ClearBuffers();
    SurfaceBuffer* GetBackBuf();

protected:
    void PrepareBuffersInner();
    void AddBuffer();
    BufferItem* GetBufferItem(const SurfaceBuffer* buffer);
    bool CanRequest(uint8_t wait);

    const uint8_t maxQueueSize_ = 3; // 3: max queue size
    uint8_t bufferCount_;
    uint8_t queueSize_;
    uint32_t size_;
    uint32_t width_;
    uint32_t height_;
    uint32_t strideAlignment_;
    uint32_t format_;
    GraphicMutex lock_;
    GraphicCondExt cond_;
    List<BufferItem*> buffers_;
    List<SurfaceBuffer*> freeList_;
    List<SurfaceBuffer*> dirtyList_;
    IBufferConsumerListener* consumerListener_;
    std::shared_ptr<bool> waitPrepareBuffer_;
};
} // end namespace
#endif
/**
 * @}
 */
