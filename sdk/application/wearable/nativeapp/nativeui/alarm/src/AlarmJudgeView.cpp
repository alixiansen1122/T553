/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Alarm
 * Author:
 * Create: 2021-09-11
 */

#include "alarm/AlarmJudgeView.h"
#include "UiConfig.h"
#include <string>
#include "alarm/AlarmPresenter.h"

namespace OHOS {
#define DELETE_INETERFACES "是否删除此闹钟？"

static AlarmJudgeView *g_JudgeView = nullptr;
constexpr int16_t CONTAINER_X = 0;
constexpr int16_t CONTAINER_Y = 0;
constexpr int16_t MAX_LABEL_X_LEN = 120;
constexpr int16_t MAX_LABEL_Y_LEN = 160;
constexpr int16_t MAX_LABEL_LENGTH_LEN = 400;
constexpr int16_t MAX_LABEL_WIDTH_LEN = 50;
constexpr int16_t MAX_LABEL_FONT_LEN = 30;
constexpr int16_t MAX_IMAGE_DELETE_X_LEN = 90;
constexpr int16_t MAX_IMAGE_DELETE_Y_LEN = 300;
constexpr int16_t MAX_IMAGE_TRUE_X_LEN = 270;
constexpr int16_t MAX_IMAGE_TRUE_Y_LEN = 300;
constexpr int16_t MAX_IMAGE_DELETE_WIDTH_LEN = 91;
constexpr int16_t MAX_IMAGE_TRUE_WIDTH_LEN = 91;

AlarmJudgeView::AlarmJudgeView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmJudgeView::AlarmJudgeView");
    g_JudgeView = this;
}

AlarmJudgeView::~AlarmJudgeView()
{
    container->RemoveAll();
    delete labelText;
    labelText = nullptr;
    delete imageViewDelete;
    imageViewDelete = nullptr;
    delete container;
    container = nullptr;
    g_JudgeView = nullptr;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmJudgeView::~AlarmJudgeView");
}

void AlarmJudgeView::CreateImage(void)
{
    UIView::OnClickListener *clickListener = (UIView::OnClickListener *)OHOS::AlarmPresenter::GetInstance();
    if (clickListener == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmJudgeView:: new OnClickListener fail");
        return;
    }
    imageViewDelete = new UIImageView();
    if (imageViewDelete == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmJudgeView:: new imageViewDelete fail");
        return;
    }
    imageViewDelete->SetPosition(MAX_IMAGE_DELETE_X_LEN, MAX_IMAGE_DELETE_Y_LEN);
    imageViewDelete->SetSrc(ALARM_DELETE_ADD_PATH);
    imageViewDelete->SetViewId(JUDGE_DELETE);
    imageViewDelete->SetWidth(MAX_IMAGE_DELETE_WIDTH_LEN);
    imageViewDelete->SetTouchable(true);
    imageViewDelete->SetOnClickListener(clickListener);
    container->Add(imageViewDelete);

    imageViewTrue = new UIImageView();
    if (imageViewTrue == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmJudgeView:: new imageViewTrue fail");
        return;
    }
    imageViewTrue->SetPosition(MAX_IMAGE_TRUE_X_LEN, MAX_IMAGE_TRUE_Y_LEN); // 50 : size
    imageViewTrue->SetSrc(ALARM_TRUE_ADD_PATH);
    imageViewTrue->SetViewId(JUDGE_TRUE);
    imageViewTrue->SetWidth(MAX_IMAGE_TRUE_WIDTH_LEN);
    imageViewTrue->SetTouchable(true);
    imageViewTrue->SetOnClickListener(clickListener);
    container->Add(imageViewTrue);
}

UIScrollView *AlarmJudgeView::InitJudgeView(void)
{
    container = new UIScrollView();
    if (container == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmJudgeView:: new container fail");
        return nullptr;
    }
    container->SetPosition(CONTAINER_X, CONTAINER_Y);
    container->SetWidth(HORIZONTAL_RESOLUTION);
    container->SetHeight(VERTICAL_RESOLUTION);

    labelText = new UILabel();
    if (container == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmJudgeView:: new labelText fail");
        return nullptr;
    }
    labelText->SetPosition(MAX_LABEL_X_LEN, MAX_LABEL_Y_LEN, MAX_LABEL_LENGTH_LEN, MAX_LABEL_WIDTH_LEN);
    labelText->SetText(DELETE_INETERFACES);
    labelText->SetFont(DEFAULT_VECTOR_FONT_FILENAME, MAX_LABEL_FONT_LEN);
    container->Add(labelText);

    CreateImage();
    return container;
}
}