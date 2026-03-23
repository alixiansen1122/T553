/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */
#ifndef LV_SWIPE_VIEW_H
#define LV_SWIPE_VIEW_H

#include "lvgl.h"
#include "misc/lv_area.h"
#include "lv_obj_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief preloading resources callback function.
 *  1. Loaded for the first time or unloaded.
 *  2. Horizontal page call this callback when is switched, and the page is active page or adjacent active page.
 *  3. Vertical page call this callback when add to crossview.
 * @param obj   pointer to a page object
 */
typedef void (*PreloadCallback)(lv_obj_t* obj);

/**
 * @brief Uninstalling Resources callback function.
 *  1. The resource has been loaded.
 *  2. Horizontal page call this callback when is switched, and the page is non-adjacent active page.
 *  3. Vertical page call this callback when remove to crossview.
 * @param obj   pointer to a page object
 */
typedef void (*UnloadCallback)(lv_obj_t* obj);

/**
 * @brief Listening to the scroll start event callback function.
 *  1. Call the function at the start of the drag.
 *  2. If the scrolling direction is horizontal, the active page and horizontally adjacent pages call this callback.
 *  3. If drag down, the top page call this callback.
 *  4. If drag up, the bottom page call this callback.
 * @param obj      pointer to a page object
 * @param isActive current active page value is <b>true<b>, other pages value is <b>false<b>.
 */
typedef void (*ScrollBeginCallback)(lv_obj_t* obj, bool isActive);

/**
 * @brief Listening to the scroll end event callback function.
 *  1. scroll animation ended.
 *  2. The page called <ScrollBeginCallback> function when drag begins phase.
 * @param obj      pointer to a page object
 * @param isActive current active page value is <b>true<b>, other pages value is <b>false<b>.
 */
typedef void (*ScrollEndCallback)(lv_obj_t* obj, bool isActive);

/**
 * @brief Listening to the cover start event callback function,
 *  1. Current horizontal active page call the function at the start of the drag.
 * @param obj       pointer to a page object
 * @param isCovered The current horizontal page is overwritten by a vertical page value is <b>true<b>.
 */
typedef void (*CoverBeginCallback)(lv_obj_t* obj, bool isCovered);

/**
 * @brief Listening to the cover end event callback function.
 *  1. Current horizontal active page call the function at the end of scroll animation.
 * @param obj       pointer to a page object
 * @param isCovered The current horizontal page is overwritten by a vertical page value is <b>true<b>.
 */
typedef void (*CoverEndCallback)(lv_obj_t* obj, bool isCovered);

/**
 * @brief Obtaining the blur radius by global blur in the vertical direction callback function.
 * The more frequently the blur radius changes according to the rolling distance, the worse the performance.
 * @param distance   current scrolled distance.
 * @return blur radius of the current scroll distance.
 */
typedef uint16_t (*GetBlurRadius)(lv_coord_t distance);
typedef void (*ScrollEffect)(lv_obj_t* imgL, lv_obj_t* imgR, lv_coord_t xOffset);
typedef void (*ScrollEndEffect)(void);
typedef struct {
    ScrollEffect scrollEffect;
    ScrollEndEffect scrollEffectEnd;
} ScrollEffectFunc;

typedef struct {
    PreloadCallback preload;
    UnloadCallback unload;
    ScrollBeginCallback scrollBegin;
    ScrollEndCallback scrollEnd;
    CoverBeginCallback coverBegin;
    CoverEndCallback coverEnd;
} LvPageInfo;

extern const lv_obj_class_t LvCrossViewClass;
extern const lv_obj_class_t LvCrossViewPageClass;
/**********************
 * GLOBAL PROTOTYPES
 **********************/
/**
 * Create a crossview object.
 * @param parent pointer to an object, it will be the parent of the new crossview.
 * @return pointer to the created crossview.
 */
lv_obj_t* LvCrossViewCreate(lv_obj_t* parent);

/**
 * Create a horizontal page object, add to cross view tail.
 * @param cv pointer to a cross view object, it will be the parent of the new page.
 * @param info Information required by the page.
 * @param isSupportVerScroll Indicates whether to support sliding up and down on this page.
 * @return pointer to the created page.
 */
lv_obj_t* LvCrossViewAddHorPage(lv_obj_t* cv, LvPageInfo* info, bool isSupportVerScroll);

/**
 * Create a horizontal page object, Insert a new page in front of the node node
 * @param cv pointer to a cross view object, it will be the parent of the new page.
 * @param page pointer to a page object, insert before the page. if value is <b>NULL<b>,
 * the function is similar LvCrossViewAddHorPage.
 * @param info Information required by the page.
 * @param isSupportVerScroll Indicates whether to support sliding up and down on this page.
 * @return pointer to the created page.
 */
lv_obj_t* LvCrossViewInsertHorPage(lv_obj_t* cv, const lv_obj_t* page, LvPageInfo* info, bool isSupportVerScroll);

/**
 * Remove a horizontal page.
 * @param parent pointer to a cross view object.
 * @param page pointer to the remove page
 */
void LvCrossViewRemoveHorPage(lv_obj_t* cv, lv_obj_t* page);

/**
 * Create a vertical page object, add to cross view.
 * @param cv pointer to a cross view object, it will be the parent of the new page.
 * @param info Information required by the page.
 * @param isTopPage add page Location, <b>true<b> add a top page, <b>false<b> add a bottom page
 * @return pointer to the created page.
 */
lv_obj_t* LvCrossViewAddVerPage(lv_obj_t* cv, LvPageInfo* info, bool isTopPage);

/**
 * Remove a vertical page.
 * @param parent pointer to a cross view object.
 * @return page pointer to the created vertical page
 */
void LvCrossViewRemoveVerPage(lv_obj_t* cv, lv_obj_t* page);

/**
 * Set the current active page of horizontal pages,
 * @note must invoke this method after all pages are added or some pages are
 * modified(deleted or inserted) later, otherwise, the resource of left and right pages
 * will not be preloaded in advance.
 * @param cv pointer to a cross view object.
 * @param page pointer to a horizontal page.
 */
void LvCrossViewSetHorActPage(lv_obj_t* cv, lv_obj_t* page);

/**
 * Get the current horizontal active page.
 * @param cv pointer to a cross view object.
 * @return pointer to a horizontal active page.
 */
lv_obj_t* LvCrossViewGetHorActPage(lv_obj_t* cv);

/**
 * Get the current vertical active page.
 * @param cv pointer to a cross view object.
 * @return pointer to a vertival active page .
 */
lv_obj_t* LvCrossViewGetVerActPage(lv_obj_t* cv);

/**
 * Sets whether the upper and lower pages are blurred.
 * @param cv pointer to a cross view object.
 * @param isBlur<b>true<b> blur enabled, <b>false<b> blur disabled.
 */
void LvCrossViewSetVerBlur(lv_obj_t* cv, bool isBlur);

/**
 * Set the crossview vertical interface to global blur.
 * @param cv pointer to a cross view object.
 * @param callback function for obtaining the global fuzzy radius
 */
void LvCrossViewSetVerGlobalBlur(lv_obj_t* cv, GetBlurRadius callback);

/**
 * Is screen capture enabled or not.
 * @param cv pointer to a cross view object.
 * @param isEnable is screen capture enabled or not.
 */
bool LvCrossViewSetScreenCap(lv_obj_t* cv, bool isEnable);

/**
 * Set scroll effect.
 * @param cv pointer to a cross view object.
 * @param effect scroll effect callback func.
 */
void LvCrossViewSetScrollEffect(lv_obj_t* cv, ScrollEffectFunc effect);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // LV_SWIPE_VIEW_H
