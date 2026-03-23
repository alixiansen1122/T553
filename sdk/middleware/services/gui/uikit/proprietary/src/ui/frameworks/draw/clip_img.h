/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: ClipImg
 * Author:
 * Create: 2023-10
 */

#ifndef GRAPHIC_CLIP_IMG_H
#define GRAPHIC_CLIP_IMG_H

#include "gfx_utils/heap_base.h"
#include "gfx_utils/graphic_math.h"
#include "gfx_utils/image_info.h"
#include "gfx_utils/graphic_types.h"
#include "gfx_utils/rect.h"
#include "components/ui_canvas_ext.h"

namespace OHOS {
class ClipImg : public HeapBase {
public:
    static void ClipImgByPath(ImageInfo& dstInfo, ImageInfo& srcInfo, const Path* path, Rect* srcImgArea,
        const Matrix4<float>* pathMatrix, const Matrix4<float>* imgMatrix, bool isAlphaFF = false);
    static ImageInfo ClipImgByCircle(ImageInfo& srcInfo, Point center, int16_t radius,
        ColorMode mode = RGB888, bool immediatelySync = false);
    static ImageInfo ClipImgByRect(ImageInfo& srcInfo, Rect* srcImgArea, Rect clipArea, ColorMode mode = RGB888);
    static ImageInfo ClipImgByRoundRect(ImageInfo& srcInfo, Rect clipArea, uint16_t radius,
        ColorMode mode = RGB888, bool immediatelySync = false);
};
} // namespace OHOS
#endif // GRAPHIC_LITE_DRAW_LABEL_H
