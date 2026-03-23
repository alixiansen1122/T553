/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 */

#ifndef DIAL_VIEW_GROUP_H
#define DIAL_VIEW_GROUP_H

#include <unordered_map>
#include <memory>
#include "main/dial/DialDataBase.h"
#include "components/ui_view_group.h"
#include "components/ui_image_animator.h"
#include "components/ui_label.h"
#include "main/dial/OnDialDataUpdateListener.h"
#include "components/ui_card_page.h"
#include "components/ui_kaleidoscope_view.h"
#include "gfx_utils/list.h"
#include "gfx_utils/vector.h"
#include "main/dial/DialView.h"
#include "main/dial/DialVideoView.h"
#include "main/DialBinTypesV2.h"
#include "main/dial/DialViewContainer.h"

namespace OHOS {
class DialViewGroup : public UICardPage, public OnDialDataUpdateListener, public ContainerInterface {
public:
    DialViewGroup();
    ~DialViewGroup() override;

    /**
    * @brief  Set dial resource path.
    */
    void SetDial(std::string filePath, DisplayState state = DisplayState::NORMAL);

    /**
    * @brief  Add a view which binds with data of a specific type and could be updated periodically.
    */
    void AddView(DialDataType type, DialView* view, bool isPeriodic) override;

    /**
    * @brief  Add a view.
    */
    void AddView(UIView* view) override;

    /**
    * @brief  Add a click rect. When the rect is clicked, it will start a js app with the given bundleName.
    */
    void AddDialClick(Rect& rect, const char* bundleName);

    /**
    * @brief  Add a click rect. When the rect is clicked, it will change to a slice with the given sliceId.
    */
    void AddDialClick(Rect& rect, uint32_t sliceId);

    /**
    * @brief  Set update period.
    */
    void SetPeriod(uint32_t period);

    /**
    * @brief  Get update period.
    */
    uint32_t GetPeriod(void);

    /**
    * @brief  Update views periodically.
    */
    void UpdateViewsByPeriodicUpdateData();

    /**
    * @brief  Dial float data update callback.
    */
    void OnDialFloatDataUpdate(DialDataType type, float data) override;

    /**
    * @brief  Dial multi-float data update callback.
    */
    void OnDialFloatDataUpdate(DialDataType type, const float* data, uint16_t num) override;

    /**
    * @brief  Dial text data update callback.
    */
    void OnDialTextDataUpdate(DialDataType type, const std::string* data, uint16_t num) override;

    /**
    * @brief  Click event callback.
    */
    bool OnClickEvent(const ClickEvent& event) override;

    /**
    * @brief  Preload resource callback.
    */
    void PreLoad(void) override;

    /**
    * @brief  Scroll begin callback. If a video is played, it will be paused.
    */
    void ScrollBegin(bool isActive) override;

    /**
    * @brief  Scroll end callback. If a video is paused or stopped, it will be resumed or started.
    */
    void ScrollEnd(bool isActive) override;

    /**
    * @brief  Cover begin callback. If a video is played, it will be paused.
    */
    void CoverBegin(bool isCovered) override;

    /**
    * @brief  Cover end callback. If a video is paused or stopped, it will be resumed or started.
    */
    void CoverEnd(bool isCovered) override;

    /**
    * @brief  This function is invoked when the function SetHorCurrentPage() is called.
    */
    void OnActive(void) override;

    /**
    * @brief  This function is invoked when the function SetHorCurrentPage() is called.
    */
    void OnInactive(void) override;

    /**
    * @brief  This function is invoked when SetVerCurrentPage is called and horizontal page is covered.
    */
    void OnCovered(void) override;

    /**
    * @brief  This function is invoked when SetVerCurrentPage is called and horizontal page is not covered.
    */
    void OnUncovered(void) override;

    void OnPause() override;

    MediaVideoPlay* GetVideoPlay();

private:
    void ProcessData(DialDataType type, DialView* view);
    void DoStop(void);
    void DoStart(void);
    void DoPause(void);
    void RecordSpecial(UIView *view);
    void RecordBindData(DialDataType type, DialView* view, bool isPeriodic);
    void ClearViews();
    using Views = Graphic::Vector<DialView*>;
    std::unordered_map<int16_t, Views*> dataTypeMap_;
    Graphic::Vector<Views*> viewsMemPool_;
    List<DialDataType> periodicUpdateTypes_;
    List<DialDataType> passiveUpdateTypes_;
    List<UIView*> dialViews_;
    uint32_t period_ = DEFAULT_TASK_PERIOD;
    bool resPreloaded_ = false;
    std::string filePath_;
    FILE* fp_ = nullptr;
    DialVideoView* video_ = nullptr;
    UIKaleidoscopeView* kaleidoscope_ = nullptr;
    DisplayState state_ = DisplayState::NORMAL;

    typedef struct {
        Rect area;
        bool isSliceType;
        union {
            uint32_t sliceId;
            char* bundleName;
        };
    } DialClick;
    List<DialClick> dialClicks_;

    void ClearDial();

    void SetLableAnimatorState(uint8_t state);
    void SetImgAnimatorState(uint8_t state);
    List<UILabel*> rollingLabelList_;
    List<UIImageAnimatorView*> imgAnimList_;
    bool isSeqImgLoaded_ = false;
    std::shared_ptr<bool> isExists_;
};
}
#endif
