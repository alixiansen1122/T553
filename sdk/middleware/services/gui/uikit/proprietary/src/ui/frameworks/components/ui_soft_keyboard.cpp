/*
 * Copyright (c) 2022 CompanyNameMagicTag.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "input_method/ui_soft_keyboard.h"
#include "input_method/ui_keyboard_codetable.h"

#if ENABLE_SOFT_KEYBOARD
namespace OHOS {
constexpr uint16_t CIR_FIRST_ROW_Y = 220;
constexpr uint16_t CIR_SECOND_ROW_Y = 260;
constexpr uint16_t CIR_THIRD_ROW_Y = 300;
constexpr uint16_t CIR_FOURTH_ROW_Y = 340;
constexpr uint16_t CIR_FIFTH_ROW_Y = 380;
constexpr uint16_t CIR_KEY_W = 38;
constexpr uint16_t CIR_KEY_H = 38;
constexpr uint16_t CIR_KEY_W_40 = 40;
constexpr uint16_t CIR_KEY_W_50 = 50;
constexpr uint16_t CIR_KEY_W_60 = 60;

constexpr uint16_t ROW_1_COL_1_X = 10;
constexpr uint16_t ROW_1_COL_2_X = 50;
constexpr uint16_t ROW_1_COL_3_X = 90;
constexpr uint16_t ROW_1_COL_4_X = 130;
constexpr uint16_t ROW_1_COL_5_X = 170;
constexpr uint16_t ROW_1_COL_6_X = 210;
constexpr uint16_t ROW_1_COL_7_X = 250;
constexpr uint16_t ROW_1_COL_8_X = 290;
constexpr uint16_t ROW_1_COL_9_X = 330;
constexpr uint16_t ROW_1_COL_10_X = 370;
constexpr uint16_t ROW_1_COL_11_X = 410;
constexpr uint16_t IME_X = 136;
constexpr uint16_t IME_1_X = 188;
constexpr uint16_t IME_2_X = 230;
constexpr uint16_t IME_3_X = 272;
constexpr uint16_t SPACE_X = 168;
constexpr uint16_t SEND_X = 230;

constexpr uint16_t NUM_0_X = 30;
constexpr uint16_t NUM_1_X = 70;
constexpr uint16_t NUM_2_X = 110;
constexpr uint16_t NUM_3_X = 150;
constexpr uint16_t NUM_4_X = 190;
constexpr uint16_t NUM_5_X = 230;
constexpr uint16_t NUM_6_X = 270;
constexpr uint16_t NUM_7_X = 310;
constexpr uint16_t NUM_8_X = 350;
constexpr uint16_t NUM_9_X = 390;

constexpr uint16_t NUM_IME_X = 136;
constexpr uint16_t NUM_IME_1_X = 188;
constexpr uint16_t NUM_IME_2_X = 230;
constexpr uint16_t NUM_IME_3_X = 272;
constexpr uint16_t NUM_SPACE_X = 168;
constexpr uint16_t NUM_SEND_X = 230;

constexpr uint16_t KEY_WIDTH = 76;
constexpr uint16_t KEY_HEIGHT = 52;
constexpr uint16_t ROW_1_Y = 260;
constexpr uint16_t ROW_2_Y = 315;
constexpr uint16_t ROW_3_Y = 370;
constexpr uint16_t COL_1_X = 27;
constexpr uint16_t COL_2_X = 107;
constexpr uint16_t COL_3_X = 187;
constexpr uint16_t COL_4_X = 267;
constexpr uint16_t COL_5_X = 347;

UICircularCapSoftKeyboard* UICircularCapSoftKeyboard::GetInstance()
{
    static UICircularCapSoftKeyboard instance;
    return &instance;
}

void UICircularCapSoftKeyboard::CreateKeyButtons()
{
    std::vector<SoftKey> keySet{
        {{ROW_1_COL_1_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_Q}},
        {{ROW_1_COL_2_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_W}},
        {{ROW_1_COL_3_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_E}},
        {{ROW_1_COL_4_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_R}},
        {{ROW_1_COL_5_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_T}},
        {{ROW_1_COL_6_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_Y}},
        {{ROW_1_COL_7_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_U}},
        {{ROW_1_COL_8_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_I}},
        {{ROW_1_COL_9_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_O}},
        {{ROW_1_COL_10_X, CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_P}},

        {{ROW_1_COL_2_X,  CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_A}},
        {{ROW_1_COL_3_X,  CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_S}},
        {{ROW_1_COL_4_X,  CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_D}},
        {{ROW_1_COL_5_X,  CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_F}},
        {{ROW_1_COL_6_X,  CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_G}},
        {{ROW_1_COL_7_X,  CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_H}},
        {{ROW_1_COL_8_X,  CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_J}},
        {{ROW_1_COL_9_X,  CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_K}},
        {{ROW_1_COL_10_X, CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_L}},

        {{ROW_1_COL_3_X,  CIR_THIRD_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_Z}},
        {{ROW_1_COL_4_X,  CIR_THIRD_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_X}},
        {{ROW_1_COL_5_X,  CIR_THIRD_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_C}},
        {{ROW_1_COL_6_X,  CIR_THIRD_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_V}},
        {{ROW_1_COL_7_X,  CIR_THIRD_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_B}},
        {{ROW_1_COL_8_X,  CIR_THIRD_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_N}},
        {{ROW_1_COL_9_X,  CIR_THIRD_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_M}},

        {{ROW_1_COL_11_X, CIR_FIRST_ROW_Y, CIR_KEY_W_40, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_DEL}},

        {{IME_X,   CIR_FOURTH_ROW_Y, CIR_KEY_W_50, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_SOFT_KEYBOARD}},
        {{IME_1_X, CIR_FOURTH_ROW_Y, CIR_KEY_W_40, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_EN}},
        {{IME_2_X, CIR_FOURTH_ROW_Y, CIR_KEY_W_40, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_NUMBER}},
        {{IME_3_X, CIR_FOURTH_ROW_Y, CIR_KEY_W_40, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_SYMBOL}},

        {{SPACE_X, CIR_FIFTH_ROW_Y, CIR_KEY_W_60, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_SPACE}},
        {{SEND_X,  CIR_FIFTH_ROW_Y, CIR_KEY_W_60, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_ENTER}},
    };

    for (auto key : keySet) {
        SetupKeyButton(key);
    }
}

UICircularEngSoftKeyboard* UICircularEngSoftKeyboard::GetInstance()
{
    static UICircularEngSoftKeyboard instance;
    return &instance;
}

void UICircularEngSoftKeyboard::CreateKeyButtons()
{
    std::vector<SoftKey> keySet{
        {{ROW_1_COL_1_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_q}},
        {{ROW_1_COL_2_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_w}},
        {{ROW_1_COL_3_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_e}},
        {{ROW_1_COL_4_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_r}},
        {{ROW_1_COL_5_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_t}},
        {{ROW_1_COL_6_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_y}},
        {{ROW_1_COL_7_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_u}},
        {{ROW_1_COL_8_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_i}},
        {{ROW_1_COL_9_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_o}},
        {{ROW_1_COL_10_X, CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_p}},

        {{ROW_1_COL_2_X,  CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_a}},
        {{ROW_1_COL_3_X,  CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_s}},
        {{ROW_1_COL_4_X,  CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_d}},
        {{ROW_1_COL_5_X,  CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_f}},
        {{ROW_1_COL_6_X,  CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_g}},
        {{ROW_1_COL_7_X,  CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_h}},
        {{ROW_1_COL_8_X,  CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_j}},
        {{ROW_1_COL_9_X,  CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_k}},
        {{ROW_1_COL_10_X, CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_l}},

        {{ROW_1_COL_3_X,  CIR_THIRD_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_z}},
        {{ROW_1_COL_4_X,  CIR_THIRD_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_x}},
        {{ROW_1_COL_5_X,  CIR_THIRD_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_c}},
        {{ROW_1_COL_6_X,  CIR_THIRD_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_v}},
        {{ROW_1_COL_7_X,  CIR_THIRD_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_b}},
        {{ROW_1_COL_8_X,  CIR_THIRD_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_n}},
        {{ROW_1_COL_9_X,  CIR_THIRD_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_m}},

        {{ROW_1_COL_11_X, CIR_FIRST_ROW_Y, CIR_KEY_W_40, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_DEL}},

        {{IME_X,   CIR_FOURTH_ROW_Y, CIR_KEY_W_50, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_SOFT_KEYBOARD}},
        {{IME_1_X, CIR_FOURTH_ROW_Y, CIR_KEY_W_40, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_CAP}},
        {{IME_2_X, CIR_FOURTH_ROW_Y, CIR_KEY_W_40, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_NUMBER}},
        {{IME_3_X, CIR_FOURTH_ROW_Y, CIR_KEY_W_40, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_SYMBOL}},

        {{SPACE_X, CIR_FIFTH_ROW_Y, CIR_KEY_W_60, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_SPACE}},
        {{SEND_X,  CIR_FIFTH_ROW_Y, CIR_KEY_W_60, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_ENTER}},
    };

    for (auto key : keySet) {
        SetupKeyButton(key);
    }
}

UICircularSymbolSoftKeyboard* UICircularSymbolSoftKeyboard::GetInstance()
{
    static UICircularSymbolSoftKeyboard instance;
    return &instance;
}

void UICircularSymbolSoftKeyboard::CreateKeyButtons()
{
    std::vector<SoftKey> keySet {
        {{ROW_1_COL_1_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_EXCLAM}},
        {{ROW_1_COL_2_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_DOUBLEQUOTE}},
        {{ROW_1_COL_3_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_NUMBERSIGN}},
        {{ROW_1_COL_4_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_DOLLAR}},
        {{ROW_1_COL_5_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_PERCENT}},
        {{ROW_1_COL_6_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_AMPERSAND}},
        {{ROW_1_COL_7_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_QUOTE}},
        {{ROW_1_COL_8_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_PARENLEFT}},
        {{ROW_1_COL_9_X,  CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_PARENRIGHT}},
        {{ROW_1_COL_10_X, CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_COMMA}},

        {{ROW_1_COL_2_X,  CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_COLON}},
        {{ROW_1_COL_3_X,  CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_SEMICOLON}},
        {{ROW_1_COL_4_X,  CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_LESS}},
        {{ROW_1_COL_5_X,  CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_GREATER}},
        {{ROW_1_COL_6_X,  CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_QUESTION}},
        {{ROW_1_COL_7_X,  CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_AT}},
        {{ROW_1_COL_8_X,  CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_BRACKETLEFT}},
        {{ROW_1_COL_9_X,  CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_BACKSLASH}},
        {{ROW_1_COL_10_X, CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_BRACKETRIGHT}},

        {{ROW_1_COL_3_X,  CIR_THIRD_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_CARET}},
        {{ROW_1_COL_4_X,  CIR_THIRD_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_UNDERSCORE}},
        {{ROW_1_COL_5_X,  CIR_THIRD_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_BACKQUOTE}},
        {{ROW_1_COL_6_X,  CIR_THIRD_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_BRACELEFT}},
        {{ROW_1_COL_7_X,  CIR_THIRD_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_BAR}},
        {{ROW_1_COL_8_X,  CIR_THIRD_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_BRACERIGHT}},
        {{ROW_1_COL_9_X,  CIR_THIRD_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_TILDE}},

        {{ROW_1_COL_11_X, CIR_FIRST_ROW_Y, CIR_KEY_W_40, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_DEL}},

        {{IME_X,   CIR_FOURTH_ROW_Y, CIR_KEY_W_50, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_SOFT_KEYBOARD}},
        {{IME_1_X, CIR_FOURTH_ROW_Y, CIR_KEY_W_40, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_CAP}},
        {{IME_2_X, CIR_FOURTH_ROW_Y, CIR_KEY_W_40, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_EN}},
        {{IME_3_X, CIR_FOURTH_ROW_Y, CIR_KEY_W_40, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_NUMBER}},

        {{SPACE_X, CIR_FIFTH_ROW_Y, CIR_KEY_W_60, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_SPACE}},
        {{SEND_X,  CIR_FIFTH_ROW_Y, CIR_KEY_W_60, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_ENTER}},
    };
    for (auto key : keySet) {
        SetupKeyButton(key);
    }
}

UICircularNumberSoftKeyboard* UICircularNumberSoftKeyboard::GetInstance()
{
    static UICircularNumberSoftKeyboard instance;
    return &instance;
}

void UICircularNumberSoftKeyboard::CreateKeyButtons()
{
    std::vector<SoftKey> keySet {
        {{NUM_0_X, CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_0}},
        {{NUM_1_X, CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_1}},
        {{NUM_2_X, CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_2}},
        {{NUM_3_X, CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_3}},
        {{NUM_4_X, CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_4}},
        {{NUM_5_X, CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_5}},
        {{NUM_6_X, CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_6}},
        {{NUM_7_X, CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_7}},
        {{NUM_8_X, CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_8}},
        {{NUM_9_X, CIR_FIRST_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_9}},

        {{NUM_2_X, CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_PLUS}},
        {{NUM_3_X, CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_MINUS}},
        {{NUM_4_X, CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_ASTERISK}},
        {{NUM_5_X, CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_SLASH}},
        {{NUM_6_X, CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_DOT}},
        {{NUM_7_X, CIR_SECOND_ROW_Y, CIR_KEY_W, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_EQUAL}},

        {{NUM_IME_X,   CIR_THIRD_ROW_Y, CIR_KEY_W_50, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_SOFT_KEYBOARD}},
        {{NUM_IME_1_X, CIR_THIRD_ROW_Y, CIR_KEY_W_40, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_CAP}},
        {{NUM_IME_2_X, CIR_THIRD_ROW_Y, CIR_KEY_W_40, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_EN}},
        {{NUM_IME_3_X, CIR_THIRD_ROW_Y, CIR_KEY_W_40, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_SYMBOL}},

        {{NUM_SPACE_X, CIR_FOURTH_ROW_Y, CIR_KEY_W_60, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_DEL}},
        {{NUM_SEND_X,  CIR_FOURTH_ROW_Y, CIR_KEY_W_60, CIR_KEY_H}, {UI_KEYCODE::UI_KEY_ENTER}},
    };

    for (auto key : keySet) {
        SetupKeyButton(key);
    }
}

UIEnglishSoftKeyboard* UIEnglishSoftKeyboard::GetInstance()
{
    static UIEnglishSoftKeyboard instance;
    return &instance;
}

void UIEnglishSoftKeyboard::CreateKeyButtons()
{
    std::vector<SoftKey> keySet {
        {{COL_2_X, ROW_1_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_CAP,
            UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_NUMBER, UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_SYMBOL,
            UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_PINYIN}},
        {{COL_3_X, ROW_1_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_a, UI_KEYCODE::UI_KEY_b, UI_KEYCODE::UI_KEY_c}},
        {{COL_4_X, ROW_1_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_d, UI_KEYCODE::UI_KEY_e, UI_KEYCODE::UI_KEY_f}},
        {{COL_2_X, ROW_2_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_g, UI_KEYCODE::UI_KEY_h, UI_KEYCODE::UI_KEY_i}},
        {{COL_3_X, ROW_2_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_j, UI_KEYCODE::UI_KEY_k, UI_KEYCODE::UI_KEY_l}},
        {{COL_4_X, ROW_2_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_m, UI_KEYCODE::UI_KEY_n, UI_KEYCODE::UI_KEY_o}},
        {{COL_2_X, ROW_3_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_p, UI_KEYCODE::UI_KEY_q, UI_KEYCODE::UI_KEY_r,
            UI_KEYCODE::UI_KEY_s}},
        {{COL_3_X, ROW_3_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_t, UI_KEYCODE::UI_KEY_u, UI_KEYCODE::UI_KEY_v}},
        {{COL_4_X, ROW_3_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_w, UI_KEYCODE::UI_KEY_x, UI_KEYCODE::UI_KEY_y,
            UI_KEYCODE::UI_KEY_z}},
        //  FUNCTION_KEY
        {{COL_1_X, ROW_1_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_DEL}},
        {{COL_5_X, ROW_1_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_ENTER}},
    };
    for (auto key : keySet) {
        SetupKeyButton(key);
    }
}

UICapitalSoftKeyboard* UICapitalSoftKeyboard::GetInstance()
{
    static UICapitalSoftKeyboard instance;
    return &instance;
}

void UICapitalSoftKeyboard::CreateKeyButtons()
{
    std::vector<SoftKey> keySet {
        {{COL_2_X, ROW_1_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_EN,
            UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_NUMBER, UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_SYMBOL,
            UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_PINYIN}},
        {{COL_3_X, ROW_1_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_A, UI_KEYCODE::UI_KEY_B, UI_KEYCODE::UI_KEY_C}},
        {{COL_4_X, ROW_1_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_D, UI_KEYCODE::UI_KEY_E, UI_KEYCODE::UI_KEY_F}},
        {{COL_2_X, ROW_2_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_G, UI_KEYCODE::UI_KEY_H, UI_KEYCODE::UI_KEY_I}},
        {{COL_3_X, ROW_2_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_J, UI_KEYCODE::UI_KEY_K, UI_KEYCODE::UI_KEY_L}},
        {{COL_4_X, ROW_2_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_M, UI_KEYCODE::UI_KEY_N, UI_KEYCODE::UI_KEY_O}},
        {{COL_2_X, ROW_3_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_P, UI_KEYCODE::UI_KEY_Q, UI_KEYCODE::UI_KEY_R,
            UI_KEYCODE::UI_KEY_S}},
        {{COL_3_X, ROW_3_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_T, UI_KEYCODE::UI_KEY_U, UI_KEYCODE::UI_KEY_V}},
        {{COL_4_X, ROW_3_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_W, UI_KEYCODE::UI_KEY_X, UI_KEYCODE::UI_KEY_Y,
            UI_KEYCODE::UI_KEY_Z}},
        //  FUNCTION_KEY
        {{COL_1_X, ROW_1_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_DEL}},
        {{COL_5_X, ROW_1_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_ENTER}},
    };
    for (auto key : keySet) {
        SetupKeyButton(key);
    }
}

UINumberSoftKeyboard* UINumberSoftKeyboard::GetInstance()
{
    static UINumberSoftKeyboard instance;
    return &instance;
}

void UINumberSoftKeyboard::CreateKeyButtons()
{
    std::vector<SoftKey> keySet {
        {{COL_2_X, ROW_1_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_1, UI_KEYCODE::UI_KEY_2, UI_KEYCODE::UI_KEY_3}},
        {{COL_3_X, ROW_1_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_4, UI_KEYCODE::UI_KEY_5, UI_KEYCODE::UI_KEY_6}},
        {{COL_4_X, ROW_1_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_7, UI_KEYCODE::UI_KEY_8, UI_KEYCODE::UI_KEY_9}},
        {{COL_2_X, ROW_2_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_PLUS, UI_KEYCODE::UI_KEY_MINUS,
            UI_KEYCODE::UI_KEY_DOT}},
        {{COL_3_X, ROW_2_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_0}},
        {{COL_4_X, ROW_2_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_ASTERISK, UI_KEYCODE::UI_KEY_SLASH,
            UI_KEYCODE::UI_KEY_EQUAL}},
        {{COL_2_X, ROW_3_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_CAP,
                                                     UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_EN}},
        {{COL_3_X, ROW_3_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_PINYIN}},
        {{COL_4_X, ROW_3_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_SYMBOL}},
        //  FUNCTION_KEY
        {{COL_1_X, ROW_1_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_DEL}},
        {{COL_5_X, ROW_1_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_ENTER}},
    };
    for (auto key : keySet) {
        SetupKeyButton(key);
    }
}

UISymbolSoftKeyboard* UISymbolSoftKeyboard::GetInstance()
{
    static UISymbolSoftKeyboard instance;
    return &instance;
}

void UISymbolSoftKeyboard::CreateKeyButtons()
{
    std::vector<SoftKey> keySet {
        {{COL_2_X, ROW_1_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_CAP,
            UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_EN, UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_PINYIN,
            UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_NUMBER}},
        {{COL_3_X, ROW_1_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_EXCLAM, UI_KEYCODE::UI_KEY_DOUBLEQUOTE,
            UI_KEYCODE::UI_KEY_NUMBERSIGN}},
        {{COL_4_X, ROW_1_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_DOLLAR, UI_KEYCODE::UI_KEY_PERCENT,
            UI_KEYCODE::UI_KEY_AMPERSAND}},
        {{COL_2_X, ROW_2_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_QUOTE, UI_KEYCODE::UI_KEY_PARENLEFT,
            UI_KEYCODE::UI_KEY_PARENRIGHT}},
        {{COL_3_X, ROW_2_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_COMMA, UI_KEYCODE::UI_KEY_COLON,
            UI_KEYCODE::UI_KEY_SEMICOLON}},
        {{COL_4_X, ROW_2_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_LESS, UI_KEYCODE::UI_KEY_GREATER,
            UI_KEYCODE::UI_KEY_QUESTION}},
        {{COL_2_X, ROW_3_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_AT, UI_KEYCODE::UI_KEY_BRACKETLEFT,
            UI_KEYCODE::UI_KEY_BACKSLASH, UI_KEYCODE::UI_KEY_BRACKETRIGHT}},
        {{COL_3_X, ROW_3_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_CARET, UI_KEYCODE::UI_KEY_UNDERSCORE,
            UI_KEYCODE::UI_KEY_BACKQUOTE}},
        {{COL_4_X, ROW_3_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_BRACELEFT, UI_KEYCODE::UI_KEY_BAR,
            UI_KEYCODE::UI_KEY_BRACERIGHT, UI_KEYCODE::UI_KEY_TILDE}},
        //  FUNCTION_KEY
        {{COL_1_X, ROW_1_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_DEL}},
        {{COL_5_X, ROW_1_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_ENTER}},
    };
    for (auto key : keySet) {
        SetupKeyButton(key);
    }
}

void UIPinyinSoftKeyboard::CreateKeyButtons()
{
    std::vector<SoftKey> keySet {
        {{COL_2_X, ROW_1_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_EN,
                                                     UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_CAP,
                                                     UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_NUMBER,
                                                     UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_SYMBOL}},
        {{COL_3_X, ROW_1_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_a, UI_KEYCODE::UI_KEY_b, UI_KEYCODE::UI_KEY_c}},
        {{COL_4_X, ROW_1_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_d, UI_KEYCODE::UI_KEY_e, UI_KEYCODE::UI_KEY_f}},
        {{COL_2_X, ROW_2_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_g, UI_KEYCODE::UI_KEY_h, UI_KEYCODE::UI_KEY_i}},
        {{COL_3_X, ROW_2_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_j, UI_KEYCODE::UI_KEY_k, UI_KEYCODE::UI_KEY_l}},
        {{COL_4_X, ROW_2_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_m, UI_KEYCODE::UI_KEY_n, UI_KEYCODE::UI_KEY_o}},
        {{COL_2_X, ROW_3_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_p, UI_KEYCODE::UI_KEY_q, UI_KEYCODE::UI_KEY_r,
            UI_KEYCODE::UI_KEY_s}},
        {{COL_3_X, ROW_3_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_t, UI_KEYCODE::UI_KEY_u, UI_KEYCODE::UI_KEY_v}},
        {{COL_4_X, ROW_3_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_w, UI_KEYCODE::UI_KEY_x, UI_KEYCODE::UI_KEY_y,
            UI_KEYCODE::UI_KEY_z}},
        //  FUNCTION_KEY
        {{COL_1_X, ROW_1_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_DEL}},
        {{COL_5_X, ROW_1_Y, KEY_WIDTH, KEY_HEIGHT}, {UI_KEYCODE::UI_KEY_ENTER}},
    };
    for (auto key : keySet) {
        SetupKeyButton(key);
    }
}

UIPinyinSoftKeyboard* UIPinyinSoftKeyboard::GetInstance()
{
    static UIPinyinSoftKeyboard pinyinSoftkeyboard;
    return &pinyinSoftkeyboard;
}
} // namespace OHOS
#endif // ENABLE_SOFT_KEYBOARD
