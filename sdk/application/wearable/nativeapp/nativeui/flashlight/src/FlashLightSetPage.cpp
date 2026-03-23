/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: FlashLightSetPage
 * Create: 2025-04-24
 */

#include "AppViewIDs.h"
#include "wearable_log.h"
#include "NativeAbility.h"
#include "main/LoadImg.h"
#include "SlicePageFactory.h"
#include "flashlight/FlashLightView.h"
#include "flashlight/FlashLightModel.h"
#include "flashlight/FlashLightPresenter.h"
#include "flashlight/FlashLightSetPage.h"

namespace OHOS {
static constexpr uint16_t SET_LABEL_POS_X = 117;
static constexpr uint16_t SET_LABEL_POS_Y = 31;
static constexpr uint16_t SET_LABEL_WIDTH = 220;
static constexpr uint16_t SET_LABEL_HEIGHT = 53;
static constexpr uint16_t SET_LABEL_FONT_SIZE = 38;
static constexpr uint16_t SET_LIST_POS_X = 59;
static constexpr uint16_t SET_LIST_POS_Y = 127;
static constexpr uint16_t SET_LIST_WIDTH = 340;
static constexpr uint16_t SET_LIST_HEIGHT = 244;
static constexpr uint16_t RECOVER_BUTTON_POS_X = 120;
static constexpr uint16_t RECOVER_BUTTON_POS_Y = 408;
static constexpr uint16_t RECOVER_BUTTON_WIDTH = 220;
static constexpr uint16_t RECOVER_BUTTON_HEIGHT = 80;
static constexpr uint16_t RECOVER_BUTTON_FONT_SIZE = 36;
static constexpr uint16_t CONTAINER_BLANK_SIZE = 100;
static constexpr uint16_t ITEM_X = 0;
static constexpr uint16_t ITEM_Y = 0;
static constexpr uint16_t ITEM_W = 340;
static constexpr uint16_t ITEM_H = 122;
static constexpr char *COLOR__BUTTON_ID = "setColorItem";
static constexpr char *BLINK_BUTTON_ID = "setBlinkItem";
static constexpr char *RECOVER_BUTTON_ID = "recoverDefault";
static std::string g_blinkItemContent[] = {"慢", "快", "不闪烁"};
static std::string g_colorItemContent[] = {"黑色", "白色",   "红色",   "橙色", "黄色",
                                           "绿色", "浅蓝色", "深蓝色", "紫色", "彩色"};
static FlashLightSetItem g_listItem[] = {
    {IMAGE_FLASHLIGHT_COLORITEM, "灯光颜色", "", ""},
    {IMAGE_FLASHLIGHT_BLINKITEM, "闪烁频率", "", ""}
};
REGIST_SLICE_PAGE(VIEW_FLASHLIGHT, FlashLightAllView::FLASH_LIGHT_SET_VIEW, FlashLightSetPage, false);
static FlashLightSetPage *g_pFlashLightSetPage = nullptr;

static UIView* CreateViewCb(uint8_t type)
{
    FlashLightSetItemView* item = new FlashLightSetItemView();
    if (item == nullptr) {
        return nullptr;
    }

    item->SetPosition(ITEM_X, ITEM_Y, ITEM_W, ITEM_H);
    item->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    item->SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
    item->SetTouchable(true);
    item->SetOnClickListener(g_pFlashLightSetPage);
    return item;
}

static void UpdateViewCb(UIView* view, void* data, uint8_t type)
{
    FlashLightSetItemView* item = dynamic_cast<FlashLightSetItemView*>(view);
    if (view == nullptr || data == nullptr || item == nullptr) {
        return;
    }
    FlashLightSetItem* content = static_cast<FlashLightSetItem *>(data);
    item->SetItemInfo(*content);
}

void FlashLightSetPage::OnStart(void *data)
{
    g_pFlashLightSetPage = this;
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
        container_->SetStyle(STYLE_BACKGROUND_OPA, 0);
        container_->SetScrollBlankSize(CONTAINER_BLANK_SIZE, UIAbstractScroll::Direction::TOP);
        container_->SetScrollBlankSize(CONTAINER_BLANK_SIZE, UIAbstractScroll::Direction::BOTTOM);
    }

    if (titleLabel_ == nullptr) {
        titleLabel_ = new UILabel();
    }
    titleLabel_->SetPosition(SET_LABEL_POS_X, SET_LABEL_POS_Y, SET_LABEL_WIDTH, SET_LABEL_HEIGHT);
    titleLabel_->SetText("手电筒设置");
    titleLabel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    titleLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    titleLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, SET_LABEL_FONT_SIZE);
    container_->Add(titleLabel_);
    titleLabel_->SetVisible(true);

    if (setItemList_ == nullptr) {
        setItemList_ = new UISimpleList();
    }
    setItemList_->SetPosition(SET_LIST_POS_X, SET_LIST_POS_Y, SET_LIST_WIDTH, SET_LIST_HEIGHT);
    setItemList_->SetLoopState(false);
    setItemList_->EnableAutoAlign(true);
    setItemList_->SetBoundaryIntercept(DragEvent::DIRECTION_TOP_TO_BOTTOM, false);
    setItemList_->SetBoundaryIntercept(DragEvent::DIRECTION_BOTTOM_TO_TOP, false);

    uint16_t colorIndex = (uint16_t)FlashLightModel::GetInstance().GetViewColorId();
    uint16_t blinkIndex = (uint16_t)FlashLightModel::GetInstance().GetBlinkFrequency();

    g_listItem[0].itemLabel = g_colorItemContent[colorIndex];
    g_listItem[0].viewId = COLOR__BUTTON_ID;
    g_listItem[1].itemLabel = g_blinkItemContent[blinkIndex];
    g_listItem[1].viewId = BLINK_BUTTON_ID;

    for (uint8_t i = 0; i < (sizeof(g_listItem) / sizeof(FlashLightSetItem)); ++i) {
        Contents content;
        content.createFunc = CreateViewCb;
        content.updateFunc = UpdateViewCb;
        content.data = reinterpret_cast<void *>(&g_listItem[i]);
        setItemList_->AddContent(content);
    }
    setItemList_->RefreshList();

    container_->Add(setItemList_);
    setItemList_->SetVisible(true);

    if (recoverButton_ == nullptr) {
        recoverButton_ = new UILabelButtonExt();
    }
    recoverButton_->SetPosition(RECOVER_BUTTON_POS_X, RECOVER_BUTTON_POS_Y, RECOVER_BUTTON_WIDTH,
                                RECOVER_BUTTON_HEIGHT);
    recoverButton_->SetViewId(RECOVER_BUTTON_ID);
    recoverButton_->SetText("恢复默认\n");
    recoverButton_->SetTextColor(Color::Red());
    recoverButton_->SetAlign(TEXT_ALIGNMENT_CENTER);
    recoverButton_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, RECOVER_BUTTON_FONT_SIZE);
    LOADIMG::LoadBtnImage(recoverButton_, FLASHLIGHT_IMAGE, IMAGE_FLASHLIGHT_RECOVERBUTTON,
                          IMAGE_FLASHLIGHT_RECOVERBUTTON);
    recoverButton_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    recoverButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    container_->Add(recoverButton_);
    recoverButton_->SetVisible(true);

    // 设置界面组件回调函数
    container_->SetOnDragListener(this);
    recoverButton_->SetOnClickListener(this);
    AddViewToPageContainer(container_);
}

FlashLightSetPage::FlashLightSetPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FlashLightSetPage::FlashLightSetPage");
}

FlashLightSetPage::~FlashLightSetPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FlashLightSetPage::~FlashLightSetPage");
    if (container_ != nullptr) {
        container_->RemoveAll();
        delete container_;
        container_ = nullptr;
    }
    if (titleLabel_ != nullptr) {
        delete titleLabel_;
        titleLabel_ = nullptr;
    }
    if (recoverButton_ != nullptr) {
        delete recoverButton_;
        recoverButton_ = nullptr;
    }
    if (setItemList_ != nullptr) {
        setItemList_->ClearAll();
        delete setItemList_;
        setItemList_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadSingleRes(FLASHLIGHT_IMAGE);
}

bool FlashLightSetPage::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().SwitchPageInSlice(FlashLightAllView::FLASH_LIGHT_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool FlashLightSetPage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), COLOR__BUTTON_ID) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(FlashLightAllView::FLASH_LIGHT_COLOR_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    } else if (strcmp(view.GetViewId(), BLINK_BUTTON_ID) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(FlashLightAllView::FLASH_LIGHT_BLINK_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    } else if (strcmp(view.GetViewId(), RECOVER_BUTTON_ID) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(FlashLightAllView::FLASH_LIGHT_RECOVER_VIEW,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}
}
