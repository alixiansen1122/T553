/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: List with built-in adapter sample
 * Author: Hisi Team
 * Created: 2025-07
 */

#include "ui_test_simple_transform_list.h"
#include "common/screen.h"
#include "components/ui_menu_item.h"
#include "common/image_cache_manager.h"
#include "graphic_config.h"
#include "ui_test_list_fps/ui_list_fps_res.h"

namespace OHOS {
static const int16_t LIST_WIDTH = 454;
static const int16_t LIST_HEIGHT = 454;
static const int16_t SCROLL_STEP = 10;
static const int16_t DEFAULT_RADIUS = 10;
static const uint16_t ITEM_WIDTH = 400;
static const uint16_t ITEM_HEIGHT = 100;
static const uint16_t IMAGE_WIDTH = 50;
static const uint16_t IMAGE_HEIGHT = 50;
static const uint16_t LABEL_WIDTH = 300;
static const uint16_t LABEL_HEIGHT = 100;
static const uint16_t LIST_FONT_SIZE = 40;
static const uint16_t SCALE_FACTOR = 400;
static const uint16_t X_POS = 50;
static const uint16_t Y_POS = 50;
static const uint16_t IMAGE_W = 220;
static const uint16_t IMAGE_H = 220;
static const uint16_t IMAGE_NUM = 3;

struct ItemNode {
    int resId;
    const char* txt;
    bool isEnableImgAni;
    bool isEnableSlider;
    ImageInfo* imgInfo;
};

// First type can't be 0
enum class ItemType {
    IMGANI_SLIDER = 1,
    NULL_SLIDER,
    IMGANI_NULL,
    IMG_LABEL,
};

static ItemNode g_itemNode[] = {
    {0, nullptr, true, true},
    {APPLIST_ALARM, "闹钟", false, false},
    {BLUETOOTH_TITLE, "蓝牙", false, false},
    {APPLIST_CALLER_IMAGE, "通话记录", false, false},
    {APPLIST_COMPASS_IMAGE, "指南针", false, false},
    {APPLIST_SETTING, "设置", false, false},
    {APPLIST_SPO2, "血氧饱和度", false, false},
    {APPLIST_VOCASSIST_IMAGE, "语音助手", false, false},
    {APPLIST_ALARM, "长文本滚动测试ABCD", false, false},
    {BLUETOOTH_TITLE, "蓝牙", false, false},
    {APPLIST_CALLER_IMAGE, "通话记录", false, false},
    {APPLIST_COMPASS_IMAGE, "指南针", false, false},
    {APPLIST_SETTING, "设置", false, false},
    {APPLIST_SPO2, "血氧饱和度", false, false},
    {APPLIST_VOCASSIST_IMAGE, "语音助手", false, false}
};

static char* g_imagePath[IMAGE_NUM] = {
    RES_PATH"A094_051_8888.bin",
    RES_PATH"A094_051_888.bin",
    RES_PATH"A094_051_565.bin",
};

static ImageAnimatorInfo g_binImageAnimatorInfo[IMAGE_NUM] = {{{0}}};

static bool TransformItemBaseOnPosition(UITransformList* list, UIView* item)
{
    if (list == nullptr || item == nullptr) {
        return false;
    }
    if (item->GetViewType() != UI_TRANSFORM_GROUP) {
        return false;
    }
    UITransformGroup* group = dynamic_cast<UITransformGroup*>(item);
    if (group == nullptr) {
        return false;
    }
    int16_t w = group->GetWidth();
    int16_t h = group->GetHeight();
    if (list->GetDirection() == UIList::VERTICAL) {
        int16_t midY = list->GetY() + (list->GetHeight() >> 1);
        int16_t y = group->GetY();
        int offset = (y + (h >> 1) - midY);
        int temp = EasingEquation::SineEaseIn(0, SCALE_FACTOR, abs(offset), SCALE_FACTOR);
        float scale = 1.0 - (static_cast<float>(temp) / SCALE_FACTOR);
        group->Scale(Vector2<float>(scale, scale), Vector2<float>(w >> 1,  h >> 1));
        if ((y <= midY) && (y + group->GetHeight() >= midY)) {
            return true;
        }
    }
    return false;
}

void ItemContainer::ClearRes()
{
    RemoveAll();
    if (leftImg_ != nullptr) {
        delete leftImg_;
        leftImg_ = nullptr;
    }
    if (label_ != nullptr) {
        delete label_;
        label_ = nullptr;
    }
    if (rightImg_ != nullptr) {
        delete rightImg_;
        rightImg_ = nullptr;
    }
}

UIImageView* ItemContainer::CreateLeftImageView()
{
    if (leftImg_ == nullptr) {
        leftImg_ = new UIImageView();
        if (leftImg_ != nullptr) {
            leftImg_->SetResizeMode(UIImageView::ImageResizeMode::CENTER);
            Add(leftImg_);
        } else {
            GRAPHIC_LOGE("ItemContainer init UIImageView failed");
        }
    }
    return leftImg_;
}

UILabelExt* ItemContainer::CreateLabelView()
{
    if (label_ == nullptr) {
        label_ = new UILabelExt();
        if (label_ != nullptr) {
            label_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
            label_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 40);  // 40: item font size
            Add(label_);
        } else {
            GRAPHIC_LOGE("ItemContainer init UILabelExt failed");
        }
    }
    return label_;
}

UIImageView* ItemContainer::CreateRightImgView()
{
    if (rightImg_ == nullptr) {
        rightImg_ = new UIImageView();
        if (rightImg_ != nullptr) {
            rightImg_->SetResizeMode(UIImageView::ImageResizeMode::CENTER);
            Add(rightImg_);
        } else {
            GRAPHIC_LOGE("ItemContainer init UIImageView failed");
        }
    }
    return rightImg_;
}

UIImageAnimatorView* ItemContainer::CreateImageAniView()
{
    if (imgAni_ == nullptr) {
        imgAni_ = new UIImageAnimatorView();
        if (imgAni_ != nullptr) {
            imgAni_->SetResizeMode(UIImageView::ImageResizeMode::CENTER);
            Add(imgAni_);
        } else {
            GRAPHIC_LOGE("ItemContainer init UIImageAnimatorView failed");
        }
    }
    return imgAni_;
}

UISlider* ItemContainer::CreateSlider()
{
    if (slider_ == nullptr) {
        slider_ = new UISlider();
        if (slider_ != nullptr) {
            Add(slider_);
        } else {
            GRAPHIC_LOGE("ItemContainer init UIImageAnimatorView failed");
        }
    }
    return slider_;
}

UIImageView* ItemContainer::GetLeftImageView()
{
    return leftImg_;
}

UILabelExt* ItemContainer::GetLabelView()
{
    return label_;
}

UIImageView* ItemContainer::GetRightImgView()
{
    return rightImg_;
}

UIImageAnimatorView* ItemContainer::GetImageAniView()
{
    return imgAni_;
}

UISlider* ItemContainer::GetSlider()
{
    return slider_;
}

static UIView* CreateViewCb(uint8_t type)
{
    ItemContainer* item = new ItemContainer();
    if (item == nullptr) {
        return nullptr;
    }
    item->Resize(ITEM_WIDTH, ITEM_HEIGHT);

    if (static_cast<ItemType>(type) == ItemType::IMGANI_SLIDER ||
        static_cast<ItemType>(type) == ItemType::IMGANI_NULL) {
        UIImageAnimatorView* imgAni = item->CreateImageAniView();
        if (imgAni != nullptr) {
            imgAni->SetPosition(0, 25, IMAGE_WIDTH, IMAGE_HEIGHT); // 25:image start pos y
        }
    }

    if (static_cast<ItemType>(type) == ItemType::IMGANI_SLIDER ||
        static_cast<ItemType>(type) == ItemType::NULL_SLIDER) {
        UISlider* slider = item->CreateSlider();
        if (slider != nullptr) {
            slider->SetPosition(IMAGE_WIDTH, 0, LABEL_WIDTH, LABEL_HEIGHT);
        }
    }

    if (static_cast<ItemType>(type) == ItemType::IMG_LABEL) {
        UIImageView* leftImg = item->CreateLeftImageView();
        if (leftImg != nullptr) {
            leftImg->SetPosition(0, 25, IMAGE_WIDTH, IMAGE_HEIGHT); // 25:image start pos y
        }
        UILabelExt* label = item->CreateLabelView();
        if (label != nullptr) {
            label->SetPosition(IMAGE_WIDTH, 0, LABEL_WIDTH, LABEL_HEIGHT);
        }
    }

    return item;
}

static void UpdateViewCb(UIView* view, void* data, uint8_t type)
{
    if (view == nullptr || data == nullptr) {
        return;
    }
    ItemContainer* item = dynamic_cast<ItemContainer*>(view);
    ItemNode* content = static_cast<ItemNode *>(data);

    if (static_cast<ItemType>(type) == ItemType::IMG_LABEL) {
        UIImageView* leftImg = item->GetLeftImageView();
        if (leftImg != nullptr) {
            content->imgInfo = ImageCacheManager::GetInstance().LoadOneInMultiRes(content->resId, LIST_FPS);
            leftImg->SetSrc(content->imgInfo);
        }
        UILabelExt* label = item->GetLabelView();
        if (label != nullptr) {
            label->SetText(content->txt);
            label->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        }
    }

    if (static_cast<ItemType>(type) == ItemType::IMGANI_SLIDER ||
        static_cast<ItemType>(type) == ItemType::IMGANI_NULL) {
        UIImageAnimatorView* imgAni = item->GetImageAniView();
        if (imgAni != nullptr) {
            imgAni->SetImageAnimatorSrc(g_binImageAnimatorInfo, IMAGE_NUM, 50); // 50: update time Interval
            imgAni->SetSizeFixed(true);
            imgAni->Start();
            imgAni->Pause();
        }
    }

    if (static_cast<ItemType>(type) == ItemType::IMGANI_SLIDER ||
        static_cast<ItemType>(type) == ItemType::NULL_SLIDER) {
        UISlider* slider = item->GetSlider();
        if (slider != nullptr) {
            slider->SetValidHeight(40);          // 40: valid height;
            slider->SetValue(20);                 // 20:  progress bar current value
            slider->SetDirection(UISlider::Direction::DIR_LEFT_TO_RIGHT);
            slider->SetSliderColor(Color::Silver(), Color::Blue());
            slider->SetKnobColor(Color::Red());
            slider->SetSliderRadius(DEFAULT_RADIUS, DEFAULT_RADIUS);
        }
    }
}

void UITestSimpleTransformList::OnItemSelected(int16_t index, UIView* view)
{
    if (view == nullptr) {
        GRAPHIC_LOGE("No select view!");
        return;
    }
    if (lastSelectedView_ != nullptr) {
        if (lastSelectedView_ != view) {
            ItemContainer* selectedItem = dynamic_cast<ItemContainer *>(lastSelectedView_);
            if (selectedItem->GetImageAniView() != nullptr) {
                selectedItem->GetImageAniView()->Pause();
            }
            lastSelectedView_ = nullptr;
        }
    }
    ItemContainer* item = dynamic_cast<ItemContainer *>(view);
    if (item == nullptr) {
        GRAPHIC_LOGE("View is not ItemContainer!");
        return;
    }
    if (item->GetImageAniView() != nullptr) {
        item->GetImageAniView()->Resume();
        lastSelectedView_ = view;
    }
    return;
}

void UITestSimpleTransformList::SetUp()
{
    LoadImg();
    if (list_ == nullptr) {
        list_ = new UISimpleTransformList();
    }
    list_->SetPosition(0, 0, LIST_WIDTH, LIST_HEIGHT);
    list_->SetThrowDrag(true);
    list_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    list_->SetScrollBlankSize(LIST_HEIGHT / 2); // 2: divisor
    list_->SetItemTransformFunc(&TransformItemBaseOnPosition);
    list_->SetScrollStateListener(this);
    list_->SetSelectPosition(227); // 227 : middle of screen

    Contents* content = nullptr;
    for (uint8_t i = 0; i < (sizeof(g_itemNode) / sizeof(ItemNode)); ++i) {
        content = new Contents();
        if (content == nullptr) {
            continue;
        }
        content->createFunc = CreateViewCb;
        content->updateFunc = UpdateViewCb;
        if (g_itemNode[i].isEnableImgAni && g_itemNode[i].isEnableSlider) {
            content->type = static_cast<uint8_t>(ItemType::IMGANI_SLIDER);
        } else if (!g_itemNode[i].isEnableImgAni && g_itemNode[i].isEnableSlider) {
            content->type = static_cast<uint8_t>(ItemType::NULL_SLIDER);
        } else if (g_itemNode[i].isEnableImgAni && !g_itemNode[i].isEnableSlider) {
            content->type = static_cast<uint8_t>(ItemType::IMGANI_NULL);
        } else {
            content->type = static_cast<uint8_t>(ItemType::IMG_LABEL);
        }
        content->data = reinterpret_cast<void *>(&g_itemNode[i]);
        data_.PushBack(content);
        list_->AddContent(*content);
    }
    list_->RefreshList();
}

void UITestSimpleTransformList::TearDown()
{
    ReleaseImg();
    if (list_ != nullptr) {
        delete list_;
        list_ = nullptr;
    }

    while (!data_.IsEmpty()) {
        Contents* element = data_.Back();
        if (element != nullptr) {
            delete element;
        }
        data_.PopBack();
    }
    if (lastSelectedView_ != nullptr) {
        lastSelectedView_ = nullptr;
    }
}

UIView* UITestSimpleTransformList::GetTestView()
{
    return list_;
}

void UITestSimpleTransformList::LoadImg()
{
    ImageAnimatorInfo baseinfo = {nullptr, {0, 0}, IMAGE_WIDTH, IMAGE_HEIGHT, IMG_SRC_IMAGE_INFO};
    for (int i = 0; i < IMAGE_NUM; i++) {
        ImageInfo* img = ImageCacheManager::GetInstance().LoadSingleRes(g_imagePath[i]);
        if (img == nullptr) {
            GRAPHIC_LOGE("Load image failed: %s", g_imagePath[i]);
            ReleaseImg();
            return;
        }
        g_binImageAnimatorInfo[i] = baseinfo;
        g_binImageAnimatorInfo[i].imageInfo = img;
    }
}

void UITestSimpleTransformList::ReleaseImg()
{
    for (int i = 0; i < IMAGE_NUM; i++) {
        if (g_binImageAnimatorInfo[i].imageInfo != nullptr) {
            ImageCacheManager::GetInstance().UnloadSingleRes(g_imagePath[i]);
            g_binImageAnimatorInfo[i].imageInfo = nullptr;
        }
    }
}
} // namespace OHOS