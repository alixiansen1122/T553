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

#include <cmath>
#include <securec.h>
#include "common/graphic_utils.h"
#include "input_method/ui_pinyin_candidate.h"

#if ENABLE_SOFT_KEYBOARD
namespace OHOS {

PinyinCandidate::PinyinCandidate()
{
    memset_s(m_pyTabBuf, sizeof(m_pyTabBuf), 0, sizeof(m_pyTabBuf));
    memset_s(m_hzTabBuf, sizeof(m_hzTabBuf), 0, sizeof(m_hzTabBuf));
}

PinyinCandidate::~PinyinCandidate()
{
}

PinyinCandidate::PinyinState PinyinCandidate::InitPinyinTablePath(const std::string &path)
{
    size_t len = path.length();
    char *tablePath = new char[len + 1];
    defer {
        delete []tablePath;
    };
    if (tablePath == nullptr) {
        GRAPHIC_LOGE("the length of path is too long.");
        return PinyinState::MEM_ALLOC_ERROR;
    }
    int32_t ret = snprintf_s(tablePath, len + 1, len, "%s", path.c_str());
    if (ret == -1) {
        GRAPHIC_LOGE("Secure fuction issue.");
        return PinyinState::MEM_COPY_ERROR;
    }
    char truePath[PATH_MAX + 1] = {0x00};
#ifndef WIN32
    if ((path.length() == 0) || (realpath(tablePath, truePath) == nullptr)) {
        GRAPHIC_LOGE("The path of pin yin table is invalid.");
        return PinyinState::INVALID_PATH;
    }
#else
    memcpy_s(truePath, sizeof(truePath), tablePath, len + 1);
#endif
    m_pinyinTablePath = truePath;
    return PinyinState::SUCCESS;
}

int32_t PinyinCandidate::SetLv2NumPerPage(uint8_t num)
{
    if ((num < 1) || (num > 10)) { //  10是汉字候选码的个数
        return -1;
    } else {
        m_lv2NumPerPage = num;
        return 0;
    }
}

PinyinCandidate::PinyinState PinyinCandidate::Init(const std::string &path)
{
    PinyinState pathState = InitPinyinTablePath(path);
    if (pathState != PinyinState::SUCCESS) {
        return pathState;
    }
    InitEx();
    return PinyinState::SUCCESS;
}

void PinyinCandidate::InitEx(void)
{
    int32_t ret;
    size_t len = m_pinyinTablePath.length();
    char *table = new char[len + 1];
    defer {
        delete []table;
    };
    if (table == nullptr) {
        GRAPHIC_LOGE("Malloc mem fails.");
        return;
    }

    ret = snprintf_s(table, len + 1, len, "%s", m_pinyinTablePath.c_str());
    if (ret == -1) {
        GRAPHIC_LOGE("Secure fuction issue.");
        return;
    }

    ret = LoadCodeTable((const int8_t *)table);
    if (ret != 0) {
        GRAPHIC_LOGE("LoadCodeTable failed!");
    }
}

std::string PinyinCandidate::DebugInfo(PinyinCandidate::PinyinState state) const
{
    static const struct {
        PinyinState state;
        std::string info;
    } infoTable[] = {
        {PinyinState::SUCCESS, "SUCCESS"},
        {PinyinState::INVALID_PINYIN, "INVALID_PINYIN"},
        {PinyinState::INVALID_PATH, "INVALID_PATH"},
        {PinyinState::MEM_NOT_ENOUGH, "MEM_NOT_ENOUGH"},
        {PinyinState::MEM_SET_ERROR, "MEM_SET_ERROR"},
        {PinyinState::MEM_COPY_ERROR, "MEM_COPY_ERROR"},
        {PinyinState::MEM_ALLOC_ERROR, "MEM_ALLOC_ERROR"},
        {PinyinState::INVALID_PAGE_INDEX, "INVALID_PAGE_INDEX"},
    };
    for (size_t i = 0; i < sizeof(infoTable) / sizeof(infoTable[0]); ++i) {
        if (infoTable[i].state == state) {
            return infoTable[i].info;
        }
    }
    return "not defined!";
}

PinyinCandidate::PinyinState PinyinCandidate::FindHanzi(const std::string &sourceStr)
{
    if (sourceStr.length() > IME_PYMAXLENTH) {
        return PinyinState::INVALID_PINYIN;
    }

    bool candidateFlag = false;

    hanziStatus_.m_currentHzPage = 0;
    hanziStatus_.m_currentHzIndex = 0;
    hanziStatus_.m_matchHzNumber = 0;
    pinyinStatus_.m_currentPyIndex = sourceStr[0] - 'a';

    if (pinyinStatus_.m_currentPyIndex >= MAXCHARNUM) {
        return PinyinState::INVALID_PINYIN;
    }

    uint8_t index;
    for (index = 0; index < MAXSAMEPREFIXPY; index++) {
        if (strncmp(sourceStr.c_str(),
                reinterpret_cast<char *>(m_pyTabBuf[pinyinStatus_.m_currentPyIndex][index].pinyin),
                sourceStr.length()) == 0) {
            candidateFlag = true;
            break;
        }
    }

    if (candidateFlag == false) {
        GRAPHIC_LOGE("查找拼音索引出错!!\n");
        return PinyinState::INVALID_PINYIN;
    }

    hanziStatus_.m_currentHzIndex = m_pyTabBuf[pinyinStatus_.m_currentPyIndex][index].hzIndex;
    hanziStatus_.m_matchHzNumber = m_pyTabBuf[pinyinStatus_.m_currentPyIndex][index].hzNum;
    if (m_lv2NumPerPage != 0) {
        hanziStatus_.m_matchHZPageNumber =
            static_cast<uint8_t>(ceil(1.0 * hanziStatus_.m_matchHzNumber / m_lv2NumPerPage));
    }

    return PinyinState::SUCCESS;
}

PinyinCandidate::PinyinState PinyinCandidate::GetHanziPage(
    uint8_t pageNumber, int8_t candidate[][IME_HZLENTH], uint8_t len)
{
    if (len < m_lv2NumPerPage) {
        /* 存放结果的字符串长度小于要求长度 */
        return PinyinState::MEM_NOT_ENOUGH;
    }

    if (pageNumber >= GetHanziPagesNum()) {
        return PinyinState::INVALID_PAGE_INDEX;
    }

    for (uint8_t i = 0; i < m_lv2NumPerPage; i++) {
        uint16_t curIndex = hanziStatus_.m_currentHzIndex + (pageNumber * m_lv2NumPerPage) + i;
        if (curIndex >= (hanziStatus_.m_currentHzIndex + hanziStatus_.m_matchHzNumber)) {
            break;
        }

        if (curIndex >= MAXHZNUM) {
            break;
        }

        candidate[i][0] =
            (int8_t)(m_hzTabBuf[curIndex]
                         .hz >>
                     16);  //  Shift right by 16 bytes and get the storage bitin the encoding format
        candidate[i][1] =
            (int8_t)(m_hzTabBuf[curIndex]
                         .hz >>
                     8);   //  Shift right by 8 bytes and get the storage bit in the encoding format
        candidate[i][2] =  // 2 is the index of the Array
            (int8_t)(m_hzTabBuf[curIndex]
                         .hz);
    }
    return PinyinState::SUCCESS;
}

PinyinCandidate::PinyinState PinyinCandidate::GetFwdHanziPage(int8_t candidate[][IME_HZLENTH], uint8_t len)
{
    if (hanziStatus_.m_currentHzPage > 0) {
        --hanziStatus_.m_currentHzPage;
        return GetHanziPage(hanziStatus_.m_currentHzPage, candidate, len);
    }
    return PinyinState::INVALID_PAGE_INDEX;
}

PinyinCandidate::PinyinState PinyinCandidate::GetCurHanziPage(int8_t candidate[][IME_HZLENTH], uint8_t len)
{
    return GetHanziPage(hanziStatus_.m_currentHzPage, candidate, len);
}

PinyinCandidate::PinyinState PinyinCandidate::GetNxtHanziPage(int8_t candidate[][IME_HZLENTH], uint8_t len)
{
    if (hanziStatus_.m_currentHzPage + 1 < GetHanziPagesNum()) {
        ++hanziStatus_.m_currentHzPage;
        return GetHanziPage(hanziStatus_.m_currentHzPage, candidate, len);
    }
    return PinyinState::INVALID_PAGE_INDEX;
}

PinyinCandidate::PinyinState PinyinCandidate::GetAllHanzi(int8_t candidate[][IME_HZLENTH], uint8_t len)
{
    if (len < hanziStatus_.m_matchHzNumber) {
        return PinyinState::MEM_NOT_ENOUGH;
    }

    for (uint16_t i = hanziStatus_.m_currentHzIndex; i < hanziStatus_.m_currentHzIndex + hanziStatus_.m_matchHzNumber;
         ++i) {
        if (i >= MAXHZNUM) {
            break;
        }
        candidate[i - hanziStatus_.m_currentHzIndex][0] =
            static_cast<int8_t>(m_hzTabBuf[i]
                         .hz >>
                     16);  //  Shift right by 16 bytes and get the storage bitin the encoding format
        candidate[i - hanziStatus_.m_currentHzIndex][1] =
            static_cast<int8_t>(m_hzTabBuf[i]
                         .hz >>
                     8);   //  Shift right by 8 bytes and get the storage bit in the encoding format
        candidate[i - hanziStatus_.m_currentHzIndex][2] =  // 2 is the index of the Array
            static_cast<int8_t>(m_hzTabBuf[i]
                         .hz);
    }
    return PinyinState::SUCCESS;
}

int32_t PinyinCandidate::LoadCodeTable(const int8_t *filePath)
{
    int32_t ret;
    FILE *tableFile = nullptr;
    uint32_t bufLen;
    char resolvedPath[PATH_MAX + 1] = {0};

#ifndef WIN32
    if (realpath((const char *)filePath, resolvedPath) == nullptr) {
        return -1;
    }
#else
    memcpy_s(resolvedPath, sizeof(resolvedPath), filePath, strlen(reinterpret_cast<const char*>(filePath)) + 1);
#endif

    tableFile = fopen((const char *)resolvedPath, "rb");
    if (tableFile == nullptr) {
        return -1;
    }

    ret = memset_s(m_hzTabBuf, sizeof(m_hzTabBuf), 0, sizeof(m_hzTabBuf));
    if (ret != EOK) {
        (void)fclose(tableFile);
        return -1;
    }

    ret = memset_s(m_pyTabBuf, sizeof(m_pyTabBuf), 0, sizeof(m_pyTabBuf));
    if (ret != EOK) {
        (void)fclose(tableFile);
        return -1;
    }

    bufLen = fread(m_hzTabBuf, 1, sizeof(m_hzTabBuf), tableFile);
    if (bufLen != sizeof(m_hzTabBuf)) {
        (void)fclose(tableFile);
        return -1;
    }

    if (fread(m_pyTabBuf, 1, sizeof(m_pyTabBuf), tableFile) != sizeof(m_pyTabBuf)) {
        (void)fclose(tableFile);
        return -1;
    }

    (void)fclose(tableFile);

    return 0;
}

int32_t PinyinCandidate::RecoverCodeTable(const int8_t *filePath, const int8_t *BackupPath)
{
    FILE *tableFile = nullptr;
    FILE *backupFile = nullptr;
    uint32_t fileSizeCounter = 0;
    int32_t temp = 0;
    char resolvedPath1[PATH_MAX + 1] = {0};
    char resolvedPath2[PATH_MAX + 1] = {0};

#ifndef WIN32
    if (realpath((const char *)filePath, resolvedPath1) == nullptr) {
        return -1;
    }

    if (realpath((const char *)BackupPath, resolvedPath2)  == nullptr) {
        return -1;
    }
#endif

    /* 只写方式打开 */
    tableFile = fopen(reinterpret_cast<char *>(resolvedPath1), "w");
    if (tableFile == nullptr) {
        return -1;
    }

    /* 只读方式打开 */
    backupFile = fopen(reinterpret_cast<char *>(resolvedPath2), "r");
    if (backupFile == nullptr) {
        (void)fclose(tableFile);
        return -1;
    }

    while (1) {
        temp = fgetc(backupFile);
        if ((temp == EOF) || (feof(backupFile) != 0)) {
            break;
        }

        fputc((int8_t)temp, tableFile);
        fileSizeCounter++;
        if (fileSizeCounter > (sizeof(m_hzTabBuf) + sizeof(m_pyTabBuf))) {
            /* 防止出错造成死循环 */
            break;
        }
    }

    fclose(tableFile);
    fclose(backupFile);
    if (fileSizeCounter == (sizeof(m_hzTabBuf) + sizeof(m_pyTabBuf))) {
        GRAPHIC_LOGD("Code table file recover successed!");
        return 0;
    } else {
        return -1;
    }
}

} // namespace OHOS
#endif // ENABLE_SOFT_KEYBOARD