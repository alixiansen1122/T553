/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: UITestTransitionModel
 * Create: 2025-04
 */

#ifndef UI_TEST_TRANSITION_MODEL_H
#define UI_TEST_TRANSITION_MODEL_H

#include <list>
#include <vector>
#include "TransitionType.h"
#include "ui_test_transition_sample.h"

namespace OHOS {

    class UITestTransitionModel {
    public:
        static UITestTransitionModel &GetInstance(void);
        std::list<UITestTransitionSample> GetTransitionListItems();
    private:
        UITestTransitionModel();
        UITestTransitionModel(const UITestTransitionModel &);
        UITestTransitionModel &operator=(const UITestTransitionModel &);
        virtual ~UITestTransitionModel();

        std::list<UITestTransitionSample> transitionInfoList_;
    };
}

#endif // UI_TEST_TRANSITION_MODEL_H