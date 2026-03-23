/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay presenter.
 * Author:
 * Create:
 */

#ifndef ALIPAY_PRESENTER_H
#define ALIPAY_PRESENTER_H

#include "Presenter.h"
#include "UiConfig.h"
#include "wearable_log.h"
#include "color.h"
#include "alipay/AlipayModel.h"

namespace OHOS {
constexpr char *ALIPAY_LIST_PAY_BUTTON = (char *)"alipaylistpay";
constexpr char *ALIPAY_LIST_TRANS_BUTTON = (char *)"alipaylisttrans";
constexpr char *ALIPAY_LIST_SETTING_BUTTON = (char *)"alipaylistsetting";
constexpr char *ALIPAY_LIST_HELP_BUTTON = (char *)"alipaylisthelp";

constexpr char *ALIPAY_PAY_JUMP = (char *)"alipaypayjump";
constexpr char *ALIPAY_BIND_BUTTON = (char *)"bindbutton";
constexpr char *ALIPAY_UNBIND_BUTTON = (char *)"unbindbutton";
constexpr char *ALIPAY_BIND_CONTINUE = (char *)"alipaybindcontinue";
constexpr char *ALIPAY_INTER_EXIT = (char *)"alipayexit";

constexpr char *ALIPAY_SETTING_UNBIND = (char *)"alipaysettingunbind";
constexpr char *ALIPAY_SETTING_CANCEL = (char *)"alipaysettingcancel";
constexpr char *ALIPAY_SETTING_FINISH = (char *)"alipaysettingfinish";

constexpr char *ALIPAY_PAY_QRCODE = (char *)"alipaypayqrcode";
constexpr char *ALIPAY_PAY_BARCODE = (char *)"alipaypaybarcode";
constexpr char *ALIPAY_PAY_SWITCH_BAR = (char *)"alipaypayswitchbar";
constexpr char *ALIPAY_PAY_SWITCH_QR = (char *)"alipaypayswitchqrcode";

constexpr char *ALIPAY_TRANS_LIST_BUTTON = (char *)"alipaytranslistbutton";
constexpr char *ALIPAY_TRANS_REFRESH_LIST = (char *)"alipaytransrefreshlist";
constexpr char *ALIPAY_TRANS_GET_LIST_EXIT = (char *)"alipaytransgetlistexit";
constexpr char *ALIPAY_TRANS_GET_LIST_FINISH = (char *)"alipaytransgetlistfin";
constexpr char *ALIPAY_TRANS_GET_LIST_RETRY = (char *)"alipaytransgetlistre";
constexpr char *ALIPAY_TRANS_GET_LIST_DETAIL = (char *)"alipaytransgetlistdet";
constexpr char *ALIPAY_TRANS_CERTAIN_BACK_LIST = (char *)"alipaytransbacklist";
constexpr char *ALIPAY_TRANS_CERTAIN_TO_TRANS = (char *)"alipaytranstotranslist";
constexpr char *ALIPAY_TRANS_RETRY_TRANS_CODE = (char *)"alipaytransretrycode";
constexpr char *ALIPAY_TRANS_CODE_RES_DETAIL = (char *)"alipaytranscoderesde";
constexpr char *ALIPAY_TRANS_LIST_RES_DETAIL = (char *)"alipaytranslistresde";
constexpr char *ALIPAY_TRANS_CODE_BACK_LIST = (char *)"alipaytranscodebacklist";

#define ALIPAY_SET_ZERO 0
#define ALIPAY_SET_ONE 1
#define ALIPAY_SET_TWO 2

typedef enum {
    ALIPAY_BIND_VIEW = 0,
    ALIPAY_BIND_ANI_VIEW,
    ALIPAY_BIND_EXIT_VIEW,
    ALIPAY_BIND_RES_VIEW,
    ALIPAY_LIST_VIEW,
    ALIPAY_PAY_VIEW,

    ALIPAY_PAY_BARCODE_VIEW,
    ALIPAY_TRANS_VIEW,
    ALIPAY_SETTING_VIEW,
    ALIPAY_HELP_VIEW,
    ALIPAY_SETTING_UNBIND_VIEW,

    ALIPAY_SETTING_FINISH_VIEW,
    ALIPAY_GET_BIND_FAILURE_VIEW,
    ALIPAY_TRANS_LIST_VIEW,
    ALIPAY_TRANS_ENABLE_VIEW,
    ALIPAY_TRANS_TIPS_VIEW,

    ALIPAY_TRANS_RES_VIEW,
    ALIPAY_GET_TRANS_LIST_VIEW,
    ALIPAY_TRANS_ANI_VIEW,

    ALIPAY_INTERFACES_MAX
} alipayInterfacesId;

typedef enum {
    ALIPAY_TRANS_TIPS_UNKNOW = 0,
    ALIPAY_TRANS_TIPS_TO_MUCH,
    ALIPAY_TRANS_TIPS_SECURE,
    ALIPAY_TRANS_TIPS_LIST_UNKNOW,
    ALIPAY_TRANS_TIPS_NOT_SUPPORT,
} alipayTransTipsId;

typedef enum {
    ALIPAY_TRANS_RES_LIST = 0,
    ALIPAY_TRANS_RES_CODE,
} alipayTransResId;

typedef enum {
    ALIPAY_TRANS_GET_LIST_FIRST = 0,
    ALIPAY_TRANS_GET_LIST_CLICK_DETAIL,
    ALIPAY_TRANS_GET_LIST_CODE_DETAIL,
} alipayTransGetListId;

class AlipayMainView;
class AlipayPresenter : public Presenter<AlipayMainView>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    AlipayPresenter();
    ~AlipayPresenter() override;
    void OnStart() override;
    void OnResume() override;
    void OnStop() override;
    static AlipayPresenter *GetInstance();

    void ClickExitAlipay(UIView &view);
    void ClickJumpSetting(UIView &view);
    void ClickJumpPay(UIView &view);
    void ClickJumpHelp(UIView &view);
    void ClickJumpBindAni(UIView &view);
    void ClickJumpUnbind(UIView &view);
    void ClickJumpBindFinish(UIView &view);
    void ClickBackUnbind(UIView &view);
    void ClickBackBindCode(UIView &view);
    void ClickRefreshQrcode(UIView &view);
    void ClickRefreshBarcode(UIView &view);
    void ClickGetBindStr(UIView &view);
    void ClickSwitchBar(UIView &view);
    void ClickSwitchQr(UIView &view);
    void ClickJumpTrans(UIView &view);

    void ClickRefreshList(UIView &view);
    void ClickBackList(UIView &view);
    void ClickUpdateList(UIView &view);
    void ClickTransToTransList(UIView &view);
    void ClickRetryTransCode(UIView &view);
    void ClickTransJumpHelp(UIView &view);
    void ClickTranstBackList(UIView &view);
    void ClickTransToTransCode(UIView &view);
    void ClickTransJumpGetList(UIView &view);
    void ClickTransJumpGetCode(UIView &view);
    void ClickTransCodeBackList(UIView &view);
    ColorType CyanColor(void);
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDrag(UIView &view, const DragEvent &event) override;
    bool ClickFuncId(UIView &view);
    int32_t AlipayGetPayCode(uint8_t *payStr);
    int32_t AlipayGetNickName(uint8_t *nickName);
    int32_t AlipayGetBindCode(uint8_t *bindStr);
    int32_t AlipayGetIdName(uint8_t *idStr);
    int32_t AlipayGetAidCode(int8_t *aidStr);
    bool AlipayGetBindFlag(void);
    void AlipaySetBindFlag(bool bindFlag);
    uint32_t AlipayGetSwitchIndex(void);
    void AlipaySetSwitchIndex(uint32_t switchIndex);
    uint32_t AlipayGetCardNum(void);
    void AlipaySetCardNum(uint32_t cardNum);
    uint32_t AlipayGetCardSum(void);
    void AlipaySetCardSum(uint32_t cardSum);
    char* AlipayGetCardTitle(uint32_t index);
    uint8_t* AlipayGetTransCode(void);
    uint32_t AlipayGetTransCodeLen(void);
    char* AlipayGetCardTitle(void);
private:
    AlipayModel *alipayMode_;
};
}
#endif