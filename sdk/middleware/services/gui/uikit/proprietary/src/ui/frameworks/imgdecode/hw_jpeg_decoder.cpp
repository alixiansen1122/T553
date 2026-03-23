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

#include <climits>
#include "imgdecode/hw_jpeg_decoder.h"
#if CAPABILITY_HW_JPEG_DECODE
#include "gfx_utils/file.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/mem_api.h"
#include "hals/gralloc_engines.h"
#include "securec.h"

namespace OHOS {
static uint32_t g_fileLen = 0;

RetCode HWJpegDecoder::Open(ImgResDsc& dsc)
{
    char path[PATH_MAX] = {0};
    if (realpath(dsc.path, path) == nullptr) {
        GRAPHIC_LOGE("Invalid path.");
        return RetCode::FAIL;
    }
    FILE* fp = fopen(path, "rb");
    if (fp == nullptr) {
        return RetCode::FAIL;
    }
    dsc.fd = 0;
    dsc.imgInfo.data = nullptr;
    dsc.imgInfo.phyAddr = nullptr;
    dsc.inCache_ = false;

    fseek(fp, 0, SEEK_END);
    g_fileLen = static_cast<uint32_t>(ftell(fp));
    filedata_ = GrallocEngines::GetInstance()->AllocBufferBySize(g_fileLen,
        HBM_USE_ASSIGN_SIZE | HBM_USE_MEM_JPEG_DATA);
    if (filedata_ == nullptr) {
        fclose(fp);
        GRAPHIC_LOGE("Alloc buffer for data failed.");
        return RetCode::FAIL;
    }

    fseek(fp, 0, SEEK_SET);
    if (fread(filedata_, 1, g_fileLen, fp) != g_fileLen) {
        fclose(fp);
        GrallocEngines::GetInstance()->FreeBuffer(filedata_);
        filedata_ = nullptr;
        GRAPHIC_LOGE("Read data failed.");
        return RetCode::FAIL;
    }
    fclose(fp);

    return RetCode::OK;
}

RetCode HWJpegDecoder::CreateDecoder(ImgResDsc& dsc)
{
    if (filedata_ == nullptr) {
        return RetCode::FAIL;
    }

    memset_s(&dinfo_, sizeof(jpeg_decompress_struct), 0, sizeof(jpeg_decompress_struct));
    struct jpeg_source_struct src = {0};

    src.vir_buf = reinterpret_cast<td_char *>(filedata_);
    src.phy_buf = reinterpret_cast<td_u32>(src.vir_buf);
    src.size = g_fileLen;
    dinfo_.scale = JPEG_SCALE_1; // suggest to customize the scaling ratio based on the file name.
    dinfo_.output_yuv420sp = true;

    int ret = uapi_jpeg_decomress_create(&dinfo_, &src, true);
    if (ret != 0) {
        GrallocEngines::GetInstance()->FreeBuffer(filedata_);
        filedata_ = nullptr;
        return RetCode::FAIL;
    }
    return RetCode::OK;
}

RetCode HWJpegDecoder::GetHeader(ImgResDsc& dsc)
{
    if (CreateDecoder(dsc) != RetCode::OK) {
        return RetCode::FAIL;
    }

    dsc.imgInfo.header.width = dinfo_.output_width;
    dsc.imgInfo.header.height = dinfo_.output_height;
    dsc.imgInfo.header.colorMode = RGB888;
    dsc.imgInfo.header.compressMode = 0;
    return RetCode::OK;
}

RetCode HWJpegDecoder::DecodeJPEG(ImgResDsc& dsc)
{
    struct jpeg_dst_struct dst = {0};
    dst.stride = dinfo_.output.stride[0];
    dst.uv_stride = dinfo_.output.stride[1];
    dst.uv_offset = dinfo_.output.size[0];
    dst.size = dinfo_.output.size[0] + dinfo_.output.size[1];

    ImageInfo temp = {{0}};
    temp.dataSize = dst.size;
    dst.phy_buf = reinterpret_cast<uint32_t>(ImageCacheMalloc(temp));
    dst.vir_buf = reinterpret_cast<char*>(dst.phy_buf);
    uapi_jpeg_decompress_start(&dinfo_, &dst);
    ConvertYUVToRGB(dsc, &dst);
    uapi_jpeg_decompress_destroy(&dinfo_);
    GrallocEngines::GetInstance()->FreeBuffer(filedata_);
    filedata_ = nullptr;
    ImageCacheFree(temp);
    dst.phy_buf = 0;
    dst.vir_buf = nullptr;
    dsc.inCache_ = true;
    dsc.fd = -1;
    return RetCode::OK;
}

static uint8_t Clamp(int16_t value, int16_t min, int16_t max)
{
    if (min >= max) {
        GRAPHIC_LOGE("ERROR: min >= max.");
    }
    if (value < min) {
        return 0;
    } else if (value > max) {
        return max;
    } else {
        return value;
    }
}

RetCode HWJpegDecoder::ConvertYUVToRGB(ImgResDsc& dsc, jpeg_dst_struct* dst)
{
    uint8_t* yStart = reinterpret_cast<uint8_t*>(dst->vir_buf);
    uint8_t* uvStart = reinterpret_cast<uint8_t*>(dst->vir_buf) + dst->uv_offset;

    uint16_t imgWidth = dinfo_.output_width;
    uint16_t imgHeight = dinfo_.output_height;
    uint16_t alignedImgWidth = ALIGN_BYTE(imgWidth, BYTE_ALIGNMENT);
    uint32_t rgbSize = alignedImgWidth * imgHeight * 3; // 3: bpp of RGB888
    ImageInfo tempImgInfo = {{0}};
    tempImgInfo.dataSize = rgbSize;
    uint8_t* rgbAddr = reinterpret_cast<uint8_t*>(ImageCacheMalloc(tempImgInfo));
    for (uint16_t i = 0; i < imgHeight; i++) {
        uint8_t* rowStart = rgbAddr + i * alignedImgWidth * 3; // 3: bpp of RGB888
        for (uint16_t j = 0; j < imgWidth; j++) {
            uint8_t y = yStart[i * dst->stride + j];
            int16_t u = uvStart[i / 2 * dst->uv_stride + (j / 2) * 2]; // 2 : divider, multiplier
            int16_t v = uvStart[i / 2 * dst->uv_stride + (j / 2) * 2 + 1]; // 2 : divider, multiplier
            uint8_t* temp = rowStart + j * 3; // 3: bpp of RGB888
            // 1.40200: v coefficient, 128: minus 128
            temp[0] = Clamp(y + (1.40200f * (v - 128)), 0, 0xff);
            // 0.714136286: v coefficient, 128: minus 128, 0.344136286: u coefficient
            temp[1] = Clamp(y - (0.714136286f * (v - 128)) - (0.344136286f * (u - 128)), 0, 0xff);
            // 1.772: u coefficient, 128: minus 128
            temp[2] = Clamp(y + (1.772f * (u - 128)), 0, 0xff);
        }
    }
    dsc.imgInfo.data = rgbAddr;
    dsc.imgInfo.phyAddr = rgbAddr;
    dsc.imgInfo.dataSize = rgbSize;
    return RetCode::OK;
}

RetCode HWJpegDecoder::ReadToCache(ImgResDsc& dsc)
{
    if (dsc.inCache_) {
        return RetCode::OK;
    }

    DecodeJPEG(dsc);
    return RetCode::OK;
}
} // namespace OHOS
#endif
