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

#ifndef GRAPHIC_LITE_HW_DRAW_H
#define GRAPHIC_LITE_HW_DRAW_H

#include "graphic_config.h"
#if ENABLE_VGU_ENGINE
#include "engines/gfx/gfx_engine_manager.h"
#include "common/graphic_hardware_types.h"
#include "display_vgu.h"
#include "display_type.h"
#include "gfx_utils/graphic_types.h"

namespace OHOS {
class HWDrawUtils : public HeapBase {
public:
    static HWDrawUtils* GetInstance()
    {
        static HWDrawUtils instance;
        return &instance;
    }
    bool ObtainArcPath(ArcInfo arc, uint32_t& cmdNum, uint8_t*& cmds, uint32_t& dataNum, float*& data);
    bool ObtainSectorPath(ArcInfo arc, uint32_t& cmdNum, uint8_t*& cmds, uint32_t& dataNum, float*& data);
    bool ObtainCirclePath(const ArcInfo& arc, uint32_t& cmdNum, uint8_t*& cmds, uint32_t& dataNum, float*& data);
    bool AllocBufferForCmd(uint16_t cmdNum, uint8_t*& addr);
    bool AllocBufferForData(uint16_t dataNum, float*& addr);
    bool AllocBufferForPath(Path* path, uint16_t cmdNum, uint16_t dataNum);
    void FreeBufferForPath(Path* path);
    void Obtain30DegSectorPath(const ArcInfo& arc, float x, float y, uint8_t* cmd, float* data);
    bool ObtainRectPathWithRadius(const Rect& rect, Path& path, int16_t topRadius, int16_t bottomRadius);
    bool ObtainRectPathWithRadius(const Rect& rect, Path& path, int16_t radius);
    bool ObtainEllipsePath(const Point& center, uint16_t rx, uint16_t ry, Path& path);
    void Matrix3ToVguMatrix3(const Matrix3<float>& matrix, VGUMatrix3& vguMat);
    void Matrix4ToVguMatrix3(const Matrix4<float>& matrix, VGUMatrix3& vguMat);
    void ImageInfo2BufferInfo(const ImageInfo &imageInfo, BufferInfo &bufferInfo);
    void BufferInfo2ImageInfo(const BufferInfo &bufferInfo, ImageInfo &imageInfo);
    static ColorMode ConvertPixelFmt2ColorMode(PixelFormat fmt);
    static PixelFormat ConvertColorMode2PixelFmt(ColorMode mode);

private:
    HWDrawUtils() {}
    virtual ~HWDrawUtils() {}
    void GetArcData(float* arcData, const ArcInfo& arc, uint32_t quaNum, float bezMore);
    void GetArcControlPoint(float* arcData, float centerX, float centerY, float mangicNum, int8_t neg);
    void GetFilledRoundRectData(float* data, uint16_t radius, const Rect& rect);
    void GetFilledRoundRectData(float* data, uint16_t topRadius, uint16_t bottomRadius, const Rect& rect);
};
}
#endif
#endif
