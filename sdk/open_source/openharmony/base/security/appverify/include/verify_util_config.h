/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef VERIFY_UTIL_CONFIG_H
#define VERIFY_UTIL_CONFIG_H

#ifndef LINUX_X64_PC
#define LINUX_X64_PC 0
#endif

#ifndef HI3518EV300
#define HI3518EV300 1
#endif

#ifndef HI3516DV300
#define HI3516DV300 2
#endif

#ifndef HI3861EV100
#define HI3861EV100 3
#endif

#ifndef LITE_WEARABLE
#define LITE_WEARABLE 4
#endif

#define RUNNING_BOARD LITE_WEARABLE

#ifndef RUNNING_BOARD
#define RUNNING_BOARD LINUX_X64_PC
#endif

#endif
