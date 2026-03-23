/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: EmptyPage
 * Create: 2025-06-06
 */

#include "UiConfig.h"
#include "main/LoadImg.h"
#include "ui_resource_empty.h"
#include "SlicePageFactory.h"
#include "wearable_log.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "emptypage/EmptyPresenter.h"
#include "emptypage/EmptyPage.h"

namespace OHOS {
static constexpr uint16_t EMPTY_PAGE = 1;
static constexpr uint16_t MEM_LABEL_X = 121;
static constexpr uint16_t MEM_LABEL_W = 193;
static constexpr uint16_t MEM_LABEL_H = 40;
static constexpr uint16_t MEM_LABEL_FONT = 32;
static constexpr uint16_t MEM_USED_LABEL_Y = 161;
static constexpr uint16_t MEM_FREE_LABEL_Y = 272;

REGIST_SLICE_PAGE(VIEW_EMPTY, EMPTY_PAGE, EmptyPage, true);

void EmptyPage::OnStart(void *data)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "EmptyPage OnStart");
    bool ret = ImageCacheManager::GetInstance().LoadAllInMultiRes(EMPTY_IMAGE);
    if (ret == false) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "LoadEmptyImage fail");
        return;
    }
    container_ = new UIViewGroup();
    if (container_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "EmptyPage container_ new fail");
        return;
    }
    container_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    container_->SetOnDragListener(this);
    container_->SetDraggable(true);
    container_->SetTouchable(true);

    bgImg_ = new UIImageView();
    if (bgImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "EmptyPage bgImg_ new fail");
        return;
    }
    bgImg_->SetPosition(0, 0);
    LOADIMG::LoadImageViewImg(bgImg_, EMPTY_IMAGE, IMAGE_EMPTY_BG);
    container_->Add(bgImg_);

    uint32_t memUsed = 0;
    uint32_t memFree = 0;
#if !defined(_WIN32)
    test_get_mem_info(&memUsed, &memFree);
#endif
    memUsedLabel_ = new UILabel();
    if (memUsedLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "EmptyPage memUsedLabel_ new fail");
        return;
    }
    memUsedLabel_->SetPosition(MEM_LABEL_X, MEM_USED_LABEL_Y, MEM_LABEL_W, MEM_LABEL_H);
    memUsedLabel_->SetText(std::to_string(memUsed).c_str());
    memUsedLabel_->SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    memUsedLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, MEM_LABEL_FONT);
    container_->Add(memUsedLabel_);

    memFreeLabel_ = new UILabel();
    if (memFreeLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "EmptyPage memFreeLabel_ new fail");
        return;
    }
    memFreeLabel_->SetPosition(MEM_LABEL_X, MEM_FREE_LABEL_Y, MEM_LABEL_W, MEM_LABEL_H);
    memFreeLabel_->SetText(std::to_string(memFree).c_str());
    memFreeLabel_->SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    memFreeLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, MEM_LABEL_FONT);
    container_->Add(memFreeLabel_);
    AddViewToPageContainer(container_);
}

EmptyPage::EmptyPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "EmptyPage::EmptyPage");
}

EmptyPage::~EmptyPage()
{
    if (container_ != nullptr) {
        container_->RemoveAll();
        delete container_;
        container_ = nullptr;
    }
    if (bgImg_ != nullptr) {
        delete bgImg_;
        bgImg_ = nullptr;
    }
    if (memUsedLabel_ != nullptr) {
        delete memUsedLabel_;
        memUsedLabel_ = nullptr;
    }
    if (memFreeLabel_ != nullptr) {
        delete memFreeLabel_;
        memFreeLabel_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(EMPTY_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "EmptyPage::~EmptyPage");
}

bool EmptyPage::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().ChangeSliceToApplist();
    }
    return false;
}
}
