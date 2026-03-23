/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingSystemOperationPage
 * Create: 2025-06-01
 */
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "NativeAbility.h"
#include "settings/model/SettingSystemModel.h"
#include "settings/page/SettingSystemOperationPage.h"

namespace OHOS {
REGIST_SLICE_PAGE(VIEW_SETTING, SettingPageId::SYSTEM_OPERATION_PAGE, SettingSystemOperationPage, false);

#define SYSTEM_SLIDER_LABEL_TEXT_SIZE 20
#define SETTING_SYSTEM_OPERATION_POWEROFF_FG_COLOR Color::GetColorFromRGB(250, 60, 60)  // 0xFA3C3CFF
#define SETTING_SYSTEM_OPERATION_RESTART_FG_COLOR Color::GetColorFromRGB(100, 206, 122)  // 0x6ACE7AFF
#define SETTING_SYSTEM_OPERATION_RESUME_FG_COLOR Color::GetColorFromRGB(153, 153, 153)  // 0x999999FF

static constexpr int16_t SETTING_SYSTEM_OPERATION_SLIDER_X = 55;
static constexpr int16_t SETTING_SYSTEM_OPERATION_SLIDER_Y = 135;
static constexpr int16_t SETTING_SYSTEM_OPERATION_SLIDER_WIDTH = 349;
static constexpr int16_t SETTING_SYSTEM_OPERATION_SLIDER_HEIGHT = 96;
static constexpr int16_t SETTING_SYSTEM_OPERATION_SLIDER_RADIUS = 88;
static constexpr int16_t SETTING_SYSTEM_OPERATION_SLIDER_KNOBWIDTH = 84;
static constexpr int16_t SETTING_SYSTEM_OPERATION_SLIDER_RANGE_MAX = 349;
static constexpr int16_t SETTING_SYSTEM_OPERATION_SLIDER_RANGE_MIN = 0;
static constexpr int16_t SETTING_SYSTEM_OPERATION_SLIDER_START_POINT = 50;
static constexpr int16_t SETTING_SYSTEM_OPERATION_SLIDER_START_RANG_MAX = 110;
static constexpr int16_t SETTING_SYSTEM_OPERATION_SLIDER_START_RANG_MIN = 10;
static constexpr int16_t SETTING_SYSTEM_OPERATION_SLIDER_END_POINT = 299;
static constexpr int16_t SETTING_SYSTEM_OPERATION_SLIDER_RIGHT_POINT = 249;
static constexpr int16_t SETTING_SYSTEM_OPERATION_SLIDE_BORDER_WIDTH = 16;
static constexpr int16_t SETTING_SYSTEM_OPERATION_SLIDE_FG_OPA = 128;

static constexpr int16_t SETTING_SYSTEM_OPERATION_SLIDER_LABEL_X = 177;
static constexpr int16_t SETTING_SYSTEM_OPERATION_SLIDER_LABEL_Y = 164;
static constexpr int16_t SETTING_SYSTEM_OPERATION_SLIDER_LABEL_WIDTH = 192;
static constexpr int16_t SETTING_SYSTEM_OPERATION_SLIDER_LABEL_HEIGHT = 38;
static constexpr int16_t SETTING_SYSTEM_OPERATION_SLIDER_LABEL_FSIZE = 32;

static constexpr int16_t SETTING_SYSTEM_OPERATION_BUTTON_X = 184;
static constexpr int16_t SETTING_SYSTEM_OPERATION_BUTTON_Y = 294;
static constexpr int16_t SETTING_SYSTEM_OPERATION_BUTTON_WIDTH = 85;
static constexpr int16_t SETTING_SYSTEM_OPERATION_BUTTON_HEIGHT = 85;
static constexpr int16_t SETTING_SYSTEM_OPERATION_BUTTON_RADIUS = 45;

static constexpr int16_t SETTING_SYSTEM_OPERATION_BUTTON_LABEL_X = 194;
static constexpr int16_t SETTING_SYSTEM_OPERATION_BUTTON_LABEL_Y = 394;
static constexpr int16_t SETTING_SYSTEM_OPERATION_BUTTON_LABEL_WIDTH = 64;
static constexpr int16_t SETTING_SYSTEM_OPERATION_BUTTON_LABEL_HEIGHT = 38;
static constexpr int16_t SETTING_SYSTEM_OPERATION_BUTTON_LABEL_FSIZE = 32;
static const char* SETTING_SYSTEM_OPERATION_BUTTON_LABEL_TEXT = "取消";

SettingSystemOperationPage::SettingSystemOperationPage()
{
    std::map<std::string, OperationData> tempData = {
        {"关机", {SETTING_SYSTEM_OPERATION_POWEROFF_FG_COLOR, IMAGE_SYSTEM_LIST_POWEROFF}},
        {"重启", {SETTING_SYSTEM_OPERATION_RESTART_FG_COLOR, IMAGE_SYSTEM_LIST_RESTART}},
        {"恢复出厂", {SETTING_SYSTEM_OPERATION_RESUME_FG_COLOR, IMAGE_SYSTEM_LIST_RESUME}},
    };
    operationData_.swap(tempData);
}

void SettingSystemOperationPage::OnStart(void* data)
{
    operationName_ = static_cast<const char*>(data);
    InitSliderView();
    InitButtonView();
    RefreshSlider();
    AddViewToPageContainer(slider_);
    AddViewToPageContainer(button_);
    AddViewToPageContainer(sliderLabel_);
    AddViewToPageContainer(buttonDescription_);
}

void SettingSystemOperationPage::OnResume()
{
    GetSlicePageContainer()->SetIntercept(true);
    GetSlicePageContainer()->SetOnDragListener(nullptr); // 取消跟手返回
    if (sliderLabel_ != nullptr) {
        char temp[SYSTEM_SLIDER_LABEL_TEXT_SIZE];
        if (sprintf_s(temp, sizeof(temp), "滑动%s", operationName_.c_str()) > 0) {
            sliderLabel_->SetText(temp);
        }
    }
}

void SettingSystemOperationPage::OnStop()
{
    if (slider_ != nullptr) {
        delete slider_;
        slider_ = nullptr;
    }
    if (sliderLabel_ != nullptr) {
        delete sliderLabel_;
        sliderLabel_ = nullptr;
    }
    if (button_ != nullptr) {
        delete button_;
        button_ = nullptr;
    }
    if (buttonDescription_ != nullptr) {
        delete buttonDescription_;
        buttonDescription_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadOneInMultiRes(operationData_[operationName_].sliderKnobImage, SETTING_IMAGE);
    ImageCacheManager::GetInstance().UnloadOneInMultiRes(IMAGE_SYSTEM_OPERATION_CLOSE, SETTING_IMAGE);
}

bool SettingSystemOperationPage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), "slider") == 0) {
        // Slide的click需要视为无效点击
        slider_->SetValue(SETTING_SYSTEM_OPERATION_SLIDER_START_POINT);
    } else if (strcmp(view.GetViewId(), "button") == 0) {
        NativeAbility::GetInstance().BackToPrevSlicePage();
    }
    return true;
}

void SettingSystemOperationPage::OnChange(int32_t value)
{
    // 若滑动开始的坐标在这个区域则允许滑动
    if (value > SETTING_SYSTEM_OPERATION_SLIDER_START_RANG_MIN &&
        value < SETTING_SYSTEM_OPERATION_SLIDER_START_RANG_MAX) {
        sliderBegin_ = true;
    }
    // 若起点不在指定区域 或 往不允许的滑动区域滑动, 则重置为区域起点
    if (!sliderBegin_ || value < SETTING_SYSTEM_OPERATION_SLIDER_START_POINT) {
        slider_->SetValue(SETTING_SYSTEM_OPERATION_SLIDER_START_POINT);
    }
    // 若起点在指定区域, 终点超过允许的滑动区域, 则重置为区域终点
    if (sliderBegin_ && value > SETTING_SYSTEM_OPERATION_SLIDER_END_POINT) {
        slider_->SetValue(SETTING_SYSTEM_OPERATION_SLIDER_END_POINT);
    }
    // 按照滑动坐标修改label的透明度 滑动坐标[50, 299] label透明度[255, 6]
    uint8_t labelOPA = OPA_OPAQUE - (slider_->GetValue() - SETTING_SYSTEM_OPERATION_SLIDER_START_POINT);
    sliderLabel_->SetStyle(STYLE_TEXT_OPA, labelOPA);
}

void SettingSystemOperationPage::OnRelease(int32_t value)
{
    if (sliderBegin_ && value > SETTING_SYSTEM_OPERATION_SLIDER_RIGHT_POINT) {
        SettingSystemModel::GetInstance().SystemOperation(operationName_.c_str());
    }
    slider_->SetValue(SETTING_SYSTEM_OPERATION_SLIDER_START_POINT);
    sliderLabel_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    sliderBegin_ = false;
}

void SettingSystemOperationPage::InitSliderView()
{
    slider_ = new UISlider();
    if (slider_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingSystemOperationPage::InitSliderView slider_ new fail");
        return;
    }
    slider_->SetPosition(SETTING_SYSTEM_OPERATION_SLIDER_X, SETTING_SYSTEM_OPERATION_SLIDER_Y);
    slider_->Resize(SETTING_SYSTEM_OPERATION_SLIDER_WIDTH, SETTING_SYSTEM_OPERATION_SLIDER_HEIGHT);
    slider_->SetViewId("slider");
    slider_->SetSliderRadius(SETTING_SYSTEM_OPERATION_SLIDER_RADIUS, SETTING_SYSTEM_OPERATION_SLIDER_RADIUS);
    slider_->SetKnobWidth(SETTING_SYSTEM_OPERATION_SLIDER_KNOBWIDTH);
    slider_->SetRange(SETTING_SYSTEM_OPERATION_SLIDER_RANGE_MAX, SETTING_SYSTEM_OPERATION_SLIDER_RANGE_MIN);
    slider_->SetValue(SETTING_SYSTEM_OPERATION_SLIDER_START_POINT);
    slider_->SetSliderEventListener(this);
    slider_->SetOnClickListener(this);
    slider_->SetIntercept(true);

    sliderLabel_ = new UILabel();
    if (sliderLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingSystemOperationPage::InitSliderView sliderlabel new fail");
        return;
    }
    sliderLabel_->SetPosition(SETTING_SYSTEM_OPERATION_SLIDER_LABEL_X, SETTING_SYSTEM_OPERATION_SLIDER_LABEL_Y);
    sliderLabel_->Resize(SETTING_SYSTEM_OPERATION_SLIDER_LABEL_WIDTH, SETTING_SYSTEM_OPERATION_SLIDER_LABEL_HEIGHT);
    sliderLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    sliderLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_SYSTEM_OPERATION_SLIDER_LABEL_FSIZE);
    sliderLabel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    sliderLabel_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    sliderLabel_->SetStyle(STYLE_BACKGROUND_COLOR, SETTING_LIST_DEFAULT_BG_COLOR.full);
}

void SettingSystemOperationPage::InitButtonView()
{
    button_ = new UIButton();
    if (button_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingSystemOperationPage::InitButtonView button_ new fail");
        return;
    }
    button_->SetPosition(SETTING_SYSTEM_OPERATION_BUTTON_X, SETTING_SYSTEM_OPERATION_BUTTON_Y);
    button_->Resize(SETTING_SYSTEM_OPERATION_BUTTON_WIDTH, SETTING_SYSTEM_OPERATION_BUTTON_HEIGHT);
    button_->SetViewId("button");
    ImageInfo* image = ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_SYSTEM_OPERATION_CLOSE, SETTING_IMAGE);
    button_->SetImageSrc(image, image);
    button_->SetStyle(STYLE_BORDER_RADIUS, SETTING_SYSTEM_OPERATION_BUTTON_RADIUS);
    button_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    button_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::RELEASED);
    button_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::INACTIVE);
    button_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    button_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::RELEASED);
    button_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::INACTIVE);
    button_->SetOnClickListener(this);

    buttonDescription_ = new UILabel();
    if (buttonDescription_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingSystemOperationPage::InitButtonView buttonlabel new fail");
        return;
    }
    buttonDescription_->SetPosition(SETTING_SYSTEM_OPERATION_BUTTON_LABEL_X, SETTING_SYSTEM_OPERATION_BUTTON_LABEL_Y);
    buttonDescription_->Resize(SETTING_SYSTEM_OPERATION_BUTTON_LABEL_WIDTH, SETTING_SYSTEM_OPERATION_BUTTON_LABEL_HEIGHT);
    buttonDescription_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    buttonDescription_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_SYSTEM_OPERATION_BUTTON_LABEL_FSIZE);
    buttonDescription_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    buttonDescription_->SetText(SETTING_SYSTEM_OPERATION_BUTTON_LABEL_TEXT);
}

void SettingSystemOperationPage::RefreshSlider()
{
    slider_->SetBackgroundStyle(STYLE_BACKGROUND_COLOR, SETTING_LIST_DEFAULT_BG_COLOR.full);
    slider_->SetForegroundStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    slider_->SetBackgroundStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    slider_->SetForegroundStyle(STYLE_BORDER_OPA, OPA_TRANSPARENT);
    slider_->SetForegroundStyle(STYLE_BORDER_WIDTH, SETTING_SYSTEM_OPERATION_SLIDE_BORDER_WIDTH);
    slider_->SetForegroundStyle(STYLE_BACKGROUND_OPA, SETTING_SYSTEM_OPERATION_SLIDE_FG_OPA);

    ImageInfo* image = ImageCacheManager::GetInstance().LoadOneInMultiRes(
        operationData_[operationName_].sliderKnobImage, SETTING_IMAGE);
    slider_->SetKnobImage(image);
    slider_->SetForegroundStyle(STYLE_BACKGROUND_COLOR, operationData_[operationName_].sliderBgColor.full);
}
}