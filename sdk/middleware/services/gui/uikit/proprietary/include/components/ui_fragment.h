/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: UI fragment
 * Author: Hisi Graphic Team
 * Created: 2025-5
 */
#ifndef UI_FRAGMENT_H
#define UI_FRAGMENT_H

#include <unordered_map>
#include "components/ui_view_group.h"

namespace OHOS {
// UIFragment represents a reusable fragment of UI content, it has its own life cycle, and can handle its own event
enum class FragmentState : uint8_t {
    INITIALIZED,
    CREATED,
    RESUMED
};

class UIFragment : public HeapBase {
public:
    /**
     * @brief Constructor of UIFragment without parameter, it will not be added into UIFragmentManager
     */
    UIFragment() : id_(), state_(FragmentState::INITIALIZED) {}

    /**
     * @brief Constructor of UIFragment with parameter, it will be added into UIFragmentManager if strlen(id) > 0
     * @param id Used to distinguish and get different UIFragment Instance from UIFragmentManager
     */
    UIFragment(const std::string& id);

    virtual ~UIFragment();

    /**
     * @brief Invoked to call OnCreateView if state is INITIALIZED.
     */
    UIView* CreateView(void *data = nullptr);

    /**
     * @brief Invoked to call OnResumeView if state is CREATED
     */
    void ResumeView();

    /**
     * @brief Invoked to call OnPauseView if state is RESUMED
     */
    void PauseView();

    /**
     * @brief Invoked to call OnDestroyView if state is RESUMED or CREATED.
     */
    void DestroyView();

    UIView* GetFragmentView()
    {
        return &fragmentView_;
    }

    FragmentState GetState()
    {
        return state_;
    }

    UIFragment(const UIFragment&) = delete;
    UIFragment& operator=(const UIFragment&) = delete;
    UIFragment(UIFragment&&) = delete;
    UIFragment& operator=(UIFragment&&) = delete;

protected:
    /**
     * @brief Recommended for creating views and add them into UIViewGroup fragmentView_.
     */
    virtual void OnCreateView(void* data) = 0;

    /**
     * @brief Recommended for setting input event listener, requesting focus, or starting animator here.
     */
    virtual void OnResumeView() {}

    /**
     * @brief Recommended for clearing input event listener, clearing focus, or stopping animator here.
     */
    virtual void OnPauseView() {}

    /**
     * @brief Recommended for destroying UIView or UIViewGroup which belong to UIFragment.
     */
    virtual void OnDestroyView() = 0;
    UIViewGroup fragmentView_;

private:
    std::string id_;
    FragmentState state_;
};

// UIFragmentManager can be used to fetch fragment by id
class UIFragmentManager : public HeapBase {
public:
    static UIFragmentManager& GetInstance();
    void AddFragment(const std::string& id, UIFragment* fragment);
    void RemoveFragment(const std::string& id);
    UIFragment* GetFragmentById(const std::string& id);
    uint32_t GetFragmentsCount()
    {
        return fragments_.size();
    }
private:
    UIFragmentManager() = default;
    std::unordered_map<std::string, UIFragment*> fragments_;
};
}
#endif