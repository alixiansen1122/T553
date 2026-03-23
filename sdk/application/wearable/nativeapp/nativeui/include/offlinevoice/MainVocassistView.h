/*
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: voice assistant
 * Author: CompanyName
 * Create: 2022-01-18
 */
#ifndef MAIN_VOCASSIST_VIEW_H
#define MAIN_VOCASSIST_VIEW_H

#include "View.h"
#include "offlinevoice/VocassistModel.h"
#include "offlinevoice/VocassistView.h"

namespace OHOS {
class VocassistPresenter;
class MainVocassistView : public View<VocassistPresenter> {
public:
    MainVocassistView();
    ~MainVocassistView() override;
    void OnStart() override;
    void OnStop() override;
    static MainVocassistView *GetInstance();
    VoiceAssistance *GetVocassist();
private:
    VoiceAssistance* vocassistance{nullptr};
    UIView::OnDragListener* draglistener{nullptr};
};
}
#endif