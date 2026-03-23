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

#include "source.h"

namespace OHOS {
namespace Media {
namespace {
    constexpr int32_t INVALID_FILE_DESCRIPTOR = -1;
    constexpr int32_t FILE_OFFSET_ZERO = 0;
    constexpr uint64_t MAX_FD_LENGTH = 0x7ffffffffffffffL;
}

Source::Source(int32_t fd, uint64_t offset)
    : fd_(fd),
      offset_(offset),
      length_(MAX_FD_LENGTH),
      sourceType_(SourceType::SOURCE_TYPE_FD)
{}

Source::Source(const std::string &uri)
    : fd_(INVALID_FILE_DESCRIPTOR),
      offset_(FILE_OFFSET_ZERO),
      length_(MAX_FD_LENGTH),
      uri_(uri),
      sourceType_(SourceType::SOURCE_TYPE_URI)
{}

Source::Source(int32_t fd, uint64_t offset, uint64_t len)
    : fd_(fd),
      offset_(offset),
      length_(len),
      sourceType_(SourceType::SOURCE_TYPE_FD)
{}

Source::Source(const std::string &uri, const std::map<std::string, std::string> &header)
    : fd_(INVALID_FILE_DESCRIPTOR),
      offset_(FILE_OFFSET_ZERO),
      length_(MAX_FD_LENGTH),
      uri_(uri),
      sourceType_(SourceType::SOURCE_TYPE_URI),
      header_(header)
{}

Source::Source(const std::shared_ptr<StreamSource> &stream, const Format &formats)
    : fd_(INVALID_FILE_DESCRIPTOR),
      offset_(FILE_OFFSET_ZERO),
      length_(MAX_FD_LENGTH),
      sourceType_(SourceType::SOURCE_TYPE_STREAM),
      stream_(stream)
{
    format_.CopyFrom(formats);
}

int32_t Source::GetSourceFd() const
{
    return fd_;
}

SourceType Source::GetSourceType() const
{
    return sourceType_;
}

uint64_t Source::GetSourceFdOffset() const
{
    return offset_;
}

uint64_t Source::GetSourceFdLength() const
{
    return length_;
}

const std::string &Source::GetSourceUri() const
{
    return uri_;
}

const std::map<std::string, std::string> &Source::GetSourceHeader() const
{
    return header_;
}

const std::shared_ptr<StreamSource> &Source::GetSourceStream() const
{
    return stream_;
}
const Format &Source::GetSourceStreamFormat() const
{
    return format_;
}
}  // namespace Media
}  // namespace OHOS
