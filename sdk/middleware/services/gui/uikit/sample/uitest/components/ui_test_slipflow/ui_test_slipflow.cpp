/*
 * Copyright (c) CompanyNameMagicTag 2024. All rights reserved.
 * Description: UITestSlip
 * Author:
 * Create: 2024-08
 */

#include "ui_test_slipflow.h"
#include "common/screen.h"
#include "font/ui_font.h"
#include "gfx_utils/graphic_math.h"
#include "gfx_utils/rect.h"
#include "common/image_cache_manager.h"

namespace OHOS {
namespace {
struct FaceInfo {
    const char* name;
    const char* path;
} ;

static constexpr uint8_t SLIPFLOW_MAX_PAGE_NUM = 6;
static FaceInfo g_faceInfo[SLIPFLOW_MAX_PAGE_NUM] = {
    {"1 rainbow", RES_PATH"/rainbow_circle.bin"},
    {"2 kaleidoscope", RES_PATH"/kaleidoscope_circle.bin"},
    {"3 planet", RES_PATH"/planet_circle.bin"},
    {"4 rainbow", RES_PATH"/rainbow_circle.bin"},
    {"5 kaleidoscope", RES_PATH"/kaleidoscope_circle.bin"},
    {"6 planet", RES_PATH"/planet_circle.bin"}
};
static constexpr const char* BACK_IMAGE_PATH = RES_PATH"/image_background_circle.bin";

static constexpr uint8_t SLIPFLOW_PRELOAD_PAGE_NUM = 3;
static constexpr int16_t LABEL_FONT_SIZE = 30;
static constexpr uint8_t DEFAULT_STEP = 10;
static constexpr uint16_t BUTTON_WIDTH = 60;
static constexpr uint16_t BUTTON_HEIGHT = 40;
static constexpr uint16_t BUTTON_GROUP_WIDTH = 300;
static constexpr uint16_t BUTTON_GROUP_HEIGHT = 80;
static constexpr uint8_t DIVISOR_TWO = 2;
static constexpr uint16_t PAGE_WIDTH = 200;
static constexpr uint16_t PAGE_HEIGHT = 200;
} // namespace

void UITestSlipflow::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetThrowDrag(true);
        container_->SetHorizontalScrollState(false);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    }
}

void UITestSlipflow::TearDown()
{
    container_->Remove(slipflow_);
    DeleteChildren(container_);
    if (container_ != nullptr) {
        container_ = nullptr;
    }
    delete slipflow_;
    slipflow_ = nullptr;

    label_ = nullptr;
    slipUpNoticelabel_ = nullptr;
    lastIndex_ = 0;
    loadNum_ = 0;

    imgWidth_ = 200;        // 200: default value
    imgPadding_ = 50;       // 50: default value
    mirrorOpa_ = 30;        // 30: default value
    rotateAngle_ = 70.0f;   // 70: default value
}

const UIView* UITestSlipflow::GetTestView()
{
    UITestSlipflowView();
    return container_;
}

void UITestSlipflow::UITestSlipflowView()
{
    if (container_ != nullptr) {
        if (slipflow_ == nullptr) {
            slipflow_ = new UISlipflowView();
        }
        slipflow_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        slipflow_->SetIntercept(true);
        slipflow_->SetThrowDrag(true);
        slipflow_->SetOnSlipflowListener(this);
        UITestSlipflowViewInitFlow();

        if (label_ == nullptr) {
            label_ = new UILabel();
        }
        label_->SetPosition(127, 50, 200, 50); // 127 50 200 50: label position
        label_->SetTextColor(Color::White());
        label_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        lastIndex_ = slipflow_->GetCurrentPage();
        label_->SetText(g_faceInfo[lastIndex_].name);
        label_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT_SIZE);
        if (slipUpNoticelabel_ == nullptr) {
            slipUpNoticelabel_ = new UILabel();
        }
        slipUpNoticelabel_->SetPosition(127, 400, 200, 50); // 127 400 200 50: label position
        slipUpNoticelabel_->SetTextColor(Color::White());
        slipUpNoticelabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        slipUpNoticelabel_->SetText("继续上滑删除");
        slipUpNoticelabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30 : font size
        slipUpNoticelabel_->SetVisible(false);
        container_->Add(slipflow_);
        container_->Add(label_);
        container_->Add(slipUpNoticelabel_);
    }
}

void UITestSlipflow::UITestSlipflowViewInitFlow()
{
    for (uint8_t i = 0; i < SLIPFLOW_MAX_PAGE_NUM; i++) {
        UIImageView* image = new UIImageView();
        ImageInfo* src = ImageCacheManager::GetInstance().LoadSingleRes(std::string(g_faceInfo[i].path));
        image->SetViewId(g_faceInfo[i].name);
        image->SetSrc(src);
        image->Resize(PAGE_WIDTH, PAGE_HEIGHT);
        image->SetResizeMode(UIImageView::ImageResizeMode::FILL);
        image->SetAutoEnable(false);
        UIViewGroup* group = new UIViewGroup();
        group->SetViewId(g_faceInfo[i].name);
        group->SetPosition(0, 0, PAGE_WIDTH, Screen::GetInstance().GetHeight());
        group->SetStyle(STYLE_BACKGROUND_OPA, 0);
        group->SetDragParentInstead(true);
        UIImageView* backImage = new UIImageView();
        backImage->SetSrc(ImageCacheManager::GetInstance().LoadSingleRes(std::string(BACK_IMAGE_PATH)));
        backImage->Resize(PAGE_WIDTH, PAGE_HEIGHT);
        backImage->SetResizeMode(UIImageView::ImageResizeMode::FILL);
        backImage->SetAutoEnable(false);
        group->Add(backImage);
        group->Add(image);
        slipflow_->Add(group);
        backImage->LayoutCenterOfParent();
        image->LayoutCenterOfParent();
        loadNum_++;
    }
}

bool UITestSlipflow::OnScroll(int16_t distance)
{
    if (slipflow_->GetChildrenNumber() == 0) {
        label_->SetText("");
        return true;
    }
    int16_t contMidX = static_cast<int16_t>(Screen::GetInstance().GetWidth() / DIVISOR_TWO);
    int16_t minOffset = contMidX;
    uint8_t newCenterIndex = lastIndex_;
    for (uint8_t i = 0; i < slipflow_->GetChildrenNumber(); i++) {
        Rect rect =  slipflow_->GetViewByIndex(i)->GetOrigRect();
        int16_t x = rect.GetLeft();
        int16_t x1 = rect.GetRight();
        int16_t xMid = static_cast<int16_t>(x + (x1 - x + 1) / DIVISOR_TWO);
        int16_t distance = MATH_ABS(xMid - contMidX);
        if (distance < minOffset) {
            newCenterIndex = i;
            minOffset = distance;
        }
    }
    lastIndex_ = newCenterIndex;
    label_->SetText(slipflow_->GetViewByIndex(lastIndex_)->GetViewId());

    Rect curViewRect = slipflow_->GetViewByIndex(lastIndex_)->GetOrigRect();
    int16_t centerMid = curViewRect.GetLeft() + curViewRect.GetWidth() / DIVISOR_TWO;
    int16_t offset = MATH_ABS(contMidX - centerMid);
    if (offset > curViewRect.GetWidth() / DIVISOR_TWO) {
        label_->SetStyle(STYLE_TEXT_OPA, 0);
    } else {
        uint8_t alpha =  (uint8_t)((1.0f - ((float)offset / (curViewRect.GetWidth() / DIVISOR_TWO))) * OPA_OPAQUE);
        label_->SetStyle(STYLE_TEXT_OPA, alpha);
    }
    return true;
}

void UITestSlipflow::OnScrollUpStart()
{
    label_->SetVisible(false);
    slipUpNoticelabel_->SetVisible(true);
}

void UITestSlipflow::OnScrollUpEnd()
{
    OnScroll(0);
    label_->SetVisible(true);
    slipUpNoticelabel_->SetVisible(false);
}

void UITestSlipflow::OnRefreshPageOpaScale(UIView* view, int16_t xOffset, int16_t yOffset)
{
    UIView* image = static_cast<UIViewGroup*>(view)->GetChildrenHead()->GetNextSibling();
    if (xOffset < 0) {
        image->SetOpaScale(OPA_OPAQUE);
    } else if (xOffset > slipflow_->GetPageWidth()) {
        image->SetOpaScale(0);
    } else {
        uint8_t alpha = (uint8_t)(OPA_OPAQUE * (1.0f - ((float)xOffset / slipflow_->GetPageWidth())));
        image->SetOpaScale(alpha);
    }
    if (yOffset > 0) {
        if (yOffset > (slipflow_->GetPageHeight() / DIVISOR_TWO)) {
            image->SetOpaScale(OPA_OPAQUE * 0.4f);
        } else {
            uint8_t alpha = (uint8_t)(OPA_OPAQUE *
                            (1.0f - 0.6f * ((float)yOffset / (slipflow_->GetPageHeight() / DIVISOR_TWO))));
            image->SetOpaScale(alpha);
        }
    }
}

bool UITestSlipflow::OnRemove(UIView* view)
{
    if (view == nullptr) {
        return false;
    }
    UIViewGroup* group = static_cast<UIViewGroup*>(view);
    UIView* curView = group->GetChildrenHead();
    UIView* deleteView = nullptr;
    while (curView != nullptr) {
        deleteView = curView;
        curView = curView->GetNextSibling();
        group->Remove(deleteView);
        delete deleteView;
        deleteView = nullptr;
    }
    delete group;
    group = nullptr;
    return true;
}
} // namespace OHOS
