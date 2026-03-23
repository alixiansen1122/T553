/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: UITestTransitionSample
 * Create: 2025-04
 */

#ifndef UI_TEST_TRANSITION_SAMPLE_H
#define UI_TEST_TRANSITION_SAMPLE_H

#include <list>
#include <vector>

namespace OHOS {

    struct UITestTransitionSample {
        uint16_t viewId;
        std::vector<int> resIds;
        const char* label;
        const char* resourceImage;

        explicit UITestTransitionSample(uint16_t viewId = 0, std::vector<int> resIds = {},
                const char* label = nullptr, const char* resourceImage = nullptr)
            : viewId(viewId),
            resIds(std::move(resIds)),
            label(label),
            resourceImage(resourceImage)
            {}
    };
}

#endif // UI_TEST_TRANSITION_SAMPLE_H