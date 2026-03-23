/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Alarm
 * Author:
 * Create: 2021-09-11
 */

#ifndef ALARM_CONFIG_VIEW_H
#define ALARM_CONFIG_VIEW_H

#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_digital_clock.h"
#include "components/ui_scroll_view.h"
#include "layout/grid_layout.h"
#include "font/ui_font.h"
#include "common/screen.h"
#include "gfx_utils/graphic_log.h"
#include "alarm/AlarmClockModel.h"

namespace OHOS {
class AlarmConfigView {
public:
    AlarmConfigView();
    virtual ~AlarmConfigView();
    UIScrollView *InitConfigView();
    static AlarmConfigView *GetInstance(void);
    void RefreshConfig(const uint32 time, uint8 week, uint16_t alarmNum);
private:
    UILabelButton *deleteImage{nullptr};
    UIScrollView *configScroll{nullptr};
    UIScrollView *group{nullptr};
    UILabel *editLabel{nullptr};
    UIDigitalClock *labelTime{nullptr};
    UILabel *labelSetTime{nullptr};
    UILabel *labelDay{nullptr};
    UILabel *labelSetWeek{nullptr};
    UILabel *labelChangeName{nullptr};
    UILabel *labelSetName{nullptr};
    UIImageView *changeIcon1{nullptr};
    UIImageView *changeIcon2{nullptr};
    UIImageView *changeIcon3{nullptr};
    UILabel *CreateConfigLabel(const char *buttonText, int16_t labelFont, int16_t x, int16_t y, int16_t width);
    void CreateConfigButton(void);
    void CreateChangeIcons(void);
};
} // namespace OHOS
#endif // ALARM_CONFIG_VIEW_H