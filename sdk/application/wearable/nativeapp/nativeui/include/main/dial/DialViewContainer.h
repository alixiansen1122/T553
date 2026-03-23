/*
 * Copyright (c) CompanyNameMagicTag 2025. All rights reserved.
 * Description: DialViewContainer
 * Author:
 * Create: 2025-02-06
 */

#ifndef DIAL_VIEW_CONTAINER_H
#define DIAL_VIEW_CONTAINER_H
#include "components/ui_view_group.h"
#include "gfx_utils/list.h"
#include "main/dial/DialView.h"
#include "main/dial/DialDataBase.h"

namespace OHOS {
class ContainerInterface {
public:
    virtual void AddView(UIView* view) = 0;

    virtual void AddView(DialDataType type, DialView* view, bool isPeriodic) = 0;

    virtual ~ContainerInterface() {}
};

class DialViewContainer : public UIViewGroup, public ContainerInterface {
public:
    struct SubView {
        UIView *view;
        DialView *dialView;
        DialDataType type;
        bool isPeriodic;
    };
    DialViewContainer(uint8_t dialviewNum, uint16_t dataSize) :
        dialViewNum_(dialviewNum), dataSize_(dataSize)
    {
        SetTouchable(true);
    }
    DialViewContainer() {}
    ~DialViewContainer() override;
    DialViewContainer(const DialViewContainer&) = delete;
    DialViewContainer& operator=(const DialViewContainer&) = delete;

    uint8_t GetDialviewNum() const
    {
        return dialViewNum_;
    }

    uint16_t GetDataSize() const
    {
        return dataSize_;
    }

    void AddView(UIView* view) override;

    void AddView(DialDataType type, DialView* view, bool isPeriodic) override;

    ListNode<SubView>* Begin()
    {
        return dialViews_.Begin();
    }

    const ListNode<SubView>* End()
    {
        return dialViews_.End();
    }

    bool HasInvalidBindData() const
    {
        return hasInvalidBindData_;
    }

    bool RemoveSubView(uint8_t index, SubView &subView);

    bool IsInvalidateBindData(uint8_t index);

private:
    uint8_t dialViewNum_;
    uint16_t dataSize_;
    List<SubView> dialViews_;
    bool hasInvalidBindData_ = false;
}; // class DialViewContainer
} // namesapce OHOS
#endif // DIAL_VIEW_CONTAINER_H