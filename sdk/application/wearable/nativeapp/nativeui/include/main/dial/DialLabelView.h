/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef DIAL_LABEL_VIEW_H
#define DIAL_LABEL_VIEW_H

#include <cstdint>
#include "main/dial/DialView.h"
#include "components/ui_label.h"
#include "font/font_global_manager.h"

namespace OHOS {
class DialLabelView : public UILabel, public DialView, public FontGlobalManager::LangChangeListener {
public:
    DialLabelView();
    ~DialLabelView() override;

    bool SetText(const char* text);
    bool SetTextWithWildcard(const char* text);
    void HandleTextData(const std::string* data, uint16_t num) override;
    void HandleFloatData(const float* data, uint16_t num) override;
    void HandleFloatData(float data) override;
    void SetLang(uint8_t lang);
    void OnLangChange(uint8_t langId) override;

private:
    void SetTextWithWildcardInner(const char* text);
    static const uint8_t MAX_BUF_SIZE = 255;
    char* text_ = nullptr;
    char dstBuf_[MAX_BUF_SIZE] = {0};
    uint8_t language_ = FontGlobalManager::INVALID_LANG_ID;
};
}
#endif
