/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportMistouchPage
 * Create: 2025-06-06
 */

#include "UiConfig.h"
#include "main/LoadImg.h"
#include "wearable_log.h"
#include "ui_resource_sport.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "sport/SportView.h"
#include "sport/SportMistouchPage.h"

namespace OHOS {
static constexpr uint16_t TITLE_X = 100;
static constexpr uint16_t TITLE_Y = 39;
static constexpr uint16_t TITLE_W = 253;
static constexpr uint16_t TITLE_H = 53;
static constexpr uint16_t TITLE_FONT = 38;
static constexpr uint16_t SCREEN_BG_X = 19;
static constexpr uint16_t SCREEN_BG_Y = 107;
static constexpr uint16_t SCREEN_ICON_X = 27;
static constexpr uint16_t SCREEN_ICON_Y = 115;
static constexpr uint16_t SCREEN_TITLE_X = 100;
static constexpr uint16_t SCREEN_TITLE_Y = 134;
static constexpr uint16_t SCREEN_TITLE_W = 222;
static constexpr uint16_t SCREEN_TITLE_H = 39;
static constexpr uint16_t SCREEN_TITLE_FONT = 32;
static constexpr uint16_t SCREEN_BUTTON_X = 323;
static constexpr uint16_t SCREEN_BUTTON_Y = 131;
static constexpr uint16_t SCREEN_BUTTON_W = 82;
static constexpr uint16_t SCREEN_BUTTON_H = 47;
static constexpr uint16_t SCREEN_TEXT_X = 19;
static constexpr uint16_t SCREEN_TEXT_Y = 224;
static constexpr uint16_t SCREEN_TEXT_W = 415;
static constexpr uint16_t SCREEN_TEXT_H = 146;
static constexpr uint16_t SCREEN_TEXT_FONT = 36;
static constexpr uint16_t CROWN_BG_X = 19;
static constexpr uint16_t CROWN_BG_Y = 394;
static constexpr uint16_t CROWN_ICON_X = 27;
static constexpr uint16_t CROWN_ICON_Y = 402;
static constexpr uint16_t CROWN_TITLE_X = 100;
static constexpr uint16_t CROWN_TITLE_Y = 421;
static constexpr uint16_t CROWN_TITLE_W = 222;
static constexpr uint16_t CROWN_TITLE_H = 39;
static constexpr uint16_t CROWN_TITLE_FONT = 32;
static constexpr uint16_t CROWN_BUTTON_X = 323;
static constexpr uint16_t CROWN_BUTTON_Y = 417;
static constexpr uint16_t CROWN_BUTTON_W = 82;
static constexpr uint16_t CROWN_BUTTON_H = 47;
static constexpr uint16_t CROWN_TEXT_X = 19;
static constexpr uint16_t CROWN_TEXT_Y = 510;
static constexpr uint16_t CROWN_TEXT_W = 415;
static constexpr uint16_t CROWN_TEXT_H = 146;
static constexpr uint16_t CROWN_TEXT_FONT = 36;
static constexpr uint16_t REBOUND_SIZE = 60;
static constexpr uint16_t TOP_BLANK_SIZE = 36;
static constexpr uint16_t BOTTOM_BLANK_SIZE = 100;

REGIST_SLICE_PAGE(VIEW_SPORT, SportPages::SPORT_PAGE_MISTOUCH, SportMistouchPage, false);

SportMistouchPage::~SportMistouchPage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }
    if (title_ != nullptr) {
        delete title_;
        title_ = nullptr;
    }
    if (bgScreen_ != nullptr) {
        delete bgScreen_;
        bgScreen_ = nullptr;
    }
    if (imageScreen_ != nullptr) {
        delete imageScreen_;
        imageScreen_ = nullptr;
    }
    if (titleScreen_ != nullptr) {
        delete titleScreen_;
        titleScreen_ = nullptr;
    }
    if (buttonScreen_ != nullptr) {
        delete buttonScreen_;
        buttonScreen_ = nullptr;
    }
    if (textScreen_ != nullptr) {
        delete textScreen_;
        textScreen_ = nullptr;
    }
    if (bgCrown_ != nullptr) {
        delete bgCrown_;
        bgCrown_ = nullptr;
    }
    if (imageCrown_ != nullptr) {
        delete imageCrown_;
        imageCrown_ = nullptr;
    }
    if (titleCrown_ != nullptr) {
        delete titleCrown_;
        titleCrown_ = nullptr;
    }
    if (buttonCrown_ != nullptr) {
        delete buttonCrown_;
        buttonCrown_ = nullptr;
    }
    if (textCrown_ != nullptr) {
        delete textCrown_;
        textCrown_ = nullptr;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SportMistouchPage::~SportMistouchPage");
}

void SportMistouchPage::OnStart(void *data)
{
    group_ = new UIScrollView();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportMistouchPage group_ new fail");
        return;
    }
    group_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    group_->SetOnDragListener(this);
    group_->SetDraggable(true);
    group_->SetTouchable(true);
    group_->SetElastic(true);
    group_->SetYScrollBarVisible(true);
    group_->SetReboundSize(REBOUND_SIZE);
    group_->SetScrollBlankSize(TOP_BLANK_SIZE, UIAbstractScroll::Direction::TOP);
    group_->SetScrollBlankSize(BOTTOM_BLANK_SIZE, UIAbstractScroll::Direction::BOTTOM);

    title_ = new UILabel();
    if (title_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportMistouchPage title_ new fail");
        return;
    }
    title_->SetPosition(TITLE_X, TITLE_Y, TITLE_W, TITLE_H);
    title_->SetText("运动防误触");
    title_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    title_->SetFont(BOLD_VECTOR_FONT_FILENAME, TITLE_FONT);
    group_->Add(title_);

    bgScreen_ = new UIImageView();
    if (bgScreen_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportMistouchPage bgScreen_ new fail");
        return;
    }
    bgScreen_->SetPosition(SCREEN_BG_X, SCREEN_BG_Y);
    LOADIMG::LoadImageViewImg(bgScreen_, SPORT_IMAGE, IMAGE_SPORT_ITEMBG);
    group_->Add(bgScreen_);

    buttonScreen_ = new UIToggleButton();
    if (buttonScreen_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportMistouchPage buttonScreen_ new fail");
        return;
    }
    buttonScreen_->SetPosition(SCREEN_BUTTON_X, SCREEN_BUTTON_Y, SCREEN_BUTTON_W, SCREEN_BUTTON_H);
    buttonScreen_->SetViewId(SPORT_MISTOUCH_SCREEN_BUTTON);
    buttonScreen_->SetImages(ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_SPORT_TOGGLE1, SPORT_IMAGE),
                             ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_SPORT_TOGGLE0, SPORT_IMAGE));
    buttonScreen_->SetState(false);
    buttonScreen_->SetOnClickListener(this);
    group_->Add(buttonScreen_);

    titleScreen_ = new UILabel();
    if (titleScreen_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportMistouchPage titleScreen_ new fail");
        return;
    }
    titleScreen_->SetPosition(SCREEN_TITLE_X, SCREEN_TITLE_Y, SCREEN_TITLE_W, SCREEN_TITLE_H);
    titleScreen_->SetText("自动锁定屏幕");
    titleScreen_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    titleScreen_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SCREEN_TITLE_FONT);
    group_->Add(titleScreen_);

    imageScreen_ = new UIImageView();
    if (imageScreen_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportMistouchPage imageScreen_ new fail");
        return;
    }
    imageScreen_->SetPosition(SCREEN_ICON_X, SCREEN_ICON_Y);
    LOADIMG::LoadImageViewImg(imageScreen_, SPORT_IMAGE, IMAGE_SPORT_SCREEN);
    group_->Add(imageScreen_);

    textScreen_ = new UILabel();
    if (textScreen_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportMistouchPage textScreen_ new fail");
        return;
    }
    textScreen_->SetPosition(SCREEN_TEXT_X, SCREEN_TEXT_Y, SCREEN_TEXT_W, SCREEN_TEXT_H);
    textScreen_->SetText("开启后，运动中熄屏将自动锁定屏幕触摸功能，长按表冠可解锁。");
    textScreen_->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);
    textScreen_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    textScreen_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SCREEN_TEXT_FONT);
    group_->Add(textScreen_);

    bgCrown_ = new UIImageView();
    if (bgCrown_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportMistouchPage bgCrown_ new fail");
        return;
    }
    bgCrown_->SetPosition(CROWN_BG_X, CROWN_BG_Y);
    LOADIMG::LoadImageViewImg(bgCrown_, SPORT_IMAGE, IMAGE_SPORT_ITEMBG);
    group_->Add(bgCrown_);

    buttonCrown_ = new UIToggleButton();
    if (buttonCrown_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportMistouchPage buttonCrown_ new fail");
        return;
    }
    buttonCrown_->SetPosition(CROWN_BUTTON_X, CROWN_BUTTON_Y, CROWN_BUTTON_W, CROWN_BUTTON_H);
    buttonCrown_->SetViewId(SPORT_MISTOUCH_CROWN_BUTTON);
    buttonCrown_->SetImages(ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_SPORT_TOGGLE1, SPORT_IMAGE),
                            ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_SPORT_TOGGLE0, SPORT_IMAGE));
    buttonCrown_->SetState(false);
    buttonCrown_->SetOnClickListener(this);
    group_->Add(buttonCrown_);

    titleCrown_ = new UILabel();
    if (titleCrown_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportMistouchPage titleCrown_ new fail");
        return;
    }
    titleCrown_->SetPosition(CROWN_TITLE_X, CROWN_TITLE_Y, CROWN_TITLE_W, CROWN_TITLE_H);
    titleCrown_->SetText("自动锁定表冠");
    titleCrown_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    titleCrown_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, CROWN_TITLE_FONT);
    group_->Add(titleCrown_);

    imageCrown_ = new UIImageView();
    if (imageCrown_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportMistouchPage imageCrown_ new fail");
        return;
    }
    imageCrown_->SetPosition(CROWN_ICON_X, CROWN_ICON_Y);
    LOADIMG::LoadImageViewImg(imageCrown_, SPORT_IMAGE, IMAGE_SPORT_CROWN);
    group_->Add(imageCrown_);

    textCrown_ = new UILabel();
    if (textCrown_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportMistouchPage textCrown_ new fail");
        return;
    }
    textCrown_->SetPosition(CROWN_TEXT_X, CROWN_TEXT_Y, CROWN_TEXT_W, CROWN_TEXT_H);
    textCrown_->SetText("开启后，运动中熄屏将自动锁定表冠点按、旋钮功能，长按表冠可解锁。");
    textCrown_->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);
    textCrown_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    textCrown_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, CROWN_TEXT_FONT);
    group_->Add(textCrown_);

    AddViewToPageContainer(group_);
}

bool SportMistouchPage::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_SETTINGS,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool SportMistouchPage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), SPORT_MISTOUCH_SCREEN_BUTTON) == 0) {
        // 暂不支持
    } else if (strcmp(view.GetViewId(), SPORT_MISTOUCH_CROWN_BUTTON) == 0) {
        // 暂不支持
    }
    return true;
}
}