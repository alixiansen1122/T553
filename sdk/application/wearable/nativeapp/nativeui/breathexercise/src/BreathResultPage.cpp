/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BreathReult
 * Create: 2025-5-13
 */
#include <sstream>
#include <sys/time.h>
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "PageTransitionMgr.h"
#include "SlicePageFactory.h"
#include "TransitionType.h"
#include "UiConfig.h"
#include "components/root_view.h"
#include "gfx_utils/graphic_math.h"
#include "main/LoadImg.h"
#include "ui_resource_breath.h"
#include "wearable_log.h"
#ifdef BUILD_IN_LINUX
#include "localtime.h"
#endif
#include "breathexercise/BreathView.h"
#include "breathexercise/BreathResultPage.h"

namespace OHOS {
static constexpr char *BREATH_BUTTON_COMPLETE_ID = "completeButton";
static constexpr char *BREATH_BUTTON_RESTART_ID = "restartButton";
static constexpr uint16_t BREATH_RESULTS_X = 97;
static constexpr uint16_t BREATH_RESULTS_Y = 34;
static constexpr uint16_t BREATH_RESULTS_W = 260;
static constexpr uint16_t BREATH_RESULTS_H = 53;
static constexpr uint16_t BREATH_MAX_FONT_SIZE = 38;
static constexpr uint16_t BREATH_MIN_FONT_SIZE = 28;
static constexpr uint16_t BREATH_SCOUT_FONT_SIZE = 40;
static constexpr uint16_t BREATH_RHYTHM_X = 77;
static constexpr uint16_t BREATH_RHYTHM_Y = 83;
static constexpr uint16_t BREATH_RHYTHM_W = 300;
static constexpr uint16_t BREATH_RHYTHM_H = 30;
static constexpr uint16_t BREATH_SCORES_X = 241;
static constexpr uint16_t BREATH_SCORES_Y = 190;
static constexpr uint16_t BREATH_SCORES_W = 72;
static constexpr uint16_t BREATH_SCORES_H = 32;
static constexpr uint16_t BREATH_COMPLETE_X = 111;
static constexpr uint16_t BREATH_COMPLETE_Y = 730;
static constexpr uint16_t BREATH_RESTART_Y = 815;
static constexpr uint16_t BREATH_COMPLETE_W = 232;
static constexpr uint16_t BREATH_COMPLETE_H = 76;
static constexpr uint16_t BREATH_GUINEAS_SIZE = 100;
static constexpr uint16_t BREATH_NINETY_SIZE = 90;
static constexpr uint16_t BREATH_EIGHTY_NINE_SIZE = 89;
static constexpr uint16_t BREATH_EIGHTY_SIZE = 80;
static constexpr uint16_t BREATH_SEVENTY_NINE_SIZE = 79;
static constexpr uint16_t BREATH_SEVENTY_SIZE = 70;
static constexpr uint16_t BREATH_SIXTY_NINE_SIZE = 69;
static constexpr uint16_t BREATH_SIXTY_SIZE = 60;
static constexpr uint16_t TEXT_OPACITY = 147;
static constexpr uint16_t TEXT_OPACITY_TWO = 220;
static constexpr uint16_t INDEX_0 = 0;
static constexpr uint16_t INDEX_1 = 1;
static constexpr uint16_t INDEX_2 = 2;
static constexpr uint16_t INDEX_3 = 3;
static constexpr uint16_t INDEX_4 = 4;
static constexpr uint16_t BREATH_RESULT_POSY_OFFSET = 118;
static constexpr uint16_t BREATH_RESULT_BUTTON_BG_POSITION_X = 92;
static constexpr uint16_t BREATH_RESULT_BUTTON_BG_POSITION_Y = 132;
static constexpr uint16_t BREATH_RESULT_BUTTON_BG_WIDTH = 270;
static constexpr uint16_t BREATH_RESULT_BUTTON_BG_HEIGHT = 108;
static constexpr uint16_t BREATH_RESULT_LABEL_DES_POSITION_X = 112;
static constexpr uint16_t BREATH_RESULT_LABEL_DES_POSITION_Y = 142;
static constexpr uint16_t BREATH_RESULT_LABEL_DES_WIDTH = 200;
static constexpr uint16_t BREATH_RESULT_LABEL_DES_HEIGHT = 30;
static constexpr uint16_t BREATH_RESULT_LABEL_COUNT_POSITION_X = 112;
static constexpr uint16_t BREATH_RESULT_LABEL_COUNT_POSITION_Y = 182;
static constexpr uint16_t BREATH_RESULT_LABEL_COUNT_WIDTH = 50;
static constexpr uint16_t BREATH_RESULT_LABEL_COUNT_HEIGHT = 44;
static constexpr uint16_t BREATH_RESULT_LABEL_UNIT_POSITION_X = 112;
static constexpr uint16_t BREATH_RESULT_LABEL_UNIT_POSITION_Y = 192;
static constexpr uint16_t BREATH_RESULT_LABEL_UNIT_WIDTH = 70;
static constexpr uint16_t BREATH_RESULT_LABEL_UNIT_HEIGHT = 34;
static constexpr uint16_t BREATH_RESULT_BORDER_RADIUS = 8;

REGIST_SLICE_PAGE(VIEW_BREATH, BREATH_PAGE::BREATH_RESULT_VIEW, BreathResultPage, false);

BreathResultPage::BreathResultPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BreathResultPage::BreathResultPage");
}

BreathResultPage::~BreathResultPage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }

    if (labelTitle_ != nullptr) {
        delete labelTitle_;
        labelTitle_ = nullptr;
    }

    if (labelTimeAndRhythmTitle_ != nullptr) {
        delete labelTimeAndRhythmTitle_;
        labelTimeAndRhythmTitle_ = nullptr;
    }

    if (scores_ != nullptr) {
        delete scores_;
        scores_ = nullptr;
    }

    if (pageBgImg_ != nullptr) {
        delete pageBgImg_;
        pageBgImg_ = nullptr;
    }

    if (buttonComplete_ != nullptr) {
        delete buttonComplete_;
        buttonComplete_ = nullptr;
    }

    if (buttonRestart_ != nullptr) {
        delete buttonRestart_;
        buttonRestart_ = nullptr;
    }

    for (int i = 0; i < LABEL_COUNT; i++) {
        if (buttonBg_[i] != nullptr) {
            delete buttonBg_[i];
            buttonBg_[i] = nullptr;
        }
    }

    for (int i = 0; i < LABEL_COUNT; i++) {
        if (labelDes_[i] != nullptr) {
            delete labelDes_[i];
            labelDes_[i] = nullptr;
        }
    }

    for (int i = 0; i < LABEL_COUNT; i++) {
        if (labelValue_[i] != nullptr) {
            delete labelValue_[i];
            labelValue_[i] = nullptr;
        }
    }

    for (int i = 0; i < LABEL_COUNT; i++) {
        if (labelUnit_[i] != nullptr) {
            delete labelUnit_[i];
            labelUnit_[i] = nullptr;
        }
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(BREATH_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BreathResultPage::~BreathResultPage");
}

void BreathResultPage::OnStart(void *data)
{
    group_ = new UIScrollView();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new group_ ERROR");
        return;
    }

    group_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    group_->SetThrowDrag(true);
    group_->SetScrollBlankSize(BREATH_RESULTS_Y);
    group_->SetOnDragListener(this);
    group_->SetDraggable(true);
    group_->SetTouchable(true);
    group_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    labelTitle_ = new UILabel();
    if (labelTitle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelTitle_ ERROR");
        return;
    }
    labelTitle_->SetPosition(BREATH_RESULTS_X, BREATH_RESULTS_Y);
    labelTitle_->Resize(BREATH_RESULTS_W, BREATH_RESULTS_H);
    labelTitle_->SetText("训练结果");
    labelTitle_->SetTextColor(Color::White());
    labelTitle_->SetFont(BOLD_VECTOR_FONT_FILENAME, BREATH_MAX_FONT_SIZE);
    labelTitle_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    group_->Add(labelTitle_);

    labelTimeAndRhythmTitle_ = new UILabel();
    if (labelTimeAndRhythmTitle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new Rhythm ERROR");
        return;
    }
    labelTimeAndRhythmTitle_->SetPosition(BREATH_RHYTHM_X, BREATH_RHYTHM_Y);
    labelTimeAndRhythmTitle_->Resize(BREATH_RHYTHM_W, BREATH_RHYTHM_H);
    labelTimeAndRhythmTitle_->SetTextColor(Color::Green());
    labelTimeAndRhythmTitle_->SetFont(BOLD_VECTOR_FONT_FILENAME, BREATH_MIN_FONT_SIZE);
    labelTimeAndRhythmTitle_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    uint8_t durationTime = BreathModel::GetInstance().GetTrainTime() + 1;
    std::ostringstream oss;
    oss << std::to_string(durationTime) << " 分钟" << " | ";
    if (const char* rhythm = BreathModel::GetInstance().GetRhythmText()) {
        oss << rhythm;
    } else {
        oss << "缓慢";  // 默认值
    }
    
    std::string weatherHumidity = oss.str();
    labelTimeAndRhythmTitle_->SetText(weatherHumidity.c_str());
    group_->Add(labelTimeAndRhythmTitle_);

    BreathModel::GetInstance().SetFrequencyScore();
    BreathModel::GetInstance().SetHeartRate();
    BreathModel::GetInstance().SetTrainingDurationTime(BreathModel::GetInstance().GetTrainTime() + 1);
    BreathModel::GetInstance().SetTrainingCount();
    InitBtnBg();
    InitLabelDes();
    InitLabelCount();
    InitLabelUnit();
    InitBtn();
    pageBgImg_ = new UIImageView();
    if (pageBgImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new pageBgImg_ ERROR");
        return;
    }
    pageBgImg_->SetPosition(0, 0);
    LOADIMG::LoadImageViewImg(pageBgImg_, BREATH_IMAGE, IMAGE_BREATH_BG2);
    AddViewToPageContainer(pageBgImg_);
    AddViewToPageContainer(group_);
}

void BreathResultPage::InitBtn()
{
    buttonComplete_ = new UIButton();
    if (buttonComplete_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new buttonComplete_ ERROR");
        return;
    }
    buttonComplete_->SetPosition(BREATH_COMPLETE_X, BREATH_COMPLETE_Y);
    buttonComplete_->Resize(BREATH_COMPLETE_W, BREATH_COMPLETE_H);
    buttonComplete_->SetViewId(BREATH_BUTTON_COMPLETE_ID);
    buttonComplete_->SetOnClickListener(this);
    LOADIMG::LoadBtnImage(buttonComplete_, BREATH_IMAGE, IMAGE_BREATH_COMPLETE, IMAGE_BREATH_COMPLETE);
    buttonComplete_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    buttonComplete_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    group_->Add(buttonComplete_);

    buttonRestart_ = new UIButton();
    if (buttonRestart_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new buttonRestart_ ERROR");
        return;
    }
    buttonRestart_->SetPosition(BREATH_COMPLETE_X, BREATH_RESTART_Y);
    buttonRestart_->Resize(BREATH_COMPLETE_W, BREATH_COMPLETE_H);
    buttonRestart_->SetViewId(BREATH_BUTTON_RESTART_ID);
    buttonRestart_->SetOnClickListener(this);
    LOADIMG::LoadBtnImage(buttonRestart_, BREATH_IMAGE, IMAGE_BREATH_RETEST, IMAGE_BREATH_RETEST);
    buttonRestart_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    buttonRestart_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    group_->Add(buttonRestart_);
}

void BreathResultPage::InitBtnBg()
{
    for (int i = 0; i < LABEL_COUNT; i++) {
        buttonBg_[i] = new UIButton();
        if (buttonBg_[i] == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new button ERROR");
            return;
        }

        int posY = BREATH_RESULT_BUTTON_BG_POSITION_Y + BREATH_RESULT_POSY_OFFSET * i;
        buttonBg_[i]->SetPosition(BREATH_RESULT_BUTTON_BG_POSITION_X, posY, BREATH_RESULT_BUTTON_BG_WIDTH,
                                  BREATH_RESULT_BUTTON_BG_HEIGHT);
        buttonBg_[i]->SetStyleForState(STYLE_BACKGROUND_COLOR, 0X36FFFFFF, UIButton::PRESSED);
        buttonBg_[i]->SetStyleForState(STYLE_BACKGROUND_COLOR, 0X36FFFFFF, UIButton::INACTIVE);
        buttonBg_[i]->SetStyleForState(STYLE_BACKGROUND_COLOR, 0X36FFFFFF, UIButton::RELEASED);
        buttonBg_[i]->SetStyleForState(STYLE_BORDER_RADIUS, BREATH_RESULT_BORDER_RADIUS, UIButton::PRESSED);
        buttonBg_[i]->SetStyleForState(STYLE_BORDER_RADIUS, BREATH_RESULT_BORDER_RADIUS, UIButton::INACTIVE);
        buttonBg_[i]->SetStyleForState(STYLE_BORDER_RADIUS, BREATH_RESULT_BORDER_RADIUS, UIButton::RELEASED);
        group_->Add(buttonBg_[i]);
    }
}

void BreathResultPage::InitLabelDes()
{
    std::string strDes[LABEL_COUNT] = {"呼吸频率", "呼吸前心率", "训练时长", "今日累计时长", "今日累计次数"};
    for (int i = 0; i < LABEL_COUNT; i++) {
        labelDes_[i] = new UILabel();
        if (labelDes_[i] == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new button ERROR");
            return;
        }

        int posY = BREATH_RESULT_LABEL_DES_POSITION_Y + BREATH_RESULT_POSY_OFFSET * i;
        labelDes_[i]->SetPosition(BREATH_RESULT_LABEL_DES_POSITION_X, posY, BREATH_RESULT_LABEL_DES_WIDTH,
                                  BREATH_RESULT_LABEL_DES_HEIGHT);
        labelDes_[i]->SetText(strDes[i].c_str());
        labelDes_[i]->SetTextColor(Color::White());
        labelDes_[i]->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BREATH_MIN_FONT_SIZE);
        labelDes_[i]->SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
        labelDes_[i]->SetStyle(STYLE_TEXT_OPA, TEXT_OPACITY);
        group_->Add(labelDes_[i]);
    }
}

void BreathResultPage::InitLabelCount()
{
    for (int i = 0; i < LABEL_COUNT; i++) {
        labelValue_[i] = new UILabel();
        if (labelValue_[i] == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new button ERROR");
            return;
        }

        int posY = BREATH_RESULT_LABEL_COUNT_POSITION_Y + BREATH_RESULT_POSY_OFFSET * i;
        labelValue_[i]->SetPosition(BREATH_RESULT_LABEL_COUNT_POSITION_X, posY, BREATH_RESULT_LABEL_COUNT_WIDTH,
                                    BREATH_RESULT_LABEL_COUNT_HEIGHT);
        labelValue_[i]->SetText("20");
        labelValue_[i]->SetTextColor(Color::White());
        labelValue_[i]->SetFont(BOLD_VECTOR_FONT_FILENAME, BREATH_SCOUT_FONT_SIZE);
        labelValue_[i]->SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
        group_->Add(labelValue_[i]);
    }
    scores_ = new UIImageView();
    if (scores_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new Scores ERROR");
        return;
    }
    scores_->SetPosition(BREATH_SCORES_X, BREATH_SCORES_Y);
    scores_->Resize(BREATH_SCORES_W, BREATH_SCORES_H);
    group_->Add(scores_);
    uint8_t heartRate_ = BreathModel::GetInstance().GetHeartRate();
    uint8_t frequencyScore_ = BreathModel::GetInstance().GetFrequencyScore();
    if (frequencyScore_ >= BREATH_NINETY_SIZE && frequencyScore_ <= BREATH_GUINEAS_SIZE) {
        LOADIMG::LoadImageViewImg(scores_, BREATH_IMAGE, IMAGE_BREATH_EXCELLENT);
    } else if (frequencyScore_ >= BREATH_EIGHTY_SIZE && frequencyScore_ <= BREATH_EIGHTY_NINE_SIZE) {
        LOADIMG::LoadImageViewImg(scores_, BREATH_IMAGE, IMAGE_BREATH_GOOD);
    } else if (frequencyScore_ >= BREATH_SEVENTY_SIZE && frequencyScore_ <= BREATH_SEVENTY_NINE_SIZE) {
        LOADIMG::LoadImageViewImg(scores_, BREATH_IMAGE, IMAGE_BREATH_NORMAL);
    } else if (frequencyScore_ >= BREATH_SIXTY_SIZE && frequencyScore_ <= BREATH_SIXTY_NINE_SIZE) {
        LOADIMG::LoadImageViewImg(scores_, BREATH_IMAGE, IMAGE_BREATH_PASS);
    } else {
        LOADIMG::LoadImageViewImg(scores_, BREATH_IMAGE, IMAGE_BREATH_BAD);
    }
    labelValue_[INDEX_0]->SetText(std::to_string(heartRate_).c_str());
    labelValue_[INDEX_1]->SetText(std::to_string(frequencyScore_).c_str());
    int selectTime = BreathModel::GetInstance().GetTrainTime() + 1;
    labelValue_[INDEX_2]->SetText(std::to_string(selectTime).c_str());
    labelValue_[INDEX_3]->SetText(BreathModel::GetInstance().GetTrainingDurationTime().c_str());
    labelValue_[INDEX_4]->SetText(BreathModel::GetInstance().GetTrainingCount().c_str());
}

void BreathResultPage::InitLabelUnit()
{
    std::string strDes[LABEL_COUNT] = {"次/分", "次/分", "分", "分钟", "次"};
    for (int i = 0; i < LABEL_COUNT; i++) {
        labelUnit_[i] = new UILabel();
        if (labelUnit_[i] == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new button ERROR");
            return;
        }

        int offset = 10;
        int posY = BREATH_RESULT_LABEL_UNIT_POSITION_Y + BREATH_RESULT_POSY_OFFSET * i;
        int posX = BREATH_RESULT_LABEL_UNIT_POSITION_X + labelValue_[i]->GetTextWidth() + offset;
        labelUnit_[i]->SetPosition(posX, posY, BREATH_RESULT_LABEL_UNIT_WIDTH, BREATH_RESULT_LABEL_UNIT_HEIGHT);
        labelUnit_[i]->SetText(strDes[i].c_str());
        labelUnit_[i]->SetTextColor(Color::White());
        labelUnit_[i]->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BREATH_MIN_FONT_SIZE);
        labelUnit_[i]->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        labelUnit_[i]->SetStyle(STYLE_TEXT_OPA, TEXT_OPACITY_TWO);
        labelUnit_[i]->SetWidth(labelUnit_[i]->GetTextWidth());
        group_->Add(labelUnit_[i]);
    }
}

bool BreathResultPage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), BREATH_BUTTON_COMPLETE_ID) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(BREATH_PAGE::BREATH_MAIN_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    } else if (strcmp(view.GetViewId(), BREATH_BUTTON_RESTART_ID) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(BREATH_PAGE::BREATH_START_VIEW,
            TransitionType::TRANSITION_INVALID, false);
#ifdef _WIN32
        struct timeval tv;
        gettimeofday(&tv, NULL);
#else
        struct timeval64 tv;
        gettimeofday64(&tv, NULL);
#endif
        uint32_t remTime = tv.tv_sec;
        BreathModel::GetInstance().SetStartTime(remTime);
    }
    return true;
}

bool BreathResultPage::OnDrag(UIView &view, const DragEvent &event)
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