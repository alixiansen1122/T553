/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: TransitionCallback
 * Author:
 * Create: 2023-11
 */

#ifndef SLICE_SWITCH_CALLBACK_H
#define SLICE_SWITCH_CALLBACK_H
#include "components/ui_image_view.h"
#include "components/ui_view_group.h"
namespace OHOS {
class TransitionCallback : public HeapBase {
public:
    /**
     * @brief constructor of base class for Transition algorithm
     *   Note that:
     *   1. The transition parameters will impact process flow:
     *      if enableCurrentSnapshot is true and enableTargetSnapshot is true
     *        OnPause(current slice) -> Snap (current slice) -> OnStop(current slice) -> OnStart(target slice)
     *          -> Snap (target slice) -> Add snapshot views -> Transition animator -> Destroy Snapshots
     *          -> OnResume(target slice)
     *      if enableCurrentSnapshot is true and enableTargetSnapshot is false
     *        OnPause(current slice) -> Snap (current slice) -> OnStop(current slice) -> OnStart(target slice)
     *          -> Add snapshot view -> Transition animator -> Destroy Snapshots
     *          -> OnResume(target slice)
     *      if enableCurrentSnapshot is false and enableTargetSnapshot is true
     *        OnPause(current slice) -> OnStart(target slice) -> Snap (target slice)
     *          -> Add snapshot view -> Transition animator -> Destroy Snapshots
     *          -> OnStop(current slice) -> OnResume(target slice)
     *      if enableCurrentSnapshot is false and enableTargetSnapshot is false
     *        OnPause(current slice) -> OnStart(target slice)
     *           -> Transition animator
     *           -> OnStop(current slice) -> OnResume(target slice)
     *   2. Memory usage is diffrent when using different transition parameters refer to above process flow
     *      - Snapshot will cost memory: stride (of screen) * height (of screen) * 3 (RGB888)
     *      - Start slice will cost new memory usage
     *      - Stop slice will release its memory usage
     *   3. Diffrent methods are invoked when using different parameters, refer to comments of below methods
     * @param time Snapshot of current slice
     * @param enableCurrentSnapshot Whether use snapshot for current slice in the transition
     * @param enableTargetSnapshot Whether use snapshot for target slice in the transition
     */
    TransitionCallback(uint32_t time, bool enableCurrentSnapshot, bool enableTargetSnapshot)
    {
        duration_ = time;
        enableCurrentSnapshot_ = enableCurrentSnapshot;
        enableTargetSnapshot_ = enableTargetSnapshot;
    }

    virtual ~TransitionCallback() = default;

    /* *
     * @brief Invoked when transition start if enableCurrentSnapshot is true and enableTargetSnapshot is true
     * @param current Snapshot of current slice
     * @param target Snapshot of target slice
     * @param time Elapse milliseconds since start time of the transition
     *   Note that:Both current and target UIImageViews are full screen and have aleady been added into Root View,
     *   and target view is on top of current UIImageView. you can modify it in this function if necessary
     */
    virtual void OnTransitionStart(UIImageView* current, UIImageView* target) {};

    /* *
     * @brief Invoked when transition start if enableCurrentSnapshot is true and enableTargetSnapshot is false
     * @param current Snapshot of current slice
     * @param target Container view of target slice
     * @param time Elapse milliseconds since start time of the transition
    *   Note that: Both current and target views have aleady been added into Root View, current UIImageView is full
    *   screen, and current view is on top of target view. you can modify it in this function if necessary
     */
    virtual void OnTransitionStart(UIImageView* current, UIViewGroup* target) {};

    /* *
     * @brief Invoked when transition start if enableCurrentSnapshot is false and enableTargetSnapshot is true
     * @param current Container view  of current slice
     * @param target Snapshot of target slice
     * @param time Elapse milliseconds since start time of the transition
     *  Note that: Both current and target views have aleady been added into Root View, target UIImageView is full
     *  screen, and target view is on top of current view. you can modify it in this function if necessary
     */
    virtual void OnTransitionStart(UIViewGroup* current, UIImageView* target) {};

    /* *
     * @brief Invoked when transition start if enableCurrentSnapshot is false and enableTargetSnapshot is false
     * @param current Container view  of current slice
     * @param target Container view  of target slice
     *  Note that: Both current and target views have aleady been added into Root View,
     *  and target view is on top of current view. you can modify it in this function if necessary
     */
    virtual void OnTransitionStart(UIViewGroup* current, UIViewGroup* target) {};

    /* *
     * @brief Invoked in the transition animator
     * @param time Elapse milliseconds since start time of the transition
     */
    virtual void TransitionAlg(uint32_t time) {};
    /* *
     * @brief Invoked when the transition ends
     */
    virtual void OnTransitionEnd() {};

    uint32_t duration_ = 0;
    bool enableCurrentSnapshot_ = true;
    bool enableTargetSnapshot_ = true;
};
} // namespace OHOS
#endif // SLICE_SWITCH_CALLBACK_H
