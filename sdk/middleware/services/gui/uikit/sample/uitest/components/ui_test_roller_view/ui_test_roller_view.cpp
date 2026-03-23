/*
 * Copyright (c) @CompanyNameMagicTag. 2024. All rights reserved.
 */

#include "ui_test_roller_view.h"
#include "components/ui_roller_view.h"
#include "common/image_cache_manager.h"
#include "gfx_utils/image_info.h"

namespace OHOS {

struct RollerItemStruct {
    const char* name;
    const char* path;
    ImageInfo* info;
};

static constexpr uint8_t ITEM_CNT = 10;
static RollerItemStruct g_rollerItems[ITEM_CNT] = {
    {"planet", RES_PATH"/planet.bin", nullptr},
    {"kaleidoscope1", RES_PATH"/kaleidoscope.bin", nullptr},
    {"kaleidoscope2", RES_PATH"/kaleidoscope.bin", nullptr},
    {"kaleidoscope3", RES_PATH"/kaleidoscope.bin", nullptr},
    {"kaleidoscope4", RES_PATH"/kaleidoscope.bin", nullptr},
    {"kaleidoscope5", RES_PATH"/kaleidoscope.bin", nullptr},
    {"kaleidoscope6", RES_PATH"/kaleidoscope.bin", nullptr},
    {"kaleidoscope7", RES_PATH"/kaleidoscope.bin", nullptr},
    {"kaleidoscope8", RES_PATH"/kaleidoscope.bin", nullptr},
    {"rainbow", RES_PATH"/rainbow.bin", nullptr}
};

void UITestRollerView::CreateButton(UILabelButton*& button, const char* text, int16_t x, int16_t y)
{
    button = new UILabelButton();
    button->SetPosition(x, y, 100, 40); // 100: width, 40: height
    button->SetText(text);
    button->SetOnClickListener(this);
    button->SetVisible(isBtnVisible_);
    container_->Add(button);
}

bool UITestRollerView::ChildStateListener::StateChange(UIView* view, float angle)
{
    if (view == nullptr) {
        return true;
    }
    UIViewGroup* group = dynamic_cast<UIViewGroup*>(view);
    UIImageView* head = dynamic_cast<UIImageView*>(group->GetChildrenHead());
    UIImageView* tail = dynamic_cast<UIImageView*>(group->GetChildrenTail());
    if (FloatMore(angle, 90.0f) && FloatLess(angle, 270.0f)) { // 90 270:rotate range
        head->SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE / 2); // 2: hald of opa
        if (roller_->IsShowMirrorMode()) {
            tail->SetStyle(STYLE_IMAGE_OPA, 50); // 50: hald of opa
        }
    } else {
        head->SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
        if (roller_->IsShowMirrorMode()) {
            tail->SetStyle(STYLE_IMAGE_OPA, 100); // 100: hald of opa
        }
    }
    return true;
}
void UITestRollerView::CreateNoMirrorRollerView()
{
    noMirrorRollerView_ = new UIRollerView(false);
    noMirrorRollerView_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    noMirrorRollerView_->SetItemSize(150, 150); // 150: item size
    noMirrorRollerView_->SetThrowDrag(true);
    noMirrorRollerView_->SetItemPadding(80); // 80: item padding
    noMirrorRollerView_->SetCamDistance(380);  // 380: Camer distance
    noMirrorRollerView_->SetCamY(48);          // 48: Camer Y position
    for (uint8_t i = 0; i < ITEM_CNT; i++) {
        if (g_rollerItems[i].info == nullptr) {
            g_rollerItems[i].info = ImageCacheManager::GetInstance().LoadSingleRes(g_rollerItems[i].path);
        }
        noMirrorRollerView_->AddItem(g_rollerItems[i].name, g_rollerItems[i].info);
    }
    noMirrorRollerView_->SetCurrentItem("rainbow", false);
}

void UITestRollerView::SetUp()
{
    CreateNoMirrorRollerView();

    container_ = new UIViewGroup();
    container_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    container_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    container_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);

    rollerView_ = new UIRollerView();
    rollerView_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    rollerView_->SetItemSize(150, 150); // 150: item size
    rollerView_->SetThrowDrag(true);
    rollerView_->SetItemPadding(80); // 80: item padding
    if (stateChangeListener_ == nullptr) {
        stateChangeListener_ = new ChildStateListener();
        stateChangeListener_->SetRollerView(rollerView_);
        rollerView_->SetChildStateListener(stateChangeListener_);
    }
    container_->Add(rollerView_);
    curRollerView_ = rollerView_;
    for (uint8_t i = 0; i < ITEM_CNT; i++) {
        if (g_rollerItems[i].info == nullptr) {
            g_rollerItems[i].info = ImageCacheManager::GetInstance().LoadSingleRes(g_rollerItems[i].path);
        }
        rollerView_->AddItem(g_rollerItems[i].name, g_rollerItems[i].info);
    }

    rollerView_->SetCurrentItem("rainbow", false);
    rollerView_->SetOnClickListener(this);
    rollerView_->SetOnModeChangeListener(this);

    CreateButton(removeCurrentBtn_, "remove", 60, 320); // 60, 320: x y
    CreateButton(addBackBtn_, "add", 180, 320); // 180, 320: x y
    CreateButton(clearBtn_, "clear", 300, 320); // 300, 320: x y
    CreateButton(alignBtn_, "align", 60, 370); // 60, 370: x y
    CreateButton(switchAnimBtn_, "switchAnim", 180, 370); // 180, 370: x y
    CreateButton(switchBtn_, "switch", 300, 370); // 300, 370: x y
    CreateButton(offMirrorBtn_, "offMirror", 180, 420); // 180, 420: x y
}

void UITestRollerView::HideOrShowBtn()
{
    bool isVisible = (isBtnVisible_) ? false : true;
    removeCurrentBtn_->SetVisible(isVisible);
    addBackBtn_->SetVisible(isVisible);
    clearBtn_->SetVisible(isVisible);
    alignBtn_->SetVisible(isVisible);
    switchAnimBtn_->SetVisible(isVisible);
    switchBtn_->SetVisible(isVisible);
    offMirrorBtn_->SetVisible(isVisible);
    isBtnVisible_ = isVisible;
}

void UITestRollerView::OnModeChange(UIRollerView* view, RollerViewMode targetMode)
{
    printf("RollerView Change Mode to %d!\n", targetMode);
}

bool UITestRollerView::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == rollerView_) {
        if (event.GetCurrentPos().y > (view.GetY()+ view.GetHeight() - 50)) { // 50: offset
            HideOrShowBtn();
        } else {
            RollerViewMode mode = rollerView_->GetMode();
            RollerViewMode setMode = (mode == DISPLAY) ? COVER : DISPLAY;
            rollerView_->SetMode(setMode, true);
        }
    } else if (&view == removeCurrentBtn_) {
        rollerView_->RemoveItem(rollerView_->GetCurrentItemName());
    } else if (&view == addBackBtn_) {
        for (uint8_t i = 0; i < ITEM_CNT; i++) {
            if (g_rollerItems[i].info == nullptr) {
                g_rollerItems[i].info = ImageCacheManager::GetInstance().LoadSingleRes(g_rollerItems[i].path);
            }
            rollerView_->AddItem(g_rollerItems[i].name, g_rollerItems[i].info);
        }
    } else if (&view == clearBtn_) {
        rollerView_->ClearAll();
    } else if (&view == alignBtn_) {
        printf("Align kaleidoscope.");
        rollerView_->SetCurrentItem("kaleidoscope5", true);
    } else if (&view == switchBtn_) {
        RollerViewMode mode = rollerView_->GetMode();
        RollerViewMode setMode = (mode == DISPLAY) ? COVER : DISPLAY;
        rollerView_->SetMode(setMode, false);
    } else if (&view == switchAnimBtn_) {
        RollerViewMode mode = rollerView_->GetMode();
        RollerViewMode setMode = (mode == DISPLAY) ? COVER : DISPLAY;
        rollerView_->SetMode(setMode, true);
    } else if (&view == offMirrorBtn_) {
        if (curRollerView_ == rollerView_) {
            container_->Remove(rollerView_);
            container_->Insert(nullptr, noMirrorRollerView_);
            curRollerView_ = noMirrorRollerView_;
        } else {
            container_->Remove(noMirrorRollerView_);
            container_->Insert(nullptr, rollerView_);
            curRollerView_ = rollerView_;
        }
    }
    return true;
}

void UITestRollerView::TearDown()
{
    if (curRollerView_ == rollerView_) {
        container_->Remove(rollerView_);
    } else {
        container_->Remove(noMirrorRollerView_);
    }

    if (stateChangeListener_ != nullptr) {
        delete stateChangeListener_;
        stateChangeListener_ = nullptr;
    }
    curRollerView_ = nullptr;
    delete rollerView_;
    delete noMirrorRollerView_;
    rollerView_ = nullptr;
    noMirrorRollerView_ = nullptr;

    DeleteChildren(container_);
    removeCurrentBtn_ = nullptr;
    addBackBtn_ = nullptr;
    clearBtn_ = nullptr;
    alignBtn_ = nullptr;
    switchAnimBtn_ = nullptr;
    switchBtn_ = nullptr;
    container_ = nullptr;
    offMirrorBtn_ = nullptr;
    isBtnVisible_ = false;
}

const UIView* UITestRollerView::GetTestView()
{
    return container_;
}
}
