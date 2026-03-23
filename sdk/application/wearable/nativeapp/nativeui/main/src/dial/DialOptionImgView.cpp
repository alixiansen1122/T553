/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "main/dial/DialOptionImgView.h"
#include "securec.h"

namespace OHOS {
DialOptionImgView::DialOptionImgView(void)
{
    SetAutoEnable(true);
}

DialOptionImgView::~DialOptionImgView(void)
{
    if (info_ != nullptr) {
        UIFree(info_);
        info_ = nullptr;
    }
}

bool DialOptionImgView::SetOptionRes(const ImageInfo* src, uint16_t num)
{
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
    return true;
}

void DialOptionImgView::HandleFloatData(float data)
{
    if (info_ == nullptr) {
        return;
    }
    int16_t selected = (int16_t)data;
    if ((selected > infoNum_) || (selected < 0)) {
        return;
    }
    if (selected_ == selected) {
        return;
    }
    selected_ = selected;
    SetSrc(&info_[selected_]);
}

void DialOptionImgView::SetLang(uint8_t lang)
{
    language_ = lang;
    OnLangChange(FontGlobalManager::GetInstance()->GetCurrentLangId());
}

void DialOptionImgView::OnLangChange(uint8_t langId)
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

void DialOptionImgView::OnTimeUpdate(const struct tm &curTime)
{
    if (IsInTimeInterval(curTime)) {
        timeVisible_ = true;
    } else {
        timeVisible_ = false;
    }
    SetVisible(langVisible_ && timeVisible_);
}
}