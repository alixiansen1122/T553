/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingCardEffectItemGroup
 * Create: 2025-04
 */
#include "common/image_cache_manager.h"
#include "gfx_utils/graphic_log.h"
#include "UiConfig.h"
#include "settings/common/SettingCommon.h"
#include "settings/common/SettingCardEffectSample.h"
#include "settings/common/SettingCardEffectItemGroup.h"

namespace OHOS {
static constexpr int16_t NUMS_2 = 2;
static constexpr int16_t SCREEN_MIDDLE = HORIZONTAL_RESOLUTION / NUMS_2;
static constexpr int16_t ANIMATOR_SICON_SIZE = 148;
static constexpr int16_t ANIMATOR_SICON_POINT_X = (SCREEN_MIDDLE - ANIMATOR_SICON_SIZE) / NUMS_2;
static constexpr int16_t ANIMATOR_SICON_POINT_Y = 150;
static constexpr int16_t BIND_IMAGE_DELAY = 180;
static constexpr int16_t PAUSE_TIME = 800;
static constexpr int16_t ITEM_LABEL_X = 0;
static constexpr int16_t ITEM_LABEL_Y = 320;
static constexpr int16_t ITEM_LABEL_WIDTH = SCREEN_MIDDLE;
static constexpr int16_t ITEM_LABEL_HEIGHT = 40;
static constexpr int16_t SETTING_ITEM_FONT_SIZE = 30;
static constexpr int16_t BACKDROP_ICON_X = ANIMATOR_SICON_POINT_X - 21;
static constexpr int16_t BACKDROP_ICON_Y = ANIMATOR_SICON_POINT_Y - 21;
static constexpr int16_t BACKDROP_ICON_SIZE = 190;

SettingCardEffectItemGroup::SettingCardEffectItemGroup()
{
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
    SetTouchable(true);
    SetUpChild();
    Add(&leftSelectIcon_);
    Add(&leftPreviewIcon_);
    Add(&leftAnimator_);
    Add(&leftLabel_);
    Add(&rightSelectIcon_);
    Add(&rightPreviewIcon_);
    Add(&rightAnimator_);
    Add(&rightLabel_);
}

SettingCardEffectItemGroup::~SettingCardEffectItemGroup()
{
    Remove(&leftAnimator_);
    Remove(&leftLabel_);
    Remove(&leftSelectIcon_);
    Remove(&leftPreviewIcon_);
    Remove(&rightSelectIcon_);
    Remove(&rightPreviewIcon_);
    Remove(&rightAnimator_);
    Remove(&rightLabel_);
}

void SettingCardEffectItemGroup::SetItemInfo(const SettingCardEffectSample& leftItemInfo,
    const SettingCardEffectSample& rightItemInfo)
{
    leftItemInfo_ = leftItemInfo;
    rightItemInfo_ = rightItemInfo;

    leftLabel_.SetText(leftItemInfo_.label);
    leftSelectIcon_.SetVisible(leftItemInfo_.selected);

    if (!LoadMultiImages(leftItemInfo_, false)) {
        GRAPHIC_LOGE("leftItemInfo_ cannot open file: %s", leftItemInfo_.resourceImage);
        return;
    }

    rightLabel_.SetText(rightItemInfo_.label);
    rightSelectIcon_.SetVisible(rightItemInfo_.selected);

    if (!LoadMultiImages(rightItemInfo_, true)) {
        GRAPHIC_LOGE("rightItemInfo_ cannot open file: %s", rightItemInfo_.resourceImage);
        return;
    }
}

bool SettingCardEffectItemGroup::LoadMultiImages(SettingCardEffectSample& itemInfo, bool right)
{
    UIImageView* previewIcon = &leftPreviewIcon_;
    UIImageView* selectIcon = &leftSelectIcon_;
    UIImageAnimatorView* animator = &leftAnimator_;
    uint16_t animatorSiconX =  ANIMATOR_SICON_POINT_X;
    if (right) {
        previewIcon = &rightPreviewIcon_;
        selectIcon = &rightSelectIcon_;
        animator = &rightAnimator_;
        animatorSiconX = ANIMATOR_SICON_POINT_X + SCREEN_MIDDLE;
    }

    if (itemInfo.viewId == CardEffectID::UI_INVALIDE) { // -1: UI_INVALIDE
        previewIcon->SetVisible(false);
        selectIcon->SetVisible(false);
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
    selectIcon->SetSrc(ImageCacheManager::GetInstance().
        LoadOneInMultiRes(itemInfo.backgroundImageId, itemInfo.resourceImage, fp));

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

void SettingCardEffectItemGroup::SetUpChild()
{
    InitializeAnimator(leftAnimator_, ANIMATOR_SICON_POINT_X, ANIMATOR_SICON_POINT_Y, ANIMATOR_SICON_SIZE);

    InitializeIcon(leftPreviewIcon_, ANIMATOR_SICON_POINT_X, ANIMATOR_SICON_POINT_Y, ANIMATOR_SICON_SIZE);

    InitializeLabel(leftLabel_, ITEM_LABEL_X, ITEM_LABEL_Y, ITEM_LABEL_WIDTH, ITEM_LABEL_HEIGHT);

    InitializeIcon(leftSelectIcon_, BACKDROP_ICON_X, BACKDROP_ICON_Y, BACKDROP_ICON_SIZE);

    int rightItemX = SCREEN_MIDDLE + ANIMATOR_SICON_POINT_X;
    InitializeAnimator(rightAnimator_, rightItemX, ANIMATOR_SICON_POINT_Y, ANIMATOR_SICON_SIZE);

    InitializeIcon(rightPreviewIcon_, rightItemX, ANIMATOR_SICON_POINT_Y, ANIMATOR_SICON_SIZE);

    rightItemX = SCREEN_MIDDLE + ITEM_LABEL_X;
    InitializeLabel(rightLabel_, rightItemX, ITEM_LABEL_Y, ITEM_LABEL_WIDTH, ITEM_LABEL_HEIGHT);

    rightItemX = SCREEN_MIDDLE + BACKDROP_ICON_X;
    InitializeIcon(rightSelectIcon_, rightItemX, BACKDROP_ICON_Y, BACKDROP_ICON_SIZE);
}

void SettingCardEffectItemGroup::InitializeAnimator(UIImageAnimatorView& animator, int x, int y, int size)
{
    animator.SetPosition(x, y);
    animator.Resize(size, size);
    animator.SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    animator.SetRepeat(true);
    animator.SetTimeOfPause(PAUSE_TIME);
}

void SettingCardEffectItemGroup::InitializeIcon(UIImageView& icon, int x, int y, int size)
{
    icon.SetPosition(x, y);
    icon.Resize(size, size);
    icon.SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    icon.SetVisible(true);
}

void SettingCardEffectItemGroup::InitializeLabel(UILabel& label, int x, int y, int width, int height)
{
    label.SetPosition(x, y);
    label.Resize(width, height);
    label.SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_ITEM_FONT_SIZE);
    label.SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    label.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    label.SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    label.SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    label.SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
}

CardEffectID SettingCardEffectItemGroup::GetViewId(const ClickEvent& event)
{
    int16_t clickY = event.GetCurrentPos().y;
    int16_t minItemY = BACKDROP_ICON_Y;
    int16_t maxItemY = BACKDROP_ICON_Y + BACKDROP_ICON_SIZE + ITEM_LABEL_HEIGHT;

    // Check if the click position is within the valid Y-axis range
    if (clickY < minItemY || clickY > maxItemY) {
        // -1 indicates an invalid UI element
        return CardEffectID::UI_INVALIDE;
    }

    int16_t clickX = event.GetCurrentPos().x;
    int16_t leftItemX = BACKDROP_ICON_X;
    int16_t rightItemX = SCREEN_MIDDLE + BACKDROP_ICON_X;
    int16_t itemWidth = BACKDROP_ICON_SIZE;
    // Check if the click position is within the left item's X-axis range
    if (clickX > leftItemX && clickX < (leftItemX + itemWidth)) {
        return leftItemInfo_.viewId;
    }

    // Check if the click position is within the right item's X-axis range
    if (clickX > rightItemX && clickX < rightItemX + itemWidth) {
        return rightItemInfo_.viewId;
    }

    // -1 indicates an invalid UI element
    return CardEffectID::UI_INVALIDE;
}

void SettingCardEffectItemGroup::SelectedChange()
{
    leftAnimator_.SetVisible(true);
    leftAnimator_.Start();
    leftPreviewIcon_.SetVisible(false);

    rightAnimator_.SetVisible(true);
    rightAnimator_.Start();
    rightPreviewIcon_.SetVisible(false);
}

void SettingCardEffectItemGroup::Reset()
{
    leftAnimator_.SetVisible(false);
    leftAnimator_.Stop();
    leftPreviewIcon_.SetVisible(true);

    rightAnimator_.SetVisible(false);
    rightAnimator_.Stop();
    rightPreviewIcon_.SetVisible(true);
}
}