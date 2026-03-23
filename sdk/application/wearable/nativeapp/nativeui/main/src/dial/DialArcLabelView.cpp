/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "main/dial/DialArcLabelView.h"
#include "securec.h"

namespace OHOS {
DialArcLabelView::DialArcLabelView(void)
{
}

DialArcLabelView::~DialArcLabelView(void)
{
    if (text_ != nullptr) {
        UIFree(text_);
    }
}

bool DialArcLabelView::SetText(const char* text)
{
    if (text == nullptr) {
        return false;
    }
    UIArcLabel::SetText(text);
    return true;
}

void DialArcLabelView::SetTextWithWildcardInner(const char* text)
{
    if (strcmp(text, dstBuf_) == 0) {
        return;
    }
    memset_s(dstBuf_, sizeof(dstBuf_), 0, sizeof(dstBuf_));
    if (strcpy_s(dstBuf_, MAX_BUF_SIZE, text) != EOK) {
        GRAPHIC_LOGE("strcpy_s failed!\n");
        return;
    }
    UIArcLabel::SetText(dstBuf_);
}

bool DialArcLabelView::SetTextWithWildcard(const char* text)
{
    if (text == nullptr) {
        return false;
    }
    if (text_ != nullptr) {
        if (strcmp(text, text_) == 0) {
            return true;
        }
        UIFree(text_);
        text_ = nullptr;
    }

    uint32_t textLen = static_cast<uint32_t>(strlen(text));
    if (textLen > (MAX_BUF_SIZE - 1)) {
        return false;
    }
    text_ = static_cast<char*>(UIMalloc(++textLen));
    if (text_ == nullptr) {
        return false;
    }
    if (strcpy_s(text_, textLen, text) != EOK) {
        GRAPHIC_LOGE("strcpy_s failed");
        UIFree(text_);
        text_ = nullptr;
        return false;
    }
    return true;
}

void DialArcLabelView::HandleTextData(const std::string* data, uint16_t num)
{
    char tempBuf[MAX_BUF_SIZE] = {0};
    if (num == 1) {
        if (snprintf_s(tempBuf, MAX_BUF_SIZE, MAX_BUF_SIZE - 1, text_, data[0].c_str()) < 0) {
            GRAPHIC_LOGE("snprintf_s failed.\n");
            return;
        }
    } else if (num == 2) { // 2: wildcard cnt
        if (snprintf_s(tempBuf, MAX_BUF_SIZE, MAX_BUF_SIZE - 1, text_, data[0].c_str(), data[1].c_str()) < 0) {
            GRAPHIC_LOGE("snprintf_s failed.\n");
            return;
        }
    } else if (num == 3) { // 3: wildcard cnt
        if (snprintf_s(tempBuf, MAX_BUF_SIZE, MAX_BUF_SIZE - 1, text_,
            data[0].c_str(), data[1].c_str(), data[2].c_str()) < 0) { // 2: index
            GRAPHIC_LOGE("snprintf_s failed.\n");
            return;
        }
    } else {
        GRAPHIC_LOGE("max size is 3.\n");
        return;
    }
    SetTextWithWildcardInner(tempBuf);
}

void DialArcLabelView::HandleFloatData(const float* data, uint16_t num)
{
    char tempBuf[MAX_BUF_SIZE] = {0};
    if (num == 1) {
        if (snprintf_s(tempBuf, MAX_BUF_SIZE, MAX_BUF_SIZE - 1, text_, data[0]) < 0) {
            GRAPHIC_LOGE("snprintf_s failed.\n");
            return;
        }
    } else if (num == 2) { // 2: wildcard cnt
        if (snprintf_s(tempBuf, MAX_BUF_SIZE, MAX_BUF_SIZE - 1, text_, data[0], data[1]) < 0) {
            GRAPHIC_LOGE("snprintf_s failed.\n");
            return;
        }
    } else if (num == 3) { // 3: wildcard cnt
        if (snprintf_s(tempBuf, MAX_BUF_SIZE, MAX_BUF_SIZE - 1, text_, data[0], data[1], data[2]) < 0) { // 2: index
            GRAPHIC_LOGE("snprintf_s failed.\n");
            return;
        }
    } else {
        GRAPHIC_LOGE("max size is 3.\n");
        return;
    }
    SetTextWithWildcardInner(tempBuf);
}

void DialArcLabelView::HandleFloatData(float data)
{
    char tempBuf[MAX_BUF_SIZE] = {0};
    if (snprintf_s(tempBuf, MAX_BUF_SIZE, MAX_BUF_SIZE - 1, text_, data) < 0) {
        GRAPHIC_LOGE("snprintf_s failed.\n");
        return;
    }
    SetTextWithWildcardInner(tempBuf);
}

void DialArcLabelView::SetLang(uint8_t lang)
{
    language_ = lang;
    OnLangChange(FontGlobalManager::GetInstance()->GetCurrentLangId());
}

void DialArcLabelView::OnLangChange(uint8_t langId)
{
    if (language_ == FontGlobalManager::INVALID_LANG_ID) {
        SetVisible(true);
    } else if (language_ == langId) {
        SetVisible(true);
    } else {
        SetVisible(false);
    }
}
}