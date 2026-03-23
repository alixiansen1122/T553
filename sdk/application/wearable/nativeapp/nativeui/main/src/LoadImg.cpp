/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: LoadImg.cpp
 * Author:
 * Create: 2025-06-05
 */

#include "main/LoadImg.h"

namespace OHOS {
namespace LOADIMG {
// 加载图片资源
void LoadBtnImage(UIButton *btn, std::string file, uint32_t resId1, uint32_t resId2)
{
    ImageInfo *image1 = ImageCacheManager::GetInstance().LoadOneInMultiRes(resId1, file);
    if (image1 == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "LoadBtnImage resId1 fail");
        return;
    }
    ImageInfo *image2 = ImageCacheManager::GetInstance().LoadOneInMultiRes(resId2, file);
    if (image2 == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "LoadBtnImage resId2 fail");
        return;
    }
    btn->SetImageSrc(image1, image2);
}

void LoadImageViewImg(UIImageView *view, std::string file, uint32_t resId)
{
    ImageInfo *image = ImageCacheManager::GetInstance().LoadOneInMultiRes(resId, file);
    if (image == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "LoadImageViewImg %x fail", resId);
        return;
    }
    view->SetSrc(image);
    return;
}

void LoadImageImg(Image *view, std::string file, uint32_t resId)
{
    ImageInfo *image = ImageCacheManager::GetInstance().LoadOneInMultiRes(resId, file);
    if (image == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "LoadImageImg %x fail", resId);
        return;
    }
    view->SetSrc(image);
    return;
}
}
}
