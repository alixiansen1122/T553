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

#include "ui_test_image_packer.h"
#if (ENABLE_IMAGE_PACKER && ENABLE_IMAGE_PACKER_TEST)
#include <string>
#include "common/image_cache_manager.h"
#include "components/ui_image_view.h"
#include "gfx_utils/graphic_log.h"

namespace OHOS {
static constexpr uint16_t LABEL_HEIGHT = 50;
#define SPORT_IMAGE_BIN  RES_PATH"sport.bin"
const char* files[] = {
    RES_PATH"red.bin",
    RES_PATH"run3.bin",
    RES_PATH"run2.bin",
    RES_PATH"yellow.bin",
    RES_PATH"whilte.bin",
    RES_PATH"run1.bin"
};

void UITestImagePacker::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetThrowDrag(true);
        container_->SetHorizontalScrollState(false);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - LABEL_HEIGHT);
        positionX_ = 50; // 50: init position x
        positionY_ = 5;  // 5: init position y
    }
    if (loadAllOnceBtn_ == nullptr) {
        loadAllOnceBtn_ = new UILabelButton();
        loadAllOnceBtn_->SetPosition(350, 100, 100, 50); // 350, 100, 100, 50: left, top, width, height
        loadAllOnceBtn_->SetText("LoadAllOnce");
        loadAllOnceBtn_->SetOnClickListener(this);
        container_->Add(loadAllOnceBtn_);
    }
    if (unloadAllOnceBtn_ == nullptr) {
        unloadAllOnceBtn_ = new UILabelButton();
        unloadAllOnceBtn_->SetPosition(350, 200, 100, 50); // 350, 200, 100, 50: left, top, width, height
        unloadAllOnceBtn_->SetText("UnloadAllOnce");
        unloadAllOnceBtn_->SetOnClickListener(this);
        container_->Add(unloadAllOnceBtn_);
    }
}

void UITestImagePacker::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    loadAllOnceBtn_ = nullptr;
    unloadAllOnceBtn_ = nullptr;

    std::string file = SPORT_IMAGE_BIN;
    if (isUnloadLoadAllOnce_) {
        GRAPHIC_LOGW("UnloadAllInMultiRes: %s", SPORT_IMAGE_BIN);
        ImageCacheManager::GetInstance().UnloadAllInMultiRes(file);
    } else {
        uint32_t resId = 0x100001;
        for (uint16_t i = 0; i < 6; i++) { // 6: res count
            GRAPHIC_LOGW("UnloadOneInMultiRes: resId = %x, filePath = %s", resId, file.c_str());
            ImageCacheManager::GetInstance().UnloadOneInMultiRes(resId, file);
            resId += 1;
        }
    }
    for (uint32_t i = 0; i < sizeof(files) / sizeof(files[0]); i++) {
        file = files[i];
        ImageCacheManager::GetInstance().UnloadSingleRes(file);
        GRAPHIC_LOGW("UnloadSingleRes: filePath = %s", files[i]);
    }
}

const UIView* UITestImagePacker::GetTestView()
{
    TestLoadOneInMultiRes();
    TestLoadSingleRes();
    return container_;
}

bool UITestImagePacker::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == loadAllOnceBtn_) {
        isLoadAllOnce_ = !isLoadAllOnce_;
        GRAPHIC_LOGW("isLoadAllOnce_: %d.", isLoadAllOnce_);
    } else if (&view == unloadAllOnceBtn_) {
        isUnloadLoadAllOnce_ = !isUnloadLoadAllOnce_;
        GRAPHIC_LOGW("isUnloadLoadAllOnce_: %d.", isUnloadLoadAllOnce_);
    }
    return true;
}

UIImageView* UITestImagePacker::AddImageView(ImageInfo* image)
{
    UIImageView* imageView = new UIImageView();
    imageView->SetSrc(image);
    imageView->SetPosition(positionX_, positionY_);
    container_->Add(imageView);
    positionY_ += imageView->GetHeight() + 10; // 10: height offset
    return imageView;
}

void UITestImagePacker::TestSingleImage()
{
    uint16_t maxWidth = 0;
    for (uint32_t i = 0; i < sizeof(files) / sizeof(files[0]); i++) {
        GRAPHIC_LOGW("LoadSingleRes: %s", files[i]);
        std::string file = files[i];
        ImageInfo* image = ImageCacheManager::GetInstance().LoadSingleRes(file);
        auto imageView = AddImageView(image);
        maxWidth = maxWidth > imageView->GetWidth() ? maxWidth : imageView->GetWidth();
    }
    positionX_ = positionX_ + maxWidth + 10; // 10: position x offset
}

void UITestImagePacker::TestMutiImage(bool isLoadAllOnce)
{
    if (container_ == nullptr) {
        return;
    }
    std::string file = SPORT_IMAGE_BIN;
    if (isLoadAllOnce) {
        if (!ImageCacheManager::GetInstance().LoadAllInMultiRes(file)) {
            GRAPHIC_LOGE("LoadAllInMultiRes failed: %s", SPORT_IMAGE_BIN);
            return;
        }
        GRAPHIC_LOGW("LoadAllInMultiRes success: %s", SPORT_IMAGE_BIN);
    }
    uint32_t resId = 0x100001;  // 0x100001: the first id
    constexpr uint16_t MAX_RES_COUNT = 10; // 10: the max count res
    uint16_t maxWidth = 0;
    for (uint16_t i = 0; i < MAX_RES_COUNT; i++) {
        GRAPHIC_LOGW("LoadOneInMultiRes: resId = %x, filePath = %s", resId, file.c_str());
        ImageInfo* image = ImageCacheManager::GetInstance().LoadOneInMultiRes(resId, file);
        auto imageView = AddImageView(image);
        maxWidth = maxWidth > imageView->GetWidth() ? maxWidth : imageView->GetWidth();
        resId += 1;
    }
    positionX_ = positionX_ + maxWidth + 20; // 20: position x offset
}

void UITestImagePacker::TestLoadOneInMultiRes()
{
    positionY_ = 0;
    TestMutiImage(isLoadAllOnce_);
}

void UITestImagePacker::TestLoadSingleRes()
{
    positionY_ = 0;
    TestSingleImage();
}
} // OHOS
#endif // ENABLE_IMAGE_PACKER && ENABLE_IMAGE_PACKER_TEST
