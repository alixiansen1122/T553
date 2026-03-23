/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef UI_TEST_UI_LIST_H
#define UI_TEST_UI_LIST_H

#include "components/text_adapter.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_list.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"

namespace OHOS {
class UITestUIList : public UITest, UIView::OnClickListener, ListScrollListener {
public:
    UITestUIList() {}
    ~UITestUIList() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    bool OnClick(UIView& view, const ClickEvent& event) override;
    void OnScrollStart(int16_t index, UIView* view) override;
    void OnScrollEnd(int16_t index, UIView* view) override;
    void OnItemSelected(int16_t index, UIView* view) override;
    void UIKitListInitTestFullScreen001();
    void UIKitListInitTesthalhScreen001();
    void UIKitListScrollTestBlankSet001();

private:
    static constexpr int16_t ALINE_TIME_CHANGE_VALUE = 100;
    void SetLastPos(UIView* view);
    void SetUpButton(UILabelButton* btn, const char* title);
    void SetControlButton();
    UILabelButton* setBlankBtn_ = nullptr;
    UILabelButton* setBlankOffBtn_ = nullptr;
    UILabelButton* setThrowDragBtn_ = nullptr;
    UILabelButton* setThrowDragOffBtn_ = nullptr;
    UILabelButton* setRefreshBtn_ = nullptr;
    UILabelButton* setLoopBtn_ = nullptr;
    UILabelButton* setLoopOffBtn_ = nullptr;
    UILabelButton* setSelectBtn_ = nullptr;
    UILabelButton* setSelectOffBtn_ = nullptr;
    UILabelButton* setAutoAlignBtn_ = nullptr;
    UILabelButton* setAutoAlignOffBtn_ = nullptr;
    UILabelButton* setBackAlignBtn_ = nullptr;
    UILabelButton* setBackAlignOffBtn_ = nullptr;
    UILabelButton* setAutoAlignACCIncBtn_ = nullptr;
    UILabelButton* setAutoAlignACCDncBtn_ = nullptr;
    UILabelButton* setYScrollBarVisableBtn_ = nullptr;
    UILabelButton* setYScrollBarInvisableBtn_ = nullptr;
    UILabel* scrollStateLabel_ = nullptr;
    UILabel* scrollSelectLabel_ = nullptr;
    UIScrollView* container_ = nullptr;
    UIScrollView* scroll_ = nullptr;
    UIViewGroup* uiViewGroup1_ = nullptr;
    UIViewGroup* uiViewGroup2_ = nullptr;
    List<const char*>* adapterData1_ = nullptr;
    List<const char*>* adapterData2_ = nullptr;
    TextAdapter* adapter1_ = nullptr;
    TextAdapter* adapter2_ = nullptr;
    TextAdapter* adapter4_ = nullptr;
    UIList* currentList_ = nullptr;
    int16_t lastX_ = 0;
    int16_t lastY_ = 0;
    uint16_t autoAlignTime_ = 100; // 100: default list auto aling ACC
};
} // namespace OHOS
#endif // UI_TEST_UI_LIST_H
