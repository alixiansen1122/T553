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

#ifndef GRAPHIC_LITE_LITE_M_GFX_ENGINE_H
#define GRAPHIC_LITE_LITE_M_GFX_ENGINE_H

#include "gfx_utils/list.h"
#include "graphic_config.h"
#include "display_type.h"
#include "display_vgu.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "gfx_utils/geometry2d.h"
#include "gfx_utils/graphic_buffer.h"
#include "gfx_utils/graphic_math.h"
#include "gfx_utils/graphic_types.h"
#include "gfx_utils/heap_base.h"
#include "gfx_utils/style.h"
#include "gfx_utils/transform.h"
#include "gfx_utils/vector.h"
#include "common/ihardware_view.h"
#include "common/graphic_hardware_types.h"

namespace OHOS {
struct EllipseInfo {
    Point center;
    uint16_t width;
    uint16_t height;
};

struct AsyncSubmitInfo {
    Rect rect;
    bool hasPendingFrame;
    bool needSwapToPendingFrame;
};

static constexpr uint16_t INIT_PATH_NUM = 20;

class LiteMGfxEngine : public BaseGfxEngine {
public:
    using BaseGfxEngine::Flush;
    static LiteMGfxEngine* GetInstance()
    {
        static LiteMGfxEngine instance;
        return &instance;
    }

    uint8_t* AllocBuffer(uint32_t size, uint32_t usage) override;
    void FreeBuffer(uint8_t* buffer) override;
    BufferInfo* GetFBBufferInfo() override;
    BufferInfo* GetNextFBBufferInfo() override;
    void Flush(const Rect& rect);
    bool SwapBuffer(const Rect* rect);
    void EnableAsyncMode(bool enable);
    bool IsAsyncModeEnabled();
    void StartDraw();

#if ENABLE_VGU_ENGINE
    bool InitDriver();
    void CloseDriver();
    void SyncHwDraw(bool isNeedWait = true);

    void DrawArc(BufferInfo& dst, ArcInfo& arcInfo, const Rect& mask,
        const Style& style, OpacityType opacity, uint8_t cap) override;

    void DrawLine(BufferInfo& dst, const Point& start, const Point& end,
        const Rect& mask, int16_t width, ColorType color, OpacityType opacity) override;

    void DrawLetter(BufferInfo& gfxDstBuffer, const uint8_t* fontMap,
        const Rect& fontRect, const Rect& subRect, const uint8_t fontWeight,
        const ColorType& color, const OpacityType opa) override;

    void DrawCubicBezier(BufferInfo& dst, const Point& start, const Point& control1,
        const Point& control2, const Point& end, const Rect& mask,
        int16_t width, ColorType color, OpacityType opacity) override;

    void DrawRect(BufferInfo& dst,
        const Rect& rect,
        const Rect& dirtyRect,
        const Style& style,
        OpacityType opacity) override;

    void DrawRect(BufferInfo& dst,
        const Rect& rect,
        const Rect& dirtyRect,
        const Style& style,
        OpacityType opacity,
        TransformMap* transMap);

    void DrawTransform(BufferInfo& dst,
        const Rect& mask,
        const Point& position,
        ColorType color,
        OpacityType opacity,
        const TransformMap& transMap,
        const TransformDataInfo& dataInfo) override;

    void Blit(BufferInfo& dst,
        const Point& dstPos,
        const BufferInfo& src,
        const Rect& subRect,
        const BlendOption& blendOption) override;

    void Blur(BufferInfo& dst, float radius) override;

    void Fill(BufferInfo& dst,
        const Rect& fillArea,
        const ColorType color,
        const OpacityType opacity) override;

    void DrawEllipse(BufferInfo& dst,
        const Rect& mask,
        const Point& center,
        uint16_t rx,
        uint16_t ry,
        VGUStrokeAttr* strokeAttr,
        VGUFillAttr* fillAttr,
        VGUMatrix3* matrix);

#if CAPABILITY_HWDRAW_PATH
    uintptr_t InitPath(bool enAlias);
    void DeinitPath(uintptr_t path) const;
    bool AppendPathData(uintptr_t path, uint32_t num, const uint8_t* cmd, uint32_t dataNum, const float* data) const;
    bool SetPathData(uintptr_t path, uint32_t num, const uint8_t* cmd, uint32_t dataNum, const float* data) const;
#endif
    void FillPath(BufferInfo& dst, uintptr_t path, const Rect& mask,
        ColorType color, OpacityType opacity, VGUMatrix3* matrix);

    void StrokePath(BufferInfo& dst, uintptr_t path, const Rect& mask,
        int16_t width, ColorType color, OpacityType opacity, VGUMatrix3* matrix);

    void RenderPath(BufferInfo& dst, uintptr_t path, const Rect& mask,
        int16_t strokeWidth, ColorType strokeColor, ColorType fillColor,
        OpacityType opacity, VGUMatrix3* matrix);

    void RenderPath(BufferInfo& dst, uintptr_t path, const Rect& mask,
        VGUStrokeAttr* strokeAttr, VGUFillAttr* fillAttr, VGUMatrix3* matrix);

    void ClipImgByPath(ImageInfo& dstInfo, ImageInfo& srcInfo, uintptr_t path, Rect* srcImgArea,
        const Matrix4<float>* pathMatrix, const Matrix4<float>* imgMatrix, bool isAlphaFF = false);

    bool Convert2VGUFormat(const ColorMode& colorMode, VGUPixelFormat& dstColorFmt) const;
    void AddHardwareLayer(IHardwareView* view)
    {
        hardwareView_ = view;
    }

    IHardwareView* GetHardwareLayer()
    {
        return hardwareView_;
    }

    void ClearHardwareLayer(IHardwareView* view)
    {
        if (hardwareView_ == view) {
            hardwareView_ = nullptr;
        }
    }

    bool HasPendingFrame()
    {
        return lastAsyncInfo_.hasPendingFrame;
    }

#endif

private:
    LiteMGfxEngine() {}
    virtual ~LiteMGfxEngine()
    {
        if (bufferInfo_ != nullptr) {
            UIFree(bufferInfo_);
            bufferInfo_ = nullptr;
        }
        if (nextBufferInfo_ != nullptr) {
            UIFree(nextBufferInfo_);
            nextBufferInfo_ = nullptr;
        }
        ReleasePath();
    }

#if defined(HW_ROTATE_ANGLE)
    bool RotateBufferIfNecessary();
#endif
    void DrawRectWithRound(BufferInfo& dst, const Rect& rect, const Style& style, const Rect& dirtyRect,
        OpacityType opacity, VGUMatrix3* matrix = nullptr);
    void DrawRectWithoutRound(BufferInfo& dst, const Rect& rect, const Style& style, const Rect& dirtyRect,
        OpacityType opacity, VGUMatrix3* matrix = nullptr);
    bool HandleImageSrc(const Image* imgSrc, VGUImage& image, const Style& style);
    void ReleasePath();
    void DrawArcInner(BufferInfo& dst, const ArcInfo& arcInfo, const Rect& mask,
        const Style& style, OpacityType opacity, uint8_t cap, VGUImage& image);

    BufferInfo* bufferInfo_ = nullptr;
    BufferInfo* nextBufferInfo_ = nullptr;
    Graphic::Vector<Path> pathSubmit_{INIT_PATH_NUM};
    Graphic::Vector<Path> pathRender_{INIT_PATH_NUM};

    AsyncSubmitInfo lastAsyncInfo_ = {{0, 0, 1, 1}, false, false};
    bool enableAsyncSubmit_ = true;

#if ENABLE_VGU_ENGINE
    VGUFuncs* vguFuncs_ = nullptr;
    uint8_t blitNum_ = 0;
    VGUImage images_[BLIT_COUNT_MAX] = {{{PIXEL_FMT_BUTT}}};
    VGUSurface surface_ = {{PIXEL_FMT_BUTT}};
    bool isHwDrawPending_ = false;
    uint16_t hwDrawPendingCnt_ = 0;
    uint16_t hwStrokePendingCnt_ = 0;
    IHardwareView* hardwareView_ {nullptr};

    bool Convert2VGUBuffer(const BufferInfo& bufferInfo, VGUBuffer& buffer) const;
    void BlitImages(bool calledBySync = false);
    void CheckHwDrawLimit(uint16_t nodeCnt, uint16_t strokeCnt = 0);
    void AddHwDraw(uint16_t nodeCnt, uint16_t strokeCnt = 0);
    void AsyncFlush();
    void RenderSubmitInner(bool isNeedWait = true);
#endif
};
}
#endif // GRAPHIC_LITE_LITE_M_GFX_ENGINE_H