/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: UITestTransitionView
 * Create: 2025-04
 */

#include "AppViewIDs.h"
#include "components/ui_scroll_view_nested.h"
#include "components/ui_list_nested.h"
#include "NativeAbility.h"
#include "PageTransitionMgr.h"
#include "TransitionType.h"
#include "UiConfig.h"
#include "ui_test_transition_itemview.h"
#include "ui_test_transition_model.h"
#include "ui_test_transition_view.h"


namespace OHOS {
    static constexpr uint8_t SETTINGS_ITEM_FONT_SIZE = 40;
    static constexpr int16_t SETTINGS_RESIZE_SIZE_WIDTH = 260;
    static constexpr int16_t SETTINGS_RESIZE_SIZE_HEIGHT = 60;
    static constexpr int16_t SETTING_TITLE_POSITION_X = 100;
    static constexpr int16_t SETTING_TITLE_POSITION_Y = 20;
    static constexpr int16_t SETTING_LIST_POSITION_X = 0;
    static constexpr int16_t SETTING_LIST_POSITION_Y = 0;
    static constexpr int16_t SETTING_SCROLLBLANKSIZE = 100;
    static constexpr int16 MAX_SCR_DIS = 1000;
    static constexpr int16 SNUM_2 = 2;
    static constexpr int16 SNUM_4 = 4;

    template <typename T>
    void SafeDelete(T*& ptr)
    {
        if (ptr != nullptr) {
            delete ptr;
            ptr = nullptr;
        }
    }

    UITestTransitionView::~UITestTransitionView()
    {
        if (scrollView_ != nullptr) {
            scrollView_->RemoveAll();
            delete scrollView_;
            scrollView_ = nullptr;
        }

        if (contentList_ != nullptr) {
            delete contentList_;
            contentList_ = nullptr;
        }

        if (listAdapter_ != nullptr) {
            delete listAdapter_;
            listAdapter_ = nullptr;
        }
    }

    void UITestTransitionView::SetUp()
    {
        titleText_.SetPosition(SETTING_TITLE_POSITION_X, SETTING_TITLE_POSITION_Y);
        titleText_.Resize(SETTINGS_RESIZE_SIZE_WIDTH, SETTINGS_RESIZE_SIZE_HEIGHT);
        titleText_.SetText("页面动效");
        titleText_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTINGS_ITEM_FONT_SIZE);
        titleText_.SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        titleText_.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
        titleText_.SetStyle(STYLE_TEXT_COLOR, Color::White().full);
        titleText_.SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
        titleText_.SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);

        contentList_ = new UIListNested(UIList::HORIZONTAL);
        if (contentList_ == nullptr) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SetTransitionView::OnStart::contentList_ is nullptr");
            return;
        }

        contentList_->SetPosition(SETTING_LIST_POSITION_X, SETTING_LIST_POSITION_Y);
        contentList_->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
        contentList_->SetScrollBlankSize(SETTING_SCROLLBLANKSIZE);
        // middle position
        contentList_->SetSelectPosition(HORIZONTAL_RESOLUTION / SNUM_2);
        contentList_->SetDraggable(true);
        contentList_->SetThrowDrag(true);
        contentList_->SetLoopState(false);
        contentList_->EnableAutoAlign(true);
        contentList_->SetMaxScrollDistance(MAX_SCR_DIS);

        listAdapter_ = new UITestTransitionAdapter();
        const std::list<UITestTransitionSample> effectList =
            UITestTransitionModel::GetInstance().GetTransitionListItems();
        if (listAdapter_ == nullptr) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "new SetTransitionAdapter failed...");
            return;
        }
        listAdapter_->SetItemClickListener(this);

        auto it = effectList.begin();
        while (it != effectList.end()) {
            UITestTransitionSample sample;
            UITestTransitionSample first = *it;
            it++;
            UITestTransitionSample second = (it != effectList.end()) ? *it: sample;
            listAdapter_->AddListItem(first, second);
            it++;
        }
        int16_t index = 0;
        contentList_->SetAdapter(listAdapter_);
        contentList_->ScrollTo(index);
        contentList_->ScrollBy(SNUM_2);
        contentList_->RefreshList();

        // scroll view
        scrollView_ = new UIScrollViewNested(UIAbstractScroll::HORIZONTAL);
        if (scrollView_ == nullptr) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SetTransitionView::OnStart::scrollView_ is nullptr");
            return;
        }
        scrollView_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
        scrollView_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
        scrollView_->SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
        scrollView_->SetThrowDrag(true);

        if (PageTransitionMgr::GetInstance().GetTopSlideBackImage() == nullptr) {
            // support default slide back firstly
            scrollView_->SetOnDragListener(this);
        }
        scrollView_->Add(contentList_);
        scrollView_->Add(&titleText_);
    }

    void UITestTransitionView::TearDown()
    {
        if (scrollView_ != nullptr) {
            scrollView_->RemoveAll();
            delete scrollView_;
            scrollView_ = nullptr;
        }

        SafeDelete(contentList_);
        SafeDelete(listAdapter_);
        SafeDelete(contentList_);
    }

    const UIView *OHOS::UITestTransitionView::GetTestView()
    {
        if (scrollView_ == nullptr) {
            SetUp();
        }
        return scrollView_;
    }

    bool UITestTransitionView::OnClick(UIView &view, const ClickEvent &event)
    {
        UITestTransitionItemView *setTransitionView = static_cast<UITestTransitionItemView *>(&view);
        TransitionType type = static_cast<TransitionType>(setTransitionView->GetClickViewId(event));
        if (type != TransitionType::TRANSITION_INVALID) {
            uint16_t priority = 4;
            NativeAbility::GetInstance().ChangeSlice(VIEW_APPLIST, type, priority, true);
        }
        return false;
    }

    void UITestTransitionView::OnItemSelected(int16_t index, UIView *view)
    {
        UNUSED(index);
        if (view != nullptr) {
            itemViewSelected_ = static_cast<UITestTransitionItemView *>(view);
            itemViewSelected_->SelectedChange();

            if (preItemViewSelected_ != nullptr && preItemViewSelected_ != itemViewSelected_) {
                itemViewSelected_->Reset();
            }
            preItemViewSelected_ = itemViewSelected_;
        }
    }
}