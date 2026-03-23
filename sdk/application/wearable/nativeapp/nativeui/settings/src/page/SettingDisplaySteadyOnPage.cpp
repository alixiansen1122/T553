/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingDisplaySteadyOnPage
 * Create: 2025-06-01
 */
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "NativeAbility.h"
#include "settings/model/SettingDisplayModel.h"
#include "settings/page/SettingDisplaySteadyOnPage.h"

namespace OHOS {
REGIST_SLICE_PAGE(VIEW_SETTING, SettingPageId::DISPLAY_STEADY_ON_PAGE,
    SettingDisplaySteadyOnPage, false);

static constexpr int16_t SETTING_DISPLAY_STEADY_TITLE_X = 100;
static constexpr int16_t SETTING_DISPLAY_STEADY_TITLE_Y = 39;
static constexpr int16_t SETTING_DISPLAY_STEADY_TITLE_WIDTH = 254;
static constexpr int16_t SETTING_DISPLAY_STEADY_TITLE_HEIGHT = 53;
static constexpr int16_t SETTING_DISPLAY_STEADY_TITLE_FSIZE = 38;
static const char* SETTING_DISPLAY_STEADY_TITLE_TEXT = "长亮时刻";

static constexpr int16_t SETTING_DISPLAY_STEADY_PICKER_X = 64;
static constexpr int16_t SETTING_DISPLAY_STEADY_PICKER_Y = 118;
static constexpr int16_t SETTING_DISPLAY_STEADY_PICKER_WIDTH = 321;
static constexpr int16_t SETTING_DISPLAY_STEADY_PICKER_HEIGHT = 218;
static constexpr int16_t SETTING_DISPLAY_STEADY_PICKER_FSIZE = 44;
static constexpr int16_t SETTING_DISPLAY_STEADY_PICKER_ITEM_HEIGHT = 84;

static constexpr int16_t SETTING_DISPLAY_STEADY_BUTTON_X = 63;
static constexpr int16_t SETTING_DISPLAY_STEADY_BUTTON_Y = 358;
static constexpr int16_t SETTING_DISPLAY_STEADY_BUTTON_WIDTH = 328;
static constexpr int16_t SETTING_DISPLAY_STEADY_BUTTON_HEIGHT = 94;
static constexpr int16_t SETTING_DISPLAY_STEADY_BUTTON_BG_OPA = 51;
static const char* SETTING_DISPLAY_STEADY_BUTTON_ID = "button";

void SettingDisplaySteadyOnPage::OnStart(void* data)
{
    title_ = new UILabel();
    if (title_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingDisplaySteadyOnTimePage::OnStart title_ new fail");
        return;
    }
    title_->SetPosition(SETTING_DISPLAY_STEADY_TITLE_X, SETTING_DISPLAY_STEADY_TITLE_Y);
    title_->Resize(SETTING_DISPLAY_STEADY_TITLE_WIDTH, SETTING_DISPLAY_STEADY_TITLE_HEIGHT);
    title_->SetFont(BOLD_VECTOR_FONT_FILENAME, SETTING_DISPLAY_STEADY_TITLE_FSIZE);
    title_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    title_->SetTextColor(Color::White());
    title_->SetText(SETTING_DISPLAY_STEADY_TITLE_TEXT);

    picker_ = new UIPicker();
    if (picker_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingDisplaySteadyOnTimePage::OnStart picker_ new fail");
        return;
    }
    picker_->SetPosition(SETTING_DISPLAY_STEADY_PICKER_X, SETTING_DISPLAY_STEADY_PICKER_Y);
    picker_->Resize(SETTING_DISPLAY_STEADY_PICKER_WIDTH, SETTING_DISPLAY_STEADY_PICKER_HEIGHT);
    picker_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    picker_->SetBackgroundFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_DISPLAY_STEADY_PICKER_FSIZE);
    picker_->SetHighlightFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_DISPLAY_STEADY_PICKER_FSIZE);
    picker_->SetItemHeight(SETTING_DISPLAY_STEADY_PICKER_ITEM_HEIGHT);
    picker_->SetTextColor(SETTING_LIST_DEFAULT_BG_COLOR, Color::White());
    picker_->SetIntercept(true);

    button_ = new UIButton();
    if (button_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingDisplaySteadyOnTimePage::OnStart button_ new fail");
        return;
    }
    button_->SetPosition(SETTING_DISPLAY_STEADY_BUTTON_X, SETTING_DISPLAY_STEADY_BUTTON_Y);
    button_->Resize(SETTING_DISPLAY_STEADY_BUTTON_WIDTH, SETTING_DISPLAY_STEADY_BUTTON_HEIGHT);
    ImageInfo* image = ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_DISPLAY_OK, SETTING_IMAGE);
    button_->SetImageSrc(image, image);
    button_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::RELEASED);
    button_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    button_->SetStyleForState(STYLE_BACKGROUND_OPA, SETTING_DISPLAY_STEADY_BUTTON_BG_OPA, UIButton::RELEASED);
    button_->SetStyleForState(STYLE_BACKGROUND_OPA, SETTING_DISPLAY_STEADY_BUTTON_BG_OPA, UIButton::PRESSED);
    button_->SetStyleForState(STYLE_IMAGE_OPA, OPA_OPAQUE, UIButton::RELEASED);
    button_->SetStyleForState(STYLE_IMAGE_OPA, OPA_OPAQUE, UIButton::PRESSED);
    button_->SetViewId(SETTING_DISPLAY_STEADY_BUTTON_ID);
    button_->SetOnClickListener(this);

    AddViewToPageContainer(title_);
    AddViewToPageContainer(picker_);
    AddViewToPageContainer(button_);
}

void SettingDisplaySteadyOnPage::OnResume()
{
    const char** data = SettingDisplayModel::GetInstance().GetSteadyOnTimeAllStr();
    uint8_t size = SettingDisplayModel::GetInstance().GetSteadyOnTimeAllStrSize();
    uint8_t selectIndex = SettingDisplayModel::GetInstance().GetSteadyOnTimeIndex();
    picker_->SetValues(data, size);
    picker_->SetSelected(selectIndex);
}

void SettingDisplaySteadyOnPage::OnStop()
{
    if (title_ != nullptr) {
        delete title_;
        title_ = nullptr;
    }
    if (picker_ != nullptr) {
        delete picker_;
        picker_ = nullptr;
    }
    if (button_ != nullptr) {
        delete button_;
        button_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadOneInMultiRes(IMAGE_DISPLAY_OK, SETTING_IMAGE);
}

bool SettingDisplaySteadyOnPage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), SETTING_DISPLAY_STEADY_BUTTON_ID) == 0) {
        uint8_t index = picker_->GetSelected();
        SettingDisplayModel::GetInstance().SetSteadyOnTimeIndex(index);
        NativeAbility::GetInstance().BackToPrevSlicePage();
    }
    return true;
}
}
