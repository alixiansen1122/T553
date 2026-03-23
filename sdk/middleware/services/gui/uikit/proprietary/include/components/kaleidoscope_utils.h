/*
 * Copyright (c) @CompanyNameMagicTag. 2024. All rights reserved.
 */

/**
 * @file kaleidoscope_utils.h
 *
 * @brief Declares a KaleidoscopeUtils.
 */

#ifndef KALEIDOSCOPE_UTILS_H
#define KALEIDOSCOPE_UTILS_H

#include "gfx_utils/heap_base.h"
#include "components/ui_canvas_ext.h"
#include "gfx_utils/image_info.h"

namespace OHOS {

struct ImageInfoExt {
    ImageInfo info;
    uint32_t stride;
};

class KaleidoscopeUtils : public HeapBase {
public:

    KaleidoscopeUtils() {};

    virtual ~KaleidoscopeUtils();

    /**
    * @brief  Init KaleidoscopeUtils.
    * @param  [in]  src    Pointer to a ImageInfo struct, which is the source img for kaleidoscope img.
    * @param  [in]  dst    Pointer to a ImageInfoExt struct, which will be used to store kaleidoscope data.
                           If it is NULL, it will be created and managed internally.
    * @return Return true if init successfully; and false otherwise.
    */
    bool Init(const ImageInfo* src, const ImageInfoExt* dst);

    /**
    * @brief  Update the kaleidoscope img by a rotate value.
    * @param  [in]  rotate    Rotate value.
    * @return Return true if update successfully; and false otherwise.
    */
    bool Rotate(float rotate);

    /**
    * @brief  Update the address of kaleidoscope data, when init with an externally managed ImageInfoExt.
    * @param  [in]  addr    Address of kaleidoscope data.
    * @return Return true if update successfully; and false otherwise.
    */
    bool UpdateKaleidoscopeImgInfo(const ImageInfoExt &imgInfo);

    /**
    * @brief  Obtain kaleidoscope img.
    * @return Return pointer to ImageInfoExt struct.
    */
    ImageInfoExt* GetKaleidoscopeInfo();

    /**
    * @brief  Get pointer to source ImageInfo struct.
    * @return Return pointer to source ImageInfo struct.
    */
    const ImageInfo* GetSrcImgInfo();

private:
    ImageInfo* imgInfo_ = nullptr;
    ImageInfoExt* kaleidoscopeInfo_ = nullptr;
    ImageInfoExt* partialInfo_ = nullptr;
    Path sectorPath_ = {0};
    Path imgPath_ = {0};
    float thetaVal_ = 0.0f;
    float radius_ = 0.0f;
    float x_ = 0.0f;
    float y_ = 0.0f;
    Point startPoint_ = {0, 0};
    bool isInternalMem_ = false;

    void UpdatePoint(float rotate);
    bool ConstructPartial();
    bool ConstructKaleidoscope();
    Rect GetBoundingArea(const ImageHeader& header);
    bool InitKaleidoscopeInfo(const ImageInfo* src, const ImageInfoExt* dst);
    bool InitInnerPath();
    bool InitPartialInfo();
    void DeinitKaleidoscopeInfo();
    void DeInitPartialInfo();
    void DeinitInnerPath();
    void DeinitImgInfo();
    bool ConstructFirstQuadrant(BufferInfo& dstBufInfo);
    bool ConstructRestQuadrant(BufferInfo& dstBufInfo);
};
}
#endif
/**
 * @}
 */
