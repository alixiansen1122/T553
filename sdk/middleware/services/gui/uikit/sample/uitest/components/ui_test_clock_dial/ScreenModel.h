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

#ifndef SCREEN_MODEL_H
#define SCREEN_MODEL_H

#include "gfx_utils/list.h"
#include "components/ui_image_view.h"

namespace OHOS {
enum class CardId {
    WATCH_FACE = 0,
    ACTIVITY,
    HEART_RATE,
    COMPASS,
    MAX_CARD,
};

struct CardInfo {
    int16_t cardId;
};

class ScreenModel {
public:
    ScreenModel(const ScreenModel &) = delete;
    ScreenModel &operator=(const ScreenModel &) = delete;
    ~ScreenModel();
    static ScreenModel *GetInstance(void);
 // card funtion
    bool SetUpListCase(void);
    bool InitCardConfig(void);
    void SetCardConfigDefault(void);
    int16_t GetCardPage(uint16_t page);
    bool CardInsertFirst(int32_t card);
    bool CardDelete(int32_t card);
    int16_t GetMaxCard(void);
    int16_t GetLastCardPage(void);
    bool LoadAppImages(std::string file, UIImageView *view, uint32_t resId);
    void PreLoadAppimages(void);
private:
    ScreenModel();
    void LoadAppMultiImages(std::string file, uint32_t startId, uint32_t lastId);
    List<CardInfo> *listMainView{nullptr};  // record main view pages
    int16_t maxPage = static_cast<int16_t>(CardId::MAX_CARD);
};
}
#endif
