/*
 * Copyright (c) CompanyNameMagicTag 2024. All rights reserved.
 * Description: NativeRegister
 * Author:
 * Create: 2024-03
 */

#ifndef NATIVEREGISTER_TYPES_H
#define NATIVEREGISTER_TYPES_H
namespace OHOS {
enum class NativeRegisterType : uint16_t {
    VIEW_MAX_INTER_ARRY_APP = 0x7FFF,
    VIEW_MAX_INTER_APP = 0xFFFD, // 在此之前添加内部静态注册native应用ID
    VIEW_EXTERN_APP = 0xFFFE, // 三方应用, 在此之前添加内部静态注册native应用ID
    VIEW_INVALIDE_APP = 0xFFFF,
};
}
#endif
