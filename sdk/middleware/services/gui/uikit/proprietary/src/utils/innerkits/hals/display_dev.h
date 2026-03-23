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

#ifndef GRAPHIC_LITE_DISPLAY_DEV_H
#define GRAPHIC_LITE_DISPLAY_DEV_H

#include "display_layer.h"
#include "display_type.h"
#include "gfx_utils/color.h"
#include "gfx_utils/heap_base.h"
#include "gfx_utils/rect.h"
#include "gfx_utils/pixel_format_utils.h"
#include "common/lite_surface.h"

#ifdef _WIN32
#ifdef ERROR
#undef ERROR
#endif
#endif

namespace OHOS {
enum class LayerRotateType {
    LAYER_ROTATE_NONE = 0,        /**< No rotation */
    LAYER_ROTATE_90,              /**< Rotation by 90 degrees */
    LAYER_ROTATE_180,             /**< Rotation by 180 degrees */
    LAYER_ROTATE_270,             /**< Rotation by 270 degrees */
    LAYER_ROTATE_BUTT             /**< Invalid operation */
};

enum LayerId : uint8_t {
    LAYER_0 = 0,
    LAYER_1,
    LAYER_2,
    LAYER_MAX,
};

enum BufferId : uint8_t {
    BUFFER_0 = 0,
    BUFFER_1,
    BUFFER_2,
    BUFFER_MAX,
};

class DisplayDev : public HeapBase {
public:
    static DisplayDev* GetInstance()
    {
        static DisplayDev instance;
        return &instance;
    }

    /**
     * @brief Initialize display, layerFuncs and  master hardware layer.
     */
    void DisplayDevInit();

    /**
     * @brief Deinit display, layerFuncs and master hardware layer.
     */
    void DisplayDevClose();

    /**
     * @brief Get layer rotate type.
     */
    LayerRotateType GetLayerRotateType(uint8_t layerId);

    /**
     * @brief flush
     */
    void LcdFlush(uint8_t layerId);

    /**
     * @brief wait for vertical blanking.
     */
    void WaitForVBlank();

    /**
     * @brief set layer dirty region.
     */
    void SetLayerDirtyRegion(uint8_t layerId, const Rect& rect);

    /**
     * @brief set layer color key.
     */
    void SetLayerColorKey(uint8_t layerId, bool enColorKey, uint32_t color = 0xffffffff);

    /**
     * @brief set layer z order.
     */
    void SetLayerZOrder(uint8_t layerId, uint8_t order);

    /**
     * @brief set layer position.
     */
    void SetLayerPositon(uint8_t layerId, int16_t x, int16_t y);

    /**
     * @brief set layer visible state.
     */
    void SetLayerVisible(uint8_t layerId, bool visible);

    /**
     * @brief Alloc memory for all buffers of a given layer.
     * must call OpenLayer before calling this.
     * need to call FreeLayerBuffer accordingly.
     */
    bool AllocLayerBuffer(uint8_t layerId);

    /**
     * @brief Free memory for all buffers of a given layer if the memory was allocated in AllocLayerBuffer.
     */
    void FreeLayerBuffer(uint8_t layerId);

    /**
     * @brief Open a given layer.
     * Must call this before all other layer-related operations.
     */
    bool OpenLayer(uint8_t layerId, const LayerInfo& info, uint8_t bufferCount,
        LayerRotateType rotateType = LayerRotateType::LAYER_ROTATE_NONE);

    /**
     * @brief Close a given layer.
     */
    void CloseLayer(uint8_t layerId);

    /**
     * @brief Set layerbuffer for the given buffer.
     */
    void SetHardwareLayerBuffer(uint8_t layerId, uint8_t bufferId, const LayerBuffer& layer);

    /**
     * @brief Get current LayerBuffer in given layer.
     * This is often used when there is one-to-many relationship between layer and layerBuffer.
     */
    LayerBuffer* GetCurrentBuffer(uint8_t layerId);

    LayerBuffer* GetNextBuffer(uint8_t layerId);

    LayerBuffer* GetCurrentRotateBuffer(uint8_t layerId);

    void GetRotatePosition(int32_t width, int32_t height, int32_t &x, int32_t &y, LayerRotateType type) const;
    void GetRotateRect(int32_t width, int32_t height, Rect &rect, LayerRotateType type) const;

    uint32_t CalcStride(uint32_t width, PixelFormat fmt, uint8_t cmpMode = 0);
    uint32_t CalcSize(uint32_t stride, uint32_t height, uint8_t cmpMode);
    bool IsUlpsEnabled(void);
    bool TryExitUlps(bool isFlushing);
    bool IsDoubleBuffer(void);

    /**
     * @brief swap current LayerBuffer in given layer.
     * @return Returns <b>true</b> if swaps successfully; returns <b>false</b> otherwise.
     */
    bool SwapBuffer(uint8_t layerId);

    void SetPendingReleaseSurface(std::shared_ptr<LiteSurface> surface);

    struct Layer {
        uint32_t layerHdl;
        LayerInfo layerInfo;
        LayerInfo rotateInfo;
        uint8_t bufferCount;
        LayerBuffer* buffer;
        LayerBuffer* rotate;
        LayerRotateType rotateType;
        Point pos;
    };
    Layer layer_[LAYER_MAX];

private:
    DisplayDev();
    virtual ~DisplayDev() {}
    bool AllocBuffer(LayerBuffer *layerBuffer, const LayerInfo &layerInfo, uint8_t count, bool isFb = true);
    void FreeBuffer(LayerBuffer *buffer, uint8_t count);

    bool isDisplayDevInited_ : 1;
    uint32_t devId_;
    LayerFuncs* layerFuncs_;
    uint8_t currentBufferId_[LAYER_MAX];
    bool exitUlps_;

#ifdef LAYER_PF_ARGB1555
    const uint8_t layerBpp_ = 16;
    const PixelFormat layerPixFmt_ = PIXEL_FMT_RGBA_5551;
#elif defined LAYER_PF_ARGB8888
    const uint8_t layerBpp_ = 32;
    const PixelFormat layerPixFmt_ = PIXEL_FMT_RGBA_8888;
#elif defined LAYER_PF_RGB888
    const uint8_t layerBpp_ = 24;
    const PixelFormat layerPixFmt_ = PIXEL_FMT_RGB_888;
#endif
    std::shared_ptr<LiteSurface> pendingSurface_;
};
} // namespace OHOS
#endif // GRAPHIC_LITE_DISPLAY_DEV_H
