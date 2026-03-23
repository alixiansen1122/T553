/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: ClipImg
 * Author:
 * Create: 2023-10
 */

#include <securec.h>
#include "hals/gralloc_engines.h"
#include "draw/hw_draw_utils.h"
#include "draw/draw_utils.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "clip_img.h"

namespace OHOS {
static bool AllocImageInfo(ImageInfo& info, uint16_t width, uint16_t height, ColorMode mod)
{
    info.header.width = width;
    info.header.height = height;
    info.header.colorMode = mod;
    info.dataSize = ALIGN_BYTE(width, BYTE_ALIGNMENT) * height * DrawUtils::GetByteSizeByColorMode(mod);
    info.header.compressMode = 0;
    if (ImageCacheMalloc(info) == nullptr) {
        return false;
    }
    return true;
}

void ClipImg::ClipImgByPath(ImageInfo& dstInfo, ImageInfo& srcInfo, const Path* path, Rect* srcImgArea,
    const Matrix4<float>* pathMatrix, const Matrix4<float>* imgMatrix, bool isAlphaFF)
{
    if (path == nullptr || path->cmds == nullptr || path->data == nullptr) {
        return;
    }

    uintptr_t handle = LiteMGfxEngine::GetInstance()->InitPath(true);
    LiteMGfxEngine::GetInstance()->SetPathData(handle, path->cmdNum, path->cmds, path->dataNum, path->data);
    if (isAlphaFF) {
        LiteMGfxEngine::GetInstance()->ClipImgByPath(dstInfo, srcInfo, handle, srcImgArea, pathMatrix, imgMatrix, true);
    } else {
        LiteMGfxEngine::GetInstance()->ClipImgByPath(dstInfo, srcInfo, handle, srcImgArea, pathMatrix, imgMatrix);
    }
}

ImageInfo ClipImg::ClipImgByCircle(ImageInfo& srcInfo, Point center, int16_t radius,
    ColorMode mode, bool immediatelySync)
{
    ImageInfo dstInfo = {{0}};
    Path path = {0};
    ArcInfo arcInfo = {center, {0, 0}, radius, 0, 0, nullptr};
    if (!HWDrawUtils::GetInstance()->ObtainCirclePath(arcInfo, path.cmdNum, path.cmds, path.dataNum, path.data)) {
        return dstInfo;
    }

    float scale = 1.0;
    if (srcInfo.userData != nullptr) {
        scale = *(static_cast<float*>(srcInfo.userData));
    }
    if (!AllocImageInfo(dstInfo, 2 * radius * scale, 2 * radius * scale, mode)) { // 2: the diameter of the circle
        HWDrawUtils::GetInstance()->FreeBufferForPath(&path);
        return dstInfo;
    }

    Matrix4<float> pathMat;
    Matrix4<float> imgMat;
    if (!FloatEqual(scale, 1.0)) {
        pathMat[0][0] = scale;
        pathMat[1][1] = scale;
        imgMat[0][0] = scale;
        imgMat[1][1] = scale;
    }
    ClipImgByPath(dstInfo, srcInfo, &path, nullptr, &pathMat, &imgMat);
    if (immediatelySync) {
        LiteMGfxEngine::GetInstance()->SyncHwDraw();
    }
    HWDrawUtils::GetInstance()->FreeBufferForPath(&path);
    return dstInfo;
}

ImageInfo ClipImg::ClipImgByRect(ImageInfo& srcInfo, Rect* srcImgArea, Rect clipArea, ColorMode mode)
{
    ImageInfo dstInfo = {{0}};
    Path path = {0};

    if (!HWDrawUtils::GetInstance()->AllocBufferForPath(&path, 5, 5)) { // 5: cmd and data num
        return dstInfo;
    }

    path.data[0] = clipArea.GetX();
    path.data[1] = clipArea.GetY();
    path.data[2] = clipArea.GetRight();  // 2: index
    path.data[3] = clipArea.GetBottom(); // 3: index
    path.data[4] = clipArea.GetX();      // 4: index

    path.cmds[0] = CMD_MOVE_TO;
    path.cmds[1] = CMD_HLINE_TO;
    path.cmds[2] = CMD_VLINE_TO; // 2: index
    path.cmds[3] = CMD_HLINE_TO; // 3: index
    path.cmds[4] = CMD_CLOSE;    // 4: index

    if (!AllocImageInfo(dstInfo, clipArea.GetWidth(), clipArea.GetHeight(), mode)) {
        HWDrawUtils::GetInstance()->FreeBufferForPath(&path);
        return dstInfo;
    }

    ClipImgByPath(dstInfo, srcInfo, &path, nullptr, nullptr, nullptr);
    HWDrawUtils::GetInstance()->FreeBufferForPath(&path);
    return dstInfo;
}

ImageInfo ClipImg::ClipImgByRoundRect(ImageInfo& srcInfo, Rect clipArea, uint16_t radius,
    ColorMode mode, bool immediatelySync)
{
    ImageInfo dstInfo = {{0}};
    Path path = {0};

    if (!HWDrawUtils::GetInstance()->ObtainRectPathWithRadius(clipArea, path, radius)) { // 5: cmd and data num
        return dstInfo;
    }

    float scale = 1.0;
    if (srcInfo.userData != nullptr) {
        scale = *(static_cast<float*>(srcInfo.userData));
    }
    if (!AllocImageInfo(dstInfo, clipArea.GetWidth() * scale, clipArea.GetHeight() * scale, mode)) {
        HWDrawUtils::GetInstance()->FreeBufferForPath(&path);
        return dstInfo;
    }

    Matrix4<float> pathMat;
    Matrix4<float> imgMat;
    if (!FloatEqual(scale, 1.0)) {
        pathMat[0][0] = scale;
        pathMat[1][1] = scale;
        imgMat[0][0] = scale;
        imgMat[1][1] = scale;
    }
    ClipImgByPath(dstInfo, srcInfo, &path, nullptr, &pathMat, &imgMat);
    if (immediatelySync) {
        LiteMGfxEngine::GetInstance()->SyncHwDraw();
    }
    HWDrawUtils::GetInstance()->FreeBufferForPath(&path);
    return dstInfo;
}
}
