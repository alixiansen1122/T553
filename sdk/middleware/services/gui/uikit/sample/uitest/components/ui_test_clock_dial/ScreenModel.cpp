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

#include <iostream>
#include <fstream>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include "common/image_cache_manager.h"
#include "UiConfig.h"
#include "ui_resource_image.h"
#include "ScreenModel.h"

namespace OHOS {
ScreenModel::ScreenModel()
{
}

ScreenModel::~ScreenModel()
{
    if (listMainView != nullptr) {
        delete listMainView;
        listMainView = nullptr;
    }
}

ScreenModel *ScreenModel::GetInstance(void)
{
    static ScreenModel screenModel;
    return &screenModel;
}

bool ScreenModel::SetUpListCase(void)
{
    GRAPHIC_LOGD("SetUpListCase\n");
    listMainView = new List<CardInfo>();
    if (listMainView == nullptr) {
        return false;
    }
    listMainView->Clear();
    return true;
}

bool ScreenModel::InitCardConfig(void)
{
    if (listMainView != nullptr) {
        return true;
    }
    if (!SetUpListCase()) {
        return false;
    }
    SetCardConfigDefault();
    return true;
}

void ScreenModel::SetCardConfigDefault(void)
{
    int16_t i;
    struct CardInfo cardInfoData;
    GRAPHIC_LOGD("SetCardConfigDefault\n");
    for (i = 0; i < static_cast<uint16_t>(CardId::MAX_CARD); i++) {
        cardInfoData.cardId = i;
        listMainView->PushBack(cardInfoData);
    }
}

int16_t ScreenModel::GetCardPage(uint16_t page)
{
    ListNode<CardInfo> *node = listMainView->Begin();
    for (uint8_t i = 0; i < page; i++) {
        if (node->next_ != nullptr) {
            node = node->next_;
        } else {
            return -1;
        }
    }
    GRAPHIC_LOGD("GetCardPage cardId:%d\n", node->data_.cardId);
    return node->data_.cardId;
}

int16_t ScreenModel::GetLastCardPage(void)
{
    ListNode<CardInfo> *node = listMainView->Begin();
    for (uint8_t i = 0; i < static_cast<uint8_t>(CardId::MAX_CARD); i++) {
        if (node->next_ == nullptr) {
            node = node->next_;
        }
    }
    return node->data_.cardId;
}

int16_t ScreenModel::GetMaxCard(void)
{
    GRAPHIC_LOGD("GetMaxCard maxPage:%d\n", maxPage);
    return maxPage;
}

void ScreenModel::LoadAppMultiImages(std::string file, uint32_t startId, uint32_t lastId)
{
    if (!ImageCacheManager::GetInstance().LoadAllInMultiRes(file)) {
        GRAPHIC_LOGE("LoadAppMultiImages fail: %s\n", file.c_str());
    }
}

bool ScreenModel::LoadAppImages(std::string file, UIImageView *view, uint32_t resId)
{
    ImageInfo* image = ImageCacheManager::GetInstance().LoadOneInMultiRes(resId, file);
    if (image == nullptr) {
        GRAPHIC_LOGE("LoadAppImages %x fail\n", resId);
        return false;
    }
    view->SetSrc(image);
    return true;
}

void ScreenModel::PreLoadAppimages(void)
{
    LoadAppMultiImages(PLAYER_IAMGES, PLAYER_PLAYERSMODEL_RANDOM, PLAYER_PLAYERHIGHLIGHT);
    LoadAppMultiImages(CLOCK_IAMGES, CLOCK_HAND24HOUR_IMAGE_PATH, CLOCK_TUESDAY_IMAGE_PATH);
    LoadAppMultiImages(COMPASS_IAMGES, COMPASS_ROTATE_CENTER_IMAGE, COMPASS_COMPASS);
    LoadAppMultiImages(DROPDOWN_IAMGES, DROPDOWN_DROPDOWN_BLUETOOTH, DROPDOWN_DROPDOWN_BRI_SCREEN);
    LoadAppMultiImages(HEART_IAMGES, HEART_HEARTRATECOOR, HEART_HEARTRATE_TOP);
}
}
