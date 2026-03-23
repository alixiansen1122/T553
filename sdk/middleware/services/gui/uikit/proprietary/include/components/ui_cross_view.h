/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: UICrossView
 * Author:
 * Create: 2023-09
 */

/**
 * @addtogroup UI_Components
 * @{
 *
 * @brief Defines UI components such as buttons, texts, images, lists, and progress bars.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file ui_cross_view.h
 *
 * @brief Declares a UICrossView, witch provide horizontal vertical direction drag page.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef UI_CROSS_VIEW_H
#define UI_CROSS_VIEW_H

#include <memory>
#include "components/ui_swipe_view.h"
#include "components/card_swipe.h"
#include "components_ext/hor_swipe_view.h"
#include "components_ext/ver_swipe_view.h"

namespace OHOS {
/**
 * @brief It provide horizontal vertical direction drag page.
 *
 * @since 1.0
 * @version 1.0
 */
class UICrossView : public UIViewGroup, public UISwipeView::OnSwipeListener,
    public VerSwipeView::OnMoveChildListener {
public:
    UICrossView(const UICrossView &) = delete;
    UICrossView &operator=(const UICrossView &) = delete;

    /* *
     * @brief Scale align mode.
     */
    enum class VPage {
        UP_PAGE = 0,
        DOWN_PAGE
    };

    /* *
     * @brief A constructor used to create the <b>UICrossView</b> instance.
     * @since 1.0
     * @version 1.0
     */
    UICrossView();

    /* *
     * @brief A destructor used to delete the <b>UICrossView</b> instance.
     * @since 1.0
     * @version 1.0
     */
    ~UICrossView() override;

    /* *
     * @brief Get the static instance for <b>UICrossView</b>.
     * @since 1.0
     * @version 1.0
     */
    static UICrossView* GetInstance(void)
    {
        static UICrossView instance;
        return &instance;
    }

    void OnMoveChild(UISwipeView& view, int16_t xOffset, int16_t yOffset) override;
    void OnSwipe(UISwipeView& view) override;
    bool OnDragStartEvent(const DragEvent& event) override;
    bool OnDragEvent(const DragEvent& event) override;
    bool OnDragEndEvent(const DragEvent& event) override;
    void RemoveAll() override;
    void SetPosition(int16_t x, int16_t y, int16_t width, int16_t height) override;
    void Resize(int16_t width, int16_t height) override;

    /* *
     * @brief Add view by horizontal direction.
     * @since 1.0
     * @version 1.0
     */
    void HorAdd(UICardPage* view);

    /* *
     * @brief Insert view by horizontal direction. set as first page if preview is nullptr
     * @since 1.0
     * @version 1.0
     */
    void HorInsert(UICardPage* prevView, UICardPage* insertView);

    /* *
     * @brief Add view by vertical direction.
     * @since 1.0
     * @version 1.0
     */
    void VerAdd(UICardPage* view, VPage page);

    /* *
     * @brief Remove view of target direction.
     * @param direction The target direction: 0 means horizontal direction, 1 means vertical direction.
     * @since 1.0
     * @version 1.0
     */
    void Remove(UIView* view, uint8_t direction);

    /* *
     * @brief Set the current page index of horizontal direction.
     * @since 1.0
     * @version 1.0
     */
    void SetHorCurrentPage(uint16_t index, bool loadAdjacent = true);

    /* *
     * @brief Set the current page index of vertical direction.
     * @since 1.0
     * @version 1.0
     */
    void SetVerCurrentPage(uint16_t index);

    /* *
     * @brief Get the current page index of target direction.
     * @since 1.0
     * @version 1.0
     */
    uint16_t GetCurrentPageId(uint8_t direction = 0);

    /* *
     * @brief Get the current view of target direction.
     * @since 1.0
     * @version 1.0
     */
    UICardPage* GetCurrentPageView(uint8_t direction = 0);

    /* *
     * @brief Set animator time when you release finger from screen.
     * @since 1.0
     * @version 1.0
     */
    void SetAnimatorTime(uint16_t time);

    /* *
     * @brief Set loop state for horizontal direction.
     * @since 1.0
     * @version 1.0
     */
    void SetLoopState(bool loop)
    {
        loop_ = loop;
        horizontal_.SetLoopState(loop);
    }

    /* *
     * @brief Enable screen capture when swipe card(true) or nort(false).
     * @since 1.0
     * @version 1.0
     */
    void EnableScreenCap(bool enable)
    {
        enableScreenCap_ = enable;
        horizontal_.EnableScreenCap(enable);
    }

    /* *
     * @brief Is screen capture enabled or not.
     * @since 1.0
     * @version 1.0
     */
    bool IsEnableScreenCap(void)
    {
        return enableScreenCap_;
    }

    /* *
     * @brief Register swipe callback witch used when drag card.
     * @since 1.0
     * @version 1.0
     */
    void RegisterSwipeCallback(CardSwipe* callback)
    {
        horizontal_.RegisterSwipeCallback(callback);
        if (callback != nullptr) {
            isNeedClip_ = callback->isNeedClip_;
        }
    }

    /* *
     * @brief Register on swipe callback when swipe animator stops.
     * @since 1.0
     * @version 1.0
     */
    void SetOnSwipeListener(UISwipeView::OnSwipeListener* onSwipeListener)
    {
        swipeListener_ = onSwipeListener;
    }

    /**
     * @brief Obtains the view type.
     *
     * @return Returns <b>UI_CROSS_VIEW</b>, as defined in {@link UIViewType}.
     * @since 1.0
     * @version 1.0
     */
    UIViewType GetViewType() const override
    {
        return UI_CROSS_VIEW;
    }

    /**
     * @brief Set background blur radius. Default value is 10.0f, The effective range is [0.0f ~ 32.0f].
     * @since 1.0
     * @version 1.0
     */
    void SetBlurRadius(float radius)
    {
        blurRadius_ = radius;
    }

    /**
     * @brief Set the compensation distance after a finger lifts the screen.
     * @param value Indicates the compensation distance to set.
     * @param direction The target direction: 0 means horizontal direction, 1 means vertical direction.
     * @since 1.0
     * @version 1.0
     */
    void SetSwipeACCLevel(uint8_t value, uint8_t direction = 0);

    /**
    * @brief Set scale factor for screen capture.
    * @param scale The scale factor to set.
    * @since 1.0
    * @version 1.0
    */
    void SetScaleFactor(float scale);

    /**
    * @brief Retrieves the current scale factor.
    * @return The current scale factor.
    * @since 1.0
    * @version 1.0
    */
    float GetScaleFactor() const
    {
        return scale_;
    }

private:
    using UIViewGroup::Remove;
    float blurRadius_ = 10.0f;
    void UpdateCardPageIndex();
    static constexpr uint16_t INVALID_VIEW_INDEX = 0xff;
    bool loop_ = false;
    bool enableScreenCap_ = false;
    int swipeDirect_ = 0;
    int16_t hDragStartIndex_ = INVALID_VIEW_INDEX;
    int16_t vBlankIndex_ = INVALID_VIEW_INDEX;
    UIView* vBlankView_ = nullptr;
    VerSwipeView vertical_;
    HorSwipeView horizontal_;
    UIView* tailView_ = nullptr;
    UISwipeView::OnSwipeListener *swipeListener_ = nullptr;
    bool isNeedClip_ = false;
    std::shared_ptr<bool> exists_;
    bool waitProcessRes_ = false;
    float scale_ = 1.0f;

    void RemoveAllInner(UISwipeView& view);
    void ProcessRes(bool loadAdjacent = true);
    void HorHandleCardEvent(int16_t index, uint16_t scrollStage);
    void VertHandleCardEvent(uint16_t scrollStage);
};
}
#endif
/**
 * @}
 */
