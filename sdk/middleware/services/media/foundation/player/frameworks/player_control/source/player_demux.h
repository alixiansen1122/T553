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

#ifndef PLAYER_DEMUX_H
#define PLAYER_DEMUX_H

#include <string>
#include <memory>
#include <vector>

#include "hi_demuxer.h"
#include "hi_liteplayer.h"
#include "player_define.h"

namespace OHOS {
namespace Media {
using namespace std;
class PlayerDemuxer {
public:
    PlayerDemuxer();
    ~PlayerDemuxer();
    int32_t Init(void);
    int32_t SetSource(int32_t fd, uint64_t offset);
    int32_t SetSource(const char *url);
    int32_t SetSource(const StreamSourceParam &streamSource);
    int32_t SetCallBack(const PlayEventCallback &callBack);
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

    static int32_t ErrorEventPro(void *handle, int32_t errorType, int32_t errorCode);
    static int32_t InfoEventPro(void *handle, int32_t type, int32_t extra);
private:
    int32_t GetFormatDemuxer(void);
    void GetFormatFileInfoFromProgramInfo(int32_t trackId[], int32_t trackNum, FormatFileInfo &fileInfo,
        ProgramInfo *programInfo) const;
    void GetStreamResolutionFromFileInfo(FormatFileInfo &fileInfo, FileInfo info) const;
    int32_t GetStreamSourceFileInfo(FormatFileInfo &fileInfo) const;
    void GetAlbumInfoToFileInfo(FormatFileInfo &fileInfo) const;
    bool inited_;
    bool prepared_;
    bool started_;
    void *demuxer_;
    SourceType sourceType_;
    int32_t fd_;
    uint64_t offset_;
    std::string filePath_;
    StreamSourceParam streamSource_;
    PlayEventCallback callBack_;
    FormatCallback formatListener_;
};
}  // namespace Media
}  // namespace OHOS

#endif  // PLAYER_DEMUX_H
