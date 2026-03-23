/*
 * Copyright (c) 2023 CompanyNameMagicTag.
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
#ifndef GRAPHIC_LITE_DIAL_BIN_PARSER_H
#define GRAPHIC_LITE_DIAL_BIN_PARSER_H
#include "main/DialBinParserManager.h"
#include "main/DialBinTypesV2.h"
#include "components/ui_abstract_progress.h"

namespace OHOS {
class DialBinParser : public DialBinParserInterface {
public:
    bool UpdateDial(const std::string& file, FILE* fp, DialViewGroup* group, DisplayState state) override;
    bool GetDialPreviewInfo(const std::string& file, FILE* fp, DialPreviewInfo* info) override;
    DialBinParser();
    ~DialBinParser() override;

protected:
    int32_t ParseDialHeader(FILE* file);
    int32_t ParserDialStaticImg(FILE* fp);
    int32_t ParserDialSequenceImg(FILE* fp);
    int32_t ParserDialRotateImg(FILE* fp);
    int32_t ParserDialOptionImg(FILE* fp);
    int32_t ParserDialDigitalImg(FILE* fp);
    int32_t ParserDialLabel(FILE* fp);
    int32_t ParserDialArcLabel(FILE* fp);
    int32_t ParserDialBoxProgress(FILE* fp);
    int32_t ParserDialArcProgress(FILE* fp);
    int32_t ParserDialVideo(FILE* fp);
    int32_t ParserDialClick(FILE* fp);
    int32_t ParserDialKaleidoscope(FILE* fp);

private:
    void* MallocAndReadBuf(FILE* file, uint32_t readSize);
    bool SetProgressType(FILE* fp, UIAbstractProgress* view, DialProgressType data);

    static constexpr uint8_t DIAL_BIN_PARSER_FUNC_NUM = 12; // 12: func max value
    int32_t (DialBinParser::*funcArray_[DIAL_BIN_PARSER_FUNC_NUM])(FILE*);

    DialViewGroup* dialGroup_ = nullptr;
    std::string file_;
    uint32_t dirId_ = 0xfef << 20; // 20: offset
    uint32_t imageOffset_ = 0;
    bool v2Version_ = false;
    DisplayState state_ = DisplayState::NORMAL;
};
}
#endif
