/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: UICardPage
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
 * @file ui_card_page.h
 *
 * @brief View cooperated with UICrossView.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef UI_CARD_PAGE_H
#define UI_CARD_PAGE_H

#include "components/ui_view_group.h"
namespace OHOS {
/**
 * @brief View cooperated with UICrossView.
 *
 * @since 1.0
 * @version 1.0
 */
class UICardPage : public UIViewGroup {
public:
    /* *
     * @brief A constructor used to create the <b>UICardPage</b> instance.
     * @since 1.0
     * @version 1.0
     */
    UICardPage() {}

    /* *
     * @brief A destructor used to delete the <b>UICardPage</b> instance.
     * @since 1.0
     * @version 1.0
     */
    ~UICardPage() override {}

    /* *
     * @brief This function is invoked when the function SetHorCurrentPage() or Presenter:OnResume() is called.
     * @since 1.0
     * @version 1.0
     */
    virtual void OnActive(void) {}

    /* *
     * @brief This function is invoked when the function SetHorCurrentPage() is called.
     * @since 1.0
     * @version 1.0
     */
    virtual void OnInactive(void) {}

    /* *
     * @brief Preload resource callback function, it will be called at the end of swipe animator or the function
     *        SetHorCurrentPage() is called.
     * @since 1.0
     * @version 1.0
     */
    virtual void PreLoad(void) {}

    /* *
     * @brief Unload resource callback function, it will be called at the end of swipe animator or the function
     *        SetHorCurrentPage is called.
     * @since 1.0
     * @version 1.0
     */
    virtual void UnLoad(void) {}

    /* *
     * @brief Scrolling start notification func, witch is called at the start of scrolling.
     *        If it's scrolling horizontal, the horizontal current/left/right page's ScrollBegin() func will be called.
     *        If it's scrolling vertical, the vertical up/down page's ScrollBegin() func will be called.
     * @since 1.0
     * @version 1.0
     */
    virtual void ScrollBegin(bool isActive) {}

    /* *
     * @brief Scrolling end notification func, witch is called at the end of scrolling.
     *        If it's scrolling horizontal, the horizontal current/left/right page's ScrollEnd() func will be called.
     *        If it's scrolling vertical, the vertical up/down page's ScrollEnd() func will be called.
     * @since 1.0
     * @version 1.0
     */
    virtual void ScrollEnd(bool isActive) {}

    /* *
     * @brief Cover begin notification func, witch is called at the start of vertical scrolling.
     *        At the start of scrolling, the horizontal current page's CoverBegin() func will be called.
     * @since 1.0
     * @version 1.0
     */
    virtual void CoverBegin(bool isCovered) {}

    /* *
     * @brief Cover end notification func, witch is called at the end of vertical scrolling.
     *        At the end of scrolling, the horizontal current page's CoverEnd() func will be called.
     * @since 1.0
     * @version 1.0
     */
    virtual void CoverEnd(bool isCovered) {}

    /* *
     * @brief This function is invoked when SetVerCurrentPage is called and horizontal page is covered.
     * @since 1.0
     * @version 1.0
     */
    virtual void OnCovered(void) {}

    /* *
     * @brief This function is invoked when SetVerCurrentPage is called and horizontal page is not covered.
     * @since 1.0
     * @version 1.0
     */
    virtual void OnUncovered(void) {}

    /* *
     * @brief This function is invoked when Presenter::OnPause is called.
     * @since 1.0
     * @version 1.0
     */
    virtual void OnPause(void) {}

    /* *
     * @brief Set coverable state.
     * @since 1.0
     * @version 1.0
     */
    void SetCoverable(bool coverable)
    {
        isCoverable_ = coverable;
    }

    /* *
     * @brief Get coverable state.
     * @since 1.0
     * @version 1.0
     */
    bool IsCoverable(void)
    {
        return isCoverable_;
    }

    /**
     * @brief Obtains the view type.
     *
     * @return Returns <b>UI_CARD_PAGE</b>, as defined in {@link UIViewType}.
     * @since 1.0
     * @version 1.0
     */
    UIViewType GetViewType() const override
    {
        return UI_CARD_PAGE;
    }

    /* *
     * @brief Clear the image cache of this page.
     * @since 1.0
     * @version 1.0
     */
    void ClearCache(void);

private:
    bool isCoverable_ = false;
};
}
#endif
/**
 * @}
 */
