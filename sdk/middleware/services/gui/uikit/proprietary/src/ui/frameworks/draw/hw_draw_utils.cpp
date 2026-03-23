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

#include "draw/hw_draw_utils.h"
#include <cmath>
#include "components/ui_canvas_ext.h"
#include "gfx_utils/graphic_math.h"
#include "gfx_utils/graphic_log.h"
#include "hals/gralloc_engines.h"
#include "imgdecode/image_load.h"
#include "hals/display_dev.h"

#if ENABLE_VGU_ENGINE
namespace OHOS {
constexpr float ARC_TO_BEZIER_90_DEG = 0.551915024494f;
constexpr int16_t DATA_NUM_PER_BEZ = 6;
constexpr float PRECISION_ANGLE = 0.01f;

bool HWDrawUtils::AllocBufferForCmd(uint16_t cmdNum, uint8_t*& addr)
{
    uint8_t* cmdAddr = GrallocEngines::GetInstance()->AllocBufferBySize(sizeof(uint8_t) * cmdNum,
        HBM_USE_ASSIGN_SIZE | HBM_USE_MEM_PATH);
    if (cmdAddr == nullptr) {
        GRAPHIC_LOGE("Alloc buffer for cmd failed.");
        addr = nullptr;
        return false;
    }
    addr = cmdAddr;
    return true;
}

bool HWDrawUtils::AllocBufferForData(uint16_t dataNum, float*& addr)
{
    uint8_t* dataAddr = GrallocEngines::GetInstance()->AllocBufferBySize(sizeof(float) * dataNum,
        HBM_USE_ASSIGN_SIZE | HBM_USE_MEM_PATH);
    if (dataAddr == nullptr) {
        GRAPHIC_LOGE("Alloc buffer for data failed.");
        addr = nullptr;
        return false;
    }
    addr = reinterpret_cast<float*>(dataAddr);
    return true;
}

bool HWDrawUtils::AllocBufferForPath(Path* path, uint16_t cmdNum, uint16_t dataNum)
{
    if (path == nullptr) {
        return false;
    }

    if (!AllocBufferForCmd(cmdNum, path->cmds)) {
        return false;
    }

    if (!AllocBufferForData(dataNum, path->data)) {
        GrallocEngines::GetInstance()->FreeBuffer(path->cmds);
        path->cmds = nullptr;
        return false;
    }

    path->cmdNum = cmdNum;
    path->dataNum = dataNum;
    return true;
}

void HWDrawUtils::FreeBufferForPath(Path* path)
{
    if (path == nullptr) {
        return;
    }
    GrallocEngines::GetInstance()->FreeBuffer(path->cmds);
    GrallocEngines::GetInstance()->FreeBuffer(reinterpret_cast<uint8_t*>(path->data));
    path->cmds = nullptr;
    path->data = nullptr;
    path->cmdNum = 0;
    path->dataNum = 0;
}

void HWDrawUtils::GetArcControlPoint(float* arcData, float centerX, float centerY, float mangicNum, int8_t neg)
{
    arcData[2] = arcData[0] - (neg * mangicNum * (arcData[1] - centerY)); // 2: index
    arcData[3] = arcData[1] + (neg * mangicNum * (arcData[0] - centerX)); // 3: index
    arcData[4] = arcData[6] + (neg * mangicNum * (arcData[7] - centerY)); // 4, 6, 7: index
    arcData[5] = arcData[7] - (neg * mangicNum * (arcData[6] - centerX)); // 5, 7, 6: index
}

static float ConvertArcDegree(ArcInfo& arc, uint32_t& quaNum, uint32_t& bezNum)
{
    // convert angle in the interface to gpu angle
    arc.startAngle = arc.startAngle - QUARTER_IN_DEGREE;
    arc.endAngle = arc.endAngle - QUARTER_IN_DEGREE;
    float deltaAngle = abs(arc.endAngle - arc.startAngle);
    if (FloatMore(deltaAngle, CIRCLE_IN_DEGREE, PRECISION_ANGLE)) {
        arc.startAngle = 0.0f;
        arc.endAngle = CIRCLE_IN_DEGREE;
        quaNum = 4; // 4: num of quaerter in circle
        bezNum = 4; // 4: num of quaerter in circle
        return 0.0f;
    }
    uint32_t deltaAngleInt = static_cast<uint32_t>(deltaAngle);
    if (FloatEqual(deltaAngle - deltaAngleInt, 1.0f, PRECISION_ANGLE)) {
        deltaAngleInt += 1;
    }
    quaNum = deltaAngleInt / QUARTER_IN_DEGREE;
    float remain = deltaAngle - quaNum * QUARTER_IN_DEGREE;
    bezNum = FloatMore(remain, 0.0f, PRECISION_ANGLE) ? quaNum + 1: quaNum;
    return remain;
}

void HWDrawUtils::GetArcData(float* arcData, const ArcInfo& arc, uint32_t quaNum, float remain)
{
    Point center = arc.center;
    int16_t radius = arc.radius;
    int neg = (arc.startAngle < arc.endAngle) ? 1 : -1;

    arcData[0] = center.x + radius * Sin(QUARTER_IN_DEGREE - arc.startAngle);
    arcData[1] = center.y + radius * Sin(arc.startAngle);
    float angle = arc.startAngle;
    for (uint32_t i = 0; i < quaNum; ++i) {
        angle += (neg * QUARTER_IN_DEGREE); // when neg == 1, then clockwise; when neg == -1, counter clockwise
        arcData[DATA_NUM_PER_BEZ * i + 6] = center.x + radius * Sin(QUARTER_IN_DEGREE - angle);  // 6: offset
        arcData[DATA_NUM_PER_BEZ * i + 7] = center.y + radius * Sin(angle);  // 7: offset
        GetArcControlPoint(&arcData[DATA_NUM_PER_BEZ * i], center.x, center.y, ARC_TO_BEZIER_90_DEG, neg);
    }

    if (FloatMore(remain, 0.0f, PRECISION_ANGLE)) {
        arcData[DATA_NUM_PER_BEZ * quaNum + 6] = center.x + radius * Sin(QUARTER_IN_DEGREE - arc.endAngle); // 6: offset
        arcData[DATA_NUM_PER_BEZ * quaNum + 7] = center.y + radius * Sin(arc.endAngle); // 7: offset
        // 4.0: multiplier, 3.0: divider, 2: divider
        float magicNum = 4.0f / 3.0f * (1 - Sin(QUARTER_IN_DEGREE - remain / 2)) / Sin(remain / 2);
        GetArcControlPoint(&arcData[DATA_NUM_PER_BEZ * quaNum], center.x, center.y, magicNum, neg);
    }
}

void HWDrawUtils::Obtain30DegSectorPath(const ArcInfo& arc, float x, float y, uint8_t* cmd, float* data)
{
    // convert angle in the interface to gpu angle
    float startAngle = (arc.startAngle - QUARTER_IN_DEGREE) / RADIAN_TO_ANGLE;
    // make sure the range is accurate, not use (arc.endAngle - QUARTER_IN_DEGREE) / RADIAN_TO_ANGLE
    float endAngle = startAngle + (arc.endAngle - arc.startAngle) / RADIAN_TO_ANGLE;
    float neg = (startAngle < endAngle) ? 1 : -1;

    cmd[0] = CMD_MOVE_TO;
    cmd[1] = CMD_LINE_TO;
    cmd[2] = CMD_CUBIC_TO; // 2: index
    cmd[3] = CMD_CLOSE; // 3: index

    float a = (endAngle - startAngle) * neg;
    data[0] = x;
    data[1] = y;
    data[2] = x + arc.radius * cos(startAngle); // 2: index
    data[3] = y + arc.radius * sin(startAngle); // 3: index
    data[8] = x + arc.radius * cos(endAngle); // 8: index
    data[9] = y + arc.radius * sin(endAngle); // 9: index
    float magicNum = 4.0f / 3.0f * (1 - cos(a / 2)) / sin(a / 2); // 2: divisor
    GetArcControlPoint(&data[2], x, y, magicNum, neg); // 2: index
}

bool HWDrawUtils::ObtainArcPath(ArcInfo arc, uint32_t& cmdNum, uint8_t*& cmds, uint32_t& dataNum, float*& data)
{
    uint32_t quaNum = 0;
    uint32_t bezNum = 0;
    float remain = ConvertArcDegree(arc, quaNum, bezNum);

    cmdNum = bezNum + 1;
    dataNum = bezNum * DATA_NUM_PER_BEZ + 2; // 2: the number of start point path data
    if (!AllocBufferForCmd(cmdNum, cmds)) {
        return false;
    }
    if (!AllocBufferForData(dataNum, data)) {
        GrallocEngines::GetInstance()->FreeBuffer(cmds);
        cmds = nullptr;
        return false;
    }

    GetArcData(data, arc, quaNum, remain);
    cmds[0] = CMD_MOVE_TO;
    for (uint32_t i = 1; i < cmdNum; i++) {
        cmds[i] = CMD_CUBIC_TO;
    }
    return true;
}

bool HWDrawUtils::ObtainSectorPath(ArcInfo arc, uint32_t& cmdNum, uint8_t*& cmds,
    uint32_t& dataNum, float*& data)
{
    uint32_t quaNum = 0;
    uint32_t bezNum = 0;
    float remain = ConvertArcDegree(arc, quaNum, bezNum);

    cmdNum = bezNum + 3; // 3: num of cmds, which are assigned in the below code
    dataNum = bezNum * DATA_NUM_PER_BEZ + 4; // 4: the number of path data, include center and start point
    if (!AllocBufferForCmd(cmdNum, cmds)) {
        return false;
    }
    if (!AllocBufferForData(dataNum, data)) {
        GrallocEngines::GetInstance()->FreeBuffer(cmds);
        cmds = nullptr;
        return false;
    }
    data[0] = arc.center.x;
    data[1] = arc.center.y;
    GetArcData(data + 2, arc, quaNum, remain); // 2: offset
    cmds[0] = CMD_MOVE_TO;
    cmds[1] = CMD_LINE_TO;
    cmds[cmdNum - 1] = CMD_CLOSE;
    for (uint32_t i = 2; i < cmdNum - 1; i++) {
        cmds[i] = CMD_CUBIC_TO;
    }
    return true;
}

bool HWDrawUtils::ObtainCirclePath(const ArcInfo& arc, uint32_t& cmdNum,
    uint8_t*& cmds, uint32_t& dataNum, float*& data)
{
    float r = arc.radius;
    float h = r * ARC_TO_BEZIER_90_DEG;
    Point center = arc.center;

    cmdNum = 5; // 5: Circle path command number is 5
    if (!AllocBufferForCmd(cmdNum, cmds)) {
        return false;
    }
    dataNum = 26; // Circle path data number is 26
    if (!AllocBufferForData(dataNum, data)) {
        GrallocEngines::GetInstance()->FreeBuffer(cmds);
        cmds = nullptr;
        return false;
    }

    cmds[0] = CMD_MOVE_TO;
    cmds[1] = CMD_CUBIC_TO;
    cmds[2] = CMD_CUBIC_TO; // 2: cmd index
    cmds[3] = CMD_CUBIC_TO; // 3: cmd index
    cmds[4] = CMD_CUBIC_TO; // 4: cmd index

    data[0] = center.x + r;
    data[1] = center.y;
    data[2] = center.x + r; // 2: data index
    data[3] = center.y + h; // 3: data index
    data[4] = center.x + h; // 4: data index
    data[5] = center.y + r; // 5: data index
    data[6] = center.x;     // 6: data index
    data[7] = center.y + r; // 7: data index
    data[8] = center.x - h; // 8: data index
    data[9] = center.y + r; // 9: data index
    data[10] = center.x - r; // 10: data index
    data[11] = center.y + h; // 11: data index
    data[12] = center.x - r; // 12: data index
    data[13] = center.y;     // 13: data index
    data[14] = center.x - r; // 14: data index
    data[15] = center.y - h; // 15: data index
    data[16] = center.x - h; // 16: data index
    data[17] = center.y - r; // 17: data index
    data[18] = center.x;     // 18: data index
    data[19] = center.y - r; // 19: data index
    data[20] = center.x + h; // 20: data index
    data[21] = center.y - r; // 21: data index
    data[22] = center.x + r; // 22: data index
    data[23] = center.y - h; // 23: data index
    data[24] = center.x + r; // 24: data index
    data[25] = center.y;     // 25: data index
    return true;
}

bool HWDrawUtils::ObtainRectPathWithRadius(const Rect& rect, Path& path, int16_t topRadius, int16_t bottomRadius)
{
    uint32_t cmdNum = 10; // 10: Circle path command number is 10
    if (path.cmdNum < cmdNum) {
        GrallocEngines::GetInstance()->FreeBuffer(path.cmds);
        path.cmds = nullptr;
        if (!AllocBufferForCmd(cmdNum, path.cmds)) {
            return false;
        }
    }
    path.cmdNum = cmdNum;

    path.cmds[0] = CMD_MOVE_TO;
    path.cmds[1] = CMD_LINE_TO;
    path.cmds[2] = CMD_CUBIC_TO;  // 2: cmd index
    path.cmds[3] = CMD_LINE_TO; // 3: cmd index
    path.cmds[4] = CMD_CUBIC_TO;  // 4: cmd index
    path.cmds[5] = CMD_LINE_TO; // 5: cmd index
    path.cmds[6] = CMD_CUBIC_TO;  // 6: cmd index
    path.cmds[7] = CMD_LINE_TO; // 7: cmd index
    path.cmds[8] = CMD_CUBIC_TO;    // 8: cmd index
    path.cmds[9] = CMD_CLOSE;    // 9: cmd index

    uint32_t dataNum = 34; // Circle path data number is 34
    if (path.dataNum < dataNum) {
        GrallocEngines::GetInstance()->FreeBuffer(reinterpret_cast<uint8_t*>(path.data));
        path.data = nullptr;
        if (!AllocBufferForData(dataNum, path.data)) {
            GrallocEngines::GetInstance()->FreeBuffer(path.cmds);
            path.cmds = nullptr;
            return false;
        }
    }
    path.dataNum = dataNum;

    GetFilledRoundRectData(path.data, topRadius, bottomRadius, rect);
    return true;
}

bool HWDrawUtils::ObtainRectPathWithRadius(const Rect& rect, Path& path, int16_t radius)
{
    return ObtainRectPathWithRadius(rect, path, radius, radius);
}

void HWDrawUtils::GetFilledRoundRectData(float* data, uint16_t radius, const Rect& rect)
{
    GetFilledRoundRectData(data, radius, radius, rect);
}

void HWDrawUtils::GetFilledRoundRectData(float* data, uint16_t topRadius, uint16_t bottomRadius, const Rect& rect)
{
    float tr = topRadius;
    float br = bottomRadius;
    int16_t x1 = rect.GetLeft();
    int16_t y1 = rect.GetTop();
    int16_t x2 = rect.GetRight();
    int16_t y2 = rect.GetBottom();

    data[0] = x1;
    data[1] = y1 + tr;
    data[2] = x1; // 2: index
    data[3] = y2 - br; // 3: index

    // 1 arc end point
    data[8] = x1 + br; // 8: index
    data[9] = y2 + 1; // 9: index
    data[10] = x2 - br; // 10: index
    data[11] = y2 + 1; // 11: index

    // 2 arc end point
    data[16] = x2 + 1; // 16: index
    data[17] = y2 - br; // 17: index
    data[18] = x2 + 1; // 18: index
    data[19] = y1 + tr; // 19: index

    // 3 arc end point
    data[24] = x2 - tr; // 24: index
    data[25] = y1; // 25: index
    data[26] = x1 + tr; // 26: index
    data[27] = y1; // 27: index

    // 4 arc end point
    data[32] = x1; // 32: index
    data[33] = y1 + tr; // 33: index

    GetArcControlPoint(&data[2], x1 + br, y2 - br, ARC_TO_BEZIER_90_DEG, -1); // 2: index

    GetArcControlPoint(&data[10], x2 - br, y2 - br, ARC_TO_BEZIER_90_DEG, -1); // 10: index

    GetArcControlPoint(&data[18], x2 - tr, y1 + tr, ARC_TO_BEZIER_90_DEG, -1); // 18: index

    GetArcControlPoint(&data[26], x1 + tr, y1 + tr, ARC_TO_BEZIER_90_DEG, -1); // 26: index
}

static void GetEllipsePathData(const Point& center, uint16_t rx, uint16_t ry, Path& path)
{
    path.cmds[0] = CMD_MOVE_TO;
    path.cmds[1] = CMD_CUBIC_TO;
    path.cmds[2] = CMD_CUBIC_TO;  // 2: cmd index
    path.cmds[3] = CMD_CUBIC_TO;  // 3: cmd index
    path.cmds[4] = CMD_CUBIC_TO;  // 4: cmd index
    path.cmds[5] = CMD_CLOSE;     // 5: cmd index

    float magicX = rx * ARC_TO_BEZIER_90_DEG;
    float magicY = ry * ARC_TO_BEZIER_90_DEG;

    // start point
    path.data[0] = center.x;
    path.data[1] = center.y - ry;

    // right top control point
    path.data[2] = center.x + magicX;  // 2: index
    path.data[3] = center.y - ry;      // 3: index
    path.data[4] = center.x + rx;      // 4: index
    path.data[5] = center.y - magicY;  // 5: index
    path.data[6] = center.x + rx;      // 6: index
    path.data[7] = center.y;           // 7: index

    // right bottom control point
    path.data[8] = center.x + rx;       // 8: index
    path.data[9] = center.y + magicY;   // 9: index
    path.data[10] = center.x + magicX;  // 10: index
    path.data[11] = center.y + ry;      // 11: index
    path.data[12] = center.x;           // 12: index
    path.data[13] = center.y + ry;      // 13: index

    // left bottom control point
    path.data[14] = center.x - magicX;  // 14: index
    path.data[15] = center.y + ry;      // 15: index
    path.data[16] = center.x - rx;      // 16: index
    path.data[17] = center.y + magicY;  // 17: index
    path.data[18] = center.x - rx;      // 18: index
    path.data[19] = center.y;           // 19: index

    // left top control point
    path.data[20] = center.x - rx;       // 20: index
    path.data[21] = center.y - magicY;   // 21: index
    path.data[22] = center.x - magicX;   // 22: index
    path.data[23] = center.y - ry;       // 23: index
    path.data[24] = center.x;            // 24: index
    path.data[25] = center.y - ry;       // 25: index
}

bool HWDrawUtils::ObtainEllipsePath(const Point& center, uint16_t rx, uint16_t ry, Path& path)
{
    uint32_t cmdNum = 6; // 6: Ellipse path command number is 6
    if (path.cmdNum < cmdNum) {
        if (path.cmds != nullptr) {
            GrallocEngines::GetInstance()->FreeBuffer(path.cmds);
            path.cmds = nullptr;
            path.cmdNum = 0;
        }
        if (!AllocBufferForCmd(cmdNum, path.cmds)) {
            return false;
        }
    }
    path.cmdNum = cmdNum;

    uint32_t dataNum = 26; // 26: Ellipse path data number is 26
    if (path.dataNum < dataNum) {
        if (path.data != nullptr) {
            GrallocEngines::GetInstance()->FreeBuffer(reinterpret_cast<uint8_t*>(path.data));
            path.data = nullptr;
            path.dataNum = 0;
        }
        if (!AllocBufferForData(dataNum, path.data)) {
            GrallocEngines::GetInstance()->FreeBuffer(path.cmds);
            path.cmds = nullptr;
            path.cmdNum = 0;
            return false;
        }
    }
    path.dataNum = dataNum;

    GetEllipsePathData(center, rx, ry, path);
    return true;
}

void HWDrawUtils::Matrix3ToVguMatrix3(const Matrix3<float>& matrix, VGUMatrix3& vguMat)
{
    const float* data = matrix.GetData();
    vguMat.m[0][0] = data[0]; // 0, 0: index
    vguMat.m[0][1] = data[3]; // 0, 3: index
    vguMat.m[0][2] = data[6]; // 0, 2, 6: index
    vguMat.m[1][0] = data[1]; // 1, 0: index
    vguMat.m[1][1] = data[4]; // 1, 4: index
    vguMat.m[1][2] = data[7]; // 1, 2, 7: index
    vguMat.m[2][0] = data[2]; // 2, 0: index
    vguMat.m[2][1] = data[5]; // 2, 1, 5: index
    vguMat.m[2][2] = data[8]; // 2, 8: index
}

void HWDrawUtils::Matrix4ToVguMatrix3(const Matrix4<float>& matrix, VGUMatrix3& vguMat)
{
    const float* data = matrix.GetData();
    vguMat.m[0][0] = data[0];
    vguMat.m[1][0] = data[1];
    vguMat.m[2][0] = data[3];  // 2, 3: index
    vguMat.m[0][1] = data[4];  // 4: index
    vguMat.m[1][1] = data[5];  // 5: index
    vguMat.m[2][1] = data[7];  // 2, 7: index
    vguMat.m[0][2] = data[12]; // 2, 12: index
    vguMat.m[1][2] = data[13]; // 2, 13: index
    vguMat.m[2][2] = data[15]; // 2, 15: index
}

ColorMode HWDrawUtils::ConvertPixelFmt2ColorMode(PixelFormat fmt)
{
    switch (fmt) {
        case PIXEL_FMT_RGBA_8888:
            return ARGB8888;
        case PIXEL_FMT_RGB_888:
            return RGB888;
        case PIXEL_FMT_RGB_565:
            return RGB565;
        case PIXEL_FMT_A8:
            return A8;
        case PIXEL_FMT_A1:
            return A1;
        default:
            return UNKNOW;
    }
}

PixelFormat HWDrawUtils::ConvertColorMode2PixelFmt(ColorMode mode)
{
    switch (mode) {
        case ARGB8888:
            return PIXEL_FMT_RGBA_8888;
        case RGB888:
            return PIXEL_FMT_RGB_888;
        case RGB565:
            return PIXEL_FMT_RGB_565;
        case A8:
            return PIXEL_FMT_A8;
        case A1:
            return PIXEL_FMT_A1;
        default:
            return PIXEL_FMT_BUTT;
    }
}

void HWDrawUtils::ImageInfo2BufferInfo(const ImageInfo &imageInfo, BufferInfo &bufferInfo)
{
    bufferInfo.color = 0;
    bufferInfo.rect = {0, 0, imageInfo.header.width - 1, imageInfo.header.height - 1};
    bufferInfo.width = imageInfo.header.width;
    bufferInfo.height = imageInfo.header.height;
    bufferInfo.compressMode = imageInfo.header.compressMode;
    bufferInfo.mode = static_cast<ColorMode>(imageInfo.header.colorMode);
    bufferInfo.stride = static_cast<int32_t>(DisplayDev::GetInstance()->CalcStride(bufferInfo.width,
        ConvertColorMode2PixelFmt(bufferInfo.mode), bufferInfo.compressMode));
    bufferInfo.virAddr = static_cast<void*>(const_cast<uint8_t*>(imageInfo.data));
    bufferInfo.phyAddr = bufferInfo.virAddr;
}

void HWDrawUtils::BufferInfo2ImageInfo(const BufferInfo &bufferInfo, ImageInfo &imageInfo)
{
    memset_s(&imageInfo, sizeof(imageInfo), 0, sizeof(imageInfo));
    imageInfo.header.width = bufferInfo.width;
    imageInfo.header.height = bufferInfo.height;
    imageInfo.header.colorMode = bufferInfo.mode;
    imageInfo.header.compressMode = bufferInfo.compressMode;
    imageInfo.dataSize = bufferInfo.stride * bufferInfo.height;
    imageInfo.data = static_cast<uint8_t*>(bufferInfo.virAddr);
}
}
#endif
