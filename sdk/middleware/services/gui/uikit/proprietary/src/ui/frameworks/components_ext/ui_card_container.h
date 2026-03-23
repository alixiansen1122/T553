/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: UICardContainer
 * Author:
 * Create: 2023-09
 */

#ifndef UI_CARD_CONTAINER_H
#define UI_CARD_CONTAINER_H

#include "components/ui_card_page.h"
#include "components/ui_image_view.h"

namespace OHOS {
class UICardContainer : public UIViewGroup {
public:
    UICardContainer();
    ~UICardContainer() override;
    UICardContainer(const UICardContainer &) = delete;
    UICardContainer &operator=(const UICardContainer &) = delete;
    void SwitchCardToBitmap(bool isNeedClip, bool bitMapFromFB = false, float scale = 1.0);
    void SwitchBitmapToCard(void);

    void SetCardPage(UICardPage* page)
    {
        cardPage_ = page;
        Add(cardPage_);
        Resize(page->GetWidth(), page->GetHeight());
    }

    UICardPage* GetCardPage(void)
    {
        return cardPage_;
    }

    UIImageView* GetCardimg(bool isNeedClip)
    {
        if (!isBitMapExist_) {
            SwitchCardToBitmap(isNeedClip);
        }
        return imgView_;
    }

    UIViewType GetViewType() const override
    {
        return UI_CARD_CONTAINER;
    }

    void PreLoad(void);
    void UnLoad(void);

private:
    UIImageView* imgView_ = nullptr;
    UICardPage* cardPage_ = nullptr;
    ImageInfo image_ = {{0}};
    bool isBitMapExist_ = false;
    bool isSrcLoaded_ = false;

    bool OnDragStartEvent(const DragEvent& event) override;
    bool OnDragEvent(const DragEvent& event) override;
    bool OnDragEndEvent(const DragEvent& event) override;
    bool OnLongPressEvent(const LongPressEvent& event) override;
    bool OnClickEvent(const ClickEvent& event) override;
    bool OnPressEvent(const PressEvent& event) override;
    bool OnReleaseEvent(const ReleaseEvent& event) override;
    bool OnCancelEvent(const CancelEvent& event) override;
#if ENABLE_ROTATE_INPUT
    bool OnRotateEvent(const RotateEvent& event) override;
#endif
    void ClipImg(ImageInfo &img, float scale);
};
}
#endif
