/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef DIAL_OPTION_IMG_VIEW_H
#define DIAL_OPTION_IMG_VIEW_H

#include <cstdint>
#include "main/dial/DialView.h"
#include "main/dial/DialModelTime.h"
#include "components/ui_image_view.h"
#include "font/font_global_manager.h"

namespace OHOS {

class DialOptionImgView : public UIImageView,
                          public DialView,
                          public FontGlobalManager::LangChangeListener,
                          public TimeChangeListener {
public:
    DialOptionImgView();
    ~DialOptionImgView() override;

    void HandleFloatData(float data) override;

    bool SetOptionRes(const ImageInfo* src, uint16_t num);

    void SetLang(uint8_t lang);
    
    void OnLangChange(uint8_t langId) override;

    void OnTimeUpdate(const struct tm &curTime) override;

private:
    ImageInfo* info_ = nullptr;
    uint16_t infoNum_ = 0;
    int16_t selected_ = -1;
    uint8_t language_ = FontGlobalManager::INVALID_LANG_ID;
    bool langVisible_ = true;
    bool timeVisible_ = true;
};
}
#endif
/**
 * @}
 */
