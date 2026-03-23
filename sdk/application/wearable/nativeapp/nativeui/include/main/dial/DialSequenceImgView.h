/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef DIAL_SEQUENCE_IMG_VIEW_H
#define DIAL_SEQUENCE_IMG_VIEW_H

#include "components/ui_image_animator.h"

namespace OHOS {
struct SequenceInfo : public HeapBase {
    long imgOffset = 0;
    long resPos = 0;
    uint8_t imgNum = 0;
    FILE *fp = nullptr;
    std::string fileName;
    uint16_t interval = 0;
    bool repeat = false;
};

class DialSequenceImgView : public UIImageAnimatorView, public UIImageAnimatorView::AnimatorStopListener {
public:
    DialSequenceImgView();
    ~DialSequenceImgView() override;

    bool SetSequenceRes(const ImageAnimatorInfo *imgSrc, uint8_t imgNum);
    bool SetPreviewImg();
    bool LoadCurSeqImgs();
    bool AddSequenceRes(const SequenceInfo &para);
    bool OnClickEvent(const ClickEvent &event) override;
    void OnAnimatorStop(UIView &view) override;

private:
    ImageAnimatorInfo *LoadImgs(const SequenceInfo &para, uint8_t num) const;
    void UpdateCurrentSeq();
    struct AnimatorInfo : public HeapBase {
        SequenceInfo info;
        ImageAnimatorInfo *imgs = nullptr;
        uint8_t num = 0;
        ~AnimatorInfo()
        {
            if (imgs != nullptr) {
                UIFree(imgs);
                imgs = nullptr;
                num = 0;
            }
        }
    };
    void SetAnimatorInfo(AnimatorInfo *info, uint8_t num);
    static constexpr uint8_t SEQ_IMG_NUM = 3;
    AnimatorInfo *repeatSeq_ = nullptr;
    AnimatorInfo *nonCycleSeq_[SEQ_IMG_NUM];
    AnimatorInfo *curSeq_ = nullptr;
    uint8_t nonCycleNum_ = 0;
    uint8_t loop_ = 0;
    bool isClick_ = false;
    uint8_t viewNum_ = 0;
    bool isAllImgLoaded_ = false;
};
}  // namespace OHOS
#endif
