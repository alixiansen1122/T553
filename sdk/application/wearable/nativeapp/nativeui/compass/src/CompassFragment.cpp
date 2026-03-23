/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: CompassFragment.cpp
 * Author:
 * Create: 2021-12-21
 */
#include "compass/CompassFragment.h"

#include <string>
#include "UiConfig.h"
#include "common/image_cache_manager.h"
#include "ui_resource_image.h"
#include "compass/CompassModel.h"
#include "wearable_log.h"

namespace OHOS {
static constexpr float VIEW_CENTERS = 227;
static constexpr int16_t POINT_CENTER_IMAGE_X = 150;
static constexpr int16_t POINT_CENTER_IMAGE_Y = 150;
static constexpr int16_t trianImg__X = 210;
static constexpr int16_t trianImg__Y = 0;
static constexpr int16_t directionLabel__X = 160;
static constexpr int16_t directionLabel__Y = 215;
static constexpr int16_t FONT_TEXT_SIZE = 25;
static constexpr int16_t angleLabel__X = 215;
static constexpr int16_t angleLabel__Y = 215;
static constexpr int16_t TEXT_WIDTH = 90;
static constexpr int16_t TEXT_HEIGHT = 35;
static constexpr int16_t RANDOM_NUMBER_720 = 720;
static constexpr int16_t RANDOM_NUMBER_360 = 360;
static constexpr int16_t DELAYING_NUMS = 2000;
static constexpr int16_t RIGHT_NORTH_DOWN_23 = 23;
static constexpr int16_t RIGHT_EASTNORTH_UP_68 = 68;
static constexpr int16_t RIGHT_WESTNORTH_UP_338 = 338;
static constexpr int16_t RIGHT_SOUTH_UP_203 = 203;
static constexpr int16_t RIGHT_EASTSOUTH_UP_158 = 158;
static constexpr int16_t RIGHT_WESTSOUTH_UP_248 = 248;
static constexpr int16_t RIGHT_WEST_UP_293 = 293;
static constexpr int16_t RIGHT_EAST_UP_113 = 113;
static constexpr int16_t DIRECTION_COUNT = 7;

typedef struct {
    uint16_t beginAngle;
    uint16_t endAngle;
    const char *dir;
} CompassItem;

const CompassItem g_compassList[] = {
    {RIGHT_NORTH_DOWN_23, RIGHT_EASTNORTH_UP_68, "东北"},
    {RIGHT_EASTNORTH_UP_68, RIGHT_EAST_UP_113, "东"},
    {RIGHT_EAST_UP_113, RIGHT_EASTSOUTH_UP_158, "东南"},
    {RIGHT_EASTSOUTH_UP_158, RIGHT_SOUTH_UP_203, "南"},
    {RIGHT_SOUTH_UP_203, RIGHT_WESTSOUTH_UP_248, "西南"},
    {RIGHT_WESTSOUTH_UP_248, RIGHT_WEST_UP_293, "西"},
    {RIGHT_WEST_UP_293, RIGHT_WESTNORTH_UP_338, "西北"}
};

void CompassFragment::Update(void)
{
    if (!initialized_) {
        return;
    }
    int16_t angle  = CompassModel::GetInstance()->GetAngle();
    bgImg_->Rotate(-angle, Vector2<float>(VIEW_CENTERS, VIEW_CENTERS));
    if ((angle >= RIGHT_WESTNORTH_UP_338) || (angle >= 0 && angle < RIGHT_NORTH_DOWN_23)) {
        directionLabel_->SetText("北");
        angleLabel_->SetText((std::to_string(angle) + "°").c_str());
    }
    for (int i = 0; i < DIRECTION_COUNT; i++) {
        if ((angle >= g_compassList[i].beginAngle) && (angle < g_compassList[i].endAngle)) {
            directionLabel_->SetText(g_compassList[i].dir);
            angleLabel_->SetText((std::to_string(angle) + "°").c_str());
        }
    }
    CompassModel::GetInstance()->UpdateAngle();
}

bool CompassFragment::LoadCompassImage(UIImageView *view, uint32_t resId)
{
    std::string file = COMPASS_IAMGES;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "CompassFragment LoadCompassImage: %s, resID=0x%x", COMPASS_IAMGES, resId);
    ImageInfo* image = ImageCacheManager::GetInstance().LoadOneInMultiRes(resId, file);
    if (image == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CompassFragment::LoadCompassImage %x fail", resId);
        return false;
    }
    view->SetSrc(image);
    return true;
}

bool CompassFragment::InitCompassImage(void)
{
    bgImg_ = new UIImageView();
    if (bgImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%d], bgImg_ new fail", __LINE__);
        return false;
    }
    bgImg_->SetPosition(0, 0);

    if (!LoadCompassImage(bgImg_, COMPASS_COMPASS)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%d], trianImg_ load fail", __LINE__);
        return false;
    }

    fgImg_ = new UIImageView();
    if (fgImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%d], fgImg_ new fail", __LINE__);
        return false;
    }
    fgImg_->SetPosition(POINT_CENTER_IMAGE_X, POINT_CENTER_IMAGE_Y);
    if (!LoadCompassImage(fgImg_, COMPASS_ROTATE_CENTER_IMAGE)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%d], fgImg_ load fail", __LINE__);
        return false;
    }

    trianImg_ = new UIImageView();
    if (trianImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%d], trianImg_ new fail", __LINE__);
        return false;
    }
    trianImg_->SetPosition(trianImg__X, trianImg__Y);
    if (!LoadCompassImage(trianImg_, COMPASS_CORRESPONDENCE_DEGREE)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "trianImg_ load fail");
        return false;
    }
    return true;
}

void CompassFragment::OnCreateView(void* data)
{
    fragmentView_.SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "CompassFragment InitView");
    if (!InitCompassImage()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitCompassImage fail");
        initialized_ = false;
        return;
    }

    directionLabel_ = new UILabel();
    if (directionLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "directionLabel_ new fail");
        initialized_ = false;
        return;
    }
    directionLabel_->SetPosition(directionLabel__X, directionLabel__Y);
    directionLabel_->Resize(TEXT_WIDTH, TEXT_HEIGHT);
    directionLabel_->SetText("北");
    directionLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_TEXT_SIZE);
    directionLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    directionLabel_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    directionLabel_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    directionLabel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);

    angleLabel_ = new UILabel();
    if (angleLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "angleLabel_ new fail");
        initialized_ = false;
        return;
    }
    angleLabel_->SetPosition(angleLabel__X, angleLabel__Y);
    angleLabel_->Resize(TEXT_WIDTH, TEXT_HEIGHT);
    angleLabel_->SetText("0°");
    angleLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_TEXT_SIZE);
    angleLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    angleLabel_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    angleLabel_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    angleLabel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);

    fragmentView_.Add(bgImg_);
    fragmentView_.Add(fgImg_);
    fragmentView_.Add(trianImg_);
    fragmentView_.Add(directionLabel_);
    fragmentView_.Add(angleLabel_);
    CompassModel::GetInstance()->ResetAngle();
    initialized_ = true;
}

void CompassFragment::OnPauseView()
{
}

void CompassFragment::OnResumeView()
{
}

void CompassFragment::OnDestroyView()
{
    fragmentView_.RemoveAll();
    if (directionLabel_ != nullptr) {
        delete directionLabel_;
        directionLabel_= nullptr;
    }
    if (angleLabel_ != nullptr) {
        delete angleLabel_;
        angleLabel_ = nullptr;
    }
    if (bgImg_ != nullptr) {
        delete bgImg_;
        bgImg_ = nullptr;
    }
    if (fgImg_ != nullptr) {
        delete fgImg_;
        fgImg_ = nullptr;
    }
    if (trianImg_ != nullptr) {
        delete trianImg_;
        trianImg_ = nullptr;
    }
}
}
