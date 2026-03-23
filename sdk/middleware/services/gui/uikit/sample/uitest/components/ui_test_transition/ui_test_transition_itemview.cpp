/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: UiTestTransitionItemView.cpp
 * Create: 2025-04
 */

#include "ui_test_transition_itemview.h"
#include "common/image_cache_manager.h"
#include "UiConfig.h"
#include "gfx_utils/graphic_log.h"

namespace OHOS {

    static constexpr int16_t NUMS_2 = 2;
    static constexpr int16_t ITEM_BUTTON_WIDTH = HORIZONTAL_RESOLUTION / NUMS_2;
    static constexpr int16_t ANIMATOR_SICON_SIZE = 148;
    static constexpr int16_t ANIMATOR_SICON_POINT_X = (ITEM_BUTTON_WIDTH - ANIMATOR_SICON_SIZE) / NUMS_2;
    static constexpr int16_t ANIMATOR_SICON_POINT_Y = 150;
    static constexpr int16_t BIND_IMAGE_DELAY = 180;
    static constexpr int16_t PAUSE_TIME = 800;
    static constexpr int16_t ITEM_LABEL_X = 0;
    static constexpr int16_t ITEM_LABEL_Y = 320;
    static constexpr int16_t ITEM_LABEL_WIDTH = ITEM_BUTTON_WIDTH;
    static constexpr int16_t ITEM_LABEL_HEIGHT = 40;
    static constexpr int16_t SETTING_ITEM_FONT_SIZE = 30;
    static constexpr int16_t SETTING_BASE_ICON_X = ANIMATOR_SICON_POINT_X - 21;
    static constexpr int16_t SETTING_BASE_ICON_Y = ANIMATOR_SICON_POINT_Y - 21;
    static constexpr int16_t BACKDROP_ICON_SIZE = 190;

    UITestTransitionItemView::UITestTransitionItemView()
    {
        SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
        SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
        SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
        SetTouchable(true);
        SetUpChild();

        Add(&leftPreviewIcon_);
        Add(&sAnimator_);
        Add(&sLabel_);
        Add(&rightPreviewIcon_);
        Add(&rightAnimator_);
        Add(&rightLabel_);
    }

    UITestTransitionItemView::~UITestTransitionItemView()
    {
        Remove(&sAnimator_);
        Remove(&sLabel_);
        Remove(&leftPreviewIcon_);
        Remove(&rightPreviewIcon_);
        Remove(&rightAnimator_);
        Remove(&rightLabel_);
    }

    void UITestTransitionItemView::SetItemInfo(const UITestTransitionSample& leftItemInfo,
        const UITestTransitionSample& rightItemInfo)
    {
        leftItemInfo_ = leftItemInfo;
        rightItemInfo_ = rightItemInfo;

        sLabel_.SetText(leftItemInfo_.label);
        if (!LoadMultiImages(leftItemInfo_, false)) {
            GRAPHIC_LOGE("leftItemInfo_ cannot open file: %s", leftItemInfo_.resourceImage);
            return;
        }

        rightLabel_.SetText(rightItemInfo_.label);
        if (!LoadMultiImages(rightItemInfo_, true)) {
            GRAPHIC_LOGE("rightItemInfo_ cannot open file: %s", rightItemInfo_.resourceImage);
            return;
        }
        SetUpChild();
    }

    bool UITestTransitionItemView::LoadMultiImages(UITestTransitionSample& itemInfo, bool right)
    {
        UIImageView* previewIcon = &leftPreviewIcon_;
        UIImageAnimatorView* animator = &sAnimator_;
        uint16_t animatorSiconX =  ANIMATOR_SICON_POINT_X;
        if (right) {
            previewIcon = &rightPreviewIcon_;
            animator = &rightAnimator_;
            animatorSiconX = ANIMATOR_SICON_POINT_X + ITEM_BUTTON_WIDTH;
        }

        if (itemInfo.viewId == 0) { // 0: UI_INVALIDE
            previewIcon->SetVisible(false);
            animator->SetVisible(false);
            GRAPHIC_LOGE("itemInfo.viewId is  UI_INVALIDE");
            return false;
        }
        
        FILE* fp = fopen(itemInfo.resourceImage, "rb");
        if (fp == nullptr) {
            GRAPHIC_LOGE("cannot open file: %s", itemInfo.resourceImage);
            return false;
        }

        previewIcon->SetSrc(ImageCacheManager::GetInstance().
            LoadOneInMultiRes(itemInfo.resIds[0], itemInfo.resourceImage, fp));

        uint8_t imageNum = itemInfo.resIds.size();
        std::vector<ImageAnimatorInfo> searchImageInfo(imageNum);
        for (uint8_t i = 0; i < imageNum; i++) {
            searchImageInfo[i].imageInfo = ImageCacheManager::GetInstance().LoadOneInMultiRes(
                itemInfo.resIds[i], itemInfo.resourceImage, fp);
            searchImageInfo[i].imageType = IMG_SRC_IMAGE_INFO;
            searchImageInfo[i].pos.x = animatorSiconX;
            searchImageInfo[i].pos.y = ANIMATOR_SICON_POINT_Y;
            searchImageInfo[i].width = ANIMATOR_SICON_SIZE;
            searchImageInfo[i].height = ANIMATOR_SICON_SIZE;
        }
        animator->SetImageAnimatorSrc(searchImageInfo.data(), imageNum, BIND_IMAGE_DELAY);
        fclose(fp);
        return true;
    }

    void UITestTransitionItemView::SetUpChild()
    {
        sAnimator_.SetPosition(ANIMATOR_SICON_POINT_X, ANIMATOR_SICON_POINT_Y);
        sAnimator_.Resize(ANIMATOR_SICON_SIZE, ANIMATOR_SICON_SIZE);
        sAnimator_.SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
        sAnimator_.SetRepeat(true);
        sAnimator_.SetTimeOfPause(PAUSE_TIME);
        sAnimator_.Start();

        leftPreviewIcon_.SetPosition(ANIMATOR_SICON_POINT_X, ANIMATOR_SICON_POINT_Y);
        leftPreviewIcon_.Resize(ANIMATOR_SICON_SIZE, ANIMATOR_SICON_SIZE);
        leftPreviewIcon_.SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
        leftPreviewIcon_.SetVisible(true);

        sLabel_.SetPosition(ITEM_LABEL_X, ITEM_LABEL_Y);
        sLabel_.Resize(ITEM_LABEL_WIDTH, ITEM_LABEL_HEIGHT);
        sLabel_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_ITEM_FONT_SIZE);
        sLabel_.SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        sLabel_.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
        sLabel_.SetStyle(STYLE_TEXT_COLOR, Color::White().full);
        sLabel_.SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
        sLabel_.SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);

        rightAnimator_.SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
        rightAnimator_.SetRepeat(true);
        rightAnimator_.SetTimeOfPause(PAUSE_TIME);
        rightAnimator_.Start();

        rightPreviewIcon_.SetPosition(ITEM_BUTTON_WIDTH + ANIMATOR_SICON_POINT_X, ANIMATOR_SICON_POINT_Y);
        rightPreviewIcon_.Resize(ANIMATOR_SICON_SIZE, ANIMATOR_SICON_SIZE);
        rightPreviewIcon_.SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
        rightPreviewIcon_.SetVisible(true);

        rightLabel_.SetPosition(ITEM_BUTTON_WIDTH + ITEM_LABEL_X, ITEM_LABEL_Y);
        rightLabel_.Resize(ITEM_LABEL_WIDTH, ITEM_LABEL_HEIGHT);
        rightLabel_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_ITEM_FONT_SIZE);
        rightLabel_.SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        rightLabel_.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
        rightLabel_.SetStyle(STYLE_TEXT_COLOR, Color::White().full);
        rightLabel_.SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
        rightLabel_.SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    }

    uint16_t UITestTransitionItemView::GetClickViewId(const ClickEvent& event)
    {
        int16_t clickY = event.GetCurrentPos().y;
        int16_t minItemY = SETTING_BASE_ICON_Y;
        int16_t maxItemY = SETTING_BASE_ICON_Y + BACKDROP_ICON_SIZE + ITEM_LABEL_HEIGHT;
        // Check if the click position is within the valid Y-axis range
        if (clickY < minItemY || clickY > maxItemY) {
            // 0 indicates an invalid UI element
            return 0;
        }

        int16_t clickX = event.GetCurrentPos().x;
        int16_t leftItemX = SETTING_BASE_ICON_X;
        int16_t rightItemX = ITEM_BUTTON_WIDTH + SETTING_BASE_ICON_X;
        int16_t itemWidth = BACKDROP_ICON_SIZE;
        // Check if the click position is within the left item's X-axis range
        if (clickX > leftItemX && clickX < (leftItemX + itemWidth)) {
            return leftItemInfo_.viewId;
        }

        // Check if the click position is within the right item's X-axis range
        if (clickX > rightItemX && clickX < rightItemX + itemWidth) {
            return rightItemInfo_.viewId;
        }

        // 0 indicates an invalid UI element
        return 0;
    }

    void UITestTransitionItemView::SelectedChange()
    {
        sAnimator_.SetVisible(true);
        sAnimator_.Start();
        rightAnimator_.SetVisible(true);
        rightAnimator_.Start();
        leftPreviewIcon_.SetVisible(false);
        rightPreviewIcon_.SetVisible(false);
    }

    void UITestTransitionItemView::Reset()
    {
        sAnimator_.SetVisible(false);
        sAnimator_.Stop();
        rightAnimator_.SetVisible(false);
        rightAnimator_.Stop();
        leftPreviewIcon_.SetVisible(true);
        rightPreviewIcon_.SetVisible(true);
    }
}