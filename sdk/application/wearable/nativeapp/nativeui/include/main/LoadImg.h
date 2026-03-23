/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: LoadImg
 * Create: 2025-06-05
 */

#ifndef LOAD_IMG_H
#define LOAD_IMG_H

#include <string>
#include "wearable_log.h"
#include "components/ui_button.h"
#include "components/ui_image_view.h"
#include "common/image_cache_manager.h"

namespace OHOS {
namespace LOADIMG {
// 加载图片资源
void LoadBtnImage(UIButton *btn, std::string file, uint32_t resId1, uint32_t resId2);

void LoadImageViewImg(UIImageView *view, std::string file, uint32_t resId);

void LoadImageImg(Image *view, std::string file, uint32_t resId);
}
}
#endif  // LOAD_IMG_H