/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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
#ifndef LITE_BOOTSTRAP_SERVICE_H
#define LITE_BOOTSTRAP_SERVICE_H
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#define MODULE_NAME(step, name, pri) ".zinitcall." #step "." #name #pri ".init"

#define MODULE_CALL(step, name, pri)                                      \
    do {                                                                  \
        InitCall *initcall = (InitCall *)(MODULE_BEGIN(step, name, pri)); \
        InitCall *initend = (InitCall *)(MODULE_END(step, name, pri));    \
        for (; initcall < initend; initcall++) {                          \
            (*initcall)();                                                \
        }                                                                 \
    } while (0)

#define MODULE_CALL_EXT(name, step)                                      \
    do {                                                                 \
        InitCall *initcall = (InitCall *)(MODULE_BEGIN_EXT(name, step)); \
        InitCall *initend = (InitCall *)(MODULE_END_EXT(name, step));    \
        for (; initcall < initend; initcall++) {                         \
            (*initcall)();                                               \
        }                                                                \
    } while (0)

#if (defined(__GNUC__) || defined(__clang__))
#define MODULE_BEGIN(step, name, pri)                              \
    ({  extern InitCall __zinitcall_##step##_##name##_start;       \
        InitCall *initCall = &__zinitcall_##step##_##name##_start; \
        (initCall);                                                \
    })

#define MODULE_END(step, name, pri)                              \
    ({  extern InitCall __zinitcall_##step##_##name##_end;       \
        InitCall *initCall = &__zinitcall_##step##_##name##_end; \
        (initCall);                                              \
    })

#define MODULE_BEGIN_EXT(name, step)                          \
    ({        extern InitCall __zinitcall_##name##_start;     \
        InitCall *initCall = &__zinitcall_##name##_start;     \
        (initCall);                                           \
    })

#define MODULE_END_EXT(name, step)                          \
    ({        extern InitCall __zinitcall_##name##_end;     \
        InitCall *initCall = &__zinitcall_##name##_end;     \
        (initCall);                                         \
    })

#define INIT_TEST_CALL()           \
    do {                           \
        MODULE_CALL_EXT(test, 0);  \
    } while (0)

#define INIT_MODULE_CALL(step, name) MODULE_CALL(step, name, 0)

#elif (defined(__ICCARM__))
#define MODULE_BEGIN(step, name, pri) __section_begin(MODULE_NAME(step, name, pri))
#define MODULE_END(step, name, pri) __section_end(MODULE_NAME(step, name, pri))

#pragma section = MODULE_NAME(sysex, service, 0)
#pragma section = MODULE_NAME(sysex, service, 1)
#pragma section = MODULE_NAME(sysex, service, 2)
#pragma section = MODULE_NAME(sysex, service, 3)
#pragma section = MODULE_NAME(sysex, service, 4)
#pragma section = MODULE_NAME(sysex, feature, 0)
#pragma section = MODULE_NAME(sysex, feature, 1)
#pragma section = MODULE_NAME(sysex, feature, 2)
#pragma section = MODULE_NAME(sysex, feature, 3)
#pragma section = MODULE_NAME(sysex, feature, 4)
#pragma section = MODULE_NAME(app, service, 0)
#pragma section = MODULE_NAME(app, service, 1)
#pragma section = MODULE_NAME(app, service, 2)
#pragma section = MODULE_NAME(app, service, 3)
#pragma section = MODULE_NAME(app, service, 4)
#pragma section = MODULE_NAME(app, feature, 0)
#pragma section = MODULE_NAME(app, feature, 1)
#pragma section = MODULE_NAME(app, feature, 2)
#pragma section = MODULE_NAME(app, feature, 3)
#pragma section = MODULE_NAME(app, feature, 4)

#define INIT_MODULE_CALL(step, name)   \
    do {                               \
        MODULE_CALL(step, name, 0);    \
        MODULE_CALL(step, name, 1);    \
        MODULE_CALL(step, name, 2);    \
        MODULE_CALL(step, name, 3);    \
        MODULE_CALL(step, name, 4);    \
    } while (0)
#else
#error Not support current compiler!
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif // LITE_BOOTSTRAP_SERVICE_H
