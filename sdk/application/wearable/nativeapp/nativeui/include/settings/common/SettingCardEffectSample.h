/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingCardEffectSample
 * Create: 2025-04
 */
#ifndef SETTING_CARD_EFFECT_SAMPLE_H
#define SETTING_CARD_EFFECT_SAMPLE_H
#include <cstdint>
#include <vector>

namespace OHOS {
enum class CardEffectID {
    UI_INVALIDE = -1,
    UI_ZOOM = 0,
    UI_CARD_FLIP = 1,
    UI_CUBE = 2,
    UI_TURN_PAGE = 3,
    UI_PUSH_PRESS = 4,
    UI_WINDMILL = 5,
};

struct SettingCardEffectSample {
    CardEffectID viewId;
    std::vector<int> resIds;
    const char* label;
    int backgroundImageId;
    const char* resourceImage;
    bool selected;

    explicit SettingCardEffectSample(CardEffectID viewId = CardEffectID::UI_INVALIDE, std::vector<int> resIds = {},
               const char* label = nullptr, const int backgroundImageId = 0, const char* resourceImage = nullptr,
               bool selected = false)
        : viewId(viewId),
          resIds(std::move(resIds)),
          label(label),
          backgroundImageId(backgroundImageId),
          resourceImage(resourceImage),
          selected(selected)
    {}
};
}
#endif