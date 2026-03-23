/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: ScreenModels
 * Author:
 * Create: 2022-01-22
 */

#ifndef SCREEN_MODEL_H
#define SCREEN_MODEL_H

#include "gfx_utils/list.h"
#include "ui_image_view.h"

namespace OHOS {
class ScreenModels {
public:
    ScreenModels();
    ~ScreenModels();
    static ScreenModels *GetInstance(void);
    bool LoadAppImages(std::string file, UIImageView *view, uint32_t resId);
    void PreLoadAppimages(void);
    void InitKvStorage(void);
private:
    void LoadAppMultiImages(std::string file, uint32_t startId, uint32_t lastId);
};
}
#endif
