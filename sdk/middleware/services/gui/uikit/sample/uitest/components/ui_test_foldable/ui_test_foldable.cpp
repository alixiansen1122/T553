/*
 * Copyright (c) CompanyNameMagicTag 2025. All rights reserved.
 * Description: UITestFoldable
 * Author:
 * Create: 2025-09
 */

#include "ui_test_foldable.h"
#include "components/ui_transform_group.h"
#include "common/screen.h"
#include "font/ui_font.h"
#include "common/image_cache_manager.h"

namespace OHOS {
namespace {
struct FaceInfo {
    const char* name;
    const char* path;
};

static constexpr uint8_t FOLDABLE_MAX_PAGE_NUM = 6;

UIViewGroup *foldPages_[FOLDABLE_MAX_PAGE_NUM] = {nullptr};
UIImageView *foldPageImages_[FOLDABLE_MAX_PAGE_NUM] = {nullptr};
UIImageView *editMiniButtonImages_[FOLDABLE_MAX_PAGE_NUM][2] = {nullptr}; // 2: left and right mini button
uint8_t pageStates[FOLDABLE_MAX_PAGE_NUM] = {0};
uint8_t pagesOrder[FOLDABLE_MAX_PAGE_NUM] = {0};
ImageInfo *pinStateLabelSrc_ = nullptr;
ImageInfo *pinButtonSrc_ = nullptr;
ImageInfo *cancelPinButtonSrc_ = nullptr;
ImageInfo *deleteButtonSrc_ = nullptr;

static FaceInfo g_faceInfoEdit =
    {"0 foldpage0", RES_PATH"/foldpage0_CS6.bin"};
static FaceInfo g_faceInfo[FOLDABLE_MAX_PAGE_NUM] = {
    {"1 foldpage1", RES_PATH"/foldpage1_CS6.bin"},
    {"2 foldpage2", RES_PATH"/foldpage2_CS6.bin"},
    {"3 foldpage3", RES_PATH"/foldpage3_CS6.bin"},
    {"4 foldpage4", RES_PATH"/foldpage4_CS6.bin"},
    {"5 foldpage5", RES_PATH"/foldpage5_CS6.bin"},
    {"6 foldpage6", RES_PATH"/foldpage6_CS6.bin"}
};
static constexpr const char* DESKTOP_IMAGE_PATH = RES_PATH"/Desktop_CS6.bin";
static constexpr const char* PIN_STATE_LABEL_IMAGE_PATH = RES_PATH"/pinstatelabel_CS6.bin";
static constexpr const char* PIN_BUTTON_IMAGE_PATH = RES_PATH"/pinbutton_CS6.bin";
static constexpr const char* CANCEL_PIN_BUTTON_IMAGE_PATH = RES_PATH"/cancelpinbutton_CS6.bin";
static constexpr const char* DELETE_BUTTON_IMAGE_PATH = RES_PATH"/deletebutton_CS6.bin";

static constexpr int16_t LABEL_FONT_SIZE = 30;
static constexpr uint8_t DIVISOR_TWO = 2;
static constexpr uint16_t PAGE_WIDTH = 360;
static constexpr uint16_t PAGE_HEIGHT = 150;
static constexpr uint16_t EDIT_PAGE_WIDTH = 270;
static constexpr uint16_t EDIT_PAGE_HEIGHT = 60;
static constexpr uint16_t EDIT_FINISH_BUTTON_X = 290;
static constexpr uint16_t EDIT_FINISH_BUTTON_Y = 70;
static constexpr uint16_t EDIT_FINISH_BUTTON_WIDTH = 100;
static constexpr uint16_t EDIT_FINISH_BUTTON_HEIGHT = 50;
static constexpr uint16_t MINI_BUTTON_HEIGHT = 30;
static constexpr uint16_t MINI_BUTTON_WIDTH = 30;
static constexpr uint16_t PIN_STATE_LABEL_WIDTH = 60;
static constexpr uint16_t FOLD_DEFAULT_BG_OPA = 160;
static constexpr uint16_t OVER_SCROLL_DOWN_LIMIT = 100;
static constexpr uint16_t OVER_SCROLL_UP_LIMIT = 100;
static constexpr uint16_t MAX_SCROLL_DIS = 500;

static constexpr uint16_t PAGE_STATE_DEFAULT = 0;
static constexpr uint16_t PAGE_STATE_PINTOP = 1;
static constexpr uint16_t PAGE_STATE_DELETE = 2;
} // namespace

void UITestFoldable::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIViewGroup();
        container_->SetDraggable(true);
        container_->SetOnDragListener(this);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    }
}

void UITestFoldable::TearDown()
{
    if (container_ != nullptr) {
        if (desktop_) {
            ImageCacheManager::GetInstance().UnloadSingleRes(std::string(DESKTOP_IMAGE_PATH));
        }
        if (foldableView_) {
            foldableView_->RemoveAll();
            if (lastFocusView_) {
                lastFocusView_->Remove(focusLabel_);
                delete focusLabel_;
                focusLabel_ = nullptr;
                lastFocusView_ = nullptr;
            }
            DeinitEditMiniButtons();
            for (uint8_t i = 0; i < FOLDABLE_MAX_PAGE_NUM; i++) {
                ImageCacheManager::GetInstance().UnloadSingleRes(std::string(g_faceInfo[i].path));
                UIViewGroup::RemoveAndDeleteAllRecursively(foldPages_[i]);
                foldPages_[i] = nullptr;
                foldPageImages_[i] = nullptr;
            }
            UIViewGroup::RemoveAndDeleteAllRecursively(editPage_);
            editPage_ = nullptr;
            UIViewGroup::RemoveAndDeleteAllRecursively(addCardPage_);
            addCardPage_ = nullptr;
            ImageCacheManager::GetInstance().UnloadSingleRes(std::string(g_faceInfoEdit.path));
        }

        DeleteChildren(container_);
        container_ = nullptr;
        desktop_ = nullptr;
        applistLabel_ = nullptr;
        foldableView_ = nullptr;
        focusLabel_ = nullptr;
        titleLabel_ = nullptr;
        editFinishButton_ = nullptr;
    }
    direction_ = VERTICAL;
    startDragDirection_ = 0;
    curDragYoffset_ = 0;
    startDragYoffset_ = 0;
    maySlidingIn_ = false;
    slidingIn_ = false;
    bottomSlidingOut_ = false;
    topSlidingOut_ = false;
    layoutMode_ = UIFoldableView::LayoutMode::LAYOUT_MODE_FOLD;
}

const UIView* UITestFoldable::GetTestView()
{
    UITestFoldableView();
    return container_;
}

void UITestFoldable::InitDesktopPage()
{
    if (desktop_ == nullptr) {
        desktop_ = new UIImageView();
        ImageInfo* src = ImageCacheManager::GetInstance().LoadSingleRes(std::string(DESKTOP_IMAGE_PATH));
        desktop_->SetViewId("Desktop");
        desktop_->SetSrc(src);
        desktop_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        desktop_->SetPosition(0, 0);
        desktop_->SetResizeMode(UIImageView::ImageResizeMode::FILL);
        desktop_->SetAutoEnable(false);
        desktop_->SetDragParentInstead(true);
        container_->Add(desktop_);
    }
}

void UITestFoldable::InitApplistPage()
{
    if (applistLabel_ == nullptr) {
        applistLabel_ = new UILabel();
        applistLabel_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        applistLabel_->SetTextColor(Color::Blue());
        applistLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        applistLabel_->SetText("应用列表页面\n下滑返回");
        applistLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT_SIZE);
        applistLabel_->SetVisible(false);
        container_->Add(applistLabel_);
    }
}

void UITestFoldable::InitEditFinishButton()
{
    if (editFinishButton_ == nullptr) {
        editFinishButton_ = new UILabelButton();
        editFinishButton_->SetPosition(EDIT_FINISH_BUTTON_X, EDIT_FINISH_BUTTON_Y,
            EDIT_FINISH_BUTTON_WIDTH, EDIT_FINISH_BUTTON_HEIGHT);
        editFinishButton_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
        editFinishButton_->SetTextColor(Color::White());
        editFinishButton_->SetAlign(TEXT_ALIGNMENT_CENTER);
        editFinishButton_->SetText("完成");
        editFinishButton_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT_SIZE);
        editFinishButton_->SetTouchable(true);
        editFinishButton_->SetOnClickListener(this);
        editFinishButton_->SetVisible(false);
        container_->Add(editFinishButton_);
    }
}

void UITestFoldable::InitEditCard()
{
    UIImageView* image = new UIImageView();
    ImageInfo* src = ImageCacheManager::GetInstance().LoadSingleRes(std::string(g_faceInfoEdit.path));
    image->SetViewId(g_faceInfoEdit.name);
    image->SetSrc(src);
    image->Resize(EDIT_PAGE_WIDTH, EDIT_PAGE_HEIGHT);
    image->SetResizeMode(UIImageView::ImageResizeMode::FILL);
    image->SetAutoEnable(false);
    editPage_ = new UITransformGroup();
    editPage_->SetViewId(g_faceInfoEdit.name);
    editPage_->SetStyle(STYLE_BACKGROUND_OPA, 0);
    editPage_->SetDragParentInstead(true);
    editPage_->SetTouchable(true);
    editPage_->SetOnClickListener(this);
    editPage_->SetPosition(0, 0, EDIT_PAGE_WIDTH, EDIT_PAGE_HEIGHT);
    editPage_->Add(image);
    image->LayoutCenterOfParent();
    foldableView_->Add(editPage_);
}

void UITestFoldable::InitAddPageCard()
{
    addCardPage_ = new UITransformGroup();
    addCardPage_->SetStyle(STYLE_BACKGROUND_OPA, 0);
    addCardPage_->SetDragParentInstead(true);
    addCardPage_->SetTouchable(true);
    addCardPage_->SetOnClickListener(this);
    addCardPage_->SetPosition(0, 0, PAGE_WIDTH - MINI_BUTTON_WIDTH, PAGE_HEIGHT - MINI_BUTTON_HEIGHT);

    UILabel *label = new UILabel();
    label->Resize(PAGE_WIDTH, PAGE_HEIGHT);
    label->SetStyle(STYLE_BACKGROUND_OPA, 20); // 20: backgroud opacity
    label->SetTextColor(Color::White());
    label->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    label->SetText("+");
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 100); // 100: font size

    addCardPage_->Add(label);
    label->LayoutCenterOfParent();
}

void UITestFoldable::InitEditMiniButtons()
{
    pinStateLabelSrc_ = ImageCacheManager::GetInstance().LoadSingleRes(std::string(PIN_STATE_LABEL_IMAGE_PATH));
    pinButtonSrc_ = ImageCacheManager::GetInstance().LoadSingleRes(std::string(PIN_BUTTON_IMAGE_PATH));
    cancelPinButtonSrc_ = ImageCacheManager::GetInstance().LoadSingleRes(std::string(CANCEL_PIN_BUTTON_IMAGE_PATH));
    deleteButtonSrc_ = ImageCacheManager::GetInstance().LoadSingleRes(std::string(DELETE_BUTTON_IMAGE_PATH));

    for (uint8_t i = 0; i < FOLDABLE_MAX_PAGE_NUM; i++) {
        editMiniButtonImages_[i][0] = new UIImageView();
        editMiniButtonImages_[i][0]->SetSrc(deleteButtonSrc_);
        editMiniButtonImages_[i][0]->Resize(MINI_BUTTON_WIDTH, MINI_BUTTON_HEIGHT);
        editMiniButtonImages_[i][0]->SetResizeMode(UIImageView::ImageResizeMode::FILL);
        editMiniButtonImages_[i][0]->SetAutoEnable(false);
        editMiniButtonImages_[i][0]->SetTouchable(true);
        editMiniButtonImages_[i][0]->SetOnClickListener(this);
        editMiniButtonImages_[i][0]->SetVisible(false);

        editMiniButtonImages_[i][1] = new UIImageView();
        editMiniButtonImages_[i][1]->SetSrc(pinButtonSrc_);
        editMiniButtonImages_[i][1]->Resize(MINI_BUTTON_WIDTH, MINI_BUTTON_HEIGHT);
        editMiniButtonImages_[i][1]->SetResizeMode(UIImageView::ImageResizeMode::FILL);
        editMiniButtonImages_[i][1]->SetAutoEnable(false);
        editMiniButtonImages_[i][1]->SetTouchable(true);
        editMiniButtonImages_[i][1]->SetOnClickListener(this);
        editMiniButtonImages_[i][1]->SetVisible(false);
        pageStates[i] = PAGE_STATE_DEFAULT;
        pagesOrder[i] = i;
    }
}

void UITestFoldable::DeinitEditMiniButtons()
{
    for (uint8_t i = 0; i < FOLDABLE_MAX_PAGE_NUM; i++) {
        (dynamic_cast<UIViewGroup*>(editMiniButtonImages_[i][0]->GetParent()))->Remove(editMiniButtonImages_[i][0]);
        delete editMiniButtonImages_[i][0];
        editMiniButtonImages_[i][0] = nullptr;

        (dynamic_cast<UIViewGroup*>(editMiniButtonImages_[i][1]->GetParent()))->Remove(editMiniButtonImages_[i][1]);
        delete editMiniButtonImages_[i][1];
        editMiniButtonImages_[i][1] = nullptr;
    }

    ImageCacheManager::GetInstance().UnloadSingleRes(std::string(PIN_STATE_LABEL_IMAGE_PATH));
    pinStateLabelSrc_ = nullptr;
    ImageCacheManager::GetInstance().UnloadSingleRes(std::string(PIN_BUTTON_IMAGE_PATH));
    pinButtonSrc_ = nullptr;
    ImageCacheManager::GetInstance().UnloadSingleRes(std::string(CANCEL_PIN_BUTTON_IMAGE_PATH));
    cancelPinButtonSrc_ = nullptr;
    ImageCacheManager::GetInstance().UnloadSingleRes(std::string(DELETE_BUTTON_IMAGE_PATH));
    deleteButtonSrc_ = nullptr;
}


void UITestFoldable::InitFlodableViewCards()
{
    InitEditCard();
    InitAddPageCard();
    InitEditMiniButtons();
    for (uint8_t i = 0; i < FOLDABLE_MAX_PAGE_NUM; i++) {
        foldPageImages_[i] = new UIImageView();
        ImageInfo* src = ImageCacheManager::GetInstance().LoadSingleRes(std::string(g_faceInfo[i].path));
        foldPageImages_[i]->SetViewId(g_faceInfo[i].name);
        foldPageImages_[i]->SetSrc(src);
        foldPageImages_[i]->Resize(PAGE_WIDTH, PAGE_HEIGHT);
        foldPageImages_[i]->SetResizeMode(UIImageView::ImageResizeMode::FILL);
        foldPageImages_[i]->SetAutoEnable(false);

        UITransformGroup* group = new UITransformGroup();
        group->SetViewId(g_faceInfo[i].name);
        group->SetStyle(STYLE_BACKGROUND_OPA, 0);
        group->SetDragParentInstead(true);
        group->SetTouchable(true);
        group->SetOnClickListener(this);
        group->SetOnLongPressListener(this);
        group->SetPosition(0, 0, PAGE_WIDTH, PAGE_HEIGHT);
        group->Add(foldPageImages_[i]);
        group->Add(editMiniButtonImages_[i][0]);
        group->Add(editMiniButtonImages_[i][1]);
        foldPageImages_[i]->LayoutCenterOfParent();
        editMiniButtonImages_[i][0]->LayoutLeftOfParent();
        editMiniButtonImages_[i][1]->LayoutRightOfParent();
        foldableView_->Add(group);
        foldPages_[i] = group;
    }
}

void UITestFoldable::InitFlodableView()
{
    if (foldableView_ == nullptr) {
        foldableView_ = new UIFoldableView();
        foldableView_->SetPosition(0, 0,
            Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        foldableView_->SetIntercept(true);
        foldableView_->SetThrowDrag(true);
        foldableView_->SetMaxPageNum(10); // 10: max page num
        foldableView_->SetPageSize(PAGE_WIDTH, PAGE_HEIGHT);
        foldableView_->SetFoldableScrollListener(this);
        foldableView_->SetDragParentInstead(true);
        foldableView_->SetVisible(false);
        foldableView_->SetFoldScaleGradient(0.2); // 0.2 scale gradient
        foldableView_->SetFoldOpacityChange(OPA_OPAQUE, 155, 10); // 155 min fold page opacity, 10 opacity gradient
        foldableView_->SetExpandPagesMargin(15); // 15 default blank space between expand pages
        foldableView_->SetMaxScrollDistance(MAX_SCROLL_DIS);
        foldableView_->SetStyle(STYLE_BACKGROUND_OPA, FOLD_DEFAULT_BG_OPA);
        InitFlodableViewCards();
        container_->Add(foldableView_);

        if (focusLabel_ == nullptr) {
            focusLabel_ = new UILabel();
            focusLabel_->SetPosition(0, 0, PAGE_WIDTH, PAGE_HEIGHT);
            focusLabel_->SetTextColor(Color::Green());
            focusLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_TOP);
            focusLabel_->SetText("focus");
            focusLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT_SIZE);
        }
        if (titleLabel_ == nullptr) {
            titleLabel_ = new UILabel();
            titleLabel_->SetPosition(127, 50, 200, 50); // 127 50 200 50: label position
            titleLabel_->SetTextColor(Color::White());
            titleLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
            titleLabel_->SetText("foldable sample");
            titleLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT_SIZE);
            titleLabel_->SetVisible(false);
            container_->Add(titleLabel_);
        }
        foldableView_->SetDragYOffInFoldMode(foldableView_->GetHeight() / DIVISOR_TWO);
    }
}

void UITestFoldable::UITestFoldableView()
{
    if (container_ != nullptr) {
        InitDesktopPage();
        InitApplistPage();
        InitFlodableView();
        InitEditFinishButton();
    }
}

void UITestFoldable::OnReLayoutInFoldMode()
{
    int16_t topPageY = foldableView_->GetTopPageY();
    int16_t dragYoffset = foldableView_->GetDragYOffInFoldMode();
    GRAPHIC_LOGD("top page y(%d) changed in fold mode, dragYOff = %d.", topPageY, dragYoffset);
    if (titleLabel_ == nullptr) {
        return;
    }
    titleLabel_->SetY(topPageY - 150); // 150: label position
    if (dragYoffset > 0) {
        if (dragYoffset > Screen::GetInstance().GetHeight()) {
            return;
        }
        float coefficient = (float)dragYoffset / Screen::GetInstance().GetHeight();
        foldableView_->SetStyle(STYLE_BACKGROUND_OPA, FOLD_DEFAULT_BG_OPA - FOLD_DEFAULT_BG_OPA * coefficient);
        titleLabel_->SetOpaScale((uint8_t)(OPA_OPAQUE - OPA_OPAQUE * coefficient));
        return;
    }
    foldableView_->SetStyle(STYLE_BACKGROUND_OPA, FOLD_DEFAULT_BG_OPA);
    foldableView_->SetOpaScale(OPA_OPAQUE);
    titleLabel_->SetOpaScale(OPA_OPAQUE);
}

void UITestFoldable::OnReLayoutInFlatMode()
{
    GRAPHIC_LOGD("top page y(%d) changed in flat mode.", foldableView_->GetTopPageY());
}

void UITestFoldable::OnEntranceAnimatorStop()
{
    GRAPHIC_LOGD("OnEntranceAnimatorStop.");
    maySlidingIn_ = false;
    slidingIn_ = false;
}

void UITestFoldable::OnSlideAnimatorStop()
{
    GRAPHIC_LOGD("OnSlideAnimatorStop.");
    if (topSlidingOut_) {
        foldableView_->SetVisible(false);
        titleLabel_->SetVisible(false);
        applistLabel_->SetVisible(true);
        topSlidingOut_ = false;
    } else if (bottomSlidingOut_) {
        foldableView_->SetVisible(false);
        titleLabel_->SetVisible(false);
        bottomSlidingOut_ = false;
    }
}

void UITestFoldable::OnMoveChildAnimatorStop()
{
    GRAPHIC_LOGD("OnMoveChildAnimatorStop.");
}

void UITestFoldable::OnRemoveChildAnimatorStop()
{
    GRAPHIC_LOGD("OnRemoveChildAnimatorStop.");
}

void UITestFoldable::OnSwitchingLayout(UIFoldableView::LayoutMode targetMode, float progress)
{
    GRAPHIC_LOGD("OnSwitchingLayout.");
    if (targetMode == UIFoldableView::LayoutMode::LAYOUT_MODE_FLAT) {
        int16_t pageWidth = PAGE_WIDTH - MINI_BUTTON_WIDTH * progress;
        int16_t pageHeight = PAGE_HEIGHT - MINI_BUTTON_HEIGHT * progress;
        for (int16_t i = 0; i < FOLDABLE_MAX_PAGE_NUM; i++) {
            foldPageImages_[i]->Resize(pageWidth, pageHeight);
            foldPageImages_[i]->LayoutCenterOfParent();
        }
        editFinishButton_->SetOpaScale(OPA_OPAQUE * progress);
    } else {
        int16_t pageWidth = PAGE_WIDTH - MINI_BUTTON_WIDTH * (1 - progress);
        int16_t pageHeight = PAGE_HEIGHT - MINI_BUTTON_HEIGHT * (1 - progress);
        for (int16_t i = 0; i < FOLDABLE_MAX_PAGE_NUM; i++) {
            foldPageImages_[i]->Resize(pageWidth, pageHeight);
            foldPageImages_[i]->LayoutCenterOfParent();
        }
        titleLabel_->SetOpaScale(OPA_OPAQUE * progress);
    }
}

void UITestFoldable::OnSwitchLayoutAnimatorStop()
{
    GRAPHIC_LOGD("OnSwitchLayoutAnimatorStop.");
    if (layoutMode_ == UIFoldableView::LayoutMode::LAYOUT_MODE_FLAT) {
        for (int16_t i = 0; i < FOLDABLE_MAX_PAGE_NUM; i++) {
            editMiniButtonImages_[i][0]->SetVisible(true);
            editMiniButtonImages_[i][1]->SetVisible(true);
        }
    }
}

bool UITestFoldable::OnDragStart(UIView& view, const DragEvent& event)
{
    if (MATH_ABS(event.GetDeltaX()) < MATH_ABS(event.GetDeltaY())) {
        direction_ = VERTICAL;
    } else {
        direction_ = HORIZONTAL;
        GRAPHIC_LOGW("drag on horizontal is not support.\n");
    }
    if (&view != container_ || direction_ != VERTICAL) {
        return false;
    }

    curDragYoffset_ = 0;
    if (layoutMode_ == UIFoldableView::LayoutMode::LAYOUT_MODE_FOLD) {
        maySlidingIn_ = false;
        if (slidingIn_ || bottomSlidingOut_ || topSlidingOut_) {
            return true;
        }
        if (event.GetDeltaY() > 0) {
            startDragDirection_ = DragEvent::DIRECTION_TOP_TO_BOTTOM;
        } else {
            startDragDirection_ = DragEvent::DIRECTION_BOTTOM_TO_TOP;
        }
        startDragYoffset_ = foldableView_->GetDragYOffInFoldMode();
        if (foldableView_->IsVisible() == false) {
            if (startDragDirection_ == DragEvent::DIRECTION_BOTTOM_TO_TOP &&
                startDragYoffset_ > 0) {
                dragFoldableView_ = false;
                maySlidingIn_ = true;
                return true;
            } else if (startDragDirection_ == DragEvent::DIRECTION_TOP_TO_BOTTOM &&
                startDragYoffset_ < foldableView_->GetDragToEndYOff()) {
                dragFoldableView_ = false;
                maySlidingIn_ = true;
            }
        }
    }

    if (foldableView_->IsVisible() == false) {
        dragFoldableView_ = false;
        return true;
    }

    dragFoldableView_ = true;
    return foldableView_->OnDragStartEvent(event);
}

bool UITestFoldable::OnDrag(UIView& view, const DragEvent& event)
{
    if (&view != container_ || direction_ != VERTICAL) {
        return false;
    }

    curDragYoffset_ += event.GetDeltaY();
    if (layoutMode_ == UIFoldableView::LayoutMode::LAYOUT_MODE_FOLD) {
        if (slidingIn_ || bottomSlidingOut_ || topSlidingOut_) {
            return true;
        }
        if (maySlidingIn_) {
            if (curDragYoffset_ <= -OVER_SCROLL_DOWN_LIMIT) {
                titleLabel_->SetVisible(true);
                foldableView_->SetVisible(true);
                slidingIn_ = foldableView_->StartEntranceAnimator();
                return true;
            } else if (curDragYoffset_ > OVER_SCROLL_UP_LIMIT) {
                applistLabel_->SetVisible(false);
                titleLabel_->SetVisible(true);
                foldableView_->SetVisible(true);
                slidingIn_ = foldableView_->StartEntranceAnimator();
                return true;
            }
        } else if (startDragYoffset_ == 0 && startDragDirection_ == DragEvent::DIRECTION_TOP_TO_BOTTOM) {
            if (curDragYoffset_ > OVER_SCROLL_DOWN_LIMIT) {
                bottomSlidingOut_ = foldableView_->StartSlideAnimator(foldableView_->GetHeight() / DIVISOR_TWO -
                    foldableView_->GetDragYOffInFoldMode());
                return true;
            }
        } else if (curDragYoffset_ + startDragYoffset_ <
            foldableView_->GetDragToEndYOff() - OVER_SCROLL_UP_LIMIT) {
            topSlidingOut_ = foldableView_->StartSlideAnimator(foldableView_->GetDragToEndYOff() -
                foldableView_->GetHeight() - foldableView_->GetDragYOffInFoldMode());
            return true;
        }
    }

    if (!dragFoldableView_) {
        return true;
    }

    return foldableView_->OnDragEvent(event);
}

bool UITestFoldable::OnDragEnd(UIView& view, const DragEvent& event)
{
    if (&view != container_ || direction_ != VERTICAL) {
        return false;
    }

    if (layoutMode_ == UIFoldableView::LayoutMode::LAYOUT_MODE_FOLD) {
        if (slidingIn_ || bottomSlidingOut_ || topSlidingOut_) {
            if (dragFoldableView_) {
                foldableView_->OnDragEndEvent(event);
            }
            return true;
        }
        if (startDragYoffset_ == 0 && startDragDirection_ == DragEvent::DIRECTION_TOP_TO_BOTTOM) {
            int16_t dragDistanceY =
                foldableView_->CalculateDragThrowYDistance(event.GetCurrentPos(), event.GetPreLastPoint());
            if (foldableView_->GetDragYOffInFoldMode() + dragDistanceY > OVER_SCROLL_DOWN_LIMIT) {
                bottomSlidingOut_ = foldableView_->StartSlideAnimator(foldableView_->GetHeight() / DIVISOR_TWO -
                    foldableView_->GetDragYOffInFoldMode());
                return true;
            }
        } else if (startDragYoffset_ <= 0) {
            int16_t dragDistanceY =
                foldableView_->CalculateDragThrowYDistance(event.GetCurrentPos(), event.GetPreLastPoint());
            if (foldableView_->GetDragYOffInFoldMode() + dragDistanceY <
                foldableView_->GetDragToEndYOff() - OVER_SCROLL_UP_LIMIT) {
                topSlidingOut_ = foldableView_->StartSlideAnimator(foldableView_->GetDragToEndYOff() -
                    foldableView_->GetHeight() - foldableView_->GetDragYOffInFoldMode());
            }
        }
    }

    if (!dragFoldableView_) {
        return true;
    }

    return foldableView_->OnDragEndEvent(event);
}

bool UITestFoldable::OnClickInFoldMode(UIView& view, const ClickEvent& event)
{
    if (&view == editPage_) {
        titleLabel_->SetVisible(false);
        editFinishButton_->SetVisible(true);
        editFinishButton_->SetOpaScale(OPA_TRANSPARENT);

        layoutMode_ = UIFoldableView::LayoutMode::LAYOUT_MODE_FLAT;
        foldableView_->StartSwitchLayoutAnimator(layoutMode_,
            UIFoldableView::SwitchAction::SWITCH_ADDTAIL_DELHEAD, addCardPage_);
        return true;
    }

    for (int16_t i = 0; i < FOLDABLE_MAX_PAGE_NUM; i++) {
        if (&view == foldPages_[i]) {
            if (lastFocusView_) {
                lastFocusView_->Remove(focusLabel_);
            }
            if (lastFocusView_ != foldPages_[i]) {
                foldPages_[i]->Add(focusLabel_);
                lastFocusView_ = foldPages_[i];
            } else {
                lastFocusView_ = NULL;
            }
            foldableView_->Invalidate();
        }
    }
    return true;
}

void UITestFoldable::PinPageOnTop(int16_t id)
{
    for (int16_t i = FOLDABLE_MAX_PAGE_NUM - 1; i >= 0; i--) {
        if (pageStates[pagesOrder[i]] != PAGE_STATE_DEFAULT) {
            continue;
        }
        if (pagesOrder[i] == id) {
            pageStates[id] = PAGE_STATE_PINTOP;
            break;
        }
        foldableView_->StartMoveChildAnimator(foldPages_[pagesOrder[i]], foldPages_[id]);
        int16_t tmp = pagesOrder[i];
        for (int16_t j = i - 1; j >= 0; j--) {
            if (pagesOrder[j] == id) {
                pagesOrder[j] = tmp;
                pagesOrder[i] = id;
                pageStates[id] = PAGE_STATE_PINTOP;
                break;
            }
            int tmp1 = tmp;
            tmp = pagesOrder[j];
            pagesOrder[j] = tmp1;
        }
        break;
    }
    editMiniButtonImages_[id][0]->SetSrc(pinStateLabelSrc_);
    editMiniButtonImages_[id][0]->Resize(PIN_STATE_LABEL_WIDTH, MINI_BUTTON_HEIGHT);
    editMiniButtonImages_[id][1]->SetSrc(cancelPinButtonSrc_);
    editMiniButtonImages_[id][0]->Invalidate();
    editMiniButtonImages_[id][1]->Invalidate();
    for (int16_t i = FOLDABLE_MAX_PAGE_NUM - 1, j = 0; i >= 0; i--, j++) {
        GRAPHIC_LOGD("the position %d layout card %d state = %d", j, pagesOrder[i], pageStates[pagesOrder[i]]);
    }
}

void UITestFoldable::CancelPinPageOnTop(int16_t id)
{
    for (int16_t i = FOLDABLE_MAX_PAGE_NUM - 1; i >= 0; i--) {
        if (pagesOrder[i] != id) {
            continue;
        }
        for (int16_t j = i; j >= 0; j--) {
            if (j == 0 || pageStates[pagesOrder[j - 1]] == PAGE_STATE_DELETE) {
                foldableView_->StartMoveChildAnimator(nullptr, foldPages_[id]);
                pagesOrder[j] = id;
                pageStates[id] = PAGE_STATE_DEFAULT;
                break;
            } else if (pageStates[pagesOrder[j - 1]] == PAGE_STATE_PINTOP) {
                pagesOrder[j] = pagesOrder[j - 1];
            } else {
                foldableView_->StartMoveChildAnimator(foldPages_[pagesOrder[j - 1]], foldPages_[id]);
                pagesOrder[j] = id;
                pageStates[id] = PAGE_STATE_DEFAULT;
                break;
            }
        }
        break;
    }
    editMiniButtonImages_[id][0]->SetSrc(deleteButtonSrc_);
    editMiniButtonImages_[id][0]->Resize(MINI_BUTTON_WIDTH, MINI_BUTTON_HEIGHT);
    editMiniButtonImages_[id][1]->SetSrc(pinButtonSrc_);
    editMiniButtonImages_[id][0]->Invalidate();
    editMiniButtonImages_[id][1]->Invalidate();
    for (int16_t i = FOLDABLE_MAX_PAGE_NUM - 1, j = 0; i >= 0; i--, j++) {
        GRAPHIC_LOGD("the position %d layout card %d state = %d", j, pagesOrder[i], pageStates[pagesOrder[i]]);
    }
}

void UITestFoldable::DeletePage(int16_t id)
{
    for (int16_t i = FOLDABLE_MAX_PAGE_NUM - 1; i >= 0; i--) {
        if (pagesOrder[i] == id) {
            foldableView_->StartRemoveChildAnimator(foldPages_[id]);
            for (int j = i; j > 0; j--) {
                pagesOrder[j] = pagesOrder[j - 1];
            }
            pagesOrder[0] = id;
            pageStates[id] = PAGE_STATE_DELETE;
            break;
        }
    }
    for (int16_t i = FOLDABLE_MAX_PAGE_NUM - 1, j = 0; i >= 0; i--, j++) {
        GRAPHIC_LOGD("the position %d layout card %d state = %d", j, pagesOrder[i], pageStates[pagesOrder[i]]);
    }
}

void UITestFoldable::AddPage(int16_t id)
{
    for (int16_t i = FOLDABLE_MAX_PAGE_NUM - 1; i >= 0; i--) {
        if (pageStates[pagesOrder[i]] == PAGE_STATE_PINTOP) {
            continue;
        }
        if (pageStates[pagesOrder[i]] == PAGE_STATE_DEFAULT) {
            foldableView_->Insert(foldPages_[pagesOrder[i]], foldPages_[id]);
        } else {
            foldableView_->Insert(nullptr, foldPages_[id]);
        }
        foldableView_->StartSlideAnimator(foldPages_[id]);
        if (pagesOrder[i] == id) {
            pageStates[id] = PAGE_STATE_DEFAULT;
            break;
        }
        int16_t tmp = pagesOrder[i];
        for (int16_t j = i - 1; j >= 0; j--) {
            if (pagesOrder[j] == id) {
                pagesOrder[j] = tmp;
                pagesOrder[i] = id;
                pageStates[id] = PAGE_STATE_DEFAULT;
                break;
            }
            int16_t tmp1 = tmp;
            tmp = pagesOrder[j];
            pagesOrder[j] = tmp1;
        }
        break;
    }
    for (int16_t i = FOLDABLE_MAX_PAGE_NUM - 1, j = 0; i >= 0; i--, j++) {
        GRAPHIC_LOGD("the position %d layout card %d state = %d", j, pagesOrder[i], pageStates[pagesOrder[i]]);
    }
}

bool UITestFoldable::OnClickInFlatMode(UIView& view, const ClickEvent& event)
{
    if (&view == editFinishButton_) {
        titleLabel_->SetVisible(true);
        titleLabel_->SetOpaScale(OPA_TRANSPARENT);
        editFinishButton_->SetVisible(false);
        for (int16_t i = 0; i < FOLDABLE_MAX_PAGE_NUM; i++) {
            editMiniButtonImages_[i][0]->SetVisible(false);
            editMiniButtonImages_[i][1]->SetVisible(false);
        }
        layoutMode_ = UIFoldableView::LayoutMode::LAYOUT_MODE_FOLD;
        foldableView_->StartSwitchLayoutAnimator(layoutMode_,
            UIFoldableView::SwitchAction::SWITCH_ADDHEAD_DELTAIL, editPage_);
        return true;
    }

    if (&view == addCardPage_) {
        for (int16_t i = 0; i < FOLDABLE_MAX_PAGE_NUM; i++) {
            if (pageStates[i] == PAGE_STATE_DELETE) {
                AddPage(i);
                break;
            }
        }
        return true;
    }

    for (int16_t i = 0; i < FOLDABLE_MAX_PAGE_NUM; i++) {
        if (&view == editMiniButtonImages_[i][0]) {
            if (pageStates[i] == PAGE_STATE_DEFAULT) {
                DeletePage(i);
            } else {
                GRAPHIC_LOGD("pin state button %d clicked.\n", i);
            }
            return true;
        }
        if (&view == editMiniButtonImages_[i][1]) {
            if (pageStates[i] == PAGE_STATE_DEFAULT) {
                PinPageOnTop(i);
            } else {
                CancelPinPageOnTop(i);
            }
            return true;
        }
    }
    return true;
}

bool UITestFoldable::OnClick(UIView& view, const ClickEvent& event)
{
    if (layoutMode_ == UIFoldableView::LayoutMode::LAYOUT_MODE_FOLD) {
        return OnClickInFoldMode(view, event);
    } else {
        return OnClickInFlatMode(view, event);
    }

    return true;
}

bool UITestFoldable::OnLongPress(UIView& view, const LongPressEvent& event)
{
    if (layoutMode_ == UIFoldableView::LayoutMode::LAYOUT_MODE_FOLD) {
        for (int16_t i = 0; i < FOLDABLE_MAX_PAGE_NUM; i++) {
            if (&view == foldPages_[i]) {
                DeletePage(i);
                return true;
            }
        }
    }
    return true;
}
} // namespace OHOS
