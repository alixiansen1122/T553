/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "main/dial/DialSequenceImgView.h"
#include "securec.h"
#include "common/graphic_utils.h"
#include "common/image_cache_manager.h"

namespace OHOS {
namespace {
const uint32_t DIRID = 0xfef << 20;  // 20: offset
}

DialSequenceImgView::DialSequenceImgView(void)
{
    SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    memset_s(nonCycleSeq_, sizeof(nonCycleSeq_), 0, sizeof(nonCycleSeq_));
}

DialSequenceImgView::~DialSequenceImgView(void)
{
    if (repeatSeq_ != nullptr) {
        delete repeatSeq_;
        repeatSeq_ = nullptr;
    }

    for (uint8_t i = 0; i < nonCycleNum_; ++i) {
        delete nonCycleSeq_[i];
        nonCycleSeq_[i] = nullptr;
    }
    nonCycleNum_ = 0;
    loop_ = 0;
    isClick_ = false;
    viewNum_ = 0;
    isAllImgLoaded_ = false;
    curSeq_ = nullptr;
}

bool DialSequenceImgView::SetSequenceRes(const ImageAnimatorInfo *imgSrc, uint8_t imgNum)
{
    if (imgSrc == nullptr || imgNum == 0) {
        return false;
    }

    SetImageAnimatorSrc(imgSrc, imgNum);
    return true;
}

bool DialSequenceImgView::SetPreviewImg()
{
    if (curSeq_ == nullptr) {
        UpdateCurrentSeq();
    }
    if (curSeq_ == nullptr) {
        return false;
    }
    SetAnimatorInfo(curSeq_, 1);
    isAllImgLoaded_ = false;
    SetSrc(curSeq_->imgs[0].imageInfo);
    SetPosition(curSeq_->imgs[0].pos.x, curSeq_->imgs[0].pos.y);
    SetWidth(curSeq_->imgs[0].width);
    SetHeight(curSeq_->imgs[0].height);
    Invalidate();
    return true;
}

bool DialSequenceImgView::LoadCurSeqImgs()
{
    if (curSeq_ == nullptr) {
        UpdateCurrentSeq();
    }
    if (curSeq_ == nullptr) {
        return false;
    }
    isAllImgLoaded_ = true;
    SetAnimatorInfo(curSeq_, curSeq_->info.imgNum);
    return true;
}

bool DialSequenceImgView::AddSequenceRes(const SequenceInfo &para)
{
    if (viewNum_ >= SEQ_IMG_NUM) {
        GRAPHIC_LOGE("seq num has exceeded!");
        return false;
    }

    if (para.repeat) {
        if (repeatSeq_ != nullptr) {
            GRAPHIC_LOGE("the repeat seq has existed!");
            return false;
        }
        repeatSeq_ = new AnimatorInfo;
        repeatSeq_->info = para;
    } else {
        if (nonCycleNum_ >= SEQ_IMG_NUM) {
            GRAPHIC_LOGE("non cycle seq have been exceedeed!");
            return false;
        }
        nonCycleSeq_[nonCycleNum_] = new AnimatorInfo;
        nonCycleSeq_[nonCycleNum_]->info = para;
        ++nonCycleNum_;
    }

    SetTouchable(true);
    SetAnimatorStopListener(this);
    SetRepeat(false);
    ++viewNum_;
    return true;
}

bool DialSequenceImgView::OnClickEvent(const ClickEvent &event)
{
    if (!isAllImgLoaded_) {
        return false;
    }
    if (curSeq_ == nullptr) {
        return false;
    }
    uint8_t curState = GetState();
    switch (curState) {
        case Animator::STOP: {
            if (curSeq_ == repeatSeq_) {
                isClick_ = true;
            } else {
                UpdateCurrentSeq();
                SetAnimatorInfo(curSeq_, curSeq_->info.imgNum);
                Start();
            }
            break;
        }
        case Animator::PAUSE: {
            return false;
        }
        case Animator::RUNNING:
        case Animator::START: {
            if (curSeq_ == repeatSeq_) {
                isClick_ = true;
            }
            break;
        }
        default: {
            GRAPHIC_LOGE("error state");
        }
    }

    return UIImageAnimatorView::OnClickEvent(event);
}

void DialSequenceImgView::OnAnimatorStop(UIView &view)
{
    if (!isAllImgLoaded_) {
        return;
    }
    bool isClick = isClick_;
    isClick_ = false;
    bool isRepeat = (curSeq_ == repeatSeq_);
    if (isRepeat && isClick) {
        UpdateCurrentSeq();
        SetAnimatorInfo(curSeq_, curSeq_->info.imgNum);
        Start();
    } else if (isRepeat && !isClick) {
        Start();
    } else {
        if (repeatSeq_ != nullptr) {
            UpdateCurrentSeq();
            SetAnimatorInfo(curSeq_, curSeq_->info.imgNum);
            Start();
        }
    }
}

ImageAnimatorInfo *DialSequenceImgView::LoadImgs(const SequenceInfo &para, uint8_t num) const
{
    if (para.fp == nullptr || num > para.imgNum) {
        return nullptr;
    }

    long curPt = ftell(para.fp);
    fseek(para.fp, para.resPos, SEEK_SET);
    uint16_t *imgIdArray = static_cast<uint16_t *>(UIMalloc(sizeof(uint16_t) * num));
    defer
    {
        if (imgIdArray != nullptr) {
            UIFree(imgIdArray);
        }
        fseek(para.fp, curPt, SEEK_SET);
    };
    if (imgIdArray == nullptr) {
        return nullptr;
    }
    ImageAnimatorInfo *imgInfoArray = static_cast<ImageAnimatorInfo *>(UIMalloc(num * sizeof(ImageAnimatorInfo)));
    if (imgInfoArray == nullptr) {
        return nullptr;
    }

    if (fread(imgIdArray, 1, sizeof(uint16_t) * num, para.fp) != sizeof(uint16_t) * num) {
        GRAPHIC_LOGE("failed to read resId");
        UIFree(imgInfoArray);
        return nullptr;
    }
    for (uint8_t i = 0; i < num; ++i) {
        uint32_t resId = imgIdArray[i] | DIRID;
        ImageInfo *info =
            ImageCacheManager::GetInstance().LoadOneInMultiRes(resId, para.fileName, para.fp, false, para.imgOffset);
        if (info == nullptr) {
            GRAPHIC_LOGE("load img fail! file:%s resId:%x", para.fileName.c_str(), resId);
            UIFree(imgInfoArray);
            return nullptr;
        }
        imgInfoArray[i].imageInfo = info;
        imgInfoArray[i].pos = {GetRelativeRect().GetX(), GetRelativeRect().GetY()};
        imgInfoArray[i].width = info->header.width;
        imgInfoArray[i].height = info->header.height;
        imgInfoArray[i].imageType = IMG_SRC_IMAGE_INFO;
    }
    return imgInfoArray;
}

void DialSequenceImgView::UpdateCurrentSeq()
{
    if (curSeq_ != nullptr) {
        if (repeatSeq_ != nullptr && nonCycleNum_ == 0) {
            return;
        } else if (repeatSeq_ == nullptr && nonCycleNum_ != 0) {
            curSeq_ = nonCycleSeq_[loop_];
            loop_ = (loop_ + 1) % nonCycleNum_;
            return;
        }

        // both repeat and noncycle exist
        if (curSeq_ == repeatSeq_) {
            curSeq_ = nonCycleSeq_[loop_];
            loop_ = (loop_ + 1) % nonCycleNum_;
        } else {
            curSeq_ = repeatSeq_;
        }
        return;
    }

    if (repeatSeq_ != nullptr) {
        curSeq_ = repeatSeq_;
    } else if (nonCycleNum_ != 0) {
        curSeq_ = nonCycleSeq_[loop_];
        loop_ = (loop_ + 1) % nonCycleNum_;
    } else {
        GRAPHIC_LOGE("sequence img has not been added!");
        return;
    }
}

void DialSequenceImgView::SetAnimatorInfo(AnimatorInfo *info, uint8_t num)
{
    if (info == nullptr || num == 0) {
        GRAPHIC_LOGE("para error");
        return;
    }
    if (num > info->info.imgNum) {
        GRAPHIC_LOGE("The number of images exceeds the limit.");
        return;
    }

    if (num == info->num && info->imgs != nullptr) {
        SetImageAnimatorSrc(info->imgs, info->num, info->info.interval);
    } else {
        if (info->imgs != nullptr) {
            UIFree(info->imgs);
        }
        info->num = num;
        info->imgs = LoadImgs(info->info, info->num);
        if (info->imgs == nullptr) {
            return;
        }
        SetImageAnimatorSrc(info->imgs, info->num, info->info.interval);
    }
}
}  // namespace OHOS