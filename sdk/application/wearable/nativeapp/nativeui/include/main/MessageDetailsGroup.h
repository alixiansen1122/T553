/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: MessageDetailsGroup
 * Create: 2025-06-29
 */

#ifndef MESSAGE_DETAILS_GROUP_H
#define MESSAGE_DETAILS_GROUP_H

#include "graphic_types.h"
#include "wearable_log.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_scroll_view.h"
#include "ui_resource_message.h"
#include "MessageModel.h"

namespace OHOS {
class DetailsGroup : public UIScrollView {
public:
    DetailsGroup();
    ~DetailsGroup() override;
    void InitDetailsPage();
    void RefreshNotify();
    MessageItem detailsMsg_;

private:
    UIImageView *detailsMsgIcon_{nullptr};
    UILabel *detailsMsgTitle_{nullptr};
    UILabel *detailsMsgContacts_{nullptr};
    UILabel *detailsMsgContents_{nullptr};
    UILabel *detailsMsgTime_{nullptr};
};
}
#endif
