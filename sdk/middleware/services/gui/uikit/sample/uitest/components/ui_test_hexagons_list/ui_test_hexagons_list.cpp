/*
 * Copyright (c) @CompanyNameMagicTag. 2024. All rights reserved.
 */

#include "ui_test_hexagons_list/ui_test_hexagons_list.h"
#include "components/root_view.h"
#include "components/ui_image_view.h"
#include "components/ui_transform_group.h"
#include "common/image_cache_manager.h"
#include "common/screen.h"
#include "ui_test_list_fps/ui_list_fps_res.h"
#include "dock/focus_manager.h"

namespace OHOS {
const uint8_t IMG_COUNT = 45;
const uint8_t IMG_DISTANCE_DELTA = 1;
const uint8_t IMG_SIZE_DELTA = 1;
const int16_t BUTTON_SIZE = 60;
const float IMG_SCALE_FACTOR_DELTA = 0.01f;
const int16_t MARGIN_DELTA = 1;
const int16_t BORDER_RADIUS_DELTA = 1;
const float GLOBAL_SCALE = 1.0f;
const float GLOBAL_SCALE_DELTA = 0.02f;
const float CLICK_SCALE_TARGET_IN_NORMAL = 2.0f;

void UITestHexagonsList::ConfigButtonAttr(UIView& view, int16_t x, int16_t y, int16_t width, int16_t height)
{
    view.SetOpaScale(OPA_TRANSPARENT);
    view.SetOnClickListener(this);
    view.SetOnLongPressListener(this);
    view.SetPosition(x, y, width, height);
    container_->Add(&view);
}

void UITestHexagonsList::SetUpButtons(int16_t xEnd, int16_t yEnd, int16_t xMiddle, int16_t yMiddle)
{
    if (modifyImgSizeButton_ == nullptr) {
        modifyImgSizeButton_ = new UIButton();
        ConfigButtonAttr(*modifyImgSizeButton_, xMiddle - BUTTON_SIZE, 0, BUTTON_SIZE, BUTTON_SIZE);
    }
    if (modifyImgDistanceButton_ == nullptr) {
        modifyImgDistanceButton_ = new UIButton();
        ConfigButtonAttr(*modifyImgDistanceButton_, xMiddle, 0, BUTTON_SIZE, BUTTON_SIZE);
    }
    if (modifyScaleFatorButton_ == nullptr) {
        modifyScaleFatorButton_ = new UIButton();
        ConfigButtonAttr(*modifyScaleFatorButton_, xEnd - BUTTON_SIZE, yMiddle - BUTTON_SIZE, BUTTON_SIZE, BUTTON_SIZE);
    }
    if (modifyGlobalScaleButton_ == nullptr) {
        modifyGlobalScaleButton_ = new UIButton();
        ConfigButtonAttr(*modifyGlobalScaleButton_, xEnd - BUTTON_SIZE, yMiddle, BUTTON_SIZE, BUTTON_SIZE);
    }
    if (modifyHorMarginButton_ == nullptr) {
        modifyHorMarginButton_ = new UIButton();
        ConfigButtonAttr(*modifyHorMarginButton_, xMiddle - BUTTON_SIZE, yEnd - BUTTON_SIZE, BUTTON_SIZE, BUTTON_SIZE);
    }
    if (modifyVerMarginButton_ == nullptr) {
        modifyVerMarginButton_ = new UIButton();
        ConfigButtonAttr(*modifyVerMarginButton_, xMiddle, yEnd - BUTTON_SIZE, BUTTON_SIZE, BUTTON_SIZE);
    }
    if (modifyBorderRadiusButton_ == nullptr) {
        modifyBorderRadiusButton_ = new UIButton();
        ConfigButtonAttr(*modifyBorderRadiusButton_, xMiddle - BUTTON_SIZE, yMiddle, BUTTON_SIZE, BUTTON_SIZE);
    }
    if (modifyModeButton_ == nullptr) {
        modifyModeButton_ = new UIButton(); // click bottom mid to change mode for next time.
        ConfigButtonAttr(*modifyModeButton_,
            (container_->GetWidth() - BUTTON_SIZE) / 2, container_->GetHeight() - BUTTON_SIZE, // 2: divisor
            BUTTON_SIZE, BUTTON_SIZE);
    }
}

void UITestHexagonsList::SetUpChildren()
{
    ImageInfo* logo = ImageCacheManager::GetInstance().LoadSingleRes(RES_PATH"hexagons.bin");
    ImageInfo* heart = ImageCacheManager::GetInstance().LoadSingleRes(RES_PATH"/A094_051_8888.bin");
    for (int16_t i = 0; i < IMG_COUNT; i++) {
        UIImageView* img = new UIImageView();
        img->SetSrc(logo);
        if (isImageMode_) {  // test UIImageView
            img->SetTouchable(true);
            img->Resize(imgSize_, imgSize_);
            img->SetOnClickListener(this);
            hexagonsList_->Add(img);
            continue;
        }
        // isImageMode_ == false, test UITransformGroup
        UITransformGroup* transformGroup = new UITransformGroup();
        transformGroup->SetStyle(STYLE_BACKGROUND_OPA, 0);
        transformGroup->Add(img);

        UIImageView* topImg = new UIImageView();
        topImg->SetSrc(heart);
        transformGroup->Add(topImg);

        UILabel* label = new UILabel();
        label->SetText("6");
        transformGroup->Add(label);

        // Layout transformgroup as if displayed in center.
        img->SetPosition(0, 0, imgSize_, imgSize_);
        topImg->SetPosition(0, 0, imgSize_ / 3, imgSize_ / 3); // 3: divisor
        label->SetPosition(imgSize_ - (imgSize_ / 3), 0, imgSize_ / 3, imgSize_ / 3); // 3: divisor
        transformGroup->SetPosition(0, 0, imgSize_, imgSize_);

        transformGroup->SetTouchable(true);
        transformGroup->SetOnClickListener(this);
        hexagonsList_->Add(transformGroup);

        if (i == 0) { // test TransformChild
            if (callback_ == nullptr) {
                callback_ = new TransformCallback(transformGroup, topImg);
            }
            if (animator_ == nullptr) {
                animator_ = new Animator(callback_, hexagonsList_, 0, true);
            }
            if (animator_ != nullptr) {
                animator_->Start();
            }
        }
    }
}

void UITestHexagonsList::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIViewGroup();
    }
    container_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    if (hexagonsList_ == nullptr) {
        hexagonsList_ = new UICustomHexagonsList();
    }
    container_->Add(hexagonsList_);

    if (Screen::GetInstance().GetScreenShape() == RECTANGLE) {
        borderRadius_ = Screen::GetInstance().GetWidth() * 0.336; // 0.336, empirical coefficient
        hexagonsList_->SetStyle(STYLE_BORDER_RADIUS, borderRadius_);
        // STYLE_BORDER_WIDTH was used to debug, it should be removed in the actual running content
        hexagonsList_->SetStyle(STYLE_BORDER_WIDTH, 1);
        hexagonsList_->SetStyle(STYLE_MARGIN_LEFT, horMargin_);
        hexagonsList_->SetStyle(STYLE_MARGIN_RIGHT, horMargin_);
        hexagonsList_->SetStyle(STYLE_MARGIN_TOP, verMargin_);
        hexagonsList_->SetStyle(STYLE_MARGIN_BOTTOM, verMargin_);
        hexagonsList_->SetStyle(STYLE_BORDER_COLOR, Color::Red().full);
        imgDistance_ = container_->GetWidth() * 0.31; // 0.31, empirical coefficient
        imgSize_ = container_->GetWidth() * 0.27; // 0.27, empirical coefficient
    } else {
        imgDistance_ = container_->GetWidth() * 0.25991; // 0.25991, empirical coefficient
        imgSize_ = container_->GetWidth() * 0.23348; // 0.23348, empirical coefficient
    }
    hexagonsList_->SetPosition(0, 0,
        container_->GetWidth() - 2 * (horMargin_ + 1), container_->GetHeight() - 2 * (verMargin_ + 1)); // 2, two

    SetUpChildren();
    imgScaleFactor_ = hexagonsList_->GetScaleFactorByDistance();
    globalScale_ = hexagonsList_->GetGlobalScale();
    hexagonsList_->SetImageDistance(imgDistance_);
    hexagonsList_->SetImageSizeInCenter(imgSize_);
    int16_t blankSize = hexagonsList_->GetRelativeRect().GetWidth() > hexagonsList_->GetRelativeRect().GetHeight() ?
        hexagonsList_->GetRelativeRect().GetWidth() / 2 : hexagonsList_->GetRelativeRect().GetHeight() / 2; // 2, half
    hexagonsList_->SetScrollBlankSize(blankSize);
    hexagonsList_->SetReboundSize(blankSize);
    hexagonsList_->SetThrowDrag(true);
    hexagonsList_->LayoutChildren(true);
    SetUpButtons(container_->GetWidth() - 1, container_->GetHeight() - 1,
        container_->GetWidth() / 2, container_->GetHeight() / 2); // 2, half size
    FocusManager::GetInstance()->RequestFocus(hexagonsList_);
}

void UITestHexagonsList::TearDown()
{
    if (animator_ != nullptr) {
        animator_->Stop();

        delete callback_;
        callback_ = nullptr;

        delete animator_;
        animator_ = nullptr;
    }

    FocusManager::GetInstance()->ClearFocus();
    DeleteChildren(container_);
    container_ = nullptr;
    hexagonsList_ = nullptr;
    modifyImgSizeButton_ = nullptr;
    modifyImgDistanceButton_ = nullptr;
    modifyScaleFatorButton_ = nullptr;
    modifyGlobalScaleButton_ = nullptr;
    modifyHorMarginButton_ = nullptr;
    modifyVerMarginButton_ = nullptr;
    modifyBorderRadiusButton_ = nullptr;
    modifyModeButton_ = nullptr;
}

void UITestHexagonsList::HandleClickEventForRectScreen(const UIView& view)
{
    if (modifyHorMarginButton_ == &view) {
        horMargin_ += isIncreaseHorMargin_ ? MARGIN_DELTA : -MARGIN_DELTA;
        horMargin_ = horMargin_ < 0 ? 0 : horMargin_;
        hexagonsList_->SetPosition(horMargin_, verMargin_,
            container_->GetWidth() - 2 * (horMargin_ + 1), container_->GetHeight() - 2 * (verMargin_ + 1)); // 2, two
        hexagonsList_->LayoutChildren(true);
    } else if (modifyVerMarginButton_ == &view) {
        verMargin_ += isIncreaseVerMargin_ ? MARGIN_DELTA : -MARGIN_DELTA;
        verMargin_ = verMargin_ < 0 ? 0 : verMargin_;
        hexagonsList_->SetPosition(horMargin_, verMargin_,
            container_->GetWidth() - 2 * (horMargin_ + 1), container_->GetHeight() - 2 * (verMargin_ + 1)); // 2, two
        hexagonsList_->LayoutChildren(true);
    } else if (modifyBorderRadiusButton_ == &view) {
        borderRadius_ += isIncreaseBorderRadius_ ? BORDER_RADIUS_DELTA : -BORDER_RADIUS_DELTA;
        borderRadius_ = borderRadius_ < 0 ? 0 : borderRadius_;
        int8_t index = hexagonsList_->GetFocusedImgIndex();
        hexagonsList_->SetStyle(STYLE_BORDER_RADIUS, borderRadius_);
    }
    container_->Invalidate();
}

bool UITestHexagonsList::OnClick(UIView& view, const ClickEvent& event)
{
    // Handle content click for image view
    if ((view.GetViewType() == UI_IMAGE_VIEW) || (view.GetViewType() == UI_TRANSFORM_GROUP)) {
        uint8_t index = static_cast<uint8_t>(view.GetViewIndex());
        float scaleInSmall = hexagonsList_->GetScaleInSmallState();
        Vector2<int16_t> center(hexagonsList_->GetWidth() / 2, hexagonsList_->GetHeight() / 2); // 2, center
        Vector2<int16_t> delta = center - hexagonsList_->GetCurrentPosition(index);
        if (FloatEqual(hexagonsList_->GetGlobalScale(), scaleInSmall)) {
            hexagonsList_->StartRotateAnimator(index, delta.x_, delta.y_, scaleInSmall, 1.0f);
        } else if (FloatEqual(hexagonsList_->GetGlobalScale(), 1.0f)) {
            hexagonsList_->StartRotateAnimator(index, delta.x_, delta.y_, 1.0f, CLICK_SCALE_TARGET_IN_NORMAL);
        } else if (FloatEqual(hexagonsList_->GetGlobalScale(), CLICK_SCALE_TARGET_IN_NORMAL)) {
            hexagonsList_->StartRotateAnimator(index, delta.x_, delta.y_, CLICK_SCALE_TARGET_IN_NORMAL, 1.0f);
        }
    }

    if (modifyImgSizeButton_ == &view) {
        imgSize_ += isIncreaseImgSize_ ? IMG_SIZE_DELTA : -IMG_SIZE_DELTA;
        hexagonsList_->SetImageSizeInCenter(imgSize_);
        hexagonsList_->LayoutChildren(true);
    } else if (modifyImgDistanceButton_ == &view) {
        imgDistance_ += isIncreaseImgDistance_ ? IMG_DISTANCE_DELTA : -IMG_DISTANCE_DELTA;
        hexagonsList_->SetImageDistance(imgDistance_);
        hexagonsList_->LayoutChildren(true);
    } else if (modifyScaleFatorButton_ == &view) {
        imgScaleFactor_ += isIncreaseScaleFactor_ ? IMG_SCALE_FACTOR_DELTA : -IMG_SCALE_FACTOR_DELTA;
        hexagonsList_->SetScaleFactorByDistance(imgScaleFactor_);
        hexagonsList_->LayoutChildren(true);
    } else if (modifyGlobalScaleButton_ == &view) {
        globalScale_ += isIncreaseGlobalScale_ ? GLOBAL_SCALE_DELTA : -GLOBAL_SCALE_DELTA;
        int8_t index = hexagonsList_->GetFocusedImgIndex();
        if (index >= 0) {
            hexagonsList_->Scale(globalScale_, static_cast<uint8_t>(index));
        }
    } else if (modifyModeButton_ == &view) {
        isImageMode_ = !isImageMode_;
        printf("Change display mode to: %d\n", isImageMode_);
    }
    if (FloatLess(globalScale_, hexagonsList_->GetScaleThesholdOfZeroFactor())) {
        hexagonsList_->SetReboundMode(UIHexagonsList::ReboundMode::CENTER_IMG_ON_SCR_CENTER);
    } else {
        hexagonsList_->SetReboundMode(UIHexagonsList::ReboundMode::NEAREST_IMG_ON_SCR_CENTER);
    }

    if (Screen::GetInstance().GetScreenShape() == RECTANGLE) {
        HandleClickEventForRectScreen(view);
    }

    GRAPHIC_LOGE("%d-%d imgDist_=%d imgSize=%d imgScaleFactor=%f globalScale=%f horPad=%d verPad=%d boderRadius=%d",
        view.GetViewType(), view.GetViewIndex(), imgDistance_, imgSize_, imgScaleFactor_, globalScale_,
        horMargin_, verMargin_, borderRadius_);
    return true;
}

bool UITestHexagonsList::OnLongPress(UIView& view, const LongPressEvent& event)
{
    if (modifyImgSizeButton_ == &view) {
        isIncreaseImgSize_ = !isIncreaseImgSize_;
    } else if (modifyImgDistanceButton_ == &view) {
        isIncreaseImgDistance_ = !isIncreaseImgDistance_;
    } else if (modifyScaleFatorButton_ == &view) {
        isIncreaseScaleFactor_ = !isIncreaseScaleFactor_;
    } else if (modifyGlobalScaleButton_ == &view) {
        isIncreaseGlobalScale_ = !isIncreaseGlobalScale_;
    } else if (modifyHorMarginButton_ == &view) {
        isIncreaseHorMargin_ = !isIncreaseHorMargin_;
    } else if (modifyVerMarginButton_ == &view) {
        isIncreaseVerMargin_ = !isIncreaseVerMargin_;
    } else if (modifyBorderRadiusButton_ == &view) {
        isIncreaseBorderRadius_ = !isIncreaseBorderRadius_;
    }
    GRAPHIC_LOGE("incSize=%d incDist=%d incFactor=%d incGloScale=%d incHPadding=%d incVPadding=%d incBRadius=%d",
        isIncreaseImgSize_, isIncreaseImgDistance_, isIncreaseScaleFactor_, isIncreaseGlobalScale_,
        isIncreaseHorMargin_, isIncreaseVerMargin_, isIncreaseBorderRadius_);
    return true;
}

const UIView* UITestHexagonsList::GetTestView()
{
    return container_;
}
}