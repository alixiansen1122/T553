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

#ifndef UI_TEST_IMAGE_PACKER_H
#define UI_TEST_IMAGE_PACKER_H
#include "components/ui_image_view.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "graphic_config.h"
#include "ui_test.h"

#if (ENABLE_IMAGE_PACKER && ENABLE_IMAGE_PACKER_TEST)
namespace OHOS {
class UITestImagePacker : public UITest, public UIView::OnClickListener {
public:
    UITestImagePacker() {}
    ~UITestImagePacker() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    UIImageView* AddImageView(ImageInfo* image);
    void TestSingleImage();
    void TestMutiImage(bool isLoadAllOnce);
    void TestLoadOneInMultiRes();
    void TestLoadSingleRes();
    bool isLoadAllOnce_ = false;
    bool isUnloadLoadAllOnce_ = false;
    UIScrollView* container_ = nullptr;
    UILabelButton* loadAllOnceBtn_ = nullptr;
    UILabelButton* unloadAllOnceBtn_ = nullptr;
};
} // OHOS
#endif // ENABLE_IMAGE_PACKER && ENABLE_IMAGE_PACKER_TEST
#endif // UI_TEST_IMAGE_PACKER_H
