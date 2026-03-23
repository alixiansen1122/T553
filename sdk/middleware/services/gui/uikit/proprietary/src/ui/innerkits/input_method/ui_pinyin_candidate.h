/*
 * Copyright (c) 2022 CompanyNameMagicTag.
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

#ifndef GRAPHIC_LITE_UI_PINYIN_CANDIDATE_H
#define GRAPHIC_LITE_UI_PINYIN_CANDIDATE_H
#include "components/ui_label.h"
#include "components/ui_view_group.h"
#include "gfx_utils/vector.h"
#include "graphic_config.h"
#include "input_method/ui_keyboard_codetable.h"

#if ENABLE_SOFT_KEYBOARD

namespace OHOS {
class PinyinCandidate : public HeapBase {
public:
    enum class PinyinState : int32_t {
        SUCCESS = 0,
        INVALID_PINYIN, // input source str is invalid
        INVALID_PATH,
        MEM_NOT_ENOUGH,
        MEM_SET_ERROR,
        MEM_COPY_ERROR,
        MEM_ALLOC_ERROR,
        INVALID_PAGE_INDEX,
    };
    static const uint32_t IME_PYMAXLENTH = 8;  //  单个拼音的最大字母个数
    static const uint32_t IME_HZLENTH = 4;  //  单个汉字占内存大小
    static const uint8_t MAXSAMEPREFIXPY = 45;  //  前缀相同的拼音的最大个数
    static const int32_t MAXHZNUM = 0x2192;  //  汉字的总共个数
    static const uint8_t MAXNUMBEROFCHAR = 5;  //  5 is the Maximum number of characters on key
    static const uint8_t MAXCHARNUM = 26; // 26 characters, a ~ z
    static const uint32_t MAX_TABLE_LEN = 12; // 12 is the max Length of Table
    static const uint32_t MAX_TABLE_MEMBER_LEN = 5; // 5 is the max length of Tabel Member

    PinyinCandidate(const PinyinCandidate&) = delete;
    PinyinCandidate& operator=(const PinyinCandidate&) = delete;
    ~PinyinCandidate();
    PinyinCandidate();

    std::string DebugInfo(PinyinState state) const;
    PinyinState FindHanzi(const std::string &sourceStr);
    PinyinState GetHanziPage(uint8_t pageNumber, int8_t candidate[][IME_HZLENTH], uint8_t len);
    PinyinState GetFwdHanziPage(int8_t candidate[][IME_HZLENTH], uint8_t len);
    PinyinState GetCurHanziPage(int8_t candidate[][IME_HZLENTH], uint8_t len);
    PinyinState GetNxtHanziPage(int8_t candidate[][IME_HZLENTH], uint8_t len);
    PinyinState GetAllHanzi(int8_t candidate[][IME_HZLENTH], uint8_t len);
    uint8_t GetHanziPagesNum() const
    {
        return hanziStatus_.m_matchHZPageNumber;
    }
    uint8_t GetMatchHanziNum() const
    {
        return hanziStatus_.m_matchHzNumber;
    }
    uint8_t GetHanziPageIndex() const
    {
        return hanziStatus_.m_currentHzPage;
    }

    PinyinState Init(const std::string &path);

    /* 设定每页显示二级候选码的个数 */
    int32_t SetLv2NumPerPage(uint8_t num);
    uint8_t GetLv2NumPerPage()
    {
        return m_lv2NumPerPage;
    }

protected:
    /*  从外部文件装载候选码 */
    int32_t LoadCodeTable(const int8_t *filePath);
    /*  修复损坏的码表文件 */
    int32_t RecoverCodeTable(const int8_t *filePath, const int8_t *backupPath);

private:
    /* init pinyin table path */
    PinyinState InitPinyinTablePath(const std::string &path);
    void InitEx(void);

    /* 以下两结构组成了拼音码表，并且以只读方式保存,
    * 今后扩展词频调整，则将词频数据以另外文件单独保存，
    * FreqTableIndex数据成员用于实现拼音码表中的结构与词频表中的记录相映射
    */
    struct PYTab {
        int8_t pinyin[IME_PYMAXLENTH];
        uint32_t freqTableIndex; // 指向本结构在频率记录表中的位置
        uint16_t hzIndex;
        uint8_t hzNum;  // 拼音对应的汉字个数
    };

    struct HZTab {
        uint32_t hz;
        uint16_t freqTableIndex;  // 指向本结构在频率记录表中的位置
    };

    uint8_t m_lv2NumPerPage = 5; // 5 是二级候选码每页显示的个数

    /* 拼音码表的缓存 */
    PYTab m_pyTabBuf[MAXCHARNUM][MAXSAMEPREFIXPY];
    HZTab m_hzTabBuf[MAXHZNUM] = {0, 0};  // 汉字码表缓存

    struct {
        uint8_t m_currentPyPage = 0; // current pinyin page number
        uint8_t m_matchPyPage = 0; // matched pinyin page num
        int8_t m_matchPyStartIndex = -1; /*  在码表中与指定拼音匹配的拼音的起始索引 */
        int8_t m_matchPyEndIndex = -1;   /*  在码表中与指定拼音匹配的结束索引 */
        uint8_t m_matchPyNumber = 0;     /*  与指定拼音匹配的候选码数量 */
        uint8_t m_currentPyIndex = 0;
    } pinyinStatus_;
    struct {
        uint16_t m_currentHzIndex = 0;
        uint8_t m_matchHzNumber = 0;
        uint8_t m_matchHZPageNumber = 0;
        uint8_t m_currentHzPage = 0;
    } hanziStatus_;

    std::string m_pinyinTablePath = "";
};
} // namespace OHOS
#endif // ENABLE_SOFT_KEYBOARD
#endif // GRAPHIC_LITE_UI_PINYIN_CANDIDATE_H