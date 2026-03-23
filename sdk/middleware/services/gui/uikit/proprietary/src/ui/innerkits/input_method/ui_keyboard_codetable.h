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

#ifndef GRAPHIC_LITE_UI_KEYBOARD_CODETABEL_H
#define GRAPHIC_LITE_UI_KEYBOARD_CODETABEL_H

#include <string>
#include "gfx_utils/vector.h"

#if ENABLE_SOFT_KEYBOARD
namespace OHOS {
constexpr uint8_t MAX_KEY_VALUE_NUM = 4;

enum class UI_KEYCODE : uint8_t {
    /* The follow is digit key */
    UI_KEY_0 = 0x30,
    UI_KEY_1 = 0x31,
    UI_KEY_2 = 0x32,
    UI_KEY_3 = 0x33,
    UI_KEY_4 = 0x34,
    UI_KEY_5 = 0x35,
    UI_KEY_6 = 0x36,
    UI_KEY_7 = 0x37,
    UI_KEY_8 = 0x38,
    UI_KEY_9 = 0x39,

    /* The follow is Capital key */
    UI_KEY_A = 0x41,
    UI_KEY_B = 0x42,
    UI_KEY_C = 0x43,
    UI_KEY_D = 0x44,
    UI_KEY_E = 0x45,
    UI_KEY_F = 0x46,
    UI_KEY_G = 0x47,
    UI_KEY_H = 0x48,
    UI_KEY_I = 0x49,
    UI_KEY_J = 0x4A,
    UI_KEY_K = 0x4B,
    UI_KEY_L = 0x4C,
    UI_KEY_M = 0x4D,
    UI_KEY_N = 0x4E,
    UI_KEY_O = 0x4F,
    UI_KEY_P = 0x50,
    UI_KEY_Q = 0x51,
    UI_KEY_R = 0x52,
    UI_KEY_S = 0x53,
    UI_KEY_T = 0x54,
    UI_KEY_U = 0x55,
    UI_KEY_V = 0x56,
    UI_KEY_W = 0x57,
    UI_KEY_X = 0x58,
    UI_KEY_Y = 0x59,
    UI_KEY_Z = 0x5A,

    /* The follow is English key */
    UI_KEY_a = 0x61,
    UI_KEY_b = 0x62,
    UI_KEY_c = 0x63,
    UI_KEY_d = 0x64,
    UI_KEY_e = 0x65,
    UI_KEY_f = 0x66,
    UI_KEY_g = 0x67,
    UI_KEY_h = 0x68,
    UI_KEY_i = 0x69,
    UI_KEY_j = 0x6A,
    UI_KEY_k = 0x6B,
    UI_KEY_l = 0x6C,
    UI_KEY_m = 0x6D,
    UI_KEY_n = 0x6E,
    UI_KEY_o = 0x6F,
    UI_KEY_p = 0x70,
    UI_KEY_q = 0x71,
    UI_KEY_r = 0x72,
    UI_KEY_s = 0x73,
    UI_KEY_t = 0x74,
    UI_KEY_u = 0x75,
    UI_KEY_v = 0x76,
    UI_KEY_w = 0x77,
    UI_KEY_x = 0x78,
    UI_KEY_y = 0x79,
    UI_KEY_z = 0x7A,

    /* The follow is punctuation key */
    UI_KEY_SPACE = 0x20,              /* ' ' */
    UI_KEY_EXCLAM = 0x21,             /* '!' */
    UI_KEY_DOUBLEQUOTE = 0x22,        /* '"' */
    UI_KEY_NUMBERSIGN = 0x23,         /* '#' */
    UI_KEY_DOLLAR = 0x24,             /* '$' */
    UI_KEY_PERCENT = 0x25,            /* '%' */
    UI_KEY_AMPERSAND = 0x26,          /* '&' */
    UI_KEY_QUOTE = 0x27,              /* ''' */
    UI_KEY_PARENLEFT = 0x28,          /* '(' */
    UI_KEY_PARENRIGHT = 0x29,         /* ')' */
    UI_KEY_ASTERISK = 0x2A,           /* '*' */
    UI_KEY_PLUS = 0x2B,               /* '+' */
    UI_KEY_COMMA = 0x2C,              /* ',' */
    UI_KEY_MINUS = 0x2D,              /* '-' */
    UI_KEY_DOT = 0x2E,                /* '.' */
    UI_KEY_SLASH = 0x2F,              /* '/' */
    UI_KEY_COLON = 0x3A,              /* ':' */
    UI_KEY_SEMICOLON = 0x3B,          /* ';' */
    UI_KEY_LESS = 0x3C,               /* '<' */
    UI_KEY_EQUAL = 0x3D,              /* '=' */
    UI_KEY_GREATER = 0x3E,            /* '>' */
    UI_KEY_QUESTION = 0x3F,           /* '?' */
    UI_KEY_AT = 0x40,                 /* '@' */
    UI_KEY_BRACKETLEFT = 0x5B,        /* '[' */
    UI_KEY_BACKSLASH = 0x5C,          /* '|' */
    UI_KEY_BRACKETRIGHT = 0x5D,       /* ']' */
    UI_KEY_CARET = 0x5E,              /* '^' */
    UI_KEY_UNDERSCORE = 0x5F,         /* '_' */
    UI_KEY_BACKQUOTE = 0x60,          /* '`' */
    UI_KEY_BRACELEFT = 0x7B,          /* '{' */
    UI_KEY_BAR = 0x7C,                /* '|' */
    UI_KEY_BRACERIGHT = 0x7D,         /* '}' */
    UI_KEY_TILDE = 0x7E,              /* '~' */
    UI_KEY_DELETE = 0x7F,             /* '<-' */

    /* Function Key */
    UI_KEY_ENTER    = 0xF0,
    UI_KEY_DEL      = 0xF2,
    UI_KEY_EXIT     = 0xF3,
    UI_KEY_CAPITAL  = 0xF4,
    UI_KEY_SOFT_KEYBOARD      = 0xF5,

    /* System IME Key */
    UI_KEY_SOFT_KEYBOARD_EN      = 0xF6,
    UI_KEY_SOFT_KEYBOARD_CAP     = 0xF7,
    UI_KEY_SOFT_KEYBOARD_NUMBER  = 0xF8,
    UI_KEY_SOFT_KEYBOARD_SYMBOL  = 0xF9,
    UI_KEY_SOFT_KEYBOARD_PINYIN  = 0xFA,

    UI_KEY_BUTT = 0XFF,
};

struct KeyRect {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
};

struct SoftKey {
    KeyRect rect;
    UI_KEYCODE keys[MAX_KEY_VALUE_NUM];
};
} // namespace OHOS
#endif
#endif // GRAPHIC_LITE_UI_KEYBOARD_CODETABEL_H
