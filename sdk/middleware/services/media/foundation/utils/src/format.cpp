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

#include "format.h"

#include <cstdio>

#include "media_log.h"
#include "securec.h"

namespace OHOS {
namespace Media {
const char *AUDIO_CODEC_MIME = "audio_mime";
const char *VIDEO_CODEC_MIME = "video_mime";
const char *SAMPLERATE_MIME = "samplerate";
const char *CHANNEL_MIME = "channel";
const char *MIME_AUDIO_AAC = "audio/aac-adts";
const char *MIME_AUDIO_MP3 = "audio/mp3";
const char *MIME_AUDIO_PCM = "audio/pcm";
const char *MIME_AUDIO_RAW = "audio/raw";
const char *MIME_VIDEO_H264 = "video/h264";
const char *MIME_VIDEO_JPEG = "video/jpeg";
const char *MIME_VIDEO_FREAMRATE = "freamrate";
const char *MIME_VIDEO_WIDTH = "width";
const char *MIME_VIDEO_HEIGHT = "height";
const char *PAUSE_AFTER_PLAY = "pause_after_play";
const char *FORMAT_KEY = "key";
const char *FORMAT_GET_ALBUM_INFO = "albuminfo";
const char *FORMAT_KEY_ALBUM = "album";
const char *FORMAT_KEY_TITLE = "title";
const char *FORMAT_KEY_ARTIST = "artist";
const char *FORMAT_KEY_COMPOSER = "composer";
const char *FORMAT_KEY_AUTHOR = "author";

// For Class FormatData
FormatData::FormatData() : type_(FORMAT_TYPE_NONE), val_({ 0 }) {}

FormatData::FormatData(FormatDataType type) : type_(type), val_({ 0 }) {}

FormatData::~FormatData()
{
    if (type_ == FORMAT_TYPE_STRING) {
        if (val_.stringVal != nullptr) {
            delete val_.stringVal;
            val_.stringVal = nullptr;
        }
    }
    if (type_ == FORMAT_TYPE_CHARPTR) {
        if (val_.charVal.data != nullptr) {
            delete[] val_.charVal.data;
            val_.charVal.data = nullptr;
        }
    }
}

bool FormatData::SetValue(int32_t val)
{
    if (type_ != FORMAT_TYPE_INT32) {
        MEDIA_ERR_LOG("FormatData set int32 value error, current type is %u", type_);
        return false;
    }
    val_.int32Val = val;
    return true;
}

bool FormatData::SetValue(int64_t val)
{
    if (type_ != FORMAT_TYPE_INT64) {
        MEDIA_ERR_LOG("FormatData set int64 value error, current type is %u", type_);
        return false;
    }
    val_.int64Val = val;
    return true;
}

bool FormatData::SetValue(float val)
{
    if (type_ != FORMAT_TYPE_FLOAT) {
        MEDIA_ERR_LOG("FormatData set float value error, current type is %u", type_);
        return false;
    }
    val_.floatVal = val;
    return true;
}

bool FormatData::SetValue(double val)
{
    if (type_ != FORMAT_TYPE_DOUBLE) {
        MEDIA_ERR_LOG("FormatData set double value error, current type is %u", type_);
        return false;
    }
    val_.doubleVal = val;
    return true;
}

bool FormatData::SetValue(const std::string &val)
{
    if (type_ != FORMAT_TYPE_STRING) {
        MEDIA_ERR_LOG("FormatData set string value error, current type is %u", type_);
        return false;
    }
    val_.stringVal = new (std::nothrow) std::string();
    if (val_.stringVal == nullptr) {
        type_ = FORMAT_TYPE_NONE;
        MEDIA_ERR_LOG("FormatData set string value error, new string failed");
        return false;
    }
    *(val_.stringVal) = val;
    return true;
}

bool FormatData::SetValue(const CharValue &val)
{
    if (type_ != FORMAT_TYPE_CHARPTR) {
        MEDIA_ERR_LOG("FormatData set string value error, current type is %u", type_);
        return false;
    }
    val_.charVal.data = new (std::nothrow) char[val.len];
    if (val_.charVal.data == nullptr) {
        type_ = FORMAT_TYPE_NONE;
        MEDIA_ERR_LOG("FormatData set char value error, new char failed");
        return false;
    }
    if (memcpy_s(val_.charVal.data, val.len, val.data, val.len) != EOK) {
        MEDIA_ERR_LOG("memcpy_s failed");
        return false;
    }
    val_.charVal.len = val.len;
    return true;
}

bool FormatData::GetInt32Value(int32_t &val) const
{
    if (type_ != FORMAT_TYPE_INT32) {
        MEDIA_ERR_LOG("FormatData get int32 value error, current type is %u", type_);
        return false;
    }
    val = val_.int32Val;
    return true;
}

bool FormatData::GetInt64Value(int64_t &val) const
{
    if (type_ != FORMAT_TYPE_INT64) {
        MEDIA_ERR_LOG("FormatData get int64 value error, current type is %u", type_);
        return false;
    }
    val = val_.int64Val;
    return true;
}

bool FormatData::GetFloatValue(float &val) const
{
    if (type_ != FORMAT_TYPE_FLOAT) {
        MEDIA_ERR_LOG("FormatData get float value error, current type is %u", type_);
        return false;
    }
    val = val_.floatVal;
    return true;
}

bool FormatData::GetDoubleValue(double &val) const
{
    if (type_ != FORMAT_TYPE_DOUBLE) {
        MEDIA_ERR_LOG("FormatData get double value error, current type is %u", type_);
        return false;
    }
    val = val_.doubleVal;
    return true;
}

bool FormatData::GetStringValue(std::string &val) const
{
    if (type_ != FORMAT_TYPE_STRING) {
        MEDIA_ERR_LOG("FormatData get string value error, current type is %u", type_);
        return false;
    }
    if (val_.stringVal == nullptr) {
        MEDIA_ERR_LOG("FormatData get string value error, stringVal is null");
        return false;
    }
    val = *(val_.stringVal);
    return true;
}

bool FormatData::GetCharValue(CharValue &val) const
{
    if (type_ != FORMAT_TYPE_CHARPTR) {
        MEDIA_ERR_LOG("FormatData get char value error, current type is %u", type_);
        return false;
    }
    if (val_.charVal.data == nullptr) {
        MEDIA_ERR_LOG("FormatData get char value error, charVal is null");
        return false;
    }
    val.data = val_.charVal.data;
    val.len = val_.charVal.len;
    return true;
}

// For Class Format
Format::Format()
{}

Format::~Format()
{
    for (auto &iter : formatMap_) {
        if (iter.second != nullptr) {
            delete iter.second;
        }
    }
    formatMap_.clear();
}

bool Format::PutIntValue(const std::string &key, int32_t value)
{
    return SetFormatCommon(key, value, FORMAT_TYPE_INT32);
}

bool Format::PutLongValue(const std::string &key, int64_t value)
{
    return SetFormatCommon(key, value, FORMAT_TYPE_INT64);
}

bool Format::PutFloatValue(const std::string &key, float value)
{
    return SetFormatCommon(key, value, FORMAT_TYPE_FLOAT);
}

bool Format::PutDoubleValue(const std::string &key, double value)
{
    return SetFormatCommon(key, value, FORMAT_TYPE_DOUBLE);
}

bool Format::PutStringValue(const std::string &key, const std::string &value)
{
    return SetFormatCommon(key, value, FORMAT_TYPE_STRING);
}

bool Format::PutCharValue(const std::string &key, const CharValue &value)
{
    return SetFormatCommon(key, value, FORMAT_TYPE_CHARPTR);
}

template<typename T>
bool Format::SetFormatCommon(const std::string &key, const T &value, FormatDataType type)
{
    auto iter = formatMap_.find(key);
    if (iter != formatMap_.end()) {
        delete iter->second;
        iter->second = nullptr;
        formatMap_.erase(iter);
    }
    FormatData *data = new (std::nothrow) FormatData(type);
    if (data == nullptr) {
        MEDIA_ERR_LOG("Format::SetFormatCommon new FormatData failed");
        return false;
    }
    if (!data->SetValue(value)) {
        MEDIA_ERR_LOG("Format::SetFormatCommon failed. Key: %s", key.c_str());
        delete data;
        return false;
    }
    formatMap_[key] = data;
    return true;
}

bool Format::GetStringValue(const std::string &key, std::string &value) const
{
    auto iter = formatMap_.find(key);
    if (iter == formatMap_.end() || iter->second == nullptr) {
        MEDIA_DEBUG_LOG("Format::GetFormat failed. Key: %s", key.c_str());
        return false;
    }

    return (*iter->second).GetStringValue(value);
}

bool Format::GetCharValue(const std::string &key, CharValue &value) const
{
    auto iter = formatMap_.find(key);
    if (iter == formatMap_.end() || iter->second == nullptr) {
        MEDIA_DEBUG_LOG("Format::GetFormat failed. Key: %s", key.c_str());
        return false;
    }

    return (*iter->second).GetCharValue(value);
}

bool Format::GetIntValue(const std::string &key, int32_t &value) const
{
    auto iter = formatMap_.find(key);
    if ((iter == formatMap_.end()) || (iter->second == nullptr)) {
        MEDIA_DEBUG_LOG("Format::GetFormat failed. Key: %s", key.c_str());
        return false;
    }

    return (*iter->second).GetInt32Value(value);
}

bool Format::GetLongValue(const std::string &key, int64_t &value) const
{
    auto iter = formatMap_.find(key);
    if ((iter == formatMap_.end()) || (iter->second == nullptr)) {
        MEDIA_DEBUG_LOG("Format::GetFormat failed. Key: %s", key.c_str());
        return false;
    }

    return (*iter->second).GetInt64Value(value);
}

bool Format::GetFloatValue(const std::string &key, float &value) const
{
    auto iter = formatMap_.find(key);
    if ((iter == formatMap_.end()) || (iter->second == nullptr)) {
        MEDIA_DEBUG_LOG("Format::GetFormat failed. Key: %s", key.c_str());
        return false;
    }

    return (*iter->second).GetFloatValue(value);
}

bool Format::GetDoubleValue(const std::string &key, double &value) const
{
    auto iter = formatMap_.find(key);
    if ((iter == formatMap_.end()) || (iter->second == nullptr)) {
        MEDIA_DEBUG_LOG("Format::GetFormat failed. Key: %s", key.c_str());
        return false;
    }

    return (*iter->second).GetDoubleValue(value);
}

const std::map<std::string, FormatData *> &Format::GetFormatMap() const
{
    return formatMap_;
}

static void SetInt32Format(Format *format, std::string key, FormatData *formatData)
{
    int32_t val;
    if (formatData->GetInt32Value(val)) {
        format->PutIntValue(key, val);
    }
}

static void SetInt64Format(Format *format, std::string key, FormatData *formatData)
{
    int64_t val;
    if (formatData->GetInt64Value(val)) {
        format->PutLongValue(key, val);
    }
}

static void SetFloatFormat(Format *format, std::string key, FormatData *formatData)
{
    float val;
    if (formatData->GetFloatValue(val)) {
        format->PutFloatValue(key, val);
    }
}

static void SetDoubleFormat(Format *format, std::string key, FormatData *formatData)
{
    double val;
    if (formatData->GetDoubleValue(val)) {
        format->PutDoubleValue(key, val);
    }
}

static void SetStringFormat(Format *format, std::string key, FormatData *formatData)
{
    std::string val;
    if (formatData->GetStringValue(val)) {
        format->PutStringValue(key, val);
    }
}

static void SetCharValueFormat(Format *format, std::string key, FormatData *formatData)
{
    CharValue val;
    if (formatData->GetCharValue(val)) {
        format->PutCharValue(key, val);
    }
}

bool Format::CopyFrom(const Format &format)
{
    for (auto &iter : format.GetFormatMap()) {
        std::string key = iter.first;
        FormatData *formatData = iter.second;
        if (formatData == nullptr) {
            MEDIA_ERR_LOG("CopyFrom FormatData is null, key is %s", key.c_str());
            continue;
        }
        switch (formatData->GetType()) {
            case FORMAT_TYPE_INT32: {
                SetInt32Format(this, key, formatData);
                break;
            }
            case FORMAT_TYPE_INT64: {
                SetInt64Format(this, key, formatData);
                break;
            }
            case FORMAT_TYPE_FLOAT: {
                SetFloatFormat(this, key, formatData);
                break;
            }
            case FORMAT_TYPE_DOUBLE: {
                SetDoubleFormat(this, key, formatData);
                break;
            }
            case FORMAT_TYPE_STRING: {
                SetStringFormat(this, key, formatData);
                break;
            }
            case FORMAT_TYPE_CHARPTR: {
                SetCharValueFormat(this, key, formatData);
                break;
            }
            default: {
                MEDIA_ERR_LOG("CopyFrom unknown type, type is %d", formatData->GetType());
                break;
            }
        }
    }
    return true;
}
}  // namespace Media
}  // namespace OHOS
