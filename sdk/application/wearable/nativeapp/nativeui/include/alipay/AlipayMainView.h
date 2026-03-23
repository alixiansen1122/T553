/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay main view.
 * Author:
 * Create:
 */

#ifndef ALIPAY_MAIN_VIEW_H
#define ALIPAY_MAIN_VIEW_H

#include <string>
#include "View.h"
#include "components/ui_view_group.h"
#include "ChangeSliceListener.h"
#include "components/ui_swipe_view.h"
#include "alipay/AlipayPresenter.h"
#include "alipay/AlipayModel.h"
#include "alipay/AlipayBind.h"
#include "alipay/AlipayExitBind.h"
#include "alipay/AlipayBindRes.h"
#include "alipay/AlipayBindAni.h"
#include "alipay/AlipayList/AlipayListView.h"
#include "alipay/AlipayBarCode.h"
#include "alipay/AlipayPayCode.h"
#include "alipay/AlipaySetting.h"
#include "alipay/AlipayHelp.h"
#include "alipay/AlipayUnbindFinish.h"
#include "alipay/AlipayUnbind.h"

#include "alipay/AlipayTrans/AlipayGetTransList.h"
#include "alipay/AlipayTrans/AlipayTransTips.h"
#include "alipay/AlipayTrans/AlipayTransCode.h"
#include "alipay/AlipayTrans/AlipayTransEnable.h"
#include "alipay/AlipayTrans/AlipayTransRes.h"
#include "alipay/AlipayTrans/AlipayTransListView.h"
#include "alipay/AlipayTrans/AlipayTransAni.h"

namespace OHOS {
class AlipayMainView;
class AlipayMainView : public View<AlipayPresenter> {
public:
    AlipayMainView();
    ~AlipayMainView() override;
    static AlipayMainView *GetInstance(void);
    void OnStart() override;
    void DropView(UIView &view);
    void SwitchView(UIView &view);

    void ChangeView(int16 hideView, int16 showView);
    bool ChangePayView(int16 thisView);
    bool ChangeSettingView(int16 thisView);
    bool ChangeHelpView(int16 thisView);
    bool ChangeListView(int16 thisView);
    bool ChangeBindAniView(int16 thisView);
    bool ChangeBindExitView(int16 thisView);
    bool ChangeBindResView(int16 thisView);
    bool ChangeUnbindView(int16 thisView);
    bool ChangeUnbindFinishView(int16 thisView);
    bool ChangeBindView(int16 thisView);
    bool ChangeBarCodeView(int16 thisView);

    bool ChangeTransView(int16 thisView);
    bool ChangeTransListView(int16 thisView);
    bool ChangeTransEnableView(int16 thisView);
    bool ChangeTransTipsView(int16 thisView);
    bool ChangeTransResView(int16 thisView);
    bool ChangeGetTransListView(int16 thisView);
    bool ChangeTransAniView(int16 thisView);

    int16 GetListenInter(void);
    int16 GetWhichInter(void);
    bool ClickFunc(int16 interface);

private:
    bool SetSwipeInter(int16 num);
    void DeleteViceInter(void);
    void DestroyTempView(int16 hideView);
    UIViewGroup *group { nullptr };
    UIViewGroup *container[ALIPAY_INTERFACES_MAX] { nullptr };

    AlipayBind *alipayBind { nullptr };
    AlipayExitBind *alipayExitBind { nullptr };
    AlipayBindRes *alipayBindRes { nullptr };
    AlipayBindAni *alipayBindAni { nullptr };
    AlipayPayCode *alipayPayCode { nullptr };
    AlipayBarCode *alipayBarCode { nullptr };
    AlipaySetting *alipaySetting { nullptr };
    AlipayUnbind *alipayExitBindSetting { nullptr };
    AlipayUnbindFinish *alipayUnbindFinish { nullptr };
    AlipayHelp *alipayHelp { nullptr };
    AlipayList *alipayList { nullptr };

    AlipayGetTransList *alipayGetTransList { nullptr };
    AlipayTransTips *alipayTransTips { nullptr };
    AlipayTransCode *alipayTransCode { nullptr };
    AlipayTransEnable *alipayTransEnable { nullptr };
    AlipayTransRes *alipayTransRes { nullptr };
    AlipayTransList *alipayTransList { nullptr };
    AlipayTransAni *alipayTransAni { nullptr };

    int16 interListen { -1 };     // 监测上一个界面,-1用于还没获得监测界面时的初始化
    int16 alipayWhichInter { 0 }; // 监测当前是哪一个温度界面
    int16 alipayWhichMea { -1 };  // 监测当前处于哪一个监测界面
};
} // namespace OHOS
#endif // MAIN_ALARM_VIEW_Ha