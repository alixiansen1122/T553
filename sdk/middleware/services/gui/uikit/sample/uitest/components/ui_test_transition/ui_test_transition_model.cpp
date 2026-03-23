/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: UITestTransitionModel
 * Create: 2025-04
 */

#include <map>
#include <string>
#include <cstdint>
#include <vector>
#include "ohos_errno.h"
#include "securec.h"
#include "UiConfig.h"
#include "wearable_log.h"
#include "ui_test_transition_model.h"

namespace OHOS {
    static const int DIR_MASK = 0xfff00000;
    static const int PNG_MASK = 0x000fffff;
    static const int PNG_3D_FOLD_01 = 0x100001;
    static const int PNG_3D_FOLD_02 = 0x100002;
    static const int PNG_3D_FOLD_03 = 0x100003;
    static const int PNG_3D_FOLD_04 = 0x100004;
    static const int PNG_3D_FOLD_05 = 0x100005;
    static const int PNG_3D_FOLD_06 = 0x100006;
    static const int PNG_3D_FOLD_07 = 0x100007;
    static const int PNG_3D_FOLD_08 = 0x100008;
    static const int PNG_3D_FOLD_09 = 0x100009;
    static const int PNG_3D_FOLD_10 = 0x10000a;
    static const int PNG_CARD_FLIP_01 = 0x10000b;
    static const int PNG_CARD_FLIP_02 = 0x10000c;
    static const int PNG_CARD_FLIP_03 = 0x10000d;
    static const int PNG_CARD_FLIP_04 = 0x10000e;
    static const int PNG_CARD_FLIP_05 = 0x10000f;
    static const int PNG_CARD_FLIP_06 = 0x100010;
    static const int PNG_CARD_FLIP_07 = 0x100011;
    static const int PNG_CARD_FLIP_08 = 0x100012;
    static const int PNG_CARD_FLIP_09 = 0x100013;
    static const int PNG_CARD_FLIP_10 = 0x100014;
    static const int PNG_DESKTOP_BG = 0x100015;
    static const int PNG_FADEINOUT_01 = 0x100016;
    static const int PNG_FADEINOUT_02 = 0x100017;
    static const int PNG_FADEINOUT_03 = 0x100018;
    static const int PNG_FADEINOUT_04 = 0x100019;
    static const int PNG_FADEINOUT_05 = 0x10001a;
    static const int PNG_FADEINOUT_06 = 0x10001b;
    static const int PNG_FADEINOUT_07 = 0x10001c;
    static const int PNG_FADEINOUT_08 = 0x10001d;
    static const int PNG_FADEINOUT_09 = 0x10001e;
    static const int PNG_FADEINOUT_10 = 0x10001f;
    static const int PNG_GAUSSIAN_01 = 0x100020;
    static const int PNG_GAUSSIAN_02 = 0x100021;
    static const int PNG_GAUSSIAN_03 = 0x100022;
    static const int PNG_GAUSSIAN_04 = 0x100023;
    static const int PNG_GAUSSIAN_05 = 0x100024;
    static const int PNG_GAUSSIAN_06 = 0x100025;
    static const int PNG_GAUSSIAN_07 = 0x100026;
    static const int PNG_GAUSSIAN_08 = 0x100027;
    static const int PNG_GAUSSIAN_09 = 0x100028;
    static const int PNG_GAUSSIAN_10 = 0x100029;
    static const int PNG_SLICE_ZOOM_01 = 0x10002a;
    static const int PNG_SLICE_ZOOM_02 = 0x10002b;
    static const int PNG_SLICE_ZOOM_03 = 0x10002c;
    static const int PNG_SLICE_ZOOM_04 = 0x10002d;
    static const int PNG_SLICE_ZOOM_05 = 0x10002e;
    static const int PNG_SLICE_ZOOM_06 = 0x10002f;
    static const int PNG_SLICE_ZOOM_07 = 0x100030;
    static const int PNG_SLICE_ZOOM_08 = 0x100031;
    static const int PNG_SLICE_ZOOM_09 = 0x100032;
    static const int PNG_SLICE_ZOOM_10 = 0x100033;
    static const int PNG_TRAINSTION_END = 0x100034;
    static const int PNG_TRAINSTION_START = 0x100035;
    static const int PNG_TRANSPARENTGRADIENT_01 = 0x100036;
    static const int PNG_TRANSPARENTGRADIENT_02 = 0x100037;
    static const int PNG_TRANSPARENTGRADIENT_03 = 0x100038;
    static const int PNG_TRANSPARENTGRADIENT_04 = 0x100039;
    static const int PNG_TRANSPARENTGRADIENT_05 = 0x10003a;
    static const int PNG_TRANSPARENTGRADIENT_06 = 0x10003b;
    static const int PNG_TRANSPARENTGRADIENT_07 = 0x10003c;
    static const int PNG_TRANSPARENTGRADIENT_08 = 0x10003d;
    static const int PNG_TRANSPARENTGRADIENT_09 = 0x10003e;
    static const int PNG_TRANSPARENTGRADIENT_10 = 0x10003f;

    static std::vector<int> PNG_FADEINOUT_RESIDS = {
        PNG_TRAINSTION_START, PNG_FADEINOUT_01, PNG_FADEINOUT_02, PNG_FADEINOUT_03, PNG_FADEINOUT_04, PNG_FADEINOUT_05,
        PNG_FADEINOUT_06, PNG_FADEINOUT_07, PNG_FADEINOUT_08, PNG_FADEINOUT_09, PNG_FADEINOUT_10, PNG_TRAINSTION_END,
        PNG_TRAINSTION_END, PNG_FADEINOUT_10, PNG_FADEINOUT_09, PNG_FADEINOUT_08, PNG_FADEINOUT_07, PNG_FADEINOUT_06,
        PNG_FADEINOUT_05, PNG_FADEINOUT_04, PNG_FADEINOUT_03, PNG_FADEINOUT_02, PNG_FADEINOUT_01, PNG_TRAINSTION_START
    };

    static std::vector<int> PNG_TRANSPARENTGRADIENT_RESIDS = {
        PNG_TRAINSTION_START, PNG_TRANSPARENTGRADIENT_01, PNG_TRANSPARENTGRADIENT_02, PNG_TRANSPARENTGRADIENT_03,
        PNG_TRANSPARENTGRADIENT_04, PNG_TRANSPARENTGRADIENT_05, PNG_TRANSPARENTGRADIENT_06, PNG_TRANSPARENTGRADIENT_07,
        PNG_TRANSPARENTGRADIENT_08, PNG_TRANSPARENTGRADIENT_09, PNG_TRANSPARENTGRADIENT_10, PNG_TRAINSTION_END,
        PNG_TRAINSTION_END, PNG_TRANSPARENTGRADIENT_10, PNG_TRANSPARENTGRADIENT_09, PNG_TRANSPARENTGRADIENT_08,
        PNG_TRANSPARENTGRADIENT_07, PNG_TRANSPARENTGRADIENT_06, PNG_TRANSPARENTGRADIENT_05, PNG_TRANSPARENTGRADIENT_04,
        PNG_TRANSPARENTGRADIENT_03, PNG_TRANSPARENTGRADIENT_02, PNG_TRANSPARENTGRADIENT_01, PNG_TRAINSTION_START
    };

    static std::vector<int> PNG_GAUSSIAN_RESIDS = {
        PNG_TRAINSTION_START, PNG_GAUSSIAN_01, PNG_GAUSSIAN_02, PNG_GAUSSIAN_03, PNG_GAUSSIAN_04, PNG_GAUSSIAN_05,
        PNG_GAUSSIAN_06, PNG_GAUSSIAN_07, PNG_GAUSSIAN_08, PNG_GAUSSIAN_09, PNG_GAUSSIAN_10, PNG_TRAINSTION_END,
        PNG_TRAINSTION_END, PNG_GAUSSIAN_10, PNG_GAUSSIAN_09, PNG_GAUSSIAN_08, PNG_GAUSSIAN_07, PNG_GAUSSIAN_06,
        PNG_GAUSSIAN_05, PNG_GAUSSIAN_04, PNG_GAUSSIAN_03, PNG_GAUSSIAN_02, PNG_GAUSSIAN_01, PNG_TRAINSTION_START
    };

    static std::vector<int> PNG_CARD_FLIP_RESIDS = {
        PNG_TRAINSTION_START, PNG_CARD_FLIP_01, PNG_CARD_FLIP_02, PNG_CARD_FLIP_03, PNG_CARD_FLIP_04, PNG_CARD_FLIP_05,
        PNG_CARD_FLIP_06, PNG_CARD_FLIP_07, PNG_CARD_FLIP_08, PNG_CARD_FLIP_09, PNG_CARD_FLIP_10, PNG_TRAINSTION_END,
        PNG_TRAINSTION_END, PNG_CARD_FLIP_10, PNG_CARD_FLIP_09, PNG_CARD_FLIP_08, PNG_CARD_FLIP_07, PNG_CARD_FLIP_06,
        PNG_CARD_FLIP_05, PNG_CARD_FLIP_04, PNG_CARD_FLIP_03, PNG_CARD_FLIP_02, PNG_CARD_FLIP_01, PNG_TRAINSTION_START,
    };

    static std::vector<int> PNG_SLICE_ZOOM_RESIDS = {
        PNG_TRAINSTION_START, PNG_SLICE_ZOOM_01, PNG_SLICE_ZOOM_02, PNG_SLICE_ZOOM_03, PNG_SLICE_ZOOM_04,
        PNG_SLICE_ZOOM_05, PNG_SLICE_ZOOM_06, PNG_SLICE_ZOOM_07, PNG_SLICE_ZOOM_08, PNG_SLICE_ZOOM_09,
        PNG_SLICE_ZOOM_10, PNG_TRAINSTION_END,
        PNG_TRAINSTION_END, PNG_SLICE_ZOOM_10, PNG_SLICE_ZOOM_09, PNG_SLICE_ZOOM_08, PNG_SLICE_ZOOM_07,
        PNG_SLICE_ZOOM_06, PNG_SLICE_ZOOM_05, PNG_SLICE_ZOOM_04, PNG_SLICE_ZOOM_03, PNG_SLICE_ZOOM_02,
        PNG_SLICE_ZOOM_01, PNG_TRAINSTION_START,
    };

    static std::vector<int> PNG_3D_FOLD_RESIDS = {
        PNG_TRAINSTION_START, PNG_3D_FOLD_01, PNG_3D_FOLD_02, PNG_3D_FOLD_03, PNG_3D_FOLD_04, PNG_3D_FOLD_05,
        PNG_3D_FOLD_06, PNG_3D_FOLD_07, PNG_3D_FOLD_08, PNG_3D_FOLD_09, PNG_3D_FOLD_10, PNG_TRAINSTION_END,
        PNG_TRAINSTION_END, PNG_3D_FOLD_10, PNG_3D_FOLD_09, PNG_3D_FOLD_08, PNG_3D_FOLD_07, PNG_3D_FOLD_06,
        PNG_3D_FOLD_05, PNG_3D_FOLD_04, PNG_3D_FOLD_03, PNG_3D_FOLD_02, PNG_3D_FOLD_01, PNG_TRAINSTION_START,
    };

    static const std::map<TransitionType, std::pair<std::string, std::vector<int>>> g_transitionsMap = {
        {TransitionType::TRANSITION_FADEINOUT, std::make_pair("淡入淡出", PNG_FADEINOUT_RESIDS)},
        {TransitionType::TRANSITION_TRANSPARENT_GRADIENT, std::make_pair("透明渐变", PNG_TRANSPARENTGRADIENT_RESIDS)},
        {TransitionType::TRANSITION_GAUSSIAN_BLUR, std::make_pair("高斯模糊", PNG_GAUSSIAN_RESIDS)},
        {TransitionType::TRANSITION_CROSS_SHAPED, std::make_pair("十字平滑", PNG_CARD_FLIP_RESIDS)},
        {TransitionType::TRANSITION_ZOOM, std::make_pair("缩放", PNG_SLICE_ZOOM_RESIDS)},
        {TransitionType::TRANSITION_3DFOLD, std::make_pair("3D折叠", PNG_3D_FOLD_RESIDS)},
    };

    UITestTransitionModel::UITestTransitionModel()
    {
        for (const auto& entry: g_transitionsMap) {
            UITestTransitionSample info(static_cast<uint16_t>(entry.first), entry.second.second,\
                                    entry.second.first.c_str(), PNG_TRANSITION_SLICE_SAMPLE);
            transitionInfoList_.push_back(info);
        }
    }
    UITestTransitionModel::~UITestTransitionModel() {}

    UITestTransitionModel& UITestTransitionModel::GetInstance(void)
    {
        static UITestTransitionModel instance;
        return instance;
    }

    std::list<UITestTransitionSample> UITestTransitionModel::GetTransitionListItems()
    {
        return transitionInfoList_;
    }
}