/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: ScreenModels
 * Author:
 * Create: 2022-01-22
 */

#include "wearable_log.h"
#include "iostream"
#include <fstream>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include "kvstore_env.h"
#include "kv_store.h"
#include "screensetting/ScreenModels.h"
#include "common/image_cache_manager.h"
#include "UiConfig.h"
#include "ui_resource_image.h"

namespace OHOS {

ScreenModels::ScreenModels()
{
}

ScreenModels::~ScreenModels()
{
}

ScreenModels *ScreenModels::GetInstance(void)
{
    static ScreenModels screenModel;
    return &screenModel;
}

void ScreenModels::InitKvStorage(void)
{
#if !defined(_WIN32)
#ifndef XTS_SUPPORT
    int ret = UtilsSetEnv("/user");
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "UtilsSetEnv /user ret = %d", ret);
#endif
#else
    int ret = UtilsSetEnv("../bin");
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "UtilsSetEnv ../user ret = %d", ret);
#endif
}

void ScreenModels::LoadAppMultiImages(std::string file, uint32_t startId, uint32_t lastId)
{
    for (uint32_t i = startId; i <= lastId; i++) {
        ImageInfo* image = ImageCacheManager::GetInstance().LoadOneInMultiRes(i, file);
        if (image == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "LoadAppMultiImages %x fail", i);
            return;
        }
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "LoadAppMultiImages %x", i);
    }
}

bool ScreenModels::LoadAppImages(std::string file, UIImageView *view, uint32_t resId)
{
    ImageInfo* image = ImageCacheManager::GetInstance().LoadOneInMultiRes(resId, file);
    if (image == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "LoadAppImages %x fail", resId);
        return false;
    }
    view->SetSrc(image);
    return true;
}

void ScreenModels::PreLoadAppimages(void)
{
    LoadAppMultiImages(PLAYER_IAMGES, PLAYER_PLAYERSMODEL_RANDOM, PLAYER_PLAYERHIGHLIGHT);
    LoadAppMultiImages(CLOCK_IAMGES, CLOCK_HAND24HOUR_IMAGE_PATH, CLOCK_TUESDAY_IMAGE_PATH);
    LoadAppMultiImages(COMPASS_IAMGES, COMPASS_ROTATE_CENTER_IMAGE, COMPASS_COMPASS);
    LoadAppMultiImages(DROPDOWN_IAMGES, DROPDOWN_DROPDOWN_BLUETOOTH, DROPDOWN_DROPDOWN_BRI_SCREEN_B);
}
}
