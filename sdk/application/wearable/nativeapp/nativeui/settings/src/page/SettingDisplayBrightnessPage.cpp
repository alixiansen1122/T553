/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingDisplayBrightnessPage
 * Create: 2025-06-01
 */
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "NativeAbility.h"
#include "settings/model/SettingDisplayModel.h"
#include "settings/page/SettingDisplayBrightnessPage.h"

namespace OHOS {
REGIST_SLICE_PAGE(VIEW_SETTING, SettingPageId::DISPLAY_BRIGHTNESS_PAGE, SettingDisplayBrightnessPage, false);

static constexpr int8_t SETTING_DISPLAY_TEMP_STR_LEN = 10;
static constexpr uint8_t SETTING_DISPLAY_BRIGHTNESS_MAX = 100;
static constexpr uint8_t SETTING_DISPLAY_BRIGHTNESS_MIN = 0;
static constexpr int16_t SETTING_DISPLAY_BRIGHTNESS_AUTO_TIME = 300;

static constexpr int16_t SETTING_DISPLAY_BRI_TITLE_X = 100;
static constexpr int16_t SETTING_DISPLAY_BRI_TITLE_Y = 39;
static constexpr int16_t SETTING_DISPLAY_BRI_TITLE_WIDTH = 254;
static constexpr int16_t SETTING_DISPLAY_BRI_TITLE_HEIGHT = 53;
static constexpr int16_t SETTING_DISPLAY_BRI_TITLE_FSIZE = 38;
static const char* SETTING_DISPLAY_BRI_TITLE_TEXT = "调整亮度";

static constexpr int16_t SETTING_DISPLAY_BRI_PROGRESS_X = 127;
static constexpr int16_t SETTING_DISPLAY_BRI_PROGRESS_Y = 127;
static constexpr int16_t SETTING_DISPLAY_BRI_PROGRESS_WIDTH = 200;
static constexpr int16_t SETTING_DISPLAY_BRI_PROGRESS_HEIGHT = 200;
static constexpr int16_t SETTING_DISPLAY_BRI_PROGRESS_LINE_WIDTH = 25;
static constexpr int16_t SETTING_DISPLAY_BRI_PROGRESS_RADIUS = 100;
static constexpr int16_t SETTING_DISPLAY_BRI_START_ANGLE = 360;
static constexpr int16_t SETTING_DISPLAY_BRI_END_ANGLE = 0;

static constexpr int16_t SETTING_DISPLAY_BRI_PROGRESSNUM_X = 173;
static constexpr int16_t SETTING_DISPLAY_BRI_PROGRESSNUM_Y = 187;
static constexpr int16_t SETTING_DISPLAY_BRI_PROGRESSNUM_WIDTH = 108;
static constexpr int16_t SETTING_DISPLAY_BRI_PROGRESSNUM_HEIGHT = 60;
static constexpr int16_t SETTING_DISPLAY_BRI_PROGRESSNUM_FSIZE = 59;

static constexpr int16_t SETTING_DISPLAY_BRI_ADD_X = 343;
static constexpr int16_t SETTING_DISPLAY_BRI_ADD_Y = 187;
static constexpr int16_t SETTING_DISPLAY_BRI_ADD_WIDTH = 80;
static constexpr int16_t SETTING_DISPLAY_BRI_ADD_HEIGHT = 80;
static constexpr int16_t SETTING_DISPLAY_BRI_ADD_OPA = 37;
static const char* SETTING_DISPLAY_BRI_ADD_BUTTON_ID = "add";

static constexpr int16_t SETTING_DISPLAY_BRI_SUB_X = 31;
static constexpr int16_t SETTING_DISPLAY_BRI_SUB_Y = 187;
static constexpr int16_t SETTING_DISPLAY_BRI_SUB_WIDTH = 80;
static constexpr int16_t SETTING_DISPLAY_BRI_SUB_HEIGHT = 80;
static constexpr int16_t SETTING_DISPLAY_BRI_SUB_OPA = 37;
static const char* SETTING_DISPLAY_BRI_SUB_BUTTON_ID = "sub";

void SettingDisplayBrightnessPage::OnStart(void* data)
{
    InitProgressView();
    InitButtonView();
    AddViewToPageContainer(title_);
    AddViewToPageContainer(progress_);
    AddViewToPageContainer(progressNum_);
    AddViewToPageContainer(add_);
    AddViewToPageContainer(sub_);
}

void SettingDisplayBrightnessPage::OnResume()
{
    char temp[SETTING_DISPLAY_TEMP_STR_LEN] = "";
    uint8_t brightness = SettingDisplayModel::GetInstance().GetBrightness();
    progress_->SetValue(brightness);
    if (sprintf_s(temp, sizeof(temp), "%u", brightness) > 0) {
        progressNum_->SetText(temp);
    }
}

void SettingDisplayBrightnessPage::OnPause()
{
    animator_->Stop();
}

void SettingDisplayBrightnessPage::OnStop()
{
    if (title_ != nullptr) {
        delete title_;
        title_ = nullptr;
    }
    if (progress_ != nullptr) {
        delete progress_;
        progress_ = nullptr;
    }
    if (progressNum_ != nullptr) {
        delete progressNum_;
        progressNum_ = nullptr;
    }
    if (add_ != nullptr) {
        delete add_;
        add_ = nullptr;
    }
    if (sub_ != nullptr) {
        delete sub_;
        sub_ = nullptr;
    }
    if (animator_ != nullptr) {
        delete animator_;
        animator_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadOneInMultiRes(IMAGE_DESKTOP_BRIGHTNESS_FG, SETTING_IMAGE);
    ImageCacheManager::GetInstance().UnloadOneInMultiRes(IMAGE_DISPLAY_BRIGHTNESS_ADD, SETTING_IMAGE);
    ImageCacheManager::GetInstance().UnloadOneInMultiRes(IMAGE_DISPLAY_BRIGHTNESS_SUB, SETTING_IMAGE);
}

bool SettingDisplayBrightnessPage::OnClick(UIView &view, const ClickEvent &event)
{
    char temp[SETTING_DISPLAY_TEMP_STR_LEN] = "";
    if (strcmp(view.GetViewId(), SETTING_DISPLAY_BRI_ADD_BUTTON_ID) == 0) {
        uint8_t brightness = SettingDisplayModel::GetInstance().GetBrightness();
        if (brightness != SETTING_DISPLAY_BRIGHTNESS_MAX) {
            brightness++;
            SettingDisplayModel::GetInstance().SetBrightness(brightness);
            progress_->SetValue(brightness);
            if (sprintf_s(temp, sizeof(temp), "%u", brightness) > 0) {
                progressNum_->SetText(temp);
            }
        }
    } else if (strcmp(view.GetViewId(), SETTING_DISPLAY_BRI_SUB_BUTTON_ID) == 0) {
        uint8_t brightness = SettingDisplayModel::GetInstance().GetBrightness();
        if (brightness != SETTING_DISPLAY_BRIGHTNESS_MIN) {
            brightness--;
            SettingDisplayModel::GetInstance().SetBrightness(brightness);
            progress_->SetValue(brightness);
            if (sprintf_s(temp, sizeof(temp), "%u", brightness) > 0) {
                progressNum_->SetText(temp);
            }
        }
    }
    return true;
}

bool SettingDisplayBrightnessPage::OnLongPress(UIView& view, const LongPressEvent& event)
{
    if (strcmp(view.GetViewId(), SETTING_DISPLAY_BRI_ADD_BUTTON_ID) == 0) {
        if (curLongPressButton_ == nullptr) {
            curLongPressButton_ = add_;
            animator_->Start();
        }
    } else if (strcmp(view.GetViewId(), SETTING_DISPLAY_BRI_SUB_BUTTON_ID) == 0) {
        if (curLongPressButton_ == nullptr) {
            curLongPressButton_ = sub_;
            animator_->Start();
        }
    }
    return true;
}

bool SettingDisplayBrightnessPage::OnRelease(UIView& view, const ReleaseEvent& event)
{
    animator_->Stop();
    curLongPressButton_ = nullptr;
    return true;
}

bool SettingDisplayBrightnessPage::OnDragEnd(UIView& view, const DragEvent& event)
{
    if (view.GetViewId() &&
        (strcmp(view.GetViewId(), SETTING_DISPLAY_BRI_ADD_BUTTON_ID) == 0 ||
        strcmp(view.GetViewId(), SETTING_DISPLAY_BRI_SUB_BUTTON_ID) == 0)) {
        animator_->Stop();
        curLongPressButton_ = nullptr;
    }
    return false;
}

void SettingDisplayBrightnessPage::Callback(UIView* view)
{
    static uint32_t lastRunTime = 0;
    uint32_t currRunTime = animator_->GetRunTime();
    if (currRunTime - lastRunTime < SETTING_DISPLAY_BRIGHTNESS_AUTO_TIME) {
        return;
    }
    lastRunTime = currRunTime;
    char temp[SETTING_DISPLAY_TEMP_STR_LEN] = "";
    uint8_t brightness = SettingDisplayModel::GetInstance().GetBrightness();
    if (curLongPressButton_ == add_ && brightness != SETTING_DISPLAY_BRIGHTNESS_MAX) {
        brightness++;
    } else if (curLongPressButton_ == sub_ && brightness != SETTING_DISPLAY_BRIGHTNESS_MIN) {
        brightness--;
    }
    SettingDisplayModel::GetInstance().SetBrightness(brightness);
    progress_->SetValue(brightness);
    if (sprintf_s(temp, sizeof(temp), "%u", brightness) > 0) {
        progressNum_->SetText(temp);
    }
}

void SettingDisplayBrightnessPage::InitProgressView()
{
    title_ = new UILabel();
    if (title_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingDisplayBrightness::InitProgressView title_ new fail");
        return;
    }
    title_->SetPosition(SETTING_DISPLAY_BRI_TITLE_X, SETTING_DISPLAY_BRI_TITLE_Y);
    title_->Resize(SETTING_DISPLAY_BRI_TITLE_WIDTH, SETTING_DISPLAY_BRI_TITLE_HEIGHT);
    title_->SetTextColor(Color::White());
    title_->SetFont(BOLD_VECTOR_FONT_FILENAME, SETTING_DISPLAY_BRI_TITLE_FSIZE);
    title_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    title_->SetText(SETTING_DISPLAY_BRI_TITLE_TEXT);

    progress_ = new UICircleProgress();
    if (progress_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingDisplayBrightness::InitProgressView progress_ new fail");
        return;
    }
    progress_->SetPosition(SETTING_DISPLAY_BRI_PROGRESS_X, SETTING_DISPLAY_BRI_PROGRESS_Y);
    progress_->Resize(SETTING_DISPLAY_BRI_PROGRESS_WIDTH, SETTING_DISPLAY_BRI_PROGRESS_HEIGHT);
    ImageInfo* fgImage = ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_DESKTOP_BRIGHTNESS_FG, SETTING_IMAGE);
    ImageInfo* bgImage = ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_DESKTOP_BRIGHTNESS_BG, SETTING_IMAGE);
    progress_->SetImage(fgImage, bgImage);
    progress_->SetBackgroundStyle(STYLE_LINE_WIDTH, SETTING_DISPLAY_BRI_PROGRESS_LINE_WIDTH);
    progress_->SetForegroundStyle(STYLE_LINE_WIDTH, SETTING_DISPLAY_BRI_PROGRESS_LINE_WIDTH);
    progress_->SetProgressImagePosition(0, 0);
    progress_->SetBackgroundImagePosition(0, 0);
    progress_->SetCenterPosition(SETTING_DISPLAY_BRI_PROGRESS_RADIUS, SETTING_DISPLAY_BRI_PROGRESS_RADIUS);
    progress_->SetRadius(SETTING_DISPLAY_BRI_PROGRESS_RADIUS);
    progress_->SetStartAngle(SETTING_DISPLAY_BRI_START_ANGLE);
    progress_->SetEndAngle(SETTING_DISPLAY_BRI_END_ANGLE);

    progressNum_ = new UILabel();
    if (progressNum_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingDisplayBrightness::InitProgressView progressNum_ new fail");
        return;
    }
    progressNum_->SetPosition(SETTING_DISPLAY_BRI_PROGRESSNUM_X, SETTING_DISPLAY_BRI_PROGRESSNUM_Y);
    progressNum_->Resize(SETTING_DISPLAY_BRI_PROGRESSNUM_WIDTH, SETTING_DISPLAY_BRI_PROGRESSNUM_HEIGHT);
    progressNum_->SetTextColor(Color::White());
    progressNum_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_DISPLAY_BRI_PROGRESSNUM_FSIZE);
    progressNum_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
}

void SettingDisplayBrightnessPage::InitButtonView()
{
    add_ = new UIButton();
    if (add_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingDisplayBrightness::InitButtonView add_ new fail");
        return;
    }
    add_->SetPosition(SETTING_DISPLAY_BRI_ADD_X, SETTING_DISPLAY_BRI_ADD_Y);
    add_->Resize(SETTING_DISPLAY_BRI_ADD_WIDTH, SETTING_DISPLAY_BRI_ADD_HEIGHT);
    ImageInfo* addImage =
        ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_DISPLAY_BRIGHTNESS_ADD, SETTING_IMAGE);
    add_->SetImageSrc(addImage, addImage);
    add_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::RELEASED);
    add_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    add_->SetStyleForState(STYLE_BACKGROUND_OPA, SETTING_DISPLAY_BRI_ADD_OPA, UIButton::RELEASED);
    add_->SetStyleForState(STYLE_BACKGROUND_OPA, SETTING_DISPLAY_BRI_ADD_OPA, UIButton::PRESSED);
    add_->SetViewId(SETTING_DISPLAY_BRI_ADD_BUTTON_ID);
    add_->SetDraggable(true);
    add_->SetOnClickListener(this);
    add_->SetOnTouchListener(this);
    add_->SetOnLongPressListener(this);
    add_->SetOnDragListener(this);
    add_->SetIntercept(true);

    sub_ = new UIButton();
    if (sub_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingDisplayBrightness::InitButtonView sub_ new fail");
        return;
    }
    sub_->SetPosition(SETTING_DISPLAY_BRI_SUB_X, SETTING_DISPLAY_BRI_SUB_Y);
    sub_->Resize(SETTING_DISPLAY_BRI_SUB_WIDTH, SETTING_DISPLAY_BRI_SUB_HEIGHT);
    ImageInfo* subImage =
        ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_DISPLAY_BRIGHTNESS_SUB, SETTING_IMAGE);
    sub_->SetImageSrc(subImage, subImage);
    sub_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::RELEASED);
    sub_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    sub_->SetStyleForState(STYLE_BACKGROUND_OPA, SETTING_DISPLAY_BRI_SUB_OPA, UIButton::RELEASED);
    sub_->SetStyleForState(STYLE_BACKGROUND_OPA, SETTING_DISPLAY_BRI_SUB_OPA, UIButton::PRESSED);
    sub_->SetViewId(SETTING_DISPLAY_BRI_SUB_BUTTON_ID);
    sub_->SetDraggable(true);
    sub_->SetOnClickListener(this);
    sub_->SetOnTouchListener(this);
    sub_->SetOnLongPressListener(this);
    sub_->SetOnDragListener(this);
    sub_->SetIntercept(true);

    animator_ = new Animator(this, nullptr, 0, true);
    if (animator_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingDisplayBrightness::InitButtonView animator_ new fail");
        return;
    }
}
}