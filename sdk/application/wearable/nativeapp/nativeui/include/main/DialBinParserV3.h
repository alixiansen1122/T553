/*
 * Copyright (c) CompanyNameMagicTag 2025. All rights reserved.
 * Description: DialBinParserV3
 * Author:
 * Create: 2025-02-06
 */

#ifndef GRAPHIC_LITE_DIAL_BIN_PARSER_V3_H
#define GRAPHIC_LITE_DIAL_BIN_PARSER_V3_H
#include "main/DialBinParserManager.h"
#include "main/dial/DialModelTime.h"
#include "components/ui_abstract_progress.h"
#include "main/dial/DialViewContainer.h"
#include "main/dial/DialDataType.h"
#include "main/dial/DialOHSliceId.h"

namespace OHOS {
class DialBinParserV3 : public DialBinParserInterface {
public:
    DialBinParserV3();
    ~DialBinParserV3() override;
    bool UpdateDial(const std::string& file, FILE* fp, DialViewGroup* group, DisplayState state) override;
    bool GetDialPreviewInfo(const std::string& file, FILE* fp, DialPreviewInfo* info) override;
    void SetEncryptData(uint8_t *data, uint32_t len) override final;

protected:
    bool ParseDialHeader(FILE* file);
    bool ParserDialStaticImg(ContainerInterface &parent);
    bool ParserDialSequenceImg(ContainerInterface &parent);
    bool ParserDialRotateImg(ContainerInterface &parent);
    bool ParserDialOptionImg(ContainerInterface &parent);
    bool ParserDialDigitalImg(ContainerInterface &parent);
    bool ParserDialLabel(ContainerInterface &parent);
    bool ParserDialArcLabel(ContainerInterface &parent);
    bool ParserDialBoxProgress(ContainerInterface &parent);
    bool ParserDialArcProgress(ContainerInterface &parent);
    bool ParserDialVideo(ContainerInterface &parent);
    bool ParserDialClick(ContainerInterface &parent);
    bool ParserDialKaleidoscope(ContainerInterface &parent);
    bool ParserDialContainer(ContainerInterface &parent);
    bool ParserDialOptions(ContainerInterface &parent);

private:
    void* MallocAndReadBuf(FILE* file, uint32_t readSize);
    bool SetProgressType(FILE* fp, UIAbstractProgress* view, DialProgressType data);

    static constexpr uint8_t DIAL_BIN_PARSER_FUNC_NUM = 14; // 14: func max value
    bool (DialBinParserV3::*funcArray_[DIAL_BIN_PARSER_FUNC_NUM])(ContainerInterface&);
    void AddToParentWithView(ContainerInterface &parent, uint8_t displayMode, UIView *view);
    void AddToParentWithDialView(ContainerInterface &parent, uint8_t displayMode,
        DialView *view, DialDataType bindData, bool isPeriodic);
    struct {
        uint8_t *viewType;
        uint8_t dialViewCount;
        FILE *file;
        uint8_t curIndex;
    } curViewType_;
    bool ParseTimeIntervals(FILE* fp, uint8_t cnt, TimeChangeListener *listener);
    bool ParseViewType(uint8_t *viewType, uint8_t dialViewCount, FILE *file);
    bool ParseContainerSubviews(DialViewContainer *container, uint8_t num);
    bool ParseOptionsSubviews(ContainerInterface &parent);
    DialHeader *GetHeader(FILE *fp);
    uint8_t *GetVeiwType(FILE *fp, DialHeader *dialHeader);

    DialViewGroup* dialGroup_ = nullptr;
    std::string file_;
    const uint32_t dirId_ = 0xfef << 20; // 20: offset
    uint32_t imageOffset_ = 0;
    DisplayState state_ = DisplayState::NORMAL;
    uint8_t *encryptedData_ = nullptr;
    uint32_t encrpytedLen_ = 0;
}; // class DialBinParserV3
} // namespace OHOS

#endif // GRAPHIC_LITE_DIAL_BIN_PARSER_V3_H