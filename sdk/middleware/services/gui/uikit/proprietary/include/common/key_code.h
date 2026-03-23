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

#ifndef KEYCODE_H
#define KEYCODE_H

namespace OHOS {
enum class ZLITE_KEYTYPE_E {
    ZLITE_KEYTYPE_UNICODE = 0x0,
    ZLITE_KEYTYPE_FUNCTION = 0X1000,
};
#define ZLITE_KEY_CAT(type, index) (static_cast<int>(ZLITE_KEYTYPE_E::ZLITE_KEYTYPE_##type) | (index))
enum class ZliteKeyCode {
    ZLITE_KEY_1 = ZLITE_KEY_CAT(UNICODE, 0x30),
    ZLITE_KEY_2 = ZLITE_KEY_CAT(UNICODE, 0x31),
    ZLITE_KEY_3 = ZLITE_KEY_CAT(UNICODE, 0x32),
    ZLITE_KEY_4 = ZLITE_KEY_CAT(UNICODE, 0x33),
    ZLITE_KEY_5 = ZLITE_KEY_CAT(UNICODE, 0x34),
    ZLITE_KEY_6 = ZLITE_KEY_CAT(UNICODE, 0x35),
    ZLITE_KEY_7 = ZLITE_KEY_CAT(UNICODE, 0x36),
    ZLITE_KEY_8 = ZLITE_KEY_CAT(UNICODE, 0x37),
    ZLITE_KEY_9 = ZLITE_KEY_CAT(UNICODE, 0x38),
    ZLITE_KEY_0 = ZLITE_KEY_CAT(UNICODE, 0x39),

    /* Function key */
    ZLITE_KEY_ENTER = ZLITE_KEY_CAT(FUNCTION, 0x00),
    ZLITE_KEY_DEL = ZLITE_KEY_CAT(FUNCTION, 0x02),
    ZLITE_KEY_BACK = ZLITE_KEY_CAT(FUNCTION, 0x03),

    ZLITE_KEY_UP = ZLITE_KEY_CAT(FUNCTION, 0x06),
    ZLITE_KEY_DOWN = ZLITE_KEY_CAT(FUNCTION, 0x07),
    ZLITE_KEY_LEFT = ZLITE_KEY_CAT(FUNCTION, 0x08),
    ZLITE_KEY_RIGHT = ZLITE_KEY_CAT(FUNCTION, 0x09),
    ZLITE_KEY_PGUP = ZLITE_KEY_CAT(FUNCTION, 0x0A),
    ZLITE_KEY_PGDOWN = ZLITE_KEY_CAT(FUNCTION, 0x0B),
    ZLITE_KEY_PREV = ZLITE_KEY_CAT(FUNCTION, 0x0E),
    ZLITE_KEY_NEXT = ZLITE_KEY_CAT(FUNCTION, 0x10),
    ZLITE_KEY_MENU = ZLITE_KEY_CAT(FUNCTION, 0x15),
    ZLITE_KEY_POWER = ZLITE_KEY_CAT(FUNCTION, 0X29),
    ZLITE_KEY_MUTE = ZLITE_KEY_CAT(FUNCTION, 0x2B),
    ZLITE_KEY_AUDIO = ZLITE_KEY_CAT(FUNCTION, 0x2A),
    ZLITE_KEY_HOME = ZLITE_KEY_CAT(FUNCTION, 0x3F),
    ZLITE_KEY_FUNC = ZLITE_KEY_CAT(FUNCTION, 0x100),
};
}
#endif