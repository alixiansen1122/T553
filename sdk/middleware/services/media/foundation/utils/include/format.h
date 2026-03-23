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

/**
 * @addtogroup MultiMedia_MediaCommon
 * @{
 *
 * @brief Provides data types and media formats required for recording and playing audio and videos.
 *
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file format.h
 *
 * @brief Declares the media formats provided in the </b>Format</b> class.
 *
 *
 * @since 1.0
 * @version 1.0
 */


#ifndef FORMAT_H
#define FORMAT_H

#include <map>
#include <string>
#include <functional>

namespace OHOS {
namespace Media {
/** Indicates the key representing the codec type. */
extern const char *AUDIO_CODEC_MIME;
extern const char *VIDEO_CODEC_MIME;

/** Indicates the codec for Advanced Audio Coding (AAC) streams, which is a value of the codec type key. */
extern const char *MIME_AUDIO_AAC;
extern const char *MIME_AUDIO_MP3;
extern const char *MIME_AUDIO_PCM;
extern const char *SAMPLERATE_MIME;
extern const char *MIME_VIDEO_H264;
extern const char *MIME_VIDEO_JPEG;
extern const char *MIME_VIDEO_FREAMRATE;
extern const char *MIME_VIDEO_WIDTH;
extern const char *MIME_VIDEO_HEIGHT;

extern const char *CHANNEL_MIME;

/** Indicates the codec for RAW audios (not supported yet), which is a value of the codec type key. */
extern const char *MIME_AUDIO_RAW;
extern const char *PAUSE_AFTER_PLAY;

/** Indicates the albumInfo, which is the type key. */
extern const char *FORMAT_KEY;
extern const char *FORMAT_GET_ALBUM_INFO;
extern const char *FORMAT_KEY_ALBUM;
extern const char *FORMAT_KEY_TITLE;
extern const char *FORMAT_KEY_ARTIST;
extern const char *FORMAT_KEY_COMPOSER;
extern const char *FORMAT_KEY_AUTHOR;

/**
 * @brief Enumerates formats.
 *
 * @since 1.0
 * @version 1.0
 */
enum FormatDataType : uint32_t {
    /** None */
    FORMAT_TYPE_NONE,
    /** Int32 */
    FORMAT_TYPE_INT32,
    /** Int64 */
    FORMAT_TYPE_INT64,
    /** Float */
    FORMAT_TYPE_FLOAT,
    /** Double */
    FORMAT_TYPE_DOUBLE,
    /** String */
    FORMAT_TYPE_STRING,
    /** char ptr */
    FORMAT_TYPE_CHARPTR
};

struct CharValue {
    char *data;
    uint32_t len;
};

/**
 * @brief Represents the data format.
 *
 * @since 1.0
 * @version 1.0
 */
class FormatData {
public:
    explicit FormatData(FormatDataType type);
    FormatData();
    ~FormatData();

    /**
     * @brief Obtains the format type.
     *
     * @return Returns the format type. For details, see {@link OHOS::Media::FormatDataType}.
     * @since 1.0
     * @version 1.0
     */
    FormatDataType GetType() const
    {
        return type_;
    }

    /**
     * @brief Sets a 32-bit integer.
     *
     * @param val Indicates the 32-bit integer to set.
     * @return Returns <b>true</b> if the setting is successful; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool SetValue(int32_t val);

    /**
     * @brief Sets a 64-bit long integer.
     *
     * @param val Indicates the 64-bit long integer to set.
     * @return Returns <b>true</b> if the setting is successful; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool SetValue(int64_t val);

    /**
     * @brief Sets a single-precision floating-point number.
     *
     * @param val Indicates the single-precision floating-point number to set.
     * @return Returns <b>true</b> if the setting is successful; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool SetValue(float val);

    /**
     * @brief Sets a double-precision floating-point number.
     *
     * @param val Indicates the double-precision floating-point number to set.
     * @return Returns <b>true</b> if the double-precision floating-point number is successfully set; returns
     * <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool SetValue(double val);

    /**
     * @brief Sets a string.
     *
     * @param val Indicates the string to set.
     * @return Returns <b>true</b> if the setting is successful; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool SetValue(const std::string &val);

    /**
     * @brief Sets a string.
     *
     * @param val Indicates the char to set.
     * @return Returns <b>true</b> if the setting is successful; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool SetValue(const CharValue &val);
    /**
     * @brief Obtains a 32-bit integer.
     *
     * @param val Indicates the 32-bit integer to obtain.
     * @return Returns <b>true</b> if the integer is successfully obtained; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool GetInt32Value(int32_t &val) const;

    /**
     * @brief Obtains a long integer.
     *
     * @param val Indicates the long integer to obtain.
     * @return Returns <b>true</b> if the integer is successfully obtained; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool GetInt64Value(int64_t &val) const;

    /**
     * @brief Obtains a single-precision floating-point number.
     *
     * @param val Indicates the single-precision floating-point number to obtain.
     * @return Returns <b>true</b> if the single-precision number is successfully obtained; returns
     * <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool GetFloatValue(float &val) const;

    /**
     * @brief Obtains a double-precision floating-point number.
     *
     * @param val Indicates the double-precision floating-point number to obtain.
     * @return Returns <b>true</b> if the double-precision number is successfully obtained; returns
     * <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool GetDoubleValue(double &val) const;

    /**
     * @brief Obtains a string.
     *
     * @param val Indicates the string to obtain.
     * @return Returns <b>true</b> if the string is successfully obtained; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool GetStringValue(std::string &val) const;
    /**
     * @brief Obtains a string.
     *
     * @param val Indicates the char to obtain.
     * @return Returns <b>true</b> if the char is successfully obtained; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool GetCharValue(CharValue &val) const;
private:
    FormatDataType type_;
    union {
        int32_t int32Val;
        int64_t int64Val;
        float floatVal;
        double doubleVal;
        std::string *stringVal;
        CharValue charVal;
    } val_;
};

/**
 * @brief Saves and sets media metadata, such as the media playback duration.
 *
 * @since 1.0
 * @version 1.0
 */
class Format {
public:
    /**
     * @brief Default constructor of the {@link Format} instance.
     *
     */
    Format();
    ~Format();

    /**
     * @brief Sets metadata of the integer type.
     *
     * @param key Indicates the metadata key.
     * @param value Indicates the metadata value, which is a 32-bit integer.
     * @return Returns <b>true</b> if the setting is successful; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool PutIntValue(const std::string &key, int32_t value);

    /**
     * @brief Sets metadata of the long integer type.
     *
     * @param key Indicates the metadata key.
     * @param value Indicates the metadata value, which is a 64-bit integer.
     * @return Returns <b>true</b> if the setting is successful; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool PutLongValue(const std::string &key, int64_t value);

    /**
     * @brief Sets metadata of the single-precision floating-point type.
     *
     * @param key Indicates the metadata key.
     * @param value Indicates the metadata value, which is a single-precision floating-point number.
     * @return Returns <b>true</b> if the metadata is successfully set; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool PutFloatValue(const std::string &key, float value);

    /**
     * @brief Sets metadata of the double-precision floating-point type.
     *
     * @param key Indicates the metadata key.
     * @param value Indicates the metadata value, which is a double-precision floating-point number.
     * @return Returns <b>true</b> if the setting is successful; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool PutDoubleValue(const std::string &key, double value);

    /**
     * @brief Sets metadata of the string type.
     *
     * @param key Indicates the metadata key.
     * @param value Indicates the metadata value, which is a string.
     * @return Returns <b>true</b> if the metadata is successfully set; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool PutStringValue(const std::string &key, const std::string &value);
    /**
     * @brief Sets metadata of the string type.
     *
     * @param key Indicates the metadata key.
     * @param value Indicates the metadata value, which is a char ptr.
     * @return Returns <b>true</b> if the metadata is successfully set; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool PutCharValue(const std::string &key, const CharValue &value);

    /**
     * @brief Obtains the metadata value of the integer type.
     *
     * @param key Indicates the metadata key.
     * @param value Indicates the metadata value to obtain, which is a 32-bit integer.
     * @return Returns <b>true</b> if the integer is successfully obtained; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool GetIntValue(const std::string &key, int32_t &value) const;

    /**
     * @brief Obtains the metadata value of the long integer type.
     *
     * @param key Indicates the metadata key.
     * @param value Indicates the metadata value to obtain, which is a 64-bit long integer.
     * @return Returns <b>true</b> if the integer is successfully obtained; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool GetLongValue(const std::string &key, int64_t &value) const;

    /**
     * @brief Obtains the metadata value of the single-precision floating-point type.
     *
     * @param key Indicates the metadata key.
     * @param value Indicates the metadata value to obtain, which is a single-precision floating-point number.
     * @return Returns <b>true</b> if the single-precision number is successfully obtained; returns
     * <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool GetFloatValue(const std::string &key, float &value) const;

    /**
     * @brief Obtains the metadata value of the double-precision floating-point type.
     *
     * @param key Indicates the metadata key.
     * @param value Indicates the metadata value to obtain, which is a double-precision floating-point number.
     * @return Returns <b>true</b> if the double-precision number is successfully obtained; returns
     * <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool GetDoubleValue(const std::string &key, double &value) const;

    /**
     * @brief Obtains the metadata value of the string type.
     *
     * @param key Indicates the metadata key.
     * @param value Indicates the metadata value to obtain, which is a string.
     * @return Returns <b>true</b> if the string is successfully obtained; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool GetStringValue(const std::string &key, std::string &value) const;

    /**
     * @brief Obtains the metadata value of the string type.
     *
     * @param key Indicates the metadata key.
     * @param value Indicates the metadata value to obtain, which is a char ptr.
     * @return Returns <b>true</b> if the char is successfully obtained; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool GetCharValue(const std::string &key, CharValue &value) const;

    /**
     * @brief Obtains the metadata map.
     *
     * @return Returns the map object.
     * @since 1.0
     * @version 1.0
     */
    const std::map<std::string, FormatData *> &GetFormatMap() const;

    /**
     * @brief Sets all metadata to a specified format.
     *
     * @param format Indicates the format. For details, see {@link Format}.
     * @return Returns <b>true</b> if the setting is successful; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool CopyFrom(const Format &format);

private:
    template<typename T>
    bool SetFormatCommon(const std::string &key, const T &value, FormatDataType type);
    std::map<std::string, FormatData *> formatMap_;
};
}  // namespace Media
}  // namespace OHOS
#endif  // FORMAT_H
