/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#include "liteplayer_comm.h"
#include <sys/time.h>
#include <unistd.h>
#ifdef SUPPORT_DL
#include <dlfcn.h>
#endif
#include "liteplayer.h"

using OHOS::Media::PlayerControl;

static const long long AV_NS_2_MS_SCALE = 1000000;
static const long long AV_SEC_2_MS_SCALE = 1000;

int32_t PlayerControlOnEvent(void* priv, EventCbType event, int32_t ext1, int32_t ext2)
{
    (void)ext1;
    (void)ext2;
    PlayerControl *player = static_cast<PlayerControl *>(priv);
    if (player == nullptr) {
        return -1;
    }
    return player->OnPlayControlEvent(priv, event);
}

uint64_t PlayerControlGetCurRelativeTime()
{
    struct timespec ts = { 0, 0 };
    (void)clock_gettime(CLOCK_MONOTONIC, &ts);
    uint64_t curTime = (static_cast<uint64_t>(ts.tv_sec)) * AV_SEC_2_MS_SCALE +
        (static_cast<uint64_t>(ts.tv_nsec)) / AV_NS_2_MS_SCALE;
    return curTime;
}

