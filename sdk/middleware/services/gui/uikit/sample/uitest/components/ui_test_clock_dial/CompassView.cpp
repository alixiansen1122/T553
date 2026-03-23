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

#include <string>
#include "common/image_cache_manager.h"
#include "ui_resource_image.h"
#include "CompassView.h"

namespace OHOS {
static constexpr int16_t POINT_CENTER_IMAGE_X = 150;
static constexpr int16_t POINT_CENTER_IMAGE_Y = 150;
static constexpr int16_t TRIANGULATION_X = 210;
static constexpr int16_t TRIANGULATION_Y = 0;
static constexpr int16_t DIRECTIONLABEL_X = 160;
static constexpr int16_t DIRECTIONLABEL_Y = 215;
static constexpr int16_t FONT_TEXT_SIZE = 25;
static constexpr int16_t ANGLEDLABEL_X = 215;
static constexpr int16_t ANGLEDLABEL_Y = 215;
static constexpr int16_t TEXT_WIDTH = 90;
static constexpr int16_t TEXT_HEIGHT = 35;

static constexpr int16_t DIRECTION_COUNT = 7;
static constexpr int16_t RIGHT_NORTH_DOWN_23 = 23;
static constexpr int16_t RIGHT_EASTNORTH_UP_68 = 68;
static constexpr int16_t RIGHT_WESTNORTH_UP_338 = 338;
static constexpr int16_t RIGHT_SOUTH_UP_203 = 203;
static constexpr int16_t RIGHT_EASTSOUTH_UP_158 = 158;
static constexpr int16_t RIGHT_WESTSOUTH_UP_248 = 248;
static constexpr int16_t RIGHT_WEST_UP_293 = 293;
static constexpr int16_t RIGHT_EAST_UP_113 = 113;
static constexpr const char *NORTH_DIRECTION = "北";
static constexpr const char *SOUTH_DIRECTION = "南";
static constexpr const char *WEST_DIRECTION = "西";
static constexpr const char *EAST_DIRECTION = "东";
static constexpr const char *WESTNORTH_DIRECTION = "西北";
static constexpr const char *WESTSOUTH_DIRECTION = "西南";
static constexpr const char *EASTSOUTH_DIRECTION = "东南";
static constexpr const char *EASTNORTH_DIRECTION = "东北";
const CompassItem COMPASS_LIST[] = {
    {RIGHT_NORTH_DOWN_23, RIGHT_EASTNORTH_UP_68, EASTNORTH_DIRECTION},
    {RIGHT_EASTNORTH_UP_68, RIGHT_EAST_UP_113, EAST_DIRECTION},
    {RIGHT_EAST_UP_113, RIGHT_EASTSOUTH_UP_158, EASTSOUTH_DIRECTION},
    {RIGHT_EASTSOUTH_UP_158, RIGHT_SOUTH_UP_203, SOUTH_DIRECTION},
    {RIGHT_SOUTH_UP_203, RIGHT_WESTSOUTH_UP_248, WESTSOUTH_DIRECTION},
    {RIGHT_WESTSOUTH_UP_248, RIGHT_WEST_UP_293, WEST_DIRECTION},
    {RIGHT_WEST_UP_293, RIGHT_WESTNORTH_UP_338, WESTNORTH_DIRECTION}
};

CompassView::~CompassView()
{
    GRAPHIC_LOGD("CompassView::~CompassView()\n");
    if (initViewStatus) {
        RemoveAll();
        delete directionLabel;
        directionLabel = nullptr;

        delete angledLabel;
        angledLabel = nullptr;

        delete backGround;
        backGround = nullptr;

        delete rotateShaft;
        rotateShaft = nullptr;

        delete triangulation;
        triangulation = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(COMPASS_IAMGES);
}

void CompassView::Clear(void)
{
    if (directionLabel != nullptr) {
        delete directionLabel;
        directionLabel = nullptr;
    }
    if (angledLabel != nullptr) {
        delete angledLabel;
        angledLabel = nullptr;
    }
    if (backGround != nullptr) {
        delete backGround;
        backGround = nullptr;
    }
    if (rotateShaft != nullptr) {
        delete rotateShaft;
        rotateShaft = nullptr;
    }
    if (triangulation != nullptr) {
        delete triangulation;
        triangulation = nullptr;
    }
}

void CompassView::SetAngleText(uint16_t angle)
{
    if ((angle >= RIGHT_WESTNORTH_UP_338) || (angle >= 0 && angle < RIGHT_NORTH_DOWN_23)) {
        DirectionSetText(NORTH_DIRECTION, (std::to_string(angle) + "°").c_str());
    }
    for (int i = 0; i < DIRECTION_COUNT; i++) {
        if ((angle >= COMPASS_LIST[i].beginAngle) && (angle < COMPASS_LIST[i].endAngle)) {
            DirectionSetText(COMPASS_LIST[i].dir, (std::to_string(angle) + "°").c_str());
        }
    }
}

void CompassView::DirectionSetText(const char *text, const char *angle)
{
    directionLabel->SetText(text);
    angledLabel->SetText(angle);
}

bool CompassView::LoadCompassImage(UIImageView *view, uint32_t resId)
{
    std::string file = COMPASS_IAMGES;
    ImageInfo* image = ImageCacheManager::GetInstance().LoadOneInMultiRes(resId, file);
    if (image == nullptr) {
        GRAPHIC_LOGE("LoadCompassImage(0x%x): %s, fail", resId, file.c_str());
        return false;
    }
    view->SetSrc(image);
    return true;
}

bool CompassView::InitCompassImage(void)
{
    backGround = new UIImageView();
    if (backGround == nullptr) {
        GRAPHIC_LOGE("backGround new fail\n");
        return false;
    }
    backGround->SetPosition(0, 0);

    if (!LoadCompassImage(backGround, COMPASS_COMPASS)) {
        Clear();
        GRAPHIC_LOGE("triangulation load fail\n");
        return false;
    }

    rotateShaft = new UIImageView();
    if (rotateShaft == nullptr) {
        Clear();
        GRAPHIC_LOGE("rotateShaft new fail\n");
        return false;
    }
    rotateShaft->SetPosition(POINT_CENTER_IMAGE_X, POINT_CENTER_IMAGE_Y);
    if (!LoadCompassImage(rotateShaft, COMPASS_ROTATE_CENTER_IMAGE)) {
        Clear();
        GRAPHIC_LOGE("rotateShaft load fail\n");
        return false;
    }

    triangulation = new UIImageView();
    if (triangulation == nullptr) {
        Clear();
        GRAPHIC_LOGE("triangulation new fail\n");
        return false;
    }
    triangulation->SetPosition(TRIANGULATION_X, TRIANGULATION_Y);
    if (!LoadCompassImage(triangulation, COMPASS_CORRESPONDENCE_DEGREE)) {
        Clear();
        GRAPHIC_LOGE("triangulation load fail\n");
        return false;
    }

    return true;
}

bool CompassView::InitPage(void)
{
    if (!InitCompassImage()) {
        GRAPHIC_LOGE("InitCompassImage fail\n");
        return false;
    }

    directionLabel = new UILabel();
    if (directionLabel == nullptr) {
        Clear();
        GRAPHIC_LOGE("directionLabel new fail\n");
        return false;
    }
    directionLabel->SetPosition(DIRECTIONLABEL_X, DIRECTIONLABEL_Y);
    directionLabel->Resize(TEXT_WIDTH, TEXT_HEIGHT);
    directionLabel->SetText("北");
    directionLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_TEXT_SIZE);
    directionLabel->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    directionLabel->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    directionLabel->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    directionLabel->SetStyle(STYLE_TEXT_COLOR, Color::White().full);

    angledLabel = new UILabel();
    if (angledLabel == nullptr) {
        Clear();
        GRAPHIC_LOGE("angledLabel new fail\n");
        return false;
    }
    angledLabel->SetPosition(ANGLEDLABEL_X, ANGLEDLABEL_Y);
    angledLabel->Resize(TEXT_WIDTH, TEXT_HEIGHT);
    angledLabel->SetText("0°");
    angledLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_TEXT_SIZE);
    angledLabel->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    angledLabel->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    angledLabel->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    angledLabel->SetStyle(STYLE_TEXT_COLOR, Color::White().full);

    Add(backGround);
    Add(rotateShaft);
    Add(triangulation);
    Add(directionLabel);
    Add(angledLabel);
    SetDraggable(false);
    SetTouchable(false);
    initViewStatus = true;
    return true;
}

bool CompassView::InitView()
{
    if (!InitPage()) {
        GRAPHIC_LOGE("InitPage fail\n");
        return false;
    }
    SetDraggable(true);
    SetTouchable(true);
    return true;
}

bool CompassView::GetInitViewState(void)
{
    return initViewStatus;
}

void CompassView::CompassStartRotate(int16_t numb)
{
    if (backGround == nullptr) {
        GRAPHIC_LOGE("backGround is nullptr\n");
        return;
    }
    backGround->Rotate(-numb, VIEW_CENTER);
}
}
