/*
 * Copyright (c) CompanyNameMagicTag 2024. All rights reserved.
 * Description: DialBinParserManager
 * Author:
 * Create: 2024-12-03
 */

#ifndef GRAPHIC_LITE_DIAL_BIN_PARSER_MANAGER_H
#define GRAPHIC_LITE_DIAL_BIN_PARSER_MANAGER_H
#include <iostream>
#include <cstring>
#include "graphic_types.h"
#include "main/dial/DialViewGroup.h"
#include "main/DialBinTypes.h"

namespace OHOS {
class DialBinParserInterface {
public:
    virtual bool UpdateDial(const std::string& file, FILE* fp, DialViewGroup* group, DisplayState state) = 0;
    virtual bool GetDialPreviewInfo(const std::string& file, FILE* fp, DialPreviewInfo* info) = 0;
    virtual void SetEncryptData(uint8_t *data, uint32_t len) {}
    virtual ~DialBinParserInterface() {}
}; // class DialBinParserInterface

class DialBinParserManager {
public:
    static DialBinParserManager *GetInstance(void);
    bool UpdateDial(const std::string &file, FILE *fp, DialViewGroup *group, DisplayState state);
    bool GetDialPreviewInfo(const std::string& file, FILE *fp, DialPreviewInfo *info, bool isNeedVerify = false);
    bool RegisterParser(const std::string &version, DialBinParserInterface *parser);

    uint8_t GetSupportVersionCnt() const
    {
        return cnt_;
    }

    bool GetVersionByIndex(uint8_t index, std::string &version)
    {
        if (index >= cnt_) {
            return false;
        }
        version = parsers_[index].version;
        return true;
    }

private:
    DialBinParserManager();
    virtual ~DialBinParserManager();
    DialBinParserInterface *GetParser(FILE* fp) const;
    DialBinParserInterface *GetParser(const std::string &fileName, FILE *fp, bool isNeedVerify = false) const;
    static constexpr uint8_t PARSER_MAX_NUM = 8;
    struct Pair {
        std::string version;
        DialBinParserInterface *parser = nullptr;
    };
    Pair parsers_[PARSER_MAX_NUM];
    uint8_t cnt_ = 0;
}; // class DialBinParserManager

class DialBinParserRegister {
public:
    DialBinParserRegister(const std::string &version, DialBinParserInterface *parser)
    {
        if (!DialBinParserManager::GetInstance()->RegisterParser(version, parser)) {
            GRAPHIC_LOGE("failed to register %s.", version.c_str());
        }
    }
}; // class DialBinParserRegister
} // namespace OHOS

#define REGIST_PARSER(version, func) \
    const static OHOS::DialBinParserRegister STATIC_PARSER_VAR_NAME(version, new func)
#endif // GRAPHIC_LITE_DIAL_BIN_PARSER_MANAGER_H
