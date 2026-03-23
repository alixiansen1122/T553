/*
 * Copyright (c) 2020-2023 Huawei Device Co., Ltd.
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

#ifndef PLAYER_SOURCE_H
#define PLAYER_SOURCE_H

#include <memory>
#include <vector>
#include "player_demux.h"

namespace OHOS {
namespace Media {
using namespace std;
class PlayerSource {
friend PlayerDemuxer;
public:
    PlayerSource();
    ~PlayerSource();
    int32_t Init();
    int32_t SetSource(int32_t fd, uint64_t offset) const;
    int32_t SetSource(const char *url) const;
    int32_t SetSource(const StreamSourceParam &streamSource) const;
    int32_t SetCallBack(const PlayEventCallback &callBack) const;
    int32_t Prepare(void);
    int32_t GetFileInfo(FormatFileInfo &fileInfo) const;
    int32_t SelectTrack(int32_t programId, int32_t trackId) const;
    int32_t UnselectTrack(int32_t programId, int32_t trackId) const;
    int32_t GetSelectedTrack(int32_t &programId, int32_t trackId[], int32_t &nums) const;
    int32_t Start();
    int32_t ReadFrame(FormatFrame &frame) const;
    int32_t FreeFrame(FormatFrame &frame) const;
    int32_t Seek(int32_t streamIndex, int64_t timeStampUs, FormatSeekMode mode) const;
    int32_t Stop() const;
    int32_t SetParam(int32_t trackId, const ParameterItem *metaData, int32_t metaDataCnt) const;
    int32_t GetParam(int32_t trackId, ParameterItem &metaData) const;
private:
    bool inited_;
    bool prepared_;
    bool started_;
    std::shared_ptr<PlayerDemuxer> demuxer_;
};
}  // namespace Media
}  // namespace OHOS

#endif  // PLAYER_SOURCE_H
