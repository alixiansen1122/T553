 /*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: FloatAccessView
 * Author: Hisi Graphic Team
 * Created: 2025-09
 */

#ifndef FLOAT_ACCESS_VIEW_H
#define FLOAT_ACCESS_VIEW_H

#include "components/ui_view_group.h"
#include "components/ui_float_box_view.h"
#include "applist/AppItemView.h"

namespace OHOS {
namespace {
    constexpr uint16_t MAX_ITEM_SIZE = 3;
}
class FloatAnimatorCallback;
class FloatAccessView : public UIViewGroup,
            public UIFloatBoxView::OnItemClickListener,
            public UIView::OnClickListener {
public:
    static FloatAccessView *GetInstance(void);
    FloatAccessView(const FloatAccessView &) = delete;
    FloatAccessView &operator=(const FloatAccessView &) = delete;
    void AddItem(AppItem item, uint8_t index);
    void DeleteItem(const char* label);
    AppItem GetItem(uint8_t index);

    void OnItemClick(const std::string& name) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
    
    void ShowView();
    void ExitView();
    void CleanView();
    bool IsShowView();
    void LoadAppItemsData();
private:
    FloatAccessView() {}
    ~FloatAccessView() {}
    uint8_t GetItemSize();
    void SaveAppItemsData();
    void AddAppItemsData();
    void InitView();
    void EnterApp();
    bool isInitView_{false};
    bool isShowView_{false};
    bool isLoadData_{false};
    bool isClickItem_{false}; // 标记点击了快捷应用
    UIFloatBoxView* floatBoxView_ = nullptr;
    AppItem items_[MAX_ITEM_SIZE] = {};
    FloatAnimatorCallback* enterCallback_ = nullptr;
};

class FloatAnimatorCallback : public AnimatorCallback {
public:
    explicit FloatAnimatorCallback(UIView* view)
        : startPos_(0), endPos_(0), startTime_(0), passTime_(0), isExit_(false),
          animator_(new Animator(this, view, 300, false)) {} // 300 ms: 动画持续时间

    ~FloatAnimatorCallback() override
    {
        if (animator_ != nullptr) {
            delete animator_;
            animator_ = nullptr;
        }
    }
    void Callback(UIView* view) override;
    void OnStop(UIView& view) override;
    void DoEnter();
    void DoExit();
    uint8_t GetState()
    {
        return animator_->GetState();
    }
protected:
    int16_t startPos_;
    int16_t endPos_;
    uint32_t startTime_;
    uint32_t passTime_;
    bool isExit_;
    Animator* animator_;
};
} // namespace OHOS
#endif // LOADING_FRAGMENT_H