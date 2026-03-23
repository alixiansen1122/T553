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

#ifndef GRAPHIC_LITE_UI_SYSTEM_IME_H
#define GRAPHIC_LITE_UI_SYSTEM_IME_H

#include "ui_input_method.h"

#if ENABLE_SOFT_KEYBOARD
namespace OHOS {
class UICircularCapSoftKeyboard : public UIAbstractSoftKeyboard {
public:
    UICircularCapSoftKeyboard() {}
    ~UICircularCapSoftKeyboard() override {}
    void CreateKeyButtons() override;
    static UICircularCapSoftKeyboard* GetInstance();
};

class UICircularEngSoftKeyboard : public UIAbstractSoftKeyboard {
public:
    UICircularEngSoftKeyboard() {}
    ~UICircularEngSoftKeyboard() override {}
    void CreateKeyButtons() override;
    static UICircularEngSoftKeyboard* GetInstance();
};

class UICircularNumberSoftKeyboard : public UIAbstractSoftKeyboard {
public:
    UICircularNumberSoftKeyboard() {}
    ~UICircularNumberSoftKeyboard() override {}
    void CreateKeyButtons() override;
    static UICircularNumberSoftKeyboard* GetInstance();
};

class UICircularSymbolSoftKeyboard : public UIAbstractSoftKeyboard {
public:
    UICircularSymbolSoftKeyboard() {}
    ~UICircularSymbolSoftKeyboard() override {}
    void CreateKeyButtons() override;
    static UICircularSymbolSoftKeyboard* GetInstance();
};

class UIEnglishSoftKeyboard : public UIAbstractSoftKeyboard {
public:
    UIEnglishSoftKeyboard() {}
    ~UIEnglishSoftKeyboard() override {}
    void CreateKeyButtons() override;
    static UIEnglishSoftKeyboard* GetInstance();
};

class UICapitalSoftKeyboard : public UIAbstractSoftKeyboard {
public:
    UICapitalSoftKeyboard() {}
    ~UICapitalSoftKeyboard() override {}
    void CreateKeyButtons() override;
    static UICapitalSoftKeyboard* GetInstance();
};

class UINumberSoftKeyboard : public UIAbstractSoftKeyboard {
public:
    UINumberSoftKeyboard() {}
    ~UINumberSoftKeyboard() override {}
    void CreateKeyButtons() override;
    static UINumberSoftKeyboard* GetInstance();
};

class UISymbolSoftKeyboard : public UIAbstractSoftKeyboard {
public:
    UISymbolSoftKeyboard() {}
    ~UISymbolSoftKeyboard() override {}
    void CreateKeyButtons() override;
    static UISymbolSoftKeyboard* GetInstance();
};

class UIPinyinSoftKeyboard : public UIAbstractSoftKeyboard {
public:
    UIPinyinSoftKeyboard() {}
    ~UIPinyinSoftKeyboard() override {}
    void CreateKeyButtons() override;
    static UIPinyinSoftKeyboard* GetInstance();
};

} // namespace OHOS
#endif // ENABLE_SOFT_KEYBOARD
#endif // GRAPHIC_LITE_UI_INPUT_METHOD_H
