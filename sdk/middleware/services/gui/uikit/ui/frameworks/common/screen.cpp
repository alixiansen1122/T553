/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "common/screen.h"
#include "core/render_manager.h"
#include "draw/draw_utils.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "gfx_utils/mem_api.h"
#include "imgdecode/image_load.h"
#include "hals/display_dev.h"
#include "securec.h"

namespace OHOS {
Screen& Screen::GetInstance()
{
    static Screen instance;
    return instance;
}

uint16_t Screen::GetWidth()
{
    return BaseGfxEngine::GetInstance()->GetScreenWidth();
}

uint16_t Screen::GetHeight()
{
    return BaseGfxEngine::GetInstance()->GetScreenHeight();
}

uint16_t Screen::GetRadius()
{
    return BaseGfxEngine::GetInstance()->GetScreenRadius();
}

bool Screen::GetCurrentScreenBitmap(ImageInfo& info)
{
    return GetScreenBitmapByRect(info, nullptr);
}

bool Screen::GetScreenBitmapByRect(ImageInfo& info, const Rect* rect, float scale, bool isClearMem)
{
    BufferInfo* fbInfo = BaseGfxEngine::GetInstance()->GetFBBufferInfo();
    if (fbInfo == nullptr) {
        GRAPHIC_LOGE("GetFBBufferInfo failed.");
        return false;
    }
    return GetBitMapFromBuffer(*fbInfo, info, rect, scale, isClearMem);
}

ScreenShape Screen::GetScreenShape()
{
    return BaseGfxEngine::GetInstance()->GetScreenShape();
}

bool Screen::GetBitMapFromBuffer(const BufferInfo &srcBuf, ImageInfo& info, const Rect *rect, float scale, bool isClearMem)
{
    BufferInfo srcInfo = srcBuf;
    Rect mask;
    if (rect == nullptr) {
        mask = srcInfo.rect;
    } else {
        mask = *rect;
        if (!mask.Intersect(mask, srcInfo.rect)) {
            GRAPHIC_LOGE("invalid input rect [%d, %d, %d, %d].",
                rect->GetLeft(), rect->GetTop(), rect->GetRight(), rect->GetBottom());
            return false;
        }
    }
    srcInfo.rect = mask;

    info.header.width = mask.GetWidth() * scale;
    info.header.height = mask.GetHeight() * scale;
    info.header.compressMode = COMPRESS_MODE_NONE;

#ifdef VERSION_IOT
#ifdef _WIN32
    info.header.colorMode = ARGB8888;
    info.dataSize = ALIGN_BYTE(info.header.width, BYTE_ALIGNMENT) * info.header.height * 4; // 4: bpp
#else
    info.header.colorMode = RGB888;
    uint32_t stride = DisplayDev::GetInstance()->CalcStride(info.header.width,
        PIXEL_FMT_RGB_888, info.header.compressMode);
    info.dataSize = DisplayDev::GetInstance()->CalcSize(stride, info.header.height, info.header.compressMode);
#endif
#else
    info.header.colorMode = ARGB8888;
    info.dataSize = info.header.width *  info.header.height * 4; // 4: bpp
#endif

    Rect dstRect = {0, 0, info.header.width - 1, info.header.height - 1};
    BlendOption opt;
    opt.opacity = OPA_OPAQUE;
    opt.mode = BLEND_SRC_OVER;
    opt.transMap.SetTransMapRect(srcInfo.rect);
    opt.transMap.Scale(Vector2<float>(scale, scale), Vector2<float>(0.0, 0.0));
    opt.transMap.Translate(Vector2<int16_t>(-srcInfo.rect.GetX(), -srcInfo.rect.GetY()));
    if (!dstRect.Intersect(dstRect, opt.transMap.GetBoxRect())) {
        return false;
    }
    if (ImageCacheMalloc(info) == nullptr) {
        return false;
    }

    BufferInfo dstInfo;
    memset_s(&dstInfo, sizeof(BufferInfo), 0, sizeof(BufferInfo));
    dstInfo.width = info.header.width;
    dstInfo.height = info.header.height;
#ifdef VERSION_IOT
#ifdef _WIN32
    dstInfo.mode = ARGB8888;
    dstInfo.stride = ALIGN_BYTE(info.header.width, BYTE_ALIGNMENT) * 4; // 4: bpp
#else
    dstInfo.mode = RGB888;
    dstInfo.compressMode = COMPRESS_MODE_NONE;
    dstInfo.stride = DisplayDev::GetInstance()->CalcStride(dstInfo.width,
        PIXEL_FMT_RGB_888, dstInfo.compressMode);
#endif
#else
    dstInfo.mode = ARGB8888;
    dstInfo.stride = info.header.width * 4; // 4: bpp
#endif
    dstInfo.phyAddr = (void*)info.phyAddr;
    dstInfo.virAddr = (void*)info.data;
    dstInfo.rect = dstRect;

    if (isClearMem) {
        int fillHeight = info.header.height;
        int fileWeight = info.header.width;
#ifdef VERSION_IOT
        fileWeight = ALIGN_BYTE(info.header.width, BYTE_ALIGNMENT);
#endif
        Rect fillArea = {0, 0, fileWeight - 1, fillHeight - 1}; // 4: bpp
        BaseGfxEngine::GetInstance()->Fill(dstInfo, fillArea, Color::Black(), OPA_OPAQUE);
    }

    BaseGfxEngine::GetInstance()->Blit(dstInfo, {0, 0}, srcInfo, dstRect, opt);
#if ENABLE_VGU_ENGINE
    LiteMGfxEngine::GetInstance()->SyncHwDraw();
#endif
    return true;
}
} // namespace OHOS
