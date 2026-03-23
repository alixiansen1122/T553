/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SleepMainPage
 * Created: 2025-06-05
 */

#include <string>
#include "components/ui_list_nested.h"
#include "components/ui_scroll_view_nested.h"
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "common/image_cache_manager.h"
#include "diag_service.h"
#include "msg_center_device.h"
#include "diag_common.h"
#include "sleep/SleepTargetTime.h"
#include "sleep/SleepPresenter.h"
#include "sleep/SleepView.h"
#include "sleep/SleepMainPage.h"

namespace OHOS {
static constexpr int16_t SLEEPMAIN_SLEEP_X = 97;
static constexpr int16_t SLEEPMAIN_SLEEP_Y = 31;
static constexpr int16_t SLEEPMAIN_SLEEP_WIDTH = 260;
static constexpr int16_t SLEEPMAIN_SLEEP_HEIGHT = 53;
static constexpr int16_t SLEEPMAIN_SLEEP_FONT_SIZE = 38;
static constexpr int16_t SLEEPMAIN_NORECORD_X = 179;
static constexpr int16_t SLEEPMAIN_NORECORD_Y = 260;
static constexpr int16_t SLEEPMAIN_NORECORD_WIDTH = 96;
static constexpr int16_t SLEEPMAIN_NORECORD_HEIGHT = 48;
static constexpr int16_t SLEEPMAIN_NORECORD_FONT_SIZE = 32;
static constexpr int16_t SLEEPMAIN_TIPS_X = 78;
static constexpr int16_t SLEEPMAIN_TIPS_Y = 310;
static constexpr int16_t SLEEPMAIN_TIPS_WIDTH = 300;
static constexpr int16_t SLEEPMAIN_TIPS_HEIGHT = 130;
static constexpr int16_t SLEEPMAIN_TIPS_FONT_SIZE = 24;
static constexpr int16_t SLEEPMAIN_TIPS_FONT_SIZE_TWO = 28;
static constexpr int16_t SLEEPMAIN_SCORE_X = 164;
static constexpr int16_t SLEEPMAIN_SCORE_Y = 129;
static constexpr int16_t SLEEPMAIN_SCORE_WIDTH = 95;
static constexpr int16_t SLEEPMAIN_SCORE_HEIGHT = 65;
static constexpr int16_t SLEEPMAIN_SCORE_FONT_SIZE = 56;
static constexpr int16_t SLEEPMAIN_MOONNORECORD_X = 154;
static constexpr int16_t SLEEPMAIN_MOONNORECORD_Y = 60;
static constexpr int16_t SLEEPMAIN_SCORETEXT_FONT_SIZE = 21;
static constexpr int16_t SLEEP_SCORE_THRESHOLD_1 = 20;
static constexpr int16_t SLEEP_SCORE_THRESHOLD_2 = 40;
static constexpr int16_t SLEEP_SCORE_THRESHOLD_3 = 60;
static constexpr int16_t SLEEP_SCORE_THRESHOLD_4 = 80;
static constexpr int16_t SLEEP_SCORE_THRESHOLD_5 = 100;
static constexpr int16_t SLEEP_FONT_SIZE_LARGE = 40;
static constexpr int16_t SLEEPMAIN_SCORETEXT_X = 265;
static constexpr int16_t SLEEPMAIN_SCORETEXT_Y = 161;
static constexpr int16_t SLEEP_LABEL_WIDTH = 48;
static constexpr int16_t SLEEP_LABEL_HEIGHT = 29;
static constexpr int16_t SLEEP_TIME_LABEL_WIDTH = 100;
static constexpr int16_t SLEEP_TIME_LABEL_HEIGHT = 50;
static constexpr int16_t SLEEPMAIN_SETTING_X = 280;
static constexpr int16_t SLEEPMAIN_SETTING_Y = 374;
static constexpr int16_t SLEEPMAIN_ACTUALMINUTE_X = 230;
static constexpr int16_t SLEEPMAIN_ACTUALMINUTE_Y = 319;
static constexpr int16_t SLEEPMAIN_ACTUALHOUR_X = 135;
static constexpr int16_t ACTUALMINUTES_Y = 303;
static constexpr int16_t SLEEPMAIN_ACTUALHOURTEXT_X = 170;
static constexpr int16_t SLEEPDETAIL_ACTUALSLEEPTIMEHOURS_FONT_SIZE = 32;
static constexpr int16_t SLEEPMAIN_HEIGHT = 50;
static constexpr int16_t SLEEPMAIN_ACTUALMINUTETEXT_X = 280;
static constexpr int16_t SLEEPMAIN_CIR_HIGHT = 444;
static constexpr int16_t SLEEPMAIN_CIR_X = 228;
static constexpr int16_t SLEEPMAIN_CIR_Y = 228;
static constexpr int16_t SLEEPMAIN_CIR_RADIUS = 215;
static constexpr int16_t SLEEPMAIN_CIR_ANGLE = 360;
static constexpr int16_t SLEEPMAIN_CIR_RANGE = 100;
static constexpr int16_t SLEEPMAIN_CIR_LINE_WIDTH = 10;
static constexpr int16_t SLEEPMAIN_SCORE = 100;
static constexpr int16_t SLEEPMAIN_RATE = 100;
static constexpr float MINUTE_HOUR = 60.0f;
static constexpr int16_t TARGETTIME_LABEL_Y = 363;
static constexpr int16_t TARGETTIMEBUTTON_X = 137;
static constexpr int16_t TARGETTIME_LABEL_WIDTH = 184;
static constexpr int16_t TARGETTIME_LABEL_HEIGHT = 46;
static constexpr int16_t MAIN_SLEEP_COLOR_R = 181;
static constexpr int16_t MAIN_SLEEP_COLOR_G = 402;
static constexpr int16_t MAIN_SLEEP_COLOR_B = 255;
static constexpr int16_t MAIN_SLEEP_OPT = 110;
static constexpr int16_t MAIN_SLEEP_RANGE = 10;
static constexpr int16_t MAIN_SLEEP_POSX_2 = 2;
static constexpr int16_t MAIN_SLEEP_POSX_3 = 3;
static constexpr int16_t MAIN_SLEEP_POSX_4 = 4;
static constexpr int16_t MAIN_SLEEP_MOON_POSX = 198;
static constexpr int16_t MAIN_SLEEP_SCORE_HEIGHT = 192;
static constexpr int16_t MAIN_SLEEP_SCORE_X = 131;
static constexpr int16_t MAIN_SLEEP_SCORE_Y = 200;
static constexpr int16_t MAIN_SLEEP_STAR1_X = 145;
static constexpr int16_t MAIN_SLEEP_STAR2_X = 180;
static constexpr int16_t MAIN_SLEEP_STAR3_X = 215;
static constexpr int16_t MAIN_SLEEP_STAR4_X = 250;
static constexpr int16_t MAIN_SLEEP_STAR5_X = 285;
static constexpr int16_t MAIN_SLEEP_STAR_Y = 255;
static constexpr int16_t MAIN_SLEEP_SCORE_TEXT_FONT_SIZE = 55;
static constexpr int16_t MAIN_SLEEP_STAR_NUM = 5;
static constexpr int16_t SLEEP_MOON_NO_RECORD_X = 128;
static constexpr char *TARGET_TIME = "targetTime";
static constexpr int16_t g_posX[MAIN_SLEEP_STAR_NUM] = {MAIN_SLEEP_STAR1_X, MAIN_SLEEP_STAR2_X, MAIN_SLEEP_STAR3_X, MAIN_SLEEP_STAR4_X,
                                 MAIN_SLEEP_STAR5_X};
static constexpr int16_t g_posY = MAIN_SLEEP_STAR_Y;

REGIST_SLICE_PAGE(VIEW_SLEEPING, SLEEPING_PAGES::SLEEPING_MAIN_PAGE, SleepMainPage, true);

SleepMainPage::SleepMainPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SleepMainPage::SleepMainPage");
}

SleepMainPage::~SleepMainPage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }

    if (imageViewMain_ != nullptr) {
        delete imageViewMain_;
        imageViewMain_ = nullptr;
    }

    if (sleepTitleLabel_ != nullptr) {
        delete sleepTitleLabel_;
        sleepTitleLabel_ = nullptr;
    }

    if (moonNoRecordImg_ != nullptr) {
        delete moonNoRecordImg_;
        moonNoRecordImg_ = nullptr;
    }

    if (moonImg_ != nullptr) {
        delete moonImg_;
        moonImg_ = nullptr;
    }

    if (noRecordLabel_ != nullptr) {
        delete noRecordLabel_;
        noRecordLabel_ = nullptr;
    }

    if (tipsLabel_ != nullptr) {
        delete tipsLabel_;
        tipsLabel_ = nullptr;
    }

    if (sleepScoresLabel_ != nullptr) {
        delete sleepScoresLabel_;
        sleepScoresLabel_ = nullptr;
    }

    if (scoreUnitLabel_ != nullptr) {
        delete scoreUnitLabel_;
        scoreUnitLabel_ = nullptr;
    }

    if (sleepScoreTextLabel_ != nullptr) {
        delete sleepScoreTextLabel_;
        sleepScoreTextLabel_ = nullptr;
    }

    if (scoreStarImg1_ != nullptr) {
        delete scoreStarImg1_;
        scoreStarImg1_ = nullptr;
    }

    if (scoreStarImg2_ != nullptr) {
        delete scoreStarImg2_;
        scoreStarImg2_ = nullptr;
    }

    if (scoreStarImg3_ != nullptr) {
        delete scoreStarImg3_;
        scoreStarImg3_ = nullptr;
    }
    if (scoreStarImg4_ != nullptr) {
        delete scoreStarImg4_;
        scoreStarImg4_ = nullptr;
    }

    if (scoreStarImg5_ != nullptr) {
        delete scoreStarImg5_;
        scoreStarImg5_ = nullptr;
    }

    if (settingImg_ != nullptr) {
        delete settingImg_;
        settingImg_ = nullptr;
    }

    if (targetTimeLabel_ != nullptr) {
        delete targetTimeLabel_;
        targetTimeLabel_ = nullptr;
    }

    if (targetSleepTimeHoursLabel_ != nullptr) {
        delete targetSleepTimeHoursLabel_;
        targetSleepTimeHoursLabel_ = nullptr;
    }

    if (targetSleepTimeHoursUnitLabel_ != nullptr) {
        delete targetSleepTimeHoursUnitLabel_;
        targetSleepTimeHoursUnitLabel_ = nullptr;
    }

    if (targetSleepTimeMinutesLabel_ != nullptr) {
        delete targetSleepTimeMinutesLabel_;
        targetSleepTimeMinutesLabel_ = nullptr;
    }

    if (targetSleepTimeMinutesUnitLabel_ != nullptr) {
        delete targetSleepTimeMinutesUnitLabel_;
        targetSleepTimeMinutesUnitLabel_ = nullptr;
    }

    if (targetTimeButton_ != nullptr) {
        delete targetTimeButton_;
        targetTimeButton_ = nullptr;
    }

    if (circleProgress_ != nullptr) {
        delete circleProgress_;
        circleProgress_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(SLEEP_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SleepMainPage::~SleepMainPage");
}

void SleepMainPage::OnStart(void *data)
{
    group_ = new UIScrollView();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "group_ is nullptr");
        return;
    }
    group_->SetPosition(0, 0);
    group_->Resize(RESOLUTION_WIDTH, RESOLUTION_HEIGHT);
    group_->SetViewId("sleepMain");

    imageViewMain_ = new UIImageView();
    if (imageViewMain_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "imageViewMain_ is nullptr");
        return;
    }
    LOADIMG::LoadImageViewImg(imageViewMain_, SLEEP_IMAGE, IMAGE_SLEEP_BACKGROUND);
    imageViewMain_->SetPosition(0, 0, RESOLUTION_WIDTH, RESOLUTION_HEIGHT);
    group_->Add(imageViewMain_);

    if (GetRecord() == false) {
        InitNoRecordPage();
    } else {
        InitRecordPage();
    }

    group_->SetOnDragListener(this);
    group_->SetDraggable(true);
    group_->SetTouchable(true);
    AddViewToPageContainer(group_);
}

void SleepMainPage::InitNoRecordPage()
{
    sleepTitleLabel_ = new UILabel();
    if (sleepTitleLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "sleepTitleLabel_ is nullptr");
        return;
    }
    sleepTitleLabel_->SetPosition(SLEEPMAIN_SLEEP_X, SLEEPMAIN_SLEEP_Y);
    sleepTitleLabel_->Resize(SLEEPMAIN_SLEEP_WIDTH, SLEEPMAIN_SLEEP_HEIGHT);
    sleepTitleLabel_->SetText("睡眠");
    sleepTitleLabel_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    sleepTitleLabel_->SetTextColor(Color::White());
    sleepTitleLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, SLEEPMAIN_SLEEP_FONT_SIZE);
    sleepTitleLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    group_->Add(sleepTitleLabel_);

    moonNoRecordImg_ = new UIImageView();
    if (moonNoRecordImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "moonNoRecordImg_ is nullptr");
        return;
    }
    LOADIMG::LoadImageViewImg(moonNoRecordImg_, SLEEP_IMAGE, IMAGE_SLEEP_NORECORD_SLEEP);
    moonNoRecordImg_->SetPosition(SLEEP_MOON_NO_RECORD_X, SLEEPMAIN_MOONNORECORD_Y);
    group_->Add(moonNoRecordImg_);

    noRecordLabel_ = new UILabel();
    if (noRecordLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "noRecordLabel_ is nullptr");
        return;
    }
    noRecordLabel_->SetPosition(SLEEPMAIN_NORECORD_X, SLEEPMAIN_NORECORD_Y);
    noRecordLabel_->Resize(SLEEPMAIN_NORECORD_WIDTH, SLEEPMAIN_NORECORD_HEIGHT);
    noRecordLabel_->SetText("无记录");
    noRecordLabel_->SetTextColor(Color::White());
    noRecordLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SLEEPMAIN_NORECORD_FONT_SIZE);
    noRecordLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    group_->Add(noRecordLabel_);

    tipsLabel_ = new UILabel();
    if (tipsLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "tipsLabel_ is nullptr");
        return;
    }
    tipsLabel_->SetPosition(SLEEPMAIN_TIPS_X, SLEEPMAIN_TIPS_Y);
    tipsLabel_->Resize(SLEEPMAIN_TIPS_WIDTH, SLEEPMAIN_TIPS_HEIGHT);
    tipsLabel_->SetText("睡眠时间过短或睡眠中动作幅度大，手表可能检测不到睡眠");
    tipsLabel_->SetTextColor(Color::White());
    tipsLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SLEEPMAIN_TIPS_FONT_SIZE_TWO);
    tipsLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    group_->Add(tipsLabel_);
}

void SleepMainPage::InitRecordPage()
{
    SleepModel::GetInstance().SendData();
    InitCircleProgress();
    moonImg_ = new UIImageView();
    if (moonImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "moonImg_ is nullptr");
        return;
    }
    moonImg_->SetPosition(MAIN_SLEEP_MOON_POSX, TARGETTIME_LABEL_HEIGHT);
    LOADIMG::LoadImageViewImg(moonImg_, SLEEP_IMAGE, IMAGE_SLEEP_RECORD_SLEEP);
    group_->Add(moonImg_);

    sleepScoresLabel_ = new UILabel();
    if (sleepScoresLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "sleepScoresLabel_ is nullptr");
        return;
    }
    sleepScoresLabel_->SetText(std::to_string(GetSleepingScores()).c_str());
    sleepScoresLabel_->SetPosition(SLEEPMAIN_SCORE_X, SLEEPMAIN_SCORE_Y);
    sleepScoresLabel_->Resize(SLEEPMAIN_SCORE_WIDTH, SLEEPMAIN_SCORE_HEIGHT);
    sleepScoresLabel_->SetTextColor(Color::White());
    sleepScoresLabel_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    sleepScoresLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, SLEEPMAIN_SCORE_FONT_SIZE);
    sleepScoresLabel_->SetAlign(TEXT_ALIGNMENT_RIGHT, TEXT_ALIGNMENT_CENTER);
    group_->Add(sleepScoresLabel_);

    scoreUnitLabel_ = new UILabel();
    if (scoreUnitLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "scoreUnitLabel_ is nullptr");
        return;
    }
    scoreUnitLabel_->SetText("分");
    scoreUnitLabel_->SetPosition(SLEEPMAIN_SCORETEXT_X, SLEEPMAIN_SCORETEXT_Y);
    scoreUnitLabel_->Resize(MAIN_SLEEP_SCORE_TEXT_FONT_SIZE, SLEEPMAIN_TIPS_FONT_SIZE);
    scoreUnitLabel_->SetTextColor(Color::White());
    scoreUnitLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SLEEPMAIN_SCORETEXT_FONT_SIZE);
    group_->Add(scoreUnitLabel_);
    ScoreDisplay();

    sleepScoreTextLabel_ = new UILabel();
    if (sleepScoreTextLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "sleepScoreTextLabel_ is nullptr");
        return;
    }
    sleepScoreTextLabel_->SetText("夜间睡眠得分");
    sleepScoreTextLabel_->Resize(MAIN_SLEEP_SCORE_HEIGHT, SLEEPMAIN_NORECORD_HEIGHT);
    sleepScoreTextLabel_->SetPosition(MAIN_SLEEP_SCORE_X, MAIN_SLEEP_SCORE_Y);
    sleepScoreTextLabel_->SetTextColor(Color::White());
    sleepScoreTextLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SLEEPDETAIL_ACTUALSLEEPTIMEHOURS_FONT_SIZE);
    group_->Add(sleepScoreTextLabel_);

    // 显示具体睡觉时间
    LoadActualSleepTime(SleepModel::GetInstance().GetSleepTime());

    // 加载目标按钮
    LoadTargetTimeButton(SleepModel::GetInstance().GetTargetTime());
}

void SleepMainPage::InitScoreDisplayPicture()
{
    scoreStarImg1_ = new UIImageView();
    if (scoreStarImg1_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "scoreStarImg1_ is nullptr");
        return;
    }
    scoreStarImg1_->SetPosition(g_posX[0], g_posY);
    group_->Add(scoreStarImg1_);

    scoreStarImg2_ = new UIImageView();
    if (scoreStarImg2_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "scoreStarImg2_ is nullptr");
        return;
    }
    scoreStarImg2_->SetPosition(g_posX[1], g_posY);
    group_->Add(scoreStarImg2_);

    scoreStarImg3_ = new UIImageView();
    if (scoreStarImg3_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "scoreStarImg3_ is nullptr");
        return;
    }
    scoreStarImg3_->SetPosition(g_posX[MAIN_SLEEP_POSX_2], g_posY);
    group_->Add(scoreStarImg3_);

    scoreStarImg4_ = new UIImageView();
    if (scoreStarImg4_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "scoreStarImg4_ is nullptr");
        return;
    }
    scoreStarImg4_->SetPosition(g_posX[MAIN_SLEEP_POSX_3], g_posY);
    group_->Add(scoreStarImg4_);

    scoreStarImg5_ = new UIImageView();
    if (scoreStarImg5_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "scoreStarImg5_ is nullptr");
        return;
    }
    scoreStarImg5_->SetPosition(g_posX[MAIN_SLEEP_POSX_4], g_posY);
    group_->Add(scoreStarImg5_);
}

void SleepMainPage::ScoreDisplay()
{
    InitScoreDisplayPicture();
    if (GetSleepingScores() < SLEEP_SCORE_THRESHOLD_1) {
        LOADIMG::LoadImageViewImg(scoreStarImg1_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_GRAY);
        LOADIMG::LoadImageViewImg(scoreStarImg2_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_GRAY);
        LOADIMG::LoadImageViewImg(scoreStarImg3_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_GRAY);
        LOADIMG::LoadImageViewImg(scoreStarImg4_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_GRAY);
        LOADIMG::LoadImageViewImg(scoreStarImg5_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_GRAY);
    } else if (GetSleepingScores() >= SLEEP_SCORE_THRESHOLD_1 && GetSleepingScores() < SLEEP_SCORE_THRESHOLD_2) {
        LOADIMG::LoadImageViewImg(scoreStarImg1_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_YELLOW);
        LOADIMG::LoadImageViewImg(scoreStarImg2_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_GRAY);
        LOADIMG::LoadImageViewImg(scoreStarImg3_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_GRAY);
        LOADIMG::LoadImageViewImg(scoreStarImg4_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_GRAY);
        LOADIMG::LoadImageViewImg(scoreStarImg5_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_GRAY);
    } else if (GetSleepingScores() >= SLEEP_SCORE_THRESHOLD_2 && GetSleepingScores() < SLEEP_SCORE_THRESHOLD_3) {
        LOADIMG::LoadImageViewImg(scoreStarImg1_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_YELLOW);
        LOADIMG::LoadImageViewImg(scoreStarImg2_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_YELLOW);
        LOADIMG::LoadImageViewImg(scoreStarImg3_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_GRAY);
        LOADIMG::LoadImageViewImg(scoreStarImg4_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_GRAY);
        LOADIMG::LoadImageViewImg(scoreStarImg5_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_GRAY);
    } else if (GetSleepingScores() >= SLEEP_SCORE_THRESHOLD_3 && GetSleepingScores() < SLEEP_SCORE_THRESHOLD_4) {
        LOADIMG::LoadImageViewImg(scoreStarImg1_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_YELLOW);
        LOADIMG::LoadImageViewImg(scoreStarImg2_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_YELLOW);
        LOADIMG::LoadImageViewImg(scoreStarImg3_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_YELLOW);
        LOADIMG::LoadImageViewImg(scoreStarImg4_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_GRAY);
        LOADIMG::LoadImageViewImg(scoreStarImg5_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_GRAY);
    } else if (GetSleepingScores() >= SLEEP_SCORE_THRESHOLD_4 && GetSleepingScores() < SLEEP_SCORE_THRESHOLD_5) {
        LOADIMG::LoadImageViewImg(scoreStarImg1_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_YELLOW);
        LOADIMG::LoadImageViewImg(scoreStarImg2_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_YELLOW);
        LOADIMG::LoadImageViewImg(scoreStarImg3_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_YELLOW);
        LOADIMG::LoadImageViewImg(scoreStarImg4_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_YELLOW);
        LOADIMG::LoadImageViewImg(scoreStarImg5_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_GRAY);
    } else if (GetSleepingScores() == SLEEP_SCORE_THRESHOLD_5) {
        LOADIMG::LoadImageViewImg(scoreStarImg1_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_YELLOW);
        LOADIMG::LoadImageViewImg(scoreStarImg2_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_YELLOW);
        LOADIMG::LoadImageViewImg(scoreStarImg3_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_YELLOW);
        LOADIMG::LoadImageViewImg(scoreStarImg4_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_YELLOW);
        LOADIMG::LoadImageViewImg(scoreStarImg5_, SLEEP_IMAGE, IMAGE_SLEEP_STAR_YELLOW);
    }
}

void SleepMainPage::InitSleepTimeLabel()
{
    targetSleepTimeHoursLabel_ = new UILabel();
    if (targetSleepTimeHoursLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "targetSleepTimeHoursLabel_ is nullptr");
        return;
    }
    targetSleepTimeHoursLabel_->SetPosition(SLEEPMAIN_ACTUALHOUR_X, ACTUALMINUTES_Y);
    targetSleepTimeHoursLabel_->Resize(SLEEPMAIN_HEIGHT, SLEEPMAIN_HEIGHT);
    targetSleepTimeHoursLabel_->SetTextColor(Color::White());
    targetSleepTimeHoursLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, SLEEP_FONT_SIZE_LARGE);
    group_->Add(targetSleepTimeHoursLabel_);

    targetSleepTimeHoursUnitLabel_ = new UILabel();
    if (targetSleepTimeHoursUnitLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "targetSleepTimeHoursUnitLabel_ is nullptr");
        return;
    }
    targetSleepTimeHoursUnitLabel_->SetPosition(SLEEPMAIN_ACTUALHOURTEXT_X, SLEEPMAIN_ACTUALMINUTE_Y);
    targetSleepTimeHoursUnitLabel_->Resize(SLEEP_LABEL_WIDTH, SLEEP_LABEL_HEIGHT);
    targetSleepTimeHoursUnitLabel_->SetTextColor(Color::White());
    targetSleepTimeHoursUnitLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SLEEPMAIN_TIPS_FONT_SIZE);
    targetSleepTimeHoursUnitLabel_->SetText("小时");
    group_->Add(targetSleepTimeHoursUnitLabel_);

    targetSleepTimeMinutesLabel_ = new UILabel();
    if (targetSleepTimeMinutesLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "targetSleepTimeMinutesLabel_ is nullptr");
        return;
    }
    targetSleepTimeMinutesLabel_->SetPosition(SLEEPMAIN_ACTUALMINUTE_X, ACTUALMINUTES_Y);
    targetSleepTimeMinutesLabel_->Resize(SLEEP_TIME_LABEL_WIDTH, SLEEP_TIME_LABEL_HEIGHT);
    targetSleepTimeMinutesLabel_->SetTextColor(Color::White());
    targetSleepTimeMinutesLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, SLEEP_FONT_SIZE_LARGE);
    group_->Add(targetSleepTimeMinutesLabel_);

    targetSleepTimeMinutesUnitLabel_ = new UILabel();
    if (targetSleepTimeMinutesUnitLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "targetSleepTimeMinutesUnitLabel_ is nullptr");
        return;
    }
    targetSleepTimeMinutesUnitLabel_->SetPosition(SLEEPMAIN_ACTUALMINUTETEXT_X, SLEEPMAIN_ACTUALMINUTE_Y);
    targetSleepTimeMinutesUnitLabel_->Resize(SLEEP_LABEL_WIDTH, SLEEP_LABEL_HEIGHT);
    targetSleepTimeMinutesUnitLabel_->SetTextColor(Color::White());
    targetSleepTimeMinutesUnitLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SLEEPMAIN_TIPS_FONT_SIZE);
    targetSleepTimeMinutesUnitLabel_->SetText("分钟");
    group_->Add(targetSleepTimeMinutesUnitLabel_);
}

void SleepMainPage::LoadTargetTimeButton(std::tuple<uint32_t, uint32_t> timeTuple)
{
    uint32_t hours = std::get<0>(timeTuple);
    uint32_t minutes = std::get<1>(timeTuple);
    std::string str = "";
    if ((hours == 0) && (minutes != 0)) {
        str = std::to_string(minutes) + "分钟";
    } else if ((hours != 0) && (minutes == 0)) {
        str = std::to_string(hours) + "小时";
    } else if ((hours != 0) && (minutes != 0)) {
        if (minutes < MAIN_SLEEP_RANGE && hours >= MAIN_SLEEP_RANGE) {
            str = " " + std::to_string(hours) + ":0" + std::to_string(minutes);
        } else if (hours < MAIN_SLEEP_RANGE && minutes >= MAIN_SLEEP_RANGE) {
            str = " 0" + std::to_string(hours) + ":" + std::to_string(minutes);
        } else if (minutes < MAIN_SLEEP_RANGE && hours < MAIN_SLEEP_RANGE) {
            str = " 0" + std::to_string(hours) + ":0" + std::to_string(minutes);
        } else if (minutes >= MAIN_SLEEP_RANGE && hours >= MAIN_SLEEP_RANGE) {
            str = " " + std::to_string(hours) + ":" + std::to_string(minutes);
        }
    }
    targetTimeButton_ = new UILabelButton();
    if (targetTimeButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "targetTimeButton_ is nullptr");
        return;
    }
    targetTimeButton_->SetPosition(TARGETTIMEBUTTON_X, TARGETTIME_LABEL_Y);
    targetTimeButton_->SetViewId(TARGET_TIME);
    targetTimeButton_->SetOnClickListener(this);
    targetTimeButton_->SetStyle(
        STYLE_BACKGROUND_COLOR,
        Color::GetColorFromRGBA(MAIN_SLEEP_COLOR_R, MAIN_SLEEP_COLOR_G, MAIN_SLEEP_COLOR_B, MAIN_SLEEP_OPT).full);
    targetTimeButton_->SetStyleForState(
        STYLE_BACKGROUND_COLOR,
        Color::GetColorFromRGBA(MAIN_SLEEP_COLOR_R, MAIN_SLEEP_COLOR_G, MAIN_SLEEP_COLOR_B, MAIN_SLEEP_OPT).full,
        UIButton::PRESSED);
    targetTimeButton_->Resize(TARGETTIME_LABEL_WIDTH, TARGETTIME_LABEL_HEIGHT);
    targetTimeButton_->SetTextColor(Color::White());
    targetTimeButton_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SLEEPMAIN_TIPS_FONT_SIZE);
    group_->Add(targetTimeButton_);

    targetTimeLabel_ = new UILabel();
    if (targetTimeLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "targetTimeLabel_ is nullptr");
        return;
    }
    std::string targetUnit = "目标" + str;
    targetTimeLabel_->SetText(targetUnit.c_str());
    targetTimeLabel_->SetPosition(SLEEPMAIN_MOONNORECORD_X, TARGETTIME_LABEL_Y);
    targetTimeLabel_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    targetTimeLabel_->Resize(TARGETTIME_LABEL_WIDTH, TARGETTIME_LABEL_HEIGHT);
    targetTimeLabel_->SetTextColor(Color::White());
    targetTimeLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SLEEPMAIN_TIPS_FONT_SIZE);
    targetTimeLabel_->SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    group_->Add(targetTimeLabel_);

    settingImg_ = new UIImageView();
    if (settingImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "settingImg_ is nullptr");
        return;
    }
    settingImg_->SetPosition(SLEEPMAIN_SETTING_X, SLEEPMAIN_SETTING_Y);
    LOADIMG::LoadImageViewImg(settingImg_, SLEEP_IMAGE, IMAGE_SLEEP_SETTING);
    group_->Add(settingImg_);
}

void SleepMainPage::LoadActualSleepTime(std::tuple<uint32_t, uint32_t> timeTuple)
{
    InitSleepTimeLabel();
    uint32_t hours = std::get<0>(timeTuple);
    uint32_t minutes = std::get<1>(timeTuple);
    targetSleepTimeHoursLabel_->SetText(std::to_string(hours).c_str());
    targetSleepTimeMinutesLabel_->SetText(std::to_string(minutes).c_str());
}

void SleepMainPage::InitCircleProgress()
{
    circleProgress_ = new UICircleProgress();
    if (circleProgress_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "circleProgress_ is nullptr");
        return;
    }
    circleProgress_->SetPosition(0, 0);
    circleProgress_->Resize(SLEEPMAIN_CIR_HIGHT, SLEEPMAIN_CIR_HIGHT);
    circleProgress_->SetCenterPosition(SLEEPMAIN_CIR_X, SLEEPMAIN_CIR_Y);
    circleProgress_->SetRadius(SLEEPMAIN_CIR_RADIUS);
    circleProgress_->SetStartAngle(0);
    circleProgress_->SetEndAngle(SLEEPMAIN_CIR_ANGLE);
    circleProgress_->SetRange(0, SLEEPMAIN_CIR_RANGE);
    circleProgress_->SetBackgroundStyle(STYLE_LINE_COLOR, 0xFF6f29a0);
    circleProgress_->SetForegroundStyle(STYLE_LINE_COLOR, 0xFFB566FF);
    circleProgress_->SetBackgroundStyle(STYLE_LINE_WIDTH, SLEEPMAIN_CIR_LINE_WIDTH);
    circleProgress_->SetForegroundStyle(STYLE_LINE_WIDTH, SLEEPMAIN_CIR_LINE_WIDTH);
    group_->Add(circleProgress_);
    circleProgress_->SetValue(GetSleepingScores());
    circleProgress_->Invalidate();
}

uint32_t SleepMainPage::GetSleepingScores()
{
    uint32_t hours = SleepModel::GetInstance().GetSleepTimeHours();
    uint32_t minutes = SleepModel::GetInstance().GetSleepTimeMinutes();
    std::tuple<uint32_t, uint32_t> timeTuple = SleepModel::GetInstance().GetTargetTime();
    uint32_t timeHours = std::get<0>(timeTuple);
    uint32_t timeMinutes = std::get<1>(timeTuple);
    if (((hours * MINUTE_HOUR) + minutes) > ((timeHours * MINUTE_HOUR) + timeMinutes)) {
        return SLEEPMAIN_SCORE;
    } else {
        float score = ((hours * MINUTE_HOUR) + minutes) / ((timeHours * MINUTE_HOUR) + timeMinutes);
        return uint32_t(score * SLEEPMAIN_RATE);
    }
}

bool SleepMainPage::GetRecord()
{
    if (SleepModel::GetInstance().GetRecord()) {
        return true;
    }
    return false;
}

bool SleepMainPage::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(view);
    if (strcmp(view.GetViewId(), TARGET_TIME) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(SLEEPING_PAGES::SLEEPING_TARGET_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool SleepMainPage::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().ChangeSliceToApplist();
    } else if (event.GetDragDirection() == DragEvent::DIRECTION_BOTTOM_TO_TOP) {
        NativeAbility::GetInstance().SwitchPageInSlice(SLEEPING_PAGES::SLEEPING_DEATIL_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}
}
