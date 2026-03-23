/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneMenuDialPage
 * Create: 2025-07-12
 */

#include "SlicePageFactory.h"
#include "NativeAbility.h"
#include "AppViewIDs.h"
#include "wearable_log.h"
#include "main/LoadImg.h"
#include "ui_resource_phonemenu.h"
#include "phoneservice/PhoneService.h"
#include "phonemenu/PhoneMenuView.h"
#include "phonemenu/PhoneMenuCallerLogModel.h"
#include "phonemenu/PhoneMenuDialPage.h"

static constexpr uint16_t DIAL_NORMAL_BUTTON_WIDTH = 88;
static constexpr uint16_t DIAL_NORMAL_BUTTON_HEIGHT = 64;
static constexpr uint16_t DIAL_WIDE_BUTTON_WIDTH = 180;
static constexpr uint16_t DIAL_WIDE_BUTTON_HEIGHT = 64;
static constexpr uint16_t FIRST_ROW_COLOR_BUTTON_POS_Y = 115;
static constexpr uint16_t SECOND_ROW_COLOR_BUTTON_POS_Y = 183;
static constexpr uint16_t THIRD_ROW_COLOR_BUTTON_POS_Y = 251;
static constexpr uint16_t FOUR_ROW_COLOR_BUTTON_POS_Y = 319;
static constexpr uint16_t FIRST_COLUMN_COLOR_BUTTON_POS_X = 45;
static constexpr uint16_t SECOND_COLUMN_COLOR_BUTTON_POS_X = 137;
static constexpr uint16_t THIRD_COLUMN_COLOR_BUTTON_POS_X = 229;
static constexpr uint16_t FOUR_COLUMN_COLOR_BUTTON_POS_X = 321;
static constexpr uint16_t NUMBER_LABEL_POS_X = 93;
static constexpr uint16_t NUMBER_LABEL_POS_Y = 53;
static constexpr uint16_t NUMBER_LABEL_WIDTH = 268;
static constexpr uint16_t NUMBER_LABEL_HEIGHT = 48;
static constexpr uint16_t NUMBER_LABEL_FONT_SIZE = 30;
static constexpr uint16_t DIAL_FONT_SIZE = 40;
static constexpr uint16_t DIAL_BUTTON_BORDER_RADIUS = 20;
static constexpr uint16_t BUTTON_BACKGROUND_OPA = 80;
static constexpr char *KEY_0_BUTTON_ID = "key0Button";
static constexpr char *KEY_1_BUTTON_ID = "key1Button";
static constexpr char *KEY_2_BUTTON_ID = "key2Button";
static constexpr char *KEY_3_BUTTON_ID = "key3Button";
static constexpr char *KEY_4_BUTTON_ID = "key4Button";
static constexpr char *KEY_5_BUTTON_ID = "key5Button";
static constexpr char *KEY_6_BUTTON_ID = "key6Bbutton";
static constexpr char *KEY_7_BUTTON_ID = "key7Button";
static constexpr char *KEY_8_BUTTON_ID = "key8Button";
static constexpr char *KEY_9_BUTTON_ID = "key9Button";
static constexpr char *KEY_STAR_BUTTON_ID = "keyStarButton";
static constexpr char *KEY_POUND_BUTTON_ID = "keyPoundButton";
static constexpr char *DELETE_BUTTON_ID = "deleteButton";
static constexpr char *DIAL_BUTTON_ID = "dialButton";

namespace OHOS {
REGIST_SLICE_PAGE(VIEW_PHONE_MENU, PhoneMenuPages::PHONE_MENU_DIAL_PAGE, PhoneMenuDialPage, false);

void PhoneMenuDialPage::OnStart(void *data)
{
    container_ = new UIScrollView();
    if (container_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuDialPage container_ new fail");
        return;
    }
    container_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    container_->SetOnDragListener(this);

    dialNumberLabel_ = new UILabel();
    if (dialNumberLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuDialPage dialNumberLabel_ new fail");
        return;
    }
    dialNumberLabel_->SetPosition(NUMBER_LABEL_POS_X, NUMBER_LABEL_POS_Y, NUMBER_LABEL_WIDTH, NUMBER_LABEL_HEIGHT);
    dialNumberLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    dialNumberLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, NUMBER_LABEL_FONT_SIZE);
    container_->Add(dialNumberLabel_);

    key1Button_ = new UILabelButton();
    if (key1Button_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuDialPage key1Button_ new fail");
        return;
    }
    key1Button_->SetPosition(FIRST_COLUMN_COLOR_BUTTON_POS_X, FIRST_ROW_COLOR_BUTTON_POS_Y, DIAL_NORMAL_BUTTON_WIDTH,
                             DIAL_NORMAL_BUTTON_HEIGHT);
    key1Button_->SetViewId(KEY_1_BUTTON_ID);
    key1Button_->SetText("1");
    key1Button_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, DIAL_FONT_SIZE);
    key1Button_->SetStyle(STYLE_BORDER_RADIUS, DIAL_BUTTON_BORDER_RADIUS);
    key1Button_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    key1Button_->SetStyle(STYLE_BACKGROUND_OPA, BUTTON_BACKGROUND_OPA);
    key1Button_->SetStyleForState(STYLE_BORDER_RADIUS, DIAL_BUTTON_BORDER_RADIUS, UIButton::PRESSED);
    key1Button_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Gray().full, UIButton::PRESSED);
    key1Button_->SetStyleForState(STYLE_BACKGROUND_OPA, BUTTON_BACKGROUND_OPA, UIButton::PRESSED);
    key1Button_->SetOnClickListener(this);
    container_->Add(key1Button_);

    key2Button_ = new UILabelButton();
    if (key2Button_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuDialPage key2Button_ new fail");
        return;
    }
    key2Button_->SetPosition(SECOND_COLUMN_COLOR_BUTTON_POS_X, FIRST_ROW_COLOR_BUTTON_POS_Y, DIAL_NORMAL_BUTTON_WIDTH,
                             DIAL_NORMAL_BUTTON_HEIGHT);
    key2Button_->SetViewId(KEY_2_BUTTON_ID);
    key2Button_->SetText("2");
    key2Button_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, DIAL_FONT_SIZE);
    key2Button_->SetStyle(STYLE_BORDER_RADIUS, DIAL_BUTTON_BORDER_RADIUS);
    key2Button_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    key2Button_->SetStyle(STYLE_BACKGROUND_OPA, BUTTON_BACKGROUND_OPA);
    key2Button_->SetStyleForState(STYLE_BORDER_RADIUS, DIAL_BUTTON_BORDER_RADIUS, UIButton::PRESSED);
    key2Button_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Gray().full, UIButton::PRESSED);
    key2Button_->SetStyleForState(STYLE_BACKGROUND_OPA, BUTTON_BACKGROUND_OPA, UIButton::PRESSED);
    key2Button_->SetOnClickListener(this);
    container_->Add(key2Button_);

    key3Button_ = new UILabelButton();
    if (key3Button_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuDialPage key3Button_ new fail");
        return;
    }
    key3Button_->SetPosition(THIRD_COLUMN_COLOR_BUTTON_POS_X, FIRST_ROW_COLOR_BUTTON_POS_Y, DIAL_NORMAL_BUTTON_WIDTH,
                             DIAL_NORMAL_BUTTON_HEIGHT);
    key3Button_->SetViewId(KEY_3_BUTTON_ID);
    key3Button_->SetText("3");
    key3Button_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, DIAL_FONT_SIZE);
    key3Button_->SetStyle(STYLE_BORDER_RADIUS, DIAL_BUTTON_BORDER_RADIUS);
    key3Button_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    key3Button_->SetStyle(STYLE_BACKGROUND_OPA, BUTTON_BACKGROUND_OPA);
    key3Button_->SetStyleForState(STYLE_BORDER_RADIUS, DIAL_BUTTON_BORDER_RADIUS, UIButton::PRESSED);
    key3Button_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Gray().full, UIButton::PRESSED);
    key3Button_->SetStyleForState(STYLE_BACKGROUND_OPA, BUTTON_BACKGROUND_OPA, UIButton::PRESSED);
    key3Button_->SetOnClickListener(this);
    container_->Add(key3Button_);

    deleteButton_ = new UILabelButton();
    if (deleteButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuDialPage deleteButton_ new fail");
        return;
    }
    deleteButton_->SetPosition(FOUR_COLUMN_COLOR_BUTTON_POS_X, FIRST_ROW_COLOR_BUTTON_POS_Y, DIAL_NORMAL_BUTTON_WIDTH,
                               DIAL_NORMAL_BUTTON_HEIGHT);
    deleteButton_->SetViewId(DELETE_BUTTON_ID);
    LOADIMG::LoadBtnImage(deleteButton_, PHONEMENU_IMAGE, IMAGE_PHONEMENU_DEL, IMAGE_PHONEMENU_DEL);
    deleteButton_->SetOnClickListener(this);
    container_->Add(deleteButton_);

    key4Button_ = new UILabelButton();
    if (key4Button_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuDialPage key4Button_ new fail");
        return;
    }
    key4Button_->SetPosition(FIRST_COLUMN_COLOR_BUTTON_POS_X, SECOND_ROW_COLOR_BUTTON_POS_Y, DIAL_NORMAL_BUTTON_WIDTH,
                             DIAL_NORMAL_BUTTON_HEIGHT);
    key4Button_->SetViewId(KEY_4_BUTTON_ID);
    key4Button_->SetText("4");
    key4Button_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, DIAL_FONT_SIZE);
    key4Button_->SetStyle(STYLE_BORDER_RADIUS, DIAL_BUTTON_BORDER_RADIUS);
    key4Button_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    key4Button_->SetStyle(STYLE_BACKGROUND_OPA, BUTTON_BACKGROUND_OPA);
    key4Button_->SetStyleForState(STYLE_BORDER_RADIUS, DIAL_BUTTON_BORDER_RADIUS, UIButton::PRESSED);
    key4Button_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Gray().full, UIButton::PRESSED);
    key4Button_->SetStyleForState(STYLE_BACKGROUND_OPA, BUTTON_BACKGROUND_OPA, UIButton::PRESSED);
    key4Button_->SetOnClickListener(this);
    container_->Add(key4Button_);

    key5Button_ = new UILabelButton();
    if (key5Button_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuDialPage key5Button_ new fail");
        return;
    }
    key5Button_->SetPosition(SECOND_COLUMN_COLOR_BUTTON_POS_X, SECOND_ROW_COLOR_BUTTON_POS_Y, DIAL_NORMAL_BUTTON_WIDTH,
                             DIAL_NORMAL_BUTTON_HEIGHT);
    key5Button_->SetViewId(KEY_5_BUTTON_ID);
    key5Button_->SetText("5");
    key5Button_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, DIAL_FONT_SIZE);
    key5Button_->SetStyle(STYLE_BORDER_RADIUS, DIAL_BUTTON_BORDER_RADIUS);
    key5Button_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    key5Button_->SetStyle(STYLE_BACKGROUND_OPA, BUTTON_BACKGROUND_OPA);
    key5Button_->SetStyleForState(STYLE_BORDER_RADIUS, DIAL_BUTTON_BORDER_RADIUS, UIButton::PRESSED);
    key5Button_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Gray().full, UIButton::PRESSED);
    key5Button_->SetStyleForState(STYLE_BACKGROUND_OPA, BUTTON_BACKGROUND_OPA, UIButton::PRESSED);
    key5Button_->SetOnClickListener(this);
    container_->Add(key5Button_);

    key6Button_ = new UILabelButton();
    if (key6Button_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuDialPage key6Button_ new fail");
        return;
    }
    key6Button_->SetPosition(THIRD_COLUMN_COLOR_BUTTON_POS_X, SECOND_ROW_COLOR_BUTTON_POS_Y, DIAL_NORMAL_BUTTON_WIDTH,
                             DIAL_NORMAL_BUTTON_HEIGHT);
    key6Button_->SetViewId(KEY_6_BUTTON_ID);
    key6Button_->SetText("6");
    key6Button_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, DIAL_FONT_SIZE);
    key6Button_->SetStyle(STYLE_BORDER_RADIUS, DIAL_BUTTON_BORDER_RADIUS);
    key6Button_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    key6Button_->SetStyle(STYLE_BACKGROUND_OPA, BUTTON_BACKGROUND_OPA);
    key6Button_->SetStyleForState(STYLE_BORDER_RADIUS, DIAL_BUTTON_BORDER_RADIUS, UIButton::PRESSED);
    key6Button_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Gray().full, UIButton::PRESSED);
    key6Button_->SetStyleForState(STYLE_BACKGROUND_OPA, BUTTON_BACKGROUND_OPA, UIButton::PRESSED);
    key6Button_->SetOnClickListener(this);
    container_->Add(key6Button_);

    keyStarButton_ = new UILabelButton();
    if (keyStarButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuDialPage keyStarButton_ new fail");
        return;
    }
    keyStarButton_->SetPosition(FOUR_COLUMN_COLOR_BUTTON_POS_X, SECOND_ROW_COLOR_BUTTON_POS_Y, DIAL_NORMAL_BUTTON_WIDTH,
                                DIAL_NORMAL_BUTTON_HEIGHT);
    keyStarButton_->SetViewId(KEY_STAR_BUTTON_ID);
    LOADIMG::LoadBtnImage(keyStarButton_, PHONEMENU_IMAGE, IMAGE_PHONEMENU_STAR_KEY, IMAGE_PHONEMENU_STAR_KEY);
    keyStarButton_->SetOnClickListener(this);
    container_->Add(keyStarButton_);

    key7Button_ = new UILabelButton();
    if (key7Button_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuDialPage key7Button_ new fail");
        return;
    }
    key7Button_->SetPosition(FIRST_COLUMN_COLOR_BUTTON_POS_X, THIRD_ROW_COLOR_BUTTON_POS_Y, DIAL_NORMAL_BUTTON_WIDTH,
                             DIAL_NORMAL_BUTTON_HEIGHT);
    key7Button_->SetViewId(KEY_7_BUTTON_ID);
    key7Button_->SetText("7");
    key7Button_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, DIAL_FONT_SIZE);
    key7Button_->SetStyle(STYLE_BORDER_RADIUS, DIAL_BUTTON_BORDER_RADIUS);
    key7Button_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    key7Button_->SetStyle(STYLE_BACKGROUND_OPA, BUTTON_BACKGROUND_OPA);
    key7Button_->SetStyleForState(STYLE_BORDER_RADIUS, DIAL_BUTTON_BORDER_RADIUS, UIButton::PRESSED);
    key7Button_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Gray().full, UIButton::PRESSED);
    key7Button_->SetStyleForState(STYLE_BACKGROUND_OPA, BUTTON_BACKGROUND_OPA, UIButton::PRESSED);
    key7Button_->SetOnClickListener(this);
    container_->Add(key7Button_);

    key8Button_ = new UILabelButton();
    if (key8Button_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuDialPage key8Button_ new fail");
        return;
    }
    key8Button_->SetPosition(SECOND_COLUMN_COLOR_BUTTON_POS_X, THIRD_ROW_COLOR_BUTTON_POS_Y, DIAL_NORMAL_BUTTON_WIDTH,
                             DIAL_NORMAL_BUTTON_HEIGHT);
    key8Button_->SetViewId(KEY_8_BUTTON_ID);
    key8Button_->SetText("8");
    key8Button_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, DIAL_FONT_SIZE);
    key8Button_->SetStyle(STYLE_BORDER_RADIUS, DIAL_BUTTON_BORDER_RADIUS);
    key8Button_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    key8Button_->SetStyle(STYLE_BACKGROUND_OPA, BUTTON_BACKGROUND_OPA);
    key8Button_->SetStyleForState(STYLE_BORDER_RADIUS, DIAL_BUTTON_BORDER_RADIUS, UIButton::PRESSED);
    key8Button_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Gray().full, UIButton::PRESSED);
    key8Button_->SetStyleForState(STYLE_BACKGROUND_OPA, BUTTON_BACKGROUND_OPA, UIButton::PRESSED);
    key8Button_->SetOnClickListener(this);
    container_->Add(key8Button_);

    key9Button_ = new UILabelButton();
    if (key9Button_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuDialPage key9Button_ new fail");
        return;
    }
    key9Button_->SetPosition(THIRD_COLUMN_COLOR_BUTTON_POS_X, THIRD_ROW_COLOR_BUTTON_POS_Y, DIAL_NORMAL_BUTTON_WIDTH,
                             DIAL_NORMAL_BUTTON_HEIGHT);
    key9Button_->SetViewId(KEY_9_BUTTON_ID);
    key9Button_->SetText("9");
    key9Button_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, DIAL_FONT_SIZE);
    key9Button_->SetStyle(STYLE_BORDER_RADIUS, DIAL_BUTTON_BORDER_RADIUS);
    key9Button_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    key9Button_->SetStyle(STYLE_BACKGROUND_OPA, BUTTON_BACKGROUND_OPA);
    key9Button_->SetStyleForState(STYLE_BORDER_RADIUS, DIAL_BUTTON_BORDER_RADIUS, UIButton::PRESSED);
    key9Button_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Gray().full, UIButton::PRESSED);
    key9Button_->SetStyleForState(STYLE_BACKGROUND_OPA, BUTTON_BACKGROUND_OPA, UIButton::PRESSED);
    key9Button_->SetOnClickListener(this);
    container_->Add(key9Button_);

    keyPoundButton_ = new UILabelButton();
    if (keyPoundButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuDialPage keyPoundButton_ new fail");
        return;
    }
    keyPoundButton_->SetPosition(FOUR_COLUMN_COLOR_BUTTON_POS_X, THIRD_ROW_COLOR_BUTTON_POS_Y, DIAL_NORMAL_BUTTON_WIDTH,
                                 DIAL_NORMAL_BUTTON_HEIGHT);
    keyPoundButton_->SetViewId(KEY_POUND_BUTTON_ID);
    LOADIMG::LoadBtnImage(keyPoundButton_, PHONEMENU_IMAGE, IMAGE_PHONEMENU_POUND_KEY, IMAGE_PHONEMENU_POUND_KEY);
    keyPoundButton_->SetOnClickListener(this);
    container_->Add(keyPoundButton_);

    key0Button_ = new UILabelButton();
    if (key0Button_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuDialPage key0Button_ new fail");
        return;
    }
    key0Button_->SetPosition(FIRST_COLUMN_COLOR_BUTTON_POS_X, FOUR_ROW_COLOR_BUTTON_POS_Y, DIAL_WIDE_BUTTON_WIDTH,
                             DIAL_WIDE_BUTTON_HEIGHT);
    key0Button_->SetViewId(KEY_0_BUTTON_ID);
    key0Button_->SetText("0");
    key0Button_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, DIAL_FONT_SIZE);
    key0Button_->SetStyle(STYLE_BORDER_RADIUS, DIAL_BUTTON_BORDER_RADIUS);
    key0Button_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    key0Button_->SetStyle(STYLE_BACKGROUND_OPA, BUTTON_BACKGROUND_OPA);
    key0Button_->SetStyleForState(STYLE_BORDER_RADIUS, DIAL_BUTTON_BORDER_RADIUS, UIButton::PRESSED);
    key0Button_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Gray().full, UIButton::PRESSED);
    key0Button_->SetStyleForState(STYLE_BACKGROUND_OPA, BUTTON_BACKGROUND_OPA, UIButton::PRESSED);
    key0Button_->SetOnClickListener(this);
    container_->Add(key0Button_);

    diagButton_ = new UILabelButton();
    if (diagButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuDialPage diagButton_ new fail");
        return;
    }
    diagButton_->SetPosition(THIRD_COLUMN_COLOR_BUTTON_POS_X, FOUR_ROW_COLOR_BUTTON_POS_Y, DIAL_WIDE_BUTTON_WIDTH,
                             DIAL_WIDE_BUTTON_HEIGHT);
    diagButton_->SetViewId(DIAL_BUTTON_ID);
    diagButton_->SetStyle(STYLE_BORDER_RADIUS, DIAL_BUTTON_BORDER_RADIUS);
    LOADIMG::LoadBtnImage(diagButton_, PHONEMENU_IMAGE, IMAGE_PHONEMENU_CALL, IMAGE_PHONEMENU_CALL);
    diagButton_->SetOnClickListener(this);
    container_->Add(diagButton_);

    AddViewToPageContainer(container_);
}

PhoneMenuDialPage::PhoneMenuDialPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PhoneMenuDialPage::PhoneMenuDialPage");
}

PhoneMenuDialPage::~PhoneMenuDialPage()
{
    if (container_ != nullptr) {
        container_->RemoveAll();
        delete container_;
        container_ = nullptr;
    }
    if (dialNumberLabel_ != nullptr) {
        delete dialNumberLabel_;
        dialNumberLabel_ = nullptr;
    }
    if (deleteButton_ != nullptr) {
        delete deleteButton_;
        deleteButton_ = nullptr;
    }
    if (diagButton_ != nullptr) {
        delete diagButton_;
        diagButton_ = nullptr;
    }
    if (keyStarButton_ != nullptr) {
        delete keyStarButton_;
        keyStarButton_ = nullptr;
    }
    if (keyPoundButton_ != nullptr) {
        delete keyPoundButton_;
        keyPoundButton_ = nullptr;
    }
    if (key0Button_ != nullptr) {
        delete key0Button_;
        key0Button_ = nullptr;
    }
    if (key1Button_ != nullptr) {
        delete key1Button_;
        key1Button_ = nullptr;
    }
    if (key2Button_ != nullptr) {
        delete key2Button_;
        key2Button_ = nullptr;
    }
    if (key3Button_ != nullptr) {
        delete key3Button_;
        key3Button_ = nullptr;
    }
    if (key4Button_ != nullptr) {
        delete key4Button_;
        key4Button_ = nullptr;
    }
    if (key5Button_ != nullptr) {
        delete key5Button_;
        key5Button_ = nullptr;
    }
    if (key6Button_ != nullptr) {
        delete key6Button_;
        key6Button_ = nullptr;
    }
    if (key7Button_ != nullptr) {
        delete key7Button_;
        key7Button_ = nullptr;
    }
    if (key8Button_ != nullptr) {
        delete key8Button_;
        key8Button_ = nullptr;
    }
    if (key9Button_ != nullptr) {
        delete key9Button_;
        key9Button_ = nullptr;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PhoneMenuDialPage::~PhoneMenuDialPage");
}

bool PhoneMenuDialPage::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
            NativeAbility::GetInstance().SwitchPageInSlice(PhoneMenuPages::PHONE_MENU_MAIN_PAGE,
                TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool PhoneMenuDialPage::OnClick(UIView &view, const ClickEvent &event)
{
    UILabelButton *button = dynamic_cast<UILabelButton *>(&view);
    if (button == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuDialPage::OnClick dynamic_cast view fail");
        return false;
    }
    if (strcmp(view.GetViewId(), DELETE_BUTTON_ID) == 0) {
        if (!dialNumber_.empty()) {
            dialNumber_.pop_back();
            dialNumberLabel_->SetText(dialNumber_.c_str());
            dialNumberLabel_->Invalidate();
        }
    } else if (strcmp(view.GetViewId(), DIAL_BUTTON_ID) == 0) {
        if (GetBtConnectStatus() != PROFILE_STATE_CONNECTED) {
            NativeAbility::GetInstance().SwitchPageInSlice(PhoneMenuPages::PHONE_MENU_DISCONNECT_PAGE,
                TransitionType::TRANSITION_INVALID, false);
            return false;
        }
        SetMicMute(false);
        PhoneMenuCallerLogModel::GetInstance()->SetMicMute(false);
        if (DialCall((unsigned char *)dialNumber_.c_str(), dialNumber_.size()) != OHOS_SUCCESS) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuDialPage::OnClick DialCall[fail]");
        }
        PhoneMenuCallerLogModel::GetInstance()->SaveCurPage();
    } else {
        if (strcmp(view.GetViewId(), KEY_STAR_BUTTON_ID) == 0) {
            dialNumber_ += "*";
        } else if (strcmp(view.GetViewId(), KEY_POUND_BUTTON_ID) == 0) {
            dialNumber_ += "#";
        } else {
            dialNumber_ += button->GetText();
        }
        dialNumberLabel_->SetText(dialNumber_.c_str());
        dialNumberLabel_->Invalidate();
    }
    return true;
}
}