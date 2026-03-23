/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TransitionUtils
 * Author: Hisi Graphic Team
 * Created: 2025-10
 */

#ifndef TRANSITION_UTILS_H
#define TRANSITION_UTILS_H

#include "components/ui_view.h"
#include "components/ui_view_group.h"

namespace OHOS {
class TransitionUtils {
public:
    static UIView* GetViewAndTransparentizePath(UIView* current, UIViewType viewType)
    {
        if (current->GetViewType() == viewType) {
            current->SetStyle(STYLE_BACKGROUND_OPA, 0);
            return current;
        }

        if (!current->IsViewGroup()) {
            return nullptr;
        }

        UIView* target = nullptr;
        UIView* child = dynamic_cast<UIViewGroup*>(current)->GetChildrenHead();
        while (child != nullptr) {
            target = GetViewAndTransparentizePath(child, viewType);
            if (target != nullptr) {
                current->SetStyle(STYLE_BACKGROUND_OPA, 0);
                break;
            }
            child = child->GetNextSibling();
        }
        return target;
    }
};
}
#endif