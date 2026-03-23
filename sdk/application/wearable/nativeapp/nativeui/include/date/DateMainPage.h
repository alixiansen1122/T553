/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: DateMainPage
 * Create: 2025-03-23
 */

#ifndef DATE_MAIN_PAGE_H
#define DATE_MAIN_PAGE_H

#include <string>
#include <iomanip>
#include <string>
#include <sys/time.h>
#include "gfx_utils/vector.h"
#include "components/ui_scroll_view.h"
#include "components/ui_label.h"
#include "components/ui_button.h"
#include "components/ui_simple_list.h"
#include "DateButtonItemView.h"
#include "DateContentItemView.h"
#include "DatePresenter.h"
#include "SlicePage.h"
#include "UiConfig.h"
#include "DateView.h"

namespace OHOS {
static constexpr int16_t WEEK_DYAS = 7;

class DatePresenter;
class DateMainPage : public SlicePage<DatePresenter>,
                        public UIView::OnClickListener,
                        public UIView::OnDragListener {
public:
    DateMainPage();
    ~DateMainPage() override;
    static DateMainPage *GetInstance();
    void OnStart(void* data) override;
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    void InitList();
    bool InitListTopDate();
    void SetContentListDate();
    void SetDateButtonList(std::string viewId);
    void RefreshDateMsg(std::string dateMsg);
    std::string GetFutureDate(int days_ahead);
    void ReloadPage();
    void ClearButtonList();
    void ClearContentList();
    void RefreshListData();
	
private:
    UIScrollView *uiScrollView_{nullptr};
    UISimpleList *detailList_{nullptr};
    UISimpleList *dateList_{nullptr};
    UIImageView *bgImage_{nullptr};
    UILabel *titlLabel_{nullptr};
    Graphic::Vector<DateButtonItem> listTop_;
    std::string strDate_[WEEK_DYAS];
    std::string strDay_[WEEK_DYAS];
};
}
#endif // SETTING_MAIN_PAGE_H