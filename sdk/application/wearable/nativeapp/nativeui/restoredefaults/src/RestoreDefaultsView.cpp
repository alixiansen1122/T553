/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: Restore Defaults View
 * Create: 2024-08-12
 */

#include "restoredefaults/RestoreDefaultsView.h"
#include "components/ui_label.h"
#include "wearable_log.h"
#include "UiConfig.h"

namespace OHOS {
static RestoreDefaultsView *g_pRestoreDefaultsView { nullptr };

RestoreDefaultsView::RestoreDefaultsView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "RestoreDefaultsView::RestoreDefaultsView");
    preProcessView[RESTORESELECTVIEW] = &RestoreDefaultsView::ChangeRestoreSelectView;
    g_pRestoreDefaultsView = this;
}

RestoreDefaultsView::~RestoreDefaultsView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "RestoreDefaultsView::~RestoreDefaultsView");
    g_pRestoreDefaultsView = nullptr;
}

RestoreDefaultsView *RestoreDefaultsView::GetInstance()
{
    return g_pRestoreDefaultsView;
}

void RestoreDefaultsView::OnStart()
{
    ChangeView(RESTOREDEFAULTS_MAX, RESTORESELECTVIEW);
    return;
}

bool RestoreDefaultsView::ChangeRestoreSelectView(int16_t hideView, int16_t thisView)
{
    UNUSED(hideView);
    if (container[thisView] == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ChangeRestoreSelectView");
        restoreSelectView = new RestoreSelectView();
        if (restoreSelectView == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "connectSelectView new fail");
            return false;
        }
        // init view
        UIViewGroup *group = restoreSelectView->InitShowUpView();
        if (group == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "group nullptr fail");
            return false;
        }
        container[thisView] = group;
        AddViewToRootContainer(container[thisView]);
    }
    return true;
}

bool RestoreDefaultsView::CheckViewParam(int16_t hideView, int16_t showView)
{
    if (showView < 0 || showView >= RESTOREDEFAULTS_MAX) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "RestoreDefaultsView::CheckViewParam showView err.");
        return false;
    }

    if (hideView < 0 || hideView > RESTOREDEFAULTS_MAX) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "RestoreDefaultsView::CheckViewParam hideView err.");
        return false;
    }

    return true;
}

void RestoreDefaultsView::ChangeView(int16_t hideView, int16_t showView)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "RestoreDefaultsView::ChangeView %d, %d", hideView, showView);

    if (!CheckViewParam(hideView, showView)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "RestoreDefaultsView::CheckViewParam failed.");
        return;
    }

    PreprosssChangeView tmpPreProcessView = preProcessView[showView];

    if (!(this->*tmpPreProcessView)(hideView, showView)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "RestoreDefaultsView::ChangeView %d %dfailed", showView, hideView);
        return;
    }
    if (hideView != RESTOREDEFAULTS_MAX) {
        if (container[hideView] == nullptr) {
            return;
        }
        container[hideView]->SetVisible(false);
    }
    container[showView]->SetVisible(true);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "RestoreDefaultsView:: ChangeView end!!!%d", showView);
    Draw();
}
}