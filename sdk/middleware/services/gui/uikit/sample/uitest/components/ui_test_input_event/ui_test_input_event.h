/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef UI_TEST_INPUT_EVENT_H
#define UI_TEST_INPUT_EVENT_H

#include "components/root_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "dock/input_device.h"
#include "ui_test.h"

namespace OHOS {
class TestKeyInputListener2 : public RootView::OnKeyActListener {
public:
    explicit TestKeyInputListener2(UILabel* label) : label_(label) {}
    virtual ~TestKeyInputListener2() {}
    bool OnKeyAct(UIView& view, const KeyEvent& event) override
    {
        if (label_ == nullptr) {
            return true;
        }
        switch (event.GetState()) {
            case InputDevice::STATE_PRESS:
                label_->SetText("key pressed!");
                break;
            case InputDevice::STATE_RELEASE:
                label_->SetText("key released!");
                break;
            default:
                label_->SetText("");
                break;
        }
        label_->Invalidate();
        return true;
    }

private:
    UILabel* label_;
};

class TestOnClickListener : public UIView::OnClickListener {
public:
    explicit TestOnClickListener(UILabel* label, std::string sentence, bool isConsume)
        : label_(label), sentence_(sentence), isConsume_(isConsume)
    {
    }
    virtual ~TestOnClickListener() {}
    virtual bool OnClick(UIView& view, const ClickEvent& event)
    {
        if (label_ != nullptr) {
            label_->SetText(sentence_.c_str());
            label_->Invalidate();
        }
        return isConsume_;
    }

private:
    UILabel* label_;
    std::string sentence_;
    bool isConsume_;
};

class TestOnLongPressListener : public UIView::OnLongPressListener {
public:
    explicit TestOnLongPressListener(UILabel* label, std::string sentence, bool isConsume)
        : label_(label), sentence_(sentence), isConsume_(isConsume)
    {
    }
    virtual ~TestOnLongPressListener() {}
    virtual bool OnLongPress(UIView& view, const LongPressEvent& event)
    {
        if (label_ != nullptr) {
            label_->SetText(sentence_.c_str());
            label_->Invalidate();
        }
        return isConsume_;
    }

private:
    UILabel* label_;
    std::string sentence_;
    bool isConsume_;
};

class TestOnTouchListener : public UIView::OnTouchListener {
public:
    explicit TestOnTouchListener(UILabel* label, std::string strPress,
        std::string strRelease, std::string strCancel, bool isConsume)
        : label_(label), strPress_(strPress), strRelease_(strRelease), strCancel_(strCancel), isConsume_(isConsume)
    {
    }
    virtual ~TestOnTouchListener() {}
    virtual bool OnPress(UIView& view, const PressEvent& event)
    {
        if (label_ != nullptr) {
            label_->SetText(strPress_.c_str());
            label_->Invalidate();
        }
        return isConsume_;
    }

    virtual bool OnRelease(UIView& view, const ReleaseEvent& event)
    {
        if (label_ != nullptr) {
            label_->SetText(strRelease_.c_str());
            label_->Invalidate();
        }
        return isConsume_;
    }

    virtual bool OnCancel(UIView& view, const CancelEvent& event)
    {
        if (label_ != nullptr) {
            label_->SetText(strCancel_.c_str());
            label_->Invalidate();
        }
        return isConsume_;
    }

private:
    UILabel* label_;
    std::string strPress_;
    std::string strRelease_;
    std::string strCancel_;
    bool isConsume_;
};

class TestOnDragListener : public UIView::OnDragListener {
public:
    explicit TestOnDragListener(UILabel* label, std::string strDragStart,
        std::string strDrag, std::string strDragEnd, bool isConsume)
        : label_(label), strDragStart_(strDragStart), strDrag_(strDrag), strDragEnd_(strDragEnd), isConsume_(isConsume)
    {
    }
    virtual ~TestOnDragListener() {}
    virtual bool OnDragStart(UIView& view, const DragEvent& event)
    {
        if (label_ != nullptr) {
            label_->SetText(strDragStart_.c_str());
            label_->Invalidate();
        }
        return isConsume_;
    }

    virtual bool OnDrag(UIView& view, const DragEvent& event)
    {
        if (label_ != nullptr) {
            label_->SetText(strDrag_.c_str());
            label_->Invalidate();
        }
        return isConsume_;
    }

    virtual bool OnDragEnd(UIView& view, const DragEvent& event)
    {
        if (label_ != nullptr) {
            label_->SetText(strDragEnd_.c_str());
            label_->Invalidate();
        }
        return isConsume_;
    }

private:
    UILabel* label_;
    std::string strDragStart_;
    std::string strDrag_;
    std::string strDragEnd_;
    bool isConsume_;
};

class UITestInputEvent : public UITest {
public:
    UITestInputEvent() {}
    ~UITestInputEvent() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    /**
     * @brief Test if dispatch press\release\longpress\cancel event act normal when target is touchable
     */
    void UIKitPointerInputTestDispatchSimpleEvent001();
    /**
     * @brief Test if dispatch press\release\longpress\cancel\drag event act normal when target is untouchable
     */
    void UIKitPointerInputTestDispatchSimpleEvent002();
    /**
     * @brief Test if dispatch drag event act normal when target is touchable and draggable and drag parent instead.
     */
    void UIKitPointerInputTestDispatchDragEven001();
    /**
     * @brief Test if dispatch drag event act normal when target is touchable and draggable and not drag parent instead.
     */
    void UIKitPointerInputTestDispatchDragEven002();
    /**
     * @brief Test if dispatch drag event act normal when target is untouchable but draggable.
     */
    void UIKitPointerInputTestDispatchDragEven003();
    /**
     * @brief Test if dispatch drag event act normal when target is untouchable but draggable.
     */
    void UIKitPointerInputTestDispatchKeyEvent001();
    /**
     * @brief Test if dispatch drag event act normal when target is untouchable but draggable.
     */
    void UIKitPointerInputTestDispatchInVisibleEvent001();
    /**
     * @brief Test click, release or longClick event bubble act normal when both of parent and child is triggered.
     */
    void UIKitPointerInputTestDispatchBubble001();
    /**
     * @brief Test click, release or longClick event bubble act normal when child is triggered but and parent not.
     */
    void UIKitPointerInputTestDispatchBubble002();
    /**
     * @brief Test click, release or longClick event bubble act normal when child is triggered but and parent not.
     */
    void UIKitPointerInputTestDispatchBubble003();
    /**
     * @brief Test click, release or longClick event bubble act normal when parent is triggered but and child not.
     */
    void UIKitPointerInputTestDispatchBubble004();
    /**
     * @brief Test drag event bubble act normal when both of parent and child is triggered.
     */
    void UIKitPointerInputTestDispatchBubble005();
    /**
     * @brief Test drag event bubble act normal when child is triggered but and parent not.
     */
    void UIKitPointerInputTestDispatchBubble006();
    /**
     * @brief Test drag event bubble act normal when child is triggered but and parent not.
     */
    void UIKitPointerInputTestDispatchBubble007();
    /**
     * @brief Test drag event bubble act normal when both of parent and child is not triggered.
     */
    void UIKitPointerInputTestDispatchBubble008();
    /**
     * @brief Test drag event bubble act normal when parent is triggered but and child not.
     */
    void UIKitPointerInputTestDispatchBubble009();
    /**
     * @brief Test drag event bubble act normal when parent is triggered but and child not.
     */
    void UIKitPointerInputTestDispatchBubble010();

private:
    UIScrollView* container_ = nullptr;
    TestKeyInputListener2* keyListener_ = nullptr;
    void InnerTest(const char* title, bool touchable, bool draggable, bool dragParent);
    void InnerBubbleTest(const char* title, bool touchable, bool draggable, bool hasListener, bool isBubble);
    void InnerBubbleDragTest(const char* title,
                             bool childDraggable,
                             bool parentDraggable,
                             bool hasListener,
                             bool isBubble);
    void InitScrollView(UIScrollView* parentScroll, const char* title, bool parentDraggable);
};
} // namespace OHOS
#endif // UI_TEST_INPUT_EVENT_H
