/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BloodResultPage
 * Create: 2025-06
 */

#include <string>
#include <random>
#include <chrono>
#include <iostream>
#include "ui_test_group.h"
#include "main/LoadImg.h"
#include "ui_resource_blood.h"
#include "msg_center_device.h"
#include "blood/BloodPresenter.h"
#include "blood/BloodView.h"
#include "blood/BloodResultPage.h"

namespace OHOS {
static constexpr uint16_t BLOOD_BUTTON_POSITION_X = 120;
static constexpr uint16_t BLOOD_BUTTON_POSITION_Y = 320;
static constexpr uint16_t BLOOD_BUTTON_WIDTH = 232;
static constexpr uint16_t BLOOD_BUTTON_HEIGHT = 76;
static constexpr uint16_t BLOOD_BUTTON_FONT_SIZE = 35;
static constexpr uint16_t BLOOD_LABEL_FRONT = 30;
static constexpr uint16_t BLOOD_LABEL_ONE_POSITION_X = 170;
static constexpr uint16_t BLOOD_LABEL_ONE_POSITION_Y = 150;
static constexpr uint16_t BLOOD_LABEL_ONE_POSITION_WIDTH = 72;
static constexpr uint16_t BLOOD_LABEL_ONE_POSITION_HEIGHT = 32;
static constexpr uint16_t BLOOD_LABEL_ONE_WIDTH = 200;
static constexpr uint16_t BLOOD_LABEL_ONE_HEIGHT = 60;
static constexpr uint16_t BLOOD_LABEL_FRONT_ONE = 60;
static constexpr uint16_t BLOOD_LABEL2_POSITION_X = 117;
static constexpr uint16_t BLOOD_LABEL2_POSITION_Y = 212;
static constexpr uint16_t BLOOD_LABEL3_POSITION_Y = 212;
static constexpr uint16_t BLOOD_LABEL2_WIDTH = 200;
static constexpr uint16_t BLOOD_LABEL2_HEIGHT = 60;
static constexpr uint16_t BLOOD_TITLE_POSITION_X = 195;
static constexpr uint16_t BLOOD_TITLE_POSITION_Y = 40;
static constexpr uint16_t BLOOD_TITLE_WIDTH = 260;
static constexpr uint16_t BLOOD_TITLE_HEIGHT = 53;
static constexpr uint16_t BLOOD_TITLE_FONT_SIZE = 38;
static constexpr uint16_t BLOOD_BUTTONLEVEL_POSITION_X = 277;
static constexpr uint16_t BLOOD_RANDOM_NUMBER_HIGH = 90;
static constexpr uint16_t BLOOD_RANDOM_NUMBER_LOW = 70;
static constexpr uint16_t BLOOD_OPA = 50;
static constexpr char *BLOOD_BUTTON_MORE = "moreButton";

REGIST_SLICE_PAGE(VIEW_BLOOD, BLOOD_PAGES::BLOOD_RESULT_PAGE, BloodResultPage, false);

BloodResultPage::BloodResultPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BloodResultPage::BloodResultPage");
}

BloodResultPage::~BloodResultPage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }

    if (buttonMore_ != nullptr) {
        delete buttonMore_;
        buttonMore_ = nullptr;
    }

    if (labelData_ != nullptr) {
        delete labelData_;
        labelData_ = nullptr;
    }

    if (labelBloodOxygen_ != nullptr) {
        delete labelBloodOxygen_;
        labelBloodOxygen_ = nullptr;
    }

    if (mainImg_ != nullptr) {
        delete mainImg_;
        mainImg_ = nullptr;
    }

    if (labelTitle_ != nullptr) {
        delete labelTitle_;
        labelTitle_ = nullptr;
    }

    if (levelImg_ != nullptr) {
        delete levelImg_;
        levelImg_ = nullptr;
    }

    ImageCacheManager::GetInstance().UnloadSingleRes(BLOOD_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BloodResultPage::~BloodResultPage");
}

void BloodResultPage::OnStart(void *data)
{
    group_ = new UIViewGroup();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new group_ fail");
        return;
    }
    group_->SetPosition(0, 0);
    group_->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    group_->SetOnDragListener(this);
    group_->SetDraggable(true);
    group_->SetTouchable(true);

    mainImg_ = new UIImageView();
    if (mainImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new mainImg_ fail");
        return;
    }
    mainImg_->SetPosition(0, 0);
    mainImg_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    LOADIMG::LoadImageViewImg(mainImg_, BLOOD_IMAGE, IMAGE_BLOOD_BG);
    group_->Add(mainImg_);

    labelTitle_ = new UILabel();
    if (labelTitle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelTitle_ fail");
        return;
    }
    labelTitle_->SetPosition(BLOOD_TITLE_POSITION_X, BLOOD_TITLE_POSITION_Y, BLOOD_TITLE_WIDTH, BLOOD_TITLE_HEIGHT);
    labelTitle_->SetText("结果");
    labelTitle_->SetFont(BOLD_VECTOR_FONT_FILENAME, BLOOD_TITLE_FONT_SIZE);
    labelTitle_->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    labelTitle_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelTitle_->SetRollSpeed(0);

    labelData_ = new UILabel();
    if (labelData_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelData_ fail");
        return;
    }
    labelData_->SetPosition(BLOOD_LABEL_ONE_POSITION_X, BLOOD_LABEL_ONE_POSITION_Y);
    labelData_->Resize(BLOOD_LABEL_ONE_WIDTH, BLOOD_LABEL_ONE_HEIGHT);
    labelData_->SetStyle(STYLE_TEXT_FONT, BLOOD_LABEL_FRONT_ONE);
    labelData_->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    labelData_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelData_->SetRollSpeed(0);
    labelData_->SetFont(BOLD_VECTOR_FONT_FILENAME, BLOOD_LABEL_FRONT_ONE);

    labelBloodOxygen_ = new UILabel();
    if (labelBloodOxygen_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelBloodOxygen_ fail");
        return;
    }
    labelBloodOxygen_->SetPosition(BLOOD_LABEL2_POSITION_X, BLOOD_LABEL2_POSITION_Y);
    labelBloodOxygen_->Resize(BLOOD_LABEL2_WIDTH, BLOOD_LABEL2_HEIGHT);
    labelBloodOxygen_->SetStyle(STYLE_TEXT_FONT, BLOOD_LABEL_FRONT);
    labelBloodOxygen_->SetText("血氧饱和度");
    labelBloodOxygen_->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    labelBloodOxygen_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelBloodOxygen_->SetRollSpeed(0);
    labelBloodOxygen_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BLOOD_LABEL_FRONT);
    labelBloodOxygen_->SetVisible(true);

    buttonMore_ = new UILabelButton();
    if (buttonMore_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new buttonMore_ fail");
        return;
    }
    buttonMore_->SetPosition(BLOOD_BUTTON_POSITION_X, BLOOD_BUTTON_POSITION_Y);
    buttonMore_->Resize(BLOOD_BUTTON_WIDTH, BLOOD_BUTTON_HEIGHT);
    buttonMore_->SetViewId(BLOOD_BUTTON_MORE);
    buttonMore_->SetOnClickListener(this);
    buttonMore_->SetStyleForState(STYLE_BACKGROUND_COLOR, 0x14FFFFFF, UIButton::RELEASED);
    buttonMore_->SetStyleForState(STYLE_BACKGROUND_COLOR, 0x14FFFFFF, UIButton::PRESSED);
    buttonMore_->SetStyleForState(STYLE_BACKGROUND_COLOR, 0x14FFFFFF, UIButton::INACTIVE);
    buttonMore_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BLOOD_BUTTON_FONT_SIZE);
    buttonMore_->SetText("进一步了解");
    buttonMore_->SetStyle(STYLE_TEXT_COLOR, 0xFFCCCCCC);
    buttonMore_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    buttonMore_->SetVisible(true);

    levelImg_ = new UIImageView();
    if (levelImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new levelImg_ fail");
        return;
    }
    levelImg_->SetPosition(BLOOD_BUTTONLEVEL_POSITION_X, BLOOD_LABEL3_POSITION_Y);
    bloodMeasurementInfo info = BloodModel::GetInstance().GetBlood();
    std::string strBlood = std::to_string(info.blood) + "%";
    labelData_->SetText(strBlood.c_str());
    errcode_t result;
#ifdef _WIN32
    result = 0;
#else
    result = msg_center_send_data(MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_DEVICE_BLOODOXYGEN, &info.blood,
                                  sizeof(info.blood));
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "send blood result:%d ", result);
#endif
    if (info.blood >= BLOOD_RANDOM_NUMBER_HIGH) {
        LOADIMG::LoadImageViewImg(levelImg_, BLOOD_IMAGE, IMAGE_BLOOD_NORMAL);
    } else if (info.blood >= BLOOD_RANDOM_NUMBER_LOW && info.blood < BLOOD_RANDOM_NUMBER_HIGH) {
        LOADIMG::LoadImageViewImg(levelImg_, BLOOD_IMAGE, IMAGE_BLOOD_COMMON);
    } else {
        LOADIMG::LoadImageViewImg(levelImg_, BLOOD_IMAGE, IMAGE_BLOOD_ABNORMAL);
    }

    group_->Add(labelTitle_);
    group_->Add(labelData_);
    group_->Add(labelBloodOxygen_);
    group_->Add(buttonMore_);
    group_->Add(levelImg_);
    AddViewToPageContainer(group_);
}

bool BloodResultPage::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if (event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) {
        NativeAbility::GetInstance().SwitchPageInSlice(BLOOD_PAGES::BLOOD_MAIN_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    } else if (event.GetDragDirection() == DragEvent::DIRECTION_BOTTOM_TO_TOP) {
        NativeAbility::GetInstance().SwitchPageInSlice(BLOOD_PAGES::BLOOD_CHART_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool BloodResultPage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), BLOOD_BUTTON_MORE) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(BLOOD_PAGES::BLOOD_EXPLAIN_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    UNUSED(event);
    return true;
}

}
