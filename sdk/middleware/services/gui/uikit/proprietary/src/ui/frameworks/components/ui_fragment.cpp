/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: UI fragment
 * Author: Hisi Graphic Team
 * Created: 2025-5
 */
#include "components/ui_fragment.h"
#include "gfx_utils/graphic_log.h"

namespace OHOS {

UIFragment::UIFragment(const std::string& id) : id_(id), state_(FragmentState::INITIALIZED)
{
    if (!id_.empty()) {
        UIFragmentManager::GetInstance().AddFragment(id_, this);
    }
}

UIFragment::~UIFragment()
{
    if (!id_.empty()) {
        UIFragmentManager::GetInstance().RemoveFragment(id_);
    }
}

UIView* UIFragment::CreateView(void* data)
{
    if (state_ == FragmentState::INITIALIZED) {
        OnCreateView(data);
    }
    
    state_ = FragmentState::CREATED;
    return &fragmentView_;
}

void UIFragment::ResumeView()
{
    if (state_ == FragmentState::CREATED) {
        OnResumeView();
    }
    state_ = FragmentState::RESUMED;
}

void UIFragment::PauseView()
{
    if (state_ == FragmentState::RESUMED) {
        OnPauseView();
    }
    state_ = FragmentState::CREATED;
}

void UIFragment::DestroyView()
{
    if ((state_ == FragmentState::CREATED || state_ == FragmentState::RESUMED)) {
        OnDestroyView();
    }
    state_ = FragmentState::INITIALIZED;
}

UIFragmentManager& UIFragmentManager::GetInstance()
{
    static UIFragmentManager instance;
    return instance;
}

void UIFragmentManager::AddFragment(const std::string& id, UIFragment* fragment)
{
    auto it = fragments_.find(id);
    if (it == fragments_.end()) {
        fragments_[id] = fragment;
    } else {
        GRAPHIC_LOGE("fragment %s already exist", id.c_str());
    }
}

void UIFragmentManager::RemoveFragment(const std::string& id)
{
    auto it = fragments_.find(id);
    if (it != fragments_.end()) {
        fragments_.erase(it);
    } else {
        GRAPHIC_LOGE("fragment %s does not exist", id.c_str());
    }
}

UIFragment* UIFragmentManager::GetFragmentById(const std::string& id)
{
    auto it = fragments_.find(id);
    if (it != fragments_.end()) {
        return it->second;
    } else {
        GRAPHIC_LOGE("fragment %s does not exist", id.c_str());
        return nullptr;
    }
}
}