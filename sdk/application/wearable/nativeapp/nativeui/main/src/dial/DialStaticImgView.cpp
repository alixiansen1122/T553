/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "main/dial/DialStaticImgView.h"

namespace OHOS {
DialStaticImgView::DialStaticImgView()
{
    SetAutoEnable(true);
}

DialStaticImgView::~DialStaticImgView()
{
    if (info_ != nullptr) {
        UIFree(info_);
        info_ = nullptr;
    }
}

void DialStaticImgView::SetLang(uint8_t lang)
{
    language_ = lang;
    OnLangChange(FontGlobalManager::GetInstance()->GetCurrentLangId());
}

void DialStaticImgView::OnLangChange(uint8_t langId)
{
    if (language_ == FontGlobalManager::INVALID_LANG_ID) {
        langVisible_ = true;
    } else if (language_ == langId) {
        langVisible_ = true;
    } else {
        langVisible_ = false;
    }
    SetVisible(langVisible_ && timeVisible_);
}

bool DialStaticImgView::SetImgs(const ImageInfo* src, uint8_t num)
{
    if (src == nullptr || num == 0) {
        return false;
    }
    if (info_ != nullptr) {
        UIFree(info_);
        info_ = nullptr;
    }
    uint32_t size = sizeof(ImageInfo) * num;
    info_ = static_cast<ImageInfo*>(UIMalloc(size));
    if (info_ == nullptr) {
        return false;
    }
    if (memcpy_s(info_, size, src, size) != EOK) {
        UIFree(info_);
        return false;
    }
    infoNum_ = num;
    selected_ = 0;
    SetSrc(&info_[selected_]);
    SetTouchable(infoNum_ > 1);
    return true;
}

bool DialStaticImgView::OnClickEvent(const ClickEvent& event)
{
    if (selected_ < 0 || info_ == nullptr || infoNum_ == 0) {
        return false;
    }
    selected_ = (selected_ + 1) % infoNum_;
    SetSrc(&info_[selected_]);
    return true;
}

void DialStaticImgView::OnTimeUpdate(const struct tm &curTime)
{
    if (IsInTimeInterval(curTime)) {
        timeVisible_ = true;
    } else {
        timeVisible_ = false;
    }
    SetVisible(langVisible_ && timeVisible_);
}
}