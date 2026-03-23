/*
 * Copyright (c) CompanyNameMagicTag.
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

#ifndef NATIVEABILITYFWK_NATIVE_ABILITY_H
#define NATIVEABILITYFWK_NATIVE_ABILITY_H

#include "Ability.h"

#include "slite_ability.h"

namespace OHOS {
/*
 * Remove parameters, variables, or expressions compiling warning.
 */
#undef UNUSED // in case others define the same macro
#define UNUSED(a) (void)(a)

#ifdef JS_ENABLE
#define OVERRIDE override
#else
#define OVERRIDE
#endif


class NativeAbility : public Ability
#ifdef JS_ENABLE
, public SliteAbility
#endif
{
public:
    static NativeAbility &GetInstance(void);
#ifdef JS_ENABLE
    void InstallAbility(void);
#endif
    // SliteAbility, AMS控制的应用生命周期
    void OnStart(const Want &want) OVERRIDE;
    void OnActive(const Want &want) OVERRIDE;
    void OnInactive() OVERRIDE;
    void OnBackground(void) OVERRIDE;

    void OnStop() OVERRIDE;
    int GetNativeAbilityState() const;

private:
    // targetId consists of slice id and page id
    void ResumeAbility(uint32_t targetId);
    void PauseAbility();
    void StopAbility();
    NativeAbility() {}
    void ResumeSliceWithJS();
    ~NativeAbility() override {}
    int nativeAbilityState_ = 0;
};
}
#endif // NATIVEABILITYFWK_NATIVE_ABILITY_H
