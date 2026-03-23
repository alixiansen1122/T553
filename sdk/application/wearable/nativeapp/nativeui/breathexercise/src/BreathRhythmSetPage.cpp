/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BreathRhythmSetPage
 * Create: 2025-5-13
 */
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "UiConfig.h"
#include "gfx_utils/graphic_math.h"
#include "main/LoadImg.h"
#include "ui_resource_breath.h"
#include "wearable_log.h"
#include "breathexercise/BreathModel.h"
#include "breathexercise/BreathView.h"
#include "breathexercise/BreathRhythmSetPage.h"

namespace OHOS {
static constexpr char *BREATH_RHYTHM_BUTTON_OK_ID = "okButton";
static constexpr uint16_t BREATH_RHYTHM_TITLE_X = 100;
static constexpr uint16_t BREATH_RHYTHM_TITLE_Y = 38;
static constexpr uint16_t BREATH_RHYTHM_TITLE_W = 253;
static constexpr uint16_t BREATH_RHYTHM_TITLE_H = 53;
static constexpr uint16_t BREATH_RHYTHM_TITLE_FONT_SIZE = 38;
static constexpr uint16_t BREATH_RHYTHM_PICKER_X = 64;
static constexpr uint16_t BREATH_RHYTHM_PICKER_Y = 109;
static constexpr uint16_t BREATH_RHYTHM_PICKER_W = 321;
static constexpr uint16_t BREATH_RHYTHM_PICKER_H = 218;
static constexpr uint16_t BREATH_RHYTHM_PICKER_FONT_SIZE = 44;
static constexpr uint16_t BREATH_RHYTHM_PICKER_ITEM_H = 80;
static constexpr uint16_t BREATH_RHYTHM_OK_X = 61;
static constexpr uint16_t BREATH_RHYTHM_OK_Y = 362;
static constexpr uint16_t BREATH_RHYTHM_OK_W = 331;
static constexpr uint16_t BREATH_RHYTHM_OK_H = 98;
static constexpr uint16_t BREATH_GROUP_SIZE = 466;
static constexpr uint8_t PICKER_SIZE = 3;
const char *g_rhythmListData[PICKER_SIZE] = {"稍快 8次/分", "舒缓 7次/分", "缓慢 6次/分"};

REGIST_SLICE_PAGE(VIEW_BREATH, BREATH_PAGE::BREATH_RHY_VIEW, BreathRhythmSetPage, false);

BreathRhythmSetPage::BreathRhythmSetPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BreathRhythmSetPage::BreathRhythmSetPage");
}

BreathRhythmSetPage::~BreathRhythmSetPage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }
    if (lableRhythm_ != nullptr) {
        delete lableRhythm_;
        lableRhythm_ = nullptr;
    }
    if (pickerRhythm_ != nullptr) {
        delete pickerRhythm_;
        pickerRhythm_ = nullptr;
    }
    if (buttonOk_ != nullptr) {
        delete buttonOk_;
        buttonOk_ = nullptr;
    }
    if (pickerImg_ != nullptr) {
        delete pickerImg_;
        pickerImg_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(BREATH_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BreathRhythmSetPage::~BreathRhythmSetPage");
}

void BreathRhythmSetPage::OnStart(void *data)
{
    group_ = new UIViewGroup();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new group_ ERROR");
        return;
    }
    group_->SetPosition(0, 0);
    group_->Resize(BREATH_GROUP_SIZE, BREATH_GROUP_SIZE);
    group_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    group_->SetTouchable(false);
    group_->SetDraggable(false);

    lableRhythm_ = new UILabel();
    if (lableRhythm_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new lableRhythm_ ERROR");
        return;
    }
    lableRhythm_->SetPosition(BREATH_RHYTHM_TITLE_X, BREATH_RHYTHM_TITLE_Y);
    lableRhythm_->Resize(BREATH_RHYTHM_TITLE_W, BREATH_RHYTHM_TITLE_H);
    lableRhythm_->SetText("节奏设置");
    lableRhythm_->SetTextColor(Color::White());
    lableRhythm_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    lableRhythm_->SetFont(BOLD_VECTOR_FONT_FILENAME, BREATH_RHYTHM_TITLE_FONT_SIZE);
    group_->Add(lableRhythm_);

    pickerRhythm_ = new UIPicker();
    if (pickerRhythm_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new pickerRhythm_ ERROR");
        return;
    }
    pickerRhythm_->SetPosition(BREATH_RHYTHM_PICKER_X, BREATH_RHYTHM_PICKER_Y);
    pickerRhythm_->Resize(BREATH_RHYTHM_PICKER_W, BREATH_RHYTHM_PICKER_H);
    pickerRhythm_->SetBackgroundFont(DEFAULT_VECTOR_FONT_FILENAME, BREATH_RHYTHM_PICKER_FONT_SIZE);
    pickerRhythm_->SetHighlightFont(DEFAULT_VECTOR_FONT_FILENAME, BREATH_RHYTHM_PICKER_FONT_SIZE);
    pickerRhythm_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    pickerRhythm_->SetItemHeight(BREATH_RHYTHM_PICKER_ITEM_H);
    pickerRhythm_->SetTextColor(Color::GetColorFromRGBA(0xfa, 0xf3, 0xf3, 0xff),
                                Color::GetColorFromRGBA(0xfa, 0xf3, 0xf3, 0xff));
    uint8_t selectPick = BreathModel::GetInstance().GetRhythm();
    pickerRhythm_->SetSelected(selectPick);
    pickerRhythm_->RegisterSelectedListener(this);
    pickerRhythm_->SetVisible(true);
    pickerRhythm_->SetOnDragListener(this);
    pickerRhythm_->SetLoopState(false);
    group_->Add(pickerRhythm_);

    pickerImg_ = new UIImageView();
    if (pickerImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new Center ERROR");
        return;
    }
    pickerImg_->SetPosition(BREATH_RHYTHM_PICKER_X, BREATH_RHYTHM_PICKER_Y);
    pickerImg_->Resize(BREATH_RHYTHM_PICKER_W, BREATH_RHYTHM_PICKER_H);
    LOADIMG::LoadImageViewImg(pickerImg_, BREATH_IMAGE, IMAGE_BREATH_CENTER);
    group_->Add(pickerImg_);

    buttonOk_ = new UIButton();
    if (buttonOk_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new buttonOk_ ERROR");
        return;
    }
    buttonOk_->SetPosition(BREATH_RHYTHM_OK_X, BREATH_RHYTHM_OK_Y);
    buttonOk_->Resize(BREATH_RHYTHM_OK_W, BREATH_RHYTHM_OK_H);
    buttonOk_->SetViewId(BREATH_RHYTHM_BUTTON_OK_ID);
    buttonOk_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    LOADIMG::LoadBtnImage(buttonOk_, BREATH_IMAGE, IMAGE_BREATH_OK, IMAGE_BREATH_OK);
    buttonOk_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    buttonOk_->SetOnClickListener(this);
    group_->Add(buttonOk_);

    AddViewToPageContainer(group_);
}

void BreathRhythmSetPage::OnResume()
{
    pickerRhythm_->SetValues(g_rhythmListData, PICKER_SIZE);
    GetSlicePageContainer()->SetIntercept(true);
    GetSlicePageContainer()->SetOnDragListener(nullptr);
}

bool BreathRhythmSetPage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), BREATH_RHYTHM_BUTTON_OK_ID) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(BREATH_PAGE::BREATH_MAIN_VIEW,
            TransitionType::TRANSITION_INVALID, false);
        BreathModel::GetInstance().SetRhythm(pickerRhythm_->GetSelected());
    }
    return true;
}

bool BreathRhythmSetPage::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().SwitchPageInSlice(BREATH_PAGE::BREATH_MAIN_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}
}