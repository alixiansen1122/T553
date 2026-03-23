/*
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: LoadingFragment.h
 * Author:
 * Create: 2025-07-17
 */

#ifndef LOADING_FRAGMENT_H
#define LOADING_FRAGMENT_H

#include "components/ui_fragment.h"
#include "components/ui_label.h"

namespace OHOS {

class LoadingFragment : public UIFragment {
public:

protected:
    void OnCreateView(void* data) override;
    void OnDestroyView() override;

private:
    UILabel* loadingLabel_ = nullptr;
};
} // namespace OHOS
#endif // LOADING_FRAGMENT_H