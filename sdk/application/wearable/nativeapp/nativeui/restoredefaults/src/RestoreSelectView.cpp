/*
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: RestoreSelect View
 * Create: 2024-08-12
 */
#include "restoredefaults/RestoreSelectView.h"
#include "PageTransitionMgr.h"
#include "wearable_log.h"
#include "UiConfig.h"
#include "restoredefaults/RestoreDefaultsPresenter.h"

namespace OHOS {
static RestoreSelectView *g_pRestoreSelectView { nullptr };

RestoreSelectView::RestoreSelectView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "RestoreSelectView::RestoreSelectView");
    g_pRestoreSelectView = this;
}

RestoreSelectView::~RestoreSelectView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "RestoreSelectView::~RestoreSelectView");
    group->RemoveAll();
    delete titleLabel;
    titleLabel = nullptr;
    delete cancelButton;
    cancelButton = nullptr;
    delete confirmButton;
    confirmButton = nullptr;
    g_pRestoreSelectView = nullptr;
}

RestoreSelectView *RestoreSelectView::GetInstance()
{
    return g_pRestoreSelectView;
}

void RestoreSelectView::ShowUpTitleLabel()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "RestoreSelectView::ShowUpTitleLabel");
    if (titleLabel == nullptr) {
        titleLabel = new UILabel();
        if (titleLabel == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "RestoreSelectView titleLabel new fail");
            return;
        }
        titleLabel->SetPosition(65, 100, 324, 160); // 65:x, 100:y, 324:width, 160:height
        titleLabel->SetText(" 恢复出厂将会丢失个人数据，是否继续？");
        titleLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 35); // 35:font data
        titleLabel->SetStyle(STYLE_BACKGROUND_COLOR, OPA_OPAQUE);
        titleLabel->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER,
            UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
        titleLabel->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);
        group->Add(titleLabel);
    }
}

void RestoreSelectView::CreateUIButton()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "RestoreSelectView::CreateUIButton");
    UIView::OnClickListener *clickListener = (UIView::OnClickListener *)OHOS::RestoreDefaultsPresenter::GetInstance();
    if (clickListener == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "clickListener nullptr");
        return;
    }
    if (cancelButton == nullptr) {
        cancelButton = new UIButton();
        if (cancelButton == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "RestoreSelectView cancelButton new fail");
            return;
        }
        cancelButton->SetPosition(85, 320, 90, 90); // 85:x, 320:y, 90:width, 90:height
        cancelButton->SetViewId(RESTORE_CANCEL_BUTTON);
        cancelButton->SetImageSrc(ALARM_DELETE_ADD_PATH, ALARM_DELETE_ADD_PATH);
        cancelButton->SetOnClickListener(clickListener);
        group->Add(cancelButton);
    }
    if (confirmButton == nullptr) {
        confirmButton = new UIButton();
        if (confirmButton == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "RestoreSelectView confirmButton new fail");
            return;
        }
        confirmButton->SetPosition(279, 320, 90, 90); // 279:x, 320:y, 90:width, 90:height
        confirmButton->SetViewId(RESTORE_CONFIRM_BUTTON);
        confirmButton->SetImageSrc(ALARM_TRUE_ADD_PATH, ALARM_TRUE_ADD_PATH);
        confirmButton->SetOnClickListener(clickListener);
        group->Add(confirmButton);
    }
}

UIViewGroup *RestoreSelectView::InitShowUpView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "RestoreSelectView init");
    if (group == nullptr) {
        group = new UIViewGroup();
        if (group == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "group new fail");
            return nullptr;
        }
        group->SetPosition(0, 0);
        group->SetWidth(HORIZONTAL_RESOLUTION);
        group->SetHeight(VERTICAL_RESOLUTION);
    }
    ShowUpTitleLabel();
    CreateUIButton();
    UIView::OnDragListener *connectDragListener =
        (UIView::OnDragListener *)OHOS::RestoreDefaultsPresenter::GetInstance();
    if (connectDragListener == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "connectDragListener GetInstance fail");
        return nullptr;
    }
    if (!PageTransitionMgr::GetInstance().CanSlideBack()) { // support default slide back firstly
        group->SetOnDragListener(connectDragListener);
    }
    group->SetViewId(RESTORE_SELECT_VIEW);
    group->SetDraggable(true);
    group->SetTouchable(true);
    return group;
}
}