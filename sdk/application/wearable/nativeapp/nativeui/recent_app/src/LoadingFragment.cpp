/*
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: LoadingFragment.cpp
 * Author:
 * Create: 2025-07-17
 */

#include "recent_app/LoadingFragment.h"
#include "wearable_log.h"

namespace OHOS {
static constexpr uint8_t DEFAULT_TEXT_SIZE = 30;

void LoadingFragment::OnCreateView(void* data)
{
    loadingLabel_ = new UILabel();
    if (loadingLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new UILabel fail");
        return;
    }
    fragmentView_.Add(loadingLabel_);
    loadingLabel_->SetText("Loading...");
    loadingLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, DEFAULT_TEXT_SIZE);
    loadingLabel_->Resize(300, 100); // 300, 100: size
    loadingLabel_->LayoutCenterOfParent();
    loadingLabel_->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER,
        UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
}

void LoadingFragment::OnDestroyView()
{
    if (loadingLabel_ != nullptr) {
        delete loadingLabel_;
    }
}
}