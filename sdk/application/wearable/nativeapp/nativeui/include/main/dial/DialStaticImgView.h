/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef DIAL_STATIC_IMG_VIEW_H
#define DIAL_STATIC_IMG_VIEW_H

#include "components/ui_image_view.h"
#include "font/font_global_manager.h"
#include "main/dial/DialModelTime.h"

namespace OHOS {
class DialStaticImgView : public UIImageView,
                          public FontGlobalManager::LangChangeListener,
                          public TimeChangeListener {
public:
    DialStaticImgView();
    ~DialStaticImgView() override;
    void SetLang(uint8_t lang);
    void OnLangChange(uint8_t langId) override;
    bool SetImgs(const ImageInfo* src, uint8_t num);
    bool OnClickEvent(const ClickEvent& event) override;
    void OnTimeUpdate(const struct tm &curTime) override;

private:
    uint8_t language_ = FontGlobalManager::INVALID_LANG_ID;
    ImageInfo* info_ = nullptr;
    uint8_t infoNum_ = 0;
    int16_t selected_ = -1;
    bool langVisible_ = true;
    bool timeVisible_ = true;
};
}
#endif
