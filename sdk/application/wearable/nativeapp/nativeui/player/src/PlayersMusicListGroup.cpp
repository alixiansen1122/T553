/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PlayersMusicListGroup
 * Created: 2025-06
 */

#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "main/LoadImg.h"
#include "wearable_log.h"
#include "UiConfig.h"
#include "common/image_cache_manager.h"
#include "ui_resource_image.h"
#include "graphic_service.h"
#include "phoneservice/PhoneService.h"
#include "phonemenu/PhoneMenuView.h"
#include "main/VolumeSettingView.h"
#include "settings/common/SettingCommon.h"
#include "player/PlayersPresenter.h"
#include "player/PlayersModel.h"
#include "player/PlayersMusicListGroup.h"
#include "bts_avrcp_controller.h"
#include "bts_avrcp_target.h"
#include "player/PlayersMusicItemView.h"
#include "player/PlayersMusicListGroup.h"

namespace OHOS {
constexpr uint16_t ITEM_HEIGHT = 64;
static PlayersMusicListGroup* g_pPlayersMusicListGroup = nullptr;

PlayersMusicListGroup::PlayersMusicListGroup()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PlayersMusicListGroup::PlayersMusicListGroup");
    g_pPlayersMusicListGroup = this;
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    SetDraggable(true);
    SetTouchable(true);
    SetOnClickListener(this);
    SetOnDragListener(this);
    musicList_ = new UISimpleList();
    if (musicList_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "list new fail");
        return;
    }
    musicList_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    musicList_->SetDraggable(true);
    musicList_->SetTouchable(true);
    musicList_->SetThrowDrag(true);
    musicList_->SetOnDragListener(this);
    musicList_->SetOnClickListener(this);
    musicList_->SetStartIndex(0);
    RefreshMusicList();
    musicList_->Invalidate();
    Add(musicList_);
}

PlayersMusicListGroup* PlayersMusicListGroup::GetInstance(void)
{
    return g_pPlayersMusicListGroup;
}

static UIView* CreateViewCb(uint8_t type)
{
    PlayersMusicItemView* item = new PlayersMusicItemView();
    if (item == nullptr) {
        return nullptr;
    }
    item->SetPosition(0, 0);
    item->Resize(HORIZONTAL_RESOLUTION, ITEM_HEIGHT);
    item->SetTouchable(true);
    item->SetOnClickListener(g_pPlayersMusicListGroup);
    return item;
}

static void UpdateViewCb(UIView* view, void* data, uint8_t type)
{
    PlayersMusicItemView* item = dynamic_cast<PlayersMusicItemView*>(view);
    if (view == nullptr || data == nullptr || item == nullptr) {
        return;
    }
    PlayerCaseInfo* content = static_cast<PlayerCaseInfo *>(data);
    item->SetItemInfo(*content);
}

void PlayersMusicListGroup::RefreshMusicList()
{
    List<PlayerCaseInfo> &playersCaseList = PlayersListGroup::GetInstance()->GetCase();
    int size = playersCaseList.Size();

    musicList_->ClearAll();
    musicList_->ScrollTo(0);
    ListNode<PlayerCaseInfo> *node = playersCaseList.Head();
    while (node != playersCaseList.End()) {
        Contents content;
        content.createFunc = CreateViewCb;
        content.updateFunc = UpdateViewCb;
        content.type = 0;
        PlayerCaseInfo &itemData = node->data_;
        content.data = reinterpret_cast<void *>(&itemData);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "PlayerCaseInfo.itemData(%s)", itemData.sliceId);
        musicList_->AddContent(content);
        node = node->next_;
    }
    musicList_->RefreshList();
}

PlayersMusicListGroup::~PlayersMusicListGroup()
{
    RemoveAll();
    if (musicList_ != nullptr) {
        musicList_->ClearAll();
        delete musicList_;
        musicList_ = nullptr;
    }
    g_pPlayersMusicListGroup = nullptr;
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(PLAYER_IAMGES);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PlayersMusicListGroup::~PlayersMusicListGroup");
}

void PlayersMusicListGroup::BtnOnListData(UIView& view)
{
    index_ = view.GetViewIndex();
    List<PlayerCaseInfo> &playersCaseList = PlayersListGroup::GetInstance()->GetCase();
    ListNode<PlayerCaseInfo>* node = playersCaseList.Begin();
    for (uint16_t i = 0; i < index_; i++) {
        node = node->next_;
    }
    sliceId_ = node->data_.sliceId;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "pureName.index_(%d)", index_);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "pureName.sliceId(%s)", sliceId_);
}

bool PlayersMusicListGroup::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    int ret;
    int32_t playerState;
    uint8_t state = UICheckBox::UICheckBoxState::MAX_STATUS_NUM;
    PlayersView *playersView = PlayersView::GetInstance();
    if (playersView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "playersView null!");
        return false;
    }
    BtnOnListData(view);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BtnOnClickListListener sliceId_: %s.", sliceId_);
    // 标记当前播放歌曲index_
    PlayersModel::GetInstance()->SetCurrentIndex(index_);
    // 本地播放文件路径和状态
    std::string strSrcMusic(APP_MUSIC_PATH);
    std::string tempStr("/");
    std::string strName = sliceId_;
    PlayersModel::GetInstance()->SetPlayerUri(strSrcMusic + tempStr + strName);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "uri: %s.", PlayersModel::GetInstance()->GetPlayerUri().c_str());
    std::string strSongName = sliceId_;
    std::string pureName = strSongName.substr(0, strSongName.rfind("."));
    playersView->TitleSetText(pureName.c_str());
    SetVisible(false);
    // 刷新UI
    RootView::GetInstance()->Invalidate();

    state = playersView->GetPlayerButtonState();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "state %d", state);
    if (state == UICheckBox::UICheckBoxState::UNSELECTED) {
        playersView->SetPlayerButtonState(UICheckBox::UICheckBoxState::SELECTED);
        /* start play */
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "play");
        PlayersModel::GetInstance()->GetPlayerCtr()->GetPlayerState(playerState);
        if (playerState == PlayerStates::PLAYER_PAUSED) {
            ret = PlayersModel::GetInstance()->GetPlayerCtr()->Reset();
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Reset, ret:%d", ret);
        }
        PlayersModel::GetInstance()->PlayerStat(); // 播放
    } else if (state == UICheckBox::UICheckBoxState::SELECTED) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "List to play");
        PlayersModel::GetInstance()->GetPlayerCtr()->GetPlayerState(playerState);
        if (playerState == PlayerStates::PLAYER_STARTED) {
            ret = PlayersModel::GetInstance()->GetPlayerCtr()->Stop(); // 停止
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Stop, ret:%d", ret);
            ret = PlayersModel::GetInstance()->GetPlayerCtr()->Reset();
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Reset, ret:%d", ret);
        }
        PlayersModel::GetInstance()->PlayerStat();
    }
    return true;
}

bool PlayersMusicListGroup::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
            SetVisible(false);
    }
    return true;
}
}