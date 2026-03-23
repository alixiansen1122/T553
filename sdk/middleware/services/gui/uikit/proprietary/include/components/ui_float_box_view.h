/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: UIFloatBoxView
 * Author: Hisi Graphic Team
 * Created: 2025-8
 */
#ifndef UI_FLOAT_BOX_VIEW_H
#define UI_FLOAT_BOX_VIEW_H

#include <cstring>
#include "components/ui_view.h"
#include "components/ui_image_view.h"
#include "common/graphic_hardware_types.h"
#include "gfx_utils/image_info.h"
#include "gfx_utils/list.h"
#include "gfx_utils/vector.h"
#include "animator/animator.h"

namespace OHOS {
struct FloatItem {
    std::string name;
    ImageInfo* imgInfo;
};

struct FloatItemInfo {
    std::string name;
    ImageInfo* imgInfo;
    float angle;
};

class UIFloatBoxView : public UIView {
public:
    class OnItemClickListener : public HeapBase {
    public:
        /**
         * @brief Called when an item is clicked.
         * @param name Indicates the name of clicked item
         */
        virtual void OnItemClick(const std::string& name) = 0;

        virtual ~OnItemClickListener() {}
    };

    UIFloatBoxView();

    virtual ~UIFloatBoxView();

    /**
     * @brief  Set OnItemClickListener.
     */
    void SetOnItemClickListener(UIFloatBoxView::OnItemClickListener* listener);

    /**
     * @brief  Get OnItemClickListener.
     */
    UIFloatBoxView::OnItemClickListener*& GetOnItemClickListener();

    /**
     * @brief  Add items, each with a unique name. Count of floatItems must be in range [2, 3].
     *         The image content should be in circle shape. ImageInfo will be deep copied.
     */
    bool AddItems(FloatItem* floatItems, uint8_t floatItemsCnt);

    /**
     * @brief  Clear all items.
     */
    void ClearItems();

    /**
     * @brief  Get current cnt of items.
     */
    uint16_t GetItemCnt() const;

    /**
     * @brief  Set an float item's size. Size is an item's width and height.
     *         Item size must be set and should be greater than 0.
     */
    void SetItemSize(uint16_t size);

    /**
     * @brief  Set the spacing distance between float items. Item spacing must be set and should be greater than 0.
     */
    void SetItemSpacing(uint16_t spacing);

    /**
     * @brief  Set drag factor.
     */
    void SetDragFactor(float value);

    /**
     * @brief  Get drag factor.
     */
    float GetDragFactor() const;

    /**
     * @brief  Set swipe acceleration factor.
     */
    void SetAccFactor(float factor);

    /**
     * @brief  Get swipe acceleration factor.
     */
    float GetAccFactor() const;

    /**
     * @brief  Set rotate factor.
     */
    void SetRotateFactor(float value);

    /**
     * @brief  Get rotate factor.
     */
    float GetRotateFactor() const;

    /**
     * @brief  Set align anim's avg speed. Align offset / speed = Anim times.
     */
    void SetAlignAvgSpeed(float speed);

    /**
     * @brief  Get align anim's avg speed.
     */
    float GetAlignAvgSpeed() const;

    /**
     * @brief Set the color for background arc.
     */
    void SetBgArcColor(uint32_t color);

    bool OnDragEvent(const DragEvent& event) override;
    bool OnDragEndEvent(const DragEvent& event) override;
#if ENABLE_ROTATE_INPUT
    bool OnRotateEvent(const RotateEvent& event) override;
    bool OnRotateEndEvent(const RotateEvent& event) override;
#endif
    bool OnClickEvent(const ClickEvent& event) override;
    void ReMeasure() override;
    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

    UIViewType GetViewType() const override
    {
        return UI_FLOAT_BOX_VIEW;
    }

private:
    class AlignAnimatorCallback : public AnimatorCallback {
    public:
        AlignAnimatorCallback()
            : curTime_(0),
              animTime_(0),
              previousAngle_(0),
              angle_(0)
        {
        }

        virtual ~AlignAnimatorCallback() {}

        void SetAnimTimes(uint16_t times)
        {
            animTime_ = times;
        }

        void SetAngle(float angle)
        {
            angle_ = angle;
        }

        void ResetCallback()
        {
            angle_ = 0;
            previousAngle_ = 0;
            animTime_ = 0;
            curTime_ = 0;
        }

        virtual void Callback(UIView* view) override;

        uint16_t curTime_;
        uint16_t animTime_;
        float previousAngle_;
        float angle_;
    };

    bool ParamCheck();
    void PrepAlign(float angle = 0.0f);
    bool RenderBgArc();
    void BlitItem(ImageInfo* info, float x, float y);
    void UpdateItem(float angle);
    Vector2<float> GetItemPosFromAngle(float angle);
    FloatItemInfo* GetClickedItem(Point point);
    void StopAlignAnim();
    float GetAdjustedAngle(float angle);
    void PrepContentInfo();

    Graphic::Vector<FloatItemInfo> items_;
    ImageInfo contentImg_ = {0};
    BufferInfo contentBuf_ = {{0, 0, 0, 0}};

    float dragFactor_ = 1.0f;
    float accFactor_ = 3.0f;
    float rotateFactor_ = 3.0f;
    float alignAvgSpeed_ = 5.0f;
    uint32_t bgArcColor_ = Color::Gray().full;
    Path bgPath_ = {0};

    const float START_ANGLE = 115; // start angle without round cap
    const uint16_t MIN_ALIGN_ANIM_TIMES = 3;
    const uint8_t MIN_ITEM_CNT = 2;
    const uint8_t MAX_ITEM_CNT = 3;
    uint16_t itemSize_ = 0;
    uint16_t itemSpacing_ = 0;
    float radius_ = 0.0f;
    float arcWidth_ = 0.0f;
    float itemAngle_ = 0.0f;
    float spacingAngle_ = 0.0f;
    float roundCapAngle_ = 0.0f;
    float startAngle_ = 0.0f;
    float endAngle_ = 0.0f;
    float totalAngle_ = 0.0f;
    bool needRemeasure_ = true;
    OnItemClickListener* onItemClickListener_ = nullptr;

    Animator alignAnim_;
    AlignAnimatorCallback alignCallback_;
    Rect contentArea_;
};
}
#endif