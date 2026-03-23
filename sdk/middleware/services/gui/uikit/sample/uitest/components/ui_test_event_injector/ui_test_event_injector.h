/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef UI_TEST_EVENT_INJECTOR_H
#define UI_TEST_EVENT_INJECTOR_H

#include "graphic_config.h"

#include "components/root_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "dock/input_device.h"
#include "ui_test.h"
#include "layout/grid_layout.h"
#include "ui_test_event_injector_view.h"

namespace OHOS {
class UITestEventInjector : public UITest, public UIView::OnClickListener {
public:
    UITestEventInjector() {}
    ~UITestEventInjector() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    void ActionEvent(const UIView& view);
    bool OnClick(UIView& view, const ClickEvent& event) override;

    void UIKitEventInjectorClickEvent001();
    void UIKitEventInjectorDragEvent002();
    void UIKitEventInjectorLongPressEvent003();
    void UIKitEventInjectorKeyEvent004();
    void UIKitEventInjectorUptodown005();
    void UIKitEventInjectorDowntoUp006();
    void UIKitEventInjectorLefttoRight007();
    void UIKitEventInjectorRighttoLeft008();
    void UIKitEventInjectorULefttoLRight009();
    void UIKitEventInjectorLRighttoULeft010();

private:
    UIScrollView* container_ = nullptr;
    GridLayout* layout_ = nullptr;
    UILabelButton* clickBtn_ = nullptr;
    UILabelButton* dragBtn_ = nullptr;
    UILabelButton* longPressBtn_ = nullptr;
    UILabelButton* keyBtn_ = nullptr;
    UILabelButton* upToDownBtn_ = nullptr;
    UILabelButton* downToUpBtn_ = nullptr;
    UILabelButton* leftToRightBtn_ = nullptr;
    UILabelButton* rightToLeftBtn_ = nullptr;
    UILabelButton* uLeftTolRightBtn_ = nullptr;
    UILabelButton* lRightTouLeftBtn_ = nullptr;
    UILabelButton* increaseDragTimeBtn_ = nullptr;
    UILabelButton* decreaseDragTimeBtn_ = nullptr;
    UILabelButton* DragTimeDisplayBtn_ = nullptr;
    UITestEventInjectorView* clickTestView_ = nullptr;
    UITestEventInjectorView* dragTestView_ = nullptr;
    UITestEventInjectorView* longPressTestView_ = nullptr;
    UITestEventInjectorView* KeyEventTestView_ = nullptr;
    UIScrollView* scrollTestView_ = nullptr;

    int16_t lastX_ = 0;
    int16_t lastY_ = 0;
    uint16_t dragTime_ = 100; // 100: 100ms

    void InnerTest(std::string title, bool touchable, bool draggable, bool dragParent,
        std::string btnTitle, UILabelButton* btn, UITestEventInjectorView*& testView);
    void SetLastPos(UIView* view);
    void SetUpButton(UILabelButton* btn, std::string title);
    void SetUpScrollView();
    void IncreaseDragTime();
    void DecreaseDragTime();
    void DragTimeDisplay();
    void SetDragTimeDisplay(uint16_t dragTime);
};
} // namespace OHOS
#endif // UI_TEST_EVENT_INJECTOR_H
