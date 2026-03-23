/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 */

#ifndef DEFER_UTILS_H
#define DEFER_UTILS_H

#include <cstdint>
#include "securec.h"
#include "gfx_utils/image_info.h"
#include "gfx_utils/graphic_math.h"

namespace OHOS {
#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

#ifndef defer
struct defer_dummy {};
template <class F> struct deferrer { F f; ~deferrer() { f(); } };
template <class F> deferrer<F> operator*(defer_dummy, F f) { return {f}; }
#define DEFER_(LINE) zz_defer##LINE
#define DEFER(LINE) DEFER_(LINE)
#define defer auto DEFER(__LINE__) = defer_dummy{} *[&]()
#endif // defer
} // OHOS
#endif // DEFER_UTILS_H